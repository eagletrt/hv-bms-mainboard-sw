/*!
 * \file can-communication-info-api.h
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Information about the CAN communication
 */

#ifndef CAN_COMMUNICATION_INFO_API_H
#define CAN_COMMUNICATION_INFO_API_H

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

#endif // CAN_COMMUNICATION_INFO_API_H
