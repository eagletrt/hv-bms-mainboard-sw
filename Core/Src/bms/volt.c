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

_STATIC _VoltHandler hvolt;

/**
 * @brief Check if the voltage values are in range otherwise set an error
 *
 * @param value The raw voltage value
 */
_STATIC_INLINE void _volt_check_value(raw_volt_t value) {
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
    /*
     * Set the initial value of the voltages as maximum to avoid
     * problems during the balancing procedure
     */
    for (CellboardId id = 0U; id < CELLBOARD_ID_COUNT; ++id)
        for (size_t cell = 0U; cell < CELLBOARD_SEGMENT_SERIES_COUNT; ++cell)
            hvolt.voltages[id][cell] = VOLT_MAX_VALUE;
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

volt_t volt_get_sum(void) {
    float sum = 0.f;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            sum += VOLT_VALUE_TO_VOLT(hvolt.voltages[id][i]);
    }
    return sum;
}

void volt_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t * payload) {
    const size_t size = 3U;
    if (payload == NULL ||
        (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
        payload->offset + size >= CELLBOARD_SEGMENT_SERIES_COUNT)
        return;
    // Update voltages
    size_t offset = payload->offset;
    raw_volt_t * volts = hvolt.voltages[payload->cellboard_id];

    volts[offset] = VOLT_VOLT_TO_VALUE(payload->voltage_0);
    volts[offset + 1U] = VOLT_VOLT_TO_VALUE(payload->voltage_1);
    volts[offset + 2U] = VOLT_VOLT_TO_VALUE(payload->voltage_2);
    /*
     * volts[payload->offset + 3U] = VOLT_VOLT_TO_VALUE(payload->voltage_3);
     * TODO: fix cantools's handling of non 2^n size for floating point values     
     */
    for (size_t i = 0U; i < size; ++i)
        _volt_check_value(offset + volts[i]);
}

primary_hv_cells_voltage_converted_t * volt_get_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.volt_can_payload);
    raw_volt_t * volts = hvolt.voltages[hvolt.cellboard_id];

    // Set payload values
    hvolt.volt_can_payload.cellboard_id = hvolt.cellboard_id;
    hvolt.volt_can_payload.offset = hvolt.offset;
    // Voltages needs to be converted in volts to be sent
    hvolt.volt_can_payload.voltage_0 = VOLT_VALUE_TO_VOLT(volts[hvolt.offset]);
    hvolt.volt_can_payload.voltage_1 = VOLT_VALUE_TO_VOLT(volts[hvolt.offset + 1]);
    hvolt.volt_can_payload.voltage_2 = VOLT_VALUE_TO_VOLT(volts[hvolt.offset + 2]);
    /*
     * hvolt.volt_can_payload.voltage_3 = VOLT_VALUE_TO_MILLIVOLT(volts[hvolt.offset + 3]) * 0.001f; 
     * TODO: fix cantools's handling of non 2^n size for floating point values     
     */

    // Update indices
    hvolt.offset += 3; /** @TODO: view above */
    if (hvolt.offset >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        hvolt.offset = 0U;
        if (++hvolt.cellboard_id >= CELLBOARD_ID_COUNT)
            hvolt.cellboard_id = 0U;
    }
    return &hvolt.volt_can_payload;
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
