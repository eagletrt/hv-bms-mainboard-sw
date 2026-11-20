/*!
 * \file can-bridge.h
 * \date 2026-11-20
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the CAN bridge
 */

#ifndef CAN_BRIDGE_H
#define CAN_BRIDGE_H

#include <stdbool.h>
#include <stdint.h>

#include "can-communication.h"

/*!
 * \brief Where a received frame goes
 *
 * \details The router is 0 so that a route which does not set its destination sends the frame to the router
 */
enum CanBridgeDestination : uint8_t {
    CAN_BRIDGE_DESTINATION_ROUTER = 0, /*!< Handled by the mainboard through the router (default) */
    CAN_BRIDGE_DESTINATION_PRIMARY,    /*!< Retransmitted on the primary network with the route forward ID */
    CAN_BRIDGE_DESTINATION_BMS,        /*!< Retransmitted on the BMS network with the route forward ID */
};

/*!
 * \brief Association between a received frame ID, its destination and the ID it is retransmitted with
 *
 * \details The same message can have a different ID on each network, so a retransmitted frame
 * keeps its payload but is sent with \c forward_id
 */
struct CanBridgeRoute {
    uint32_t id;                           /*!< The ID of the received frame */
    enum CanBridgeDestination destination; /*!< Where the frame goes */
    uint32_t forward_id;                   /*!< The ID the frame is retransmitted with, unused when the destination is the router */
};

/*!
 * \brief CAN bridge handler structure
 */
struct CanBridgeHandler {
    bool enabled;                                 /*!< When false every frame goes to the router and the routes are ignored */
    can_communication_send_callback send_primary; /*!< Immediate send on the primary network, bypasses the TX queue */
    can_communication_send_callback send_bms;     /*!< Immediate send on the BMS network, bypasses the TX queue */
};

#endif // CAN_BRIDGE_H
