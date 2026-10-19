/*!
 * \file test-tasks.c
 * \date 2026-10-19
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tests for the mainboard tasks module
 *
 * The scheduling logic is part of libtimebase and it is tested there, here only
 * the mainboard implementation is tested: the table of the tasks, the way the
 * tasks are initialized and the primary CAN messages sending
 */

#include "unity.h"
#include "mainboard-tasks-api.h"
#include "tasks-api.h"
#include "can-communication-api.h"
#include "can-primary-api.h"
#include "can-bms-api.h"

#include <stdint.h>
#include <string.h>

// Delay between the start of two consecutive tasks
#define TASKS_START_STAGGER_MS (3U)

extern struct TasksHandler tasks_handler;
extern struct MainboardTaskHandler mainboard_tasks_handler;
extern TaskList task_list;
extern const struct TaskCanPrimaryMsg can_msgs[];

extern void prv_tasks_send_to_primary(uint8_t task_id);
extern void prv_tasks_send_cellboard_set_balancing_status(uint8_t task_id);

// The number of elements of can_msgs is the one of the last CAN task
#define CAN_MSGS_COUNT (TASK_SEND_CELLBOARD_ERRORS_C + 1U)

static uint32_t fake_tick;
static uint32_t task_calls[TASKS_COUNT];
static struct Task pristine_task_list[MAX_TASKS];

static uint32_t sent_count[CAN_COMMUNICATION_NETWORK_COUNT];
static struct CanCommunicationFrame last_sent[CAN_COMMUNICATION_NETWORK_COUNT];

static uint32_t prv_get_tick(void) {
    return fake_tick;
}

static void prv_stub_task(uint8_t task_id) {
    if (task_id < TASKS_COUNT) {
        ++task_calls[task_id];
    }
}

static enum CanCommunicationReturnCode prv_send_primary(const struct CanCommunicationFrame *frame) {
    ++sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY];
    last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_send_bms(const struct CanCommunicationFrame *frame) {
    ++sent_count[CAN_COMMUNICATION_NETWORK_BMS];
    last_sent[CAN_COMMUNICATION_NETWORK_BMS] = *frame;
    return CAN_COMMUNICATION_RC_OK;
}

static enum CanCommunicationReturnCode prv_on_receive(struct CanCommunicationFrame *frame) {
    (void)frame;
    return CAN_COMMUNICATION_RC_OK;
}

/*! \brief Replace the function of every task with a stub that counts the calls */
static void prv_stub_all_tasks(void) {
    for (uint8_t i = 0U; i < TASKS_COUNT; ++i) {
        task_list[i].function = prv_stub_task;
    }
}

/*! \brief Send the queued CAN frames to the (fake) hardware */
static void prv_flush_can(void) {
    for (enum CanCommunicationNetwork network = 0; network < CAN_COMMUNICATION_NETWORK_COUNT; ++network) {
        TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_process_tx(network));
    }
}

/*!
 * \defgroup            table Tests for the table of the tasks
 * @{
 */

void test_tasks_count_fits_in_the_scheduler(void) {
    TEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(MAX_TASKS, TASKS_COUNT, "There are more tasks than the scheduler can handle");
}

void test_tasks_ids_are_sequential_and_start_from_zero(void) {
    for (uint32_t i = 0U; i < TASKS_COUNT; ++i) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(i, pristine_task_list[i].task_id, "The task id must be equal to its position in the table");
    }
}

void test_tasks_every_task_has_a_function(void) {
    for (uint32_t i = 0U; i < TASKS_COUNT; ++i) {
        TEST_ASSERT_NOT_NULL_MESSAGE(pristine_task_list[i].function, "A task without a function cannot be scheduled");
    }
}

void test_tasks_intervals_are_valid(void) {
    for (uint32_t i = 0U; i < TASKS_COUNT; ++i) {
        TEST_ASSERT_GREATER_THAN_UINT16_MESSAGE(0U, pristine_task_list[i].interval, "A zero interval would run the task at every tick");
    }
}

void test_tasks_can_table_covers_all_the_primary_tasks(void) {
    for (uint32_t i = 0U; i < TASKS_COUNT; ++i) {
        if (pristine_task_list[i].function != prv_tasks_send_to_primary) {
            continue;
        }
        TEST_ASSERT_LESS_THAN_UINT32_MESSAGE(CAN_MSGS_COUNT, i, "A primary task is not inside the CAN messages table");
        TEST_ASSERT_NOT_NULL_MESSAGE(can_msgs[i].get_payload, "A primary task has no payload getter, it would send nothing");
    }
}

void test_tasks_can_table_has_only_primary_tasks(void) {
    for (uint32_t i = 0U; i < CAN_MSGS_COUNT; ++i) {
        if (can_msgs[i].get_payload == NULL) {
            continue;
        }
        TEST_ASSERT_TRUE_MESSAGE(pristine_task_list[i].function == prv_tasks_send_to_primary, "A task with a CAN message must use the primary sending function");
    }
}

void test_tasks_can_table_frame_ids_are_unique(void) {
    for (uint32_t i = 0U; i < CAN_MSGS_COUNT; ++i) {
        if (can_msgs[i].get_payload == NULL) {
            continue;
        }
        for (uint32_t j = i + 1U; j < CAN_MSGS_COUNT; ++j) {
            if (can_msgs[j].get_payload == NULL) {
                continue;
            }
            TEST_ASSERT_NOT_EQUAL_MESSAGE(can_msgs[i].frame_id, can_msgs[j].frame_id, "Two tasks send the same CAN message");
        }
    }
}

void test_tasks_can_table_payload_matches_the_frame(void) {
    for (uint32_t i = 0U; i < CAN_MSGS_COUNT; ++i) {
        if (can_msgs[i].get_payload == NULL) {
            continue;
        }
        size_t byte_size = 0U;
        union CanPrimaryMessages *payload = can_msgs[i].get_payload(&byte_size);
        uint8_t data[CAN_COMMUNICATION_FRAME_DATA_SIZE] = { 0 };

        TEST_ASSERT_NOT_NULL_MESSAGE(payload, "The payload getter returned NULL");
        TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(0U, byte_size, "The payload getter returned an empty payload");
        TEST_ASSERT_EQUAL_INT_MESSAGE((int)byte_size, can_primary_api_serialize_from_id(can_msgs[i].frame_id, payload, data), "The payload does not match the size of the frame, are the getter and the frame id swapped?");
    }
}

void test_tasks_payload_getters_accept_null_byte_size(void) {
    for (uint32_t i = 0U; i < CAN_MSGS_COUNT; ++i) {
        if (can_msgs[i].get_payload == NULL) {
            continue;
        }
        // The sending function does not need the size
        TEST_ASSERT_NOT_NULL_MESSAGE(can_msgs[i].get_payload(NULL), "The payload getter must accept a NULL byte size");
    }
}

/** @} */

/*!
 * \defgroup            init Tests for the initialization function
 * @{
 */

void test_tasks_init_null_tick_callback(void) {
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_ERROR, mainboard_tasks_init(NULL), "Expected ERROR with a NULL tick callback");
}

void test_tasks_init_ok(void) {
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick), "Expected OK");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(TASKS_COUNT, tasks_handler.task_num, "Every task should be registered");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(prv_get_tick, tasks_handler.get_tick, "The tick callback should be stored");
}

void test_tasks_init_enables_and_staggers_all_the_tasks(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    for (uint8_t i = 0U; i < TASKS_COUNT; ++i) {
        struct Task task;
        TEST_ASSERT_EQUAL(TASKS_RC_OK, tasks_api_get_task(&tasks_handler, i, &task));

        TEST_ASSERT_EQUAL_MESSAGE(TASKS_STATE_ENABLED, task.state, "Every task should be enabled by default");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0U, task.repeats, "Every task should repeat forever");
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(i * TASKS_START_STAGGER_MS, task.start, "The tasks should start with a delay between each other");
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(i * TASKS_START_STAGGER_MS, task.next_trigger, "The first execution should be scheduled using the start delay");
    }
}

void test_tasks_init_twice_restores_default_configuration(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_STATUS));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_suppress_primary_messages(true));

    fake_tick = 500U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    struct Task task;
    TEST_ASSERT_EQUAL(TASKS_RC_OK, tasks_api_get_task(&tasks_handler, TASK_SEND_STATUS, &task));
    TEST_ASSERT_EQUAL_MESSAGE(TASKS_STATE_ENABLED, task.state, "The initialization should enable the tasks again");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(500U, task.next_trigger, "The tasks should be scheduled from the current tick");
    TEST_ASSERT_FALSE_MESSAGE(mainboard_tasks_handler.suppress_primary_messages, "The initialization should enable the primary messages");
}

/** @} */

/*!
 * \defgroup            routine Tests for the routine function
 * @{
 */

void test_tasks_routine_before_init(void) {
    memset(&tasks_handler, 0, sizeof(tasks_handler));
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_ERROR, mainboard_tasks_routine(), "The routine cannot run if the tasks are not initialized");
}

void test_tasks_routine_executes_the_first_task_at_tick_zero(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, task_calls[0], "The first task has no delay");
    for (uint8_t i = 1U; i < TASKS_COUNT; ++i) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, task_calls[i], "The other tasks have to wait for their start delay");
    }
}

void test_tasks_routine_executes_tasks_following_the_stagger(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    fake_tick = 3U * TASKS_START_STAGGER_MS;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());

    for (uint8_t i = 0U; i < TASKS_COUNT; ++i) {
        const uint32_t expected = i <= 3U ? 1U : 0U;
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, task_calls[i], "Only the tasks with an elapsed start delay should be executed");
    }
}

void test_tasks_routine_executes_every_task_once_after_a_long_pause(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    // Long enough for every start delay and every interval to be elapsed several times
    fake_tick = 100000U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());

    for (uint8_t i = 0U; i < TASKS_COUNT; ++i) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, task_calls[i], "Overdue tasks should run once, with their own id, and not in burst");
    }
}

void test_tasks_routine_executes_tasks_with_their_interval(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    const uint16_t interval = task_list[TASK_SEND_STATUS].interval;

    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL_UINT32(1U, task_calls[TASK_SEND_STATUS]);

    fake_tick = interval - 1U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, task_calls[TASK_SEND_STATUS], "The task should not be executed before its interval");

    fake_tick = interval;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2U, task_calls[TASK_SEND_STATUS], "The task should be executed after its interval");
}

void test_tasks_routine_forwards_temporal_discontinuity(void) {
    prv_stub_all_tasks();
    fake_tick = 100U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    fake_tick = 50U;
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_ERROR, mainboard_tasks_routine(), "Time cannot go backwards");
}

/** @} */

/*!
 * \defgroup            enable_disable Tests for the enable and disable functions
 * @{
 */

void test_tasks_enable_disable_before_init(void) {
    memset(&tasks_handler, 0, sizeof(tasks_handler));

    TEST_ASSERT_NOT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_STATUS), "A task cannot be enabled if the tasks are not initialized");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_STATUS), "A task cannot be disabled if the tasks are not initialized");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_all_tasks(), "The tasks cannot be enabled if they are not initialized");
}

void test_tasks_enable_task_invalid_id(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_INVALID_ID, mainboard_tasks_enable_task(TASKS_COUNT), "TASKS_COUNT is not a valid task");
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_INVALID_ID, mainboard_tasks_enable_task(UINT8_MAX), "Expected INVALID_ID");
}

void test_tasks_disable_task_invalid_id(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_INVALID_ID, mainboard_tasks_disable_task(TASKS_COUNT), "TASKS_COUNT is not a valid task");
    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_INVALID_ID, mainboard_tasks_disable_task(UINT8_MAX), "Expected INVALID_ID");
}

void test_tasks_disable_and_enable_task(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    struct Task task;

    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_CURRENT));
    TEST_ASSERT_EQUAL(TASKS_RC_OK, tasks_api_get_task(&tasks_handler, TASK_SEND_CURRENT, &task));
    TEST_ASSERT_EQUAL(TASKS_STATE_DISABLED, task.state);

    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_CURRENT));
    TEST_ASSERT_EQUAL(TASKS_RC_OK, tasks_api_get_task(&tasks_handler, TASK_SEND_CURRENT, &task));
    TEST_ASSERT_EQUAL(TASKS_STATE_ENABLED, task.state);
}

void test_tasks_repeated_enable_and_disable_are_not_errors(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    // The balancing module enables and disables the same task every time the balancing starts or stops
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS));
}

void test_tasks_disabled_task_is_not_executed(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_CURRENT));

    fake_tick = 100000U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, task_calls[TASK_SEND_CURRENT], "A disabled task must not be executed");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, task_calls[TASK_SEND_STATUS], "The other tasks must not be affected");
}

void test_tasks_enabled_task_is_executed_again(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(TASK_SEND_CURRENT));

    fake_tick = 1000U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_task(TASK_SEND_CURRENT));

    // The task restarts from the beginning, with its start delay
    fake_tick = 1000U + (uint32_t)task_list[TASK_SEND_CURRENT].start - 1U;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL_UINT32(0U, task_calls[TASK_SEND_CURRENT]);

    fake_tick = 1000U + task_list[TASK_SEND_CURRENT].start;
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_routine());
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, task_calls[TASK_SEND_CURRENT], "The enabled task should be executed again");
}

void test_tasks_enable_all_tasks(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    for (uint8_t i = 0U; i < TASKS_COUNT; i += 2U) {
        TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_disable_task(i));
    }

    TEST_ASSERT_EQUAL_MESSAGE(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_all_tasks(), "Expected OK");

    for (uint8_t i = 0U; i < TASKS_COUNT; ++i) {
        struct Task task;
        TEST_ASSERT_EQUAL(TASKS_RC_OK, tasks_api_get_task(&tasks_handler, i, &task));
        TEST_ASSERT_EQUAL_MESSAGE(TASKS_STATE_ENABLED, task.state, "Every task should be enabled");
    }
}

void test_tasks_enable_all_tasks_when_already_enabled(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_enable_all_tasks());
}

/** @} */

/*!
 * \defgroup            can Tests for the CAN messages sending
 * @{
 */

void test_tasks_send_to_primary_enqueues_the_message(void) {
    prv_stub_all_tasks();
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    prv_tasks_send_to_primary(TASK_SEND_CURRENT);
    prv_flush_can();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "Expected one message on the primary network");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, sent_count[CAN_COMMUNICATION_NETWORK_BMS], "Nothing should be sent on the BMS network");
    TEST_ASSERT_EQUAL_UINT32(CAN_PRIMARY_MESSAGE_FRAME_ID_TSACMAINBOARDCURRENTINFO, last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY].id);
    TEST_ASSERT_EQUAL_UINT8(can_primary_byte_size_tsacmainboardcurrentinfo, last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY].length);
}

void test_tasks_send_to_primary_sends_every_message_with_its_id(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    for (uint8_t i = 0U; i < CAN_MSGS_COUNT; ++i) {
        if (can_msgs[i].get_payload == NULL) {
            continue;
        }
        sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY] = 0U;

        prv_tasks_send_to_primary(i);
        prv_flush_can();

        TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "Expected one message for each primary task");
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(can_msgs[i].frame_id, last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY].id, "The message id does not match the task");
    }
}

void test_tasks_send_to_primary_ignores_tasks_without_message(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));

    // Tasks that are after the CAN messages, outside the table, and invalid ones
    const uint8_t ids[] = { TASK_SEND_CELLBOARD_SET_BALANCING_STATUS, TASK_START_INTERNAL_VOLTAGE_CONVERSION, TASKS_COUNT, UINT8_MAX };
    for (uint32_t i = 0U; i < sizeof(ids); ++i) {
        prv_tasks_send_to_primary(ids[i]);
    }
    prv_flush_can();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "No message should be sent for these tasks");
}

void test_tasks_suppress_primary_messages_blocks_everything_but_the_status(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_suppress_primary_messages(true));

    prv_tasks_send_to_primary(TASK_SEND_CURRENT);
    prv_tasks_send_to_primary(TASK_SEND_CELLBOARD1_VOLTAGE);
    prv_tasks_send_to_primary(TASK_SEND_ERRORS);
    prv_flush_can();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "The primary messages should be suppressed");

    prv_tasks_send_to_primary(TASK_SEND_STATUS);
    prv_flush_can();
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "The status must always be sent");
    TEST_ASSERT_EQUAL_UINT32(CAN_PRIMARY_MESSAGE_FRAME_ID_TSACSTATUS, last_sent[CAN_COMMUNICATION_NETWORK_PRIMARY].id);
}

void test_tasks_suppress_primary_messages_can_be_disabled(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_suppress_primary_messages(true));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_suppress_primary_messages(false));

    prv_tasks_send_to_primary(TASK_SEND_CURRENT);
    prv_flush_can();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "The primary messages should be sent again");
}

void test_tasks_suppress_primary_messages_does_not_affect_bms_network(void) {
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_init(prv_get_tick));
    TEST_ASSERT_EQUAL(MAINBOARD_TASKS_RC_OK, mainboard_tasks_suppress_primary_messages(true));

    prv_tasks_send_cellboard_set_balancing_status(TASK_SEND_CELLBOARD_SET_BALANCING_STATUS);
    prv_flush_can();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1U, sent_count[CAN_COMMUNICATION_NETWORK_BMS], "The cellboards must receive the balancing command");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, sent_count[CAN_COMMUNICATION_NETWORK_PRIMARY], "Nothing should be sent on the primary network");
    TEST_ASSERT_EQUAL_UINT32(CAN_BMS_MESSAGE_FRAME_ID_TSACMAINBOARDBALANCINGSET, last_sent[CAN_COMMUNICATION_NETWORK_BMS].id);
}

/** @} */

void setUp(void) {
    static bool pristine_saved = false;

    // The tasks table is modified by the initialization and by some tests, restore it every time
    if (!pristine_saved) {
        memcpy(pristine_task_list, task_list, sizeof(pristine_task_list));
        pristine_saved = true;
    }
    memcpy(task_list, pristine_task_list, sizeof(pristine_task_list));

    fake_tick = 0U;
    memset(task_calls, 0, sizeof(task_calls));
    memset(sent_count, 0, sizeof(sent_count));
    memset(last_sent, 0, sizeof(last_sent));
    memset(&tasks_handler, 0, sizeof(tasks_handler));
    memset(&mainboard_tasks_handler, 0, sizeof(mainboard_tasks_handler));

    const struct CanCommunicationNetworkConfig configs[CAN_COMMUNICATION_NETWORK_COUNT] = {
        [CAN_COMMUNICATION_NETWORK_PRIMARY] = { .send = prv_send_primary, .on_receive = prv_on_receive },
        [CAN_COMMUNICATION_NETWORK_BMS] = { .send = prv_send_bms, .on_receive = prv_on_receive },
    };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communication_api_init(configs));
}

void tearDown(void) {
    memcpy(task_list, pristine_task_list, sizeof(pristine_task_list));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_tasks_count_fits_in_the_scheduler);
    RUN_TEST(test_tasks_ids_are_sequential_and_start_from_zero);
    RUN_TEST(test_tasks_every_task_has_a_function);
    RUN_TEST(test_tasks_intervals_are_valid);
    RUN_TEST(test_tasks_can_table_covers_all_the_primary_tasks);
    RUN_TEST(test_tasks_can_table_has_only_primary_tasks);
    RUN_TEST(test_tasks_can_table_frame_ids_are_unique);
    RUN_TEST(test_tasks_can_table_payload_matches_the_frame);
    RUN_TEST(test_tasks_payload_getters_accept_null_byte_size);

    RUN_TEST(test_tasks_init_null_tick_callback);
    RUN_TEST(test_tasks_init_ok);
    RUN_TEST(test_tasks_init_enables_and_staggers_all_the_tasks);
    RUN_TEST(test_tasks_init_twice_restores_default_configuration);

    RUN_TEST(test_tasks_routine_before_init);
    RUN_TEST(test_tasks_routine_executes_the_first_task_at_tick_zero);
    RUN_TEST(test_tasks_routine_executes_tasks_following_the_stagger);
    RUN_TEST(test_tasks_routine_executes_every_task_once_after_a_long_pause);
    RUN_TEST(test_tasks_routine_executes_tasks_with_their_interval);
    RUN_TEST(test_tasks_routine_forwards_temporal_discontinuity);

    RUN_TEST(test_tasks_enable_disable_before_init);
    RUN_TEST(test_tasks_enable_task_invalid_id);
    RUN_TEST(test_tasks_disable_task_invalid_id);
    RUN_TEST(test_tasks_disable_and_enable_task);
    RUN_TEST(test_tasks_repeated_enable_and_disable_are_not_errors);
    RUN_TEST(test_tasks_disabled_task_is_not_executed);
    RUN_TEST(test_tasks_enabled_task_is_executed_again);
    RUN_TEST(test_tasks_enable_all_tasks);
    RUN_TEST(test_tasks_enable_all_tasks_when_already_enabled);

    RUN_TEST(test_tasks_send_to_primary_enqueues_the_message);
    RUN_TEST(test_tasks_send_to_primary_sends_every_message_with_its_id);
    RUN_TEST(test_tasks_send_to_primary_ignores_tasks_without_message);
    RUN_TEST(test_tasks_suppress_primary_messages_blocks_everything_but_the_status);
    RUN_TEST(test_tasks_suppress_primary_messages_can_be_disabled);
    RUN_TEST(test_tasks_suppress_primary_messages_does_not_affect_bms_network);

    return UNITY_END();
}
