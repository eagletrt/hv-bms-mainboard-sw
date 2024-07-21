/**
 * @file led.c
 * @date 2024-05-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions used to manage the on-board LEDs
 */

#include "led.h"

#include <string.h>

#include "identity.h"

#ifdef CONF_LED_MODULE_ENABLE

/**
 * @brief LED handler structure
 *
 * @param set A pointer to the function callback used to set the led state
 * @param toggle A pointer to the function callback used to toggle the led state
 */
_STATIC struct LedHandler {
    led_set_state_callback_t set;
    led_toggle_state_callback_t toggle;
} hled;

LedReturnCode led_init(led_set_state_callback_t set, led_toggle_state_callback_t toggle) {
    if (set == NULL || toggle == NULL)
        return LED_NULL_POINTER;
    memset(&hled, 0U, sizeof(hled));
    hled.set = set;
    hled.toggle = toggle;
    return LED_OK;
}

LedReturnCode led_set_status(LedId id, LedStatus status) {
    if (id >= LED_ID_COUNT)
        return LED_INVALID_ID;
    if (status >= LED_STATUS_COUNT)
        return LED_INVALID_STATUS;
    hled.set(id, status);
    return LED_OK;
}

LedReturnCode led_toggle_status(LedId id) {
    if (id >= LED_ID_COUNT)
        return LED_INVALID_ID;
    hled.toggle(id);
    return LED_OK;
}

#ifdef CONF_LED_STRINGS_ENABLE

_STATIC char * led_module_name = "led";

_STATIC char * led_return_code_name[] = {
    [LED_OK] = "ok",
    [LED_NULL_POINTER] = "null pointer",
    [LED_INVALID_ID] = "invalid id",
    [LED_INVALID_STATUS] = "invalid status"
};

_STATIC char * led_return_code_description[] = {
    [LED_OK] = "executed succesfully",
    [LED_NULL_POINTER] = "attempt to dereference a NULL pointer",
    [LED_INVALID_ID] = "the led identifier does not correspond to any existing led",
    [LED_INVALID_STATUS] = "the status is not a valid status"
};

#endif // CONF_LED_STRINGS_ENABLE

#endif // CONF_LED_MODULE_ENABLE
