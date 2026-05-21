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

#include "timebase.h"
#include "error.h"
#include "internal-voltage.h"
#include "volt.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

EAGLETRT_STATIC struct CurrentHandler current_handler;

/*!
 * \brief Timeout callback for the sensor communication watchdog
 */
EAGLETRT_STATIC void prv_current_sensor_communcation_timeout(void) {
    error_set(ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION, 0U);
}

/*!
 * \brief Check if the current values are in range otherwise set an error
 *
 * \param value The raw current value
 */
EAGLETRT_STATIC_INLINE void prv_current_check_value(const ampere_t value) {
    if (value <= CURRENT_MIN_A || value >= CURRENT_MAX_A) {
        error_set(ERROR_GROUP_OVER_CURRENT, 0U);
    } else {
        error_reset(ERROR_GROUP_OVER_CURRENT, 0U);
    }

    if (fabsf(current_get_power()) >= CURRENT_MAX_POWER_KW) {
        error_set(ERROR_GROUP_OVER_POWER, 0U);
    } else {
        error_reset(ERROR_GROUP_OVER_POWER, 0U);
    }
}

enum CurrentReturnCode current_init(void) {
    memset(&current_handler, 0U, sizeof(current_handler));
    (void)watchdog_init(
        &current_handler.sensor_wdg,
        CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS,
        prv_current_sensor_communcation_timeout);
    return CURRENT_RC_OK;
}

ampere_t current_get_current(void) {
    return current_handler.current;
}

kilowatt_t current_get_power(void) {

    constexpr float epsilon = 0.001F;

    return (kilowatt_t)(current_handler.current * internal_voltage_get_ts() * epsilon);
}

WatchdogReturnCode current_start_sensor_communication_watchdog(void) {
    return watchdog_start(&current_handler.sensor_wdg);
}

void current_handle(bms_ivt_msg_result_i_t *const payload) {

    constexpr float epsilon = 0.001F;

    watchdog_reset(&current_handler.sensor_wdg);
    if (payload == NULL) {
        return;
    }
    current_handler.current = (float)payload->ivt_result_i * epsilon;
    prv_current_check_value(current_handler.current);
}

primary_hv_current_converted_t *current_get_current_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(current_handler.current_can_payload);
    }
    current_handler.current_can_payload.current = current_handler.current;
    return &current_handler.current_can_payload;
}

primary_hv_power_converted_t *current_get_power_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(current_handler.power_can_payload);
    }
    current_handler.power_can_payload.power = current_get_power();
    return &current_handler.power_can_payload;
}

#ifdef CONF_CURRENT_STRINGS_ENABLE

EAGLETRT_STATIC char *current_module_name = "current";

EAGLETRT_STATIC char *current_return_code_name[] = {
    [CURRENT_OK] = "ok"
};

EAGLETRT_STATIC char *current_return_code_description[] = {
    [CURRENT_OK] = "executed succesfully"
};

#endif // CONF_CURRENT_STRINGS_ENABLE

#endif // CONF_CURRENT_MODULE_ENABLE
