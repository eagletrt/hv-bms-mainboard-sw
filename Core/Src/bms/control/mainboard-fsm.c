/*!
 * \file mainboard-fsm.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main mainboard FSM that handles all the operations of the BMS
 */

#include "mainboard-fsm.h"

#include "eagletrt-fsm-api.h"

/*!
 * \brief Handler of the mainboard FSM
 */
static struct FSMHandler hfsm;

/*!
 * \brief Initialize the mainboard
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_init(void *data) {
}

/*!
 * \brief Idle routine for the mainboard
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_idle(void *data) {
}

/*!
 * \brief Fatal error routine for the mainboard
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_fatal(void *data) {
}

/*!
 * \brief Transition function from: 
 * Idle -> Flashing
 * Fatal -> Flashing
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void enter_flashing_state(void *data) {
}

/*!
 * \brief Routine for the flashing state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_flashing(void *data) {
}

/*!
 * \brief Transition function from:
 * Flashing -> Idle
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void exit_flashing_state(void *data) {
}

/*!
 * \brief Transition function from:
 * Idle -> Balancing
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void enter_balancing_state(void *data) {
}

/*!
 * \brief Routine for the balancing state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_balancing(void *data) {
}

/*!
 * \brief Transition function from:
 * Balancing -> Idle
 * Balancing -> Fatal
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void exit_balancing_state(void *data) {
}

/*!
 * \brief Intermediate routine waiting for the ECU to acknowledge the BMS
 * 
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_wait_ecu_ok(void *data) {
}

/*!
 * \brief Routine for the AIR- checks
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_airn_check(void *data) {
}

/*!
 * \brief Routine for the precharge
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_precharge_check(void *data) {
}

/*!
 * \brief Routine for the AIR+ checks
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_airp_check(void *data) {
}

/*!
 * \brief Routine for the TS_ON state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_ts_on(void *data) {
}

/*!
 * \defgroup transitions Transition tables
 * \brief Allowed transitions out of every state
 * \{
 */

static const struct Transition init_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
};

static const struct Transition idle_transitions[] = {
    { .to = FATAL, .function = NULL },
    { .to = FLASHING, .function = enter_flashing_state },
    { .to = BALANCING, .function = enter_balancing_state },
    { .to = WAIT_ECU_OK, .function = NULL },
};

static const struct Transition fatal_transitions[] = {
    { .to = FLASHING, .function = enter_flashing_state },
};

static const struct Transition flashing_transitions[] = {
    { .to = IDLE, .function = exit_flashing_state },
};

static const struct Transition balancing_transitions[] = {
    { .to = IDLE, .function = exit_balancing_state },
    { .to = FATAL, .function = exit_balancing_state },
};

static const struct Transition wait_ecu_ok_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
    { .to = AIRN_CHECK, .function = NULL },
};

static const struct Transition airn_check_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
    { .to = PRECHARGE_CHECK, .function = NULL },
};

static const struct Transition precharge_check_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
    { .to = AIRP_CHECK, .function = NULL },
};

static const struct Transition airp_check_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
    { .to = TS_ON, .function = NULL },
};

static const struct Transition ts_on_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = NULL },
};

/*! \} */

/*!
 * \brief State table of the mainboard FSM
 *
 * \details Every state repeats itself until a transition is requested,
 *          except INIT which runs once and then moves to IDLE
 */
static const struct State mainboard_fsm_states[STATE_COUNT] = {
    [INIT] = {
        .id = INIT,
        .function = routine_init,
        .next_default = IDLE,
        .transitions = init_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(init_transitions),
    },
    [IDLE] = {
        .id = IDLE,
        .function = routine_idle,
        .next_default = IDLE,
        .transitions = idle_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(idle_transitions),
    },
    [FATAL] = {
        .id = FATAL,
        .function = routine_fatal,
        .next_default = FATAL,
        .transitions = fatal_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fatal_transitions),
    },
    [FLASHING] = {
        .id = FLASHING,
        .function = routine_flashing,
        .next_default = FLASHING,
        .transitions = flashing_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(flashing_transitions),
    },
    [BALANCING] = {
        .id = BALANCING,
        .function = routine_balancing,
        .next_default = BALANCING,
        .transitions = balancing_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(balancing_transitions),
    },
    [WAIT_ECU_OK] = {
        .id = WAIT_ECU_OK,
        .function = routine_wait_ecu_ok,
        .next_default = WAIT_ECU_OK,
        .transitions = wait_ecu_ok_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(wait_ecu_ok_transitions),
    },
    [AIRN_CHECK] = {
        .id = AIRN_CHECK,
        .function = routine_airn_check,
        .next_default = AIRN_CHECK,
        .transitions = airn_check_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(airn_check_transitions),
    },
    [PRECHARGE_CHECK] = {
        .id = PRECHARGE_CHECK,
        .function = routine_precharge_check,
        .next_default = PRECHARGE_CHECK,
        .transitions = precharge_check_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(precharge_check_transitions),
    },
    [AIRP_CHECK] = {
        .id = AIRP_CHECK,
        .function = routine_airp_check,
        .next_default = AIRP_CHECK,
        .transitions = airp_check_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(airp_check_transitions),
    },
    [TS_ON] = {
        .id = TS_ON,
        .function = routine_ts_on,
        .next_default = TS_ON,
        .transitions = ts_on_transitions,
        .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(ts_on_transitions),
    },
};

enum FSMReturnCode mainboard_fsm_api_init(void) {
    return fsm_api_init(&hfsm, mainboard_fsm_states, STATE_COUNT, INIT);
}

enum FSMReturnCode mainboard_fsm_api_run(void) {
    return fsm_api_run_state(&hfsm, NULL);
}
