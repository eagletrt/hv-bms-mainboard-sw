/*!
 * \file led-api.h
 * \date 2026-05-15
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used to manage the on-board LEDs
 */

#ifndef LED_API_H
#define LED_API_H

#include "led.h"

#ifdef CONF_LED_MODULE_ENABLE

/*!
 * \brief Initialize the internal LED handler
 *
 * \details The LED handler is disabled by default after initialization
 *
 * \param set The callback used to set the status of the LED
 * \param toggle The callback used to toggle the status of the LED
 *
 * \retval LED_RC_NULL_POINTER if the set or toggle callbacks are NULL
 * \retval LED_RC_OK otherwise
 */
enum LedReturnCode led_api_init(led_set_state_callback set, led_toggle_state_callback toggle);

/*!
 * \brief Set the status of the selected LED
 *
 * \param id The identifier of the LED
 * \param status The status to set
 *
 * \retval LED_RC_INVALID_ID if the given identifier is not valid
 * \retval LED_RC_INVALID_STATUS if the given status is not valid
 * \retval LED_RC_OK otherwise
 */
enum LedReturnCode led_api_set_status(enum LedId led_id, enum LedStatus status);

/*!
 * \brief Toggle the status of the selected LED
 *
 * \param id The identifier of the LED
 *
 * \retval LED_RC_INVALID_ID if the given identifier is not valid
 * \retval LED_RC_OK otherwise
 */
enum LedReturnCode led_api_toggle_status(enum LedId led_id);

#else // CONF_LED_MODULE_ENABLE

#define led_api_init(set, toggle) (LED_RC_OK)
#define led_api_set_status(id, status) (LED_RC_OK)
#define led_api_toggle_status(id) (LED_RC_OK)

#endif // CONF_LED_MODULE_ENABLE

#endif // LED_API_H
