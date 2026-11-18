/*!
 * \file cellboards-monitor-api.c
 * \date 2026-11-18
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Monitoring of the cellboards state and errors
 */

#include "cellboards-monitor-api.h"

#include <string.h>

#include "eagletrt.h"
#include "eagletrt-api.h"

/*!
 * \brief The global cellboards monitor handler
 */
EAGLETRT_STATIC struct CellboardsMonitorHandler cellboards_monitor_handler;

/*!
 * \brief Mask of the valid bits of the errors flag
 */
#define CELLBOARDS_MONITOR_ERRORS_MASK ((bit_flag16)((1UL << CELLBOARDS_MONITOR_ERROR_COUNT) - 1UL))

/*!
 * \brief Mask of the valid bits of the open wires flag
 */
#define CELLBOARDS_MONITOR_OPEN_WIRES_MASK ((bit_flag16)((1UL << CELLBOARDS_MONITOR_OPEN_WIRE_COUNT) - 1UL))

enum CellboardsMonitorReturnCode cellboards_monitor_api_init(void) {
    memset((void *)&cellboards_monitor_handler, 0, sizeof(cellboards_monitor_handler));

    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        cellboards_monitor_handler.states[cellboard] = CELLBOARDS_MONITOR_STATE_INIT;
    }

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_set_state(enum CellboardId cellboard, enum CellboardsMonitorState state) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
    }
    if (state >= CELLBOARDS_MONITOR_STATE_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_STATE;
    }

    cellboards_monitor_handler.states[cellboard] = state;
    cellboards_monitor_handler.is_updated_state[cellboard] = true;

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_set_errors(enum CellboardId cellboard, bit_flag16 errors, bit_flag16 open_wires) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
    }

    // Drop the bits that do not correspond to any error so they cannot report a fault
    cellboards_monitor_handler.errors[cellboard] = errors & CELLBOARDS_MONITOR_ERRORS_MASK;
    cellboards_monitor_handler.open_wires[cellboard] = open_wires & CELLBOARDS_MONITOR_OPEN_WIRES_MASK;
    cellboards_monitor_handler.is_updated_errors[cellboard] = true;

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_reset_state_updated_flag(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        cellboards_monitor_handler.is_updated_state[cellboard] = false;
        return CELLBOARDS_MONITOR_RC_OK;
    }
    return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_reset_errors_updated_flag(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        cellboards_monitor_handler.is_updated_errors[cellboard] = false;
        return CELLBOARDS_MONITOR_RC_OK;
    }
    return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
}

enum CellboardsMonitorState cellboards_monitor_api_get_state(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_STATE_COUNT;
    }
    return cellboards_monitor_handler.states[cellboard];
}

bool cellboards_monitor_api_is_error_set(enum CellboardId cellboard, enum CellboardsMonitorError error) {
    if (cellboard >= CELLBOARD_COUNT || error >= CELLBOARDS_MONITOR_ERROR_COUNT) {
        return false;
    }
    return (bool)EAGLETRT_API_BIT_GET(cellboards_monitor_handler.errors[cellboard], error);
}

bit_flag16 cellboards_monitor_api_get_errors(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_COUNT) {
        return 0U;
    }
    return cellboards_monitor_handler.errors[cellboard];
}

bit_flag16 cellboards_monitor_api_get_open_wires(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_COUNT) {
        return 0U;
    }
    return cellboards_monitor_handler.open_wires[cellboard];
}

bool cellboards_monitor_api_is_cellboard_faulty(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_COUNT) {
        return false;
    }
    return cellboards_monitor_handler.states[cellboard] == CELLBOARDS_MONITOR_STATE_ERROR ||
           cellboards_monitor_handler.errors[cellboard] != 0U ||
           cellboards_monitor_handler.open_wires[cellboard] != 0U;
}

bool cellboards_monitor_api_is_any_cellboard_faulty(void) {
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        if (cellboards_monitor_api_is_cellboard_faulty(cellboard)) {
            return true;
        }
    }
    return false;
}
