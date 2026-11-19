/*!
 * \file cellboards-monitor.h
 * \date 2026-11-18
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the monitoring of the cellboards state, errors and versions
 */

#ifndef CELLBOARDS_MONITOR_H
#define CELLBOARDS_MONITOR_H

#include <stdbool.h>

#include "mainboard-def.h"

/*!
 * \brief Number of open wire checks reported by each cellboard
 */
#define CELLBOARDS_MONITOR_OPEN_WIRE_COUNT (13U)

/*!
 * \brief Return code for the cellboards monitor module functions
 */
enum CellboardsMonitorReturnCode {
    CELLBOARDS_MONITOR_RC_OK,                /*!< The function executed succesfully */
    CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD, /*!< The given cellboard does not exist */
    CELLBOARDS_MONITOR_RC_INVALID_STATE,     /*!< The given state does not exist */
    CELLBOARDS_MONITOR_RC_INVALID_ERROR,     /*!< The given error does not exist */
    CELLBOARDS_MONITOR_RC_INVALID_VERSION    /*!< The given version kind does not exist */
};

/*!
 * \brief Kind of version reported by a cellboard
 */
enum CellboardsMonitorVersionKind {
    CELLBOARDS_MONITOR_VERSION_FIRMWARE, /*!< The version of the cellboard firmware */
    CELLBOARDS_MONITOR_VERSION_LIBCAN,   /*!< The version of the libcan used by the cellboard firmware */
    CELLBOARDS_MONITOR_VERSION_COUNT     /*!< Number of version kinds */
};

/*!
 * \brief Version reported by a cellboard
 *
 * \details The cellboard sends the version number and the version info in two different
 * messages, so the two parts of this structure are updated independently
 */
struct CellboardsMonitorVersion {
    uint16_t major;       /*!< The major version number */
    uint16_t minor;       /*!< The minor version number */
    uint16_t patch;       /*!< The patch version number */
    uint32_t time;        /*!< The firmware build time or the libcan generation time, as a Unix timestamp */
    uint32_t commit_hash; /*!< The (truncated) commit hash */
    bool dirty;           /*!< True if built from a working tree with uncommitted changes */
};

/*!
 * \brief State of the FSM of a cellboard, as reported by the cellboard itself
 */
enum CellboardsMonitorState : uint8_t {
    CELLBOARDS_MONITOR_STATE_INIT,      /*!< The cellboard is initializing */
    CELLBOARDS_MONITOR_STATE_IDLE,      /*!< The cellboard is idle */
    CELLBOARDS_MONITOR_STATE_ERROR,     /*!< The cellboard is in its fatal error state */
    CELLBOARDS_MONITOR_STATE_FLASH,     /*!< The cellboard is being flashed */
    CELLBOARDS_MONITOR_STATE_DISCHARGE, /*!< The cellboard is discharging its cells */
    CELLBOARDS_MONITOR_STATE_COOLDOWN,  /*!< The cellboard is waiting for its discharge resistors to cool down */
    CELLBOARDS_MONITOR_STATE_COUNT      /*!< Number of states */
};

/*!
 * \brief Errors that a cellboard can report
 *
 * \details The open wire errors are kept separately, see CellboardsMonitorHandler::open_wires
 */
enum CellboardsMonitorError {
    CELLBOARDS_MONITOR_ERROR_POST,                                 /*!< The Power-On Self Test failed */
    CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE,                   /*!< A cell voltage is below the minimum */
    CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE,                    /*!< A cell voltage is above the maximum */
    CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE,               /*!< A cell temperature is below the minimum */
    CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE,                /*!< A cell temperature is above the maximum */
    CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE, /*!< A discharge resistor temperature is below the minimum */
    CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE,  /*!< A discharge resistor temperature is above the maximum */
    CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION,                    /*!< The cellboard has CAN communication problems */
    CELLBOARDS_MONITOR_ERROR_FLASH,                                /*!< The cellboard flash procedure failed */
    CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION,            /*!< The cellboard cannot communicate with its BMS monitors */
    CELLBOARDS_MONITOR_ERROR_COUNT                                 /*!< Number of errors */
};

static_assert(CELLBOARDS_MONITOR_ERROR_COUNT <= 16U, "The errors must fit in a bit_flag16");
static_assert(CELLBOARDS_MONITOR_OPEN_WIRE_COUNT <= 16U, "The open wires must fit in a bit_flag16");

/*!
 * \brief Structure to hold the state and the errors of each cellboard
 */
struct CellboardsMonitorHandler {
    enum CellboardsMonitorState states[CELLBOARD_COUNT]; /*!< The FSM state of each cellboard */
    volatile bool is_received_state[CELLBOARD_COUNT];    /*!< Indicates if the state of a specific cellboard has been received at least once, never reset */

    bit_flag16 errors[CELLBOARD_COUNT];                /*!< The errors of each cellboard, bit N for CellboardsMonitorError N */
    bit_flag16 open_wires[CELLBOARD_COUNT];            /*!< The open wire errors of each cellboard, bit N for open wire N + 1 */
    volatile bool is_received_errors[CELLBOARD_COUNT]; /*!< Indicates if the errors of a specific cellboard have been received at least once, never reset */

    struct CellboardsMonitorVersion versions[CELLBOARD_COUNT][CELLBOARDS_MONITOR_VERSION_COUNT]; /*!< The versions reported by each cellboard */
};

#endif // CELLBOARDS_MONITOR_H
