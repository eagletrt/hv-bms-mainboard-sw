/*!
 * \file can-communication-api.c
 * \date 2024-06-21
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Implementation of the CAN-bus communication module.
 */

#include "can-communication-router-api.h"

#include "mainboard-conf.h"
#include "can-communication.h"
#include "can-bms-api.h"
#include "can-bms.h"
#include "can-primary-api.h"
#include "can-primary.h"

#include "fsm.h"
#include "pcu-api.h"
#include "bal-api.h"

#ifdef CONF_CAN_COMM_MODULE_ENABLE

enum CanCommunicationReturnCode can_communication_router_api_receive_bms(struct CanCommunicationFrame *frame) {
    if (frame == nullptr) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (!can_bms_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    union CanBmsMessages message = { 0 };
    if (can_bms_api_deserialize_from_id(frame->id, frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    switch (frame->id) {
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FSM:
            fsm_cellboard1_state_handle(message.tsaccellboard1fsm.status);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FSM:
            fsm_cellboard2_state_handle(message.tsaccellboard2fsm.status);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FSM:
            fsm_cellboard3_state_handle(message.tsaccellboard3fsm.status);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FSM:
            fsm_cellboard4_state_handle(message.tsaccellboard4fsm.status);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FSM:
            fsm_cellboard5_state_handle(message.tsaccellboard5fsm.status);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FSM:
            fsm_cellboard6_state_handle(message.tsaccellboard6fsm.status);
            break;

        default:
            break;
    }

    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_communication_router_api_receive_primary(struct CanCommunicationFrame *frame) {
    if (frame == nullptr) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (!can_primary_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    union CanPrimaryMessages message = { 0 };
    if (can_primary_api_deserialize_from_id(frame->id, frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    switch (frame->id) {
        case CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYBALANCINGSET:
            bal_api_set_balancing_state_handle(
                message.raspberrybalancingset.start,
                message.raspberrybalancingset.threshold);
            break;
        case CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET:
            pcu_api_bms_set_handle(message.bmsset.status);
            break;
        default:
            break;
    }

    return CAN_COMMUNICATION_RC_OK;
}

#endif // CONF_CAN_COMM_MODULE_ENABLE
