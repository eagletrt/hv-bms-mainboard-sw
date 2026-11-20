/*!
 * \file pcu-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Pack Control Unit (PCU): drives the relays and the AMS output on behalf of the FSM
 *
 * \details The PCU only actuates, the FSM decides when. Every relay command is mirrored
 *          in the corresponding *_CLOSE_COMMANDED condition of the mainboard status.
 *          The PCU also provides the time counter and the microcontroller reset to the
 *          rest of the firmware
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
 * \param tick The callback used to get the current time in ms
 * \param reset The callback used to reset the microcontroller
 *
 * \retval PCU_RC_NULL_POINTER if one of the callbacks is NULL
 * \retval PCU_RC_OK otherwise
 */
enum PcuReturnCode pcu_api_init(pcu_set_state_callback set, pcu_get_tick_callback tick, system_reset_callback reset);

/*!
 * \brief Close a relay and start counting its timeout
 *
 * \details Closing a relay that is already closed does not restart its timeout
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
 * \brief Check if the step started by closing a relay did not complete in time
 *
 * \details The step is complete when the done condition of the relay is true:
 *          - AIR-: the AIR- is mechanically closed
 *          - precharge: the precharge is complete
 *          - AIR+: the AIR+ is mechanically closed
 *
 * \param relay The relay to check
 *
 * \returns true if the relay is commanded closed, its done condition is false and
 *          its timeout has elapsed, false otherwise
 */
bool pcu_api_is_timed_out(enum PcuRelay relay);

/*!
 * \brief Activate the AMS, opening the shutdown circuit
 *
 * \attention The AMS stays active until the microcontroller is reset
 */
void pcu_api_ams_activate(void);

/*!
 * \brief Get the current time
 *
 * \returns The time elapsed since boot in ms, 0 if the PCU is not initialized
 */
milliseconds pcu_api_get_tick(void);

/*! \brief Reset the microcontroller, does nothing if the PCU is not initialized */
void pcu_api_reset_microcontroller(void);

#endif // PCU_API_H
