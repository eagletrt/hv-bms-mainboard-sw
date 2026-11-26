/*!
 * \file voltages-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells voltage measurement and control
 */

#include "voltages-api.h"

#include <string.h>

#include "mainboard-logger-api.h"

/*!
 * \brief The global voltage handler
 */
static struct VoltagesHandler voltages_handler;

enum VoltagesReturnCode voltages_api_init(void) {
    memset(&voltages_handler, 0, sizeof(voltages_handler));
    return VOLTAGES_RC_OK;
}

enum VoltagesReturnCode voltages_api_set_value(enum CellboardId cellboard, uint8_t index, volt voltage) {
    if (cellboard >= CELLBOARD_COUNT) {
        return VOLTAGES_RC_INVALID_CELLBOARD;
    }
    if (index >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        return VOLTAGES_RC_INVALID_INDEX;
    }

    voltages_handler.voltages[cellboard][index] = voltage;

    return VOLTAGES_RC_OK;
}

enum VoltagesReturnCode voltages_api_set_cellboard_voltage_info(
    enum CellboardId cellboard,
    volt min,
    volt max,
    volt average,
    volt sum) {
    if (cellboard >= CELLBOARD_COUNT) {
        return VOLTAGES_RC_INVALID_CELLBOARD;
    }

    voltages_handler.cellboard_info[cellboard].min = min;
    voltages_handler.cellboard_info[cellboard].max = max;
    voltages_handler.cellboard_info[cellboard].average = average;
    voltages_handler.cellboard_info[cellboard].sum = sum;

    if (!voltages_handler.is_received_info[cellboard]) {
        voltages_handler.is_received_info[cellboard] = true;
        if (voltages_api_is_ready()) {
            MAINBOARD_LOGGER_INFO("Cell voltages received from every cellboard");
        }
    }

    return VOLTAGES_RC_OK;
}

volt voltages_api_get_value(enum CellboardId cellboard, uint8_t index) {
    if (cellboard >= CELLBOARD_COUNT || index >= CELLBOARD_SEGMENT_SERIES_COUNT) {
        return 0.F;
    }
    return voltages_handler.voltages[cellboard][index];
}

struct VoltagesInfo voltages_api_get_cellboard_info(enum CellboardId cellboard) {
    struct VoltagesInfo info = { 0, 0, 0, 0 };

    if (cellboard < CELLBOARD_COUNT) {
        info = voltages_handler.cellboard_info[cellboard];
    }

    return info;
}

bool voltages_api_is_ready(void) {
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        if (!voltages_handler.is_received_info[cellboard]) {
            return false;
        }
    }
    return true;
}

struct VoltagesInfo voltages_api_get_pack_info(void) {
    struct VoltagesInfo pack_info = { 0, 0, 0, 0 };

    if (CELLBOARD_COUNT == 0) {
        return pack_info;
    }

    // Start from the first cellboard so that min and max are taken from real values
    pack_info.min = voltages_handler.cellboard_info[0].min;
    pack_info.max = voltages_handler.cellboard_info[0].max;

    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        const struct VoltagesInfo *info = &voltages_handler.cellboard_info[cellboard];

        if (info->min < pack_info.min) {
            pack_info.min = info->min;
        }
        if (info->max > pack_info.max) {
            pack_info.max = info->max;
        }
        pack_info.sum += info->sum;
    }

    // Every cellboard has the same number of cells in series, so the pack average is the total over all cells
    pack_info.average = pack_info.sum / (CELLBOARD_COUNT * CELLBOARD_SEGMENT_SERIES_COUNT);

    return pack_info;
}
