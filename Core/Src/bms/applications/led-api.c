/*!
 * \file led-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used to manage the on-board LEDs
 */

#include "led-api.h"
#include "eagletrt-api.h"

#include <string.h>

EAGLETRT_STATIC struct LedHandler led_handler;

enum LedReturnCode led_api_init(const led_set_state_callback set, const led_toggle_state_callback toggle) {
    if (set == NULL || toggle == NULL) {
        return LED_RC_NULL_POINTER;
    }
    memset(&led_handler, 0U, sizeof(led_handler));
    led_handler.set = set;
    led_handler.toggle = toggle;
    return LED_RC_OK;
}

enum LedReturnCode led_api_set_status(const enum LedId led_id, const enum LedStatus status) {
    if (led_id >= LED_ID_COUNT) {
        return LED_RC_INVALID_ID;
    }
    if (status >= LED_STATUS_COUNT) {
        return LED_RC_INVALID_STATUS;
    }
    led_handler.set(led_id, status);
    return LED_RC_OK;
}

enum LedReturnCode led_api_toggle_status(const enum LedId led_id) {
    if (led_id >= LED_ID_COUNT) {
        return LED_RC_INVALID_ID;
    }
    led_handler.toggle(led_id);
    return LED_RC_OK;
}
