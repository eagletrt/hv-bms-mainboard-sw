/*!
 * \file test-watchdogs.c
 * \date 2026-10-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tests for the mainboard watchdogs module
 *
 * The scheduling logic is part of libtimebase and it is tested there, here only
 * the mainboard implementation is tested: every function must use the one
 * watchdog pool owned by the module and must forward the library result as is
 */

#include "unity.h"
#include "mainboard-watchdogs-api.h"
#include "watchdogs-api.h"
#include "mainboard-conf.h"

#include <stdint.h>
#include <string.h>

// Number of watchdogs that are used at the same time by the mainboard modules:
// current sensor, balancing, programmer, AIR-, precharge, AIR+ and ECU
#define MAINBOARD_WATCHDOGS_IN_USE (7U)
#define TEST_TIMEOUT_TICKS (100U)

extern struct WatchdogHandler watchdogs_handler;

static uint32_t fake_tick;
static uint32_t callback_a_count;
static uint32_t callback_b_count;
static uint32_t callback_order[4];
static uint32_t callback_order_count;

static uint32_t prv_get_tick(void) {
    return fake_tick;
}

static void prv_callback_a(void) {
    ++callback_a_count;
    if (callback_order_count < 4U) {
        callback_order[callback_order_count++] = 'a';
    }
}

static void prv_callback_b(void) {
    ++callback_b_count;
    if (callback_order_count < 4U) {
        callback_order[callback_order_count++] = 'b';
    }
}

/*! \brief Initialize a watchdog and fail the test if it is not possible */
static void prv_init_watchdog(struct Watchdog *const watchdog, const uint32_t timeout, const watchdog_timeout_callback callback) {
    memset(watchdog, 0, sizeof(*watchdog));
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_init_watchdog(watchdog, timeout, callback), "Watchdog initialization failed");
}

/*!
 * \defgroup            pool Tests for the watchdogs pool initialization and routine
 * @{
 */

void test_watchdogs_use_before_init_returns_null_pointer(void) {
    memset(&watchdogs_handler, 0, sizeof(watchdogs_handler));
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_routine(), "The routine needs the pool to be initialized");
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_start(&watchdog), "A watchdog cannot be started if the pool is not initialized");
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_pet(&watchdog), "A watchdog cannot be petted if the pool is not initialized");
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state, "The watchdog must not change state");
}

void test_watchdogs_init_null_tick_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_init(NULL), "Expected NULL_POINTER with a NULL tick callback");
}

void test_watchdogs_init_stores_tick_callback(void) {
    fake_tick = 42U;

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_init(prv_get_tick), "Expected OK");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(prv_get_tick, watchdogs_handler.get_tick, "The tick callback should be stored in the pool");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(42U, watchdogs_handler.prev_tick, "The tick at the initialization should be the starting point");
}

void test_watchdogs_init_clears_scheduled_watchdogs(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    // Initialize again the pool, the previous watchdog is no longer scheduled
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_init(prv_get_tick));
    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine(), "Expected OK");

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "A watchdog of the previous pool should not time out");
}

void test_watchdogs_routine_without_watchdogs(void) {
    fake_tick = 1000U;
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine(), "Expected OK with no watchdogs");
}

void test_watchdogs_routine_forwards_temporal_discontinuity(void) {
    fake_tick = 10U;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());

    fake_tick = 5U;
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_TEMPORAL_DISCONTINUITY, mainboard_watchdogs_api_routine(), "Time cannot go backwards");
}

/** @} */

/*!
 * \defgroup            init_watchdog Tests for the single watchdog initialization
 * @{
 */

void test_watchdogs_init_watchdog_null_pointers(void) {
    struct Watchdog watchdog = { 0 };

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_init_watchdog(NULL, TEST_TIMEOUT_TICKS, prv_callback_a), "Expected NULL_POINTER with a NULL watchdog");
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, NULL), "Expected NULL_POINTER with a NULL callback");
}

void test_watchdogs_init_watchdog_zero_timeout(void) {
    struct Watchdog watchdog = { 0 };

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_ERROR, mainboard_watchdogs_api_init_watchdog(&watchdog, 0U, prv_callback_a), "A watchdog with no timeout would expire immediately");
    TEST_ASSERT_FALSE_MESSAGE(watchdog.is_initialized, "A failed initialization should leave the watchdog uninitialized");
}

void test_watchdogs_init_watchdog_ok(void) {
    struct Watchdog watchdog = { 0 };

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a), "Expected OK");
    TEST_ASSERT_TRUE(watchdog.is_initialized);
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);
    TEST_ASSERT_EQUAL_UINT32(TEST_TIMEOUT_TICKS, watchdog.timeout);
    TEST_ASSERT_EQUAL_PTR(prv_callback_a, watchdog.watchdog_callback);
}

void test_watchdogs_init_watchdog_twice_is_an_error(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);

    // pcu_api_reset_all() and similar functions initialize the same watchdogs more than once
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_ERROR, mainboard_watchdogs_api_init_watchdog(&watchdog, 5U, prv_callback_b), "A watchdog cannot be initialized twice");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(TEST_TIMEOUT_TICKS, watchdog.timeout, "The first configuration should be kept");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(prv_callback_a, watchdog.watchdog_callback, "The first configuration should be kept");
}

/** @} */

/*!
 * \defgroup            start Tests for the start function
 * @{
 */

void test_watchdogs_start_null_pointer(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_start(NULL));
}

void test_watchdogs_start_uninitialized(void) {
    struct Watchdog watchdog = { 0 };
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_UNINITIALIZED, mainboard_watchdogs_api_start(&watchdog), "An uninitialized watchdog cannot be started");
}

void test_watchdogs_start_sets_deadline_from_current_tick(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    fake_tick = 30U;

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog), "Expected OK");
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, watchdog.watchdog_state);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(30U + TEST_TIMEOUT_TICKS, watchdog.next_trigger, "The deadline should be the current tick plus the timeout");
}

void test_watchdogs_start_running_is_busy(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    const uint32_t deadline = watchdog.next_trigger;

    fake_tick = 50U;
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_BUSY, mainboard_watchdogs_api_start(&watchdog), "Expected BUSY for a running watchdog");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(deadline, watchdog.next_trigger, "The deadline of a running watchdog must not change");
}

/** @} */

/*!
 * \defgroup            timeout Tests for the timeout of the watchdogs
 * @{
 */

void test_watchdogs_routine_executes_callback_once_at_timeout(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    fake_tick = TEST_TIMEOUT_TICKS - 1U;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "The callback must not be executed before the timeout");
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, watchdog.watchdog_state);

    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, callback_a_count, "The callback must be executed when the timeout elapses");
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_TIMED_OUT, watchdog.watchdog_state);
    TEST_ASSERT_TRUE(watchdogs_api_watchdog_is_timed_out(&watchdog));
    TEST_ASSERT_FALSE(watchdogs_api_watchdog_is_running(&watchdog));

    fake_tick = 10U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, callback_a_count, "The callback must be executed only once");
}

void test_watchdogs_timed_out_watchdog_rejects_start_stop_and_pet(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());

    // The current sensor and the ECU watchdogs rely on the TIMED_OUT code to know that they have to be restarted
    TEST_ASSERT_EQUAL(WATCHDOG_RC_TIMED_OUT, mainboard_watchdogs_api_start(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_TIMED_OUT, mainboard_watchdogs_api_stop(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_TIMED_OUT, mainboard_watchdogs_api_pet(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_TIMED_OUT, watchdog.watchdog_state);
}

void test_watchdogs_callbacks_are_executed_in_deadline_order(void) {
    struct Watchdog slow, fast;
    prv_init_watchdog(&slow, 3U * TEST_TIMEOUT_TICKS, prv_callback_a);
    prv_init_watchdog(&fast, TEST_TIMEOUT_TICKS, prv_callback_b);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&slow));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&fast));

    fake_tick = 5U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());

    TEST_ASSERT_EQUAL_UINT32(2U, callback_order_count);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE('b', callback_order[0], "The earliest watchdog should time out first");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE('a', callback_order[1], "The latest watchdog should time out last");
}

void test_watchdogs_only_expired_watchdogs_time_out(void) {
    struct Watchdog slow, fast;
    prv_init_watchdog(&slow, 3U * TEST_TIMEOUT_TICKS, prv_callback_a);
    prv_init_watchdog(&fast, TEST_TIMEOUT_TICKS, prv_callback_b);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&slow));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&fast));

    fake_tick = 2U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());

    TEST_ASSERT_EQUAL_UINT32(0U, callback_a_count);
    TEST_ASSERT_EQUAL_UINT32(1U, callback_b_count);
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, slow.watchdog_state);
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_TIMED_OUT, fast.watchdog_state);
}

/** @} */

/*!
 * \defgroup            stop Tests for the stop function
 * @{
 */

void test_watchdogs_stop_null_pointer(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_stop(NULL));
}

void test_watchdogs_stop_not_running(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_NOT_RUNNING, mainboard_watchdogs_api_stop(&watchdog));
}

void test_watchdogs_stop_prevents_timeout(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_OK, mainboard_watchdogs_api_stop(&watchdog), "Expected OK");
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);

    fake_tick = 10U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "A stopped watchdog must not time out");
}

void test_watchdogs_stopped_watchdog_can_be_started_again(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_stop(&watchdog));

    fake_tick = 50U;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(50U + TEST_TIMEOUT_TICKS, watchdog.next_trigger, "The deadline should restart from the new start");

    fake_tick = 50U + TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32(1U, callback_a_count);
}

/** @} */

/*!
 * \defgroup            restart Tests for the restart function
 * @{
 */

void test_watchdogs_restart_null_pointer(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_restart(NULL));
}

void test_watchdogs_restart_uninitialized(void) {
    struct Watchdog watchdog = { 0 };
    TEST_ASSERT_EQUAL(WATCHDOG_RC_UNINITIALIZED, mainboard_watchdogs_api_restart(&watchdog));
}

void test_watchdogs_restart_starts_not_running_watchdog(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    fake_tick = 20U;

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_restart(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, watchdog.watchdog_state);
    TEST_ASSERT_EQUAL_UINT32(20U + TEST_TIMEOUT_TICKS, watchdog.next_trigger);
}

void test_watchdogs_restart_moves_deadline_of_running_watchdog(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    fake_tick = 60U;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_restart(&watchdog));
    TEST_ASSERT_EQUAL_UINT32(60U + TEST_TIMEOUT_TICKS, watchdog.next_trigger);

    // The old deadline is gone, the callback should be executed only once and at the new deadline
    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "The previous deadline should not be used anymore");

    fake_tick = 60U + TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32(1U, callback_a_count);
}

void test_watchdogs_restart_revives_timed_out_watchdog(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_TIMED_OUT, watchdog.watchdog_state);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_restart(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, watchdog.watchdog_state);

    fake_tick = 2U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2U, callback_a_count, "A restarted watchdog should be able to time out again");
}

/** @} */

/*!
 * \defgroup            reset Tests for the reset function
 * @{
 */

void test_watchdogs_reset_null_pointer(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_reset(NULL));
}

void test_watchdogs_reset_uninitialized(void) {
    struct Watchdog watchdog = { 0 };
    TEST_ASSERT_EQUAL(WATCHDOG_RC_UNINITIALIZED, mainboard_watchdogs_api_reset(&watchdog));
}

void test_watchdogs_reset_not_running_is_not_an_error(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_reset(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);
}

void test_watchdogs_reset_stops_running_watchdog_without_callback(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_reset(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);

    fake_tick = 10U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "A reset watchdog must not execute its callback");
}

void test_watchdogs_reset_makes_timed_out_watchdog_startable(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_TIMED_OUT, mainboard_watchdogs_api_start(&watchdog));

    // This is how pcu_api_reset_all() re-arms the AIR and precharge watchdogs
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_reset(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));
}

/** @} */

/*!
 * \defgroup            pet Tests for the pet function
 * @{
 */

void test_watchdogs_pet_null_pointer(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, mainboard_watchdogs_api_pet(NULL));
}

void test_watchdogs_pet_uninitialized(void) {
    struct Watchdog watchdog = { 0 };
    TEST_ASSERT_EQUAL(WATCHDOG_RC_UNINITIALIZED, mainboard_watchdogs_api_pet(&watchdog));
}

void test_watchdogs_pet_not_running(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);

    // A petted watchdog is not started, the sensors are petted also before the POST starts them
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NOT_RUNNING, mainboard_watchdogs_api_pet(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_NOT_RUNNING, watchdog.watchdog_state);
}

void test_watchdogs_pet_replenishes_running_watchdog(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    fake_tick = TEST_TIMEOUT_TICKS - 1U;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_pet(&watchdog));
    TEST_ASSERT_EQUAL(WATCHDOG_STATE_RUNNING, watchdog.watchdog_state);
    TEST_ASSERT_EQUAL_UINT32(TEST_TIMEOUT_TICKS - 1U + TEST_TIMEOUT_TICKS, watchdog.next_trigger);

    fake_tick = TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, callback_a_count, "A petted watchdog must not time out at the old deadline");

    fake_tick = TEST_TIMEOUT_TICKS - 1U + TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, callback_a_count, "A petted watchdog must time out at the new deadline");
}

void test_watchdogs_regular_pet_keeps_watchdog_alive(void) {
    struct Watchdog watchdog;
    prv_init_watchdog(&watchdog, TEST_TIMEOUT_TICKS, prv_callback_a);
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdog));

    // Pet at 80% of the timeout, as a sensor that sends data regularly does
    for (uint32_t i = 0U; i < 50U; ++i) {
        fake_tick += (TEST_TIMEOUT_TICKS * 8U) / 10U;
        TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_pet(&watchdog));
        TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    }
    TEST_ASSERT_EQUAL_UINT32(0U, callback_a_count);

    // The sensor goes silent
    fake_tick += TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32(1U, callback_a_count);
}

/** @} */

/*!
 * \defgroup            capacity Tests for the capacity of the pool
 * @{
 */

void test_watchdogs_pool_fits_all_the_mainboard_watchdogs(void) {
    struct Watchdog watchdogs[MAINBOARD_WATCHDOGS_IN_USE];

    TEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(MAINBOARD_WATCHDOGS_IN_USE, MAX_WATCHDOGS, "MAX_WATCHDOGS is lower than the number of watchdogs used by the mainboard");
    for (uint32_t i = 0U; i < MAINBOARD_WATCHDOGS_IN_USE; ++i) {
        prv_init_watchdog(&watchdogs[i], TEST_TIMEOUT_TICKS + i, prv_callback_a);
        TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdogs[i]));
    }

    fake_tick = 2U * TEST_TIMEOUT_TICKS;
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_routine());
    TEST_ASSERT_EQUAL_UINT32(MAINBOARD_WATCHDOGS_IN_USE, callback_a_count);
}

void test_watchdogs_pool_full_leaves_watchdog_not_running(void) {
    struct Watchdog watchdogs[MAX_WATCHDOGS + 1U];

    for (uint32_t i = 0U; i < MAX_WATCHDOGS; ++i) {
        prv_init_watchdog(&watchdogs[i], TEST_TIMEOUT_TICKS, prv_callback_a);
        TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_start(&watchdogs[i]));
    }
    prv_init_watchdog(&watchdogs[MAX_WATCHDOGS], TEST_TIMEOUT_TICKS, prv_callback_b);

    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_RC_ERROR, mainboard_watchdogs_api_start(&watchdogs[MAX_WATCHDOGS]), "No more than MAX_WATCHDOGS watchdogs can run at the same time");
    TEST_ASSERT_EQUAL_MESSAGE(WATCHDOG_STATE_NOT_RUNNING, watchdogs[MAX_WATCHDOGS].watchdog_state, "A watchdog that could not be started must not appear as running");
}

/** @} */

void setUp(void) {
    fake_tick = 0U;
    callback_a_count = 0U;
    callback_b_count = 0U;
    callback_order_count = 0U;
    memset(callback_order, 0, sizeof(callback_order));

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, mainboard_watchdogs_api_init(prv_get_tick));
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_watchdogs_use_before_init_returns_null_pointer);
    RUN_TEST(test_watchdogs_init_null_tick_callback);
    RUN_TEST(test_watchdogs_init_stores_tick_callback);
    RUN_TEST(test_watchdogs_init_clears_scheduled_watchdogs);
    RUN_TEST(test_watchdogs_routine_without_watchdogs);
    RUN_TEST(test_watchdogs_routine_forwards_temporal_discontinuity);

    RUN_TEST(test_watchdogs_init_watchdog_null_pointers);
    RUN_TEST(test_watchdogs_init_watchdog_zero_timeout);
    RUN_TEST(test_watchdogs_init_watchdog_ok);
    RUN_TEST(test_watchdogs_init_watchdog_twice_is_an_error);

    RUN_TEST(test_watchdogs_start_null_pointer);
    RUN_TEST(test_watchdogs_start_uninitialized);
    RUN_TEST(test_watchdogs_start_sets_deadline_from_current_tick);
    RUN_TEST(test_watchdogs_start_running_is_busy);

    RUN_TEST(test_watchdogs_routine_executes_callback_once_at_timeout);
    RUN_TEST(test_watchdogs_timed_out_watchdog_rejects_start_stop_and_pet);
    RUN_TEST(test_watchdogs_callbacks_are_executed_in_deadline_order);
    RUN_TEST(test_watchdogs_only_expired_watchdogs_time_out);

    RUN_TEST(test_watchdogs_stop_null_pointer);
    RUN_TEST(test_watchdogs_stop_not_running);
    RUN_TEST(test_watchdogs_stop_prevents_timeout);
    RUN_TEST(test_watchdogs_stopped_watchdog_can_be_started_again);

    RUN_TEST(test_watchdogs_restart_null_pointer);
    RUN_TEST(test_watchdogs_restart_uninitialized);
    RUN_TEST(test_watchdogs_restart_starts_not_running_watchdog);
    RUN_TEST(test_watchdogs_restart_moves_deadline_of_running_watchdog);
    RUN_TEST(test_watchdogs_restart_revives_timed_out_watchdog);

    RUN_TEST(test_watchdogs_reset_null_pointer);
    RUN_TEST(test_watchdogs_reset_uninitialized);
    RUN_TEST(test_watchdogs_reset_not_running_is_not_an_error);
    RUN_TEST(test_watchdogs_reset_stops_running_watchdog_without_callback);
    RUN_TEST(test_watchdogs_reset_makes_timed_out_watchdog_startable);

    RUN_TEST(test_watchdogs_pet_null_pointer);
    RUN_TEST(test_watchdogs_pet_uninitialized);
    RUN_TEST(test_watchdogs_pet_not_running);
    RUN_TEST(test_watchdogs_pet_replenishes_running_watchdog);
    RUN_TEST(test_watchdogs_regular_pet_keeps_watchdog_alive);

    RUN_TEST(test_watchdogs_pool_fits_all_the_mainboard_watchdogs);
    RUN_TEST(test_watchdogs_pool_full_leaves_watchdog_not_running);

    return UNITY_END();
}
