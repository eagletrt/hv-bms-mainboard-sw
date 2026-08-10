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

#include "can-primary.h"
#include "eagletrt.h"
#include "error-api.h"
#include "internal-voltage-api.h"
#include "watchdog.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_api_handler;

/*!
 * \brief Timeout callback for the sensor communication watchdog
 */
EAGLETRT_STATIC void prv_current_api_sensor_communcation_timeout(void) {
    error_api_set(ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION, 0U);
}

/*!
 * \brief Check if the current values are in range otherwise set an error
 *
 * \param value The raw current value
 */
EAGLETRT_STATIC_INLINE void prv_current_api_check_value(const ampere_t value) {
    if (value <= CURRENT_MIN_A || value >= CURRENT_MAX_A) {
        error_api_set(ERROR_GROUP_OVER_CURRENT, 0U);
    } else {
        error_api_reset(ERROR_GROUP_OVER_CURRENT, 0U);
    }

    if (fabsf(current_api_get_power()) >= CURRENT_MAX_POWER_KW) {
        error_api_set(ERROR_GROUP_OVER_POWER, 0U);
    } else {
        error_api_reset(ERROR_GROUP_OVER_POWER, 0U);
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

void current_api_set_current(ampere_t current) {
    enum WatchdogReturnCode result = watchdog_reset(&current_api_handler.sensor_wdg);
    current_api_handler.current = current;
    if (result == WATCHDOG_RC_TIMED_OUT) {
        watchdog_restart(&current_api_handler.sensor_wdg);
    }
}

kilowatt_t current_api_get_power(void) {
    constexpr float w_to_kw = 0.001F;
    return (kilowatt_t)(current_api_handler.current * internal_voltage_api_get_ts() * w_to_kw);
}

enum WatchdogReturnCode current_api_start_sensor_communication_watchdog(void) {
    return watchdog_start(&current_api_handler.sensor_wdg);
}

union CanPrimaryMessages *current_api_get_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardcurrentinfo;
    }
    current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.current = current_api_get_current();
    current_api_handler.libcan_message_current.tsacmainboardcurrentinfo.power = current_api_get_power();
    return &current_api_handler.libcan_message_current;
}

#ifdef CONF_CURRENT_STRINGS_ENABLE

EAGLETRT_STATIC char *current_api_module_name = "current";

EAGLETRT_STATIC char *current_api_return_code_name[] = {
    [CURRENT_RC_OK] = "ok"
};

EAGLETRT_STATIC char *current_api_return_code_description[] = {
    [CURRENT_RC_OK] = "executed succesfully"
};

#endif // CONF_CURRENT_STRINGS_ENABLE

#endif // CONF_CURRENT_MODULE_ENABLE
