/**
 * @file temp.c
 * @date 2024-08-30
 * @author Riccardo Segala [riccardo.segala@icloud.com]
 *
 * @brief Temperature measurment and control
 */

#include "temp.h"

#include <string.h>

#include "error.h"

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

_STATIC _TempHandler htemp;

// Array to map cells index in memory to phisical positions
_STATIC size_t _temp_cell_position_index_map[] = {
    63U, 65U, 15U, 61U,  6U, 39U, 46U,  7U,
    40U, 31U,  8U, 41U, 64U, 36U, 54U,  1U,
    51U, 42U, 32U, 37U, 55U, 48U, 52U, 43U,
    49U, 38U, 72U,  2U,  9U, 44U, 33U, 67U,
    45U,  3U, 10U, 75U, 34U, 11U, 57U,  4U,
    12U, 70U, 35U, 13U, 69U,  5U, 14U, 73U
};

/**
 * @brief Get the phisical cell position from its index in memory
 *
 * @param index The cell index in memory
 *
 * @return int32_t The cell phisical position if index is valid, otherwise -1
 */
_STATIC_INLINE int32_t _temp_cell_position_from_index(size_t index) {
    if (index < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT)
        return _temp_cell_position_index_map[index];
    return -1;
}

/**
 * @brief Check if the temperature values are in range otherwise set an error
 *
 * @param id The cellboard identifier
 * @param offset The cell offset of the segment
 * @param value The temperature value in °C
 */
_STATIC_INLINE void _temp_check_value(const CellboardId id, const size_t offset, const celsius_t value) {
    size_t index = id * CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT + offset;
    if (value < TEMP_MIN_C)
        error_set(ERROR_GROUP_UNDER_TEMPERATURE, index);
    else
        error_reset(ERROR_GROUP_UNDER_TEMPERATURE, index);

    if (value > TEMP_MAX_C)
        error_set(ERROR_GROUP_OVER_TEMPERATURE, index);
    else
        error_reset(ERROR_GROUP_OVER_TEMPERATURE, index);
}

TempReturnCode temp_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
    return TEMP_OK;
}

const cells_temp_t * temp_get_values(void) {
    return &htemp.temperatures;
}

celsius_t temp_get_min(void) {
    celsius_t min = TEMP_MAX_C;
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            min = MAINBOARD_MIN(min, htemp.temperatures[i][j]);
        }
    }
    return min;
}

celsius_t temp_get_max(void) {
    celsius_t max = 0U;
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            max = MAINBOARD_MAX(max, htemp.temperatures[i][j]);
        }
    }
    return max;
}

celsius_t temp_get_sum(void) {
    celsius_t sum = 0U;
    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            sum += htemp.temperatures[i][j];
        }
    }
    return sum;
}

celsius_t temp_get_avg(void) {
    return temp_get_sum() / CELLBOARD_TEMP_SENSOR_COUNT;
}

void temp_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t * const payload) {
    const size_t size = 4U;
    if (payload == NULL ||
       (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
       payload->offset + size > CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT)
       return;

    // Update temperatures
    const size_t offset = payload->offset;
    celsius_t * const temperatures = htemp.temperatures[payload->cellboard_id];
    temperatures[offset] = payload->temperature_0;
    temperatures[offset + 1U] = payload->temperature_1;
    temperatures[offset + 2U] = payload->temperature_2;
    temperatures[offset + 3U] = payload->temperature_3;
    for (size_t i = 0U; i < size; ++i)
        _temp_check_value((CellboardId)payload->cellboard_id, offset + i, temperatures[offset + i]);
}

primary_hv_cells_temperature_converted_t * temp_get_cells_temperature_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(htemp.temp_can_payload);

    const celsius_t * temperatures = htemp.temperatures[htemp.cellboard_id];
    htemp.temp_can_payload.cellboard_id = (primary_hv_cells_temperature_cellboard_id)htemp.cellboard_id;

    htemp.temp_can_payload.temperature_0 = temperatures[htemp.offset];
    htemp.temp_can_payload.temperature_1 = temperatures[htemp.offset + 1];
    htemp.temp_can_payload.temperature_2 = temperatures[htemp.offset + 2];
    htemp.temp_can_payload.temperature_3 = temperatures[htemp.offset + 3];

    htemp.temp_can_payload.temperature_id_0 = _temp_cell_position_from_index(htemp.offset);
    htemp.temp_can_payload.temperature_id_1 = _temp_cell_position_from_index(htemp.offset + 1);
    htemp.temp_can_payload.temperature_id_2 = _temp_cell_position_from_index(htemp.offset + 2);
    htemp.temp_can_payload.temperature_id_3 = _temp_cell_position_from_index(htemp.offset + 3);

    // Update indices
    htemp.offset += TEMP_NUM_TEMP_CAN_MESSAGE;
    if (htemp.offset >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        htemp.offset = 0U;
        if(++htemp.cellboard_id >= CELLBOARD_ID_COUNT)
            htemp.cellboard_id = 0U;
    }
    return &htemp.temp_can_payload;

}

primary_hv_cells_temp_stats_converted_t * temp_get_cells_temperature_stats_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(htemp.temp_stats_can_payload);

    htemp.temp_stats_can_payload.max = temp_get_max();
    htemp.temp_stats_can_payload.min = temp_get_min();

    htemp.temp_stats_can_payload.avg = temp_get_avg();

    return &htemp.temp_stats_can_payload;
}

#ifdef CONF_TEMPERATURE_STRINGS_ENABLE

_STATIC char * temp_module_name = "temperature";

_STATIC char * temp_return_code_name[] = {
    [TEMP_OK] = "ok",
    [TEMP_NULL_POINTER] = "null pointer",
    [TEMP_OUT_OF_BOUNDS] = "out of bounds"
};

_STATIC char * temp_return_code_description[] = {
    [TEMP_OK] = "executed successfully",
    [TEMP_NULL_POINTER] = "attempt to dereference a null pointer"
    [TEMP_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif  //  CONF_TEMPERATURE_STRINGS_ENABLE

#endif  //  CONF_TEMPERATURE_MODULE_ENABLE
