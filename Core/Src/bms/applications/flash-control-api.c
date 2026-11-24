/*!
 * \file flash-control-api.c
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Flash procedure control: resets the microcontroller when the mainboard is
 *        the target or enables the CAN bridge when a cellboard is the target
 */

#include "flash-control-api.h"
#include "can-bridge-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "mainboard-conf.h"
#include "mainboard-status-api.h"
#include "mainboard-status.h"
#include "mainboard-watchdogs-api.h"
#include "pcu-api.h"
#include "serializer-tasks-api.h"
#include "watchdogs.h"
#include <string.h>

EAGLETRT_STATIC struct FlashControlHandler flash_control_handler;

/*!
 * \brief Called when no flashing message is received within FLASH_CONTROL_MAX_MESSAGE_INTERVAL_MS
 *
 * \details Drops the flash request as if a none target was received. The flash
 *          procedure itself is stopped by the FSM once it sees the request cleared.
 *          The watchdog is left timed out, the next flashing message restarts it
 *
 * \param[in] context Unused
 */
EAGLETRT_STATIC void prv_flash_control_handle_message_timeout(void *context) {
    EAGLETRT_API_UNUSED(context);

    flash_control_handler.target = FLASH_CONTROL_REQUEST_NONE;
    flash_control_handler.valid_parameters = false;
    EAGLETRT_API_UNUSED(mainboard_status_api_set_request(MAINBOARD_STATUS_REQUEST_FLASH, false));
}

enum FlashControlReturnCode flash_control_api_init(void) {
    // Zeroing the handler would corrupt the watchdog pool, which may still hold the running watchdog
    if (flash_control_handler.initialized) {
        return FLASH_CONTROL_RC_ERROR;
    }

    memset(&flash_control_handler, 0, sizeof(flash_control_handler));
    flash_control_handler.target = FLASH_CONTROL_REQUEST_NONE;

    if (mainboard_watchdogs_api_init_watchdog(&flash_control_handler.message_watchdog,
                                              FLASH_CONTROL_MAX_MESSAGE_INTERVAL_MS,
                                              prv_flash_control_handle_message_timeout,
                                              NULL) != WATCHDOG_RC_OK) {
        return FLASH_CONTROL_RC_ERROR;
    }

    flash_control_handler.initialized = true;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_set_target(const enum FlashControlRequestState target) {
    // The watchdog is not initialized yet: touching it would latch the watchdogs fault
    if (!flash_control_handler.initialized) {
        return FLASH_CONTROL_RC_ERROR;
    }
    if (target >= FLASH_CONTROL_REQUEST_COUNT) {
        return FLASH_CONTROL_RC_INVALID_PARAMETERS;
    }

    if (target == FLASH_CONTROL_REQUEST_NONE) {
        // Reset instead of stop, since the watchdog may be idle or timed out. A failure latches the watchdogs fault in the wrapper
        EAGLETRT_API_UNUSED(mainboard_watchdogs_api_reset(&flash_control_handler.message_watchdog));

        flash_control_handler.target = target;
        flash_control_handler.valid_parameters = false;
        return FLASH_CONTROL_RC_OK;
    }

    // Restart instead of pet, since a timed out watchdog must come back for a new flash session.
    // A failure latches the watchdogs fault in the wrapper
    EAGLETRT_API_UNUSED(mainboard_watchdogs_api_restart(&flash_control_handler.message_watchdog));

    flash_control_handler.target = target;
    flash_control_handler.valid_parameters = true;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_start_flash(void) {
    if (!flash_control_handler.valid_parameters) {
        return FLASH_CONTROL_RC_INVALID_PARAMETERS;
    }
    if (flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_ALREADY_STARTED;
    }

    if (flash_control_handler.target == FLASH_CONTROL_REQUEST_MAINBOARD) {
        pcu_api_reset_microcontroller();
    }

    if (flash_control_handler.target == FLASH_CONTROL_REQUEST_CELLBOARD) {
        can_bridge_api_enable_bridge();
    }

    // Only the low priority messages are stopped, the safety ones keep being sent during the flash
    EAGLETRT_API_UNUSED(serializer_tasks_api_suppress_comms(SERIALIZER_TASKS_PRIORITY_LOW, true));

    flash_control_handler.flash_active = true;
    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_routine(void) {
    if (!flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_NOT_STARTED;
    }

    // Todaloo do nothing ->-

    return FLASH_CONTROL_RC_OK;
}

enum FlashControlReturnCode flash_control_api_stop_flash(void) {
    if (!flash_control_handler.flash_active) {
        return FLASH_CONTROL_RC_FLASH_NOT_STARTED;
    }

    EAGLETRT_API_UNUSED(serializer_tasks_api_suppress_comms(SERIALIZER_TASKS_PRIORITY_LOW, false));

    can_bridge_api_disable_bridge();

    flash_control_handler.flash_active = false;
    return FLASH_CONTROL_RC_OK;
}

bool flash_control_api_has_valid_parameters(void) {
    return flash_control_handler.valid_parameters;
}
