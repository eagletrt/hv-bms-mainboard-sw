/*!
 * \file can-bridge-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN bridge that duplicates and forwards the received frames to the
 *        other network when enabled
 *
 * \details The bridge functions are the receive callbacks of the CAN communication module:
 * each received frame is looked up in the routing table of its network and is either
 * retransmitted on a network or handed to the router. Frames without a route, and every
 * frame while the bridge is disabled, go to the router.
 */

#ifndef CAN_BRIDGE_API_H
#define CAN_BRIDGE_API_H

#include "can-bridge.h"
#include "can-communication.h"

/*!
 * \brief Initialize the CAN bridge, disabled
 *
 * \details The retransmitted frames are sent immediately through the given callbacks,
 * without going through the TX queues of the CAN communication module
 *
 * \param[in] send_primary The callback used to immediately send a frame on the primary network
 * \param[in] send_bms The callback used to immediately send a frame on the BMS network
 *
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p send_primary or \p send_bms is \c NULL
 * \retval CAN_COMMUNICATION_RC_OK otherwise
 */
enum CanCommunicationReturnCode can_bridge_api_init(can_communication_send_callback send_primary, can_communication_send_callback send_bms);

/*!
 * \brief Enable the bridge, the routing tables are applied to the received frames
 */
void can_bridge_api_enable_bridge(void);

/*!
 * \brief Disable the bridge, every received frame goes to the router
 */
void can_bridge_api_disable_bridge(void);

/*!
 * \brief Tell if the bridge forwards the frames that have a route
 *
 * \return true if the bridge is enabled, false otherwise
 */
bool can_bridge_api_is_enabled(void);

/*!
 * \brief Receive callback of the primary network
 *
 * \param[in] frame The received frame
 *
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is \c NULL
 * \return The result of the retransmission or of the router otherwise
 */
enum CanCommunicationReturnCode can_bridge_api_receive_primary(struct CanCommunicationFrame *frame);

/*!
 * \brief Receive callback of the BMS network
 *
 * \param[in] frame The received frame
 *
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is \c NULL
 * \return The result of the retransmission or of the router otherwise
 */
enum CanCommunicationReturnCode can_bridge_api_receive_bms(struct CanCommunicationFrame *frame);

#endif // CAN_BRIDGE_API_H
