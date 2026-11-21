/*!
 * \file can-communication-info-api.h
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Information about the CAN communication
 */

#ifndef CAN_COMMUNICATION_INFO_API_H
#define CAN_COMMUNICATION_INFO_API_H

#include <stdint.h>

#include "can-communication-info.h"

/*!
 * \brief Initialize the CAN communication info
 *
 * \details Initializes the cycle and leeway watchdogs of every received message
 *          and starts the timeframe switcher
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if a cycle time is not greater than the wiggle room or a watchdog fails to initialize or start
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_init(void);

/*!
 * \brief Handle the reception of a message on the primary network
 *
 * \details Stops the leeway watchdog of the message, restarts its cycle
 *          watchdog, which re-syncs the expected slots to the sender, and
 *          clears its consecutive drops
 *
 * \param[in] message_id The ID of the received message
 *
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_handle_primary_message(uint16_t message_id);

/*!
 * \brief Handle the reception of a message on the BMS network
 *
 * \details Stops the leeway watchdog of the message, restarts its cycle
 *          watchdog, which re-syncs the expected slots to the sender, and
 *          clears its consecutive drops
 *
 * \param[in] message_id The ID of the received message
 *
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_handle_bms_message(uint16_t message_id);

#endif // CAN_COMMUNICATION_INFO_API_H
