/*! 
 * \file mainboard-timebase-api.c
 * \date 2024-04-16
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \brief Implementation of the mainboard timebase API.
 */

#include "mainboard-timebase-api.h"
#include "timebase-api.h"

#ifdef CONF_TIMEBASE_MODULE_ENABLE

static struct TimebaseHandler htimebase;

enum TimebaseReturnCode mainboard_timebase_api_init(const milliseconds resolution_ms) {
    return timebase_api_init(&htimebase, resolution_ms);
}

void mainboard_timebase_api_set_enable(const bool enabled) {
    (void)timebase_set_enable(&htimebase, enabled);
}

enum TimebaseReturnCode mainboard_timebase_api_inc_tick(void) {
    return timebase_inc_tick(&htimebase);
}

ticks mainboard_timebase_api_get_tick(void) {
    return timebase_get_tick(&htimebase);
}

milliseconds mainboard_timebase_api_get_time(void) {
    return timebase_get_time(&htimebase);
}

milliseconds mainboard_timebase_api_get_resolution(void) {
    return timebase_get_resolution(&htimebase);
}

#endif // CONF_TIMEBASE_MODULE_ENABLE
