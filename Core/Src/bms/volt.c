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
 */
_STATIC struct {
    raw_volt_t voltages[CELLBOARD_SEGMENT_SERIES_COUNT];

    bms_cellboard_cells_voltage_converted_t can_payload;
    size_t offset;
} hvolt;

/**
 * @brief Check if the voltage values are in range otherwise set an error
 *
 * @param value The raw voltage value
 */
_STATIC_INLINE void _volt_check_value(raw_volt_t value) {
    ERROR_TOGGLE_IF(
        value <= VOLT_MIN_VALUE,
        ERROR_GROUP_UNDER_VOLTAGE,
        ERROR_UNDER_VOLTAGE_INSTANCE_CELLS,
        timebase_get_time()
    );
    ERROR_TOGGLE_IF(
        value >= VOLT_MAX_VALUE,
        ERROR_GROUP_OVER_VOLTAGE,
        ERROR_UNDER_VOLTAGE_INSTANCE_CELLS,
        timebase_get_time()
    );
}

VoltReturnCode volt_init(void) {
    memset(&hvolt, 0U, sizeof(hvolt));
    hvolt.can_payload.cellboard_id = identity_get_cellboard_id();
    return VOLT_OK;
}

VoltReturnCode volt_update_value(size_t index, raw_volt_t value) {
    if (index > CELLBOARD_SEGMENT_SERIES_COUNT)
        return VOLT_OUT_OF_BOUNDS;
    hvolt.voltages[index] = value;
    _volt_check_value(value);
    return VOLT_OK;
}

VoltReturnCode volt_update_values(size_t index, raw_volt_t * values, size_t size) {
    if (index + size > CELLBOARD_SEGMENT_SERIES_COUNT)
        return VOLT_OUT_OF_BOUNDS;
    memcpy(hvolt.voltages + index, values, size * sizeof(hvolt.voltages[0]));
    for (size_t i = 0U; i < size; ++i)
        _volt_check_value(hvolt.voltages[index + i]);
    return VOLT_OK;
}

const raw_volt_t * volt_get_values(void) {
    return hvolt.voltages;
}

bit_flag32_t volt_select_values(millivolt_t target) {
    bit_flag32_t bits = 0U;
    const size_t cnt = CELLBOARD_MIN(CELLBOARD_SEGMENT_SERIES_COUNT, sizeof(bits) * 8U);

    // Iterate over cells and choose cells which voltage is greater than the target
    for (size_t i = 0U; i < cnt; ++i) {
        if (VOLT_VALUE_TO_MILLIVOLT(hvolt.voltages[i]) > target)
            bits = CELLBOARD_BIT_SET(bits, i);
    }
    return bits;
}

VoltReturnCode volt_dump_values(raw_volt_t * out, size_t start, size_t size) {
    if (out == NULL)
        return VOLT_NULL_POINTER;
    if (start >= CELLBOARD_SEGMENT_SERIES_COUNT || start + size >= CELLBOARD_SEGMENT_SERIES_COUNT)
        return VOLT_OUT_OF_BOUNDS;
    memcpy(out, hvolt.voltages + start, size * sizeof(raw_volt_t));
    return VOLT_OK;
}

bms_cellboard_cells_voltage_converted_t * volt_get_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.can_payload);

    hvolt.can_payload.offset = hvolt.offset;
    hvolt.can_payload.voltage_0 = hvolt.voltages[hvolt.offset];
    hvolt.can_payload.voltage_1 = hvolt.voltages[hvolt.offset + 1];
    hvolt.can_payload.voltage_2 = hvolt.voltages[hvolt.offset + 2];
    hvolt.can_payload.voltage_3 = hvolt.voltages[hvolt.offset + 3];

    hvolt.offset += 4;
    if (hvolt.offset >= CELLBOARD_SEGMENT_SERIES_COUNT)
        hvolt.offset = 0U;
    return &hvolt.can_payload;
}

VoltReturnCode volt_routine(void) {
    return VOLT_OK;
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
