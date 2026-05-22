/*!
 * \file programmer.h
 * \date 2024-05-12
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Functions used during the flash procedure where the microcontroller
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

/*! \brief The programmer flash timeout in ms */
#define PROGRAMMER_FLASH_TIMEOUT_MS (1000U)

/*! \brief Cellboard ready bit mask */
#define PROGRAMMER_CELLBOARD_READY_MASK (0x3FU)

/*!
 * \brief Return code for the programmer module functions 
 */
enum ProgrammerReturnCode {
    PROGRAMMER_RC_OK,     /*!< The function executed successfully */
    PROGRAMMER_RC_BUSY,   /*!< The flash procedure is ongoing */
    PROGRAMMER_RC_TIMEOUT /*!< The procedure has timed out */
};

/*!
 * \brief Type definition for the programmer handler structure
 *
 * \warning This structure should never be used outside of this file
 */
struct ProgrammerHandler {
    system_reset_callback_t reset;                                /*!< A pointer to a function that resets the microcontroller */
    fsm_event_data_t flash_event;                                 /*!< The FSM event data used to trigger the flash procedure */
    primary_hv_flash_response_converted_t programmer_can_payload; /*!< The canlib payload used to send the flash response to the mainboard */

    CellboardId target; /*!< The identifier of the cellboard(or mainboard) to flash */
    bool flash_request; /*!< True if a flash request is received, false otherwise */
    bool flashing;      /*!< True if the cellboard is flashing, false otherwise */
    bool flash_stop;    /*!< True if the flash procedure should be stopped, false otherwise */

    bit_flag8_t cellboard_ready; /*!< A bit flag where each bit represent a cellboard; if 1 the cellboard is ready for the flash procedure, otherwise the flash procedure cannot be started */

    Watchdog watchdog; /*!< The watchdog used for the flash procedure */
};

#endif // PROGRAMMER_H
