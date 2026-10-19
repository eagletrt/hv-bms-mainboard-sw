/*!
 * \file mainboard-timebase-api.h
 * \date 2024-04-16
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions to manage global clock of the mainboard
 */
#ifndef MAINBOARD_TIMEBASE_API_H
#define MAINBOARD_TIMEBASE_API_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"
#include "timebase.h"

#ifdef CONF_TIMEBASE_MODULE_ENABLE

/*!
 * \brief Initialize the mainboard timebase
 *
 * \param resolution_ms The resolution of the timebase in ms
 * \return The result of the initialization
 */
enum TimebaseReturnCode mainboard_timebase_api_init(milliseconds resolution_ms);

/*!
 * \brief Set the enable state of the mainboard timebase
 *
 * \param enabled The enable state
 */
void mainboard_timebase_api_set_enable(bool enabled);

/*!
 * \brief Increment the mainboard timebase tick
 *
 * \return The result of the increment
 */
enum TimebaseReturnCode mainboard_timebase_api_inc_tick(void); // call from the TIM ISR

/*!
 * \brief Get the current tick
 *
 * \return The current tick
 */
ticks mainboard_timebase_api_get_tick(void);

/*!
 * \brief Get the current time in ms
 *
 * \return The current time in ms
 */
milliseconds mainboard_timebase_api_get_time(void);

/*!
 * \brief Get the resolution of the timebase in ms
 *
 * \return The resolution of the timebase in ms
 */
milliseconds mainboard_timebase_api_get_resolution(void);

#else

#define mainboard_timebase_api_init(resolution_ms) (TIMEBASE_RC_OK)
#define mainboard_timebase_api_set_enable(enabled) EAGLETRT_API_NOP()
#define mainboard_timebase_api_inc_tick() (TIMEBASE_RC_OK)
#define mainboard_timebase_api_get_tick() (0U)
#define mainboard_timebase_api_get_time() (0U)
#define mainboard_timebase_api_get_resolution() (1U) // avoid division by 0

#endif // CONF_TIMEBASE_MODULE_ENABLE
#endif // MAINBOARD_TIMEBASE_API_H
