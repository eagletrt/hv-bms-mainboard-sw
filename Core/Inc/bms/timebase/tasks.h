/**
 * @brief tasks.h
 * @date 2024-05-15
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Implementations of the tasks that have to be executed by the timebase
 *
 * @attention This file should only be included inside the timebase source
 * The code is separated only for clarity and to avoid having all the code inside a single
 * file
 *
 * @details To use the functions inside this file define the TASKS_IMPLEMENTATION macro
 */
#ifndef TASKS_H
#define TASKS_H

#include <stddef.h>
#include <stdbool.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/**@brief Total number of tasks */
#define TASKS_COUNT (TASKS_ID_COUNT)

/**
 * @brief List of tasks parameters
 *
 * @attention !!! DO NOT USE THIS MACRO OUTSIDE OF THIS FILE !!!
 *
 * @attention This file uses X macros (https://en.wikipedia.org/wiki/X_macro)
 * to make it easier to add more tasks without to much changes to the code
 *
 * @details To add a new task add a field to this list and give it the right parameters
 * then go to the source file and implement the callback function
 *
 * @param name The name associated with the task (have to be unique)
 * @param enabled True if the tasks should be enabled by default or not
 * @param start The first moment when the task is executed (in ticks)
 * @param interval How often the task should run
 * @param exec A pointer to the task function callback
 */
#define TASKS_X_LIST \
    TASKS_X(SEND_MAINBOARD_VERSION, true, 0U, PRIMARY_HV_MAINBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_mainboard_version) \
    TASKS_X(SEND_CELLBOARD_0_VERSION, true, 0U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_0_version) \
    TASKS_X(SEND_CELLBOARD_1_VERSION, true, 1U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_1_version) \
    TASKS_X(SEND_CELLBOARD_2_VERSION, true, 2U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_2_version) \
    TASKS_X(SEND_CELLBOARD_3_VERSION, true, 3U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_3_version) \
    TASKS_X(SEND_CELLBOARD_4_VERSION, true, 4U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_4_version) \
    TASKS_X(SEND_CELLBOARD_5_VERSION, true, 5U, PRIMARY_HV_CELLBOARD_VERSION_CYCLE_TIME_MS, _tasks_send_cellboard_5_version) \
    TASKS_X(SEND_STATUS, true, 0U, PRIMARY_HV_STATUS_CYCLE_TIME_MS, _tasks_send_hv_status) \
    TASKS_X(SEND_BALANCING_STATUS, true, 0U, PRIMARY_HV_BALANCING_STATUS_CYCLE_TIME_MS, _tasks_send_hv_balancing_status) \
    TASKS_X(SEND_CURRENT, true, 10U, PRIMARY_HV_CURRENT_CYCLE_TIME_MS, _tasks_send_hv_current) \
    TASKS_X(SEND_POWER, true, 10U, PRIMARY_HV_POWER_CYCLE_TIME_MS, _tasks_send_hv_power) \
    TASKS_X(SEND_TS_VOLTAGE, true, 0U, PRIMARY_HV_TS_VOLTAGE_CYCLE_TIME_MS, _tasks_send_hv_ts_voltage) \
    TASKS_X(SEND_CELLS_VOLTAGE, true, 10U, PRIMARY_HV_CELLS_VOLTAGE_CYCLE_TIME_MS, _tasks_send_hv_cells_voltage) \
    TASKS_X(SEND_FEEDBACK_STATUS, true, 10U, PRIMARY_HV_FEEDBACK_STATUS_CYCLE_TIME_MS, _tasks_send_hv_feedback_status) \
    TASKS_X(SEND_FEEDBACK_DIGITAL, true, 10U, PRIMARY_HV_FEEDBACK_DIGITAL_CYCLE_TIME_MS, _tasks_send_hv_feedback_digital) \
    TASKS_X(SEND_FEEDBACK_ANALOG, true, 10U, PRIMARY_HV_FEEDBACK_ANALOG_CYCLE_TIME_MS, _tasks_send_hv_feedback_analog) \
    TASKS_X(SEND_FEEDBACK_ANALOG_SD, true, 10U, PRIMARY_HV_FEEDBACK_ANALOG_SD_CYCLE_TIME_MS, _tasks_send_hv_feedback_analog_sd) \
    TASKS_X(SEND_IMD_STATUS, true, 0U, PRIMARY_HV_IMD_STATUS_CYCLE_TIME_MS, _tasks_send_hv_imd_status) \
    TASKS_X(SEND_CELLBOARD_SET_BALANCING_STATUS, false, 0U, BMS_CELLBOARD_SET_BALANCING_STATUS_CYCLE_TIME_MS, _tasks_send_cellboard_set_balancing_status) \
    TASKS_X(READ_DIGITAL_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, _tasks_read_digital_feedbacks) \
    TASKS_X(START_ANALOG_CONVERSION_FEEDBACKS, true, 0U, FEEDBACK_CYCLE_TIME_MS, _tasks_start_analog_conversion_feedbacks) \
    TASKS_X(UPDATE_FEEDBACKS_STATUS, true, 2U, FEEDBACK_CYCLE_TIME_MS, _tasks_update_feedbacks_status) \
    TASKS_X(START_INTERNAL_VOLTAGE_CONVERSION, true, 0U, INTERNAL_VOLTAGE_CYCLE_TIME_MS, _tasks_start_internal_voltage_conversion)

/** @brief Convert a task name to the corresponding TasksId name */
#define TASKS_NAME_TO_ID(NAME) (TASKS_ID_##NAME)

/**
 * @brief Enumeration of tasks
 *
 * @details This enum is mainly used to get the total number of tasks at compile time
 * but can also be used to get a specific tasks given a name in the format TASKS_ID_[NAME]
 */
#define TASKS_X(NAME, ENABLED, START, INTERVAL, EXEC) TASKS_ID_##NAME,
typedef enum {
    TASKS_X_LIST
    TASKS_ID_COUNT
} TasksId;
#undef TASKS_X

/** @brief Type definition for a function that excecutes a single task */
typedef void (* tasks_callback)(void);

/**
 * @brief Definition of a single task
 *
 * @details An interval of 0 means that the task is only run once
 *
 * @param enabled The tasks enabled flag
 * @param id The task identifier
 * @param start The time when the tasks is executed first
 * @param interval The amount of time that must elapsed before the tasks is re-executed
 * @param exec A pointer to the task callback
 */
typedef struct {
    bool enabled;
    TasksId id;
    ticks_t start;
    ticks_t interval;
    tasks_callback exec;
} Task;

/**
 * @brief Return code for the tasks module functions
 *
 * @details
 *     - TASKS_INVALID_ID the given identifer does not exists
 *     - TASKS_OK the function executed succesfully
 */
typedef enum {
    TASKS_INVALID_ID,
    TASKS_OK
} TasksReturnCode;

/**
 * @brief Tasks hanlder struct
 *
 * @warning This structure should never be used outside of this file
 *
 * @param tasks The array of tasks
 */
typedef struct {
    Task tasks[TASKS_COUNT];
} _TaskHandler;

#ifdef CONF_TASKS_MODULE_ENABLE

/**
 * @brief Initialize the tasks module
 *
 * @param resolution The timebase resolution
 *
 * @return TasksReturnCode
 *     - TASKS_OK
 */
TasksReturnCode tasks_init(milliseconds_t resolution);

/**
 * @brief Enable or disable a single task
 *
 * @param id The task identifier
 * @param enabled True to enable the tasks, false to disable
 *
 * @return TasksReturnCode
 *     - TASKS_INVALID_ID the given identifier does not exists
 *     - TASKS_OK otherwise
 */
TasksReturnCode tasks_set_enable(TasksId id, bool enabled);

/**
 * @brief Check if a task is enabled or not
 *
 * @param id The task identifier
 *
 * @return True if the task is enabled, false otherwise
 */
bool tasks_is_enabled(TasksId id);

/**
 * @brief Get a pointer to the tasks
 *
 * @param id The identifier of the task
 *
 * @return Task* The pointer to the tasks or NULL if the id is not valid
 */
Task * tasks_get_task(TasksId id);

/**
 * @brief Get the start time of the task
 *
 * @param id The identifier of the task
 *
 * @return ticks_t The task start time or 0 if the id is not valid
 */
ticks_t tasks_get_start(TasksId id);

/**
 * @brief Get the interval time of the task
 *
 * @param id The identifier of the task
 *
 * @return ticks_t The task interval time or 0 if the id is not valid
 */
ticks_t tasks_get_interval(TasksId id);

/**
 * @brief Get a pointer to the task callback
 *
 * @param id The identifier of the task
 *
 * @return tasks_callback The task callback or NULL if the id is not valid
 */
tasks_callback tasks_get_callback(TasksId id);

#else  // CONF_TASKS_MODULE_ENABLE

#define tasks_init(resolution) (TASKS_OK)
#define tasks_set_enable(id, enabled) (TASKS_OK)
#define tasks_is_enabled(id) (false)
#define tasks_get_task(id) (NULL)
#define tasks_get_start(id) (0U)
#define tasks_get_interval(id) (0U)
#define tasks_get_callback(id) (NULL)

#endif // CONF_TASKS_MODULE_ENABLE

#endif // TASKS_H
