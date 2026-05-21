/*!
 * \file bal.c
 * \date 2024-08-02
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Balancing handling functions
 */

#include "bal-api.h"

#include "eagletrt-api.h"
#include <string.h>

#include "timebase.h"
#include "volt.h"

#ifdef CONF_BALANCING_MODULE_ENABLE

EAGLETRT_STATIC struct BalHandler balancing_handler;

/*! \brief Timeout handler for the balancing module */
void prv_bal_timeout(void) {
    // Stop balancing
    balancing_handler.event.type = FSM_EVENT_TYPE_BALANCING_STOP;
    fsm_event_trigger(&balancing_handler.event);
}

enum BalReturnCode bal_api_init(void) {
    memset(&balancing_handler, 0U, sizeof(balancing_handler));

    // Set default event
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    // Set default calib payload data
    balancing_handler.set_status_can_payload.start = false;
    balancing_handler.set_status_can_payload.target = BAL_TARGET_MAX_V;
    balancing_handler.set_status_can_payload.threshold = BAL_THRESHOLD_MAX_V;

    // Set default balancing parameters
    balancing_handler.params.target = BAL_TARGET_MAX_V;
    balancing_handler.params.threshold = BAL_THRESHOLD_MAX_V;

    // Initialize main balancing watchdog
    (void)watchdog_init(
        &balancing_handler.watchdog,
        TIMEBASE_TIME_TO_TICKS(BAL_TIMEOUT_MS, timebase_get_resolution()),
        prv_bal_timeout);
    return BAL_RC_OK;
}

bool bal_api_is_active(void) {
    return balancing_handler.active;
}

enum BalReturnCode bal_api_start(void) {
    // Check actual balancing state
    if (balancing_handler.active) {
        return BAL_RC_OK;
    }

    // Start watchdog
    const WatchdogReturnCode code = watchdog_restart(&balancing_handler.watchdog);
    if (code == WATCHDOG_UNAVAILABLE) {
        return BAL_RC_WATCHDOG_ERROR;
    }

    // Start balancing task
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_SET_BALANCING_STATUS, true);
    balancing_handler.active = true;
    return BAL_RC_OK;
}

enum BalReturnCode bal_api_stop(void) {
    // Check actual balancing status
    if (!balancing_handler.active) {
        return BAL_RC_OK;
    }
    // Stop watchdog
    (void)watchdog_stop(&balancing_handler.watchdog);

    balancing_handler.active = false;

    // Stop balancing tasks
    (void)tasks_set_enable(TASKS_ID_SEND_CELLBOARD_SET_BALANCING_STATUS, false);

    return BAL_RC_OK;
}

void bal_api_set_balancing_state_from_steering_wheel_handle(primary_hv_set_balancing_status_steering_wheel_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    // Ignore stop command if not balancing
    if (!balancing_handler.active && !payload->status) {
        return;
    }

    // Update data
    const volt_t target = volt_get_min();
    const volt_t thr = payload->threshold;

    constexpr volt_t bal_target_min = BAL_TARGET_MIN_V;
    constexpr volt_t bal_target_max = BAL_TARGET_MAX_V;

    constexpr volt_t bal_threshold_min = BAL_THRESHOLD_MIN_V;
    constexpr volt_t bal_threshold_max = BAL_THRESHOLD_MAX_V;

    balancing_handler.params.target = EAGLETRT_API_CLAMP(target, bal_target_min, bal_target_max);
    balancing_handler.params.threshold = EAGLETRT_API_CLAMP(thr, bal_threshold_min, bal_threshold_max);

    // Reset watchdog for each new message
    const WatchdogReturnCode code = watchdog_reset(&balancing_handler.watchdog);
    if (code == WATCHDOG_UNAVAILABLE) {
        return;
    }

    // Send event to the FSM
    if (balancing_handler.active != (bool)(payload->status == 1)) {
        balancing_handler.event.type = payload->status ? FSM_EVENT_TYPE_BALANCING_START : FSM_EVENT_TYPE_BALANCING_STOP;
        fsm_event_trigger(&balancing_handler.event);
    }
}

void bal_api_set_balancing_state_from_handcart_handle(primary_hv_set_balancing_status_handcart_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    // Ignore stop command if not balancing
    if (!balancing_handler.active && !payload->status) {
        return;
    }

    // Update data
    const volt_t target = volt_get_min();
    const volt_t thr = payload->threshold;

    constexpr volt_t bal_target_min = BAL_TARGET_MIN_V;
    constexpr volt_t bal_target_max = BAL_TARGET_MAX_V;
    constexpr volt_t bal_threshold_min = BAL_THRESHOLD_MIN_V;
    constexpr volt_t bal_threshold_max = BAL_THRESHOLD_MAX_V;

    balancing_handler.params.target = EAGLETRT_API_CLAMP(target, bal_target_min, bal_target_max);
    balancing_handler.params.threshold = EAGLETRT_API_CLAMP(thr, bal_threshold_min, bal_threshold_max);

    // Reset watchdog for each new message
    const WatchdogReturnCode code = watchdog_reset(&balancing_handler.watchdog);
    if (code == WATCHDOG_UNAVAILABLE) {
        return;
    }
    // Send event to the FSM
    if (balancing_handler.active != (bool)(payload->status == 1)) {
        balancing_handler.event.type = payload->status ? FSM_EVENT_TYPE_BALANCING_START : FSM_EVENT_TYPE_BALANCING_STOP;
        fsm_event_trigger(&balancing_handler.event);
    }
}

void bal_api_cellboard_balancing_status_handle(bms_cellboard_balancing_status_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    // Forward balancing status info to the primary network
    balancing_handler.status_can_payload.status = (primary_hv_balancing_status_status)payload->status;
    balancing_handler.status_can_payload.cellboard_id = (primary_hv_balancing_status_cellboard_id)payload->cellboard_id;
    balancing_handler.status_can_payload.discharging_cell_0 = payload->discharging_cell_0;
    balancing_handler.status_can_payload.discharging_cell_1 = payload->discharging_cell_1;
    balancing_handler.status_can_payload.discharging_cell_2 = payload->discharging_cell_2;
    balancing_handler.status_can_payload.discharging_cell_3 = payload->discharging_cell_3;
    balancing_handler.status_can_payload.discharging_cell_4 = payload->discharging_cell_4;
    balancing_handler.status_can_payload.discharging_cell_5 = payload->discharging_cell_5;
    balancing_handler.status_can_payload.discharging_cell_6 = payload->discharging_cell_6;
    balancing_handler.status_can_payload.discharging_cell_7 = payload->discharging_cell_7;
    balancing_handler.status_can_payload.discharging_cell_8 = payload->discharging_cell_8;
    balancing_handler.status_can_payload.discharging_cell_9 = payload->discharging_cell_9;
    balancing_handler.status_can_payload.discharging_cell_10 = payload->discharging_cell_10;
    balancing_handler.status_can_payload.discharging_cell_11 = payload->discharging_cell_11;
    balancing_handler.status_can_payload.discharging_cell_12 = payload->discharging_cell_12;
    balancing_handler.status_can_payload.discharging_cell_13 = payload->discharging_cell_13;
    balancing_handler.status_can_payload.discharging_cell_14 = payload->discharging_cell_14;
    balancing_handler.status_can_payload.discharging_cell_15 = payload->discharging_cell_15;
    balancing_handler.status_can_payload.discharging_cell_16 = payload->discharging_cell_16;
    balancing_handler.status_can_payload.discharging_cell_17 = payload->discharging_cell_17;
    balancing_handler.status_can_payload.discharging_cell_18 = payload->discharging_cell_18;
    balancing_handler.status_can_payload.discharging_cell_19 = payload->discharging_cell_19;
    balancing_handler.status_can_payload.discharging_cell_20 = payload->discharging_cell_20;
    balancing_handler.status_can_payload.discharging_cell_21 = payload->discharging_cell_21;
    balancing_handler.status_can_payload.discharging_cell_22 = payload->discharging_cell_22;
    balancing_handler.status_can_payload.discharging_cell_23 = payload->discharging_cell_23;
}

bms_cellboard_set_balancing_status_converted_t *bal_api_get_set_status_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(balancing_handler.set_status_can_payload);
    }
    balancing_handler.set_status_can_payload.start = balancing_handler.active;
    balancing_handler.set_status_can_payload.target = balancing_handler.params.target;
    balancing_handler.set_status_can_payload.threshold = balancing_handler.params.threshold;
    return &balancing_handler.set_status_can_payload;
}

primary_hv_balancing_status_converted_t *bal_api_get_status_canlib_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(balancing_handler.status_can_payload);
    }
    return &balancing_handler.status_can_payload;
}

#endif // CONF_BALANCING_MODULE_ENABLE
