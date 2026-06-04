/*!
 * \file current.c
 * \date 2024-06-08
 * \author Antonio Gelain [antonio.gelain2@gmail.com]ù
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Current measurement and control
 */

#include "current-api.h"

#include <string.h>
#include <math.h>

#include "error.h"
#include "internal-voltage-api.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_api_handler;

/*!
 * \brief Timeout callback for the sensor communication watchdog
 */
EAGLETRT_STATIC void prv_current_api_sensor_communcation_timeout(void) {
    error_set(ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION, 0U);
}

/*!
 * \brief Check if the current values are in range otherwise set an error
 *
 * \param value The raw current value
 */
EAGLETRT_STATIC_INLINE void prv_current_api_check_value(const ampere_t value) {
    if (value <= CURRENT_MIN_A || value >= CURRENT_MAX_A) {
        error_set(ERROR_GROUP_OVER_CURRENT, 0U);
    } else {
        error_reset(ERROR_GROUP_OVER_CURRENT, 0U);
    }

    if (fabsf(current_api_get_power()) >= CURRENT_MAX_POWER_KW) {
        error_set(ERROR_GROUP_OVER_POWER, 0U);
    } else {
        error_reset(ERROR_GROUP_OVER_POWER, 0U);
    }
}

enum CurrentReturnCode current_api_init(void) {
    memset(&current_api_handler, 0U, sizeof(current_api_handler));
    (void)watchdog_init(
        &current_api_handler.sensor_wdg,
        CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS,
        prv_current_api_sensor_communcation_timeout);
    return CURRENT_RC_OK;
}

ampere_t current_api_get_current(void) {
    return current_api_handler.current;
}

kilowatt_t current_api_get_power(void) {
    constexpr float w_to_kw = 0.001F;
    return (kilowatt_t)(current_api_handler.current * internal_voltage_api_get_ts() * w_to_kw);
}

WatchdogReturnCode current_api_start_sensor_communication_watchdog(void) {
    return watchdog_start(&current_api_handler.sensor_wdg);
}

void current_api_handle(bms_ivt_msg_result_i_t *const payload) {
    watchdog_reset(&current_api_handler.sensor_wdg);
    if (payload == NULL) {
        return;
    }

    constexpr float ma_to_a = 0.001F;
    current_api_handler.current = (float)payload->ivt_result_i * ma_to_a;
    prv_current_api_check_value(current_api_handler.current);
}

primary_hv_current_converted_t *current_api_get_current_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(current_api_handler.current_can_payload);
    }
    current_api_handler.current_can_payload.current = current_api_handler.current;
    return &current_api_handler.current_can_payload;
}

primary_hv_power_converted_t *current_api_get_power_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(current_api_handler.power_can_payload);
    }
    current_api_handler.power_can_payload.power = current_api_get_power();
    return &current_api_handler.power_can_payload;
}

#ifdef CONF_CURRENT_STRINGS_ENABLE

EAGLETRT_STATIC char *current_module_name = "current";

EAGLETRT_STATIC char *current_return_code_name[] = {
    [CURRENT_RC_OK] = "ok"
};

EAGLETRT_STATIC char *current_return_code_description[] = {
    [CURRENT_RC_OK] = "executed succesfully"
};

#endif // CONF_CURRENT_STRINGS_ENABLE

#endif // CONF_CURRENT_MODULE_ENABLE
