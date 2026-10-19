/*!
 * \file mainboard-tasks-api.h
 * \date 2024-05-16
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief API for the mainboard tasks
 */

#ifndef MAINBOARD_TASKS_API_H
#define MAINBOARD_TASKS_API_H

#include "mainboard-tasks.h"
#include "tasks.h"

#ifdef CONF_TASKS_MODULE_ENABLE

/*!
 * \brief Initialize the mainboard tasks
 * \param tick_callback The callback function to be called on each tick
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_init(task_tick_callback tick_callback);

/*!
 * \brief Run the mainboard tasks routine
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_routine(void);

/*!
 * \brief Enable a specific mainboard task
 * \param task_id The ID of the task to enable
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_enable_task(uint8_t task_id);

/*!
 * \brief Disable a specific mainboard task
 * \param task_id The ID of the task to disable
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_disable_task(uint8_t task_id);

/*!
 * \brief Enable all mainboard tasks
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_enable_all_tasks(void);

/*!
 * \brief Suppress the sending of primary messages
 * \param suppress True to suppress, false to enable
 * \return The return code of the operation
 */
enum MainboardTasksReturnCode mainboard_tasks_suppress_primary_messages(bool suppress);

#else

#define mainboard_tasks_init(tick_callback) (MAINBOARD_TASKS_RC_OK)
#define mainboard_tasks_routine() (MAINBOARD_TASKS_RC_OK)
#define mainboard_tasks_enable_task(task_id) (MAINBOARD_TASKS_RC_OK)
#define mainboard_tasks_disable_task(task_id) (MAINBOARD_TASKS_RC_OK)
#define mainboard_tasks_enable_all_tasks() (MAINBOARD_TASKS_RC_OK)
#define mainboard_tasks_suppress_primary_messages(suppress) (MAINBOARD_TASKS_RC_OK)

#endif // CONF_TASKS_MODULE_ENABLE
#endif // MAINBOARD_TASKS_API_H