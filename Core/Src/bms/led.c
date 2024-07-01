/**
 * @file led.c
 * @date 2024-05-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions used to manage the on-board LEDs
 */

#include "led.h"

#include "blinky.h"
#include "mainboard-def.h"
#include "identity.h"

#ifdef CONF_LED_MODULE_ENABLE

/**
 * @brief LED handler structure
 *
 * @param set A pointer to the function callback used to set the led state
 * @param toggle A pointer to the function callback used to toggle the led state
 * @param blinker The structure handler used to blink the LED
 * @param pattern The main blinking pattern
 * @param pattern_size The size of the pattern
 */
static struct LedHandler {
    led_set_state_callback_t set;
    led_toggle_state_callback_t toggle;
} hled;


LedReturnCode led_init(led_set_state_callback_t set, led_toggle_state_callback_t toggle) {
    if (set == NULL || toggle == NULL)
        return LED_NULL_POINTER;
    hled.set = set;
    hled.toggle = toggle;
    return LED_OK;
}

#ifdef CONF_LED_STRINGS_ENABLE

static char * led_module_name = "led";

static char * led_return_code_name[] = {
    [LED_OK] = "ok",
    [LED_NULL_POINTER] = "null pointer",
};

static char * led_return_code_description[] = {
    [LED_OK] = "executed succesfully",
    [LED_NULL_POINTER] = "attempt to dereference a NULL pointer",
};

#endif // CONF_LED_STRINGS_ENABLE

#endif // CONF_LED_MODULE_ENABLE
