/*!
 * \file can-communication-info.h
 * \date 2026-11-21
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the CAN communication info
 */

#ifndef CAN_COMMUNICATION_INFO_H
#define CAN_COMMUNICATION_INFO_H

#include <stdint.h>

#include "mainboard-def.h"
#include "mainboard-status.h"

/*!
 * \brief Return codes of the CAN communication info functions
 */
enum CanCommunicationInfoReturnCode {
    CAN_COMMUNICATION_INFO_RC_OK = 0,          /*!< The information was retrieved successfully */
    CAN_COMMUNICATION_INFO_RC_INVALID_NETWORK, /*!< The network is out of range */
    CAN_COMMUNICATION_INFO_RC_INVALID_INDEX,   /*!< The message is not monitored on the given network */
    CAN_COMMUNICATION_INFO_RC_ERROR,           /*!< An error occurred */
};

/*!
 * \brief Information about a monitored message
 */
struct CanCommunicationInfoMessage {
    can_id id;                       /*!< The frame ID of the message */
    int32_t cycle_time;              /*!< The cycle time of the message in ms */
    uint8_t max_dropped;             /*!< The maximum number of consecutive messages that can be dropped */
    enum MainboardStatusFault fault; /*!< The fault set when the message reaches its maximum number of consecutive drops, MAINBOARD_STATUS_FAULT_COUNT if none (primary network) */
};

#endif // CAN_COMMUNICATION_INFO_H
