/*!
 * \file can-communication-router-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Routing of the received CAN frames to the modules that handle them
 *
 * \details BMS network: libcan generates a distinct struct for each cellboard (TsacCellboard1Voltage,
 * TsacCellboard2Voltage, ...) but all of them share the same packed layout and live at
 * offset 0 of the same messages union. Every per-cellboard message is therefore read through
 * the cellboard 1 member of the union by a single handler that takes the cellboard as a
 * parameter, and a route table maps each frame ID to its handler and cellboard. Messages not sent
 * by a cellboard, such as the IVT current, live in the same table with CELLBOARD_ID_COUNT as cellboard.
 * The static assertions below break the build if a libcan update makes the layouts diverge.
 *
 * Primary network: the few messages addressed to the mainboard are handled by a switch,
 * and the flashing messages of every board are forwarded as flash requests.
 */

#include "can-communication-router-api.h"

#include <stddef.h>
#include <string.h>

#include "eagletrt.h"
#include "can-bms-api.h"
#include "can-bms.h"
#include "can-primary-api.h"
#include "can-primary.h"
#include "flash-control.h"
#include "mainboard-def.h"
#include "voltages-api.h"
#include "temperatures-api.h"
#include "internal-voltages-api.h"
#include "bal-control-api.h"
#include "cellboards-monitor-api.h"
#include "request-control-api.h"
#include "eagletrt-api.h"

// Every cellboard message must have the same layout as the cellboard 1 one
static_assert(sizeof(struct CanBmsTsaccellboard2voltage) == sizeof(struct CanBmsTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3voltage) == sizeof(struct CanBmsTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4voltage) == sizeof(struct CanBmsTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5voltage) == sizeof(struct CanBmsTsaccellboard1voltage), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6voltage) == sizeof(struct CanBmsTsaccellboard1voltage), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2voltageinfo) == sizeof(struct CanBmsTsaccellboard1voltageinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3voltageinfo) == sizeof(struct CanBmsTsaccellboard1voltageinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4voltageinfo) == sizeof(struct CanBmsTsaccellboard1voltageinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5voltageinfo) == sizeof(struct CanBmsTsaccellboard1voltageinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6voltageinfo) == sizeof(struct CanBmsTsaccellboard1voltageinfo), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2temperature) == sizeof(struct CanBmsTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3temperature) == sizeof(struct CanBmsTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4temperature) == sizeof(struct CanBmsTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5temperature) == sizeof(struct CanBmsTsaccellboard1temperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6temperature) == sizeof(struct CanBmsTsaccellboard1temperature), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2temperatureinfo) == sizeof(struct CanBmsTsaccellboard1temperatureinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3temperatureinfo) == sizeof(struct CanBmsTsaccellboard1temperatureinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4temperatureinfo) == sizeof(struct CanBmsTsaccellboard1temperatureinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5temperatureinfo) == sizeof(struct CanBmsTsaccellboard1temperatureinfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6temperatureinfo) == sizeof(struct CanBmsTsaccellboard1temperatureinfo), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2balancing) == sizeof(struct CanBmsTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3balancing) == sizeof(struct CanBmsTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4balancing) == sizeof(struct CanBmsTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5balancing) == sizeof(struct CanBmsTsaccellboard1balancing), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6balancing) == sizeof(struct CanBmsTsaccellboard1balancing), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2balancingtemperature) == sizeof(struct CanBmsTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3balancingtemperature) == sizeof(struct CanBmsTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4balancingtemperature) == sizeof(struct CanBmsTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5balancingtemperature) == sizeof(struct CanBmsTsaccellboard1balancingtemperature), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6balancingtemperature) == sizeof(struct CanBmsTsaccellboard1balancingtemperature), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2fsm) == sizeof(struct CanBmsTsaccellboard1fsm), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3fsm) == sizeof(struct CanBmsTsaccellboard1fsm), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4fsm) == sizeof(struct CanBmsTsaccellboard1fsm), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5fsm) == sizeof(struct CanBmsTsaccellboard1fsm), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6fsm) == sizeof(struct CanBmsTsaccellboard1fsm), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2error) == sizeof(struct CanBmsTsaccellboard1error), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3error) == sizeof(struct CanBmsTsaccellboard1error), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4error) == sizeof(struct CanBmsTsaccellboard1error), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5error) == sizeof(struct CanBmsTsaccellboard1error), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6error) == sizeof(struct CanBmsTsaccellboard1error), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2version) == sizeof(struct CanBmsTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3version) == sizeof(struct CanBmsTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4version) == sizeof(struct CanBmsTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5version) == sizeof(struct CanBmsTsaccellboard1version), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6version) == sizeof(struct CanBmsTsaccellboard1version), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2versioninfo) == sizeof(struct CanBmsTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3versioninfo) == sizeof(struct CanBmsTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4versioninfo) == sizeof(struct CanBmsTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5versioninfo) == sizeof(struct CanBmsTsaccellboard1versioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6versioninfo) == sizeof(struct CanBmsTsaccellboard1versioninfo), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2libcanversion) == sizeof(struct CanBmsTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3libcanversion) == sizeof(struct CanBmsTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4libcanversion) == sizeof(struct CanBmsTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5libcanversion) == sizeof(struct CanBmsTsaccellboard1libcanversion), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6libcanversion) == sizeof(struct CanBmsTsaccellboard1libcanversion), "Layout mismatch");

static_assert(sizeof(struct CanBmsTsaccellboard2libcanversioninfo) == sizeof(struct CanBmsTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard3libcanversioninfo) == sizeof(struct CanBmsTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard4libcanversioninfo) == sizeof(struct CanBmsTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard5libcanversioninfo) == sizeof(struct CanBmsTsaccellboard1libcanversioninfo), "Layout mismatch");
static_assert(sizeof(struct CanBmsTsaccellboard6libcanversioninfo) == sizeof(struct CanBmsTsaccellboard1libcanversioninfo), "Layout mismatch");

/*! \brief Number of cell voltages carried by a single TsacCellboardNVoltage group */
#define CAN_ROUTER_VOLTAGES_PER_GROUP (sizeof(struct CanBmsTsaccellboard1voltageGroupMux0) / sizeof(float))

/*! \brief Number of cell temperatures carried by a single TsacCellboardNTemperature group */
#define CAN_ROUTER_TEMPERATURES_PER_GROUP (sizeof(struct CanBmsTsaccellboard1temperatureGroupMux0) / sizeof(float))

// The group payloads are copied as plain float arrays: the first group must be the largest one
// and contain nothing but floats
static_assert(sizeof(union CanBmsTsaccellboard1voltageGroupPayload) == CAN_ROUTER_VOLTAGES_PER_GROUP * sizeof(float), "Unexpected voltage group layout");
static_assert(sizeof(union CanBmsTsaccellboard1temperatureGroupPayload) == CAN_ROUTER_TEMPERATURES_PER_GROUP * sizeof(float), "Unexpected temperature group layout");

/*! \brief Number of discharge resistor temperatures carried by a TsacCellboardNBalancingTemperature message */
#define CAN_ROUTER_DISCHARGE_TEMPERATURES_COUNT (sizeof(struct CanBmsTsaccellboard1balancingtemperature) / sizeof(float))

// The discharge resistor temperatures are copied as a plain float array: the message must contain nothing but them
static_assert(CAN_ROUTER_DISCHARGE_TEMPERATURES_COUNT == CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT, "Unexpected balancing temperature layout");

/*! \brief Size in bytes of the cell flags of a TsacCellboardNBalancing message, one bit per cell */
#define CAN_ROUTER_BALANCING_FLAG_SIZE ((CELLBOARD_SEGMENT_SERIES_COUNT + 7U) / 8U)

// The cell flags are copied as raw bytes into a 32-bit flag: they must fit and fill the message
static_assert(CAN_ROUTER_BALANCING_FLAG_SIZE <= sizeof(uint32_t), "Too many cells for the balancing flag");
static_assert(sizeof(struct CanBmsTsaccellboard1balancing) == CAN_ROUTER_BALANCING_FLAG_SIZE, "Unexpected balancing layout");

/*!
 * \brief Forward one group of cell voltages to the voltages module
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_voltage(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1voltage *message = &messages->tsaccellboard1voltage;

    // The payload is packed and can be unaligned, copy it instead of reading it through a float pointer
    float cells[CAN_ROUTER_VOLTAGES_PER_GROUP];
    memcpy(cells, &message->group_payload, sizeof(cells));

    const size_t first = (size_t)message->group * CAN_ROUTER_VOLTAGES_PER_GROUP;
    for (size_t i = 0U; i < CAN_ROUTER_VOLTAGES_PER_GROUP && first + i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
        voltages_api_set_value(cellboard, (uint8_t)(first + i), cells[i]);
    }
}

/*!
 * \brief Forward one group of cell temperatures to the temperatures module
 *
 * \details The last group can carry fewer temperatures than the others; the unused
 * slots fall beyond the sensor count and are skipped
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_temperature(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1temperature *message = &messages->tsaccellboard1temperature;

    // The payload is packed and can be unaligned, copy it instead of reading it through a float pointer
    float cells[CAN_ROUTER_TEMPERATURES_PER_GROUP];
    memcpy(cells, &message->group_payload, sizeof(cells));

    const size_t first = (size_t)message->group * CAN_ROUTER_TEMPERATURES_PER_GROUP;
    for (size_t i = 0U; i < CAN_ROUTER_TEMPERATURES_PER_GROUP && first + i < CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT; ++i) {
        temperatures_api_set_cell_temperature(cellboard, (uint8_t)(first + i), cells[i]);
    }
}

/*!
 * \brief Forward the voltage summary of a cellboard to the voltages module
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_voltage_info(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1voltageinfo *message = &messages->tsaccellboard1voltageinfo;
    voltages_api_set_cellboard_voltage_info(cellboard, message->min, message->max, message->average, message->total);
}

/*!
 * \brief Forward the temperature summary of a cellboard to the temperatures module
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_temperature_info(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1temperatureinfo *message = &messages->tsaccellboard1temperatureinfo;
    temperatures_api_set_cell_temperatures_info(cellboard, message->min, message->max, message->average);
}

/*!
 * \brief Forward the discharge resistor temperatures of a cellboard to the temperatures module
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_balancing_temperature(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    // The message is packed and can be unaligned, copy it instead of reading it through a float pointer
    float resistors[CAN_ROUTER_DISCHARGE_TEMPERATURES_COUNT];
    memcpy(resistors, &messages->tsaccellboard1balancingtemperature, sizeof(resistors));

    for (size_t i = 0U; i < CAN_ROUTER_DISCHARGE_TEMPERATURES_COUNT; ++i) {
        (void)temperatures_api_set_discharge_temperature(cellboard, (uint8_t)i, resistors[i]);
    }
}

/*!
 * \brief Forward the discharging status of the cells of a cellboard to the balancing control
 *
 * \details Bit N of the flag is set when cell N + 1 is discharging
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_balancing(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    // The cell flags are packed 1-bit bitfields; on GCC little-endian targets cell1 is bit 0
    // of the first byte, which is already the layout of the balancing status flag
    uint32_t cells = 0U;
    memcpy(&cells, &messages->tsaccellboard1balancing, CAN_ROUTER_BALANCING_FLAG_SIZE);
    bal_control_api_set_balancing_status_flag(cellboard, cells);
}

/*!
 * \brief Forward the FSM state of a cellboard to the cellboards monitor
 *
 * \details Unknown states are forwarded as CELLBOARDS_MONITOR_STATE_COUNT, which the monitor rejects
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_fsm(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    enum CellboardsMonitorState state = CELLBOARDS_MONITOR_STATE_COUNT;

    switch ((enum CanBmsTsaccellboard1fsmStatus)messages->tsaccellboard1fsm.status) {
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_INIT:
            state = CELLBOARDS_MONITOR_STATE_INIT;
            break;
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_IDLE:
            state = CELLBOARDS_MONITOR_STATE_IDLE;
            break;
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_ERROR:
            state = CELLBOARDS_MONITOR_STATE_ERROR;
            break;
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_FLASH:
            state = CELLBOARDS_MONITOR_STATE_FLASH;
            break;
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_DISCHARGE:
            state = CELLBOARDS_MONITOR_STATE_DISCHARGE;
            break;
        case CAN_BMS_TSACCELLBOARD1FSM_STATUS_COOLDOWN:
            state = CELLBOARDS_MONITOR_STATE_COOLDOWN;
            break;
        default:
            break;
    }

    (void)cellboards_monitor_api_set_state(cellboard, state);
}

/*!
 * \brief Forward the errors of a cellboard to the cellboards monitor
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_error(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1error *message = &messages->tsaccellboard1error;

    bit_flag16 errors = 0U;
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_POST, message->post);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE, message->cellundervoltage);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE, message->cellovervoltage);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE, message->cellundertemperature);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE, message->cellovertemperature);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE, message->dischargeresistorundertemperature);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE, message->dischargeresistorovertemperature);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION, message->cancommunication);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_FLASH, message->flash);
    errors = EAGLETRT_API_BIT_SET_IF(errors, CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION, message->bmsmonitorcommunication);

    // Bit N is open wire N + 1
    bit_flag16 open_wires = 0U;
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 0U, message->openwire1);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 1U, message->openwire2);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 2U, message->openwire3);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 3U, message->openwire4);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 4U, message->openwire5);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 5U, message->openwire6);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 6U, message->openwire7);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 7U, message->openwire8);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 8U, message->openwire9);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 9U, message->openwire10);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 10U, message->openwire11);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 11U, message->openwire12);
    open_wires = EAGLETRT_API_BIT_SET_IF(open_wires, 12U, message->openwire13);

    (void)cellboards_monitor_api_set_errors(cellboard, errors, open_wires);
}

/*!
 * \brief Forward the firmware version number of a cellboard to the cellboards monitor
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_version(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1version *message = &messages->tsaccellboard1version;
    (void)cellboards_monitor_api_set_version(cellboard, CELLBOARDS_MONITOR_VERSION_FIRMWARE, message->major, message->minor, message->patch);
}

/*!
 * \brief Forward the firmware version info of a cellboard to the cellboards monitor
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_version_info(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1versioninfo *message = &messages->tsaccellboard1versioninfo;
    (void)cellboards_monitor_api_set_version_info(cellboard, CELLBOARDS_MONITOR_VERSION_FIRMWARE, message->buildtime, message->commithash, message->dirty != 0U);
}

/*!
 * \brief Forward the libcan version number of a cellboard to the cellboards monitor
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_libcan_version(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1libcanversion *message = &messages->tsaccellboard1libcanversion;
    (void)cellboards_monitor_api_set_version(cellboard, CELLBOARDS_MONITOR_VERSION_LIBCAN, message->major, message->minor, message->patch);
}

/*!
 * \brief Forward the libcan version info of a cellboard to the cellboards monitor
 *
 * \param[in] cellboard The cellboard that sent the message
 * \param[in] messages The deserialized message, read through its cellboard 1 member
 */
EAGLETRT_STATIC void prv_handle_libcan_version_info(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    const struct CanBmsTsaccellboard1libcanversioninfo *message = &messages->tsaccellboard1libcanversioninfo;
    (void)cellboards_monitor_api_set_version_info(cellboard, CELLBOARDS_MONITOR_VERSION_LIBCAN, message->generationtime, message->commithash, message->dirty != 0U);
}

/*!
 * \brief Forward the current measured by the IVT to the internal voltages module
 *
 * \param[in] cellboard Unused, the message is not sent by a cellboard
 * \param[in] messages The deserialized message
 */
EAGLETRT_STATIC void prv_handle_current(enum CellboardId cellboard, const union CanBmsMessages *messages) {
    (void)cellboard;

    constexpr ampere ampere_per_milliampere = 0.001F;
    // TODO: handle the other fields of this message, which are not used yet
    const milliampere current = (milliampere)messages->ivtmessageresulti.current;
    internal_voltages_api_set_current(current * ampere_per_milliampere);
}

/*!
 * \brief Forward a request to the request control
 *
 * \param[in] request The request to forward
 *
 * \retval CAN_COMMUNICATION_RC_ERROR if the request control rejected the request
 * \retval CAN_COMMUNICATION_RC_OK otherwise
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_forward_request(struct RequestControlRequest request) {
    if (request_control_api_handle_request(request) != REQUEST_CONTROL_RC_OK) {
        return CAN_COMMUNICATION_RC_ERROR;
    }
    return CAN_COMMUNICATION_RC_OK;
}

/*!
 * \brief Handler of a BMS network message
 *
 * \param[in] cellboard The cellboard that sent the message, CELLBOARD_ID_COUNT if not sent by a cellboard
 * \param[in] messages The deserialized message
 */
typedef void (*can_communication_router_handler)(enum CellboardId cellboard, const union CanBmsMessages *messages);

/*!
 * \brief Association between a BMS network frame ID, its sender cellboard and its handler
 */
struct CanCommunicationRouterRoute {
    enum CanBmsMessageFrameId id;            /*!< The frame ID of the message */
    uint8_t length;                          /*!< The expected length of the frame payload, in bytes */
    enum CellboardId cellboard;              /*!< The cellboard that sends the message, CELLBOARD_ID_COUNT if none */
    can_communication_router_handler handle; /*!< The function that handles the message */
};

/*!
 * \brief Routes of the messages received on the BMS network
 */
EAGLETRT_STATIC const struct CanCommunicationRouterRoute bms_routes[] = {
    { CAN_BMS_MESSAGE_FRAME_ID_IVTMESSAGERESULTI,                  can_bms_byte_size_ivtmessageresulti,                  CELLBOARD_ID_COUNT, prv_handle_current               },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGE,              can_bms_byte_size_tsaccellboard1voltage,              CELLBOARD_ID_0,     prv_handle_voltage               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGE,              can_bms_byte_size_tsaccellboard2voltage,              CELLBOARD_ID_1,     prv_handle_voltage               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGE,              can_bms_byte_size_tsaccellboard3voltage,              CELLBOARD_ID_2,     prv_handle_voltage               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGE,              can_bms_byte_size_tsaccellboard4voltage,              CELLBOARD_ID_3,     prv_handle_voltage               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGE,              can_bms_byte_size_tsaccellboard5voltage,              CELLBOARD_ID_4,     prv_handle_voltage               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGE,              can_bms_byte_size_tsaccellboard6voltage,              CELLBOARD_ID_5,     prv_handle_voltage               },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VOLTAGEINFO,          can_bms_byte_size_tsaccellboard1voltageinfo,          CELLBOARD_ID_0,     prv_handle_voltage_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VOLTAGEINFO,          can_bms_byte_size_tsaccellboard2voltageinfo,          CELLBOARD_ID_1,     prv_handle_voltage_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VOLTAGEINFO,          can_bms_byte_size_tsaccellboard3voltageinfo,          CELLBOARD_ID_2,     prv_handle_voltage_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VOLTAGEINFO,          can_bms_byte_size_tsaccellboard4voltageinfo,          CELLBOARD_ID_3,     prv_handle_voltage_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VOLTAGEINFO,          can_bms_byte_size_tsaccellboard5voltageinfo,          CELLBOARD_ID_4,     prv_handle_voltage_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VOLTAGEINFO,          can_bms_byte_size_tsaccellboard6voltageinfo,          CELLBOARD_ID_5,     prv_handle_voltage_info          },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATURE,          can_bms_byte_size_tsaccellboard1temperature,          CELLBOARD_ID_0,     prv_handle_temperature           },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATURE,          can_bms_byte_size_tsaccellboard2temperature,          CELLBOARD_ID_1,     prv_handle_temperature           },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATURE,          can_bms_byte_size_tsaccellboard3temperature,          CELLBOARD_ID_2,     prv_handle_temperature           },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATURE,          can_bms_byte_size_tsaccellboard4temperature,          CELLBOARD_ID_3,     prv_handle_temperature           },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATURE,          can_bms_byte_size_tsaccellboard5temperature,          CELLBOARD_ID_4,     prv_handle_temperature           },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATURE,          can_bms_byte_size_tsaccellboard6temperature,          CELLBOARD_ID_5,     prv_handle_temperature           },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard1temperatureinfo,      CELLBOARD_ID_0,     prv_handle_temperature_info      },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard2temperatureinfo,      CELLBOARD_ID_1,     prv_handle_temperature_info      },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard3temperatureinfo,      CELLBOARD_ID_2,     prv_handle_temperature_info      },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard4temperatureinfo,      CELLBOARD_ID_3,     prv_handle_temperature_info      },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard5temperatureinfo,      CELLBOARD_ID_4,     prv_handle_temperature_info      },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6TEMPERATUREINFO,      can_bms_byte_size_tsaccellboard6temperatureinfo,      CELLBOARD_ID_5,     prv_handle_temperature_info      },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard1balancingtemperature, CELLBOARD_ID_0,     prv_handle_balancing_temperature },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard2balancingtemperature, CELLBOARD_ID_1,     prv_handle_balancing_temperature },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard3balancingtemperature, CELLBOARD_ID_2,     prv_handle_balancing_temperature },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard4balancingtemperature, CELLBOARD_ID_3,     prv_handle_balancing_temperature },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard5balancingtemperature, CELLBOARD_ID_4,     prv_handle_balancing_temperature },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCINGTEMPERATURE, can_bms_byte_size_tsaccellboard6balancingtemperature, CELLBOARD_ID_5,     prv_handle_balancing_temperature },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1BALANCING,            can_bms_byte_size_tsaccellboard1balancing,            CELLBOARD_ID_0,     prv_handle_balancing             },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2BALANCING,            can_bms_byte_size_tsaccellboard2balancing,            CELLBOARD_ID_1,     prv_handle_balancing             },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3BALANCING,            can_bms_byte_size_tsaccellboard3balancing,            CELLBOARD_ID_2,     prv_handle_balancing             },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4BALANCING,            can_bms_byte_size_tsaccellboard4balancing,            CELLBOARD_ID_3,     prv_handle_balancing             },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5BALANCING,            can_bms_byte_size_tsaccellboard5balancing,            CELLBOARD_ID_4,     prv_handle_balancing             },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6BALANCING,            can_bms_byte_size_tsaccellboard6balancing,            CELLBOARD_ID_5,     prv_handle_balancing             },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1FSM,                  can_bms_byte_size_tsaccellboard1fsm,                  CELLBOARD_ID_0,     prv_handle_fsm                   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2FSM,                  can_bms_byte_size_tsaccellboard2fsm,                  CELLBOARD_ID_1,     prv_handle_fsm                   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3FSM,                  can_bms_byte_size_tsaccellboard3fsm,                  CELLBOARD_ID_2,     prv_handle_fsm                   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4FSM,                  can_bms_byte_size_tsaccellboard4fsm,                  CELLBOARD_ID_3,     prv_handle_fsm                   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5FSM,                  can_bms_byte_size_tsaccellboard5fsm,                  CELLBOARD_ID_4,     prv_handle_fsm                   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6FSM,                  can_bms_byte_size_tsaccellboard6fsm,                  CELLBOARD_ID_5,     prv_handle_fsm                   },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1ERROR,                can_bms_byte_size_tsaccellboard1error,                CELLBOARD_ID_0,     prv_handle_error                 },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2ERROR,                can_bms_byte_size_tsaccellboard2error,                CELLBOARD_ID_1,     prv_handle_error                 },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3ERROR,                can_bms_byte_size_tsaccellboard3error,                CELLBOARD_ID_2,     prv_handle_error                 },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4ERROR,                can_bms_byte_size_tsaccellboard4error,                CELLBOARD_ID_3,     prv_handle_error                 },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5ERROR,                can_bms_byte_size_tsaccellboard5error,                CELLBOARD_ID_4,     prv_handle_error                 },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6ERROR,                can_bms_byte_size_tsaccellboard6error,                CELLBOARD_ID_5,     prv_handle_error                 },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSION,              can_bms_byte_size_tsaccellboard1version,              CELLBOARD_ID_0,     prv_handle_version               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSION,              can_bms_byte_size_tsaccellboard2version,              CELLBOARD_ID_1,     prv_handle_version               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSION,              can_bms_byte_size_tsaccellboard3version,              CELLBOARD_ID_2,     prv_handle_version               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSION,              can_bms_byte_size_tsaccellboard4version,              CELLBOARD_ID_3,     prv_handle_version               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSION,              can_bms_byte_size_tsaccellboard5version,              CELLBOARD_ID_4,     prv_handle_version               },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSION,              can_bms_byte_size_tsaccellboard6version,              CELLBOARD_ID_5,     prv_handle_version               },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1VERSIONINFO,          can_bms_byte_size_tsaccellboard1versioninfo,          CELLBOARD_ID_0,     prv_handle_version_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2VERSIONINFO,          can_bms_byte_size_tsaccellboard2versioninfo,          CELLBOARD_ID_1,     prv_handle_version_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3VERSIONINFO,          can_bms_byte_size_tsaccellboard3versioninfo,          CELLBOARD_ID_2,     prv_handle_version_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4VERSIONINFO,          can_bms_byte_size_tsaccellboard4versioninfo,          CELLBOARD_ID_3,     prv_handle_version_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5VERSIONINFO,          can_bms_byte_size_tsaccellboard5versioninfo,          CELLBOARD_ID_4,     prv_handle_version_info          },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6VERSIONINFO,          can_bms_byte_size_tsaccellboard6versioninfo,          CELLBOARD_ID_5,     prv_handle_version_info          },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSION,        can_bms_byte_size_tsaccellboard1libcanversion,        CELLBOARD_ID_0,     prv_handle_libcan_version        },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSION,        can_bms_byte_size_tsaccellboard2libcanversion,        CELLBOARD_ID_1,     prv_handle_libcan_version        },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSION,        can_bms_byte_size_tsaccellboard3libcanversion,        CELLBOARD_ID_2,     prv_handle_libcan_version        },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSION,        can_bms_byte_size_tsaccellboard4libcanversion,        CELLBOARD_ID_3,     prv_handle_libcan_version        },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSION,        can_bms_byte_size_tsaccellboard5libcanversion,        CELLBOARD_ID_4,     prv_handle_libcan_version        },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSION,        can_bms_byte_size_tsaccellboard6libcanversion,        CELLBOARD_ID_5,     prv_handle_libcan_version        },

    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD1LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard1libcanversioninfo,    CELLBOARD_ID_0,     prv_handle_libcan_version_info   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD2LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard2libcanversioninfo,    CELLBOARD_ID_1,     prv_handle_libcan_version_info   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD3LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard3libcanversioninfo,    CELLBOARD_ID_2,     prv_handle_libcan_version_info   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD4LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard4libcanversioninfo,    CELLBOARD_ID_3,     prv_handle_libcan_version_info   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD5LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard5libcanversioninfo,    CELLBOARD_ID_4,     prv_handle_libcan_version_info   },
    { CAN_BMS_MESSAGE_FRAME_ID_TSACCELLBOARD6LIBCANVERSIONINFO,    can_bms_byte_size_tsaccellboard6libcanversioninfo,    CELLBOARD_ID_5,     prv_handle_libcan_version_info   },
};

/*!
 * \brief Find the route of a per-cellboard message
 *
 * \param[in] id The frame ID of the message
 *
 * \return The route of the message, \c NULL if the message is not routed
 */
EAGLETRT_STATIC const struct CanCommunicationRouterRoute *prv_find_bms_route(uint32_t id) {
    for (size_t i = 0U; i < sizeof(bms_routes) / sizeof(bms_routes[0]); ++i) {
        if (bms_routes[i].id == id) {
            return &bms_routes[i];
        }
    }
    return nullptr;
}

enum CanCommunicationReturnCode can_communication_router_api_receive_bms(struct CanCommunicationFrame *frame) {
    if (frame == nullptr) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!can_bms_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    const struct CanCommunicationRouterRoute *route = prv_find_bms_route(frame->id);
    if (route == nullptr) {
        // Valid BMS message that no module handles
        return CAN_COMMUNICATION_RC_OK;
    }

    // A length mismatch means the sender uses a different libcan version: decoding the payload would give garbage
    if (frame->length != route->length) {
        return CAN_COMMUNICATION_RC_INVALID_LENGTH;
    }

    union CanBmsMessages message = { 0 };
    if (can_bms_api_deserialize_from_id(frame->id, frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    route->handle(route->cellboard, &message);
    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_communication_router_api_receive_primary(struct CanCommunicationFrame *frame) {
    if (frame == nullptr) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (!can_primary_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    // The length is checked only for the messages whose payload is read, see the switch below:
    // a mismatch means the sender uses a different libcan version
    union CanPrimaryMessages message = { 0 };
    if (can_primary_api_deserialize_from_id(frame->id, frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    switch (frame->id) {
        case CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDFLASHINGRX: {
            const struct RequestControlRequest request = {
                .request_type = REQUEST_CONTROL_REQUEST_FLASH,
                .request_data = { .flash_request_state = FLASH_CONTROL_REQUEST_MAINBOARD },
            };
            return prv_forward_request(request);
        }

        case CAN_PRIMARY_MESSAGE_FRAME_ID_BMSSET: {
            if (frame->length != can_primary_byte_size_bmsset) {
                return CAN_COMMUNICATION_RC_INVALID_LENGTH;
            }

            // Anything that is not an explicit "on" turns the TS off
            const struct RequestControlRequest request = {
                .request_type = REQUEST_CONTROL_REQUEST_TS,
                .request_data = {
                                 .ts_request_state = (message.bmsset.status == 1U)
                                            ? REQUEST_CONTROL_TS_REQUEST_STATE_ON
                                            : REQUEST_CONTROL_TS_REQUEST_STATE_OFF,
                                 }, // TODO: modify once the canlib is updated
            };
            return prv_forward_request(request);
        }
        case CAN_PRIMARY_MESSAGE_FRAME_ID_RASPBERRYTSACBALANCINGSET: {
            if (frame->length != can_primary_byte_size_raspberrytsacbalancingset) {
                return CAN_COMMUNICATION_RC_INVALID_LENGTH;
            }

            const struct RequestControlRequest request = {
                .request_type = REQUEST_CONTROL_REQUEST_BALANCING,
                .request_data = {
                                 .balancing_request_data = {
                        .balancing_request_state = (message.raspberrytsacbalancingset.start != 0U),
                        .threshold = message.raspberrytsacbalancingset.threshold,
                    },
                                 },
            };
            return prv_forward_request(request);
        }
        default:

            if (frame->id >= CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD1FLASHINGTX && frame->id <= CAN_PRIMARY_MESSAGE_FRAME_ID_TSACCELLBOARD6FLASHINGRX) {
                const struct RequestControlRequest request = {
                    .request_type = REQUEST_CONTROL_REQUEST_FLASH,
                    .request_data = { .flash_request_state = FLASH_CONTROL_REQUEST_CELLBOARD },
                };
                return prv_forward_request(request);
            } else if (frame->id >= CAN_PRIMARY_MESSAGE_FRAME_ID_ECUFLASHINGTX && frame->id <= CAN_PRIMARY_MESSAGE_FRAME_ID_STEERINGBOARDFLASHINGRX) {
                const struct RequestControlRequest request = {
                    .request_type = REQUEST_CONTROL_REQUEST_FLASH,
                    .request_data = { .flash_request_state = FLASH_CONTROL_REQUEST_GENERAL },
                };
                return prv_forward_request(request);
            }

            break;
    }

    return CAN_COMMUNICATION_RC_OK;
}
