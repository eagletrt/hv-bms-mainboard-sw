/**
 * @file bal.c
 * @date 2024-08-02
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Balancing handling functions
 */

#include "bal.h"

#include <string.h>

#include "timebase.h"
#include "volt.h"

#ifdef CONF_BALANCING_MODULE_ENABLE

_STATIC _BalHandler hbal;

void _bal_timeout(void) {
    // Stop balancing
    hbal.event.type = FSM_EVENT_TYPE_BALANCING_STOP;
    fsm_event_trigger(&hbal.event);
}

BalReturnCode bal_init(void) {
    memset(&hbal, 0U, sizeof(hbal));

    // Set default event
    hbal.event.type = FSM_EVENT_TYPE_IGNORED;

    for (CellboardId id = 0U; id < CELLBOARD_ID_COUNT; ++id) {
        hbal.can_payload[id].start = false;
        hbal.can_payload[id].target = BAL_TARGET_MAX;
        hbal.can_payload[id].threshold = BAL_THRESHOLD_MAX;
    }

    // Set default balancing parameters
    hbal.params.target = BAL_TARGET_MAX;
    hbal.params.threshold = BAL_THRESHOLD_MAX;

    // Initialize main balancing watchdog
    (void)watchdog_init(
        &hbal.watchdog,
        TIMEBASE_TIME_TO_TICKS(BAL_TIMEOUT, timebase_get_resolution()),
        _bal_timeout
    );
    return BAL_OK;
}

bool bal_is_active(void) {
    return hbal.active;
}

BalReturnCode bal_start(void) {
    // Check actual balancing state
    if (hbal.active)
        return BAL_OK;

    // Start watchdog
    WatchdogReturnCode code = watchdog_restart(&hbal.watchdog);
    if (code == WATCHDOG_UNAVAILABLE)
        return BAL_WATCHDOG_ERROR;

    // Start balancing task
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_0_SET_BALANCING_STATUS, true);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_1_SET_BALANCING_STATUS, true);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_2_SET_BALANCING_STATUS, true);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_3_SET_BALANCING_STATUS, true);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_4_SET_BALANCING_STATUS, true);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_5_SET_BALANCING_STATUS, true);
    hbal.active = true;
    return BAL_OK;
}

BalReturnCode bal_stop(void) {
    // Check actual balancing status
    if (!hbal.active)
        return BAL_OK;
    // Stop watchdog
    (void)watchdog_stop(&hbal.watchdog);

    // Stop balancing tasks
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_0_SET_BALANCING_STATUS, false);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_1_SET_BALANCING_STATUS, false);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_2_SET_BALANCING_STATUS, false);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_3_SET_BALANCING_STATUS, false);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_4_SET_BALANCING_STATUS, false);
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_5_SET_BALANCING_STATUS, false);
    hbal.active = false;
    return BAL_OK;
}

void bal_set_balancing_state_from_steering_wheel_handle(primary_hv_set_balancing_status_steering_wheel_converted_t * payload) {
    if (payload == NULL)
        return;
    // Ignore stop command if not balancing
    if (!hbal.active && !payload->status)
        return;

    // Update data
    raw_volt_t target = volt_get_min();
    raw_volt_t thr = VOLT_MILLIVOLT_TO_VALUE(payload->threshold);
    hbal.params.target = MAINBOARD_CLAMP(target, BAL_TARGET_MIN, BAL_TARGET_MAX);
    hbal.params.threshold = MAINBOARD_CLAMP(thr, BAL_THRESHOLD_MIN, BAL_THRESHOLD_MAX);

    // Reset watchdog for each new message
    WatchdogReturnCode code = watchdog_reset(&hbal.watchdog);
    if (code == WATCHDOG_UNAVAILABLE)
        return;

    // Send event to the FSM
    if (hbal.active != payload->status) {
        hbal.event.type = payload->status ?
            FSM_EVENT_TYPE_BALANCING_START :
            FSM_EVENT_TYPE_BALANCING_STOP;
        fsm_event_trigger(&hbal.event);
    }
}

void bal_set_balancing_state_from_handcart_handle(primary_hv_set_balancing_status_handcart_converted_t * payload) {
    if (payload == NULL)
        return;
    // Ignore stop command if not balancing
    if (!hbal.active && !payload->status)
        return;

    // Update data
    raw_volt_t target = volt_get_min();
    raw_volt_t thr = VOLT_MILLIVOLT_TO_VALUE(payload->threshold);
    hbal.params.target = MAINBOARD_CLAMP(target, BAL_TARGET_MIN, BAL_TARGET_MAX);
    hbal.params.threshold = MAINBOARD_CLAMP(thr, BAL_THRESHOLD_MIN, BAL_THRESHOLD_MAX);

    // Reset watchdog for each new message
    WatchdogReturnCode code = watchdog_reset(&hbal.watchdog);
    if (code == WATCHDOG_UNAVAILABLE)
        return;

    // Send event to the FSM
    if (hbal.active != payload->status) {
        hbal.event.type = payload->status ?
            FSM_EVENT_TYPE_BALANCING_START :
            FSM_EVENT_TYPE_BALANCING_STOP;
        fsm_event_trigger(&hbal.event);
    }
}

bms_cellboard_set_balancing_status_converted_t * bal_get_canlib_payload(CellboardId id, size_t * byte_size) {
    if (id >= CELLBOARD_ID_COUNT)
        return NULL;
    if (byte_size != NULL)
        *byte_size = sizeof(hbal.can_payload[0U]);
    return &hbal.can_payload[id];
}

#endif // CONF_BALANCING_MODULE_ENABLE
