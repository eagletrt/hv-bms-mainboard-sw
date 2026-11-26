/*!
 * \file mainboard-fsm-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main mainboard FSM that handles all the operations of the BMS
 *
 * \details The FSM only decides: it reads the requests, the conditions and the faults
 *          from the mainboard status and drives the relays through the PCU.
 *          From IDLE it can start one of three procedures:
 *          - TS on: WAIT_OK -> AIRN_CHECK -> PRECHARGE_CHECK -> AIRP_CHECK -> TS_ON,
 *            any abort, timeout or open shutdown circuit goes to DISCHARGE_TS and then back to IDLE
 *          - Balancing: BALANCING_READ <-> BALANCING_DISCHARGE, back to IDLE once the request
 *            drops or the pack is balanced
 *          - Flash: FLASH, back to IDLE once the request drops
 *
 *          Any fault goes to FATAL, which has no way out since faults are never cleared
 *
 * \attention The watchdog pool must be initialized before the first run, the INIT state
 *            initializes the FSM watchdogs
 * \attention Every function must be called from the same context as
 *            \ref mainboard_watchdogs_api_routine, never from an interrupt
 */

#ifndef MAINBOARD_FSM_API_H
#define MAINBOARD_FSM_API_H

#include "eagletrt-fsm.h"
#include "mainboard-fsm.h"

/*!
 * \brief Initialize the mainboard FSM in the INIT state
 *
 * \details The modules used by the FSM are initialized by the INIT state on the first run
 *
 * \retval FSM_RC_INVALID_MACHINE if the state table is invalid
 * \retval FSM_RC_INVALID_STATE if the initial state is invalid
 * \retval FSM_RC_OK otherwise
 */
enum FSMReturnCode mainboard_fsm_api_init(void);

/*!
 * \brief Run the current state of the mainboard FSM and apply the transition it requested, if any
 *
 * \details Must be called periodically, its period adds to the resolution of every
 *          timeout of the TS on procedure
 *
 * \param context Passed unchanged to the state and to the transition functions, may be NULL
 *
 * \retval FSM_RC_INVALID_TRANSITION if a state requested a transition that is not in its table
 * \retval FSM_RC_NULL_POINTER if the FSM is not initialized
 * \retval FSM_RC_OK otherwise
 */
enum FSMReturnCode mainboard_fsm_api_run(void *context);

/*!
 * \brief Get the current state of the mainboard FSM
 *
 * \returns The current state, \ref MAINBOARD_FSM_STATE_INIT if the FSM is not initialized
 */
enum MainboardFsmStateID mainboard_fsm_api_get_state(void);

/*!
 * \brief Get the name of a state of the mainboard FSM
 *
 * \param state The state
 *
 * \returns The name of the state, "UNKNOWN" if it is out of range
 */
const char *mainboard_fsm_api_get_state_name(enum MainboardFsmStateID state);

#endif // MAINBOARD_FSM_API_H
