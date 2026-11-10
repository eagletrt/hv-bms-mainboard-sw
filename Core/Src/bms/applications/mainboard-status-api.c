/*!
 * \file mainboard-status-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Mainboard status, the single source of truth for the requests, faults
 *        and conditions of the mainboard
 */

#include "mainboard-status-api.h"

#include <string.h>

#include "eagletrt.h"

EAGLETRT_STATIC struct MainboardStatusHandler mainboard_status_handler;

static const char *const fault_names[MAINBOARD_STATUS_FAULT_COUNT] = {
    [MAINBOARD_STATUS_FAULT_POST_FAILED] = "post failed",
    [MAINBOARD_STATUS_FAULT_CELL_UNDER_VOLTAGE] = "cell under voltage",
    [MAINBOARD_STATUS_FAULT_CELL_OVER_VOLTAGE] = "cell over voltage",
    [MAINBOARD_STATUS_FAULT_CELL_UNDER_TEMPERATURE] = "cell under temperature",
    [MAINBOARD_STATUS_FAULT_CELL_OVER_TEMPERATURE] = "cell over temperature",
    [MAINBOARD_STATUS_FAULT_OVER_CURRENT] = "over current",
    [MAINBOARD_STATUS_FAULT_OVER_POWER] = "over power",
    [MAINBOARD_STATUS_FAULT_COOLING_UNDER_TEMPERATURE] = "cooling under temperature",
    [MAINBOARD_STATUS_FAULT_COOLING_OVER_TEMPERATURE] = "cooling over temperature",
    [MAINBOARD_STATUS_FAULT_CELLBOARD_FAULT] = "cellboard fault",
    [MAINBOARD_STATUS_FAULT_IMD_FAULT] = "imd fault",
    [MAINBOARD_STATUS_FAULT_VOLTAGE_INDICATOR_DISCONNECTED] = "voltage indicator disconnected",
    [MAINBOARD_STATUS_FAULT_STALE_CELL_VOLTAGES] = "stale cell voltages",
    [MAINBOARD_STATUS_FAULT_STALE_CELL_TEMPERATURES] = "stale cell temperatures",
    [MAINBOARD_STATUS_FAULT_STALE_CELLBOARD_STATUS] = "stale cellboard status",
    [MAINBOARD_STATUS_FAULT_STALE_CURRENT] = "stale current",
    [MAINBOARD_STATUS_FAULT_STALE_INTERNAL_VOLTAGES] = "stale internal voltages",
    [MAINBOARD_STATUS_FAULT_STALE_INTERNAL_TEMPERATURES] = "stale internal temperatures",
    [MAINBOARD_STATUS_FAULT_STALE_SENSORS] = "stale sensors",
};

static const char *const request_names[MAINBOARD_STATUS_REQUEST_COUNT] = {
    [MAINBOARD_STATUS_REQUEST_TS_ON] = "ts on",
    [MAINBOARD_STATUS_REQUEST_TS_OFF] = "ts off",
    [MAINBOARD_STATUS_REQUEST_ECU_OK] = "ecu ok",
    [MAINBOARD_STATUS_REQUEST_FLASH] = "flash",
    [MAINBOARD_STATUS_REQUEST_BALANCING_START] = "balancing start",
    [MAINBOARD_STATUS_REQUEST_BALANCING_STOP] = "balancing stop",
};

static const char *const condition_names[MAINBOARD_STATUS_CONDITION_COUNT] = {
    [MAINBOARD_STATUS_CONDITION_AIRN_CLOSE_COMMANDED] = "air- close commanded",
    [MAINBOARD_STATUS_CONDITION_AIRN_CLOSED] = "air- closed",
    [MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSE_COMMANDED] = "precharge close commanded",
    [MAINBOARD_STATUS_CONDITION_PRECHARGE_CLOSED] = "precharge closed",
    [MAINBOARD_STATUS_CONDITION_AIRP_CLOSE_COMMANDED] = "air+ close commanded",
    [MAINBOARD_STATUS_CONDITION_AIRP_CLOSED] = "air+ closed",
    [MAINBOARD_STATUS_CONDITION_TS_BELOW_60V] = "ts below 60v",
    [MAINBOARD_STATUS_CONDITION_TSAL_GREEN] = "tsal green",
    [MAINBOARD_STATUS_CONDITION_VOLTAGE_INDICATOR_CONNECTED] = "voltage indicator connected",
    [MAINBOARD_STATUS_CONDITION_PRECHARGE_COMPLETE] = "precharge complete",
    [MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE] = "plausible state",
    [MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_FILTERED] = "plausible state filtered",
    [MAINBOARD_STATUS_CONDITION_PLAUSIBLE_STATE_PERSISTED] = "plausible state persisted",
    [MAINBOARD_STATUS_CONDITION_AMS_FAULT_LATCHED] = "ams fault latched",
    [MAINBOARD_STATUS_CONDITION_IMD_FAULT_LATCHED] = "imd fault latched",
    [MAINBOARD_STATUS_CONDITION_PLAUSIBILITY_FAULT_LATCHED] = "plausibility fault latched",
    [MAINBOARD_STATUS_CONDITION_EXTERNAL_FAULT_LATCHED] = "external fault latched",
    [MAINBOARD_STATUS_CONDITION_LATCH_RESET_ACTIVE] = "latch reset active",
    [MAINBOARD_STATUS_CONDITION_AMS_COCKPIT_LED_ON] = "ams cockpit led on",
    [MAINBOARD_STATUS_CONDITION_IMD_COCKPIT_LED_ON] = "imd cockpit led on",
    [MAINBOARD_STATUS_CONDITION_IMD_OK] = "imd ok",
    [MAINBOARD_STATUS_CONDITION_SD_IN_CLOSED] = "sd in closed",
    [MAINBOARD_STATUS_CONDITION_SD_AMS_NODE_CLOSED] = "sd ams node closed",
    [MAINBOARD_STATUS_CONDITION_SD_IMD_NODE_CLOSED] = "sd imd node closed",
    [MAINBOARD_STATUS_CONDITION_SD_OUT_CLOSED] = "sd out closed",
    [MAINBOARD_STATUS_CONDITION_SD_END_CLOSED] = "sd end closed",
    [MAINBOARD_STATUS_CONDITION_MCU_5V_OK] = "mcu 5v ok",
    [MAINBOARD_STATUS_CONDITION_PROBING_3V3_OK] = "probing 3v3 ok",
    [MAINBOARD_STATUS_CONDITION_HANDCART_CONNECTED] = "handcart connected",
};

/*!
 * \brief Pack an array of flags into a bit flag
 *
 * \param flags The array of flags
 * \param count The number of flags
 *
 * \returns The bit flag where bit i is flags[i]
 */
static bit_flag32 prv_mainboard_status_pack(const volatile bool *const flags, const size_t count) {
    bit_flag32 packed = 0U;
    for (size_t i = 0U; i < count; ++i) {
        if (flags[i]) {
            packed |= MAINBOARD_STATUS_BIT(i);
        }
    }
    return packed;
}

void mainboard_status_api_init(void) {
    memset((void *)&mainboard_status_handler, 0U, sizeof(mainboard_status_handler));
}

/* Faults */

enum MainboardStatusReturnCode mainboard_status_api_set_fault(const enum MainboardStatusFault fault) {
    if (fault >= MAINBOARD_STATUS_FAULT_COUNT) {
        return MAINBOARD_STATUS_RC_INVALID_FAULT;
    }
    mainboard_status_handler.faults[fault] = true;
    return MAINBOARD_STATUS_RC_OK;
}

bool mainboard_status_api_is_fault_set(const enum MainboardStatusFault fault) {
    if (fault >= MAINBOARD_STATUS_FAULT_COUNT) {
        return false;
    }
    return mainboard_status_handler.faults[fault];
}

bool mainboard_status_api_has_faults(void) {
    return mainboard_status_api_get_faults() != 0U;
}

bit_flag32 mainboard_status_api_get_faults(void) {
    return prv_mainboard_status_pack(mainboard_status_handler.faults, MAINBOARD_STATUS_FAULT_COUNT);
}

const char *mainboard_status_api_get_fault_name(const enum MainboardStatusFault fault) {
    if (fault >= MAINBOARD_STATUS_FAULT_COUNT) {
        return "invalid fault";
    }
    return fault_names[fault];
}

/* Requests */

enum MainboardStatusReturnCode mainboard_status_api_set_request(const enum MainboardStatusRequest request, const bool active) {
    if (request >= MAINBOARD_STATUS_REQUEST_COUNT) {
        return MAINBOARD_STATUS_RC_INVALID_REQUEST;
    }
    mainboard_status_handler.requests[request] = active;
    return MAINBOARD_STATUS_RC_OK;
}

bool mainboard_status_api_is_request_set(const enum MainboardStatusRequest request) {
    if (request >= MAINBOARD_STATUS_REQUEST_COUNT) {
        return false;
    }
    return mainboard_status_handler.requests[request];
}

bit_flag32 mainboard_status_api_get_requests(void) {
    return prv_mainboard_status_pack(mainboard_status_handler.requests, MAINBOARD_STATUS_REQUEST_COUNT);
}

const char *mainboard_status_api_get_request_name(const enum MainboardStatusRequest request) {
    if (request >= MAINBOARD_STATUS_REQUEST_COUNT) {
        return "invalid request";
    }
    return request_names[request];
}

/* Conditions */

enum MainboardStatusReturnCode mainboard_status_api_set_condition(const enum MainboardStatusCondition condition, const bool value) {
    if (condition >= MAINBOARD_STATUS_CONDITION_COUNT) {
        return MAINBOARD_STATUS_RC_INVALID_CONDITION;
    }
    mainboard_status_handler.conditions[condition] = value;
    return MAINBOARD_STATUS_RC_OK;
}

bool mainboard_status_api_get_condition(const enum MainboardStatusCondition condition) {
    if (condition >= MAINBOARD_STATUS_CONDITION_COUNT) {
        return false;
    }
    return mainboard_status_handler.conditions[condition];
}

bit_flag32 mainboard_status_api_get_conditions(void) {
    return prv_mainboard_status_pack(mainboard_status_handler.conditions, MAINBOARD_STATUS_CONDITION_COUNT);
}

bool mainboard_status_api_check_conditions(const bit_flag32 mask, const bit_flag32 expected, bit_flag32 *const mismatches) {
    // Read all the conditions once so that every check uses the same snapshot
    const bit_flag32 different = (mainboard_status_api_get_conditions() ^ expected) & mask;

    if (mismatches != NULL) {
        *mismatches = different;
    }
    return different == 0U;
}

const char *mainboard_status_api_get_condition_name(const enum MainboardStatusCondition condition) {
    if (condition >= MAINBOARD_STATUS_CONDITION_COUNT) {
        return "invalid condition";
    }
    return condition_names[condition];
}
