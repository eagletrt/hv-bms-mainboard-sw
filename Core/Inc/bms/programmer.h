/**
 * @file programmer.h
 * @date 2024-05-12
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Functions used during the flash procedure where the microcontroller
 * is reset and openblt load the new code inside the flash memory
 */

#ifndef PROGRAMMER_H
#define PROGRAMMER_H

#include "primary_network.h"
#include "bms_network.h"
#include "mainboard-def.h"
#include "watchdog.h"
#include "identity.h"
#include "timebase.h"
#include "fsm.h"

/** @brief The programmer flash timeout in ms */
#define PROGRAMMER_FLASH_TIMEOUT_MS (1000U)

/** @brief Cellboard ready bit mask */
#define PROGRAMMER_CELLBOARD_READY_MASK (0x3FU)

/**
 * @brief Return code for the programmer module functions 
 *
 * @details
 *     - PROGRAMMER_OK the function executed succesfully
 *     - PROGRAMMER_BUSY the flash procedure is ongoing
 *     - PROGRAMMER_TIMEOUT the procedure has timed out
 */
typedef enum {
    PROGRAMMER_OK,
    PROGRAMMER_BUSY,
    PROGRAMMER_TIMEOUT
} ProgrammerReturnCode;

/**
 * @brief Type definition for the programmer handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @param reset A pointer to a function that resets the microcontroller
 * @param flash_event The FSM event data
 * @param programmer_can_payload The flash response canlib data
 * @param target The identifier of the cellboard(or mainboard) to flash
 * @param flash_request True if a flash request is received, false otherwise
 * @param flashing True if the cellboard is flashing, false otherwise
 * @param watchog The watchdog used for the flash procedure
 * @param timeout True if the watchdog has timed-out, false otherwise
 */
typedef struct {
    system_reset_callback_t reset;
    fsm_event_data_t flash_event;
    primary_hv_flash_response_converted_t programmer_can_payload;

    CellboardId target;
    bool flash_request;
    bool flashing;
    bool flash_stop;

    // Bit flag where each bit represent a cellboard; if 1 the cellboard is ready
    // for the flash procedure, otherwise the flash procedure cannot be started
    bit_flag8_t cellboard_ready;

    Watchdog watchdog;
} _ProgrammerHandler;

/**
 * @brief Intialize the internal programmer handler structure
 *
 * @param reset A pointer to the function that resets the microcontroller
 *
 * @return ProgrammerReturnCode
 *     - PROGRAMMER_OK
 */
ProgrammerReturnCode programmer_init(const system_reset_callback_t reset);

/**
 * @brief Handle the received flash request
 *
 * @param payload A pointer to the canlib payload of the request
 */
void programmer_flash_request_handle(primary_hv_flash_request_converted_t * const payload);

/**
 * @brief Handle the received flash response from the cellboards
 *
 * @param payload A pointer to the canlib payload of the response
 */
void programmer_cellboard_flash_response_handle(bms_cellboard_flash_response_converted_t * const payload);

/**
 * @brief Handle the received actual flash command
 *
 * @param payload A pointer to the canlib payload of the command
 */
void programmer_flash_handle(primary_hv_flash_converted_t * const payload);

/**
 * @brief Routine that should be called during the flash procedure
 *
 * @attention This function can reset the microcontroller
 *
 * @return ProgrammerReturnCode
 *     - PROGRAMMER_TIMEOUT if the flash procedure has timed out
 *     - PROGRAMMER_OK if the flash procedure has finished
 *     - PROGRAMMER_BUSY otherwise
 */
ProgrammerReturnCode programmer_routine(void);

#endif  // PROGRAMMER_H
