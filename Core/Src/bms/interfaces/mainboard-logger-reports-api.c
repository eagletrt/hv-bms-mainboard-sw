/*!
 * \file mainboard-logger-reports-api.c
 * \date 2026-11-26
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Reports printed by the logger console
 *
 * \details Every report is a list of rows filled one at a time, the console
 *          sends them paced so that the output is never full. An empty row
 *          separates the groups of a report.
 *          The values are read when their row is filled, so a report is a
 *          snapshot spread over the time it takes to be sent (some hundreds of ms).
 *          The values out of their allowed range are followed by '!'
 */

#include "mainboard-logger-reports-api.h"

#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "bal-control-api.h"
#include "can-bridge-api.h"
#include "can-communication-api.h"
#include "cellboards-monitor-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "feedback-api.h"
#include "internal-temperatures-api.h"
#include "internal-voltages-api.h"
#include "mainboard-conf.h"
#include "mainboard-fsm-api.h"
#include "mainboard-fsm.h"
#include "mainboard-status-api.h"
#include "pcu-api.h"
#include "sensing-api.h"
#include "temperatures-api.h"
#include "voltages-api.h"

/*!* ######################### HELPERS ################################### *!*/

/*! \brief mV in a V */
#define MAINBOARD_LOGGER_MV_PER_V (1000.F)

/*! \brief Percent in a ratio */
#define MAINBOARD_LOGGER_PERCENT (100.F)

/*! \brief Values per row of the cell voltages and temperatures */
#define MAINBOARD_LOGGER_VALUES_PER_ROW (12U)

/*! \brief Columns of a two columns layout */
#define MAINBOARD_LOGGER_HALF_WIDTH (46)

/*! \brief Size of a formatted version, "65535.65535.65535 abcdef0*" */
#define MAINBOARD_LOGGER_VERSION_SIZE (32U)

/*! \brief Cells of the precharge progress bar */
#define MAINBOARD_LOGGER_BAR_SIZE (20U)

static_assert(CELLBOARD_SEGMENT_SERIES_COUNT % MAINBOARD_LOGGER_VALUES_PER_ROW == 0U, "The cell voltages must fill whole rows");
static_assert(CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT % MAINBOARD_LOGGER_VALUES_PER_ROW == 0U, "The cell temperatures must fill whole rows");

void int_mainboard_logger_line_append(struct MainboardLoggerLine *const line, const char *const fmt, ...) {
    if (line == NULL || fmt == NULL || line->size >= sizeof(line->text) - 1U) {
        return;
    }
    const size_t available = sizeof(line->text) - line->size;
    va_list args;
    va_start(args, fmt);
    const int written = vsnprintf(&line->text[line->size], available, fmt, args);
    va_end(args);
    if (written > 0) {
        line->size += EAGLETRT_API_MIN((size_t)written, available - 1U);
    }
}

/*!
 * \brief Mark of a value that must stay within a range
 *
 * \param value The value
 * \param min The minimum allowed value
 * \param max The maximum allowed value
 *
 * \returns "!" if the value is out of range, " " otherwise, so that the columns stay aligned
 */
EAGLETRT_STATIC const char *prv_mainboard_logger_range_mark(const float value, const float min, const float max) {
    return (value < min || value > max) ? "!" : " ";
}

/*!
 * \brief Append a label followed by a number, or "n/a" if the number is not available
 *
 * \param line The line
 * \param label The label, padded to 14 columns
 * \param ready false if the number was never received
 * \param precision The digits after the decimal point
 * \param value The number
 * \param unit The unit, after the number
 */
EAGLETRT_STATIC void prv_mainboard_logger_number(struct MainboardLoggerLine *const line,
                                                 const char *const label,
                                                 const bool ready,
                                                 const int precision,
                                                 const float value,
                                                 const char *const unit) {
    constexpr int number_width = 8;
    int_mainboard_logger_line_append(line, "%-14s", label);
    if (ready) {
        int_mainboard_logger_line_append(line, "%*.*f %-3s", number_width, precision, (double)value, unit);
    } else {
        int_mainboard_logger_line_append(line, "%*s %-3s", number_width, "n/a", unit);
    }
}

/*!
 * \brief Append a flag as a dot (filled when set) and its name
 *
 * \param line The line
 * \param name The name of the flag
 * \param value The value of the flag
 * \param width The columns taken by the flag, 0 for no padding
 */
EAGLETRT_STATIC void prv_mainboard_logger_flag(struct MainboardLoggerLine *const line, const char *const name, const bool value, const int width) {
    // The dot and the space after it
    constexpr int dot_width = 2;
    const int name_width = (width > dot_width) ? (width - dot_width) : 0;
    int_mainboard_logger_line_append(line, "%s %-*s", value ? "●" : "○", name_width, name);
}

/*!
 * \brief Append the precharge progress (TS over pack voltage) as a bar
 *
 * \param line The line
 */
EAGLETRT_STATIC void prv_mainboard_logger_precharge_bar(struct MainboardLoggerLine *const line) {
    float ratio = 0.F;
    if (internal_voltages_api_is_voltages_ready()) {
        const volt pack = internal_voltages_api_get_value(INTERNAL_VOLTAGES_INDEX_PACK);
        if (pack > 0.F) {
            ratio = EAGLETRT_API_CLAMP(internal_voltages_api_get_value(INTERNAL_VOLTAGES_INDEX_TS) / pack, 0.F, 1.F);
        }
    }
    const size_t filled = (size_t)((ratio * (float)MAINBOARD_LOGGER_BAR_SIZE) + 0.5F);

    int_mainboard_logger_line_append(line, "%-14s", "Precharge");
    for (size_t i = 0U; i < MAINBOARD_LOGGER_BAR_SIZE; ++i) {
        int_mainboard_logger_line_append(line, "%s", (i < filled) ? "█" : "░");
    }
    int_mainboard_logger_line_append(line, " %5.1f %%  ", (double)(ratio * MAINBOARD_LOGGER_PERCENT));
    prv_mainboard_logger_flag(line, "complete", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE), 0);
}

/*!* ######################### NAMES ##################################### *!*/

EAGLETRT_STATIC const char *const mainboard_logger_digital_names[FEEDBACK_DIGITAL_INDEX_COUNT] = {
    [FEEDBACK_DIGITAL_INDEX_AIRN_CLOSE_COM] = "air- close com",
    [FEEDBACK_DIGITAL_INDEX_PRECHARGE_CLOSE_COM] = "precharge close com",
    [FEEDBACK_DIGITAL_INDEX_AIRP_CLOSE_COM] = "air+ close com",
    [FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC] = "precharge open mec",
    [FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V] = "ts less than 60v",
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED] = "plausible state persisted",
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE] = "plausible state",
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED] = "ams fault cockpit led",
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED] = "imd fault cockpit led",
    [FEEDBACK_DIGITAL_INDEX_INDICATOR_CONNECTED] = "indicator connected",
    [FEEDBACK_DIGITAL_INDEX_LATCH_RESET] = "latch reset",
    [FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED] = "plausible state latched",
    [FEEDBACK_DIGITAL_INDEX_AMS_FAULT_LATCHED] = "ams fault latched",
    [FEEDBACK_DIGITAL_INDEX_IMD_FAULT_LATCHED] = "imd fault latched",
    [FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED] = "ext fault latched",
    [FEEDBACK_DIGITAL_INDEX_HANDCART_CONNECTED] = "handcart connected",
};

EAGLETRT_STATIC const char *const mainboard_logger_analog_names[FEEDBACK_ANALOG_INDEX_COUNT] = {
    [FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC] = "air- open mec",
    [FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC] = "air+ open mec",
    [FEEDBACK_ANALOG_INDEX_IMD_OK] = "imd ok",
    [FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC] = "plausible state rc",
    [FEEDBACK_ANALOG_INDEX_TSAL_GREEN] = "tsal green",
    [FEEDBACK_ANALOG_INDEX_PROBING_3V3] = "probing 3v3",
    [FEEDBACK_ANALOG_INDEX_SD_IMD_FB] = "sd imd fb",
    [FEEDBACK_ANALOG_INDEX_SD_AMS_FB] = "sd ams fb",
    [FEEDBACK_ANALOG_INDEX_SD_OUT] = "sd out",
    [FEEDBACK_ANALOG_INDEX_SD_IN] = "sd in",
    [FEEDBACK_ANALOG_INDEX_SD_END] = "sd end",
    [FEEDBACK_ANALOG_INDEX_V5_MCU] = "5v mcu",
};

EAGLETRT_STATIC const char *const mainboard_logger_internal_temperature_names[INTERNAL_TEMPERATURES_INDEX_COUNT] = {
    [INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR] = "Linear reg.",
    [INTERNAL_TEMPERATURES_INDEX_PRECHARGE] = "Precharge",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_INLET] = "Cooling in",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_1] = "Cooling out 1",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_2] = "Cooling out 2",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_3] = "Cooling out 3",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_4] = "Cooling out 4",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_5] = "Cooling out 5",
    [INTERNAL_TEMPERATURES_INDEX_COOLING_OUTLET_6] = "Cooling out 6",
};

EAGLETRT_STATIC const char *const mainboard_logger_cellboard_state_names[CELLBOARDS_MONITOR_STATE_COUNT] = {
    [CELLBOARDS_MONITOR_STATE_INIT] = "INIT",
    [CELLBOARDS_MONITOR_STATE_IDLE] = "IDLE",
    [CELLBOARDS_MONITOR_STATE_ERROR] = "ERROR",
    [CELLBOARDS_MONITOR_STATE_FLASH] = "FLASH",
    [CELLBOARDS_MONITOR_STATE_DISCHARGE] = "DISCHARGE",
    [CELLBOARDS_MONITOR_STATE_COOLDOWN] = "COOLDOWN",
};

/*! \brief Short names, so that every error of a cellboard fits in a row */
EAGLETRT_STATIC const char *const mainboard_logger_cellboard_error_names[CELLBOARDS_MONITOR_ERROR_COUNT] = {
    [CELLBOARDS_MONITOR_ERROR_POST] = "POST",
    [CELLBOARDS_MONITOR_ERROR_CELL_UNDER_VOLTAGE] = "under volt",
    [CELLBOARDS_MONITOR_ERROR_CELL_OVER_VOLTAGE] = "over volt",
    [CELLBOARDS_MONITOR_ERROR_CELL_UNDER_TEMPERATURE] = "under temp",
    [CELLBOARDS_MONITOR_ERROR_CELL_OVER_TEMPERATURE] = "over temp",
    [CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_UNDER_TEMPERATURE] = "dis. under temp",
    [CELLBOARDS_MONITOR_ERROR_DISCHARGE_RESISTOR_OVER_TEMPERATURE] = "dis. over temp",
    [CELLBOARDS_MONITOR_ERROR_CAN_COMMUNICATION] = "CAN",
    [CELLBOARDS_MONITOR_ERROR_FLASH] = "flash",
    [CELLBOARDS_MONITOR_ERROR_BMS_MONITOR_COMMUNICATION] = "LTC comm",
};

/*! \brief Indexed by status + 1, IR1553204_STATUS_UNKNOWN is -1 */
EAGLETRT_STATIC const char *const mainboard_logger_imd_status_names[IR1553204_STATUS_COUNT + 1] = {
    [IR1553204_STATUS_UNKNOWN + 1] = "UNKNOWN",
    [IR1553204_STATUS_SHORT_CIRCUIT + 1] = "SHORT CIRCUIT",
    [IR1553204_STATUS_NORMAL + 1] = "NORMAL",
    [IR1553204_STATUS_UNDER_VOLTAGE + 1] = "UNDER VOLTAGE",
    [IR1553204_STATUS_START_MEASURE + 1] = "START MEASURE",
    [IR1553204_STATUS_DEVICE_ERROR + 1] = "DEVICE ERROR",
    [IR1553204_STATUS_EARTH_FAULT + 1] = "EARTH FAULT",
};

/*!
 * \brief Get the name of the state of a cellboard
 *
 * \param state The state
 *
 * \returns The name, "?" if the state is out of range
 */
EAGLETRT_STATIC const char *prv_mainboard_logger_cellboard_state_name(const enum CellboardsMonitorState state) {
    return (state < CELLBOARDS_MONITOR_STATE_COUNT) ? mainboard_logger_cellboard_state_names[state] : "?";
}

/*!* ######################### VOLTAGES ################################## *!*/

/*!
 * \brief Rows: pack summary, then two rows of 12 cells for each cellboard
 */
EAGLETRT_STATIC bool prv_mainboard_logger_voltages_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t rows_per_cellboard = CELLBOARD_SEGMENT_SERIES_COUNT / MAINBOARD_LOGGER_VALUES_PER_ROW;
    constexpr size_t header_rows = 2U;

    if (row == 0U) {
        const struct VoltagesInfo info = voltages_api_get_pack_info();
        int_mainboard_logger_line_append(line,
                                         "Pack   min %.3f V   max %.3f V   avg %.3f V   delta %.0f mV   sum %.2f V",
                                         (double)info.min,
                                         (double)info.max,
                                         (double)info.average,
                                         (double)((info.max - info.min) * MAINBOARD_LOGGER_MV_PER_V),
                                         (double)info.sum);
        if (!voltages_api_is_ready()) {
            int_mainboard_logger_line_append(line, "   waiting for data");
        }
        return true;
    }
    if (row == 1U) {
        return true;
    }

    const size_t index = row - header_rows;
    const enum CellboardId cellboard = (enum CellboardId)(index / rows_per_cellboard);
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return false;
    }
    const size_t first = (index % rows_per_cellboard) * MAINBOARD_LOGGER_VALUES_PER_ROW;

    if (first == 0U) {
        int_mainboard_logger_line_append(line, "CB%u", (unsigned)cellboard);
    } else {
        int_mainboard_logger_line_append(line, "   ");
    }
    int_mainboard_logger_line_append(line, "  %2u-%2u ", (unsigned)(first + 1U), (unsigned)(first + MAINBOARD_LOGGER_VALUES_PER_ROW));

    for (size_t i = first; i < first + MAINBOARD_LOGGER_VALUES_PER_ROW; ++i) {
        const volt value = voltages_api_get_value(cellboard, (uint8_t)i);
        int_mainboard_logger_line_append(line, " %6.3f%s", (double)value, prv_mainboard_logger_range_mark(value, PHC_MIN_CELL_VOLTAGE_V, PHC_MAX_CELL_VOLTAGE_V));
    }
    return true;
}

/*!* ######################### TEMPERATURES ############################## *!*/

/*!
 * \brief Append a temperatures info (min, avg, max) checked against a range
 *
 * \param line The line
 * \param label The label
 * \param info The temperatures
 * \param min The minimum allowed temperature
 * \param max The maximum allowed temperature
 */
EAGLETRT_STATIC void prv_mainboard_logger_temperatures_info(struct MainboardLoggerLine *const line,
                                                            const char *const label,
                                                            const struct TemperaturesInfo info,
                                                            const celsius min,
                                                            const celsius max) {
    int_mainboard_logger_line_append(line,
                                     "%-11smin %6.1f%s °C   avg %6.1f °C   max %6.1f%s °C   allowed %.0f .. %.0f °C",
                                     label,
                                     (double)info.min,
                                     prv_mainboard_logger_range_mark(info.min, min, max),
                                     (double)info.average,
                                     (double)info.max,
                                     prv_mainboard_logger_range_mark(info.max, min, max),
                                     (double)min,
                                     (double)max);
}

/*!
 * \brief Rows: pack summaries, then for each cellboard four rows of 12 cell
 *        temperatures and a row with the discharge resistors
 */
EAGLETRT_STATIC bool prv_mainboard_logger_temperatures_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t cell_rows = CELLBOARD_SEGMENT_TEMP_SENSOR_COUNT / MAINBOARD_LOGGER_VALUES_PER_ROW;
    // Cell rows, the discharge row and the empty row
    constexpr size_t rows_per_cellboard = cell_rows + 2U;
    constexpr size_t header_rows = 3U;

    switch (row) {
        case 0U:
            prv_mainboard_logger_temperatures_info(line, "Cells", temperatures_api_get_pack_cell_temperatures_info(), PHC_MIN_CELL_TEMPERATURE_C, PHC_MAX_CELL_TEMPERATURE_C);
            if (!temperatures_api_is_cell_temperatures_ready()) {
                int_mainboard_logger_line_append(line, "  waiting");
            }
            return true;
        case 1U:
            prv_mainboard_logger_temperatures_info(line, "Discharge", temperatures_api_get_pack_discharge_temperatures_info(), PHC_MIN_DISCHARGE_TEMPERATURE_C, PHC_MAX_DISCHARGE_TEMPERATURE_C);
            if (!temperatures_api_is_discharge_temperatures_ready()) {
                int_mainboard_logger_line_append(line, "  waiting");
            }
            return true;
        case 2U:
            return true;
        default:
            break;
    }

    const size_t index = row - header_rows;
    const enum CellboardId cellboard = (enum CellboardId)(index / rows_per_cellboard);
    const size_t sub_row = index % rows_per_cellboard;
    // No empty row after the last cellboard
    if (cellboard >= CELLBOARD_ID_COUNT || (cellboard == CELLBOARD_ID_COUNT - 1U && sub_row == rows_per_cellboard - 1U)) {
        return false;
    }
    if (sub_row == rows_per_cellboard - 1U) {
        return true;
    }

    if (sub_row < cell_rows) {
        const size_t first = sub_row * MAINBOARD_LOGGER_VALUES_PER_ROW;
        if (first == 0U) {
            int_mainboard_logger_line_append(line, "CB%u", (unsigned)cellboard);
        } else {
            int_mainboard_logger_line_append(line, "   ");
        }
        int_mainboard_logger_line_append(line, "  cells %2u-%2u  ", (unsigned)(first + 1U), (unsigned)(first + MAINBOARD_LOGGER_VALUES_PER_ROW));
        for (size_t i = first; i < first + MAINBOARD_LOGGER_VALUES_PER_ROW; ++i) {
            const celsius value = temperatures_api_get_cell_temperature(cellboard, (uint8_t)i);
            int_mainboard_logger_line_append(line, " %5.1f%s", (double)value, prv_mainboard_logger_range_mark(value, PHC_MIN_CELL_TEMPERATURE_C, PHC_MAX_CELL_TEMPERATURE_C));
        }
        return true;
    }

    int_mainboard_logger_line_append(line, "     discharge    ");
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_DISCHARGE_TEMP_COUNT; ++i) {
        const celsius value = temperatures_api_get_discharge_temperature(cellboard, (uint8_t)i);
        int_mainboard_logger_line_append(line, " %5.1f%s", (double)value, prv_mainboard_logger_range_mark(value, PHC_MIN_DISCHARGE_TEMPERATURE_C, PHC_MAX_DISCHARGE_TEMPERATURE_C));
    }
    return true;
}

/*!* ######################### INTERNAL ################################## *!*/

/*!
 * \brief Get the allowed range of an internal temperature
 *
 * \param index The temperature
 * \param min[out] The minimum allowed temperature
 * \param max[out] The maximum allowed temperature
 */
EAGLETRT_STATIC void prv_mainboard_logger_internal_temperature_range(const enum InternalTemperaturesIndex index, celsius *const min, celsius *const max) {
    switch (index) {
        case INTERNAL_TEMPERATURES_INDEX_LINEAR_REGULATOR:
            *min = PHC_MIN_LINEAR_REGULATOR_TEMPERATURE_C;
            *max = PHC_MAX_LINEAR_REGULATOR_TEMPERATURE_C;
            break;
        case INTERNAL_TEMPERATURES_INDEX_PRECHARGE:
            *min = PHC_MIN_PRECHARGE_TEMPERATURE_C;
            *max = PHC_MAX_PRECHARGE_TEMPERATURE_C;
            break;
        default:
            *min = PHC_MIN_COOLING_TEMPERATURE_C;
            *max = PHC_MAX_COOLING_TEMPERATURE_C;
            break;
    }
}

/*!
 * \brief Rows: TS and pack voltages, current, power, precharge, then the internal temperatures
 */
EAGLETRT_STATIC bool prv_mainboard_logger_internal_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t temperatures_per_row = 3U;
    constexpr size_t header_rows = 7U;
    const bool voltages_ready = internal_voltages_api_is_voltages_ready();
    const bool current_ready = internal_voltages_api_is_current_ready();
    const struct InternalVoltagesInfo info = internal_voltages_api_get_info();
    const struct VoltagesInfo cells = voltages_api_get_pack_info();

    switch (row) {
        case 0U:
            prv_mainboard_logger_number(line, "TS voltage", voltages_ready, 2, info.ts, "V");
            int_mainboard_logger_line_append(line, "        ");
            prv_mainboard_logger_number(line, "Pack voltage", voltages_ready, 2, info.pack, "V");
            return true;
        case 1U:
            prv_mainboard_logger_number(line, "Cells sum", voltages_api_is_ready(), 2, cells.sum, "V");
            int_mainboard_logger_line_append(line, "        ");
            prv_mainboard_logger_number(line, "Pack - cells", voltages_ready && voltages_api_is_ready(), 2, info.pack - cells.sum, "V");
            return true;
        case 2U:
            prv_mainboard_logger_number(line, "Current", current_ready, 2, info.current, "A");
            int_mainboard_logger_line_append(line, "        ");
            prv_mainboard_logger_number(line, "Power", current_ready && voltages_ready, 2, info.power, "kW");
            return true;
        case 3U:
            return true;
        case 4U:
            prv_mainboard_logger_precharge_bar(line);
            return true;
        case 5U:
            return true;
        case 6U:
            int_mainboard_logger_line_append(line, "Temperatures%s", internal_temperatures_api_is_ready() ? "" : "  waiting for every sensor");
            return true;
        default:
            break;
    }

    const size_t first = (row - header_rows) * temperatures_per_row;
    if (first >= INTERNAL_TEMPERATURES_INDEX_COUNT) {
        return false;
    }
    for (size_t i = first; i < first + temperatures_per_row && i < INTERNAL_TEMPERATURES_INDEX_COUNT; ++i) {
        const enum InternalTemperaturesIndex index = (enum InternalTemperaturesIndex)i;
        celsius min = 0.F;
        celsius max = 0.F;
        prv_mainboard_logger_internal_temperature_range(index, &min, &max);
        const celsius value = internal_temperatures_api_get_value(index);
        int_mainboard_logger_line_append(line,
                                         "%-14s%6.1f%s °C     ",
                                         mainboard_logger_internal_temperature_names[index],
                                         (double)value,
                                         prv_mainboard_logger_range_mark(value, min, max));
    }
    return true;
}

/*!* ######################### IMD ####################################### *!*/

/*!
 * \brief Rows: decoded status, PWM, then the related conditions
 */
EAGLETRT_STATIC bool prv_mainboard_logger_imd_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr int flag_width = 24;

    switch (row) {
        case 0U: {
            const enum Ir1553204Status status = sensing_api_get_imd_status();
            const bool valid = (status >= IR1553204_STATUS_UNKNOWN && status < IR1553204_STATUS_COUNT);
            int_mainboard_logger_line_append(line, "%-14s%s", "Status", valid ? mainboard_logger_imd_status_names[status + 1] : "INVALID");
            return true;
        }
        case 1U:
            prv_mainboard_logger_number(line, "Frequency", true, 1, sensing_api_get_imd_frequency(), "Hz");
            return true;
        case 2U:
            prv_mainboard_logger_number(line, "Duty cycle", true, 1, sensing_api_get_imd_duty_cycle() * MAINBOARD_LOGGER_PERCENT, "%");
            return true;
        case 3U:
            return true;
        case 4U:
            prv_mainboard_logger_flag(line, "imd ok", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_OK), flag_width);
            prv_mainboard_logger_flag(line, "imd fault latched", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_FAULT_LATCHED), flag_width);
            prv_mainboard_logger_flag(line, "imd cockpit led on", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_IMD_COCKPIT_LED_ON), flag_width);
            return true;
        case 5U:
            prv_mainboard_logger_flag(line, "sd imd node closed", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_SD_IMD_NODE_CLOSED), flag_width);
            prv_mainboard_logger_flag(line, "imd fault", mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_IMD_FAULT), flag_width);
            prv_mainboard_logger_flag(line, "stale imd", mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_STALE_IMD), flag_width);
            return true;
        default:
            return false;
    }
}

/*!* ######################### PCU ####################################### *!*/

/*!
 * \brief A relay and the conditions that show its state
 */
struct MainboardLoggerRelay {
    const char *name;                         /*!< Shown in the first column */
    enum PcuRelay relay;                      /*!< The relay as commanded by the PCU */
    enum MainboardStatusCondition readback;   /*!< The readback of the command line */
    enum MainboardStatusCondition mechanical; /*!< The mechanical feedback */
};

EAGLETRT_STATIC const struct MainboardLoggerRelay mainboard_logger_relays[PCU_RELAY_COUNT] = {
    { "AIR-",      PCU_RELAY_AIR_NEGATIVE, MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED,      MAINBOARD_STATUS_CONDITION_AIRN_CLOSED      },
    { "Precharge", PCU_RELAY_PRECHARGE,    MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED, MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED },
    { "AIR+",      PCU_RELAY_AIR_POSITIVE, MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED,      MAINBOARD_STATUS_CONDITION_AIRP_CLOSED      },
};

/*!
 * \brief Rows: the relays as commanded, read back and mechanically seen, then the TS conditions
 */
EAGLETRT_STATIC bool prv_mainboard_logger_pcu_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t header_rows = 1U;
    constexpr int flag_width = 22;

    if (row == 0U) {
        int_mainboard_logger_line_append(line, "%-14s%-13s%-13s%-13s", "Relay", "Commanded", "Readback", "Mechanical");
        return true;
    }

    const size_t index = row - header_rows;
    if (index < PCU_RELAY_COUNT) {
        const struct MainboardLoggerRelay *const relay = &mainboard_logger_relays[index];
        const bool commanded = pcu_api_is_closed(relay->relay);
        const bool readback = mainboard_status_api_get_condition(relay->readback);
        const bool mechanical = mainboard_status_api_get_condition(relay->mechanical);

        int_mainboard_logger_line_append(line,
                                         "%-14s%-13s%-13s%-13s%s",
                                         relay->name,
                                         commanded ? "closed" : "open",
                                         readback ? "closed" : "open",
                                         mechanical ? "closed" : "open",
                                         (commanded == readback && commanded == mechanical) ? "✓" : "✗ mismatch");
        return true;
    }

    switch (index - PCU_RELAY_COUNT) {
        case 0U:
            return true;
        case 1U:
            prv_mainboard_logger_precharge_bar(line);
            return true;
        case 2U:
            prv_mainboard_logger_flag(line, "ts below 60v", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_TS_BELOW_60V), flag_width);
            prv_mainboard_logger_flag(line, "tsal green", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_TSAL_GREEN), flag_width);
            prv_mainboard_logger_flag(line, "sd end closed", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_SD_END_CLOSED), flag_width);
            prv_mainboard_logger_flag(line, "ams cockpit led on", mainboard_status_api_get_condition(MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON), 0);
            return true;
        default:
            return false;
    }
}

/*!* ######################### BALANCING ################################# *!*/

/*!
 * \brief Rows: request and state of the balancing control, then the cells being discharged by each cellboard
 */
EAGLETRT_STATIC bool prv_mainboard_logger_balancing_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t header_rows = 3U;
    constexpr int flag_width = 20;

    switch (row) {
        case 0U:
            prv_mainboard_logger_flag(line, "requested", mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING), flag_width);
            prv_mainboard_logger_flag(line, "discharging", bal_control_api_is_discharging(), flag_width);
            prv_mainboard_logger_flag(line, "pack balanced", bal_control_api_is_pack_balanced(), flag_width);
            prv_mainboard_logger_flag(line, "valid parameters", bal_control_api_has_valid_parameters(), 0);
            return true;
        case 1U: {
            const struct VoltagesInfo info = voltages_api_get_pack_info();
            int_mainboard_logger_line_append(line,
                                             "target %.3f V   threshold %.0f mV   pack delta %.0f mV   min %.3f V   max %.3f V",
                                             (double)bal_control_api_get_target_voltage(),
                                             (double)(bal_control_api_get_threshold() * MAINBOARD_LOGGER_MV_PER_V),
                                             (double)((info.max - info.min) * MAINBOARD_LOGGER_MV_PER_V),
                                             (double)info.min,
                                             (double)info.max);
            return true;
        }
        case 2U:
            return true;
        default:
            break;
    }

    const enum CellboardId cellboard = (enum CellboardId)(row - header_rows);
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return false;
    }

    const uint32_t cells = bal_control_api_get_balancing_status(cellboard);
    int_mainboard_logger_line_append(line, "CB%u  ", (unsigned)cellboard);
    for (size_t i = 0U; i < CELLBOARD_SEGMENT_SERIES_COUNT; ++i) {
        if (i == MAINBOARD_LOGGER_VALUES_PER_ROW) {
            int_mainboard_logger_line_append(line, " ");
        }
        int_mainboard_logger_line_append(line, "%s ", EAGLETRT_API_BIT_GET(cells, i) ? "●" : "·");
    }
    int_mainboard_logger_line_append(line,
                                     " %2u cells   %s",
                                     (unsigned)__builtin_popcount(cells),
                                     prv_mainboard_logger_cellboard_state_name(cellboards_monitor_api_get_state(cellboard)));
    return true;
}

/*!* ######################### FEEDBACK ################################## *!*/

/*!
 * \brief Append a digital feedback as a column
 *
 * \param line The line
 * \param index The feedback
 */
EAGLETRT_STATIC void prv_mainboard_logger_digital(struct MainboardLoggerLine *const line, const enum FeedbackDigitalIndex index) {
    int_mainboard_logger_line_append(line, "%-28s%-18s", mainboard_logger_digital_names[index], feedback_api_get_digital_value(index) ? "high" : "low");
}

/*!
 * \brief Append an analog feedback as a column
 *
 * \param line The line
 * \param index The feedback
 */
EAGLETRT_STATIC void prv_mainboard_logger_analog(struct MainboardLoggerLine *const line, const enum FeedbackAnalogIndex index) {
    const char *name = "n/a";
    switch (feedback_api_get_analog_status(index)) {
        case FEEDBACK_ANALOG_STATUS_HIGH:
            name = "high";
            break;
        case FEEDBACK_ANALOG_STATUS_LOW:
            name = "low";
            break;
        case FEEDBACK_ANALOG_STATUS_ERROR:
            name = "error !";
            break;
        default:
            break;
    }
    int_mainboard_logger_line_append(line, "%-22s%5.2f V  %-12s", mainboard_logger_analog_names[index], (double)feedback_api_get_analog_value(index), name);
}

/*!
 * \brief Rows: the digital feedback, then the analog ones, two per row
 */
EAGLETRT_STATIC bool prv_mainboard_logger_feedback_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t columns = 2U;
    constexpr size_t digital_rows = (FEEDBACK_DIGITAL_INDEX_COUNT + columns - 1U) / columns;
    constexpr size_t analog_rows = (FEEDBACK_ANALOG_INDEX_COUNT + columns - 1U) / columns;

    if (row == 0U) {
        int_mainboard_logger_line_append(line, "Digital%s", feedback_api_is_ready() ? "" : "  not ready, the filters are filling up");
        return true;
    }
    if (row <= digital_rows) {
        const size_t first = (row - 1U) * columns;
        for (size_t i = first; i < first + columns && i < FEEDBACK_DIGITAL_INDEX_COUNT; ++i) {
            prv_mainboard_logger_digital(line, (enum FeedbackDigitalIndex)i);
        }
        return true;
    }

    const size_t index = row - digital_rows - 1U;
    if (index == 0U) {
        return true;
    }
    if (index == 1U) {
        int_mainboard_logger_line_append(line, "Analog  voltage at the MCU pin");
        return true;
    }
    if (index - 2U < analog_rows) {
        const size_t first = (index - 2U) * columns;
        for (size_t i = first; i < first + columns && i < FEEDBACK_ANALOG_INDEX_COUNT; ++i) {
            prv_mainboard_logger_analog(line, (enum FeedbackAnalogIndex)i);
        }
        return true;
    }
    return false;
}

/*!* ######################### TS CHECKS ################################# *!*/

/*!
 * \brief A step of the TS on procedure and the conditions it checks
 */
struct MainboardLoggerCheck {
    const char *name;               /*!< Shown in the report */
    enum MainboardFsmStateID state; /*!< The state that runs the check */
    bit_flag32 mask;                /*!< The checked conditions */
    bit_flag32 expected;            /*!< The expected value of each condition */
};

EAGLETRT_STATIC const struct MainboardLoggerCheck mainboard_logger_checks[] = {
    { "WAIT_OK -> AIRN_CHECK",         MAINBOARD_FSM_STATE_WAIT_OK,         MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_MASK,         MAINBOARD_FSM_WAIT_OK_TO_AIRN_CHECK_EXPECTED         },
    { "AIRN_CHECK -> PRECHARGE_CHECK", MAINBOARD_FSM_STATE_AIRN_CHECK,      MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_MASK, MAINBOARD_FSM_AIRN_CHECK_TO_PRECHARGE_CHECK_EXPECTED },
    { "PRECHARGE_CHECK -> AIRP_CHECK", MAINBOARD_FSM_STATE_PRECHARGE_CHECK, MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_MASK, MAINBOARD_FSM_PRECHARGE_CHECK_TO_AIRP_CHECK_EXPECTED },
    { "AIRP_CHECK -> TS_ON",           MAINBOARD_FSM_STATE_AIRP_CHECK,      MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_MASK,           MAINBOARD_FSM_AIRP_CHECK_TO_TS_ON_EXPECTED           },
    { "TS_ON (hold)",                  MAINBOARD_FSM_STATE_TS_ON,           MAINBOARD_FSM_TS_ON_MASK,                         MAINBOARD_FSM_TS_ON_EXPECTED                         },
    { "DISCHARGE_TS -> IDLE",          MAINBOARD_FSM_STATE_DISCHARGE_TS,    MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_MASK,          MAINBOARD_FSM_DISCHARGE_TS_TO_IDLE_EXPECTED          },
};

/*!
 * \brief Snapshot of the TS checks report, the number of rows depends on the conditions
 */
struct MainboardLoggerChecksCursor {
    bit_flag32 conditions;          /*!< The conditions when the report started */
    enum MainboardFsmStateID state; /*!< The FSM state when the report started */
    size_t check;                   /*!< The check being printed */
    size_t condition;               /*!< The next condition of the check to look at */
    bool header_done;               /*!< The row with the name of the check was printed */
};

EAGLETRT_STATIC struct MainboardLoggerChecksCursor mainboard_logger_checks_cursor;

/*!
 * \brief Take the snapshot of the conditions, so that every check sees the same values
 */
EAGLETRT_STATIC void prv_mainboard_logger_checks_begin(void) {
    mainboard_logger_checks_cursor = (struct MainboardLoggerChecksCursor){
        .conditions = mainboard_status_api_get_conditions(),
        .state = mainboard_fsm_api_get_state(),
    };
}

/*!
 * \brief Rows: for each step of the TS on procedure, whether it would pass and the conditions blocking it
 */
EAGLETRT_STATIC bool prv_mainboard_logger_checks_row(const size_t row, struct MainboardLoggerLine *const line) {
    EAGLETRT_API_UNUSED(row);
    struct MainboardLoggerChecksCursor *const cursor = &mainboard_logger_checks_cursor;

    while (cursor->check < MAINBOARD_LOGGER_COUNT_OF(mainboard_logger_checks)) {
        const struct MainboardLoggerCheck *const check = &mainboard_logger_checks[cursor->check];
        const bit_flag32 mismatches = (cursor->conditions ^ check->expected) & check->mask;

        if (!cursor->header_done) {
            cursor->header_done = true;
            // The step run by the current state is marked
            int_mainboard_logger_line_append(line, "%s%-32s", (cursor->state == check->state) ? "> " : "  ", check->name);
            if (mismatches == 0U) {
                int_mainboard_logger_line_append(line, "✓ pass");
            } else {
                int_mainboard_logger_line_append(line, "✗ %u blocking", (unsigned)__builtin_popcount(mismatches));
            }
            return true;
        }

        while (cursor->condition < MAINBOARD_STATUS_CONDITION_COUNT && !EAGLETRT_API_BIT_GET(mismatches, cursor->condition)) {
            ++cursor->condition;
        }
        if (cursor->condition < MAINBOARD_STATUS_CONDITION_COUNT) {
            const enum MainboardStatusCondition condition = (enum MainboardStatusCondition)cursor->condition;
            const bool expected = (bool)EAGLETRT_API_BIT_GET(check->expected, condition);
            ++cursor->condition;
            int_mainboard_logger_line_append(line,
                                             "      ✗ %-30sexpected %-5s is %s",
                                             mainboard_status_api_get_condition_name(condition),
                                             expected ? "true" : "false",
                                             expected ? "false" : "true");
            return true;
        }

        ++cursor->check;
        cursor->condition = 0U;
        cursor->header_done = false;
    }
    return false;
}

/*!* ######################### STATUS #################################### *!*/

/*!
 * \brief Rows: FSM state and requests, then every fault and every condition, two per row
 */
EAGLETRT_STATIC bool prv_mainboard_logger_status_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t columns = 2U;
    constexpr size_t fault_rows = (MAINBOARD_STATUS_FAULT_COUNT + columns - 1U) / columns;
    constexpr size_t condition_rows = (MAINBOARD_STATUS_CONDITION_COUNT + columns - 1U) / columns;
    constexpr int request_width = 14;

    if (row == 0U) {
        int_mainboard_logger_line_append(line,
                                         "%-12s%-20sfaults %u",
                                         "FSM",
                                         mainboard_fsm_api_get_state_name(mainboard_fsm_api_get_state()),
                                         (unsigned)__builtin_popcount(mainboard_status_api_get_faults()));
        return true;
    }
    if (row == 1U) {
        int_mainboard_logger_line_append(line, "%-12s", "Requests");
        for (enum MainboardStatusRequest request = 0U; request < MAINBOARD_STATUS_REQUEST_COUNT; ++request) {
            prv_mainboard_logger_flag(line, mainboard_status_api_get_request_name(request), mainboard_status_api_is_request_set(request), request_width);
        }
        return true;
    }

    size_t index = row - 2U;
    if (index == 0U) {
        return true;
    }
    if (index == 1U) {
        int_mainboard_logger_line_append(line, "Faults  latched, never cleared");
        return true;
    }
    index -= 2U;
    if (index < fault_rows) {
        for (size_t i = index * columns; i < (index + 1U) * columns && i < MAINBOARD_STATUS_FAULT_COUNT; ++i) {
            const enum MainboardStatusFault fault = (enum MainboardStatusFault)i;
            prv_mainboard_logger_flag(line, mainboard_status_api_get_fault_name(fault), mainboard_status_api_is_fault_set(fault), MAINBOARD_LOGGER_HALF_WIDTH);
        }
        return true;
    }

    index -= fault_rows;
    if (index == 0U) {
        return true;
    }
    if (index == 1U) {
        int_mainboard_logger_line_append(line, "Conditions");
        return true;
    }
    index -= 2U;
    if (index < condition_rows) {
        for (size_t i = index * columns; i < (index + 1U) * columns && i < MAINBOARD_STATUS_CONDITION_COUNT; ++i) {
            const enum MainboardStatusCondition condition = (enum MainboardStatusCondition)i;
            prv_mainboard_logger_flag(line, mainboard_status_api_get_condition_name(condition), mainboard_status_api_get_condition(condition), MAINBOARD_LOGGER_HALF_WIDTH);
        }
        return true;
    }
    return false;
}

/*!* ######################### CELLBOARDS ################################ *!*/

/*!
 * \brief Append a version, or "unknown" if it was never received
 *
 * \param line The line
 * \param label The label
 * \param version The version
 * \param with_commit Also show the commit hash
 */
EAGLETRT_STATIC void prv_mainboard_logger_version(struct MainboardLoggerLine *const line,
                                                  const char *const label,
                                                  const struct CellboardsMonitorVersion version,
                                                  const bool with_commit) {
    int_mainboard_logger_line_append(line, "%s ", label);
    if (version.major == 0U && version.minor == 0U && version.patch == 0U && version.commit_hash == 0U) {
        int_mainboard_logger_line_append(line, "%-17s", "unknown");
        return;
    }
    char text[MAINBOARD_LOGGER_VERSION_SIZE];
    const int size = with_commit
                         ? snprintf(text, sizeof(text), "%u.%u.%u %07" PRIx32 "%s", version.major, version.minor, version.patch, version.commit_hash, version.dirty ? "*" : "")
                         : snprintf(text, sizeof(text), "%u.%u.%u", version.major, version.minor, version.patch);
    int_mainboard_logger_line_append(line, "%-17s", (size > 0) ? text : "?");
}

/*!
 * \brief Rows: for each cellboard its state and versions, then its errors and open wires
 */
EAGLETRT_STATIC bool prv_mainboard_logger_cellboards_row(const size_t row, struct MainboardLoggerLine *const line) {
    // State row, errors row and empty row
    constexpr size_t rows_per_cellboard = 3U;
    const enum CellboardId cellboard = (enum CellboardId)(row / rows_per_cellboard);
    const size_t sub_row = row % rows_per_cellboard;

    if (cellboard >= CELLBOARD_ID_COUNT || (cellboard == CELLBOARD_ID_COUNT - 1U && sub_row == rows_per_cellboard - 1U)) {
        return false;
    }

    if (sub_row == 0U) {
        int_mainboard_logger_line_append(line, "CB%u   %-12s", (unsigned)cellboard, prv_mainboard_logger_cellboard_state_name(cellboards_monitor_api_get_state(cellboard)));
        prv_mainboard_logger_version(line, "firmware", cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_FIRMWARE), true);
        int_mainboard_logger_line_append(line, "  ");
        prv_mainboard_logger_version(line, "libcan", cellboards_monitor_api_get_version(cellboard, CELLBOARDS_MONITOR_VERSION_LIBCAN), false);
        return true;
    }

    if (sub_row == 1U) {
        const bit_flag16 errors = cellboards_monitor_api_get_errors(cellboard);
        const bit_flag16 open_wires = cellboards_monitor_api_get_open_wires(cellboard);

        int_mainboard_logger_line_append(line, "      errors %s", (errors == 0U) ? "none" : "");
        for (size_t i = 0U; i < CELLBOARDS_MONITOR_ERROR_COUNT; ++i) {
            if (EAGLETRT_API_BIT_GET(errors, i)) {
                int_mainboard_logger_line_append(line, "%s ", mainboard_logger_cellboard_error_names[i]);
            }
        }

        int_mainboard_logger_line_append(line, "   open wires %s", (open_wires == 0U) ? "none" : "");
        for (size_t i = 0U; i < CELLBOARDS_MONITOR_OPEN_WIRE_COUNT; ++i) {
            // Bit N is open wire N + 1
            if (EAGLETRT_API_BIT_GET(open_wires, i)) {
                int_mainboard_logger_line_append(line, "%u ", (unsigned)(i + 1U));
            }
        }
        return true;
    }

    return true;
}

/*!* ######################### CAN ####################################### *!*/

/*! \brief Columns of the labels of the CAN report */
#define MAINBOARD_LOGGER_CAN_LABEL_WIDTH (22)

/*! \brief Columns of the value of each network in the CAN report */
#define MAINBOARD_LOGGER_CAN_COLUMN_WIDTH (26)

/*! \brief Size of the text of a cell of the CAN report */
#define MAINBOARD_LOGGER_CAN_CELL_SIZE (32U)

/*! \brief Rows of the transceiver state in the CAN report */
#define MAINBOARD_LOGGER_CAN_STATE_ROWS (7U)

/*!
 * \brief Status of every network, read once when the CAN report starts so that its rows agree
 */
struct MainboardLoggerCanSnapshot {
    bool valid[CAN_NETWORK_COUNT];                                      /*!< The transceiver status was read */
    struct CanCommunicationTransceiverStatus status[CAN_NETWORK_COUNT]; /*!< The transceiver status */
    struct CanCommunicationQueueCounters queues[CAN_NETWORK_COUNT];     /*!< The counters of the queues */
};

EAGLETRT_STATIC struct MainboardLoggerCanSnapshot mainboard_logger_can_snapshot;

/*!
 * \brief A counter shown by the CAN report
 */
struct MainboardLoggerCanCounter {
    const char *label; /*!< Shown in the first column, NULL for an empty row */
    bool queue;        /*!< The counter is in the queue counters, else in the transceiver status */
    size_t offset;     /*!< Offset of the uint32_t counter in its structure */
};

/*! \brief Initializer of a counter of the transceiver status */
#define MAINBOARD_LOGGER_CAN_TRANSCEIVER(label, field) \
    { (label), false, offsetof(struct CanCommunicationTransceiverStatus, field) }

/*! \brief Initializer of a counter of the queues */
#define MAINBOARD_LOGGER_CAN_QUEUE(label, field) \
    { (label), true, offsetof(struct CanCommunicationQueueCounters, field) }

/*! \brief An empty row between the counters */
#define MAINBOARD_LOGGER_CAN_EMPTY \
    { NULL, false, 0U }

EAGLETRT_STATIC const struct MainboardLoggerCanCounter mainboard_logger_can_counters[] = {
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("TX requested", tx_requested),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("TX completed", tx_completed),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("TX lost arbitration", tx_arbitration_lost),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("TX bus errors", tx_errors),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("TX mailboxes busy", tx_mailboxes_busy),
    MAINBOARD_LOGGER_CAN_QUEUE("TX queue full", tx_dropped),
    MAINBOARD_LOGGER_CAN_QUEUE("TX send failed", tx_failed),
    MAINBOARD_LOGGER_CAN_EMPTY,
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("RX received", rx_received),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("RX FIFO overruns", rx_fifo_overruns),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("RX queue full", rx_queue_full),
    MAINBOARD_LOGGER_CAN_QUEUE("RX rejected", rx_rejected),
    MAINBOARD_LOGGER_CAN_EMPTY,
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("Bus errors (sampled)", bus_errors),
    MAINBOARD_LOGGER_CAN_TRANSCEIVER("Bus-off events", bus_off_count),
};

#undef MAINBOARD_LOGGER_CAN_TRANSCEIVER
#undef MAINBOARD_LOGGER_CAN_QUEUE
#undef MAINBOARD_LOGGER_CAN_EMPTY

/*!
 * \brief Read the status of every network
 */
EAGLETRT_STATIC void prv_mainboard_logger_can_begin(void) {
    for (enum CanNetwork network = 0; network < CAN_NETWORK_COUNT; ++network) {
        mainboard_logger_can_snapshot.valid[network] = can_communication_api_get_status(network, &mainboard_logger_can_snapshot.status[network]) == CAN_COMMUNICATION_RC_OK;
        mainboard_logger_can_snapshot.queues[network] = can_communication_api_get_queue_counters(network);
    }
}

/*!
 * \brief Fill a row of the transceiver state of the CAN report, one column per network
 *
 * \param index The index of the state row, less than MAINBOARD_LOGGER_CAN_STATE_ROWS
 * \param line The line
 */
EAGLETRT_STATIC void prv_mainboard_logger_can_state_row(const size_t index, struct MainboardLoggerLine *const line) {
    static const char *const labels[MAINBOARD_LOGGER_CAN_STATE_ROWS] = { "Bitrate", "State", "TEC / REC", "Last bus error", "TX mailboxes free", "RX FIFO pending", "RX filter" };

    int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_LABEL_WIDTH, labels[index]);
    for (enum CanNetwork network = 0; network < CAN_NETWORK_COUNT; ++network) {
        const struct CanCommunicationTransceiverStatus *const status = &mainboard_logger_can_snapshot.status[network];
        char text[MAINBOARD_LOGGER_CAN_CELL_SIZE] = "n/a";

        if (mainboard_logger_can_snapshot.valid[network]) {
            switch (index) {
                case 0U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%" PRIu32 " kbit/s", status->bitrate / 1000U));
                    break;
                case 1U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%s", can_communication_api_get_bus_state_name(status->state)));
                    break;
                case 2U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%u / %u", (unsigned)status->tx_error_counter, (unsigned)status->rx_error_counter));
                    break;
                case 3U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%s", can_communication_api_get_bus_error_name(status->last_error)));
                    break;
                case 4U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%u / %u", (unsigned)status->tx_mailboxes_free, (unsigned)status->tx_mailboxes_count));
                    break;
                case 5U:
                    EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%u / %u", (unsigned)status->rx_fifo_pending, (unsigned)status->rx_fifo_size));
                    break;
                case 6U:
                default:
                    if (status->rx_filter_entries > 0U) {
                        EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "%u entries", (unsigned)status->rx_filter_entries));
                    } else {
                        EAGLETRT_API_UNUSED(snprintf(text, sizeof(text), "all frames"));
                    }
                    break;
            }
        }
        int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_COLUMN_WIDTH, text);
    }
}

/*!
 * \brief Fill a row of the counters of the CAN report, one column per network
 *
 * \param counter The counter
 * \param line The line
 */
EAGLETRT_STATIC void prv_mainboard_logger_can_counter_row(const struct MainboardLoggerCanCounter *const counter, struct MainboardLoggerLine *const line) {
    if (counter->label == NULL) {
        return;
    }
    int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_LABEL_WIDTH, counter->label);

    for (enum CanNetwork network = 0; network < CAN_NETWORK_COUNT; ++network) {
        if (!counter->queue && !mainboard_logger_can_snapshot.valid[network]) {
            int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_COLUMN_WIDTH, "n/a");
            continue;
        }
        const void *const base = counter->queue ? (const void *)&mainboard_logger_can_snapshot.queues[network]
                                                : (const void *)&mainboard_logger_can_snapshot.status[network];
        uint32_t value = 0U;
        EAGLETRT_API_UNUSED(memcpy(&value, (const uint8_t *)base + counter->offset, sizeof(value)));
        int_mainboard_logger_line_append(line, "%-*" PRIu32, MAINBOARD_LOGGER_CAN_COLUMN_WIDTH, value);
    }
}

/*!
 * \brief Rows: the networks as columns, their transceiver state, then the frame and error counters, then the bridge
 */
EAGLETRT_STATIC bool prv_mainboard_logger_can_row(const size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t counter_count = MAINBOARD_LOGGER_COUNT_OF(mainboard_logger_can_counters);

    if (row == 0U) {
        int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_LABEL_WIDTH, "");
        for (enum CanNetwork network = 0; network < CAN_NETWORK_COUNT; ++network) {
            int_mainboard_logger_line_append(line, "%-*s", MAINBOARD_LOGGER_CAN_COLUMN_WIDTH, can_communication_api_get_network_name(network));
        }
        return true;
    }

    size_t index = row - 1U;
    if (index < MAINBOARD_LOGGER_CAN_STATE_ROWS) {
        prv_mainboard_logger_can_state_row(index, line);
        return true;
    }
    index -= MAINBOARD_LOGGER_CAN_STATE_ROWS;
    if (index == 0U) {
        return true;
    }
    --index;
    if (index < counter_count) {
        prv_mainboard_logger_can_counter_row(&mainboard_logger_can_counters[index], line);
        return true;
    }
    index -= counter_count;
    if (index == 0U) {
        return true;
    }
    if (index == 1U) {
        prv_mainboard_logger_flag(line, "bridge enabled", can_bridge_api_is_enabled(), 0);
        return true;
    }
    return false;
}

/*!* ######################### TABLE ##################################### *!*/

EAGLETRT_STATIC const struct MainboardLoggerReportInfo mainboard_logger_reports[MAINBOARD_LOGGER_REPORT_COUNT] = {
    [MAINBOARD_LOGGER_REPORT_HELP] = { 'h', "Help",         "this list",                                                  NULL,                              int_mainboard_logger_api_help_row     },
    [MAINBOARD_LOGGER_REPORT_VOLTAGES] = { 'v', "Voltages",     "cell voltages of every cellboard",                           NULL,                              prv_mainboard_logger_voltages_row     },
    [MAINBOARD_LOGGER_REPORT_TEMPERATURES] = { 't', "Temperatures", "cell and discharge resistor temperatures",                   NULL,                              prv_mainboard_logger_temperatures_row },
    [MAINBOARD_LOGGER_REPORT_INTERNAL] = { 'i', "Internal",     "TS and pack voltage, current, power, precharge, cooling",    NULL,                              prv_mainboard_logger_internal_row     },
    [MAINBOARD_LOGGER_REPORT_IMD] = { 'm', "IMD",          "insulation monitoring device",                               NULL,                              prv_mainboard_logger_imd_row          },
    [MAINBOARD_LOGGER_REPORT_PCU] = { 'p', "PCU",          "relays: commanded, read back and mechanical state",          NULL,                              prv_mainboard_logger_pcu_row          },
    [MAINBOARD_LOGGER_REPORT_BALANCING] = { 'b', "Balancing",    "balancing control and cells being discharged",               NULL,                              prv_mainboard_logger_balancing_row    },
    [MAINBOARD_LOGGER_REPORT_FEEDBACK] = { 'f', "Feedback",     "raw digital and analog feedback",                            NULL,                              prv_mainboard_logger_feedback_row     },
    [MAINBOARD_LOGGER_REPORT_CHECKS] = { 'k', "TS checks",    "conditions blocking each step of the TS on procedure",       prv_mainboard_logger_checks_begin, prv_mainboard_logger_checks_row       },
    [MAINBOARD_LOGGER_REPORT_STATUS] = { 'e', "Status",       "FSM state, requests, faults and conditions",                 NULL,                              prv_mainboard_logger_status_row       },
    [MAINBOARD_LOGGER_REPORT_CELLBOARDS] = { 'c', "Cellboards",   "state, errors, open wires and versions",                     NULL,                              prv_mainboard_logger_cellboards_row   },
    [MAINBOARD_LOGGER_REPORT_CAN] = { 'n', "CAN",          "transceivers: bus state, error counters, frames and errors", prv_mainboard_logger_can_begin,    prv_mainboard_logger_can_row          },
};

const struct MainboardLoggerReportInfo *int_mainboard_logger_reports_get(const enum MainboardLoggerReport report) {
    if (report >= MAINBOARD_LOGGER_REPORT_COUNT) {
        return NULL;
    }
    return &mainboard_logger_reports[report];
}
