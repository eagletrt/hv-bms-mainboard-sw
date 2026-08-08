/*!
 * \file post.h
 * \date 2024-04-16
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Definition used for the Power-On Self Test and initialization module
 */

#ifndef POST_H
#define POST_H

#include "can-communication.h"
#include "mainboard-def.h"

#include "led.h"
#include "imd.h"
#include "pcu.h"
#include "feedback.h"
#include "display.h"

/*!
 * \brief Return code for the post module functions
 */
enum PostReturnCode {
    POST_RC_OK,            /*!< Executed succesfully */
    POST_RC_UNINITIALIZED, /*!< Module initialization error */
    POST_RC_SETUP_ERROR,   /*!< Module setup error */
    POST_RC_NULL_POINTER   /*!< NULL pointer detected */
};

/*!
 * \brief Structure definition for the initial data that are needed by the POST module
 */
struct PostInitData {
    struct CanCommunicationNetworkConfig can_networks[CAN_COMMUNICATION_NETWORK_COUNT]; /*!< Explicit configuration blocks for the system's physical CAN networks. */

    system_reset_callback_t system_reset;                                /*!< MCU reset callback */
    interrupt_critical_section_enter_t cs_enter;                         /*!< Critical section entry point to avoid race conditions */
    interrupt_critical_section_exit_t cs_exit;                           /*!< Critical section exit point to avoid race conditions */
    led_set_state_callback led_set;                                      /*!< LED set status callback */
    led_toggle_state_callback led_toggle;                                /*!< LED toggle status callback */
    imd_pwm_start_callback imd_start;                                    /*!< IMD start measurement callback */
    pcu_set_state_callback pcu_set;                                      /*!< Pack Control Unit set status callback */
    pcu_toggle_state_callback pcu_toggle;                                /*!< Pack Control Unit toggle status callback */
    feedback_start_analog_conversion_callback feedback_start_conversion; /*!< Start feedback conversion callback */
    feedback_read_digital_all_callback feedback_read_all;                /*!< Read all feedbacks callback */
    display_segment_set_state_callback display_set;                      /*!< 7-segment display set segment status callback */
    display_segment_toggle_state_callback display_toggle;                /*!< 7-segment display toggle segment status callback */
    spi_send_callback_t spi_send;                                        /*!< SPI send message callback */
    spi_send_receive_callback_t spi_send_receive;                        /*!< SPI send and receive message callback */

    // INFO: Due to a library bug those timer callbacks are not used
    // error_update_timer_callback_t error_update_timer;                 /*!< Error timer update callback */
    // error_stop_timer_callback_t error_stop_timer;                     /*!< Error timer stop callback */
};

#endif // POST_H
