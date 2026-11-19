/*!
 * \file serializer-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN serializer: gets the updated values and builds the packets to send
 *
 * \details Every function fills a SerializerMessage with the frame ID and the signals of one
 * primary network message; the caller turns it into bytes with can_primary_api_serialize_from_id.
 * The only BMS network message, the balancing set, fills a SerializerBmsMessage instead, to be
 * serialized with can_bms_api_serialize_from_id.
 *
 * Primary network messages:
 * - TSAC status (1 message, mainboard and cellboards FSM state)
 * - Balancing (1 for each cellboard)
 * - Balancing temperatures (1 for each cellboard, discharge resistor temperatures)
 * - Cooling temperatures (2 messages)
 * - Current (1 message)
 * - Errors (1 for the mainboard, 3 for the cellboards, 2 cellboards each)
 * - Feedback (1 message)
 * - IMD (1 message)
 * - Internal voltages (1 message)
 * - Shutdown (2 messages, shutdown nodes and shutdown feedback)
 * - Temperatures (1 for each cellboard, 1 for general info)
 * - Version (4 messages, firmware and libcan version and version info)
 * - Cellboard versions (4 for each cellboard, relayed from the BMS network)
 * - Voltages (1 for each cellboard)
 *
 * BMS network messages:
 * - Balancing set (1 message, balancing start, target and threshold for the cellboards)
 *
 * libcan generates a distinct struct for each cellboard (TsacCellboard1Voltage, TsacCellboard2Voltage, ...)
 * but all of them share the same layout and live at offset 0 of the same messages union, so every
 * per-cellboard message is written through the cellboard 1 member of the union and a table gives
 * the frame ID of each cellboard. The static assertions below break the build if a libcan update
 * makes the layouts diverge. The cellboard errors messages are the exception: each one names its
 * fields after the two cellboards it carries, so they are filled field by field.
 */

#include "serializer-api.h"

#include <stddef.h>
#include <string.h>
#include <time.h>

#include "eagletrt-api.h"
#include "can-bms.h"
#include "can-primary.h"
#include "can-version.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "bal-control-api.h"
#include "cellboards-monitor-api.h"
#include "feedback-api.h"
#include "internal-temperatures-api.h"
#include "internal-voltages-api.h"
#include "mainboard-status-api.h"
#include "sensing-api.h"
#include "temperatures-api.h"
#include "voltages-api.h"

// Every cellboard message must have the same layout as the cellboard 1 one
static_assert(sizeof(struct CanPrimaryTsaccellboard2balancing) == sizeof(struct CanPrimaryTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3balancing) == sizeof(struct CanPrimaryTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4balancing) == sizeof(struct CanPrimaryTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5balancing) == sizeof(struct CanPrimaryTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6balancing) == sizeof(struct CanPrimaryTsaccellboard1balancing), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2voltage) == sizeof(struct CanPrimaryTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3voltage) == sizeof(struct CanPrimaryTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4voltage) == sizeof(struct CanPrimaryTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5voltage) == sizeof(struct CanPrimaryTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6voltage) == sizeof(struct CanPrimaryTsaccellboard1voltage), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2temperature) == sizeof(struct CanPrimaryTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3temperature) == sizeof(struct CanPrimaryTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4temperature) == sizeof(struct CanPrimaryTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5temperature) == sizeof(struct CanPrimaryTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6temperature) == sizeof(struct CanPrimaryTsaccellboard1temperature), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2balancingtemperature) == sizeof(struct CanPrimaryTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3balancingtemperature) == sizeof(struct CanPrimaryTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4balancingtemperature) == sizeof(struct CanPrimaryTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5balancingtemperature) == sizeof(struct CanPrimaryTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6balancingtemperature) == sizeof(struct CanPrimaryTsaccellboard1balancingtemperature), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2version) == sizeof(struct CanPrimaryTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3version) == sizeof(struct CanPrimaryTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4version) == sizeof(struct CanPrimaryTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5version) == sizeof(struct CanPrimaryTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6version) == sizeof(struct CanPrimaryTsaccellboard1version), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2versioninfo) == sizeof(struct CanPrimaryTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3versioninfo) == sizeof(struct CanPrimaryTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4versioninfo) == sizeof(struct CanPrimaryTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5versioninfo) == sizeof(struct CanPrimaryTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6versioninfo) == sizeof(struct CanPrimaryTsaccellboard1versioninfo), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2libcanversion) == sizeof(struct CanPrimaryTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3libcanversion) == sizeof(struct CanPrimaryTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4libcanversion) == sizeof(struct CanPrimaryTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5libcanversion) == sizeof(struct CanPrimaryTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6libcanversion) == sizeof(struct CanPrimaryTsaccellboard1libcanversion), "Layout mismatch");

static_assert(sizeof(struct CanPrimaryTsaccellboard2libcanversioninfo) == sizeof(struct CanPrimaryTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard3libcanversioninfo) == sizeof(struct CanPrimaryTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard4libcanversioninfo) == sizeof(struct CanPrimaryTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard5libcanversioninfo) == sizeof(struct CanPrimaryTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanPrimaryTsaccellboard6libcanversioninfo) == sizeof(struct CanPrimaryTsaccellboard1libcanversioninfo), "Layout mismatch");

// The discharge resistor temperatures are copied as a plain float array: the message must contain nothing but them
static_assert(sizeof(struct CanPrimaryTsaccellboard1balancingtemperature) == CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT * sizeof(float), "Unexpected balancing temperature layout");

/*! \brief Number of bytes of the balancing status flag carried by the balancing message */
#define SERIALIZER_BALANCING_FLAG_SIZE ((CELLBOARD_SEGMENT_SERIES_COUNT + 7U) / 8U)

static_assert(SERIALIZER_BALANCING_FLAG_SIZE <= sizeof(uint32_t), "Too many cells for the balancing flag");
static_assert(sizeof(struct CanPrimaryTsaccellboard1balancing) == SERIALIZER_BALANCING_FLAG_SIZE, "Unexpected balancing layout");

// Every group carries the same number of floats, the last one can have unused slots
static_assert(sizeof(union CanPrimaryTsaccellboard1voltageGroupPayload) == SERIALIZER_VOLTAGES_PER_GROUP * sizeof(float), "Unexpected voltage group layout");
static_assert(sizeof(union CanPrimaryTsaccellboard1temperatureGroupPayload) == SERIALIZER_TEMPERATURES_PER_GROUP * sizeof(float), "Unexpected temperature group layout");

// The cellboard errors messages carry the cellboards in pairs, with 10 errors and 13 open wires each
static_assert(CELLBOARD_COUNT == SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT * SERIALIZER_CELLBOARDS_PER_ERRORS_MESSAGE, "The cellboards do not fit the errors messages");
static_assert(SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT == 3U, "Expected the TsacCellboardErrorA, B and C messages");
static_assert(CELLBOARDS_MONITOR_ERROR_COUNT == 10U, "The cellboard errors messages carry 10 errors per cellboard");
static_assert(CELLBOARDS_MONITOR_OPEN_WIRE_COUNT == 13U, "The cellboard errors messages carry 13 open wires per cellboard");

// The IMD status of the shutdown message uses the same values as the one of the IMD message
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_SHORT_CIRCUIT == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_SHORT_CIRCUIT, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_NORMAL == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_NORMAL, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_UNDERVOLTAGE == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_UNDERVOLTAGE, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_START_MEASURE == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_START_MEASURE, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_DEVICE_ERROR == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_DEVICE_ERROR, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_EARTH_FAULT == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_EARTH_FAULT, "IMD status mismatch");
static_assert((int)CAN_PRIMARY_TSACMAINBOARDSHUTDOWN_IMDSTATUS_INVALID == (int)CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_INVALID, "IMD status mismatch");

/*! \brief Frame ID of the balancing message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId balancing_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING,
};

/*! \brief Frame ID of the discharge resistor temperatures message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId balancing_temperatures_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCINGTEMPERATURE,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCINGTEMPERATURE,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCINGTEMPERATURE,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCINGTEMPERATURE,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCINGTEMPERATURE,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCINGTEMPERATURE,
};

/*! \brief Frame ID of the cell voltages message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId voltages_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE,
};

/*! \brief Frame ID of the cell temperatures message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId temperatures_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE,
};

/*! \brief Frame ID of the firmware version number message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId cellboard_version_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSION,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSION,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSION,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSION,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSION,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSION,
};

/*! \brief Frame ID of the firmware version info message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId cellboard_version_info_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSIONINFO,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSIONINFO,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSIONINFO,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSIONINFO,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSIONINFO,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSIONINFO,
};

/*! \brief Frame ID of the libcan version number message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId cellboard_libcan_version_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSION,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSION,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSION,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSION,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSION,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSION,
};

/*! \brief Frame ID of the libcan version info message of each cellboard */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId cellboard_libcan_version_info_frame_ids[CELLBOARD_COUNT] = {
    [CELLBOARD_ID_0] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSIONINFO,
    [CELLBOARD_ID_1] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSIONINFO,
    [CELLBOARD_ID_2] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSIONINFO,
    [CELLBOARD_ID_3] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSIONINFO,
    [CELLBOARD_ID_4] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSIONINFO,
    [CELLBOARD_ID_5] = CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSIONINFO,
};

/*! \brief Frame ID of each cellboard errors message */
EAGLETRT_STATIC const enum CanPrimaryMessageFrameId cellboard_errors_frame_ids[SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT] = {
    CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORA,
    CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORB,
    CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARDERRORC,
};

/*!
 * \brief The global serializer handler
 */
EAGLETRT_STATIC struct SerializerHandler serializer_handler;

/*!
 * \brief Set the frame ID of a message and clear its signals
 *
 * \param[out] message The message to reset
 * \param[in] frame_id The frame ID of the message
 */
EAGLETRT_STATIC void prv_reset_message(struct SerializerMessage *message, enum CanPrimaryMessageFrameId frame_id) {
    memset(message, 0, sizeof(*message));
    message->id = frame_id;
}

/*!
 * \brief Convert the IMD status to its CAN signal value
 *
 * \param[in] status The IMD status
 *
 * \return The CAN signal value, CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_INVALID if the status is unknown
 */
EAGLETRT_STATIC enum CanPrimaryTsacmainboardimdStatus prv_imd_status_to_can(enum Ir1553204Status status) {
    switch (status) {
        case IR1553204_STATUS_SHORT_CIRCUIT:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_SHORT_CIRCUIT;
        case IR1553204_STATUS_NORMAL:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_NORMAL;
        case IR1553204_STATUS_UNDER_VOLTAGE:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_UNDERVOLTAGE;
        case IR1553204_STATUS_START_MEASURE:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_START_MEASURE;
        case IR1553204_STATUS_DEVICE_ERROR:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_DEVICE_ERROR;
        case IR1553204_STATUS_EARTH_FAULT:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_EARTH_FAULT;
        default:
            return CAN_PRIMARY_TSACMAINBOARDIMD_STATUS_INVALID;
    }
}

/*!
 * \brief Convert the state of a cellboard to its CAN signal value
 *
 * \details The six cellboard status enums of the TsacStatus message share the same values,
 * the cellboard 1 one is used for all of them
 *
 * \param[in] state The cellboard state
 *
 * \return The CAN signal value, CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_ERROR if the state is unknown
 */
EAGLETRT_STATIC enum CanPrimaryTsacstatusCellboard1status prv_cellboard_state_to_can(enum CellboardsMonitorState state) {
    switch (state) {
        case CELLBOARDS_MONITOR_STATE_INIT:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_INIT;
        case CELLBOARDS_MONITOR_STATE_IDLE:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_IDLE;
        case CELLBOARDS_MONITOR_STATE_ERROR:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_ERROR;
        case CELLBOARDS_MONITOR_STATE_FLASH:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_FLASH;
        case CELLBOARDS_MONITOR_STATE_DISCHARGE:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_DISCHARGE;
        case CELLBOARDS_MONITOR_STATE_COOLDOWN:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_COOLDOWN;
        default:
            return CAN_PRIMARY_TSACSTATUS_CELLBOARD1STATUS_ERROR;
    }
}

/*!
 * \brief Get the voltage measured by an analog feedback
 *
 * \param[in] index The index of the analog feedback
 *
 * \return The voltage of the analog feedback in V
 */
EAGLETRT_STATIC volt prv_feedback_voltage(enum FeedbackAnalogIndex index) {
    // TODO: check if everything needs the same scaling
    return FEEDBACK_TO_SD_VOLT(feedback_api_get_analog_value(index));
}

void serializer_api_init(void) {
    memset(&serializer_handler, 0, sizeof(serializer_handler));

    // __DATE__ and __TIME__ are in the local time of the build machine, mktime treats them as the local time of the target
    struct tm time = { 0 };
    if (strptime(MAINBOARD_BUILD_TIME_STR, "%b %d %Y %H:%M:%S", &time) != NULL) {
        const time_t build_time = mktime(&time);
        serializer_handler.build_time = (build_time < 0) ? 0U : (uint32_t)build_time;
    }
}

// STATUS

enum SerializerReturnCode serializer_api_get_status_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACSTATUS);

    struct CanPrimaryTsacstatus *payload = &message->payload.tsacstatus;
    // TODO: send the mainboard FSM state once the mainboard FSM exposes it
    payload->mainboardstatus = CAN_PRIMARY_TSACSTATUS_MAINBOARDSTATUS_INIT;
    payload->cellboard1status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_0));
    payload->cellboard2status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_1));
    payload->cellboard3status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_2));
    payload->cellboard4status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_3));
    payload->cellboard5status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_4));
    payload->cellboard6status = prv_cellboard_state_to_can(cellboards_monitor_api_get_state(CELLBOARD_ID_5));
    return SERIALIZER_RC_OK;
}

// BALANCING

enum SerializerReturnCode serializer_api_get_balancing_set_message(struct SerializerBmsMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    memset(message, 0, sizeof(*message));
    message->id = CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDBALANCINGSET;

    struct CanBmsTsacmainboardbalancingset *payload = &message->payload.tsacmainboardbalancingset;
    payload->start = bal_control_api_is_balancing_active() ? 1U : 0U;
    payload->target = bal_control_api_get_target_voltage();
    payload->threshold = bal_control_api_get_threshold();
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_balancing_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, balancing_frame_ids[cellboard]);

    // The cell flags are packed 1-bit bitfields; on GCC little-endian targets cell1 is bit 0
    // of the first byte, which is already the layout of the balancing status flag
    const uint32_t cells = bal_control_api_get_balancing_status(cellboard);
    memcpy(&message->payload.tsaccellboard1balancing, &cells, SERIALIZER_BALANCING_FLAG_SIZE);
    return SERIALIZER_RC_OK;
}

// COOLING TEMPERATURES

enum SerializerReturnCode serializer_api_get_cooling_temperatures_1_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCOOLINGTEMPERATURE1);

    struct CanPrimaryTsacmainboardcoolingtemperature1 *payload = &message->payload.tsacmainboardcoolingtemperature1;
    payload->inlet = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_INLET);
    payload->outlet1 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_1);
    payload->outlet2 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_2);
    payload->outlet3 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_3);
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cooling_temperatures_2_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCOOLINGTEMPERATURE2);

    struct CanPrimaryTsacmainboardcoolingtemperature2 *payload = &message->payload.tsacmainboardcoolingtemperature2;
    payload->outlet4 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_4);
    payload->outlet5 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_5);
    payload->outlet6 = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_6);
    return SERIALIZER_RC_OK;
}

// CURRENT

enum SerializerReturnCode serializer_api_get_current_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCURRENTINFO);

    const struct InternalVoltagesInfo info = internal_voltages_api_get_info();
    struct CanPrimaryTsacmainboardcurrentinfo *payload = &message->payload.tsacmainboardcurrentinfo;
    payload->current = info.current;
    payload->power = info.power;
    return SERIALIZER_RC_OK;
}

// ERRORS

enum SerializerReturnCode serializer_api_get_mainboard_errors_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDERROR);

    // The CAN communication error reports the BMS network: the data sent by the cellboards stopped arriving
    const bool can_communication = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES) ||
                                   mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES) ||
                                   mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS);

    struct CanPrimaryTsacmainboarderror *payload = &message->payload.tsacmainboarderror;
    payload->post = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_POST_FAILED);
    payload->overcurrent = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_OVER_CURRENT);
    payload->overpower = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_OVER_POWER);
    payload->undervoltage = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_CELL_UNDER_VOLTAGE);
    payload->overvoltage = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_CELL_OVER_VOLTAGE);
    payload->undertemperature = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_CELL_UNDER_TEMPERATURE);
    payload->overtemperature = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_CELL_OVER_TEMPERATURE);
    payload->cancommunication = can_communication;
    payload->currentsensorcommunication = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_STALE_CURRENT);
    payload->coolingundertemperature = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_COOLING_UNDER_TEMPERATURE);
    payload->coolingovertemperature = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_COOLING_OVER_TEMPERATURE);
    payload->cellboard = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_CELLBOARD_FAULT);
    payload->connectordisconnected = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_VOLTAGE_INDICATOR_DISCONNECTED);
    payload->imd = mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_IMD_FAULT);
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_errors_message(uint8_t index, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (index >= SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT) {
        return SERIALIZER_RC_INVALID_INDEX;
    }

    prv_reset_message(message, cellboard_errors_frame_ids[index]);

    switch (index) {
        case 0U: {
            struct CanPrimaryTsaccellboarderrora *payload = &message->payload.tsaccellboarderrora;
            const bit_flag16 errors1 = cellboards_monitor_api_get_errors(CELLBOARD_ID_0);
            const bit_flag16 open_wires1 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_0);
            const bit_flag16 errors2 = cellboards_monitor_api_get_errors(CELLBOARD_ID_1);
            const bit_flag16 open_wires2 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_1);

            payload->cellboard1post = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard1cellundervoltage = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard1cellovervoltage = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard1cellundertemperature = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard1cellovertemperature = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard1dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard1dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard1cancommunication = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard1flash = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard1bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors1, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard1openwire1 = EAGLETRT_API_BIT_GET(open_wires1, 0U);
            payload->cellboard1openwire2 = EAGLETRT_API_BIT_GET(open_wires1, 1U);
            payload->cellboard1openwire3 = EAGLETRT_API_BIT_GET(open_wires1, 2U);
            payload->cellboard1openwire4 = EAGLETRT_API_BIT_GET(open_wires1, 3U);
            payload->cellboard1openwire5 = EAGLETRT_API_BIT_GET(open_wires1, 4U);
            payload->cellboard1openwire6 = EAGLETRT_API_BIT_GET(open_wires1, 5U);
            payload->cellboard1openwire7 = EAGLETRT_API_BIT_GET(open_wires1, 6U);
            payload->cellboard1openwire8 = EAGLETRT_API_BIT_GET(open_wires1, 7U);
            payload->cellboard1openwire9 = EAGLETRT_API_BIT_GET(open_wires1, 8U);
            payload->cellboard1openwire10 = EAGLETRT_API_BIT_GET(open_wires1, 9U);
            payload->cellboard1openwire11 = EAGLETRT_API_BIT_GET(open_wires1, 10U);
            payload->cellboard1openwire12 = EAGLETRT_API_BIT_GET(open_wires1, 11U);
            payload->cellboard1openwire13 = EAGLETRT_API_BIT_GET(open_wires1, 12U);

            payload->cellboard2post = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard2cellundervoltage = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard2cellovervoltage = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard2cellundertemperature = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard2cellovertemperature = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard2dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard2dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard2cancommunication = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard2flash = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard2bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors2, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard2openwire1 = EAGLETRT_API_BIT_GET(open_wires2, 0U);
            payload->cellboard2openwire2 = EAGLETRT_API_BIT_GET(open_wires2, 1U);
            payload->cellboard2openwire3 = EAGLETRT_API_BIT_GET(open_wires2, 2U);
            payload->cellboard2openwire4 = EAGLETRT_API_BIT_GET(open_wires2, 3U);
            payload->cellboard2openwire5 = EAGLETRT_API_BIT_GET(open_wires2, 4U);
            payload->cellboard2openwire6 = EAGLETRT_API_BIT_GET(open_wires2, 5U);
            payload->cellboard2openwire7 = EAGLETRT_API_BIT_GET(open_wires2, 6U);
            payload->cellboard2openwire8 = EAGLETRT_API_BIT_GET(open_wires2, 7U);
            payload->cellboard2openwire9 = EAGLETRT_API_BIT_GET(open_wires2, 8U);
            payload->cellboard2openwire10 = EAGLETRT_API_BIT_GET(open_wires2, 9U);
            payload->cellboard2openwire11 = EAGLETRT_API_BIT_GET(open_wires2, 10U);
            payload->cellboard2openwire12 = EAGLETRT_API_BIT_GET(open_wires2, 11U);
            payload->cellboard2openwire13 = EAGLETRT_API_BIT_GET(open_wires2, 12U);
            break;
        }
        case 1U: {
            struct CanPrimaryTsaccellboarderrorb *payload = &message->payload.tsaccellboarderrorb;
            const bit_flag16 errors3 = cellboards_monitor_api_get_errors(CELLBOARD_ID_2);
            const bit_flag16 open_wires3 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_2);
            const bit_flag16 errors4 = cellboards_monitor_api_get_errors(CELLBOARD_ID_3);
            const bit_flag16 open_wires4 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_3);

            payload->cellboard3post = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard3cellundervoltage = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard3cellovervoltage = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard3cellundertemperature = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard3cellovertemperature = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard3dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard3dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard3cancommunication = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard3flash = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard3bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors3, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard3openwire1 = EAGLETRT_API_BIT_GET(open_wires3, 0U);
            payload->cellboard3openwire2 = EAGLETRT_API_BIT_GET(open_wires3, 1U);
            payload->cellboard3openwire3 = EAGLETRT_API_BIT_GET(open_wires3, 2U);
            payload->cellboard3openwire4 = EAGLETRT_API_BIT_GET(open_wires3, 3U);
            payload->cellboard3openwire5 = EAGLETRT_API_BIT_GET(open_wires3, 4U);
            payload->cellboard3openwire6 = EAGLETRT_API_BIT_GET(open_wires3, 5U);
            payload->cellboard3openwire7 = EAGLETRT_API_BIT_GET(open_wires3, 6U);
            payload->cellboard3openwire8 = EAGLETRT_API_BIT_GET(open_wires3, 7U);
            payload->cellboard3openwire9 = EAGLETRT_API_BIT_GET(open_wires3, 8U);
            payload->cellboard3openwire10 = EAGLETRT_API_BIT_GET(open_wires3, 9U);
            payload->cellboard3openwire11 = EAGLETRT_API_BIT_GET(open_wires3, 10U);
            payload->cellboard3openwire12 = EAGLETRT_API_BIT_GET(open_wires3, 11U);
            payload->cellboard3openwire13 = EAGLETRT_API_BIT_GET(open_wires3, 12U);

            payload->cellboard4post = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard4cellundervoltage = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard4cellovervoltage = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard4cellundertemperature = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard4cellovertemperature = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard4dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard4dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard4cancommunication = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard4flash = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard4bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors4, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard4openwire1 = EAGLETRT_API_BIT_GET(open_wires4, 0U);
            payload->cellboard4openwire2 = EAGLETRT_API_BIT_GET(open_wires4, 1U);
            payload->cellboard4openwire3 = EAGLETRT_API_BIT_GET(open_wires4, 2U);
            payload->cellboard4openwire4 = EAGLETRT_API_BIT_GET(open_wires4, 3U);
            payload->cellboard4openwire5 = EAGLETRT_API_BIT_GET(open_wires4, 4U);
            payload->cellboard4openwire6 = EAGLETRT_API_BIT_GET(open_wires4, 5U);
            payload->cellboard4openwire7 = EAGLETRT_API_BIT_GET(open_wires4, 6U);
            payload->cellboard4openwire8 = EAGLETRT_API_BIT_GET(open_wires4, 7U);
            payload->cellboard4openwire9 = EAGLETRT_API_BIT_GET(open_wires4, 8U);
            payload->cellboard4openwire10 = EAGLETRT_API_BIT_GET(open_wires4, 9U);
            payload->cellboard4openwire11 = EAGLETRT_API_BIT_GET(open_wires4, 10U);
            payload->cellboard4openwire12 = EAGLETRT_API_BIT_GET(open_wires4, 11U);
            payload->cellboard4openwire13 = EAGLETRT_API_BIT_GET(open_wires4, 12U);
            break;
        }
        case 2U: {
            struct CanPrimaryTsaccellboarderrorc *payload = &message->payload.tsaccellboarderrorc;
            const bit_flag16 errors5 = cellboards_monitor_api_get_errors(CELLBOARD_ID_4);
            const bit_flag16 open_wires5 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_4);
            const bit_flag16 errors6 = cellboards_monitor_api_get_errors(CELLBOARD_ID_5);
            const bit_flag16 open_wires6 = cellboards_monitor_api_get_open_wires(CELLBOARD_ID_5);

            payload->cellboard5post = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard5cellundervoltage = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard5cellovervoltage = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard5cellundertemperature = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard5cellovertemperature = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard5dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard5dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard5cancommunication = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard5flash = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard5bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors5, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard5openwire1 = EAGLETRT_API_BIT_GET(open_wires5, 0U);
            payload->cellboard5openwire2 = EAGLETRT_API_BIT_GET(open_wires5, 1U);
            payload->cellboard5openwire3 = EAGLETRT_API_BIT_GET(open_wires5, 2U);
            payload->cellboard5openwire4 = EAGLETRT_API_BIT_GET(open_wires5, 3U);
            payload->cellboard5openwire5 = EAGLETRT_API_BIT_GET(open_wires5, 4U);
            payload->cellboard5openwire6 = EAGLETRT_API_BIT_GET(open_wires5, 5U);
            payload->cellboard5openwire7 = EAGLETRT_API_BIT_GET(open_wires5, 6U);
            payload->cellboard5openwire8 = EAGLETRT_API_BIT_GET(open_wires5, 7U);
            payload->cellboard5openwire9 = EAGLETRT_API_BIT_GET(open_wires5, 8U);
            payload->cellboard5openwire10 = EAGLETRT_API_BIT_GET(open_wires5, 9U);
            payload->cellboard5openwire11 = EAGLETRT_API_BIT_GET(open_wires5, 10U);
            payload->cellboard5openwire12 = EAGLETRT_API_BIT_GET(open_wires5, 11U);
            payload->cellboard5openwire13 = EAGLETRT_API_BIT_GET(open_wires5, 12U);

            payload->cellboard6post = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_POST);
            payload->cellboard6cellundervoltage = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE);
            payload->cellboard6cellovervoltage = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE);
            payload->cellboard6cellundertemperature = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE);
            payload->cellboard6cellovertemperature = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE);
            payload->cellboard6dischargeresistorundertemperature = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE);
            payload->cellboard6dischargeresistorovertemperature = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE);
            payload->cellboard6cancommunication = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION);
            payload->cellboard6flash = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_FLASH);
            payload->cellboard6bmsmonitorcommunication = EAGLETRT_API_BIT_GET(errors6, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION);
            // Bit N is open wire N + 1
            payload->cellboard6openwire1 = EAGLETRT_API_BIT_GET(open_wires6, 0U);
            payload->cellboard6openwire2 = EAGLETRT_API_BIT_GET(open_wires6, 1U);
            payload->cellboard6openwire3 = EAGLETRT_API_BIT_GET(open_wires6, 2U);
            payload->cellboard6openwire4 = EAGLETRT_API_BIT_GET(open_wires6, 3U);
            payload->cellboard6openwire5 = EAGLETRT_API_BIT_GET(open_wires6, 4U);
            payload->cellboard6openwire6 = EAGLETRT_API_BIT_GET(open_wires6, 5U);
            payload->cellboard6openwire7 = EAGLETRT_API_BIT_GET(open_wires6, 6U);
            payload->cellboard6openwire8 = EAGLETRT_API_BIT_GET(open_wires6, 7U);
            payload->cellboard6openwire9 = EAGLETRT_API_BIT_GET(open_wires6, 8U);
            payload->cellboard6openwire10 = EAGLETRT_API_BIT_GET(open_wires6, 9U);
            payload->cellboard6openwire11 = EAGLETRT_API_BIT_GET(open_wires6, 10U);
            payload->cellboard6openwire12 = EAGLETRT_API_BIT_GET(open_wires6, 11U);
            payload->cellboard6openwire13 = EAGLETRT_API_BIT_GET(open_wires6, 12U);
            break;
        }
        default:
            break;
    }
    return SERIALIZER_RC_OK;
}

// FEEDBACK

enum SerializerReturnCode serializer_api_get_feedback_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACK);

    struct CanPrimaryTsacmainboardfeedback *payload = &message->payload.tsacmainboardfeedback;
    // The relay commands are owned by the PCU, the "open" signals are the opposite of the "close commanded" conditions
    payload->airnopencom = mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED) ? 0U : 1U;
    payload->prechargeopencom = mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED) ? 0U : 1U;
    payload->airpopencom = mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED) ? 0U : 1U;
    payload->airnopenmec = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC);
    payload->prechargeopenmec = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC) ? 1U : 0U;
    payload->airpopenmec = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC);
    payload->tslessthan60v = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V) ? 1U : 0U;
    payload->plausiblestatelatched = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED) ? 1U : 0U;
    payload->plausiblestatepersisted = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED) ? 1U : 0U;
    payload->plausiblestate = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE) ? 1U : 0U;
    payload->plausiblestaterc = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC);
    payload->bmsfaultcockpitled = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED) ? 1U : 0U;
    payload->imdfaultcockpitled = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED) ? 1U : 0U;
    // There is no external fault cockpit LED feedback, the latch drives it
    payload->extfaultcockpitled = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED) ? 1U : 0U;
    payload->tsalgreen = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_TSAL_GREEN);
    return SERIALIZER_RC_OK;
}

// IMD

enum SerializerReturnCode serializer_api_get_imd_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDIMD);

    struct CanPrimaryTsacmainboardimd *payload = &message->payload.tsacmainboardimd;
    payload->status = prv_imd_status_to_can(sensing_api_get_imd_status());
    payload->frequency = sensing_api_get_imd_frequency();
    payload->dutycycle = sensing_api_get_imd_duty_cycle();
    payload->ok = mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_OK) ? 1U : 0U;
    return SERIALIZER_RC_OK;
}

// INTERNAL VOLTAGES

enum SerializerReturnCode serializer_api_get_internal_voltages_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVOLTAGEINFO);

    const struct InternalVoltagesInfo internal = internal_voltages_api_get_info();
    const struct VoltagesInfo cells = voltages_api_get_pack_info();
    struct CanPrimaryTsacmainboardvoltageinfo *payload = &message->payload.tsacmainboardvoltageinfo;
    payload->ts = internal.ts;
    payload->total = internal.pack;
    payload->cellsum = cells.sum;
    payload->average = cells.average;
    payload->min = cells.min;
    payload->max = cells.max;
    return SERIALIZER_RC_OK;
}

// SHUTDOWN

enum SerializerReturnCode serializer_api_get_shutdown_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDSHUTDOWN);

    struct CanPrimaryTsacmainboardshutdown *payload = &message->payload.tsacmainboardshutdown;
    payload->interlocktsacvoltage = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_SD_OUT);
    payload->amsvoltage = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_SD_AMS_FB);
    payload->imdstatus = (uint8_t)prv_imd_status_to_can(sensing_api_get_imd_status());
    payload->imdvoltage = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_SD_IMD_FB);
    payload->airnvoltage = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC);
    payload->airpvoltage = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC);
    // The precharge feedback is digital, it is reported as the full scale of the shutdown circuit when high
    payload->prechargevoltage = feedback_api_get_digital_value(FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC) ? FEEDBACK_SD_VREF : 0.F;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_feedback_shutdown_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFEEDBACKSHUTDOWN);

    struct CanPrimaryTsacmainboardfeedbackshutdown *payload = &message->payload.tsacmainboardfeedbackshutdown;
    payload->sdin = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_SD_IN);
    payload->sdend = prv_feedback_voltage(FEEDBACK_ANALOG_INDEX_SD_END);
    return SERIALIZER_RC_OK;
}

// TEMPERATURES

enum SerializerReturnCode serializer_api_get_temperatures_info_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDTEMPERATUREINFO);

    const struct TemperaturesInfo cells = temperatures_api_get_pack_cell_temperatures_info();
    struct CanPrimaryTsacmainboardtemperatureinfo *payload = &message->payload.tsacmainboardtemperatureinfo;
    payload->min = cells.min;
    payload->max = cells.max;
    payload->average = cells.average;
    payload->precharge = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_PRECHARGE);
    payload->linearregulator = internal_temperatures_api_get_value(INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR);
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_temperatures_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, temperatures_frame_ids[cellboard]);

    const uint8_t group = serializer_handler.temperatures_group[cellboard];
    serializer_handler.temperatures_group[cellboard] = (uint8_t)((group + 1U) % SERIALIZER_TEMPERATURES_GROUP_COUNT);

    // The last group can carry fewer temperatures than the others, the unused slots stay at 0
    float cells[SERIALIZER_TEMPERATURES_PER_GROUP] = { 0 };
    const size_t first = (size_t)group * SERIALIZER_TEMPERATURES_PER_GROUP;
    for (size_t i = 0U; i < SERIALIZER_TEMPERATURES_PER_GROUP && first + i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        cells[i] = temperatures_api_get_cell_temperature(cellboard, (uint8_t)(first + i));
    }

    // The payload is packed and can be unaligned, copy it instead of writing it through a float pointer
    struct CanPrimaryTsaccellboard1temperature *payload = &message->payload.tsaccellboard1temperature;
    payload->group = group;
    memcpy(&payload->group_payload, cells, sizeof(cells));
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_balancing_temperatures_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, balancing_temperatures_frame_ids[cellboard]);

    float resistors[CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT];
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT; ++i) {
        resistors[i] = temperatures_api_get_discharge_temperature(cellboard, (uint8_t)i);
    }

    // The payload is packed and can be unaligned, copy it instead of writing it through a float pointer
    memcpy(&message->payload.tsaccellboard1balancingtemperature, resistors, sizeof(resistors));
    return SERIALIZER_RC_OK;
}

// VERSION

enum SerializerReturnCode serializer_api_get_mainboard_version_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSION);

    struct CanPrimaryTsacmainboardversion *payload = &message->payload.tsacmainboardversion;
    payload->major = MAINBOARD_VERSION_MAJOR;
    payload->minor = MAINBOARD_VERSION_MINOR;
    payload->patch = MAINBOARD_VERSION_PATCH;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_mainboard_version_info_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDVERSIONINFO);

    struct CanPrimaryTsacmainboardversioninfo *payload = &message->payload.tsacmainboardversioninfo;
    payload->buildtime = serializer_handler.build_time;
    // TODO: send the commit hash and the dirty flag once the build injects them
    payload->commithash = 0U;
    payload->dirty = 0U;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_libcan_version_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSION);

    struct CanPrimaryTsacmainboardlibcanversion *payload = &message->payload.tsacmainboardlibcanversion;
    payload->major = (uint16_t)can_version_major;
    payload->minor = (uint16_t)can_version_minor;
    payload->patch = (uint16_t)can_version_patch;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_libcan_version_info_message(struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }

    prv_reset_message(message, CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDLIBCANVERSIONINFO);

    struct CanPrimaryTsacmainboardlibcanversioninfo *payload = &message->payload.tsacmainboardlibcanversioninfo;
    payload->generationtime = (uint32_t)can_generation_time;
    // TODO: send the libcan commit hash and dirty flag once can-version.h provides them
    payload->commithash = 0U;
    payload->dirty = 0U;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_version_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, cellboard_version_frame_ids[cellboard]);

    const struct CellboardsMonitorVersion version = cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_FIRMWARE);
    struct CanPrimaryTsaccellboard1version *payload = &message->payload.tsaccellboard1version;
    payload->major = version.major;
    payload->minor = version.minor;
    payload->patch = version.patch;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_version_info_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, cellboard_version_info_frame_ids[cellboard]);

    const struct CellboardsMonitorVersion version = cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_FIRMWARE);
    struct CanPrimaryTsaccellboard1versioninfo *payload = &message->payload.tsaccellboard1versioninfo;
    payload->buildtime = version.time;
    payload->commithash = version.commit_hash;
    payload->dirty = version.dirty ? 1U : 0U;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_libcan_version_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, cellboard_libcan_version_frame_ids[cellboard]);

    const struct CellboardsMonitorVersion version = cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_LIBCAN);
    struct CanPrimaryTsaccellboard1libcanversion *payload = &message->payload.tsaccellboard1libcanversion;
    payload->major = version.major;
    payload->minor = version.minor;
    payload->patch = version.patch;
    return SERIALIZER_RC_OK;
}

enum SerializerReturnCode serializer_api_get_cellboard_libcan_version_info_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, cellboard_libcan_version_info_frame_ids[cellboard]);

    const struct CellboardsMonitorVersion version = cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_LIBCAN);
    struct CanPrimaryTsaccellboard1libcanversioninfo *payload = &message->payload.tsaccellboard1libcanversioninfo;
    payload->generationtime = version.time;
    payload->commithash = version.commit_hash;
    payload->dirty = version.dirty ? 1U : 0U;
    return SERIALIZER_RC_OK;
}

// VOLTAGES

// TODO: add the voltages general info message once it exists in libcan

enum SerializerReturnCode serializer_api_get_cellboard_voltages_message(enum CellboardId cellboard, struct SerializerMessage *message) {
    if (message == NULL) {
        return SERIALIZER_RC_NULL_POINTER;
    }
    if (cellboard >= CELLBOARD_COUNT) {
        return SERIALIZER_RC_INVALID_CELLBOARD;
    }

    prv_reset_message(message, voltages_frame_ids[cellboard]);

    const uint8_t group = serializer_handler.voltages_group[cellboard];
    serializer_handler.voltages_group[cellboard] = (uint8_t)((group + 1U) % SERIALIZER_VOLTAGES_GROUP_COUNT);

    float cells[SERIALIZER_VOLTAGES_PER_GROUP] = { 0 };
    const size_t first = (size_t)group * SERIALIZER_VOLTAGES_PER_GROUP;
    for (size_t i = 0U; i < SERIALIZER_VOLTAGES_PER_GROUP && first + i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
        cells[i] = voltages_api_get_value(cellboard, (uint8_t)(first + i));
    }

    // The payload is packed and can be unaligned, copy it instead of writing it through a float pointer
    struct CanPrimaryTsaccellboard1voltage *payload = &message->payload.tsaccellboard1voltage;
    payload->group = group;
    memcpy(&payload->group_payload, cells, sizeof(cells));
    return SERIALIZER_RC_OK;
}
