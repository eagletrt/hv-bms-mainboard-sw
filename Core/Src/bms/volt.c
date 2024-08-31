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
 * @param value The voltage value in V
 */
_STATIC_INLINE void _volt_check_value(const volt_t value) {
    MAINBOARD_UNUSED(value);
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
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id)
        for (size_t cell = 0U; cell < CELLBOARD_SEGMENT_SERIES_COUNT; ++cell)
            hvolt.voltages[id][cell] = VOLT_MAX_V;
    return VOLT_OK;
}

const cells_voltage_t * volt_get_values(void) {
    return &hvolt.voltages;
}

volt_t volt_get_min(void) {
    volt_t min = VOLT_MAX_V;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            min = MAINBOARD_MIN(hvolt.voltages[id][i], min);
    }
    return min;
}

volt_t volt_get_max(void) {
    volt_t max = 0.f;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            max = MAINBOARD_MAX(hvolt.voltages[id][i], max);
    }
    return max;
}

volt_t volt_get_sum(void) {
    volt_t sum = 0.f;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i)
            sum += hvolt.voltages[id][i];
    }
    return sum;
}

volt_t volt_get_avg(void) {
    return volt_get_sum() / CELLBOARD_SERIES_COUNT;
}

void volt_cells_voltage_handle(bms_cellboard_cells_voltage_converted_t * const payload) {
    const size_t size = 3U;
    if (payload == NULL ||
        (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
        payload->offset + size >= CELLBOARD_SEGMENT_SERIES_COUNT)
        return;
    // Update voltages
    const size_t offset = payload->offset;
    volt_t * volts = hvolt.voltages[payload->cellboard_id];
    volts[offset] = payload->voltage_0;
    volts[offset + 1U] = payload->voltage_1;
    volts[offset + 2U] = payload->voltage_2;
    for (size_t i = 0U; i < size; ++i)
        _volt_check_value(offset + volts[i]);
}

primary_hv_cells_voltage_converted_t * volt_get_cells_voltage_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.volt_can_payload);
    const volt_t * const volts = hvolt.voltages[hvolt.cellboard_id];
    // Set payload values
    hvolt.volt_can_payload.cellboard_id = (primary_hv_cells_voltage_cellboard_id)hvolt.cellboard_id;
    hvolt.volt_can_payload.offset = hvolt.offset;
    hvolt.volt_can_payload.voltage_0 = volts[hvolt.offset];
    hvolt.volt_can_payload.voltage_1 = volts[hvolt.offset + 1];
    hvolt.volt_can_payload.voltage_2 = volts[hvolt.offset + 2];

    // Update indices
    hvolt.offset += 3;
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
