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
#define TASKS_X_LIST                                                                                                                                         \
    TASKS_X(SEND_MAINBOARD_VERSION, true, 0U, PRIMARY_HV_MAINBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_mainboard_version)                                  \
    TASKS_X(SEND_CELLBOARD_0_VERSION, true, 0U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_0_version)                              \
    TASKS_X(SEND_CELLBOARD_1_VERSION, true, 1U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_1_version)                              \
    TASKS_X(SEND_CELLBOARD_2_VERSION, true, 2U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_2_version)                              \
    TASKS_X(SEND_CELLBOARD_3_VERSION, true, 3U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_3_version)                              \
    TASKS_X(SEND_CELLBOARD_4_VERSION, true, 4U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_4_version)                              \
    TASKS_X(SEND_CELLBOARD_5_VERSION, true, 5U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, prv_tasks_send_cellboard_5_version)                              \
    TASKS_X(SEND_STATUS, true, 0U, PRIMARY_HV_STATUS_CYCLE_TIME_MS, prv_tasks_send_hv_status)                                                                \
    TASKS_X(SEND_BALANCING_STATUS, true, 0U, PRIMARY_HV_BALANCING_STATUS_CYCLE_TIME_MS, prv_tasks_send_hv_balancing_status)                                  \
    TASKS_X(SEND_CURRENT, true, 10U, PRIMARY_HV_CURRENT_CYCLE_TIME_MS, prv_tasks_send_hv_current)                                                            \
    TASKS_X(SEND_POWER, true, 10U, PRIMARY_HV_POWER_CYCLE_TIME_MS, prv_tasks_send_hv_power)                                                                  \
    TASKS_X(SEND_TS_VOLTAGE, true, 0U, PRIMARY_HV_TS_VOLTAGE_CYCLE_TIME_MS, prv_tasks_send_hv_ts_voltage)                                                    \
    TASKS_X(SEND_CELLS_VOLTAGE, true, 10U, PRIMARY_HV_CELLS_VOLTAGE_CYCLE_TIME_MS, prv_tasks_send_hv_cells_voltage)                                          \
    TASKS_X(START_CELLS_VOLTAGE_STATS, true, 10U, PRIMARY_HV_CELLS_VOLTAGE_STATS_CYCLE_TIME_MS, prv_tasks_send_hv_cells_voltage_stats)                       \
    TASKS_X(SEND_CELLS_TEMPERATURE, true, 10U, PRIMARY_HV_CELLS_TEMPERATURE_CYCLE_TIME_MS, prv_tasks_send_hv_cells_temperature)                              \
    TASKS_X(START_CELLS_TEMPERATURE_STATS, true, 10U, PRIMARY_HV_CELLS_TEMP_STATS_CYCLE_TIME_MS, prv_tasks_send_hv_cells_temperature_stats)                  \
    TASKS_X(SEND_COOLING_TEMPERATURE, true, 10U, 50U, prv_tasks_send_hv_cooling_temperature)                                                                 \
    TASKS_X(SEND_FEEDBACK_STATUS, true, 10U, PRIMARY_HV_FEEDBACK_STATUS_CYCLE_TIME_MS, prv_tasks_send_hv_feedback_status)                                    \
    TASKS_X(SEND_FEEDBACK_DIGITAL, true, 10U, PRIMARY_HV_FEEDBACK_DIGITAL_CYCLE_TIME_MS, prv_tasks_send_hv_feedback_digital)                                 \
    TASKS_X(SEND_FEEDBACK_ANALOG, true, 10U, PRIMARY_HV_FEEDBACK_ANALOG_CYCLE_TIME_MS, prv_tasks_send_hv_feedback_analog)                                    \
    TASKS_X(SEND_FEEDBACK_ANALOG_SD, true, 10U, PRIMARY_HV_FEEDBACK_ANALOG_SD_CYCLE_TIME_MS, prv_tasks_send_hv_feedback_analog_sd)                           \
    TASKS_X(SEND_IMD_STATUS, true, 0U, PRIMARY_HV_IMD_STATUS_CYCLE_TIME_MS, prv_tasks_send_hv_imd_status)                                                    \
    TASKS_X(SEND_CELLBOARD_SET_BALANCING_STATUS, false, 0U, BMS_CELLBOARD_SET_BALANCING_STATUS_CYCLE_TIME_MS, prv_tasks_send_cellboard_set_balancing_status) \
    TASKS_X(SEND_ERRORS, false, 0U, PRIMARY_HV_ERROR_CYCLE_TIME_MS, prv_tasks_send_errors)                                                                   \
    TASKS_X(READ_DIGITAL_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_read_digital_feedbacks)                                                      \
    TASKS_X(START_ANALOG_CONVERSION_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_start_analog_conversion_feedbacks)                                \
    TASKS_X(UPDATE_FEEDBACKS_STATUS, true, 2U, FEEDBACK_CYCLE_TIME_MS, prv_tasks_update_feedbacks_status)                                                    \
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
