/*!
 * \file internal-voltage.c
 * \date 2024-07-19
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages measurements and controls
 */

#include "internal-voltage-api.h"

#include <string.h>

#include "eagletrt-api.h"
#include "volt-api.h"

#ifdef CONF_INTERNAL_VOLTAGE_MODULE_ENABLE

EAGLETRT_STATIC struct InternalVoltageHandler internal_volt_handler;

enum InternalVoltageReturnCode internal_voltage_api_init(spi_send_callback_t send, spi_send_receive_callback_t send_receive) {
    if (send == NULL || send_receive == NULL) {
        return INTERNAL_VOLTAGE_RC_NULL_POINTER;
    }
    memset(&internal_volt_handler, 0U, sizeof(internal_volt_handler));
    (void)max22530_api_init(&internal_volt_handler.max22530, send, send_receive);
    return INTERNAL_VOLTAGE_RC_OK;
}

enum InternalVoltageReturnCode internal_voltage_api_read_all(void) {
    volt_t volts[INTERNAL_VOLTAGE_CHANNEL_COUNT];
    (void)max22530_api_read_channels_all(&internal_volt_handler.max22530, true, volts, NULL);
    internal_volt_handler.ts = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE]);
    internal_volt_handler.pack = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE]);
    // TODO: Convert and update the IMD TS connected feedback and precharge
    // temperature
    return INTERNAL_VOLTAGE_RC_OK;
}

volt_t internal_voltage_api_get_ts(void) {
    return internal_volt_handler.ts;
}

volt_t internal_voltage_api_get_pack(void) {
    return internal_volt_handler.pack;
}

primary_hv_ts_voltage_converted_t *internal_voltage_api_get_ts_voltage_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(internal_volt_handler.ts_voltage_can_payload);
    }
    internal_volt_handler.ts_voltage_can_payload.ts = internal_volt_handler.ts;
    internal_volt_handler.ts_voltage_can_payload.pack = internal_volt_handler.pack;
    internal_volt_handler.ts_voltage_can_payload.cells_sum = volt_api_get_sum();
    return &internal_volt_handler.ts_voltage_can_payload;
}

#ifdef CONF_INTERNAL_VOLTAGE_STRINGS_ENABLE

EAGLETRT_STATIC char *internal_voltage_module_name = "internal voltage";

EAGLETRT_STATIC char *internal_voltage_return_code_name[] = {
    [INTERNAL_VOLTAGE_RC_OK] = "ok",
    [INTERNAL_VOLTAGE_RC_NULL_POINTER] = "null pointer"
};

EAGLETRT_STATIC char *internal_voltage_return_code_description[] = {
    [INTERNAL_VOLTAGE_RC_OK] = "executed succefully",
    [INTERNAL_VOLTAGE_RC_NULL_POINTER] = "attempt to dereference a null pointer"
};

#endif // CONF_INTERNAL_VOLTAGE_STRINGS_ENABLE

#endif // CONF_INTERNAL_VOLTAGE_MODULE_ENABLE
