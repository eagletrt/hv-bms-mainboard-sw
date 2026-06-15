/*!
 * \file led.h
 * \date 2024-05-08
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used to manage the on-board LEDs
 */

#ifndef LED_H
#define LED_H

#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/*!
 * \brief Status of a single LED
 *
 * \warning This structure should never be used outside of this file
 */
enum LedStatus {
    LED_STATUS_OFF, /*!< The LED is turned off */
    LED_STATUS_ON,  /*!< The LED is turned on */
    LED_STATUS_COUNT
};

/*!
 * \brief Definition of the LED identifier
 */
enum LedId {
    LED_ID_1, /*!< The first LED */
    LED_ID_2, /*!< The second LED */
    LED_ID_COUNT
};

/*!
 * \brief Callback used to set the state of the LED
 *
 * \param led The LED to select
 * \param state The state to set the LED to
 */
typedef void (*led_set_state_callback)(const enum LedId led, const enum LedStatus state);

/*!
 * \brief Callback used to toggle the state of the LED
 *
 * \param led The LED to select
 */
typedef void (*led_toggle_state_callback)(const enum LedId led);

/*!
 * \brief Return code for the LED module functions
 */
enum LedReturnCode {
    LED_RC_OK,            /*!< The function executed succesfully */
    LED_RC_NULL_POINTER,  /*!< A NULL pointer was given to a function */
    LED_RC_INVALID_ID,    /*!< The given identifier does not exist */
    LED_RC_INVALID_STATUS /*!< The given status is not valid */
};

/*!
 * \brief LED handler structure
 *
 * \warning This structure should never be used outside of this file
 */
struct LedHandler {
    led_set_state_callback set;       /*!< A pointer to the function callback used to set the led state */
    led_toggle_state_callback toggle; /*!< A pointer to the function callback used to toggle the led state */
};

#endif // LED_H
