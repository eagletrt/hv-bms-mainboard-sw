/*!
 * \file post-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * \brief Power-On Self Test and initialization functions needed to make sure
 *        that the system is working as expected
 */

#include "post-api.h"

#include <stdbool.h>
#include <stddef.h>

#include "bal-control-api.h"
#include "can-bridge-api.h"
#include "can-communication-api.h"
#include "can-communication-info-api.h"
#include "cellboards-monitor-api.h"
#include "display-api.h"
#include "eagletrt-api.h"
#include "eagletrt-fsm.h"
#include "eagletrt.h"
#include "feedback-api.h"
#include "flash-control-api.h"
#include "internal-temperatures-api.h"
#include "internal-voltages-api.h"
#include "led-api.h"
#include "mainboard-def.h"
#include "mainboard-fsm-api.h"
#include "mainboard-logger-api.h"
#include "mainboard-status-api.h"
#include "mainboard-tasks-api.h"
#include "mainboard-watchdogs-api.h"
#include "phc-api.h"
#include "sensing-api.h"
#include "serializer-api.h"
#include "serializer-tasks-api.h"
#include "temperatures-api.h"
#include "voltages-api.h"

/*!
 * \brief Set the POST fault if a check did not pass
 *
 * \param passed The result of the check
 * \param name The name of the checked module, used by the logs
 */
EAGLETRT_STATIC void prv_post_check(const bool passed, const char *const name) {
    if (!passed) {
        MAINBOARD_LOGGER_ERROR("POST: %s init failed", name);
        EAGLETRT_API_UNUSED(mainboard_status_api_set_fault(MAINBOARD_STATUS_FAULT_POST_FAILED));
    }
}

enum PostReturnCode post_api_run(const struct PostInitData *data) {
    if (data == NULL) {
        MAINBOARD_LOGGER_ERROR("POST: NULL init data");
        return POST_RC_NULL_POINTER;
    }

    // Must be first: it clears every fault, including the ones set below
    mainboard_status_api_init();

    // Must come before every module that initializes a watchdog
    prv_post_check(mainboard_watchdogs_api_init(data->get_tick) == WATCHDOG_RC_OK, "watchdogs");

    // CAN interface
    serializer_api_init();
    prv_post_check(can_bridge_api_init(data->send_primary, data->send_bms) == CAN_COMMUNICATION_RC_OK, "CAN bridge");

    const struct CanCommunicationNetworkConfig config[CAN_NETWORK_COUNT] = {
        [CAN_NETWORK_PRIMARY] = {
                                 .send = data->send_primary,
                                 .on_receive = can_bridge_api_receive_primary,
                                 .cs_enter = data->cs_enter,
                                 .cs_exit = data->cs_exit,
                                 },
        [CAN_NETWORK_BMS] = {
                                 .send = data->send_bms,
                                 .on_receive = can_bridge_api_receive_bms,
                                 .cs_enter = data->cs_enter,
                                 .cs_exit = data->cs_exit,
                                 },
    };
    prv_post_check(can_communication_api_init(config) == CAN_COMMUNICATION_RC_OK, "CAN communication");
    prv_post_check(can_communication_info_init() == CAN_COMMUNICATION_INFO_RC_OK, "CAN info");

    // Applications
    prv_post_check(led_api_init(data->led_set, data->led_toggle) == LED_RC_OK, "LED");
    prv_post_check(display_api_init(data->display_set, data->display_toggle) == DISPLAY_RC_OK, "display");
    prv_post_check(cellboards_monitor_api_init() == CELLBOARDS_MONITOR_RC_OK, "cellboards monitor");
    prv_post_check(voltages_api_init() == VOLTAGES_RC_OK, "voltages");
    temperatures_api_init();
    internal_voltages_api_init();
    internal_temperatures_api_init();
    feedback_api_init();
    prv_post_check(bal_control_api_init() == BAL_CONTROL_RC_OK, "bal control");
    prv_post_check(flash_control_api_init() == FLASH_CONTROL_RC_OK, "flash control");
    prv_post_check(phc_api_init() == PHC_RC_OK, "PHC");

    // Sensing, after the modules that receive its values
    prv_post_check(sensing_api_init(&data->sensing) == SENSING_RC_OK, "sensing");

    // Control, last so that the first run finds every module initialized
    prv_post_check(serializer_tasks_api_init(data->get_tick) == SERIALIZER_TASKS_RC_OK, "serializer tasks");
    prv_post_check(mainboard_fsm_api_init() == FSM_RC_OK, "FSM");
    prv_post_check(mainboard_tasks_api_init(data->get_tick) == TASKS_RC_OK, "tasks");

    if (mainboard_status_api_is_fault_set(MAINBOARD_STATUS_FAULT_POST_FAILED)) {
        MAINBOARD_LOGGER_ERROR("POST failed");
        return POST_RC_ERROR;
    }
    MAINBOARD_LOGGER_INFO("POST passed");
    return POST_RC_OK;
}
