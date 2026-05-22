/*!
 * \file temp.c
 * \date 2024-08-30
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Temperature measurment and control
 */

#include "temp-api.h"

#include <string.h>
#include "eagletrt-api.h"
#include "error.h"

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

EAGLETRT_STATIC struct TempHandler temp_handler;

// clang-format off

// Array to map cells index in memory to phisical positions
EAGLETRT_STATIC constexpr size_t temp_cell_position_index_map[] = {
    63U, 65U, 15U, 61U,  6U, 39U, 46U,  7U,
    40U, 31U,  8U, 41U, 64U, 36U, 54U,  1U,
    51U, 42U, 32U, 37U, 55U, 48U, 52U, 43U,
    49U, 38U, 72U,  2U,  9U, 44U, 33U, 67U,
    45U,  3U, 10U, 75U, 34U, 11U, 57U,  4U,
    12U, 70U, 35U, 13U, 69U,  5U, 14U, 73U
};

// clang-format on

/*!
 * \brief Get the phisical cell position from its index in memory
 *
 * \param index The cell index in memory
 *
 * \return int32_t The cell phisical position if index is valid, otherwise -1
 */
EAGLETRT_STATIC_INLINE int32_t prv_temp_cell_position_from_index(size_t index) {
    if (index < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return (int32_t)temp_cell_position_index_map[index];
    }
    return -1;
}

/*!
 * \brief Check if the temperature values are in range otherwise set an error
 *
 * \param cellboard_id The cellboard identifier
 * \param offset The cell offset of the segment
 * \param value The temperature value in °C
 */
EAGLETRT_STATIC_INLINE void prv_temp_check_value(const CellboardId cellboard_id, const size_t offset, const celsius_t value) {
    size_t index = (cellboard_id * CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) + offset;
    if (value < TEMP_MIN_C) {
        error_set(ERROR_GROUP_UNDER_TEMPERATURE, (error_instance_t)index);
    } else {
        error_reset(ERROR_GROUP_UNDER_TEMPERATURE, (error_instance_t)index);
    }

    if (value > TEMP_MAX_C) {
        error_set(ERROR_GROUP_OVER_TEMPERATURE, (error_instance_t)index);
    } else {
        error_reset(ERROR_GROUP_OVER_TEMPERATURE, (error_instance_t)index);
    }
}

enum TempReturnCode temp_api_init(void) {
    memset(&temp_handler, 0U, sizeof(temp_handler));
    return TEMP_RC_OK;
}

const cells_temp *temp_api_get_values(void) {
    return &temp_handler.temperatures;
}

celsius_t temp_api_get_min(void) {
    celsius_t min = temp_handler.temperatures[0][0];
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            min = MAINBOARD_MIN(min, temp_handler.temperatures[i][j]);
        }
    }
    return min;
}

celsius_t temp_api_get_max(void) {
    celsius_t max = temp_handler.temperatures[0][0];
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            max = MAINBOARD_MAX(max, temp_handler.temperatures[i][j]);
        }
    }
    return max;
}

celsius_t temp_api_get_sum(void) {
    celsius_t sum = 0U;
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            sum += temp_handler.temperatures[i][j];
        }
    }
    return sum;
}

celsius_t temp_api_get_avg(void) {
    return temp_api_get_sum() / CELLBOARD_TEMP_SENSOR_COUNT;
}

void temp_api_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t *const payload) {
    const size_t size = 4U;
    if (payload == NULL ||
        (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
        payload->offset + size > CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return;
    }

    // Update temperatures
    const size_t offset = payload->offset;
    celsius_t *const temperatures = temp_handler.temperatures[payload->cellboard_id];
    temperatures[offset] = payload->temperature_0;
    temperatures[offset + 1U] = payload->temperature_1;
    temperatures[offset + 2U] = payload->temperature_2;
    temperatures[offset + 3U] = payload->temperature_3;
    for (size_t i = 0U; i < size; ++i) {
        prv_temp_check_value((CellboardId)payload->cellboard_id, offset + i, temperatures[offset + i]);
    }
}

primary_hv_cells_temperature_converted_t *temp_api_get_cells_temperature_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(temp_handler.temp_can_payload);
    }

    const celsius_t *temperatures = temp_handler.temperatures[temp_handler.cellboard_id];
    temp_handler.temp_can_payload.cellboard_id = (primary_hv_cells_temperature_cellboard_id)temp_handler.cellboard_id;

    temp_handler.temp_can_payload.temperature_0 = temperatures[temp_handler.offset];
    temp_handler.temp_can_payload.temperature_1 = temperatures[temp_handler.offset + 1];
    temp_handler.temp_can_payload.temperature_2 = temperatures[temp_handler.offset + 2];
    temp_handler.temp_can_payload.temperature_3 = temperatures[temp_handler.offset + 3];

    temp_handler.temp_can_payload.temperature_id_0 = prv_temp_cell_position_from_index(temp_handler.offset);
    temp_handler.temp_can_payload.temperature_id_1 = prv_temp_cell_position_from_index(temp_handler.offset + 1);
    temp_handler.temp_can_payload.temperature_id_2 = prv_temp_cell_position_from_index(temp_handler.offset + 2);
    temp_handler.temp_can_payload.temperature_id_3 = prv_temp_cell_position_from_index(temp_handler.offset + 3);

    // Update indices
    temp_handler.offset += TEMP_NUM_TEMP_CAN_MESSAGE;
    if (temp_handler.offset >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        temp_handler.offset = 0U;
        if (++temp_handler.cellboard_id >= CELLBOARD_ID_COUNT) {
            temp_handler.cellboard_id = 0U;
        }
    }
    return &temp_handler.temp_can_payload;
}

primary_hv_cells_temp_stats_converted_t *temp_api_get_cells_temperature_stats_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(temp_handler.temp_stats_can_payload);
    }

    temp_handler.temp_stats_can_payload.max = temp_api_get_max();
    temp_handler.temp_stats_can_payload.min = temp_api_get_min();

    temp_handler.temp_stats_can_payload.avg = temp_api_get_avg();

    return &temp_handler.temp_stats_can_payload;
}

#ifdef CONF_TEMPERATURE_STRINGS_ENABLE

EAGLETRT_STATIC char *temp_module_name = "temperature";

EAGLETRT_STATIC char *temp_return_code_name[] = {
    [TEMP_OK] = "ok",
    [TEMP_NULL_POINTER] = "null pointer",
    [TEMP_OUT_OF_BOUNDS] = "out of bounds"
};

EAGLETRT_STATIC char *temp_return_code_description[] = {
    [TEMP_OK] = "executed successfully",
    [TEMP_NULL_POINTER] = "attempt to dereference a null pointer",
    [TEMP_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif //  CONF_TEMPERATURE_STRINGS_ENABLE

#endif //  CONF_TEMPERATURE_MODULE_ENABLE
