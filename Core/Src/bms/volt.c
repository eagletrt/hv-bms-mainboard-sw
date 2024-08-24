/**
 * @file volt.c
 * @date 2024-04-20
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Voltage measurment and control
 */

#include "volt.h"

#include <string.h>

#include "identity.h"
#include "timebase.h"
#include "error.h"

#ifdef CONF_VOLTAGE_MODULE_ENABLE

/**
 * @brief Voltages handler structure
 *
 * @param voltages The array of raw cells voltages
 * @param can_payload The canlib payload of the cells voltages
 * @param cellboard_id Cellboard identifier to set inside the payload
 * @param offset Cell offset to set inside the payload
 */
_STATIC struct {
    volt_matrix_t voltages;

    primary_hv_cells_voltage_converted_t can_payload;
    CellboardId cellboard_id;
    size_t offset;
} hvolt;

/**
 * @brief Check if the voltage values are in range otherwise set an error
 *
 * @param value The raw voltage value
 */
_STATIC_INLINE void _volt_check_value(raw_volt_t value) {

    /*
     * EV 5.8.7
     * The AMS must switch off the TS via the SDC, if a critical voltage value
     * according to the cell manufacturer’s datasheet or these rules
     * persistently occurs for more than 500 ms      
     */

    // TODO: Set errors
    // ERROR_TOGGLE_IF(
    //     value <= VOLT_MIN_VALUE,
    //     ERROR_GROUP_UNDER_VOLTAGE,
    //     ERROR_UNDER_VOLTAGE_INSTANCE_CELLS,
    //     timebase_get_time()
    // );
    // ERROR_TOGGLE_IF(
    //     value >= VOLT_MAX_VALUE,
    //     ERROR_GROUP_OVER_VOLTAGE,
    //     ERROR_UNDER_VOLTAGE_INSTANCE_CELLS,
    //     timebase_get_time()
    // );
}

VoltReturnCode volt_init(void) {
    memset(&hvolt, 0U, sizeof(hvolt));
    return VOLT_OK;
}

const volt_matrix_t * volt_get_values(void) {
    return &hvolt.voltages;
}

raw_volt_t volt_get_min(void) {
    raw_volt_t min = UINT16_MAX;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            min = MAINBOARD_MIN(hvolt.voltages[id][i], min);
    }
    return min;
}

raw_volt_t volt_get_max(void) {
    raw_volt_t max = 0U;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            max = MAINBOARD_MAX(hvolt.voltages[id][i], max);
    }
    return max;
}

float volt_get_avg(void) {
    float avg = 0U;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            avg += hvolt.voltages[id][i];
    }
    return avg / (CELLBOARD_SERIES_COUNT);
}

void volt_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t * payload) {
    if (payload == NULL ||
        (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
        payload->offset + 4U >= CELLBOARD_SEGMENT_SERIES_COUNT)
        return;
    // Update voltages
    raw_volt_t * volts = hvolt.voltages[payload->cellboard_id];
    volts[payload->offset] = payload->voltage_0;
    volts[payload->offset + 1U] = payload->voltage_1;
    volts[payload->offset + 2U] = payload->voltage_2;
    volts[payload->offset + 3U] = payload->voltage_3; 
    for (size_t i = 0U; i < 4U; ++i)
        _volt_check_value(volts[i]);
}

primary_hv_cells_voltage_converted_t * volt_get_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.can_payload);
    raw_volt_t * volts = hvolt.voltages[hvolt.cellboard_id];

    // Set payload values
    hvolt.can_payload.cellboard_id = hvolt.cellboard_id;
    hvolt.can_payload.offset = hvolt.offset;
    hvolt.can_payload.voltage_0 = VOLT_VALUE_TO_MILLIVOLT(volts[hvolt.offset]) * 0.001f;
    hvolt.can_payload.voltage_1 = VOLT_VALUE_TO_MILLIVOLT(volts[hvolt.offset + 1]) * 0.001f;
    hvolt.can_payload.voltage_2 = VOLT_VALUE_TO_MILLIVOLT(volts[hvolt.offset + 2]) * 0.001f;
    /**
     * hvolt.can_payload.voltage_3 = VOLT_VALUE_TO_MILLIVOLT(volts[hvolt.offset + 3]) * 0.001f; 
     * @TODO: fix cantools's handling of non 2^n size for floating point values     
     */

    // Upate indices
    hvolt.offset += 3; /** @TODO: view above */
    if (hvolt.offset >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        hvolt.offset = 0U;
        if (++hvolt.cellboard_id >= CELLBOARD_ID_COUNT)
            hvolt.cellboard_id = 0U;
    }
    return &hvolt.can_payload;
}

#ifdef CONF_VOLTAGE_STRINGS_ENABLE

_STATIC char * volt_module_name = "voltage";

_STATIC char * volt_return_code_name[] = {
    [VOLT_OK] = "ok",
    [VOLT_NULL_POINTER] = "null pointer",
    [VOLT_OUT_OF_BOUNDS] = "out of bounds"
};

_STATIC char * volt_return_code_description[] = {
    [VOLT_OK] = "executed successfully",
    [VOLT_NULL_POINTER] = "attempt to dereference a null pointer"
    [VOLT_OUT_OF_BOUNDS] = "attempt to access an invalid memory region"
};

#endif // CONF_VOLTAGE_STRINGS_ENABLE

#endif // CONF_VOLTAGE_MODULE_ENABLE
