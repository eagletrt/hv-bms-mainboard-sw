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

#include "can-bms.h"
#include "can-primary.h"
#include "mainboard-status.h"

/*!
 * \brief Return codes of the CAN communication info functions
 */
enum CanCommunicationInfoReturnCode {
    CAN_COMMUNICATION_INFO_RC_OK = 0,        /*!< The information was retrieved successfully */
    CAN_COMMUNICATION_INFO_RC_INVALID_INDEX, /*!< The index is invalid */
    CAN_COMMUNICATION_INFO_RC_NULL_POINTER,  /*!< A pointer parameter is NULL */
    CAN_COMMUNICATION_INFO_RC_ERROR,         /*!< An error occurred */
};

/*!
 * \brief Information about a message received on the BMS network
 */
struct CanCommunicationInfoBmsMessage {
    enum CanBmsMessageFrameId id;    /*!< The frame ID of the message */
    int32_t cycle_time;              /*!< The cycle time of the message in ms */
    uint8_t max_dropped;             /*!< The maximum number of consecutive messages that can be dropped */
    enum MainboardStatusFault fault; /*!< The fault set when the message reaches its maximum number of consecutive drops */
};

/*!
 * \brief Information about a message received on the primary network
 */
struct CanCommunicationInfoPrimaryMessage {
    enum CanPrimaryMessageFrameId id; /*!< The frame ID of the message */
    int32_t cycle_time;               /*!< The cycle time of the message in ms */
    uint8_t max_dropped;              /*!< The maximum number of consecutive messages that can be dropped */
};

#endif // CAN_COMMUNICATION_INFO_H
