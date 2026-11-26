/*!
 * \file mainboard-logger.h
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard logger and its serial console
 */

#ifndef MAINBOARD_LOGGER_H
#define MAINBOARD_LOGGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "logger.h"
#include "mainboard-def.h"

/*!
 * \brief Size of a live log line, timestamp and column padding included
 *
 * \details The message formatted by liblog is at most LOGGER_EMIT_BUFFER_SIZE
 *          bytes, the rest is for the timestamp and the padding of the columns
 */
#define MAINBOARD_LOGGER_MESSAGE_SIZE ((LOGGER_EMIT_BUFFER_SIZE) + 64U)

/*!
 * \brief Size of a report line, line ending included
 */
#define MAINBOARD_LOGGER_LINE_SIZE (256U)

/*!
 * \brief Number of elements of an array
 */
#define MAINBOARD_LOGGER_COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/*!
 * \brief Callback used to get the current time
 *
 * \returns The time elapsed since boot in ms, it must be monotonic
 */
typedef milliseconds (*mainboard_logger_tick_callback)(void);

/*!
 * \brief Callback used to read the commands typed in the console
 *
 * \param out[out] The array where the received bytes are copied
 * \param size The maximum number of bytes to copy
 *
 * \returns The number of bytes copied in out, 0 if nothing was received
 */
typedef size_t (*mainboard_logger_read_callback)(char *out, size_t size);

/*!
 * \brief Callback used to get the free space of the output
 *
 * \returns The maximum number of bytes that the write callback can accept right now
 */
typedef size_t (*mainboard_logger_free_space_callback)(void);

/*!
 * \brief Data needed to initialize the logger
 */
struct MainboardLoggerInitData {
    logger_write_callback write;                     /*!< Sends a full message, it must copy the data (e.g. the non-blocking USART) */
    mainboard_logger_read_callback read;             /*!< Reads the console input */
    mainboard_logger_free_space_callback free_space; /*!< Free space of the output, used to pace the reports */
    mainboard_logger_tick_callback get_tick;         /*!< Time since boot in ms */
    system_reset_callback reset;                     /*!< Resets the microcontroller, used by the reset command */
    uint8_t verbosity;                               /*!< The enabled levels as a bit flag of LoggerLevel values */
};

/*!
 * \brief The reports that the console can print
 */
enum MainboardLoggerReport : uint8_t {
    MAINBOARD_LOGGER_REPORT_HELP,         /*!< The list of commands */
    MAINBOARD_LOGGER_REPORT_VOLTAGES,     /*!< Cell voltages */
    MAINBOARD_LOGGER_REPORT_TEMPERATURES, /*!< Cell and discharge resistor temperatures */
    MAINBOARD_LOGGER_REPORT_INTERNAL,     /*!< TS and pack voltages, current, power, internal temperatures */
    MAINBOARD_LOGGER_REPORT_IMD,          /*!< Insulation monitoring device */
    MAINBOARD_LOGGER_REPORT_PCU,          /*!< Relays and precharge */
    MAINBOARD_LOGGER_REPORT_BALANCING,    /*!< Balancing control and cells being discharged */
    MAINBOARD_LOGGER_REPORT_FEEDBACK,     /*!< Raw digital and analog feedback */
    MAINBOARD_LOGGER_REPORT_CHECKS,       /*!< Conditions checked by every step of the TS on procedure */
    MAINBOARD_LOGGER_REPORT_STATUS,       /*!< Faults, requests and conditions */
    MAINBOARD_LOGGER_REPORT_CELLBOARDS,   /*!< State, errors, open wires and version of the cellboards */
    MAINBOARD_LOGGER_REPORT_CAN,          /*!< State, error counters and frame counters of the CAN transceivers */
    MAINBOARD_LOGGER_REPORT_COUNT         /*!< Number of reports, also used as "no report" */
};

/*!
 * \brief A line of a report
 *
 * \details An empty line separates the groups of a report
 */
struct MainboardLoggerLine {
    char text[MAINBOARD_LOGGER_LINE_SIZE]; /*!< The content, always terminated */
    size_t size;                           /*!< The length of text in bytes */
};

/*!
 * \brief Called once when a report starts, e.g. to take a snapshot of the data it shows
 */
typedef void (*mainboard_logger_report_begin_callback)(void);

/*!
 * \brief Fill a line of a report
 *
 * \details The rows are always requested in order starting from 0, and each
 *          row is requested exactly once per report
 *
 * \param row The index of the row
 * \param line[out] The line to fill, empty when the function is called
 *
 * \returns true if the line was filled, false if the report has no more rows
 */
typedef bool (*mainboard_logger_report_row_callback)(size_t row, struct MainboardLoggerLine *line);

/*!
 * \brief Description of a report
 */
struct MainboardLoggerReportInfo {
    char key;                                     /*!< The key that prints the report */
    const char *title;                            /*!< Shown in the title line */
    const char *description;                      /*!< Shown in the help */
    mainboard_logger_report_begin_callback begin; /*!< Optional, called when the report starts */
    mainboard_logger_report_row_callback row;     /*!< Fills the rows of the report */
};

/*!
 * \brief State of the mainboard logger and of its console
 */
struct MainboardLoggerHandler {
    logger_write_callback write;                     /*!< See MainboardLoggerInitData */
    mainboard_logger_read_callback read;             /*!< See MainboardLoggerInitData */
    mainboard_logger_free_space_callback free_space; /*!< See MainboardLoggerInitData */
    mainboard_logger_tick_callback get_tick;         /*!< See MainboardLoggerInitData */
    system_reset_callback reset;                     /*!< See MainboardLoggerInitData */
    uint8_t verbosity;                               /*!< The verbosity given at init, restored when leaving the quiet mode */
    bool quiet;                                      /*!< Only warnings and errors are logged */

    enum MainboardLoggerReport selected; /*!< The last data report asked, printed again by the space key */
    enum MainboardLoggerReport printing; /*!< The report being printed, COUNT if none */
    bool title_done;                     /*!< The title line of the report being printed was prepared */
    size_t row;                          /*!< The next row of the report being printed */
    bool pending;                        /*!< line holds a line that could not be sent yet */
    struct MainboardLoggerLine line;     /*!< The line ready to be sent */

    bool reset_armed;                /*!< The reset key was pressed once, a second press resets */
    milliseconds reset_armed_at;     /*!< When the reset key was pressed the first time */
    bool reset_pending;              /*!< The reset is confirmed, it happens once the last logs are sent */
    milliseconds reset_requested_at; /*!< When the reset was confirmed */
};

#endif // MAINBOARD_LOGGER_H
