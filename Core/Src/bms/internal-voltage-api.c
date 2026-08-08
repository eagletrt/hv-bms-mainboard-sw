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

#include "can-primary.h"
#include "eagletrt.h"
#include "max22530.h"
#include "max22530-api.h"
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
    enum Max22530ReturnCode result = max22530_api_read_channels_all(&internal_volt_handler.max22530, true, volts, NULL);
    if (result != MAX22530_RC_OK) {
        return INTERNAL_VOLTAGE_RC_DRIVER_ERROR;
    }
    internal_volt_handler.ts = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE]);
    internal_volt_handler.pack = INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(volts[INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE]);
    return INTERNAL_VOLTAGE_RC_OK;
}

volt_t internal_voltage_api_get_ts(void) {
    return internal_volt_handler.ts;
}

volt_t internal_voltage_api_get_pack(void) {
    return internal_volt_handler.pack;
}

union CanPrimaryMessages *internal_voltage_api_get_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardvoltageinfo;
    }
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.ts = internal_volt_handler.ts;
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.total = internal_volt_handler.pack;
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.cellsum = volt_api_get_sum();
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.min = volt_api_get_min();
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.max = volt_api_get_max();
    internal_volt_handler.libcan_message_voltage.tsacmainboardvoltageinfo.average = volt_api_get_avg();
    return &internal_volt_handler.libcan_message_voltage;
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
