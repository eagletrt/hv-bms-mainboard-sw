/*!
 * \file can-bridge-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN bridge that duplicates and forwards the received frames to the
 *        other network when enabled
 */

#include "can-bridge-api.h"

#include <stddef.h>
#include <string.h>

#include "can-bms.h"
#include "can-communication-router-api.h"
#include "can-primary.h"
#include "eagletrt.h"
#include "mainboard-logger-api.h"

EAGLETRT_STATIC struct CanBridgeHandler can_bridge_handler;

// TODO: change names in canlib of the messages in the BMS

/*! \brief Routes of the frames received on the primary network, a frame without a route goes to the router */
EAGLETRT_STATIC const struct CanBridgeRoute from_primary_routes[] = {
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD1 },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD2 },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD3 },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD4 },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD5 },
    { CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6FLASHINGRX, CAN_BRIDGE_DESTINATION_BMS, CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHCELLBOARD6 },
};

/*! \brief Routes of the frames received on the BMS network, a frame without a route goes to the router */
EAGLETRT_STATIC const struct CanBridgeRoute from_bms_routes[] = {
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1FLASHINGTX },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2FLASHINGTX },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3FLASHINGTX },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4FLASHINGTX },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5FLASHINGTX },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FLASH, CAN_BRIDGE_DESTINATION_PRIMARY, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6FLASHINGTX },
};

/*!
 * \brief Find the route of a frame in a routing table
 *
 * \param[in] routes The routing table
 * \param[in] count The number of routes in the table
 * \param[in] id The ID of the received frame
 *
 * \return The route of the frame, \c NULL if it has no route
 */
EAGLETRT_STATIC const struct CanBridgeRoute *prv_can_bridge_find_route(const struct CanBridgeRoute *routes, size_t count, uint32_t id) {
    for (size_t i = 0U; i < count; ++i) {
        if (routes[i].id == id) {
            return &routes[i];
        }
    }
    return NULL;
}

/*!
 * \brief Send a frame to its destination
 *
 * \details A retransmitted frame is a copy of the received one with the ID of the route,
 * the received frame is left unchanged
 *
 * \param[in] route The route of the frame, \c NULL sends the frame to the router
 * \param[in] frame The received frame
 * \param[in] router The router callback of the network the frame was received on
 *
 * \return The result of the retransmission or of the router
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_can_bridge_dispatch(
    const struct CanBridgeRoute *route,
    struct CanCommunicationFrame *frame,
    can_communication_receive_callback router) {
    if (route == NULL) {
        return router(frame);
    }

    struct CanCommunicationFrame forwarded = *frame;
    forwarded.id = route->forward_id;

    switch (route->destination) {
        case CAN_BRIDGE_DESTINATION_PRIMARY:
            return can_bridge_handler.send_primary(&forwarded);
        case CAN_BRIDGE_DESTINATION_BMS:
            return can_bridge_handler.send_bms(&forwarded);
        case CAN_BRIDGE_DESTINATION_ROUTER:
        default:
            return router(frame);
    }
}

enum CanCommunicationReturnCode can_bridge_api_init(const can_communication_send_callback send_primary, const can_communication_send_callback send_bms) {
    if (send_primary == NULL || send_bms == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    memset(&can_bridge_handler, 0, sizeof(can_bridge_handler));
    can_bridge_handler.send_primary = send_primary;
    can_bridge_handler.send_bms = send_bms;
    return CAN_COMMUNICATION_RC_OK;
}

void can_bridge_api_enable_bridge(void) {
    if (!can_bridge_handler.enabled) {
        MAINBOARD_LOGGER_INFO("CAN bridge enabled");
    }
    can_bridge_handler.enabled = true;
}

void can_bridge_api_disable_bridge(void) {
    if (can_bridge_handler.enabled) {
        MAINBOARD_LOGGER_INFO("CAN bridge disabled");
    }
    can_bridge_handler.enabled = false;
}

enum CanCommunicationReturnCode can_bridge_api_receive_primary(struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    const struct CanBridgeRoute *route = can_bridge_handler.enabled
                                             ? prv_can_bridge_find_route(from_primary_routes, sizeof(from_primary_routes) / sizeof(from_primary_routes[0]), frame->id)
                                             : NULL;
    return prv_can_bridge_dispatch(route, frame, can_communication_router_api_receive_primary);
}

enum CanCommunicationReturnCode can_bridge_api_receive_bms(struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    const struct CanBridgeRoute *route = can_bridge_handler.enabled
                                             ? prv_can_bridge_find_route(from_bms_routes, sizeof(from_bms_routes) / sizeof(from_bms_routes[0]), frame->id)
                                             : NULL;
    return prv_can_bridge_dispatch(route, frame, can_communication_router_api_receive_bms);
}
