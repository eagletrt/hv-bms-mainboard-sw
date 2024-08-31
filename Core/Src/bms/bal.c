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

    // Set default calib payload data
    hbal.set_status_can_payload.start = false;
    hbal.set_status_can_payload.target = BAL_TARGET_MAX_V;
    hbal.set_status_can_payload.threshold = BAL_THRESHOLD_MAX_V;

    // Set default balancing parameters
    hbal.params.target = BAL_TARGET_MAX_V;
    hbal.params.threshold = BAL_THRESHOLD_MAX_V;

    // Initialize main balancing watchdog
    (void)watchdog_init(
        &hbal.watchdog,
        TIMEBASE_TIME_TO_TICKS(BAL_TIMEOUT_MS, timebase_get_resolution()),
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
    const WatchdogReturnCode code = watchdog_restart(&hbal.watchdog);
    if (code == WATCHDOG_UNAVAILABLE)
        return BAL_WATCHDOG_ERROR;

    // Start balancing task
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_SET_BALANCING_STATUS, true);
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
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_SET_BALANCING_STATUS, false);
    hbal.active = false;
    return BAL_OK;
}

void bal_set_balancing_state_from_steering_wheel_handle(primary_hv_set_balancing_status_steering_wheel_converted_t * const payload) {
    if (payload == NULL)
        return;
    // Ignore stop command if not balancing
    if (!hbal.active && !payload->status)
        return;

    // Update data
    const volt_t target = volt_get_min();
    const volt_t thr = payload->threshold;
    hbal.params.target = MAINBOARD_CLAMP(target, BAL_TARGET_MIN_V, BAL_TARGET_MAX_V);
    hbal.params.threshold = MAINBOARD_CLAMP(thr, BAL_THRESHOLD_MIN_V, BAL_THRESHOLD_MAX_V);

    // Reset watchdog for each new message
    const WatchdogReturnCode code = watchdog_reset(&hbal.watchdog);
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

void bal_set_balancing_state_from_handcart_handle(primary_hv_set_balancing_status_handcart_converted_t * const payload) {
    if (payload == NULL)
        return;
    // Ignore stop command if not balancing
    if (!hbal.active && !payload->status)
        return;

    // Update data
    const volt_t target = volt_get_min();
    const volt_t thr = payload->threshold;
    hbal.params.target = MAINBOARD_CLAMP(target, BAL_TARGET_MIN_V, BAL_TARGET_MAX_V);
    hbal.params.threshold = MAINBOARD_CLAMP(thr, BAL_THRESHOLD_MIN_V, BAL_THRESHOLD_MAX_V);

    // Reset watchdog for each new message
    const WatchdogReturnCode code = watchdog_reset(&hbal.watchdog);
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

void bal_cellboard_balancing_status_handle(bms_cellboard_balancing_status_converted_t * const payload) {
    if (payload == NULL)
        return;
    // Forward balancing status info to the primary network
    hbal.status_can_payload.status = (primary_hv_balancing_status_status)payload->status;
    hbal.status_can_payload.cellboard_id = (primary_hv_balancing_status_cellboard_id)payload->cellboard_id;
    hbal.status_can_payload.discharging_cell_0 = payload->discharging_cell_0;
    hbal.status_can_payload.discharging_cell_1 = payload->discharging_cell_1;
    hbal.status_can_payload.discharging_cell_2 = payload->discharging_cell_2;
    hbal.status_can_payload.discharging_cell_3 = payload->discharging_cell_3;
    hbal.status_can_payload.discharging_cell_4 = payload->discharging_cell_4;
    hbal.status_can_payload.discharging_cell_5 = payload->discharging_cell_5;
    hbal.status_can_payload.discharging_cell_6 = payload->discharging_cell_6;
    hbal.status_can_payload.discharging_cell_7 = payload->discharging_cell_7;
    hbal.status_can_payload.discharging_cell_8 = payload->discharging_cell_8;
    hbal.status_can_payload.discharging_cell_9 = payload->discharging_cell_9;
    hbal.status_can_payload.discharging_cell_10 = payload->discharging_cell_10;
    hbal.status_can_payload.discharging_cell_11 = payload->discharging_cell_11;
    hbal.status_can_payload.discharging_cell_12 = payload->discharging_cell_12;
    hbal.status_can_payload.discharging_cell_13 = payload->discharging_cell_13;
    hbal.status_can_payload.discharging_cell_14 = payload->discharging_cell_14;
    hbal.status_can_payload.discharging_cell_15 = payload->discharging_cell_15;
    hbal.status_can_payload.discharging_cell_16 = payload->discharging_cell_16;
    hbal.status_can_payload.discharging_cell_17 = payload->discharging_cell_17;
    hbal.status_can_payload.discharging_cell_18 = payload->discharging_cell_18;
    hbal.status_can_payload.discharging_cell_19 = payload->discharging_cell_19;
    hbal.status_can_payload.discharging_cell_20 = payload->discharging_cell_20;
    hbal.status_can_payload.discharging_cell_21 = payload->discharging_cell_21;
    hbal.status_can_payload.discharging_cell_22 = payload->discharging_cell_22;
    hbal.status_can_payload.discharging_cell_23 = payload->discharging_cell_23;
}

bms_cellboard_set_balancing_status_converted_t * bal_get_set_status_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hbal.set_status_can_payload);
    hbal.set_status_can_payload.start = hbal.active;
    hbal.set_status_can_payload.target = hbal.params.target;
    hbal.set_status_can_payload.threshold = hbal.params.threshold;
    return &hbal.set_status_can_payload;
}

primary_hv_balancing_status_converted_t * bal_get_status_canlib_payload(size_t * const byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hbal.status_can_payload);
    return &hbal.status_can_payload;
}

#endif // CONF_BALANCING_MODULE_ENABLE
