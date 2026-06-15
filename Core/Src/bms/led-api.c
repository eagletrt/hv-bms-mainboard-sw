/*!
 * \file led.c
 * \date 2024-05-08
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Functions used to manage the on-board LEDs
 */

#include "led-api.h"
#include "eagletrt-api.h"

#include <string.h>

#ifdef CONF_LED_MODULE_ENABLE

EAGLETRT_STATIC struct LedHandler led_handler;

enum LedReturnCode led_init(const led_set_state_callback set, const led_toggle_state_callback toggle) {
    if (set == NULL || toggle == NULL) {
        return LED_RC_NULL_POINTER;
    }
    memset(&led_handler, 0U, sizeof(led_handler));
    led_handler.set = set;
    led_handler.toggle = toggle;
    return LED_RC_OK;
}

enum LedReturnCode led_set_status(const enum LedId led_id, const enum LedStatus status) {
    if (led_id >= LED_ID_COUNT) {
        return LED_RC_INVALID_ID;
    }
    if (status >= LED_STATUS_COUNT) {
        return LED_RC_INVALID_STATUS;
    }
    led_handler.set(led_id, status);
    return LED_RC_OK;
}

enum LedReturnCode led_toggle_status(const enum LedId led_id) {
    if (led_id >= LED_ID_COUNT) {
        return LED_RC_INVALID_ID;
    }
    led_handler.toggle(led_id);
    return LED_RC_OK;
}

#ifdef CONF_LED_STRINGS_ENABLE

EAGLETRT_STATIC char *led_module_name = "led";

EAGLETRT_STATIC char *led_return_code_name[] = {
    [LED_RC_OK] = "ok",
    [LED_RC_NULL_POINTER] = "null pointer",
    [LED_RC_INVALID_ID] = "invalid id",
    [LED_RC_INVALID_STATUS] = "invalid status"
};

EAGLETRT_STATIC char *led_return_code_description[] = {
    [LED_RC_OK] = "executed succesfully",
    [LED_RC_NULL_POINTER] = "attempt to dereference a NULL pointer",
    [LED_RC_INVALID_ID] = "the led identifier does not correspond to any existing led",
    [LED_RC_INVALID_STATUS] = "the status is not a valid status"
};

#endif // CONF_LED_STRINGS_ENABLE

#endif // CONF_LED_MODULE_ENABLE
