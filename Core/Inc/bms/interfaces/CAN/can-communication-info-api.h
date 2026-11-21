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
 * \details Initializes the cycle and leeway watchdogs of every monitored message
 *          on every network and starts the timeframe switcher
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if a cycle time is not greater than the wiggle room or a watchdog fails to initialize or start
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_init(void);

/*!
 * \brief Handle the reception of a message
 *
 * \details Stops the leeway watchdog of the message, restarts its cycle
 *          watchdog, which re-syncs the expected slots to the sender, and
 *          clears its consecutive drops
 *
 * \param[in] network The network the message was received on
 * \param[in] message_id The ID of the received message
 *
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored on the network
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_handle_message(enum CanNetwork network, can_id message_id);

/*!
 * \brief Get the consecutive drops of a message
 *
 * \details Reset to 0 every time the message is received, saturates at UINT8_MAX
 *
 * \param[in] network The network of the message
 * \param[in] message_id The ID of the message
 * \param[out] drops The number of consecutive drops
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if drops is NULL
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored on the network
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_get_consecutive_drops(enum CanNetwork network, can_id message_id, uint8_t *drops);

/*!
 * \brief Get the cumulative drops of a message since init
 *
 * \details Saturates at UINT16_MAX
 *
 * \param[in] network The network of the message
 * \param[in] message_id The ID of the message
 * \param[out] drops The number of cumulative drops
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if drops is NULL
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored on the network
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_get_cumulative_drops(enum CanNetwork network, can_id message_id, uint16_t *drops);

/*!
 * \brief Get the drops of a message in the last completed timeframe
 *
 * \details The timeframe lasts CAN_TIMEFRAME ms, the one currently being
 *          written is not returned since it is still incomplete.
 *          Saturates at UINT8_MAX
 *
 * \param[in] network The network of the message
 * \param[in] message_id The ID of the message
 * \param[out] drops The number of drops in the last completed timeframe
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if drops is NULL
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_INDEX if the message is not monitored on the network
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_get_timeframe_drops(enum CanNetwork network, can_id message_id, uint8_t *drops);

/*!
 * \brief Get the drops of all the monitored messages of a network in the last completed timeframe
 *
 * \param[in] network The network
 * \param[out] drops The sum of the timeframe drops of every monitored message of the network
 *
 * \retval CAN_COMMUNICATION_INFO_RC_ERROR if drops is NULL
 * \retval CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK if the network is out of range
 * \retval CAN_COMMUNICATION_INFO_RC_OK otherwise
 */
enum CanCommunicationInfoReturnCode can_communication_info_get_total_timeframe_drops(enum CanNetwork network, uint32_t *drops);

#endif // CAN_COMMUNICATION_INFO_API_H
