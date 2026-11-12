/*!
 * \file temperatures-api.c
 * \date 2026-11-10
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells temperature measurement and control
 */

#include "temperatures-api.h"

#include <string.h>

/*!
 * \brief The global temperature handler
 */
static struct TemperaturesHandler temperatures_handler;

void temperatures_api_init(void) {
    memset(&temperatures_handler, 0, sizeof(temperatures_handler));
}

enum TemperaturesReturnCode temperatures_api_set_value(enum CellboardId cellboard, uint8_t index, celsius temperature) {
    if (cellboard >= CELLBOARD_COUNT) {
        return TEMPERATURES_RC_INVALID_CELLBOARD;
    }
    if (index >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return TEMPERATURES_RC_INVALID_INDEX;
    }

    temperatures_handler.temperatures[cellboard][index] = temperature;
    temperatures_handler.is_updated_temperatures[cellboard] = true;

    return TEMPERATURES_RC_OK;
}

enum TemperaturesReturnCode temperatures_api_reset_temperatures_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        temperatures_handler.is_updated_temperatures[cellboard] = false;
        return TEMPERATURES_RC_OK;
    }
    return TEMPERATURES_RC_INVALID_CELLBOARD;
}

enum TemperaturesReturnCode temperatures_api_reset_info_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        temperatures_handler.is_updated_info[cellboard] = false;
        return TEMPERATURES_RC_OK;
    }
    return TEMPERATURES_RC_INVALID_CELLBOARD;
}

enum TemperaturesReturnCode temperatures_api_set_cellboard_temperature_info(
    enum CellboardId cellboard,
    celsius min,
    celsius max,
    celsius average) {
    if (cellboard >= CELLBOARD_COUNT) {
        return TEMPERATURES_RC_INVALID_CELLBOARD;
    }

    temperatures_handler.cellboard_info[cellboard].min = min;
    temperatures_handler.cellboard_info[cellboard].max = max;
    temperatures_handler.cellboard_info[cellboard].average = average;
    temperatures_handler.is_updated_info[cellboard] = true;

    return TEMPERATURES_RC_OK;
}

struct TemperaturesInfo temperatures_api_get_cellboard_info(enum CellboardId cellboard) {
    struct TemperaturesInfo info = { 0, 0, 0 };

    if (cellboard < CELLBOARD_COUNT) {
        info = temperatures_handler.cellboard_info[cellboard];
    }

    return info;
}

struct TemperaturesInfo temperatures_api_get_pack_info(void) {
    struct TemperaturesInfo pack_info = { 0, 0, 0 };

    if (CELLBOARD_COUNT == 0) {
        return pack_info;
    }

    // Start from the first cellboard: 0°C (or below) is a valid temperature, so it can't mean "no value yet"
    pack_info = temperatures_handler.cellboard_info[0];

    celsius average_sum = 0;
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        const struct TemperaturesInfo *info = &temperatures_handler.cellboard_info[cellboard];

        if (info->min < pack_info.min) {
            pack_info.min = info->min;
        }
        if (info->max > pack_info.max) {
            pack_info.max = info->max;
        }
        average_sum += info->average;
    }

    // Every cellboard has the same number of sensors, so the pack average is the mean of the cellboard averages
    pack_info.average = average_sum / CELLBOARD_COUNT;

    return pack_info;
}
