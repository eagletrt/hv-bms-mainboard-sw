/**
 * @file programmer.c
 * @date 2024-05-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions used during the flash procedure where the microcontroller
 * is reset and openblt load the new code inside the flash memory
 */

#include "programmer.h"

#include <string.h>
#include <stdbool.h>

_STATIC _ProgrammerHandler hprogrammer;

/**
 * @brief Check if all the cellboards are ready to start the flash procedure
 *
 * @return bool True if all the cellboards are ready, false otherwise
 */
bool _programmer_cellboard_ready_all(void) {
    return (hprogrammer.cellboard_ready & PROGRAMMER_CELLBOARD_READY_MASK) == PROGRAMMER_CELLBOARD_READY_MASK;
}

/** @brief Function called when the watchdog times-out */
void _programmer_flash_timeout(void) {
    hprogrammer.flash_request = false;
    hprogrammer.flashing = false;
    hprogrammer.flash_stop = false;
    hprogrammer.cellboard_ready = 0U;
}

/** @brief Function called when the flash procedure is completed */
void _programmer_flash_stop(void) {
    hprogrammer.flash_request = false;
    hprogrammer.flashing = false;
    hprogrammer.flash_stop = true;
    hprogrammer.cellboard_ready = 0U;
}

/** @brief Resets all the flash flags */
void _programmer_flash_reset_flags(void) { 
    hprogrammer.flash_request = false;
    hprogrammer.flashing = false;
    hprogrammer.flash_stop = false;
    hprogrammer.cellboard_ready = 0U;
}

ProgrammerReturnCode programmer_init(const system_reset_callback_t reset) {
    memset(&hprogrammer, 0U, sizeof(hprogrammer));

    hprogrammer.reset = reset;
    hprogrammer.flash_event.type = FSM_EVENT_TYPE_FLASH_REQUEST;
    hprogrammer.programmer_can_payload.ready = false;

    // Reset flash procedure data
    hprogrammer.target = MAINBOARD_ID;
    _programmer_flash_reset_flags();

    // Initialize watchdogs
    (void)watchdog_init(
        &hprogrammer.watchdog,
        TIMEBASE_TIME_TO_TICKS(PROGRAMMER_FLASH_TIMEOUT_MS, timebase_get_resolution()),
        _programmer_flash_timeout
    );

    return PROGRAMMER_OK;
}

void programmer_flash_request_handle(primary_hv_flash_request_converted_t * const payload) {
    if (payload == NULL)
        return;
    if (hprogrammer.flash_request)
        return;
    const fsm_state_t status = fsm_get_status();
    if (status != FSM_STATE_IDLE && status != FSM_STATE_FATAL)
        return;

    // TODO: Check the payload content

    hprogrammer.target = payload->mainboard ? MAINBOARD_ID : (CellboardId)payload->cellboard_id;
    hprogrammer.flash_request = true;
    hprogrammer.flash_stop = false;
    hprogrammer.flashing = false;

    watchdog_restart(&hprogrammer.watchdog);

    // Trigger event
    fsm_event_trigger(&hprogrammer.flash_event);
}

void programmer_cellboard_flash_response_handle(bms_cellboard_flash_response_converted_t * const payload) {
    if (payload == NULL)
        return;
    if (!hprogrammer.flash_request)
        return;

    // Set the cellboard ready bit
    hprogrammer.cellboard_ready = MAINBOARD_BIT_TOGGLE_IF(
        hprogrammer.cellboard_ready,
        payload->ready,
        payload->cellboard_id
    );
}

void programmer_flash_handle(primary_hv_flash_converted_t * const payload) {
    if (payload == NULL)
        return;
    if (payload->start == hprogrammer.flashing)
        return;
    if (fsm_get_status() != FSM_STATE_FLASH || !hprogrammer.flash_request)
        return;

    if (payload->start) {
        watchdog_reset(&hprogrammer.watchdog);
        hprogrammer.flashing = true;
    }
    else {
        watchdog_stop(&hprogrammer.watchdog);
        _programmer_flash_stop();
    }
}

ProgrammerReturnCode programmer_routine(void) {
    if (watchdog_is_timed_out(&hprogrammer.watchdog))
        return PROGRAMMER_TIMEOUT;
    if (hprogrammer.flash_stop)
        return PROGRAMMER_OK;
    // Wait until all the cellboards are ready
    if (!_programmer_cellboard_ready_all())
        return PROGRAMMER_BUSY;
    // Reset the microcontroller if the mainboard is the target
    if (hprogrammer.target == MAINBOARD_ID)
        hprogrammer.reset();
    return PROGRAMMER_BUSY;
}

