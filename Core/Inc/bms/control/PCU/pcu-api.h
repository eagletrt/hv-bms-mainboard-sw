/*!
 * \file pcu-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU): drives the relays and the AMS output on behalf of the FSM
 *
 * \details The PCU only actuates, the FSM decides when. The hardware readback of every
 *          relay command line is published by the feedback module in the corresponding
 *          *_CLOSE_COMMANDED condition of the mainboard status.
 *          The PCU also provides the microcontroller reset to the rest of the firmware
 */

#ifndef PCU_API_H
#define PCU_API_H

#include "pcu.h"

/*!
 * \brief Initialize the internal PCU handler
 *
 * \details Every relay is opened and the AMS is set inactive
 *
 * \param set The callback used to set the status of the PCU pins
 * \param reset The callback used to reset the microcontroller
 *
 * \retval PCU_RC_NULL_POINTER if one of the callbacks is NULL
 * \retval PCU_RC_OK otherwise
 */
enum PcuReturnCode pcu_api_init(pcu_set_state_callback set, system_reset_callback reset);

/*!
 * \brief Close a relay
 *
 * \param relay The relay to close
 *
 * \retval PCU_RC_INVALID_RELAY if the relay does not exist
 * \retval PCU_RC_OK otherwise
 */
enum PcuReturnCode pcu_api_close(enum PcuRelay relay);

/*!
 * \brief Open a relay
 *
 * \param relay The relay to open
 *
 * \retval PCU_RC_INVALID_RELAY if the relay does not exist
 * \retval PCU_RC_OK otherwise
 */
enum PcuReturnCode pcu_api_open(enum PcuRelay relay);

/*! \brief Open every relay, the AMS is left untouched */
void pcu_api_open_all(void);

/*!
 * \brief Check if a relay is commanded closed
 *
 * \param relay The relay to check
 *
 * \returns true if the relay is commanded closed, false if it is open or does not exist
 */
bool pcu_api_is_closed(enum PcuRelay relay);

/*!
 * \brief Activate the AMS, opening the shutdown circuit
 *
 * \attention The AMS stays active until the microcontroller is reset
 */
void pcu_api_ams_activate(void);

/*! \brief Reset the microcontroller, does nothing if the PCU is not initialized */
void pcu_api_reset_microcontroller(void);

#endif // PCU_API_H
