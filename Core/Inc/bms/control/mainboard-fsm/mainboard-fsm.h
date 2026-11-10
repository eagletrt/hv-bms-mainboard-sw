/*!
 * \file mainboard-fsm.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the mainboard FSM
 */

#ifndef MAINBOARD_FSM_H
#define MAINBOARD_FSM_H

#include "eagletrt-fsm.h"

/*!
 * \brief Identifiers of the mainboard FSM states
 *
 * \details The values are used as indices of the state table, so they must
 *          start from 0 and be contiguous
 */
enum MainboardFsmStateId {

    INIT,
    IDLE,
    FATAL,
    FLASHING,
    BALANCING,
    WAIT_ECU_OK,
    AIRN_CHECK,
    PRECHARGE_CHECK,
    AIRP_CHECK,
    TS_ON,
    STATE_COUNT

};

/*!
 * \brief Initialize the mainboard FSM in the INIT state
 *
 * \retval FSM_RC_OK if the FSM was initialized correctly
 * \retval FSM_RC_INVALID_MACHINE if the state table is not valid
 * \retval FSM_RC_INVALID_TRANSITION if a transition points to a non existing state
 */
enum FSMReturnCode mainboard_fsm_api_init(void);

/*!
 * \brief Run the routine of the current state and apply the pending transition
 *
 * \retval FSM_RC_OK if the state was executed correctly
 * \retval FSM_RC_INVALID_TRANSITION if the requested transition is not allowed
 * \retval FSM_RC_NULL_POINTER if the FSM was not initialized
 */
enum FSMReturnCode mainboard_fsm_api_run(void);

#endif // MAINBOARD_FSM_H
