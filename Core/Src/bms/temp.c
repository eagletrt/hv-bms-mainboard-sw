/**
 * @file temp.c
 * @date 2024-08-30
 * @author Riccardo Segala [riccardo.segala@icloud.com]
 *
 * @brief Temperature measurment and control
 */

#include "temp.h"

#include <string.h>

#ifdef CONF_TEMPERATURE_MODULE_ENABLE

_STATIC _TempHandler htemp;

/**
 * @brief Check if the temperature values are in range otherwise set an error
 *
 * @param value The celsius temperature value
 */
_STATIC_INLINE void _temp_check_value(celsius_temp_t value) {}

TempReturnCode temp_init(void) {
    memset(&htemp, 0U, sizeof(htemp));
    return TEMP_OK;
}

const cells_temp_t * temp_get_values(void) {
    return &htemp.temperatures;
}

celsius_t temp_get_min(void) {
    celsius_t min = UINT16_MAX;

    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            min = MAINBOARD_MIN(min, htemp.temperatures[i][j]);
        }
    }

    return min;
}

celsius_t temp_get_max(void) {
    celsius_t max = UINT16_MAX;

    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            max = MAINBOARD_MAX(max, htemp.temperatures[i][j]);
        }
    }

    return max;
}

celsius_t temp_get_avg(void) {
    float avg = 0U;

    for (size_t i = 0U; i < CELLBOARD_COUNT; ++i) {
        for (size_t j = 0U; j < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++j) {
            avg += htemp.temperatures[i][j];
        }
    }

    return avg / CELLBOARD_TEMP_SENSOR_COUNT;
}

void temp_cells_temperature_handle(bms_cellboard_cells_temperature_converted_t * payload) {
    const size_t size = 4U;
    if (payload ==NULL ||
       (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
       payload->offset + size >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT)
       return;

    // Update temperatures
    size_t offset = payload->offset;
    celsius_t * temperatures = htemp.temperatures[payload->cellboard_id];

    temperatures[offset] = payload->temperature_0;
    temperatures[offset + 1U] = payload->temperature_1;
    temperatures[offset + 2U] = payload->temperature_2;
    temperatures[offset + 3U] = payload->temperature_3;

    for (size_t i = 0U; i < size; ++i)
        _temp_check_value(temperatures[offset + i]);
}

primary_hv_cells_temperature_converted_t * temp_get_temp_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(htemp.temp_can_payload);

    celsius_t * temperatures = htemp.temperatures[htemp.cellboard_id];

    htemp.temp_can_payload.cellboard_id = htemp.cellboard_id;
    htemp.temp_can_payload.offset = htemp.offset;

    htemp.temp_can_payload.temperature_0 = temperatures[htemp.offset];
    htemp.temp_can_payload.temperature_1 = temperatures[htemp.offset + 1];
    htemp.temp_can_payload.temperature_2 = temperatures[htemp.offset + 2];
    htemp.temp_can_payload.temperature_3 = temperatures[htemp.offset + 3];

    // Update indices
    htemp.offset += 4;
    if (htemp.offset >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        htemp.offset = 0;
        if(++htemp.cellboard_id >= CELLBOARD_ID_COUNT)
            htemp.cellboard_id = 0U;
    }
    return &htemp.volt_can_payload;

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
