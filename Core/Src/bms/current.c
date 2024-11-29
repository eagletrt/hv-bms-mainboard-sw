/**
 * @file current.c
 * @date 2024-06-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Current measurement and control
 */

#include "current.h"

#include <string.h>
#include <math.h>

#include "timebase.h"
#include "error.h"
#include "internal-voltage.h"
#include "volt.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

_STATIC _CurrentHandler hcurrent;

/**
 * @brief Timeout callback for the sensor communication watchdog
 */
void _current_sensor_communcation_timeout(void) {
    error_set(ERROR_GROUP_CURRENT_SENSOR_COMMUNICATION, 0U);
}


/**
 * @brief Check if the current values are in range otherwise set an error
 *
 * @param value The raw current value
 */
_STATIC_INLINE void _current_check_value(const ampere_t value) {
    if (value <= CURRENT_MIN_A || value >= CURRENT_MAX_A)
        error_set(ERROR_GROUP_OVER_CURRENT, 0U);
    else
        error_reset(ERROR_GROUP_OVER_CURRENT, 0U);

    if (fabsf(current_get_power()) >= CURRENT_MAX_POWER_KW)
        error_set(ERROR_GROUP_OVER_POWER, 0U);
    else
        error_reset(ERROR_GROUP_OVER_POWER, 0U);
}

CurrentReturnCode current_init(void) {
    memset(&hcurrent, 0U, sizeof(hcurrent));
    (void)watchdog_init(
        &hcurrent.sensor_wdg,
        CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS,
        _current_sensor_communcation_timeout
    );
    return CURRENT_OK;
}

ampere_t current_get_current(void) {
    return hcurrent.current;
}

kilowatt_t current_get_power(void) {
    return hcurrent.current * internal_voltage_get_ts() * 0.001;
}

WatchdogReturnCode current_start_sensor_communication_watchdog(void) {
    return watchdog_start(&hcurrent.sensor_wdg);
}

void current_handle(bms_ivt_msg_result_i_t * const payload) {
    watchdog_reset(&hcurrent.sensor_wdg);
    if (payload == NULL)
        return;
    hcurrent.current = payload->ivt_result_i * 0.001f;
    // BUG: Read current has the opposite sign
    hcurrent.current = -hcurrent.current;
    _current_check_value(hcurrent.current);
}

primary_hv_current_converted_t * current_get_current_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcurrent.current_can_payload);
    hcurrent.current_can_payload.current = hcurrent.current;
    return &hcurrent.current_can_payload;
}

primary_hv_power_converted_t * current_get_power_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcurrent.power_can_payload);
    hcurrent.power_can_payload.power = current_get_power();
    return &hcurrent.power_can_payload;
}

#ifdef CONF_CURRENT_STRINGS_ENABLE

_STATIC char * current_module_name = "current";

_STATIC char * current_return_code_name[] = {
    [CURRENT_OK] = "ok"
};

_STATIC char * current_return_code_description[] = {
    [CURRENT_OK] = "executed succesfully"
};

#endif // CONF_CURRENT_STRINGS_ENABLE

#endif // CONF_CURRENT_MODULE_ENABLE
