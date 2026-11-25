/*!
 * \file mainboard-tasks-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tasks that have to be executed at a certain interval
 */

#ifndef MAINBOARD_TASKS_API_H
#define MAINBOARD_TASKS_API_H

#include "mainboard-tasks.h"
#include "tasks.h"

/*!
 * \brief Initialize the mainboard control tasks
 *
 * \param tick_callback The function that returns the current tick in milliseconds, it must be monotonic
 *
 * \returns The return code of the tasks module initialization, TASKS_RC_OK on success
 */
enum TasksReturnCode mainboard_tasks_api_init(task_tick_callback tick_callback);

/*!
 * \brief Run every control task that is due
 *
 * \returns The return code of the tasks module routine, TASKS_RC_OK on success
 */
enum TasksReturnCode mainboard_tasks_api_routine(void);

#endif // MAINBOARD_TASKS_API_H
