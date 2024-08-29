/**
 * @file current.c
 * @date 2024-06-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Current measurement and control
 */

#include "current.h"

#include <string.h>

#include "timebase.h"
#include "error.h"
#include "internal-voltage.h"
#include "volt.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

_STATIC _CurrentHandler hcurrent;

/**
 * @brief Check if the current values are in range otherwise set an error
 *
 * @param value The raw current value
 */
_STATIC_INLINE void _current_check_value(raw_current_t value) {
    if (value <= CURRENT_MIN_VALUE || value >= CURRENT_MAX_VALUE) {
        error_set(ERROR_GROUP_OVER_CURRENT, 0U);
    }
    else {
        error_reset(ERROR_GROUP_OVER_CURRENT, 0U);
    }
}

CurrentReturnCode current_init(void) {
    memset(&hcurrent, 0U, sizeof(hcurrent));
    return CURRENT_OK;
}

void current_handle(bms_ivt_msg_result_i_t * payload) {
    if (payload == NULL)
        return;
    hcurrent.current = payload->ivt_result_i;
    _current_check_value(hcurrent.current);
}

raw_current_t current_get_current(void) {
    return hcurrent.current;
}

kilowatt_t current_get_power(void) {
    return CURRENT_VALUE_TO_AMPERE(hcurrent.current) * VOLT_VALUE_TO_VOLT(internal_voltage_get_ts()) * 0.001f;
}

primary_hv_current_converted_t * current_get_current_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcurrent.current_can_payload);

    // Save current value in A
    hcurrent.current_can_payload.current = CURRENT_VALUE_TO_AMPERE(hcurrent.current);
    return &hcurrent.current_can_payload;
}

primary_hv_power_converted_t * current_get_power_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcurrent.power_can_payload);

    // Save current value in A
    hcurrent.power_can_payload.current = CURRENT_VALUE_TO_AMPERE(hcurrent.current);
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
