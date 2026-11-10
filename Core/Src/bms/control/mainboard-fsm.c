/*!
 * \file mainboard-fsm.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main mainboard FSM that handles all the operations of the BMS
 */

#include "mainboard-fsm.h"

#include "eagletrt-api.h"
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
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - INIT -> IDLE: Initialization successful
     * - INIT -> FATAL: Initialization failed
     */
}

/*!
 * \brief Idle routine for the mainboard
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_idle(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - IDLE -> FATAL: A fault was detected
     * - IDLE -> FLASHING: A flashing message was recieved
     * - IDLE -> BALANCING: Balancing is required and the mainboard is in a safe state
     * - IDLE -> WAIT_ECU_OK: The DO_TSON flag was set and the mainboard is in a safe state
     */
}

/*!
 * \brief Fatal error routine for the mainboard
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_fatal(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - FATAL -> FLASHING: A flashing message was received
     *
     * Faults are never cleared, so FATAL can't go back to IDLE
     */
}

/*!
 * \brief Transition function from: 
 * Idle -> Flashing
 * Fatal -> Flashing
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void enter_flashing_state(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*!
 * \brief Routine for the flashing state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_flashing(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - FLASHING -> FATAL: A fault was detected
     * - FLASHING -> IDLE: The flashing procedure ended or timed out
     */
}

/*!
 * \brief Transition function from:
 * Flashing -> Idle
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void exit_flashing_state(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*!
 * \brief Transition function from:
 * Idle -> Balancing
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void enter_balancing_state(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*!
 * \brief Routine for the balancing state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_balancing(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - BALANCING -> FATAL: A fault was detected
     * - BALANCING -> IDLE: A balancing stop was requested or the cells are balanced
     */
}

/*!
 * \brief Transition function from:
 * Balancing -> Idle
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void exit_balancing_state(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*!
 * \brief Intermediate routine waiting for the ECU to acknowledge the BMS
 * 
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_wait_ecu_ok(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - WAIT_ECU_OK -> FATAL: A fault was detected
     * - WAIT_ECU_OK -> IDLE: A TS-OFF was requested, the ECU didn't answer in time or the SD was opened
     * - WAIT_ECU_OK -> AIRN_CHECK: The ECU acknowledged the TS-ON request
     */
}

/*!
 * \brief Close the AIR- 
 * WAIT_ECU_OK -> AIRN_CHECK
 * 
 * \param data A pointer to the data that will be passed to the state routines
 */
static void close_airn(void *data) {
    EAGLETRT_API_UNUSED(data);
    /* Implementation for closing the AIR- */
}

/*!
 * \brief Routine for the AIR- checks
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_airn_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - AIRN_CHECK -> FATAL: A fault was detected
     * - AIRN_CHECK -> IDLE: A TS-OFF was requested, the AIR- didn't close in time or the SD was opened
     * - AIRN_CHECK -> PRECHARGE_CHECK: The AIR- is closed and every feedback has the expected value
     */
}

/*!
 * \brief Start the precharge
 * AIRN_CHECK -> PRECHARGE_CHECK
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void start_precharge(void *data) {
    EAGLETRT_API_UNUSED(data);
    /* Implementation for starting the precharge */
}

/*!
 * \brief Routine for the precharge
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_precharge_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - PRECHARGE_CHECK -> FATAL: A fault was detected
     * - PRECHARGE_CHECK -> IDLE: A TS-OFF was requested, the precharge didn't complete in time or the SD was opened
     * - PRECHARGE_CHECK -> AIRP_CHECK: The precharge is complete and every feedback has the expected value
     */
}

/*!
 * \brief Close the AIR+
 * precharge_check -> AIRP_CHECK
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void close_airp(void *data) {
    EAGLETRT_API_UNUSED(data);
    /* Implementation for closing the AIR+ */
}

/*!
 * \brief Routine for the AIR+ checks
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_airp_check(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - AIRP_CHECK -> FATAL: A fault was detected
     * - AIRP_CHECK -> IDLE: A TS-OFF was requested, the AIR+ didn't close in time or the SD was opened
     * - AIRP_CHECK -> TS_ON: The AIR+ is closed and every feedback has the expected value
     */
}

/*!
 * \brief Routine for the TS_ON state
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void routine_ts_on(void *data) {
    EAGLETRT_API_UNUSED(data);

    /* Transition logic:
     * - TS_ON -> FATAL: A fault was detected
     * - TS_ON -> IDLE: A TS-OFF was requested, the ECU timed out or a feedback lost its expected value (e.g. SD opened)
     */
}

/*!
 * \brief Return the pack into a safe state
 * AIRN_CHECK -> IDLE
 * PRECHARGE_CHECK -> IDLE
 * AIRP_CHECK -> IDLE
 * TS_ON -> IDLE
 * 
 * \param data A pointer to the data that will be passed to the state routines
 */
static void do_ts_off(void *data) {
    EAGLETRT_API_UNUSED(data);
    /* Implementation for returning the pack to a safe state */
}

/*!
 * \brief Activate the AMS, stop the precharge, open the AIRs and stop balancing
 * INIT -> FATAL
 * IDLE -> FATAL
 * FLASHING -> FATAL
 * BALANCING -> FATAL
 * WAIT_ECU_OK -> FATAL
 * AIRN_CHECK -> FATAL
 * PRECHARGE_CHECK -> FATAL
 * AIRP_CHECK -> FATAL
 * TS_ON -> FATAL
 *
 * \param data A pointer to the data that will be passed to the state routines
 */
static void enter_fatal_state(void *data) {
    EAGLETRT_API_UNUSED(data);
    /* Implementation for putting the pack in a safe state after a fault */
}

/*!
 * \defgroup transitions Transition tables
 * \brief Allowed transitions out of every state
 * \{
 */

static const struct Transition init_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = enter_fatal_state },
};

static const struct Transition idle_transitions[] = {
    { .to = FATAL, .function = enter_fatal_state },
    { .to = FLASHING, .function = enter_flashing_state },
    { .to = BALANCING, .function = enter_balancing_state },
    { .to = WAIT_ECU_OK, .function = NULL },
};

static const struct Transition fatal_transitions[] = {
    { .to = FLASHING, .function = enter_flashing_state },
};

static const struct Transition flashing_transitions[] = {
    { .to = IDLE, .function = exit_flashing_state },
    { .to = FATAL, .function = enter_fatal_state },
};

static const struct Transition balancing_transitions[] = {
    { .to = IDLE, .function = exit_balancing_state },
    { .to = FATAL, .function = enter_fatal_state },
};

static const struct Transition wait_ecu_ok_transitions[] = {
    { .to = IDLE, .function = NULL },
    { .to = FATAL, .function = enter_fatal_state },
    { .to = AIRN_CHECK, .function = close_airn },
};

static const struct Transition airn_check_transitions[] = {
    { .to = IDLE, .function = do_ts_off },
    { .to = FATAL, .function = enter_fatal_state },
    { .to = PRECHARGE_CHECK, .function = start_precharge },
};

static const struct Transition precharge_check_transitions[] = {
    { .to = IDLE, .function = do_ts_off },
    { .to = FATAL, .function = enter_fatal_state },
    { .to = AIRP_CHECK, .function = close_airp },
};

static const struct Transition airp_check_transitions[] = {
    { .to = IDLE, .function = do_ts_off },
    { .to = FATAL, .function = enter_fatal_state },
    { .to = TS_ON, .function = NULL },
};

static const struct Transition ts_on_transitions[] = {
    { .to = IDLE, .function = do_ts_off },
    { .to = FATAL, .function = enter_fatal_state },
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
