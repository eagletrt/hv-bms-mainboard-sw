/*!
 * \file tasks.h
 * \date 2024-05-15
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Implementations of the tasks that have to be executed by the timebase
 *
 * \attention This file should only be included inside the timebase source
 * The code is separated only for clarity and to avoid having all the code inside a single
 * file
 *
 * \details To use the functions inside this file define the TASKS_IMPLEMENTATION macro
 */
#ifndef TASKS_H
#define TASKS_H

#include <stddef.h>
#include <stdbool.h>

#include "can-primary.h"
#include "mainboard-conf.h"
#include "mainboard-def.h"

/*!
 * \brief Total number of tasks
 */
#define TASKS_COUNT (TASKS_ID_COUNT)

/*!
 * \brief List of tasks parameters
 *
 * \attention !!! DO NOT USE THIS MACRO OUTSIDE OF THIS FILE !!!
 *
 * \attention This file uses X macros (https://en.wikipedia.org/wiki/X_macro)
 * to make it easier to add more tasks without to much changes to the code
 *
 * \details To add a new task add a field to this list and give it the right parameters
 * then go to the source file and implement the callback function
 *
 * \param name The name associated with the task (have to be unique)
 * \param enabled True if the tasks should be enabled by default or not
 * \param start The first moment when the task is executed (in ticks)
 * \param interval How often the task should run
 * \param exec A pointer to the task function callback
 */

#define TASKS_X_LIST                                                                                                                                           \
    TASKS_X(SEND_STATUS, true, 0U, can_primary_cycle_time_tsacstatus, prv_tasks_send_hv_status)                                                                \
    TASKS_X(SEND_MAINBOARD_VERSION, true, 0U, can_primary_cycle_time_tsacmainboardversion, prv_tasks_send_mainboard_version)                                   \
    TASKS_X(SEND_MAINBOARD_VERSION_INFO, true, 0U, can_primary_cycle_time_tsacmainboardversioninfo, prv_tasks_send_mainboard_version_info)                     \
    TASKS_X(SEND_MAINBOARD_LIBCAN_VERSION, true, 0U, can_primary_cycle_time_tsacmainboardlibcanversion, prv_tasks_send_mainboard_libcan_version)               \
    TASKS_X(SEND_MAINBOARD_LIBCAN_VERSION_INFO, true, 0U, can_primary_cycle_time_tsacmainboardlibcanversioninfo, prv_tasks_send_mainboard_libcan_version_info) \
    TASKS_X(SEND_CURRENT, true, 10U, can_primary_cycle_time_tsacmainboardcurrentinfo, prv_tasks_send_hv_current)                                               \
    TASKS_X(SEND_VOLTAGE_INFO, true, 0U, can_primary_cycle_time_tsacmainboardvoltageinfo, prv_tasks_send_hv_ts_voltage)                                        \
    TASKS_X(SEND_CELLBOARD1_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard1voltage, prv_tasks_send_hv_cellboard1_voltage)                            \
    TASKS_X(SEND_CELLBOARD2_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard2voltage, prv_tasks_send_hv_cellboard2_voltage)                            \
    TASKS_X(SEND_CELLBOARD3_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard3voltage, prv_tasks_send_hv_cellboard3_voltage)                            \
    TASKS_X(SEND_CELLBOARD4_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard4voltage, prv_tasks_send_hv_cellboard4_voltage)                            \
    TASKS_X(SEND_CELLBOARD5_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard5voltage, prv_tasks_send_hv_cellboard5_voltage)                            \
    TASKS_X(SEND_CELLBOARD6_VOLTAGE, true, 10U, can_primary_cycle_time_tsaccellboard6voltage, prv_tasks_send_hv_cellboard6_voltage)                            \
    TASKS_X(SEND_TEMPERATURE_INFO, true, 0U, can_primary_cycle_time_tsacmainboardtemperatureinfo, prv_tasks_send_hv_temperature_info)                          \
    TASKS_X(SEND_CELLBOARD1_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard1temperature, prv_tasks_send_hv_cellboard1_temperature)                \
    TASKS_X(SEND_CELLBOARD2_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard2temperature, prv_tasks_send_hv_cellboard2_temperature)                \
    TASKS_X(SEND_CELLBOARD3_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard3temperature, prv_tasks_send_hv_cellboard3_temperature)                \
    TASKS_X(SEND_CELLBOARD4_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard4temperature, prv_tasks_send_hv_cellboard4_temperature)                \
    TASKS_X(SEND_CELLBOARD5_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard5temperature, prv_tasks_send_hv_cellboard5_temperature)                \
    TASKS_X(SEND_CELLBOARD6_TEMPERATURE, true, 10U, can_primary_cycle_time_tsaccellboard6temperature, prv_tasks_send_hv_cellboard6_temperature)                \
    TASKS_X(SEND_COOLING_TEMPERATURE1, true, 10U, can_primary_cycle_time_tsacmainboardcoolingtemperature1, prv_tasks_send_cooling_temperature1)                \
    TASKS_X(SEND_COOLING_TEMPERATURE2, true, 10U, can_primary_cycle_time_tsacmainboardcoolingtemperature2, prv_tasks_send_cooling_temperature2)                \
    TASKS_X(SEND_CELLBOARD1_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard1balancing, prv_tasks_send_hv_cellboard1_balancing)                      \
    TASKS_X(SEND_CELLBOARD2_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard2balancing, prv_tasks_send_hv_cellboard2_balancing)                      \
    TASKS_X(SEND_CELLBOARD3_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard3balancing, prv_tasks_send_hv_cellboard3_balancing)                      \
    TASKS_X(SEND_CELLBOARD4_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard4balancing, prv_tasks_send_hv_cellboard4_balancing)                      \
    TASKS_X(SEND_CELLBOARD5_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard5balancing, prv_tasks_send_hv_cellboard5_balancing)                      \
    TASKS_X(SEND_CELLBOARD6_BALANCING, true, 10U, can_primary_cycle_time_tsaccellboard6balancing, prv_tasks_send_hv_cellboard6_balancing)                      \
    TASKS_X(SEND_IMD_STATUS, true, 0U, can_primary_cycle_time_tsacmainboardimd, prv_tasks_send_hv_imd_status)                                                  \
    TASKS_X(SEND_FEEDBACK, true, 10U, can_primary_cycle_time_tsacmainboardfeedback, prv_tasks_send_hv_feedback)                                                \
    TASKS_X(SEND_SHUTDOWN, true, 10U, can_primary_cycle_time_tsacmainboardshutdown, prv_tasks_send_hv_shutdown)                                                \
    TASKS_X(SEND_FEEDBACK_SHUTDOWN, true, 10U, can_primary_cycle_time_tsacmainboardfeedbackshutdown, prv_tasks_send_hv_feedback_shutdown)                      \
    TASKS_X(SEND_ERRORS, true, 0U, can_primary_cycle_time_tsacmainboarderror, prv_tasks_send_errors)                                                           \
    TASKS_X(SEND_CELLBOARD_ERRORS_A, true, 0U, can_primary_cycle_time_tsaccellboarderrora, prv_tasks_send_cellboard_errors_a)                                  \
    TASKS_X(SEND_CELLBOARD_ERRORS_B, true, 0U, can_primary_cycle_time_tsaccellboarderrorb, prv_tasks_send_cellboard_errors_b)                                  \
    TASKS_X(SEND_CELLBOARD_ERRORS_C, true, 0U, can_primary_cycle_time_tsaccellboarderrorc, prv_tasks_send_cellboard_errors_c)                                  \
    TASKS_X(SEND_CELLBOARD_SET_BALANCING_STATUS, false, 0U, can_bms_cycle_time_tsacmainboardbalancingset, prv_tasks_send_cellboard_set_balancing_status)       \
    TASKS_X(READ_DIGITAL_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_read_digital_feedbacks)                                                        \
    TASKS_X(START_ANALOG_CONVERSION_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_start_analog_conversion_feedbacks)                                  \
    TASKS_X(UPDATE_FEEDBACKS_STATUS, true, 2U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_update_feedbacks_status)                                                      \
    TASKS_X(CHECK_TEMPERATURE, true, 10U, 200U, prv_tasks_check_temperature)                                                                                   \
    TASKS_X(START_INTERNAL_VOLTAGE_CONVERSION, true, 0U, INTERNAL_VOLTAGE_CYCLE_TIME_MS, prv_tasks_start_internal_voltage_conversion)

/*! \brief Convert a task name to the corresponding TasksId name */
#define TASKS_NAME_TO_ID(NAME) (TASKS_ID_##NAME)

/*!
 * \brief Enumeration of tasks
 *
 * \details This enum is mainly used to get the total number of tasks at compile time
 * but can also be used to get a specific tasks given a name in the format TASKS_ID_[NAME]
 */
#define TASKS_X(NAME, ENABLED, START, INTERVAL, EXEC) TASKS_ID_##NAME,
enum TasksId {
    TASKS_X_LIST
        TASKS_ID_COUNT
};
#undef TASKS_X

/*!
 * \brief Type definition for a function that excecutes a single task
 */
typedef void (*tasks_callback)(void);

/*!
 * \brief Definition of a single task
 *
 * \details An interval of 0 means that the task is only run once
 */
struct Task {
    bool enabled;        /*!< The tasks enabled flag */
    enum TasksId id;     /*!< The task identifier */
    ticks_t start;       /*!< The time when the tasks is executed first */
    ticks_t interval;    /*!< The amount of time that must elapsed before the tasks is re-executed */
    tasks_callback exec; /*!< A pointer to the task callback */
};

/*!
 * \brief Return code for the tasks module functions
 */
enum TasksReturnCode {
    TASKS_RC_OK,        /*!< The function executed succesfully */
    TASKS_RC_INVALID_ID /*!< The given ID is invalid */
};

/*!
 * \brief Tasks hanlder struct
 *
 * \warning This structure should never be used outside of this file
 */
struct TaskHandler {
    struct Task tasks[TASKS_COUNT]; /*!< The array of tasks */
};

#ifdef CONF_TASKS_MODULE_ENABLE

/*!
 * \brief Initialize the tasks module
 *
 * \param resolution The timebase resolution
 *
 * \retval TASKS_RC_OK Executed succesfully
 */
enum TasksReturnCode tasks_init(milliseconds_t resolution);

/*!
 * \brief Enable or disable a single task
 *
 * \param task_id The task identifier
 * \param enabled True to enable the tasks, false to disable
 *
 * \retval TASKS_RC_INVALID_ID The given identifier does not exists
 * \retval TASKS_RC_OK Otherwise
 */
enum TasksReturnCode tasks_set_enable(enum TasksId task_id, bool enabled);

/*!
 * \brief Check if a task is enabled or not
 *
 * \param task_id The task identifier
 *
 * \return True if the task is enabled, false otherwise
 */
bool tasks_is_enabled(enum TasksId task_id);

/*!
 * \brief Get a pointer to the tasks
 *
 * \param task_id The identifier of the task
 *
 * \return The pointer to the tasks or NULL if the task_id is not valid
 */
struct Task *tasks_get_task(enum TasksId task_id);

/*!
 * \brief Get the start time of the task
 *
 * \param task_id The identifier of the task
 *
 * \return The task start time or 0 if the task_id is not valid
 */
ticks_t tasks_get_start(enum TasksId task_id);

/*!
 * \brief Get the interval time of the task
 *
 * \param task_id The identifier of the task
 *
 * \return The task interval time or 0 if the task_id is not valid
 */
ticks_t tasks_get_interval(enum TasksId task_id);

/*!
 * \brief Get a pointer to the task callback
 *
 * \param task_id The identifier of the task
 *
 * \return The task callback or NULL if the task_id is not valid
 */
tasks_callback tasks_get_callback(enum TasksId task_id);

#else // CONF_TASKS_MODULE_ENABLE

#define tasks_init(resolution) (TASKS_RC_OK)
#define tasks_set_enable(task_id, enabled) (TASKS_RC_OK)
#define tasks_is_enabled(task_id) (false)
#define tasks_get_task(task_id) (NULL)
#define tasks_get_start(task_id) (0U)
#define tasks_get_interval(task_id) (0U)
#define tasks_get_callback(task_id) (NULL)

#endif // CONF_TASKS_MODULE_ENABLE

#endif // TASKS_H
