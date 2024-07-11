/**
 * @file post.h
 * @date 2024-04-16
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Power-On Self Test function to check that every internal system and
 * peripheral is working correctly
 */

#ifndef POST_H
#define POST_H

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "can-comm.h"
#include "led.h"
#include "imd.h"
#include "pcu.h"
#include "feedback.h"

/**
 * @brief Return code for the post module functions
 *
 * @details
 *     - POST_OK the function executed succesfully
 *     - POST_INVALID_CELLBOARD_ID invalid cellboard identifier
 *     - POST_NULL_POINTER a NULL pointer was given to a function
 *     - POST_WATCHDOG_INVALID_MESSAGE the watchdog are using an invalid message
 */
typedef enum {
    POST_OK,
    POST_INVALID_CELLBOARD_ID,
    POST_NULL_POINTER,
    POST_WATCHDOG_INVALID_MESSAGE
} PostReturnCode;

/**
 * @brief Structure definition for the initial data that are needed by the POST module
 *
 * @param system_reset A pointer to a function that resets the microcontroller
 * @param can_send A pointer to a function that can send data via the CAN bus
 * @param led_set A pointer to a function that sets the state of a LED
 * @param led_toggle A pointer to a function that toggles the state of a LED
 * @param imd_start A pointer to a function that should start the IMD PWM measurements
 * @param pcu_set A pointer to a function that sets the state of a PCU pin
 * @param pcu_toggle A pointer to a function that toggles the state of a PCU pin
 */
typedef struct {
    system_reset_callback_t system_reset;
    can_comm_transmit_callback_t can_send;
    led_set_state_callback_t led_set;
    led_toggle_state_callback_t led_toggle;
    imd_pwm_start_callback_t imd_start;
    pcu_set_state_callback_t pcu_set;
    pcu_toggle_state_callback_t pcu_toggle;
    feedback_read_digital_all_callback_t feedback_read_all;
    feedback_start_analog_conversion_callback_t feedback_start_conversion;
} PostInitData;

#ifdef CONF_POST_MODULE_ENABLE

/**
 * @brief Run the power-on self test
 *
 * @details This function check if all the systems and peripherals work
 * as execpected, otherwise it returns an error code
 *
 * @param data The data needed by the POST module for initialization
 *
 * @return PostReturnCode
 *     - POST_INVALID_CELLBOARD_ID if the given cellboard id is not valid
 *     - POST_NULL_POINTER if the given function pointers are NULL
 *     - POST_WATCHDOG_INVALID_MESSAGE the watchdog are using an invalid message
 *     - POST_OK otherwise
 */
PostReturnCode post_run(PostInitData data);

#else  // CONF_POST_MODULE_ENABLE

#define post_run(data) (POST_OK)

#endif // CONF_POST_MODULE_ENABLE

#endif // POST_H
