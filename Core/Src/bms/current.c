/**
 * @file current.c
 * @date 2024-08-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Current measurement and control
 */

#include "current.h"

#include <string.h>

#include "timebase.h"
#include "error.h"

#ifdef CONF_CURRENT_MODULE_ENABLE

/**
 * @brief Current handler structure
 *
 * @param current The raw current value
 * @param can_payload The canlib payload used to send the current value via CAN
 */
_STATIC struct {
    raw_current_t current;    

    primary_hv_current_converted_t can_payload;
} hcurrent;


/**
 * @brief Check if the current values are in range otherwise set an error
 *
 * @param value The raw current value
 */
_STATIC_INLINE void _current_check_value(raw_current_t value) {
    ERROR_TOGGLE_IF(
        value <= CURRENT_MIN_VALUE || value >= CURRENT_MAX_VALUE,
        ERROR_GROUP_OVER_CURRENT,
        0U,
        timebase_get_time()
    );
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

primary_hv_current_converted_t * current_get_canlib_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hcurrent.can_payload);

    // Save current value in A
    hcurrent.can_payload.current = CURRENT_VALUE_TO_MILLIAMPERE(hcurrent.current) * 0.001;
    return &hcurrent.can_payload;
}

raw_current_t current_get_current(void) {
    return hcurrent.current;
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
