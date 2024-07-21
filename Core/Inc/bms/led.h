/**
 * @file led.h
 * @date 2024-05-08
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions used to manage the on-board LEDs
 */

#ifndef LED_H
#define LED_H

#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/**
 * @brief Status of a single LED
 *
 * @details
 *     - LED_OFF the led is turned off
 *     - LED_ON the led is turned on
 */
typedef enum {
    LED_STATUS_OFF,
    LED_STATUS_ON,
    LED_STATUS_COUNT
} LedStatus;

/**
 * @brief Definition of the LED identifier
 *
 * @details
 *     - LED_ID_1 the first LED
 *     - LED_ID_2 the second LED
 */
typedef enum {
    LED_ID_1,
    LED_ID_2,
    LED_ID_COUNT
} LedId;

/**
 * @brief Callback used to set the state of the LED
 *
 * @param led The LED to select
 * @param state The state to set the LED to
 */
typedef void (* led_set_state_callback_t)(LedId led, LedStatus state);

/**
 * @brief Callback used to toggle the state of the LED
 *
 * @param led The LED to select
 */
typedef void (* led_toggle_state_callback_t)(LedId led);

/**
 * @brief Return code for the LED module functions
 *
 * @details
 *     - LED_OK the function executed succesfully
 *     - LED_NULL_POINTER a NULL pointer was given to a function
 *     - LED_INVALID_ID the given identifier does not exists
 *     - LED_INVALID_STATUS the given status is not valid
 */
typedef enum {
    LED_OK,
    LED_NULL_POINTER,
    LED_INVALID_ID,
    LED_INVALID_STATUS
} LedReturnCode;

#ifdef CONF_LED_MODULE_ENABLE 

/**
 * @brief Initialize the internal LED handler
 *
 * @details The LED handler is disabled by default after initialization
 *
 * @param set The callback used to set the status of the LED
 * @param toggle The callback used to toggle the status of the LED
 *
 * @return LedReturnCode
 *     - LED_NULL_POINTER if the set or toggle callbacks are NULL
 *     - LED_OK otherwise
 */
LedReturnCode led_init(led_set_state_callback_t set, led_toggle_state_callback_t toggle);

/**
 * @brief Set the status of the selected LED
 *
 * @param id The identifier of the LED
 * @param status The status to set
 *
 * @return LedReturnCode
 *     - LED_INVALID_ID if the given identifier is not valid
 *     - LED_INVALID_STATUS if the given status is not valid
 *     - LED_OK otherwise
 */
LedReturnCode led_set_status(LedId id, LedStatus status);

/**
 * @brief Toggle the status of the selected LED
 *
 * @param id The identifier of the LED
 *
 * @return LedReturnCode
 *     - LED_INVALID_ID if the given identifier is not valid
 *     - LED_OK otherwise
 */
LedReturnCode led_toggle_status(LedId id);

#else  // CONF_LED_MODULE_ENABLE 

#define led_init(set, toggle) (LED_OK)
#define led_set_status(id, status) (LED_OK)
#define led_toggle_status(id) (LED_OK)

#endif // CONF_LED_MODULE_ENABLE 
 
#endif // LED_H
