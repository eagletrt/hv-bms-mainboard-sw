/**
 * @file internal-voltage.c
 * @date 2024-07-19
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Internal voltages measurements and controls
 */

#include "internal-voltage.h"

#include <string.h>

#ifdef CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

_STATIC _InternalVoltageHandler hvolt;

InternalVoltageReturnCode internal_voltage_init(
    spi_send_callback_t send,
    spi_send_receive_callback_t send_receive)
{
    if (send == NULL || send_receive == NULL)
        return INTERNAL_VOLTAGE_NULL_POINTER;
    memset(&hvolt, 0U, sizeof(hvolt));
    (void)max22530_init(&hvolt.max22530, send, send_receive);
    return INTERNAL_VOLTAGE_OK;
}

InternalVoltageReturnCode internal_voltage_read_all(void) {
    (void)max22530_read_channels_all(&hvolt.max22530, true, hvolt.data, NULL);
    return INTERNAL_VOLTAGE_OK;
}

raw_volt_t internal_voltage_get_ts(void) {
    return hvolt.data[INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE];
}

raw_volt_t internal_voltage_get_batt(void) {
    return hvolt.data[INTERNAL_VOLTAGE_CHANNEL_BATT_VOLTAGE];
}

primary_hv_ts_voltage_converted_t * internal_voltage_get_ts_voltage_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.ts_voltage_can_payload);
    // TODO: Send ts voltages values
    // hvolt.ts_voltage_can_payload.ts = MAX22530_VALUE_TO_MILLIVOLT(internal_voltage_get_ts()) * 0.001;
    return &hvolt.ts_voltage_can_payload;
}

#ifdef CONF_INTERNAL_VOLTAGE_STRINGS_ENABLE

_STATIC char * internal_voltage_module_name = "internal voltage";

_STATIC char * internal_voltage_return_code_name[] = {
    [INTERNAL_VOLTAGE_OK] = "ok",
    [INTERNAL_VOLTAGE_NULL_POINTER] = "null pointer"
};

_STATIC char * internal_voltage_return_code_description[] = {
    [INTERNAL_VOLTAGE_OK] = "executed succefully",
    [INTERNAL_VOLTAGE_NULL_POINTER] = "attempt to dereference a null pointer"
};

#endif // CONF_INTERNAL_VOLTAGE_STRINGS_ENABLE

#endif // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE
