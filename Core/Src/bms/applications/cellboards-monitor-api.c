/*!
 * \file cellboards-monitor-api.c
 * \date 2026-11-18
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Monitoring of the cellboards state, errors and versions
 */

#include "cellboards-monitor-api.h"

#include <inttypes.h>
#include <string.h>

#include "eagletrt.h"
#include "eagletrt-api.h"
#include "mainboard-logger-api.h"

/*!
 * \brief The global cellboards monitor handler
 */
EAGLETRT_STATIC struct CellboardsMonitorHandler cellboards_monitor_handler;

/*! \brief Name of every cellboard state, used by the logs */
EAGLETRT_STATIC const char *const cellboards_monitor_state_names[CELLBOARDS_MONITOR_STATE_COUNT] = {
    [CELLBOARDS_MONITOR_STATE_INIT] = "INIT",
    [CELLBOARDS_MONITOR_STATE_IDLE] = "IDLE",
    [CELLBOARDS_MONITOR_STATE_ERROR] = "ERROR",
    [CELLBOARDS_MONITOR_STATE_FLASH] = "FLASH",
    [CELLBOARDS_MONITOR_STATE_DISCHARGE] = "DISCHARGE",
    [CELLBOARDS_MONITOR_STATE_COOLDOWN] = "COOLDOWN",
};

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

    const enum CellboardsMonitorState previous = cellboards_monitor_handler.states[cellboard];
    if (!cellboards_monitor_handler.is_received_state[cellboard]) {
        MAINBOARD_LOGGER_INFO("Cellboard %u: online, state %s", (unsigned)cellboard, cellboards_monitor_state_names[state]);
    } else if (state != previous) {
        if (state == CELLBOARDS_MONITOR_STATE_ERROR) {
            MAINBOARD_LOGGER_ERROR("Cellboard %u: %s -> ERROR", (unsigned)cellboard, cellboards_monitor_state_names[previous]);
        } else {
            MAINBOARD_LOGGER_INFO("Cellboard %u: %s -> %s",
                                  (unsigned)cellboard,
                                  cellboards_monitor_state_names[previous],
                                  cellboards_monitor_state_names[state]);
        }
    }

    cellboards_monitor_handler.states[cellboard] = state;
    cellboards_monitor_handler.is_received_state[cellboard] = true;

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_set_errors(enum CellboardId cellboard, bit_flag16 errors, bit_flag16 open_wires) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
    }

    // Drop the bits that do not correspond to any error so they cannot report a fault
    const bit_flag16 masked_errors = errors & CELLBOARDS_MONITOR_ERRORS_MASK;
    const bit_flag16 masked_open_wires = open_wires & CELLBOARDS_MONITOR_OPEN_WIRES_MASK;

    // The errors come with every error message, only the change is logged
    if (masked_errors != cellboards_monitor_handler.errors[cellboard] ||
        masked_open_wires != cellboards_monitor_handler.open_wires[cellboard]) {
        MAINBOARD_LOGGER_WARNING("Cellboard %u: errors 0x%04" PRIX32 ", open wires 0x%04" PRIX32,
                                 (unsigned)cellboard,
                                 (uint32_t)masked_errors,
                                 (uint32_t)masked_open_wires);
    }

    cellboards_monitor_handler.errors[cellboard] = masked_errors;
    cellboards_monitor_handler.open_wires[cellboard] = masked_open_wires;
    cellboards_monitor_handler.is_received_errors[cellboard] = true;

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_set_version(
    enum CellboardId cellboard,
    enum CellboardsMonitorVersionKind kind,
    uint16_t major,
    uint16_t minor,
    uint16_t patch) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
    }
    if (kind >= CELLBOARDS_MONITOR_VERSION_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_VERSION;
    }

    struct CellboardsMonitorVersion *version = &cellboards_monitor_handler.versions[cellboard][kind];

    // The version comes with every version message, only the change is logged
    if (version->major != major || version->minor != minor || version->patch != patch) {
        MAINBOARD_LOGGER_INFO("Cellboard %u: %s v%u.%u.%u",
                              (unsigned)cellboard,
                              (kind == CELLBOARDS_MONITOR_VERSION_FIRMWARE) ? "firmware" : "libcan",
                              (unsigned)major,
                              (unsigned)minor,
                              (unsigned)patch);
    }

    version->major = major;
    version->minor = minor;
    version->patch = patch;

    return CELLBOARDS_MONITOR_RC_OK;
}

enum CellboardsMonitorReturnCode cellboards_monitor_api_set_version_info(
    enum CellboardId cellboard,
    enum CellboardsMonitorVersionKind kind,
    uint32_t time,
    uint32_t commit_hash,
    bool dirty) {
    if (cellboard >= CELLBOARD_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_CELLBOARD;
    }
    if (kind >= CELLBOARDS_MONITOR_VERSION_COUNT) {
        return CELLBOARDS_MONITOR_RC_INVALID_VERSION;
    }

    struct CellboardsMonitorVersion *version = &cellboards_monitor_handler.versions[cellboard][kind];
    version->time = time;
    version->commit_hash = commit_hash;
    version->dirty = dirty;

    return CELLBOARDS_MONITOR_RC_OK;
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

struct CellboardsMonitorVersion cellboards_monitor_api_get_version(enum CellboardId cellboard, enum CellboardsMonitorVersionKind kind) {
    if (cellboard >= CELLBOARD_COUNT || kind >= CELLBOARDS_MONITOR_VERSION_COUNT) {
        return (struct CellboardsMonitorVersion){ 0 };
    }
    return cellboards_monitor_handler.versions[cellboard][kind];
}

bool cellboards_monitor_api_is_cellboard_faulty(enum CellboardId cellboard) {
    if (cellboard >= CELLBOARD_COUNT) {
        return false;
    }
    return cellboards_monitor_handler.states[cellboard] == CELLBOARDS_MONITOR_STATE_ERROR ||
           cellboards_monitor_handler.errors[cellboard] != 0U ||
           cellboards_monitor_handler.open_wires[cellboard] != 0U;
}

bool cellboards_monitor_api_is_ready(void) {
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        if (!cellboards_monitor_handler.is_received_state[cellboard] || !cellboards_monitor_handler.is_received_errors[cellboard]) {
            return false;
        }
    }
    return true;
}

bool cellboards_monitor_api_is_any_cellboard_faulty(void) {
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        if (cellboards_monitor_api_is_cellboard_faulty(cellboard)) {
            return true;
        }
    }
    return false;
}
