/*!
 * \file post.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Definition used for the Power-On Self Test and initialization module
 */

#ifndef POST_H
#define POST_H

#include <stdint.h>

#include "can-communication.h"
#include "display.h"
#include "led.h"
#include "mainboard-watchdogs.h"
#include "sensing.h"

/*!
 * \brief Return code for the POST module functions
 */
enum PostReturnCode : uint8_t {
    POST_RC_OK,           /*!< Every module was initialized successfully */
    POST_RC_NULL_POINTER, /*!< A NULL pointer was given to a function */
    POST_RC_ERROR,        /*!< At least one module failed to initialize */
};

/*!
 * \brief Hardware callbacks needed to initialize every module
 */
struct PostInitData {
    can_communication_send_callback send_primary;         /*!< Immediately send a frame on the primary network */
    can_communication_send_callback send_bms;             /*!< Immediately send a frame on the BMS network */
    can_communication_status_callback status_primary;     /*!< Read the transceiver state of the primary network, may be NULL */
    can_communication_status_callback status_bms;         /*!< Read the transceiver state of the BMS network, may be NULL */
    can_communication_tx_ready_callback tx_ready_primary; /*!< Tell if the primary network can take a frame, may be NULL */
    can_communication_tx_ready_callback tx_ready_bms;     /*!< Tell if the BMS network can take a frame, may be NULL */
    can_communication_critical_section_callback cs_enter; /*!< Enter the CAN critical section */
    can_communication_critical_section_callback cs_exit;  /*!< Exit the CAN critical section */
    led_set_state_callback led_set;                       /*!< Set the state of the LED */
    led_toggle_state_callback led_toggle;                 /*!< Toggle the state of the LED */
    display_segment_set_state_callback display_set;       /*!< Set the state of a display segment */
    display_segment_toggle_state_callback display_toggle; /*!< Toggle the state of a display segment */
    mainboard_watchdogs_tick_callback get_tick;           /*!< Get the current monotonic time in ms */
    struct SensingInitData sensing;                       /*!< The hardware callbacks of the sensing module */
};

#endif // POST_H
