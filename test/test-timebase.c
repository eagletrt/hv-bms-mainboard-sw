#include "unity.h"
#include "timebase.h"
#include "tasks.h"
#include "watchdog.h"

#include <string.h>


extern _TaskHandler htasks;

extern _TimebaseHandler htimebase;

int mock_watchdog_calls = 0;

int mock_tasks_calls[3] = {0};

void mock_watchdog_callback(void){
    mock_watchdog_calls++;
}

void mock_task_1_callback(void){
    mock_tasks_calls[0]++;
}

void mock_task_2_callback(void){
    mock_tasks_calls[1]++;
}

void mock_taks_3_callback(void){
    mock_tasks_calls[2]++;
}

void test_timebase_init_NULL(void);

void test_timebase_init_OK(void);

void test_timebase_inc_tick(void);

void test_timebase_register_watchdog(void);

void test_timebase_unregister_watchdog(void);

void test_timebase_watchdog(void);

void test_timebase_tasks(void){
    // Disable all tasks
    for (size_t i = 0; i < TASKS_ID_COUNT; ++i)
        TEST_ASSERT_EQUAL_MESSAGE(TASKS_OK, tasks_set_enable((TasksId)i, false), "Unable to disable default tasks");

    // Get pointers to 3 tasks and set custom callbacks and intervals
    Task * task_1 = tasks_get_task(TASKS_ID_SEND_MAINBOARD_VERSION);
    Task * task_2 = tasks_get_task(TASKS_ID_SEND_ERRORS);
    Task * task_3 = tasks_get_task(TASKS_ID_SEND_CELLBOARD_1_VERSION);

    TEST_ASSERT_NOT_NULL_MESSAGE(task_1, "Task 1 pointer should not be NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(task_2, "Task 2 pointer should not be NULL");
    TEST_ASSERT_NOT_NULL_MESSAGE(task_3, "Task 3 pointer should not be NULL");

    task_1->enabled = true;
    task_1->interval = 3U;
    task_1->exec = mock_task_1_callback;

    task_2->enabled = false;
    task_2->interval = 0U;
    task_2->exec = mock_task_2_callback;

    task_3->enabled = false;
    task_3->interval = 5U;
    task_3->exec = mock_taks_3_callback;

    (void)timebase_set_enable(true);

    // Tick 0: task 1 is enabled and should run immediately. Task 2 is disabled one-shot.
    TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_routine(), "Routine should run at tick 0");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[0], "Task 1 should run at tick 0");
    TEST_ASSERT_EQUAL_MESSAGE(0, mock_tasks_calls[1], "Task 2 should not run while disabled");
    TEST_ASSERT_EQUAL_MESSAGE(0, mock_tasks_calls[2], "Task 3 should not run before start tick");

    // Enabling task 2 now should make it run once.
    TEST_ASSERT_EQUAL_MESSAGE(TASKS_OK, tasks_set_enable(TASKS_ID_SEND_ERRORS, true), "Unable to enable task 2");

    // Tick 1..3: task 1 should run once more at tick 3. Task 2 must run once and then disable itself.
    for (int i = 0; i < 3; ++i) {
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_inc_tick(), "Failed to increment tick");
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_routine(), "Routine should run after tick increment");
    }
    TEST_ASSERT_EQUAL_MESSAGE(2, mock_tasks_calls[0], "Task 1 should run at ticks 0 and 3");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[1], "Task 2 should run once enabled");
    TEST_ASSERT_EQUAL_MESSAGE(0, mock_tasks_calls[2], "Task 3 is still disabled");

    // Task 3 first slot was at tick 1 while disabled; after enabling it, it should run at next period (tick 6).
    TEST_ASSERT_EQUAL_MESSAGE(TASKS_OK, tasks_set_enable(TASKS_ID_SEND_CELLBOARD_1_VERSION, true), "Unable to enable task 3");

    // Tick 4..6: task 1 runs at 6, task 3 runs at 6.
    for (int i = 0; i < 3; ++i) {
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_inc_tick(), "Failed to increment tick");
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_routine(), "Routine should run after tick increment");
    }
    TEST_ASSERT_EQUAL_MESSAGE(3, mock_tasks_calls[0], "Task 1 should run at ticks 0, 3 and 6");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[1], "Task 2 should never run again after first execution");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[2], "Task 3 should run once at tick 6");

    // Disable task 1 and ensure it does not run anymore.
    TEST_ASSERT_EQUAL_MESSAGE(TASKS_OK, tasks_set_enable(TASKS_ID_SEND_MAINBOARD_VERSION, false), "Unable to disable task 1");
    for (int i = 0; i < 4; ++i) {
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_inc_tick(), "Failed to increment tick");
        TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_routine(), "Routine should run after tick increment");
    }
    TEST_ASSERT_EQUAL_MESSAGE(3, mock_tasks_calls[0], "Task 1 should not run while disabled");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[1], "Task 2 should remain never executed");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[2], "Task 3 should not run again before tick 11");
    
    // Enable task 2 again to check that it can be re-enabled after execution.
    TEST_ASSERT_EQUAL_MESSAGE(TASKS_OK, tasks_set_enable(TASKS_ID_SEND_ERRORS, true), "Unable to re-enable task 2");
    TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_inc_tick(), "Failed to increment tick");
    TEST_ASSERT_EQUAL_MESSAGE(TIMEBASE_OK, timebase_routine(), "Routine should run after tick increment");
    TEST_ASSERT_EQUAL_MESSAGE(3, mock_tasks_calls[0], "Task 1 should remain disabled");
    TEST_ASSERT_EQUAL_MESSAGE(2, mock_tasks_calls[1], "Task 2 should run again after being re-enabled");
    TEST_ASSERT_EQUAL_MESSAGE(1, mock_tasks_calls[2], "Task 3 should still not run before tick 11");
}

#ifdef TIMEBASE_TESTS


void setUp(void) {
    mock_watchdog_calls=0;
    mock_tasks_calls[0]=0;
    mock_tasks_calls[1]=0;
    mock_tasks_calls[2]=0;
    (void)timebase_init(2U);
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timebase_tasks);

    return UNITY_END();
}

#endif // TIMEBASE_TESTS