/*!
 * \file bal.c
 * \date 2024-08-02
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Balancing handling functions
 */

#include "bal-api.h"

#include <string.h>

#include "eagletrt.h"
#include "eagletrt-api.h"
#include "can-bms.h"

#include "display.h"
#include "mainboard-def.h"
#include "timebase.h"
#include "volt-api.h"

#ifdef CONF_BALANCING_MODULE_ENABLE

EAGLETRT_STATIC struct BalHandler balancing_handler;

/*! \brief Timeout handler for the balancing module */
void prv_bal_api_timeout(void) {
    // Stop balancing
    balancing_handler.event.type = FSM_EVENT_TYPE_BALANCING_STOP;
    fsm_event_trigger(&balancing_handler.event);
}

enum BalReturnCode bal_api_init(void) {
    memset(&balancing_handler, 0U, sizeof(balancing_handler));

    // Set default event
    balancing_handler.event.type = FSM_EVENT_TYPE_IGNORED;

    // Set default calib payload data
    struct CanBmsTsacmainboardbalancingset *payload = &balancing_handler.libcan_message_balancing_set.tsacmainboardbalancingset;
    payload->start = false;
    payload->target = BAL_TARGET_MAX_V;
    payload->threshold = BAL_THRESHOLD_MAX_V;

    // Set default balancing parameters
    balancing_handler.params.target = BAL_TARGET_MAX_V;
    balancing_handler.params.threshold = BAL_THRESHOLD_MAX_V;

    // Initialize main balancing watchdog
    (void)watchdog_init(
        &balancing_handler.watchdog,
        TIMEBASE_TIME_TO_TICKS(BAL_TIMEOUT_MS, timebase_get_resolution()),
        prv_bal_api_timeout);
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
    const enum WatchdogReturnCode code = watchdog_restart(&balancing_handler.watchdog);
    if (code == WATCHDOG_RC_UNAVAILABLE) {
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

union CanBmsMessages *bal_api_get_balancing_set_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsacmainboardbalancingset;
    }

    struct CanBmsTsacmainboardbalancingset *payload = &balancing_handler.libcan_message_balancing_set.tsacmainboardbalancingset;
    payload->start = balancing_handler.active;
    payload->target = balancing_handler.params.target;
    payload->threshold = balancing_handler.params.threshold;
    return &balancing_handler.libcan_message_balancing_set;
}

union CanPrimaryMessages *bal_api_get_cellboard1_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard1balancing;
    }

    struct CanPrimaryTsaccellboard1balancing *payload = &balancing_handler.libcan_message_cellboard1_balancing.tsaccellboard1balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard1_balancing;
}

union CanPrimaryMessages *bal_api_get_cellboard2_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard2balancing;
    }

    struct CanPrimaryTsaccellboard2balancing *payload = &balancing_handler.libcan_message_cellboard2_balancing.tsaccellboard2balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard2_balancing;
}

union CanPrimaryMessages *bal_api_get_cellboard3_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard3balancing;
    }

    struct CanPrimaryTsaccellboard3balancing *payload = &balancing_handler.libcan_message_cellboard3_balancing.tsaccellboard3balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard3_balancing;
}

union CanPrimaryMessages *bal_api_get_cellboard4_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard4balancing;
    }

    struct CanPrimaryTsaccellboard4balancing *payload = &balancing_handler.libcan_message_cellboard4_balancing.tsaccellboard4balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard4_balancing;
}

union CanPrimaryMessages *bal_api_get_cellboard5_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard5balancing;
    }

    struct CanPrimaryTsaccellboard5balancing *payload = &balancing_handler.libcan_message_cellboard5_balancing.tsaccellboard5balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard5_balancing;
}

union CanPrimaryMessages *bal_api_get_cellboard6_canlib_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_bms_byte_size_tsaccellboard6balancing;
    }

    struct CanPrimaryTsaccellboard6balancing *payload = &balancing_handler.libcan_message_cellboard6_balancing.tsaccellboard6balancing;
    payload->cell1 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 0);
    payload->cell2 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 1);
    payload->cell3 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 2);
    payload->cell4 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 3);
    payload->cell5 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 4);
    payload->cell6 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 5);
    payload->cell7 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 6);
    payload->cell8 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 7);
    payload->cell9 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 8);
    payload->cell10 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 9);
    payload->cell11 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 10);
    payload->cell12 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 11);
    payload->cell13 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 12);
    payload->cell14 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 13);
    payload->cell15 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 14);
    payload->cell16 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 15);
    payload->cell17 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 16);
    payload->cell18 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 17);
    payload->cell19 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 18);
    payload->cell20 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 19);
    payload->cell21 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 20);
    payload->cell22 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 21);
    payload->cell23 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 22);
    payload->cell24 = EAGLETRT_API_BIT_GET(balancing_handler.discharging_cells[CELLBOARD_ID_0], 23);
    return &balancing_handler.libcan_message_cellboard6_balancing;
}

void bal_api_set_balancing_state_handle(bool balancing, volt_t threshold) {
    // Ignore stop command if not balancing
    if (!balancing_handler.active && !balancing) {
        return;
    }

    // Update data
    const volt_t target = volt_api_get_min();

    constexpr volt_t bal_target_min = BAL_TARGET_MIN_V;
    constexpr volt_t bal_target_max = BAL_TARGET_MAX_V;
    constexpr volt_t bal_threshold_min = BAL_THRESHOLD_MIN_V;
    constexpr volt_t bal_threshold_max = BAL_THRESHOLD_MAX_V;
    balancing_handler.params.target = EAGLETRT_API_CLAMP(target, bal_target_min, bal_target_max);
    balancing_handler.params.threshold = EAGLETRT_API_CLAMP(threshold, bal_threshold_min, bal_threshold_max);

    // Reset watchdog for each new message
    const enum WatchdogReturnCode code = watchdog_reset(&balancing_handler.watchdog);
    if (code == WATCHDOG_RC_UNAVAILABLE) {
        return;
    }

    // Send event to the FSM
    if (balancing_handler.active != balancing) {
        balancing_handler.event.type = balancing ? FSM_EVENT_TYPE_BALANCING_START : FSM_EVENT_TYPE_BALANCING_STOP;
        fsm_event_trigger(&balancing_handler.event);
    }
}

void bal_api_cellboard_balancing_handle(CellboardId cellboard, bit_flag32_t discharging) {
    if (cellboard >= CELLBOARD_ID_COUNT) {
        return;
    }
    balancing_handler.discharging_cells[cellboard] = discharging;
}

#endif // CONF_BALANCING_MODULE_ENABLE
