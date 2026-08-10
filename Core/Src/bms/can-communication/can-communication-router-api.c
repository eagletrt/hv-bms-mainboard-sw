/*!
 * \file can-communication-router-api.c
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
#include "volt-api.h"
#include "temp-api.h"
#include "current-api.h"
#include "error-api.h"

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
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE:
            switch (message.tsaccellboard1voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard1voltageGroupMux0 *payload = &message.tsaccellboard1voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_0, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_0, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_0, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_0, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_0, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_0, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard1voltageGroupMux1 *payload = &message.tsaccellboard1voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_0, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_0, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_0, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_0, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_0, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_0, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard1voltageGroupMux2 *payload = &message.tsaccellboard1voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_0, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_0, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_0, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_0, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_0, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_0, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard1voltageGroupMux3 *payload = &message.tsaccellboard1voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_0, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_0, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_0, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_0, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_0, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_0, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE:
            switch (message.tsaccellboard2voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard2voltageGroupMux0 *payload = &message.tsaccellboard2voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_1, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_1, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_1, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_1, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_1, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_1, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard2voltageGroupMux1 *payload = &message.tsaccellboard2voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_1, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_1, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_1, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_1, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_1, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_1, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard2voltageGroupMux2 *payload = &message.tsaccellboard2voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_1, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_1, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_1, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_1, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_1, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_1, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard2voltageGroupMux3 *payload = &message.tsaccellboard2voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_1, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_1, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_1, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_1, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_1, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_1, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE:
            switch (message.tsaccellboard3voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard3voltageGroupMux0 *payload = &message.tsaccellboard3voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_2, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_2, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_2, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_2, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_2, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_2, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard3voltageGroupMux1 *payload = &message.tsaccellboard3voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_2, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_2, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_2, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_2, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_2, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_2, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard3voltageGroupMux2 *payload = &message.tsaccellboard3voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_2, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_2, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_2, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_2, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_2, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_2, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard3voltageGroupMux3 *payload = &message.tsaccellboard3voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_2, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_2, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_2, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_2, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_2, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_2, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE:
            switch (message.tsaccellboard4voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard4voltageGroupMux0 *payload = &message.tsaccellboard4voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_3, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_3, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_3, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_3, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_3, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_3, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard4voltageGroupMux1 *payload = &message.tsaccellboard4voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_3, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_3, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_3, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_3, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_3, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_3, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard4voltageGroupMux2 *payload = &message.tsaccellboard4voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_3, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_3, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_3, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_3, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_3, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_3, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard4voltageGroupMux3 *payload = &message.tsaccellboard4voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_3, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_3, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_3, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_3, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_3, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_3, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE:
            switch (message.tsaccellboard5voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard5voltageGroupMux0 *payload = &message.tsaccellboard5voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_4, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_4, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_4, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_4, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_4, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_4, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard5voltageGroupMux1 *payload = &message.tsaccellboard5voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_4, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_4, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_4, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_4, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_4, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_4, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard5voltageGroupMux2 *payload = &message.tsaccellboard5voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_4, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_4, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_4, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_4, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_4, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_4, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard5voltageGroupMux3 *payload = &message.tsaccellboard5voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_4, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_4, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_4, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_4, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_4, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_4, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE:
            switch (message.tsaccellboard6voltage.group) {
                case 0: {
                    struct CanBmsTsaccellboard6voltageGroupMux0 *payload = &message.tsaccellboard6voltage.group_payload.mux_0;
                    volt_api_set_value(CELLBOARD_ID_5, 0, payload->cell1);
                    volt_api_set_value(CELLBOARD_ID_5, 1, payload->cell2);
                    volt_api_set_value(CELLBOARD_ID_5, 2, payload->cell3);
                    volt_api_set_value(CELLBOARD_ID_5, 3, payload->cell4);
                    volt_api_set_value(CELLBOARD_ID_5, 4, payload->cell5);
                    volt_api_set_value(CELLBOARD_ID_5, 5, payload->cell6);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard6voltageGroupMux1 *payload = &message.tsaccellboard6voltage.group_payload.mux_1;
                    volt_api_set_value(CELLBOARD_ID_5, 6, payload->cell7);
                    volt_api_set_value(CELLBOARD_ID_5, 7, payload->cell8);
                    volt_api_set_value(CELLBOARD_ID_5, 8, payload->cell9);
                    volt_api_set_value(CELLBOARD_ID_5, 9, payload->cell10);
                    volt_api_set_value(CELLBOARD_ID_5, 10, payload->cell11);
                    volt_api_set_value(CELLBOARD_ID_5, 11, payload->cell12);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard6voltageGroupMux2 *payload = &message.tsaccellboard6voltage.group_payload.mux_2;
                    volt_api_set_value(CELLBOARD_ID_5, 12, payload->cell13);
                    volt_api_set_value(CELLBOARD_ID_5, 13, payload->cell14);
                    volt_api_set_value(CELLBOARD_ID_5, 14, payload->cell15);
                    volt_api_set_value(CELLBOARD_ID_5, 15, payload->cell16);
                    volt_api_set_value(CELLBOARD_ID_5, 16, payload->cell17);
                    volt_api_set_value(CELLBOARD_ID_5, 17, payload->cell18);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard6voltageGroupMux3 *payload = &message.tsaccellboard6voltage.group_payload.mux_3;
                    volt_api_set_value(CELLBOARD_ID_5, 18, payload->cell19);
                    volt_api_set_value(CELLBOARD_ID_5, 19, payload->cell20);
                    volt_api_set_value(CELLBOARD_ID_5, 20, payload->cell21);
                    volt_api_set_value(CELLBOARD_ID_5, 21, payload->cell22);
                    volt_api_set_value(CELLBOARD_ID_5, 22, payload->cell23);
                    volt_api_set_value(CELLBOARD_ID_5, 23, payload->cell24);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_0,
                message.tsaccellboard1temperatureinfo.min,
                message.tsaccellboard1temperatureinfo.max,
                message.tsaccellboard1temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_1,
                message.tsaccellboard2temperatureinfo.min,
                message.tsaccellboard2temperatureinfo.max,
                message.tsaccellboard2temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_2,
                message.tsaccellboard3temperatureinfo.min,
                message.tsaccellboard3temperatureinfo.max,
                message.tsaccellboard3temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_3,
                message.tsaccellboard4temperatureinfo.min,
                message.tsaccellboard4temperatureinfo.max,
                message.tsaccellboard4temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_4,
                message.tsaccellboard5temperatureinfo.min,
                message.tsaccellboard5temperatureinfo.max,
                message.tsaccellboard5temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATUREINFO:
            temp_api_cellboard_temperature_info_handle(
                CELLBOARD_ID_5,
                message.tsaccellboard6temperatureinfo.min,
                message.tsaccellboard6temperatureinfo.max,
                message.tsaccellboard6temperatureinfo.average);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE:
            switch (message.tsaccellboard1temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard1temperatureGroupMux0 *payload = &message.tsaccellboard1temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_0, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_0, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_0, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_0, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_0, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard1temperatureGroupMux1 *payload = &message.tsaccellboard1temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_0, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_0, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_0, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_0, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_0, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard1temperatureGroupMux2 *payload = &message.tsaccellboard1temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_0, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_0, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_0, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_0, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_0, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard1temperatureGroupMux3 *payload = &message.tsaccellboard1temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_0, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_0, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_0, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_0, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_0, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard1temperatureGroupMux4 *payload = &message.tsaccellboard1temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_0, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_0, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_0, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_0, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_0, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard1temperatureGroupMux5 *payload = &message.tsaccellboard1temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_0, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_0, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_0, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_0, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_0, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard1temperatureGroupMux6 *payload = &message.tsaccellboard1temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_0, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_0, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_0, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_0, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_0, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard1temperatureGroupMux7 *payload = &message.tsaccellboard1temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_0, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_0, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_0, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_0, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_0, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard1temperatureGroupMux8 *payload = &message.tsaccellboard1temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_0, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_0, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_0, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_0, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_0, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard1temperatureGroupMux9 *payload = &message.tsaccellboard1temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_0, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_0, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_0, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE:
            switch (message.tsaccellboard2temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard2temperatureGroupMux0 *payload = &message.tsaccellboard2temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_1, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_1, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_1, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_1, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_1, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard2temperatureGroupMux1 *payload = &message.tsaccellboard2temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_1, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_1, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_1, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_1, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_1, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard2temperatureGroupMux2 *payload = &message.tsaccellboard2temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_1, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_1, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_1, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_1, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_1, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard2temperatureGroupMux3 *payload = &message.tsaccellboard2temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_1, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_1, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_1, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_1, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_1, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard2temperatureGroupMux4 *payload = &message.tsaccellboard2temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_1, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_1, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_1, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_1, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_1, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard2temperatureGroupMux5 *payload = &message.tsaccellboard2temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_1, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_1, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_1, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_1, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_1, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard2temperatureGroupMux6 *payload = &message.tsaccellboard2temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_1, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_1, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_1, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_1, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_1, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard2temperatureGroupMux7 *payload = &message.tsaccellboard2temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_1, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_1, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_1, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_1, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_1, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard2temperatureGroupMux8 *payload = &message.tsaccellboard2temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_1, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_1, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_1, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_1, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_1, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard2temperatureGroupMux9 *payload = &message.tsaccellboard2temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_1, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_1, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_1, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE:
            switch (message.tsaccellboard3temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard3temperatureGroupMux0 *payload = &message.tsaccellboard3temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_2, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_2, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_2, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_2, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_2, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard3temperatureGroupMux1 *payload = &message.tsaccellboard3temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_2, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_2, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_2, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_2, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_2, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard3temperatureGroupMux2 *payload = &message.tsaccellboard3temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_2, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_2, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_2, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_2, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_2, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard3temperatureGroupMux3 *payload = &message.tsaccellboard3temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_2, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_2, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_2, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_2, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_2, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard3temperatureGroupMux4 *payload = &message.tsaccellboard3temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_2, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_2, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_2, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_2, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_2, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard3temperatureGroupMux5 *payload = &message.tsaccellboard3temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_2, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_2, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_2, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_2, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_2, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard3temperatureGroupMux6 *payload = &message.tsaccellboard3temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_2, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_2, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_2, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_2, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_2, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard3temperatureGroupMux7 *payload = &message.tsaccellboard3temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_2, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_2, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_2, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_2, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_2, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard3temperatureGroupMux8 *payload = &message.tsaccellboard3temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_2, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_2, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_2, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_2, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_2, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard3temperatureGroupMux9 *payload = &message.tsaccellboard3temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_2, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_2, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_2, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE:
            switch (message.tsaccellboard4temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard4temperatureGroupMux0 *payload = &message.tsaccellboard4temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_3, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_3, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_3, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_3, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_3, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard4temperatureGroupMux1 *payload = &message.tsaccellboard4temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_3, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_3, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_3, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_3, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_3, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard4temperatureGroupMux2 *payload = &message.tsaccellboard4temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_3, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_3, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_3, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_3, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_3, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard4temperatureGroupMux3 *payload = &message.tsaccellboard4temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_3, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_3, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_3, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_3, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_3, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard4temperatureGroupMux4 *payload = &message.tsaccellboard4temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_3, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_3, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_3, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_3, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_3, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard4temperatureGroupMux5 *payload = &message.tsaccellboard4temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_3, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_3, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_3, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_3, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_3, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard4temperatureGroupMux6 *payload = &message.tsaccellboard4temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_3, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_3, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_3, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_3, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_3, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard4temperatureGroupMux7 *payload = &message.tsaccellboard4temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_3, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_3, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_3, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_3, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_3, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard4temperatureGroupMux8 *payload = &message.tsaccellboard4temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_3, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_3, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_3, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_3, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_3, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard4temperatureGroupMux9 *payload = &message.tsaccellboard4temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_3, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_3, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_3, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE:
            switch (message.tsaccellboard5temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard5temperatureGroupMux0 *payload = &message.tsaccellboard5temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_4, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_4, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_4, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_4, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_4, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard5temperatureGroupMux1 *payload = &message.tsaccellboard5temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_4, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_4, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_4, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_4, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_4, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard5temperatureGroupMux2 *payload = &message.tsaccellboard5temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_4, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_4, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_4, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_4, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_4, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard5temperatureGroupMux3 *payload = &message.tsaccellboard5temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_4, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_4, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_4, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_4, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_4, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard5temperatureGroupMux4 *payload = &message.tsaccellboard5temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_4, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_4, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_4, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_4, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_4, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard5temperatureGroupMux5 *payload = &message.tsaccellboard5temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_4, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_4, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_4, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_4, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_4, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard5temperatureGroupMux6 *payload = &message.tsaccellboard5temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_4, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_4, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_4, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_4, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_4, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard5temperatureGroupMux7 *payload = &message.tsaccellboard5temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_4, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_4, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_4, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_4, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_4, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard5temperatureGroupMux8 *payload = &message.tsaccellboard5temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_4, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_4, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_4, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_4, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_4, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard5temperatureGroupMux9 *payload = &message.tsaccellboard5temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_4, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_4, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_4, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE:
            switch (message.tsaccellboard6temperature.group) {
                case 0: {
                    struct CanBmsTsaccellboard6temperatureGroupMux0 *payload = &message.tsaccellboard6temperature.group_payload.mux_0;
                    temp_api_set_value(CELLBOARD_ID_5, 0, payload->cell1);
                    temp_api_set_value(CELLBOARD_ID_5, 1, payload->cell2);
                    temp_api_set_value(CELLBOARD_ID_5, 2, payload->cell3);
                    temp_api_set_value(CELLBOARD_ID_5, 3, payload->cell4);
                    temp_api_set_value(CELLBOARD_ID_5, 4, payload->cell5);
                    break;
                }
                case 1: {
                    struct CanBmsTsaccellboard6temperatureGroupMux1 *payload = &message.tsaccellboard6temperature.group_payload.mux_1;
                    temp_api_set_value(CELLBOARD_ID_5, 5, payload->cell6);
                    temp_api_set_value(CELLBOARD_ID_5, 6, payload->cell7);
                    temp_api_set_value(CELLBOARD_ID_5, 7, payload->cell8);
                    temp_api_set_value(CELLBOARD_ID_5, 8, payload->cell9);
                    temp_api_set_value(CELLBOARD_ID_5, 9, payload->cell10);
                    break;
                }
                case 2: {
                    struct CanBmsTsaccellboard6temperatureGroupMux2 *payload = &message.tsaccellboard6temperature.group_payload.mux_2;
                    temp_api_set_value(CELLBOARD_ID_5, 10, payload->cell11);
                    temp_api_set_value(CELLBOARD_ID_5, 11, payload->cell12);
                    temp_api_set_value(CELLBOARD_ID_5, 12, payload->cell13);
                    temp_api_set_value(CELLBOARD_ID_5, 13, payload->cell14);
                    temp_api_set_value(CELLBOARD_ID_5, 14, payload->cell15);
                    break;
                }
                case 3: {
                    struct CanBmsTsaccellboard6temperatureGroupMux3 *payload = &message.tsaccellboard6temperature.group_payload.mux_3;
                    temp_api_set_value(CELLBOARD_ID_5, 15, payload->cell16);
                    temp_api_set_value(CELLBOARD_ID_5, 16, payload->cell17);
                    temp_api_set_value(CELLBOARD_ID_5, 17, payload->cell18);
                    temp_api_set_value(CELLBOARD_ID_5, 18, payload->cell19);
                    temp_api_set_value(CELLBOARD_ID_5, 19, payload->cell20);
                    break;
                }
                case 4: {
                    struct CanBmsTsaccellboard6temperatureGroupMux4 *payload = &message.tsaccellboard6temperature.group_payload.mux_4;
                    temp_api_set_value(CELLBOARD_ID_5, 20, payload->cell21);
                    temp_api_set_value(CELLBOARD_ID_5, 21, payload->cell22);
                    temp_api_set_value(CELLBOARD_ID_5, 22, payload->cell23);
                    temp_api_set_value(CELLBOARD_ID_5, 23, payload->cell24);
                    temp_api_set_value(CELLBOARD_ID_5, 24, payload->cell25);
                    break;
                }
                case 5: {
                    struct CanBmsTsaccellboard6temperatureGroupMux5 *payload = &message.tsaccellboard6temperature.group_payload.mux_5;
                    temp_api_set_value(CELLBOARD_ID_5, 25, payload->cell26);
                    temp_api_set_value(CELLBOARD_ID_5, 26, payload->cell27);
                    temp_api_set_value(CELLBOARD_ID_5, 27, payload->cell28);
                    temp_api_set_value(CELLBOARD_ID_5, 28, payload->cell29);
                    temp_api_set_value(CELLBOARD_ID_5, 29, payload->cell30);
                    break;
                }
                case 6: {
                    struct CanBmsTsaccellboard6temperatureGroupMux6 *payload = &message.tsaccellboard6temperature.group_payload.mux_6;
                    temp_api_set_value(CELLBOARD_ID_5, 30, payload->cell31);
                    temp_api_set_value(CELLBOARD_ID_5, 31, payload->cell32);
                    temp_api_set_value(CELLBOARD_ID_5, 32, payload->cell33);
                    temp_api_set_value(CELLBOARD_ID_5, 33, payload->cell34);
                    temp_api_set_value(CELLBOARD_ID_5, 34, payload->cell35);
                    break;
                }
                case 7: {
                    struct CanBmsTsaccellboard6temperatureGroupMux7 *payload = &message.tsaccellboard6temperature.group_payload.mux_7;
                    temp_api_set_value(CELLBOARD_ID_5, 35, payload->cell36);
                    temp_api_set_value(CELLBOARD_ID_5, 36, payload->cell37);
                    temp_api_set_value(CELLBOARD_ID_5, 37, payload->cell38);
                    temp_api_set_value(CELLBOARD_ID_5, 38, payload->cell39);
                    temp_api_set_value(CELLBOARD_ID_5, 39, payload->cell40);
                    break;
                }
                case 8: {
                    struct CanBmsTsaccellboard6temperatureGroupMux8 *payload = &message.tsaccellboard6temperature.group_payload.mux_8;
                    temp_api_set_value(CELLBOARD_ID_5, 40, payload->cell41);
                    temp_api_set_value(CELLBOARD_ID_5, 41, payload->cell42);
                    temp_api_set_value(CELLBOARD_ID_5, 42, payload->cell43);
                    temp_api_set_value(CELLBOARD_ID_5, 43, payload->cell44);
                    temp_api_set_value(CELLBOARD_ID_5, 44, payload->cell45);
                    break;
                }
                case 9: {
                    struct CanBmsTsaccellboard6temperatureGroupMux9 *payload = &message.tsaccellboard6temperature.group_payload.mux_9;
                    temp_api_set_value(CELLBOARD_ID_5, 45, payload->cell46);
                    temp_api_set_value(CELLBOARD_ID_5, 46, payload->cell47);
                    temp_api_set_value(CELLBOARD_ID_5, 47, payload->cell48);
                    break;
                }
                default:
                    break;
            }
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_0,
                message.tsaccellboard1voltageinfo.min,
                message.tsaccellboard1voltageinfo.max,
                message.tsaccellboard1voltageinfo.average,
                message.tsaccellboard1voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_1,
                message.tsaccellboard2voltageinfo.min,
                message.tsaccellboard2voltageinfo.max,
                message.tsaccellboard2voltageinfo.average,
                message.tsaccellboard2voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_2,
                message.tsaccellboard3voltageinfo.min,
                message.tsaccellboard3voltageinfo.max,
                message.tsaccellboard3voltageinfo.average,
                message.tsaccellboard3voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_3,
                message.tsaccellboard4voltageinfo.min,
                message.tsaccellboard4voltageinfo.max,
                message.tsaccellboard4voltageinfo.average,
                message.tsaccellboard4voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_4,
                message.tsaccellboard5voltageinfo.min,
                message.tsaccellboard5voltageinfo.max,
                message.tsaccellboard5voltageinfo.average,
                message.tsaccellboard5voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGEINFO:
            volt_api_cellboard_voltage_info_handle(
                CELLBOARD_ID_5,
                message.tsaccellboard6voltageinfo.min,
                message.tsaccellboard6voltageinfo.max,
                message.tsaccellboard6voltageinfo.average,
                message.tsaccellboard6voltageinfo.total);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_0,
                message.tsaccellboard1error.post,
                message.tsaccellboard1error.cellundervoltage,
                message.tsaccellboard1error.cellovervoltage,
                message.tsaccellboard1error.cellundertemperature,
                message.tsaccellboard1error.cellovertemperature,
                message.tsaccellboard1error.dischargeresistorundertemperature,
                message.tsaccellboard1error.dischargeresistorovertemperature,
                message.tsaccellboard1error.cancommunication,
                message.tsaccellboard1error.flash,
                message.tsaccellboard1error.bmsmonitorcommunication,
                message.tsaccellboard1error.openwire1,
                message.tsaccellboard1error.openwire2,
                message.tsaccellboard1error.openwire3,
                message.tsaccellboard1error.openwire4,
                message.tsaccellboard1error.openwire5,
                message.tsaccellboard1error.openwire6,
                message.tsaccellboard1error.openwire7,
                message.tsaccellboard1error.openwire8,
                message.tsaccellboard1error.openwire9,
                message.tsaccellboard1error.openwire10,
                message.tsaccellboard1error.openwire11,
                message.tsaccellboard1error.openwire12,
                message.tsaccellboard1error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_1,
                message.tsaccellboard2error.post,
                message.tsaccellboard2error.cellundervoltage,
                message.tsaccellboard2error.cellovervoltage,
                message.tsaccellboard2error.cellundertemperature,
                message.tsaccellboard2error.cellovertemperature,
                message.tsaccellboard2error.dischargeresistorundertemperature,
                message.tsaccellboard2error.dischargeresistorovertemperature,
                message.tsaccellboard2error.cancommunication,
                message.tsaccellboard2error.flash,
                message.tsaccellboard2error.bmsmonitorcommunication,
                message.tsaccellboard2error.openwire1,
                message.tsaccellboard2error.openwire2,
                message.tsaccellboard2error.openwire3,
                message.tsaccellboard2error.openwire4,
                message.tsaccellboard2error.openwire5,
                message.tsaccellboard2error.openwire6,
                message.tsaccellboard2error.openwire7,
                message.tsaccellboard2error.openwire8,
                message.tsaccellboard2error.openwire9,
                message.tsaccellboard2error.openwire10,
                message.tsaccellboard2error.openwire11,
                message.tsaccellboard2error.openwire12,
                message.tsaccellboard2error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_2,
                message.tsaccellboard3error.post,
                message.tsaccellboard3error.cellundervoltage,
                message.tsaccellboard3error.cellovervoltage,
                message.tsaccellboard3error.cellundertemperature,
                message.tsaccellboard3error.cellovertemperature,
                message.tsaccellboard3error.dischargeresistorundertemperature,
                message.tsaccellboard3error.dischargeresistorovertemperature,
                message.tsaccellboard3error.cancommunication,
                message.tsaccellboard3error.flash,
                message.tsaccellboard3error.bmsmonitorcommunication,
                message.tsaccellboard3error.openwire1,
                message.tsaccellboard3error.openwire2,
                message.tsaccellboard3error.openwire3,
                message.tsaccellboard3error.openwire4,
                message.tsaccellboard3error.openwire5,
                message.tsaccellboard3error.openwire6,
                message.tsaccellboard3error.openwire7,
                message.tsaccellboard3error.openwire8,
                message.tsaccellboard3error.openwire9,
                message.tsaccellboard3error.openwire10,
                message.tsaccellboard3error.openwire11,
                message.tsaccellboard3error.openwire12,
                message.tsaccellboard3error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_3,
                message.tsaccellboard4error.post,
                message.tsaccellboard4error.cellundervoltage,
                message.tsaccellboard4error.cellovervoltage,
                message.tsaccellboard4error.cellundertemperature,
                message.tsaccellboard4error.cellovertemperature,
                message.tsaccellboard4error.dischargeresistorundertemperature,
                message.tsaccellboard4error.dischargeresistorovertemperature,
                message.tsaccellboard4error.cancommunication,
                message.tsaccellboard4error.flash,
                message.tsaccellboard4error.bmsmonitorcommunication,
                message.tsaccellboard4error.openwire1,
                message.tsaccellboard4error.openwire2,
                message.tsaccellboard4error.openwire3,
                message.tsaccellboard4error.openwire4,
                message.tsaccellboard4error.openwire5,
                message.tsaccellboard4error.openwire6,
                message.tsaccellboard4error.openwire7,
                message.tsaccellboard4error.openwire8,
                message.tsaccellboard4error.openwire9,
                message.tsaccellboard4error.openwire10,
                message.tsaccellboard4error.openwire11,
                message.tsaccellboard4error.openwire12,
                message.tsaccellboard4error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_4,
                message.tsaccellboard5error.post,
                message.tsaccellboard5error.cellundervoltage,
                message.tsaccellboard5error.cellovervoltage,
                message.tsaccellboard5error.cellundertemperature,
                message.tsaccellboard5error.cellovertemperature,
                message.tsaccellboard5error.dischargeresistorundertemperature,
                message.tsaccellboard5error.dischargeresistorovertemperature,
                message.tsaccellboard5error.cancommunication,
                message.tsaccellboard5error.flash,
                message.tsaccellboard5error.bmsmonitorcommunication,
                message.tsaccellboard5error.openwire1,
                message.tsaccellboard5error.openwire2,
                message.tsaccellboard5error.openwire3,
                message.tsaccellboard5error.openwire4,
                message.tsaccellboard5error.openwire5,
                message.tsaccellboard5error.openwire6,
                message.tsaccellboard5error.openwire7,
                message.tsaccellboard5error.openwire8,
                message.tsaccellboard5error.openwire9,
                message.tsaccellboard5error.openwire10,
                message.tsaccellboard5error.openwire11,
                message.tsaccellboard5error.openwire12,
                message.tsaccellboard5error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6ERROR:
            error_api_cellboard_handle(
                CELLBOARD_ID_5,
                message.tsaccellboard6error.post,
                message.tsaccellboard6error.cellundervoltage,
                message.tsaccellboard6error.cellovervoltage,
                message.tsaccellboard6error.cellundertemperature,
                message.tsaccellboard6error.cellovertemperature,
                message.tsaccellboard6error.dischargeresistorundertemperature,
                message.tsaccellboard6error.dischargeresistorovertemperature,
                message.tsaccellboard6error.cancommunication,
                message.tsaccellboard6error.flash,
                message.tsaccellboard6error.bmsmonitorcommunication,
                message.tsaccellboard6error.openwire1,
                message.tsaccellboard6error.openwire2,
                message.tsaccellboard6error.openwire3,
                message.tsaccellboard6error.openwire4,
                message.tsaccellboard6error.openwire5,
                message.tsaccellboard6error.openwire6,
                message.tsaccellboard6error.openwire7,
                message.tsaccellboard6error.openwire8,
                message.tsaccellboard6error.openwire9,
                message.tsaccellboard6error.openwire10,
                message.tsaccellboard6error.openwire11,
                message.tsaccellboard6error.openwire12,
                message.tsaccellboard6error.openwire13);
            break;
        case CAN_BMS_MESSAGE_FRAME_ID_IVTMESSAGERESULTI:
            constexpr float ma_to_a = 0.001F;
            current_api_set_current(message.ivtmessageresulti.current * ma_to_a);
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
