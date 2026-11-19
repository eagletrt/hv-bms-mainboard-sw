/*!
 * \file serializer-tasks-api.h
 * \date 2026-11-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN serializer tasks: periodically builds the messages with the serializer and queues them for transmission
 */

#ifndef SERIALIZER_TASKS_API_H
#define SERIALIZER_TASKS_API_H

#include <stdbool.h>

#include "serializer-tasks.h"
#include "tasks.h"

/*!
 * \brief Initialize the serializer tasks
 *
 * \details Every task of \ref serializer_tasks is scheduled with its period and enabled; the first
 * execution of each task is delayed by its ID in ticks, so the messages are spread instead of
 * being queued all at the same tick
 *
 * \param[in] get_tick The function that returns the current tick in milliseconds, it must be monotonic
 *
 * \retval SERIALIZER_TASKS_RC_NULL_POINTER if get_tick is NULL
 * \retval SERIALIZER_TASKS_RC_INVALID_PERIOD if a task period is 0 or does not fit the tasks module interval
 * \retval SERIALIZER_TASKS_RC_ERROR if the tasks module fails to initialize
 * \retval SERIALIZER_TASKS_RC_OK otherwise
 */
enum SerializerTasksReturnCode serializer_tasks_api_init(task_tick_callback get_tick);

/*!
 * \brief Run the tasks that are due
 *
 * \details Each due task builds its message, serializes it and adds it to the TX queue of its network;
 * call it in the main loop
 *
 * \retval SERIALIZER_TASKS_RC_ERROR if the tasks module fails (e.g. the tick went backwards)
 * \retval SERIALIZER_TASKS_RC_OK otherwise
 */
enum SerializerTasksReturnCode serializer_tasks_api_routine(void);

/*!
 * \brief Suppress or resume every task with the given priority
 *
 * \details A suppressed task is disabled, so when it is resumed it restarts its schedule from the
 * resume instant instead of sending all the messages it missed
 *
 * \param[in] priority The priority of the tasks to suppress or resume
 * \param[in] suppress true to stop the tasks, false to resume them
 *
 * \retval SERIALIZER_TASKS_RC_ERROR if the tasks module fails to update a task
 * \retval SERIALIZER_TASKS_RC_OK otherwise
 */
enum SerializerTasksReturnCode serializer_tasks_api_suppress_comms(enum SerializerTasksPriority priority, bool suppress);

#endif // SERIALIZER_TASKS_API_H
