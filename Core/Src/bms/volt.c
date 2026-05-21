/**
 * @file volt.c
 * @date 2024-04-20
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Voltage measurment and control
 */

#include "volt.h"

#include <string.h>

#include "error-api.h"
#include "identity.h"
#include "timebase.h"

#ifdef CONF_VOLTAGE_MODULE_ENABLE

_STATIC _VoltHandler volt_handler;

/**
 * @brief Check if the voltage values are in range otherwise set an error
 *
 * @param id The cellboard identifier
 * @param offset The cell offset of the segment
 * @param value The voltage value in V
 */
_STATIC_INLINE void _volt_check_value(const CellboardId id, const size_t offset, const volt_t value) {
    size_t index = id * CELLBOARD_SEGMENT_SERIES_COUNT + offset;
    if (value < VOLT_MIN_V)
        error_set(ERROR_GROUP_UNDER_VOLTAGE, index);
    else
        error_reset(ERROR_GROUP_UNDER_VOLTAGE, index);

    if (value > VOLT_MAX_V)
        error_set(ERROR_GROUP_OVER_VOLTAGE, index);
    else
        error_reset(ERROR_GROUP_OVER_VOLTAGE, index);
}

VoltReturnCode volt_init(void) {
    memset(&volt_handler, 0U, sizeof(volt_handler));
    /*
   * Set the initial value of the voltages as maximum to avoid
   * problems during the balancing procedure
   */
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id)
        for (size_t cell = 0U; cell < CELLBOARD_SEGMENT_SERIES_COUNT; ++cell)
            volt_handler.voltages[id][cell] = VOLT_MAX_V;
    return VOLT_OK;
}

const cells_voltage_t *volt_get_values(void) {
    return &volt_handler.voltages;
}

volt_t volt_get_min(void) {
    volt_t min = VOLT_MAX_V;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
            min = MAINBOARD_MIN(volt_handler.voltages[id][i], min);
        }
    }
    return min;
}

volt_t volt_get_max(void) {
    volt_t max = 0.f;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
            max = MAINBOARD_MAX(volt_handler.voltages[id][i], max);
        }
    }
    return max;
}

volt_t volt_get_sum(void) {
    volt_t sum = 0.f;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
            sum += volt_handler.voltages[id][i];
        }
    }
    return sum;
}

volt_t volt_get_avg(void) {
    return volt_get_sum() / CELLBOARD_SERIES_COUNT;
}

void volt_cells_voltage_handle(
    bms_cellboard_cells_voltage_converted_t *const payload) {
    const size_t size = 3U;
    if (payload == NULL ||
        (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT ||
        payload->offset + size > CELLBOARD_SEGMENT_SERIES_COUNT)
        return;
    // Update voltages
    const size_t offset = payload->offset;
    volt_t *volts = volt_handler.voltages[payload->cellboard_id];
    volts[offset] = payload->voltage_0;
    volts[offset + 1U] = payload->voltage_1;
    volts[offset + 2U] = payload->voltage_2;
    for (size_t i = 0U; i < size; ++i)
        _volt_check_value((CellboardId)payload->cellboard_id, offset + i, volts[offset + i]);
}

primary_hv_cells_voltage_converted_t *
volt_get_cells_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(volt_handler.volt_can_payload);
    const volt_t *const volts = volt_handler.voltages[volt_handler.cellboard_id];
    // Set payload values
    volt_handler.volt_can_payload.cellboard_id =
        (primary_hv_cells_voltage_cellboard_id)volt_handler.cellboard_id;
    volt_handler.volt_can_payload.offset = volt_handler.offset;
    volt_handler.volt_can_payload.voltage_0 = volts[volt_handler.offset];
    volt_handler.volt_can_payload.voltage_1 = volts[volt_handler.offset + 1];
    volt_handler.volt_can_payload.voltage_2 = volts[volt_handler.offset + 2];

    // Update indices
    volt_handler.offset += 3;
    if (volt_handler.offset >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        volt_handler.offset = 0U;
        if (++volt_handler.cellboard_id >= CELLBOARD_ID_COUNT)
            volt_handler.cellboard_id = 0U;
    }
    return &volt_handler.volt_can_payload;
}

primary_hv_cells_voltage_stats_converted_t *
volt_get_cells_voltage_stats_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(volt_handler.volt_stats_can_payload);

    const volt_t max = volt_get_max();
    const volt_t min = volt_get_min();

    volt_handler.volt_stats_can_payload.max = max;
    volt_handler.volt_stats_can_payload.min = min;

    volt_handler.volt_stats_can_payload.delta = max - min;

    volt_handler.volt_stats_can_payload.avg = volt_get_avg();

    return &volt_handler.volt_stats_can_payload;
}

#ifdef CONF_VOLTAGE_STRINGS_ENABLE

_STATIC char *volt_module_name = "voltage";

_STATIC char *volt_return_code_name[] = { [VOLT_OK] = "ok",
                                          [VOLT_NULL_POINTER] = "null pointer",
                                          [VOLT_OUT_OF_BOUNDS] =
                                              "out of bounds" };

_STATIC char *volt_return_code_description[] = {
    [VOLT_OK] = "executed successfully",
    [VOLT_NULL_POINTER] =
        "attempt to dereference a null pointer"[VOLT_OUT_OF_BOUNDS] =
            "attempt to access an invalid memory region"
};

#endif // CONF_VOLTAGE_STRINGS_ENABLE

#endif // CONF_VOLTAGE_MODULE_ENABLE
