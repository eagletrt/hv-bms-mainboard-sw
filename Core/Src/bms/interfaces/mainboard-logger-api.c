/*!
 * \file mainboard-logger-api.c
 * \date 2026-11-25
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Timestamped logger and command driven serial console
 */

#include "mainboard-logger-api.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "bal-control-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-fsm-api.h"
#include "mainboard-logger-reports-api.h"
#include "mainboard-status-api.h"
#include "request-control-api.h"

EAGLETRT_STATIC struct MainboardLoggerHandler mainboard_logger_handler;

/*! \brief Milliseconds in a second, used by the timestamps */
#define MAINBOARD_LOGGER_MS_PER_S (1000U)

/*! \brief Key of the reset command, pressed twice to confirm */
#define MAINBOARD_LOGGER_RESET_KEY ('z')

/*! \brief Columns of the source ("file.c:line") of the live logs */
#define MAINBOARD_LOGGER_SOURCE_WIDTH (34)

/*! \brief Bytes read from the console at every routine */
#define MAINBOARD_LOGGER_INPUT_SIZE (16U)

/*! \brief The levels shown in quiet mode */
#define MAINBOARD_LOGGER_QUIET_LEVELS ((uint8_t)(LOGGER_LEVEL_WARNING | LOGGER_LEVEL_ERROR))

/*!
 * \brief A command of the console that is not a report
 */
struct MainboardLoggerAction {
    char key;                /*!< The key that runs the command */
    const char *label;       /*!< How the key is shown in the help */
    const char *description; /*!< Shown in the help */
    void (*run)(void);       /*!< Runs the command */
};

/*!
 * \brief Write callback given to liblog, lays the message out in columns
 *
 * \details liblog formats the messages as "[LEVEL] @file.c:line >> message\n\r",
 *          they are rewritten as
 *          \code
 *               1.234  INFO   file.c:line                         message
 *          \endcode
 *          A message in an unexpected format is sent as it is after the timestamp.
 *          The line is sent with a single write so that it can not be split by other data
 *
 * \param msg The message formatted by liblog
 * \param size The length of msg in bytes
 *
 * \returns The value returned by the write callback, or -1 if the message can not be formatted
 */
EAGLETRT_STATIC int prv_mainboard_logger_write(const char *const msg, size_t size) {
    static char data[MAINBOARD_LOGGER_MESSAGE_SIZE];

    // liblog layout, the level is padded to 5 characters
    constexpr size_t level_start = 1U;
    constexpr size_t level_size = 5U;
    constexpr size_t source_start = 9U;
    static const char separator[] = " >> ";
    constexpr size_t separator_size = sizeof(separator) - 1U;

    // The line ending is added back at the end of the layout
    while (size > 0U && (msg[size - 1U] == '\n' || msg[size - 1U] == '\r')) {
        --size;
    }

    // Search the end of the source
    size_t source_end = size;
    if (size > source_start && msg[0] == '[' && msg[level_start + level_size] == ']' && msg[source_start - 1U] == '@') {
        for (size_t i = source_start; i + separator_size <= size; ++i) {
            if (memcmp(&msg[i], separator, separator_size) == 0) {
                source_end = i;
                break;
            }
        }
    }

    const milliseconds now = mainboard_logger_handler.get_tick();
    const uint32_t seconds = (uint32_t)(now / MAINBOARD_LOGGER_MS_PER_S);
    const uint32_t millis = (uint32_t)(now % MAINBOARD_LOGGER_MS_PER_S);
    int written = 0;
    if (source_end == size) {
        written = snprintf(data, sizeof(data), "%7" PRIu32 ".%03" PRIu32 "  %.*s\r\n", seconds, millis, (int)size, msg);
    } else {
        const size_t body_start = source_end + separator_size;
        written = snprintf(data,
                           sizeof(data),
                           "%7" PRIu32 ".%03" PRIu32 "  %.*s  %-*.*s  %.*s\r\n",
                           seconds,
                           millis,
                           (int)level_size,
                           &msg[level_start],
                           MAINBOARD_LOGGER_SOURCE_WIDTH,
                           (int)(source_end - source_start),
                           &msg[source_start],
                           (int)(size - body_start),
                           &msg[body_start]);
    }
    if (written < 0) {
        return -1;
    }

    size_t length = (size_t)written;
    if (length >= sizeof(data)) {
        // The message is cut, the line ending is kept
        length = sizeof(data) - 1U;
        data[length - 2U] = '\r';
        data[length - 1U] = '\n';
    }
    return mainboard_logger_handler.write(data, length);
}

/*!
 * \brief Add the line ending to a line, cutting the content if it does not fit
 *
 * \param line The line
 */
EAGLETRT_STATIC void prv_mainboard_logger_end_line(struct MainboardLoggerLine *const line) {
    static const char ending[] = "\r\n";
    constexpr size_t ending_size = sizeof(ending) - 1U;
    if (line->size > sizeof(line->text) - sizeof(ending)) {
        line->size = sizeof(line->text) - sizeof(ending);
    }
    memcpy(&line->text[line->size], ending, sizeof(ending));
    line->size += ending_size;
}

/*!
 * \brief Print the welcome banner with the firmware version
 *
 * \details Sent at boot, when the output is still empty
 */
EAGLETRT_STATIC void prv_mainboard_logger_print_banner(void) {
    struct MainboardLoggerLine *const line = &mainboard_logger_handler.line;
    *line = (struct MainboardLoggerLine){ 0 };

#ifdef GIT_COMMIT
    constexpr uint32_t commit = (uint32_t)(GIT_COMMIT);
    constexpr bool dirty = (bool)(GIT_DIRTY);
#else
    constexpr uint32_t commit = 0U;
    constexpr bool dirty = false;
#endif

    int_mainboard_logger_line_append(line,
                                     "\r\nHV BMS mainboard · firmware %u.%u.%u · commit %07" PRIx32 "%s · built %s\r\n"
                                     "press h for the list of commands\r\n",
                                     (unsigned)MAINBOARD_VERSION_MAJOR,
                                     (unsigned)MAINBOARD_VERSION_MINOR,
                                     (unsigned)MAINBOARD_VERSION_PATCH,
                                     commit,
                                     dirty ? "*" : "",
                                     MAINBOARD_BUILD_TIME_STR);
    prv_mainboard_logger_end_line(line);
    EAGLETRT_API_UNUSED(mainboard_logger_handler.write(line->text, line->size));
}

/*!
 * \brief Start printing a report, the one being printed is abandoned
 *
 * \param report The report to print
 */
EAGLETRT_STATIC void prv_mainboard_logger_start_report(const enum MainboardLoggerReport report) {
    const struct MainboardLoggerReportInfo *const info = int_mainboard_logger_reports_get(report);
    if (info == NULL) {
        return;
    }
    if (info->begin != NULL) {
        info->begin();
    }
    mainboard_logger_handler.printing = report;
    mainboard_logger_handler.title_done = false;
    mainboard_logger_handler.row = 0U;
    mainboard_logger_handler.pending = false;
}

/*!
 * \brief Prepare the next line of the report being printed
 *
 * \details The first line is the title, preceded by an empty line
 *
 * \returns true if a line is ready, false if there is nothing to print
 */
EAGLETRT_STATIC bool prv_mainboard_logger_prepare_line(void) {
    struct MainboardLoggerHandler *const handler = &mainboard_logger_handler;
    const struct MainboardLoggerReportInfo *const info = int_mainboard_logger_reports_get(handler->printing);
    if (info == NULL) {
        return false;
    }

    struct MainboardLoggerLine *const line = &handler->line;
    *line = (struct MainboardLoggerLine){ 0 };

    if (!handler->title_done) {
        handler->title_done = true;
        const milliseconds now = handler->get_tick();
        int_mainboard_logger_line_append(line,
                                         "\r\n%s · FSM %s · %" PRIu32 ".%03" PRIu32 " s",
                                         info->title,
                                         mainboard_fsm_api_get_state_name(mainboard_fsm_api_get_state()),
                                         (uint32_t)(now / MAINBOARD_LOGGER_MS_PER_S),
                                         (uint32_t)(now % MAINBOARD_LOGGER_MS_PER_S));
    } else if (info->row(handler->row, line)) {
        ++handler->row;
    } else {
        handler->printing = MAINBOARD_LOGGER_REPORT_COUNT;
        return false;
    }

    prv_mainboard_logger_end_line(line);
    return true;
}

/*!
 * \brief Send the next lines of the report being printed, as long as the output has room for them
 */
EAGLETRT_STATIC void prv_mainboard_logger_print_lines(void) {
    struct MainboardLoggerHandler *const handler = &mainboard_logger_handler;

    for (size_t i = 0U; i < MAINBOARD_LOGGER_REPORT_LINES_PER_RUN; ++i) {
        if (!handler->pending) {
            if (!prv_mainboard_logger_prepare_line()) {
                return;
            }
            handler->pending = true;
        }
        // Leave room for the live logs, the line is retried at the next run
        if (handler->free_space() < handler->line.size + MAINBOARD_LOGGER_REPORT_TX_HEADROOM) {
            return;
        }
        if (handler->write(handler->line.text, handler->line.size) < 0) {
            return;
        }
        handler->pending = false;
    }
}

/*! \brief Command: print the last data report again */
EAGLETRT_STATIC void prv_mainboard_logger_action_reprint(void) {
    prv_mainboard_logger_start_report(mainboard_logger_handler.selected);
}

/*! \brief Command: turn the quiet mode (only warnings and errors) on or off */
EAGLETRT_STATIC void prv_mainboard_logger_action_toggle_quiet(void) {
    if (mainboard_logger_handler.quiet) {
        mainboard_logger_handler.quiet = false;
        logger_api_set_verbosity(NULL, mainboard_logger_handler.verbosity);
        MAINBOARD_LOGGER_INFO("Console: quiet mode off");
    } else {
        // Logged before the info level is muted
        MAINBOARD_LOGGER_INFO("Console: quiet mode on, only warnings and errors are logged");
        mainboard_logger_handler.quiet = true;
        logger_api_set_verbosity(NULL, mainboard_logger_handler.verbosity & MAINBOARD_LOGGER_QUIET_LEVELS);
    }
}

#if MAINBOARD_LOGGER_ENABLE_ACTIONS

/*!
 * \brief Send a TS request through the request control, like the ECU does over CAN
 *
 * \param state The requested state
 */
EAGLETRT_STATIC void prv_mainboard_logger_request_ts(const enum RequestControlTSRequestState state) {
    const struct RequestControlRequest request = {
        .request_type = REQUEST_CONTROL_REQUEST_TS,
        .request_data = { .ts_request_state = state },
    };
    const bool on = (state == REQUEST_CONTROL_TS_REQUEST_STATE_ON);
    if (request_control_api_handle_request(request) == REQUEST_CONTROL_RC_OK) {
        MAINBOARD_LOGGER_INFO("Console: TS %s requested (FSM in %s)", on ? "on" : "off", mainboard_fsm_api_get_state_name(mainboard_fsm_api_get_state()));
    } else {
        MAINBOARD_LOGGER_WARNING("Console: TS %s request rejected", on ? "on" : "off");
    }
}

/*! \brief Command: request the TS on */
EAGLETRT_STATIC void prv_mainboard_logger_action_ts_on(void) {
    prv_mainboard_logger_request_ts(REQUEST_CONTROL_TS_REQUEST_STATE_ON);
}

/*! \brief Command: request the TS off */
EAGLETRT_STATIC void prv_mainboard_logger_action_ts_off(void) {
    prv_mainboard_logger_request_ts(REQUEST_CONTROL_TS_REQUEST_STATE_OFF);
}

/*! \brief Command: start or stop the balancing */
EAGLETRT_STATIC void prv_mainboard_logger_action_toggle_balancing(void) {
    const bool start = !mainboard_status_api_is_request_set(MAINBOARD_STATUS_REQUEST_BALANCING);
    const struct RequestControlRequest request = {
        .request_type = REQUEST_CONTROL_REQUEST_BALANCING,
        .request_data = {
                         .balancing_request_data = {
                .start = start,
                .parameters = { .threshold = start ? MAINBOARD_LOGGER_BALANCING_THRESHOLD_V : bal_control_api_get_threshold() },
            },
                         },
    };
    if (request_control_api_handle_request(request) == REQUEST_CONTROL_RC_OK) {
        MAINBOARD_LOGGER_INFO("Console: balancing %s requested", start ? "start" : "stop");
    } else {
        MAINBOARD_LOGGER_WARNING("Console: balancing %s request rejected", start ? "start" : "stop");
    }
}

/*!
 * \brief Command: reset the microcontroller, the key must be pressed twice
 *
 * \details The reset itself is done by the routine after MAINBOARD_LOGGER_RESET_DELAY_MS
 *          so that the last logs are sent
 */
EAGLETRT_STATIC void prv_mainboard_logger_action_reset(void) {
    struct MainboardLoggerHandler *const handler = &mainboard_logger_handler;
    if (handler->reset_pending) {
        return;
    }
    const milliseconds now = handler->get_tick();
    if (handler->reset_armed && now - handler->reset_armed_at <= MAINBOARD_LOGGER_RESET_CONFIRM_MS) {
        handler->reset_armed = false;
        handler->reset_pending = true;
        handler->reset_requested_at = now;
        MAINBOARD_LOGGER_WARNING("Console: resetting the board (FSM in %s)", mainboard_fsm_api_get_state_name(mainboard_fsm_api_get_state()));
        return;
    }
    handler->reset_armed = true;
    handler->reset_armed_at = now;
    MAINBOARD_LOGGER_WARNING("Console: press %c again within %u s to reset the board", MAINBOARD_LOGGER_RESET_KEY, (unsigned int)(MAINBOARD_LOGGER_RESET_CONFIRM_MS / MAINBOARD_LOGGER_MS_PER_S));
}

#endif // MAINBOARD_LOGGER_ENABLE_ACTIONS

/*! \brief The commands that are not reports, the reports are listed by the reports module */
EAGLETRT_STATIC const struct MainboardLoggerAction mainboard_logger_actions[] = {
    { ' ',                        "space", "print the last report again",                            prv_mainboard_logger_action_reprint          },
    { 'x',                        "x",     "quiet mode on/off, only warnings and errors are logged", prv_mainboard_logger_action_toggle_quiet     },
#if MAINBOARD_LOGGER_ENABLE_ACTIONS
    { 'q',                        "q",     "request TS on",                                          prv_mainboard_logger_action_ts_on            },
    { 'w',                        "w",     "request TS off",                                         prv_mainboard_logger_action_ts_off           },
    { 's',                        "s",     "start/stop balancing",                                   prv_mainboard_logger_action_toggle_balancing },
    { MAINBOARD_LOGGER_RESET_KEY, "z z",   "reset the board, press twice to confirm",                prv_mainboard_logger_action_reset            },
#endif
};

bool int_mainboard_logger_api_help_row(size_t row, struct MainboardLoggerLine *const line) {
    constexpr size_t report_count = MAINBOARD_LOGGER_REPORT_COUNT;
    constexpr size_t action_count = MAINBOARD_LOGGER_COUNT_OF(mainboard_logger_actions);

    if (row == 0U) {
        int_mainboard_logger_line_append(line, "Reports, print the state of a part of the BMS");
        return true;
    }
    row -= 1U;
    if (row < report_count) {
        const struct MainboardLoggerReportInfo *const info = int_mainboard_logger_reports_get((enum MainboardLoggerReport)row);
        int_mainboard_logger_line_append(line, "  %-8c%-14s%s", info->key, info->title, info->description);
        return true;
    }
    row -= report_count;

    // An empty line, then the commands
    if (row == 0U) {
        return true;
    }
    if (row == 1U) {
        int_mainboard_logger_line_append(line, "Commands");
        return true;
    }
    row -= 2U;
    if (row < action_count) {
        const struct MainboardLoggerAction *const action = &mainboard_logger_actions[row];
        int_mainboard_logger_line_append(line, "  %-8s%s", action->label, action->description);
        return true;
    }
    row -= action_count;

    if (row == 0U) {
        return true;
    }
    if (row == 1U) {
        int_mainboard_logger_line_append(line,
                                         "quiet mode %s · car commands %s · keys are case insensitive",
                                         mainboard_logger_handler.quiet ? "on" : "off",
                                         MAINBOARD_LOGGER_ENABLE_ACTIONS ? "enabled" : "disabled");
        return true;
    }
    return false;
}

enum LoggerReturnCode mainboard_logger_api_init(const struct MainboardLoggerInitData *const data) {
    if (data == NULL || data->write == NULL || data->read == NULL || data->free_space == NULL || data->get_tick == NULL || data->reset == NULL) {
        return LOGGER_RC_NULL_POINTER;
    }

    mainboard_logger_handler = (struct MainboardLoggerHandler){
        .write = data->write,
        .read = data->read,
        .free_space = data->free_space,
        .get_tick = data->get_tick,
        .reset = data->reset,
        .verbosity = data->verbosity,
        .selected = MAINBOARD_LOGGER_REPORT_VOLTAGES,
        .printing = MAINBOARD_LOGGER_REPORT_COUNT,
    };

    const enum LoggerReturnCode return_code = logger_api_init(NULL, prv_mainboard_logger_write, NULL, data->verbosity);
    if (return_code == LOGGER_RC_OK) {
        prv_mainboard_logger_print_banner();
    }
    return return_code;
}

void mainboard_logger_api_set_verbosity(const uint8_t verbosity) {
    mainboard_logger_handler.verbosity = verbosity;
    mainboard_logger_handler.quiet = false;
    logger_api_set_verbosity(NULL, verbosity);
}

void mainboard_logger_api_handle_command(const char key) {
    // Line endings and escape sequences (e.g. arrow keys) are not commands
    if (!isprint((unsigned char)key)) {
        return;
    }
    char command = (char)tolower((unsigned char)key);
    if (command == '?') {
        command = int_mainboard_logger_reports_get(MAINBOARD_LOGGER_REPORT_HELP)->key;
    }
    // The reset is confirmed only by pressing its key twice in a row
    if (command != MAINBOARD_LOGGER_RESET_KEY && mainboard_logger_handler.reset_armed) {
        mainboard_logger_handler.reset_armed = false;
        MAINBOARD_LOGGER_INFO("Console: reset cancelled");
    }

    for (enum MainboardLoggerReport report = 0U; report < MAINBOARD_LOGGER_REPORT_COUNT; ++report) {
        if (int_mainboard_logger_reports_get(report)->key == command) {
            // The help is not a data report, the space key prints the last data report
            if (report != MAINBOARD_LOGGER_REPORT_HELP) {
                mainboard_logger_handler.selected = report;
            }
            prv_mainboard_logger_start_report(report);
            return;
        }
    }

    for (size_t i = 0U; i < MAINBOARD_LOGGER_COUNT_OF(mainboard_logger_actions); ++i) {
        if (mainboard_logger_actions[i].key == command) {
            mainboard_logger_actions[i].run();
            return;
        }
    }

    MAINBOARD_LOGGER_WARNING("Console: unknown command '%c', press h for help", key);
}

void mainboard_logger_api_routine(void) {
    struct MainboardLoggerHandler *const handler = &mainboard_logger_handler;
    if (handler->write == NULL) {
        return;
    }

    if (handler->reset_pending) {
        if (handler->get_tick() - handler->reset_requested_at >= MAINBOARD_LOGGER_RESET_DELAY_MS) {
            handler->reset();
        }
        return;
    }

    char input[MAINBOARD_LOGGER_INPUT_SIZE];
    const size_t count = handler->read(input, sizeof(input));
    for (size_t i = 0U; i < count; ++i) {
        mainboard_logger_api_handle_command(input[i]);
    }

    prv_mainboard_logger_print_lines();
}
