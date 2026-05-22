/*!
 * \file programmer.c
 * \date 2024-05-12
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used during the flash procedure where the microcontroller
 * is reset and openblt load the new code inside the flash memory
 */

#include "programmer-api.h"

#include <string.h>
#include <stdbool.h>
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ProgrammerHandler programmer_handler;

/*!
 * \brief Check if all the cellboards are ready to start the flash procedure
 *
 * \return bool True if all the cellboards are ready, false otherwise
 */
bool prv_programmer_cellboard_ready_all(void) {
    return (programmer_handler.cellboard_ready & PROGRAMMER_CELLBOARD_READY_MASK) == PROGRAMMER_CELLBOARD_READY_MASK;
}

/*! \brief Function called when the watchdog times-out */
void prv_programmer_flash_timeout(void) {
    programmer_handler.flash_request = false;
    programmer_handler.flashing = false;
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = 0U;
}

/*! \brief Function called when the flash procedure is completed */
void prv_programmer_flash_stop(void) {
    programmer_handler.flash_request = false;
    programmer_handler.flashing = false;
    programmer_handler.flash_stop = true;
    programmer_handler.cellboard_ready = 0U;
}

/*! \brief Resets all the flash flags */
void prv_programmer_flash_reset_flags(void) {
    programmer_handler.flash_request = false;
    programmer_handler.flashing = false;
    programmer_handler.flash_stop = false;
    programmer_handler.cellboard_ready = 0U;
}

enum ProgrammerReturnCode programmer_api_init(const system_reset_callback_t reset) {
    memset(&programmer_handler, 0U, sizeof(programmer_handler));

    programmer_handler.reset = reset;
    programmer_handler.flash_event.type = FSM_EVENT_TYPE_FLASH_REQUEST;
    programmer_handler.programmer_can_payload.ready = false;

    // Reset flash procedure data
    programmer_handler.target = MAINBOARD_ID;
    prv_programmer_flash_reset_flags();

    // Initialize watchdogs
    (void)watchdog_init(
        &programmer_handler.watchdog,
        TIMEBASE_TIME_TO_TICKS(PROGRAMMER_FLASH_TIMEOUT_MS, timebase_get_resolution()),
        prv_programmer_flash_timeout);

    return PROGRAMMER_RC_OK;
}

void programmer_api_flash_request_handle(primary_hv_flash_request_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    if (programmer_handler.flash_request) {
        return;
    }
    const fsm_state_t status = fsm_get_status();
    if (status != FSM_STATE_IDLE && status != FSM_STATE_FATAL) {
        return;
    }

    // TODO: Check the payload content

    programmer_handler.target = payload->mainboard ? MAINBOARD_ID : (CellboardId)payload->cellboard_id;
    programmer_handler.flash_request = true;
    programmer_handler.flash_stop = false;
    programmer_handler.flashing = false;

    watchdog_restart(&programmer_handler.watchdog);

    // Trigger event
    fsm_event_trigger(&programmer_handler.flash_event);
}

void programmer_api_cellboard_flash_response_handle(bms_cellboard_flash_response_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    if (!programmer_handler.flash_request) {
        return;
    }

    // Set the cellboard ready bit
    programmer_handler.cellboard_ready = MAINBOARD_BIT_TOGGLE_IF(
        programmer_handler.cellboard_ready,
        payload->ready,
        payload->cellboard_id);
}

void programmer_api_flash_handle(primary_hv_flash_converted_t *const payload) {
    if (payload == NULL) {
        return;
    }
    if ((bool)payload->start == programmer_handler.flashing) {
        return;
    }
    if (fsm_get_status() != FSM_STATE_FLASH || !programmer_handler.flash_request) {
        return;
    }

    if (payload->start) {
        watchdog_reset(&programmer_handler.watchdog);
        programmer_handler.flashing = true;
    } else {
        watchdog_stop(&programmer_handler.watchdog);
        prv_programmer_flash_stop();
    }
}

enum ProgrammerReturnCode programmer_api_routine(void) {
    if (watchdog_is_timed_out(&programmer_handler.watchdog)) {
        return PROGRAMMER_RC_TIMEOUT;
    }
    if (programmer_handler.flash_stop) {
        return PROGRAMMER_RC_OK;
    }
    // Wait until all the cellboards are ready
    if (!prv_programmer_cellboard_ready_all()) {
        return PROGRAMMER_RC_BUSY;
    }
    // Reset the microcontroller if the mainboard is the target
    if (programmer_handler.target == MAINBOARD_ID) {
        programmer_handler.reset();
    }
    return PROGRAMMER_RC_BUSY;
}
