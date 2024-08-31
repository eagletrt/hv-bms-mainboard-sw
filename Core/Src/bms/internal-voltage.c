/**
 * @file internal-voltage.c
 * @date 2024-07-19
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Internal voltages measurements and controls
 */

#include "internal-voltage.h"

#include <string.h>

#include "volt.h"

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
    volt_t volts[INTERNAL_VOLTAGE_CHANNEL_COUNT];
    (void)max22530_read_channels_all(&hvolt.max22530, true, volts, NULL);
    hvolt.ts = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE]);
    hvolt.pack = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE]);
    // TODO: Convert and update the IMD TS connected feedback and precharge temperature
    return INTERNAL_VOLTAGE_OK;
}

volt_t internal_voltage_get_ts(void) {
    return hvolt.ts;
}

volt_t internal_voltage_get_pack(void) {
    return hvolt.pack;
}

primary_hv_ts_voltage_converted_t * internal_voltage_get_ts_voltage_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hvolt.ts_voltage_can_payload);
    hvolt.ts_voltage_can_payload.ts = hvolt.ts;
    hvolt.ts_voltage_can_payload.pack = hvolt.pack;
    hvolt.ts_voltage_can_payload.cells_sum = volt_get_sum();
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
