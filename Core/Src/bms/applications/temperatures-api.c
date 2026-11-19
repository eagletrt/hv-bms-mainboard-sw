/*!
 * \file temperatures-api.c
 * \date 2026-11-10
 * \author Riccardo Segala [riccardo.segala@icloud.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells and discharge resistors temperature measurement and control
 */

#include "temperatures-api.h"
#include "eagletrt.h"

#include <string.h>

/*!
 * \brief The global temperature handler
 */
EAGLETRT_STATIC struct TemperaturesHandler temperatures_handler;

/*!
 * \brief Compute the minimum, maximum and average of a group of temperatures
 *
 * \param[in] temperatures The temperature values
 * \param[in] count The number of values, must be greater than 0
 *
 * \return The temperature information of the group
 */
EAGLETRT_STATIC struct TemperaturesInfo prv_temperatures_compute_info(const celsius *temperatures, size_t count) {
    // Start from the first value: 0°C (or below) is a valid temperature, so it can't mean "no value yet"
    struct TemperaturesInfo info = { temperatures[0], temperatures[0], 0 };

    celsius sum = 0;
    for (size_t i = 0U; i < count; ++i) {
        if (temperatures[i] < info.min) {
            info.min = temperatures[i];
        }
        if (temperatures[i] > info.max) {
            info.max = temperatures[i];
        }
        sum += temperatures[i];
    }
    info.average = sum / (celsius)count;

    return info;
}

void temperatures_api_init(void) {
    memset(&temperatures_handler, 0, sizeof(temperatures_handler));
}

enum TemperaturesReturnCode temperatures_api_set_cell_temperature(enum CellboardId cellboard, uint8_t index, celsius temperature) {
    if (cellboard >= CELLBOARD_COUNT) {
        return TEMPERATURES_RC_INVALID_CELLBOARD;
    }
    if (index >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return TEMPERATURES_RC_INVALID_INDEX;
    }

    temperatures_handler.cell_temperatures[cellboard][index] = temperature;
    temperatures_handler.is_updated_cell_temperatures[cellboard] = true;

    return TEMPERATURES_RC_OK;
}

enum TemperaturesReturnCode temperatures_api_reset_cell_temperatures_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        temperatures_handler.is_updated_cell_temperatures[cellboard] = false;
        return TEMPERATURES_RC_OK;
    }
    return TEMPERATURES_RC_INVALID_CELLBOARD;
}

enum TemperaturesReturnCode temperatures_api_reset_cell_temperatures_info_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        temperatures_handler.is_updated_cell_temperatures_info[cellboard] = false;
        return TEMPERATURES_RC_OK;
    }
    return TEMPERATURES_RC_INVALID_CELLBOARD;
}

enum TemperaturesReturnCode temperatures_api_set_cell_temperatures_info(
    enum CellboardId cellboard,
    celsius min,
    celsius max,
    celsius average) {
    if (cellboard >= CELLBOARD_COUNT) {
        return TEMPERATURES_RC_INVALID_CELLBOARD;
    }

    temperatures_handler.cell_temperatures_info[cellboard].min = min;
    temperatures_handler.cell_temperatures_info[cellboard].max = max;
    temperatures_handler.cell_temperatures_info[cellboard].average = average;
    temperatures_handler.is_updated_cell_temperatures_info[cellboard] = true;

    return TEMPERATURES_RC_OK;
}

celsius temperatures_api_get_cell_temperature(enum CellboardId cellboard, uint8_t index) {
    if (cellboard >= CELLBOARD_COUNT || index >= CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT) {
        return 0.F;
    }
    return temperatures_handler.cell_temperatures[cellboard][index];
}

struct TemperaturesInfo temperatures_api_get_cell_temperatures_info(enum CellboardId cellboard) {
    struct TemperaturesInfo info = { 0, 0, 0 };

    if (cellboard < CELLBOARD_COUNT) {
        info = temperatures_handler.cell_temperatures_info[cellboard];
    }

    return info;
}

struct TemperaturesInfo temperatures_api_get_pack_cell_temperatures_info(void) {
    struct TemperaturesInfo pack_info = { 0, 0, 0 };

    if (CELLBOARD_COUNT == 0) {
        return pack_info;
    }

    // Start from the first cellboard: 0°C (or below) is a valid temperature, so it can't mean "no value yet"
    pack_info = temperatures_handler.cell_temperatures_info[0];

    celsius average_sum = 0;
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        const struct TemperaturesInfo *info = &temperatures_handler.cell_temperatures_info[cellboard];

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

enum TemperaturesReturnCode temperatures_api_set_discharge_temperature(enum CellboardId cellboard, uint8_t index, celsius temperature) {
    if (cellboard >= CELLBOARD_COUNT) {
        return TEMPERATURES_RC_INVALID_CELLBOARD;
    }
    if (index >= CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT) {
        return TEMPERATURES_RC_INVALID_INDEX;
    }

    temperatures_handler.discharge_temperatures[cellboard][index] = temperature;
    temperatures_handler.is_updated_discharge_temperatures[cellboard] = true;

    return TEMPERATURES_RC_OK;
}

enum TemperaturesReturnCode temperatures_api_reset_discharge_temperatures_updated_flags(enum CellboardId cellboard) {
    if (cellboard < CELLBOARD_COUNT) {
        temperatures_handler.is_updated_discharge_temperatures[cellboard] = false;
        return TEMPERATURES_RC_OK;
    }
    return TEMPERATURES_RC_INVALID_CELLBOARD;
}

celsius temperatures_api_get_discharge_temperature(enum CellboardId cellboard, uint8_t index) {
    if (cellboard >= CELLBOARD_COUNT || index >= CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT) {
        return 0.F;
    }
    return temperatures_handler.discharge_temperatures[cellboard][index];
}

struct TemperaturesInfo temperatures_api_get_discharge_temperatures_info(enum CellboardId cellboard) {
    struct TemperaturesInfo info = { 0, 0, 0 };

    if (cellboard < CELLBOARD_COUNT && CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT > 0) {
        info = prv_temperatures_compute_info(temperatures_handler.discharge_temperatures[cellboard], CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT);
    }

    return info;
}

struct TemperaturesInfo temperatures_api_get_pack_discharge_temperatures_info(void) {
    struct TemperaturesInfo pack_info = { 0, 0, 0 };

    if (CELLBOARD_COUNT == 0 || CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT == 0) {
        return pack_info;
    }

    // Start from the first cellboard: 0°C (or below) is a valid temperature, so it can't mean "no value yet"
    pack_info = temperatures_api_get_discharge_temperatures_info(0);

    celsius average_sum = 0;
    for (enum CellboardId cellboard = 0; cellboard < CELLBOARD_COUNT; ++cellboard) {
        const struct TemperaturesInfo info = temperatures_api_get_discharge_temperatures_info(cellboard);

        if (info.min < pack_info.min) {
            pack_info.min = info.min;
        }
        if (info.max > pack_info.max) {
            pack_info.max = info.max;
        }
        average_sum += info.average;
    }

    // Every cellboard has the same number of discharge resistors, so the pack average is the mean of the cellboard averages
    pack_info.average = average_sum / CELLBOARD_COUNT;

    return pack_info;
}
