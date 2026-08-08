/*!
 * \file can-communication-api.c
 * \date 2024-06-21
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Implementation of the CAN-bus communication module.
 */

#include "can-communication-router-api.h"

#include "eagletrt-api.h"
#include "mainboard-conf.h"
#include "can-communication.h"
#include "can-bms-api.h"
#include "can-bms.h"
#include "can-primary-api.h"
#include "can-primary.h"

#include "fsm.h"
#include "mainboard-def.h"
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
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard1balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_0, discharging);
            break;
        }
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard2balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_1, discharging);
            break;
        }
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard3balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_2, discharging);
            break;
        }
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard4balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_3, discharging);
            break;
        }
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard5balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_4, discharging);
            break;
        }
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING: {
            bit_flag32_t discharging = EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell1, 0);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell2, 1);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell3, 2);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell4, 3);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell5, 4);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell6, 5);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell7, 6);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell8, 7);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell9, 8);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell10, 9);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell11, 10);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell12, 11);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell13, 12);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell14, 13);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell15, 14);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell16, 15);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell17, 16);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell18, 17);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell19, 18);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell20, 19);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell21, 20);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell22, 21);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell23, 22);
            discharging |= EAGLETRT_API_BIT_SET(message.tsaccellboard6balancing.cell24, 23);
            bal_api_cellboard_balancing_handle(CELLBOARD_ID_5, discharging);
            break;
        }
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
