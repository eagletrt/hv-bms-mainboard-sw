/*!
 * \file serializer.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the CAN serializer
 */

#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stdbool.h>
#include <stdint.h>

#include "can-bms.h"
#include "can-primary.h"
#include "mainboard-def.h"

/*! \brief Number of cell voltages carried by one group of a cellboard voltage message */
#define SERIALIZER_VOLTAGES_PER_GROUP (sizeof(struct CanPrimaryTsaccellboard1voltageGroupMux0) / sizeof(float))

/*! \brief Number of groups needed to send all the cell voltages of a cellboard */
#define SERIALIZER_VOLTAGES_GROUP_COUNT ((CELLBOARD_SEGMENT_SERIES_COUNT + SERIALIZER_VOLTAGES_PER_GROUP - 1U) / SERIALIZER_VOLTAGES_PER_GROUP)

/*! \brief Number of cell temperatures carried by one group of a cellboard temperature message */
#define SERIALIZER_TEMPERATURES_PER_GROUP (sizeof(struct CanPrimaryTsaccellboard1temperatureGroupMux0) / sizeof(float))

/*! \brief Number of groups needed to send all the cell temperatures of a cellboard */
#define SERIALIZER_TEMPERATURES_GROUP_COUNT ((CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT + SERIALIZER_TEMPERATURES_PER_GROUP - 1U) / SERIALIZER_TEMPERATURES_PER_GROUP)

/*! \brief Number of cellboards whose errors are carried by one cellboard errors message */
#define SERIALIZER_CELLBOARDS_PER_ERRORS_MESSAGE (2U)

/*! \brief Number of cellboard errors messages (TsacCellboardErrorA, B and C) */
#define SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT (CELLBOARD_COUNT / SERIALIZER_CELLBOARDS_PER_ERRORS_MESSAGE)

/*!
 * \brief Return code for the serializer module functions
 */
enum SerializerReturnCode {
    SERIALIZER_RC_OK,                /*!< The function executed succesfully */
    SERIALIZER_RC_NULL_POINTER,      /*!< An invalid NULL pointer was given */
    SERIALIZER_RC_INVALID_CELLBOARD, /*!< The given cellboard does not exist */
    SERIALIZER_RC_INVALID_INDEX      /*!< The given message index is out of range */
};

/*!
 * \brief A primary network message ready to be serialized with libcan
 *
 * \details Pass both fields to can_primary_api_serialize_from_id to get the raw bytes
 */
struct SerializerMessage {
    enum CanPrimaryMessageFrameId id; /*!< The frame ID of the message */
    union CanPrimaryMessages payload; /*!< The message signals */
};

/*!
 * \brief A BMS network message ready to be serialized with libcan
 *
 * \details Pass both fields to can_bms_api_serialize_from_id to get the raw bytes
 */
struct SerializerBmsMessage {
    enum CanBmsMessageFrameId id; /*!< The frame ID of the message */
    union CanBmsMessages payload; /*!< The message signals */
};

/*!
 * \brief Serializer handler structure
 *
 * \details The cellboard voltages and temperatures do not fit in one frame, so they are
 * sent one group at a time; the handler keeps the next group to send for each cellboard
 */
struct SerializerHandler {
    uint8_t voltages_group[CELLBOARD_COUNT];     /*!< The next voltages group to send for each cellboard */
    uint8_t temperatures_group[CELLBOARD_COUNT]; /*!< The next temperatures group to send for each cellboard */
    uint32_t build_time;                         /*!< The firmware build time as a Unix timestamp, 0 if unknown */
    bool comms_suppressed;                       /*!< True while the periodic messages must not be sent (e.g. during a flash) */
};

#endif // SERIALIZER_H
