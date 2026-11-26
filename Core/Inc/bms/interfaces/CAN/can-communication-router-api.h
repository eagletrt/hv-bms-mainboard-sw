/*!
 * \file can-communication-router-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Routing of the received CAN frames to the modules that handle them
 */

#ifndef CAN_COMMUNICATION_ROUTER_API_H
#define CAN_COMMUNICATION_ROUTER_API_H

#include <stddef.h>

#include "can-communication.h"

/*!
 * \brief Deserialize a frame received on the BMS network and forward its content
 * to the module that handles it
 *
 * \details Frames with a valid ID that no module handles are ignored
 *
 * \param[in] frame The received frame
 *
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is \c NULL
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if the ID does not belong to the BMS network
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if the frame length differs from the libcan one of a handled message
 * \retval CAN_COMMUNICATION_RC_ERROR if the payload cannot be deserialized
 * \retval CAN_COMMUNICATION_RC_OK otherwise
 */
enum CanCommunicationReturnCode can_communication_router_api_receive_bms(struct CanCommunicationFrame *frame);

/*!
 * \brief Deserialize a frame received on the primary network and forward its content
 * to the module that handles it
 *
 * \details The flashing messages are forwarded as flash requests. Frames with a valid ID
 * that no module handles are ignored
 *
 * \param[in] frame The received frame
 *
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is \c NULL
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if the ID does not belong to the primary network
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if the frame length differs from the libcan one of a message whose payload is read
 * \retval CAN_COMMUNICATION_RC_ERROR if the payload cannot be deserialized or the request is rejected
 * \retval CAN_COMMUNICATION_RC_OK otherwise
 */
enum CanCommunicationReturnCode can_communication_router_api_receive_primary(struct CanCommunicationFrame *frame);

/*!
 * \brief Get the ID ranges handled on the primary network, used to configure its hardware filter
 *
 * \details They include the frames the CAN bridge forwards from the primary network,
 *          which are cellboard flashing messages
 *
 * \param[out] count The number of ranges
 *
 * \return The ranges, \c NULL with \p count 0 if \p count is \c NULL
 */
const struct CanCommunicationIdRange *can_communication_router_api_get_primary_rx_ranges(size_t *count);

#endif // CAN_COMMUNICATION_ROUTER_API_H
