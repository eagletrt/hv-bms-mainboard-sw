/*!
 * \file voltages-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells voltage measurement and control
 */

#include "voltages-api.h"
#include "string.h"

/*!
 * \brief The global voltage handler
 */
static struct VoltagesHandler voltage_handler;

void voltages_api_init(void) {
    memset(&voltage_handler, 0, sizeof(voltage_handler));
}

void voltages_api_set_value(enum CellboardId cellboard, uint8_t index, volt voltage) {
    if (cellboard < CELLBOARD_COUNT && index < CELLBOARD_SEGMENT_SERIES_COUNT) {
        voltage_handler.voltages[cellboard][index] = voltage;
        voltage_handler.is_updated_voltages[cellboard] = true;
    }
}

void voltages_api_reset_voltages_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        voltage_handler.is_updated_voltages[cellboard] = false;
    }
}

void voltages_api_reset_info_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        voltage_handler.is_updated_info[cellboard] = false;
    }
}

void voltages_api_set_cellboard_voltage_info(
    enum CellboardId cellboard,
    volt min,
    volt max,
    volt average,
    volt sum) {
    if (cellboard < CELLBOARD_COUNT) {
        voltage_handler.cellboard_info[cellboard].min = min;
        voltage_handler.cellboard_info[cellboard].max = max;
        voltage_handler.cellboard_info[cellboard].average = average;
        voltage_handler.cellboard_info[cellboard].sum = sum;
        voltage_handler.is_updated_info[cellboard] = true;
    }
}

struct VoltagesInfo voltages_api_get_cellboard_info(enum CellboardId cellboard) {
    struct VoltagesInfo info = { 0, 0, 0, 0 };

    if (cellboard < CELLBOARD_COUNT) {
        info = voltage_handler.cellboard_info[cellboard];
    }

    return info;
}

struct VoltagesInfo voltages_api_get_pack_info(void) {
    struct VoltagesInfo pack_info = { 0, 0, 0, 0 };

    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        if (pack_info.min == 0 || voltage_handler.cellboard_info[cellboard].min < pack_info.min) {
            pack_info.min = voltage_handler.cellboard_info[cellboard].min;
        }
        if (voltage_handler.cellboard_info[cellboard].max > pack_info.max) {
            pack_info.max = voltage_handler.cellboard_info[cellboard].max;
        }
        pack_info.sum += voltage_handler.cellboard_info[cellboard].sum;
    }

    if (CELLBOARD_COUNT > 0) {
        pack_info.average = pack_info.sum / (CELLBOARD_COUNT * CELLBOARD_SEGMENT_SERIES_COUNT);
    }

    return pack_info;
}