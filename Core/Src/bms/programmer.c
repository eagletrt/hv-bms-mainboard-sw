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

#include "fsm.h"
#include "identity.h"
#include "timebase.h"
#include "watchdog.h"

/**
 * @brief Programmer handler structure
 *
 * @param reset A pointer to a function that resets the microcontroller
 * @param flash_event The FSM event data
 * @param can_payload The flash response canlib data
 * @param target The identifier of the cellboard(or mainboard) to flash
 * @param flash_request True if a flash request is received, false otherwise
 * @param flashing True if the cellboard is flashing, false otherwise
 * @param watchog The watchdog used for the flash procedure
 * @param timeout True if the watchdog has timed-out, false otherwise
 */
_STATIC struct {
    system_reset_callback_t reset;
    fsm_event_data_t flash_event;
    primary_hv_flash_response_converted_t can_payload;

    CellboardId target;
    bool flash_request;
    bool flashing;
    bool flash_stop;

    // Bit flag where each bit represent a cellboard; if 1 the cellboard is ready
    // for the flash procedure, otherwise the flash procedure cannot be started
    bit_flag8_t cellboard_ready;

    Watchdog watchdog;
} hprogrammer;

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
    hprogrammer.cellboard_ready = 0;
}

/** @brief Function called when the flash procedure is completed */
void _programmer_flash_stop(void) {
    hprogrammer.flash_request = false;
    hprogrammer.flashing = false;
    hprogrammer.flash_stop = true;
    hprogrammer.cellboard_ready = 0;
}

/** @brief Resets all the flash flags */
void _programmer_flash_reset_flags(void) { 
    hprogrammer.flash_request = false;
    hprogrammer.flashing = false;
    hprogrammer.flash_stop = false;
    hprogrammer.cellboard_ready = 0;
}

ProgrammerReturnCode programmer_init(system_reset_callback_t reset) {
    memset(&hprogrammer, 0U, sizeof(hprogrammer));

    hprogrammer.reset = reset;
    hprogrammer.flash_event.type = FSM_EVENT_TYPE_FLASH_REQUEST;
    hprogrammer.can_payload.ready = false;

    // Reset flash procedure data
    hprogrammer.target = MAINBOARD_ID;
    _programmer_flash_reset_flags();

    // Initialize watchdogs
    (void)watchdog_init(
        &hprogrammer.watchdog,
        TIMEBASE_TIME_TO_TICKS(PROGRAMMER_FLASH_TIMEOUT, timebase_get_resolution()),
        _programmer_flash_timeout
    );

    return PROGRAMMER_OK;
}

void programmer_flash_request_handle(primary_hv_flash_request_converted_t * payload) {
    if (payload == NULL)
        return;
    if (hprogrammer.flash_request)
        return;
    fsm_state_t status = fsm_get_status();
    if (status != FSM_STATE_IDLE && status != FSM_STATE_FATAL)
        return;

    // TODO: Check the payload content

    hprogrammer.target = payload->mainboard ? MAINBOARD_ID : payload->cellboard_id;
    hprogrammer.flash_request = true;
    hprogrammer.flash_stop = false;
    hprogrammer.flashing = false;

    watchdog_restart(&hprogrammer.watchdog);

    // Trigger event
    fsm_event_trigger(&hprogrammer.flash_event);
}

void programmer_cellboard_flash_response_handle(bms_cellboard_flash_response_converted_t * payload) {
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

void programmer_flash_handle(primary_hv_flash_converted_t * payload) {
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

