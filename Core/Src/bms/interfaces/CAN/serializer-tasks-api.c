/*!
 * \file serializer-tasks-api.c
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN serializer tasks definition
 *
 * \details The periods come from the libcan cycle times; the CAN cellboard numbering starts
 * from 1 while the cellboard IDs start from 0, so TsacCellboard1 maps to CELLBOARD_ID_0
 */

#include "serializer-tasks-api.h"

#include <stdint.h>
#include <string.h>

#include "can-bms-api.h"
#include "can-communication-api.h"
#include "can-primary-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"
#include "serializer-api.h"
#include "tasks-api.h"

// Every task needs a slot in the tasks module and its ID must fit the tasks module ID type
static_assert(SERIALIZER_TASKS_COUNT <= TASKS_MAX_COUNT, "Increase TASKS_MAX_COUNT");
static_assert(SERIALIZER_TASKS_COUNT <= UINT8_MAX, "Too many serializer tasks");

EAGLETRT_STATIC struct TasksHandler serializer_tasks_handler;

/*! \brief Shorthands to keep the table readable */
#define HIGH (SERIALIZER_TASKS_PRIORITY_HIGH)
#define LOW (SERIALIZER_TASKS_PRIORITY_LOW)

/*!
 * \brief Task initializers, each one sets the kind together with the matching callback so they cannot mismatch
 *
 * \details The arg of the cellboard tasks is the cellboard ID, or the message index for the cellboard errors
 */
#define MB_TASK(prio, period, fn)                               \
    {                                                           \
        (prio), SERIALIZER_TASK_KIND_MAINBOARD, 0U, (period), { \
            .task_function = (fn)                               \
        }                                                       \
    }
#define CB_TASK(prio, arg, period, fn)                             \
    {                                                              \
        (prio), SERIALIZER_TASK_KIND_CELLBOARD, (arg), (period), { \
            .task_cellboard_function = (fn)                        \
        }                                                          \
    }
#define BMS_TASK(prio, period, fn)                        \
    {                                                     \
        (prio), SERIALIZER_TASK_KIND_BMS, 0U, (period), { \
            .task_bms_function = (fn)                     \
        }                                                 \
    }

const struct SerializerTask serializer_tasks[SERIALIZER_TASKS_COUNT] = {
    // Status
    [SERIALIZER_TASKS_SEND_STATUS] = MB_TASK(HIGH, can_primary_cycle_time_tsacstatus, serializer_api_get_status_message),

    // Mainboard versions
    [SERIALIZER_TASKS_SEND_MAINBOARD_VERSION] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardversion, serializer_api_get_mainboard_version_message),
    [SERIALIZER_TASKS_SEND_MAINBOARD_VERSION_INFO] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardversioninfo, serializer_api_get_mainboard_version_info_message),
    [SERIALIZER_TASKS_SEND_MAINBOARD_LIBCAN_VERSION] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardlibcanversion, serializer_api_get_libcan_version_message),
    [SERIALIZER_TASKS_SEND_MAINBOARD_LIBCAN_VERSION_INFO] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardlibcanversioninfo, serializer_api_get_libcan_version_info_message),

    // Cellboard versions (relayed)
    [SERIALIZER_TASKS_SEND_CELLBOARD1_VERSION] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1version, serializer_api_get_cellboard_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_VERSION] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2version, serializer_api_get_cellboard_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_VERSION] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3version, serializer_api_get_cellboard_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_VERSION] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4version, serializer_api_get_cellboard_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_VERSION] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5version, serializer_api_get_cellboard_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_VERSION] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6version, serializer_api_get_cellboard_version_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1versioninfo, serializer_api_get_cellboard_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2versioninfo, serializer_api_get_cellboard_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3versioninfo, serializer_api_get_cellboard_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4versioninfo, serializer_api_get_cellboard_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5versioninfo, serializer_api_get_cellboard_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6versioninfo, serializer_api_get_cellboard_version_info_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1libcanversion, serializer_api_get_cellboard_libcan_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2libcanversion, serializer_api_get_cellboard_libcan_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3libcanversion, serializer_api_get_cellboard_libcan_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4libcanversion, serializer_api_get_cellboard_libcan_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5libcanversion, serializer_api_get_cellboard_libcan_version_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_LIBCAN_VERSION] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6libcanversion, serializer_api_get_cellboard_libcan_version_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_LIBCAN_VERSION_INFO] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6libcanversioninfo, serializer_api_get_cellboard_libcan_version_info_message),

    // Current and voltages
    [SERIALIZER_TASKS_SEND_CURRENT] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardcurrentinfo, serializer_api_get_current_message),
    [SERIALIZER_TASKS_SEND_VOLTAGE_INFO] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardvoltageinfo, serializer_api_get_internal_voltages_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1voltage, serializer_api_get_cellboard_voltages_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2voltage, serializer_api_get_cellboard_voltages_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3voltage, serializer_api_get_cellboard_voltages_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4voltage, serializer_api_get_cellboard_voltages_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5voltage, serializer_api_get_cellboard_voltages_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_VOLTAGE] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6voltage, serializer_api_get_cellboard_voltages_message),

    // Temperatures
    [SERIALIZER_TASKS_SEND_TEMPERATURE_INFO] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardtemperatureinfo, serializer_api_get_temperatures_info_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1temperature, serializer_api_get_cellboard_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2temperature, serializer_api_get_cellboard_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3temperature, serializer_api_get_cellboard_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4temperature, serializer_api_get_cellboard_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5temperature, serializer_api_get_cellboard_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6temperature, serializer_api_get_cellboard_temperatures_message),

    [SERIALIZER_TASKS_SEND_COOLING_TEMPERATURE1] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardcoolingtemperature1, serializer_api_get_cooling_temperatures_1_message),
    [SERIALIZER_TASKS_SEND_COOLING_TEMPERATURE2] = MB_TASK(LOW, can_primary_cycle_time_tsacmainboardcoolingtemperature2, serializer_api_get_cooling_temperatures_2_message),

    // Balancing
    [SERIALIZER_TASKS_SEND_CELLBOARD1_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1balancing, serializer_api_get_balancing_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2balancing, serializer_api_get_balancing_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3balancing, serializer_api_get_balancing_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4balancing, serializer_api_get_balancing_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5balancing, serializer_api_get_balancing_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_BALANCING] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6balancing, serializer_api_get_balancing_message),

    [SERIALIZER_TASKS_SEND_CELLBOARD1_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_0, can_primary_cycle_time_tsaccellboard1balancingtemperature, serializer_api_get_balancing_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD2_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_1, can_primary_cycle_time_tsaccellboard2balancingtemperature, serializer_api_get_balancing_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD3_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_2, can_primary_cycle_time_tsaccellboard3balancingtemperature, serializer_api_get_balancing_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD4_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_3, can_primary_cycle_time_tsaccellboard4balancingtemperature, serializer_api_get_balancing_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD5_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_4, can_primary_cycle_time_tsaccellboard5balancingtemperature, serializer_api_get_balancing_temperatures_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD6_BAL_TEMPERATURE] = CB_TASK(LOW, CELLBOARD_ID_5, can_primary_cycle_time_tsaccellboard6balancingtemperature, serializer_api_get_balancing_temperatures_message),

    // Safety
    [SERIALIZER_TASKS_SEND_IMD_STATUS] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardimd, serializer_api_get_imd_message),
    [SERIALIZER_TASKS_SEND_FEEDBACK] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardfeedback, serializer_api_get_feedback_message),
    [SERIALIZER_TASKS_SEND_SHUTDOWN] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardshutdown, serializer_api_get_shutdown_message),
    [SERIALIZER_TASKS_SEND_FEEDBACK_SHUTDOWN] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboardfeedbackshutdown, serializer_api_get_feedback_shutdown_message),

    // Errors (the cellboard errors messages are selected by index, not by cellboard: the arg holds the index)
    [SERIALIZER_TASKS_SEND_ERRORS] = MB_TASK(HIGH, can_primary_cycle_time_tsacmainboarderror, serializer_api_get_mainboard_errors_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_A] = CB_TASK(HIGH, 0U, can_primary_cycle_time_tsaccellboarderrora, serializer_api_get_cellboard_errors_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_B] = CB_TASK(HIGH, 1U, can_primary_cycle_time_tsaccellboarderrorb, serializer_api_get_cellboard_errors_message),
    [SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_C] = CB_TASK(HIGH, 2U, can_primary_cycle_time_tsaccellboarderrorc, serializer_api_get_cellboard_errors_message),

    // BMS network
    [SERIALIZER_TASKS_SEND_CELLBOARD_SET_BALANCING_STATUS] = BMS_TASK(LOW, can_bms_cycle_time_tsacmainboardbalancingset, serializer_api_get_balancing_set_message),
};

#undef HIGH
#undef LOW
#undef MB_TASK
#undef CB_TASK
#undef BMS_TASK

/*!
 * \brief Serialize a primary network message and add it to the primary network TX queue
 *
 * \details The frame is dropped if the serialization fails or the queue is full; the next
 * execution of the task sends updated values anyway
 *
 * \param[in] message The message to send
 */
EAGLETRT_STATIC void prv_serializer_tasks_send_primary(struct SerializerMessage *message) {
    struct CanCommunicationFrame frame = { .id = (uint32_t)message->id };
    const int length = can_primary_api_serialize_from_id(message->id, &message->payload, frame.data);
    if (length < 0 || length > (int)CAN_COMMUNICATION_FRAME_DATA_SIZE) {
        return;
    }
    frame.length = (uint8_t)length;
    EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_NETWORK_PRIMARY, &frame));
}

/*!
 * \brief Serialize a BMS network message and add it to the BMS network TX queue
 *
 * \details Same drop policy as \ref prv_serializer_tasks_send_primary
 *
 * \param[in] message The message to send
 */
EAGLETRT_STATIC void prv_serializer_tasks_send_bms(struct SerializerBmsMessage *message) {
    struct CanCommunicationFrame frame = { .id = (uint32_t)message->id };
    const int length = can_bms_api_serialize_from_id(message->id, &message->payload, frame.data);
    if (length < 0 || length > (int)CAN_COMMUNICATION_FRAME_DATA_SIZE) {
        return;
    }
    frame.length = (uint8_t)length;
    EAGLETRT_API_UNUSED(can_communication_api_add_to_tx(CAN_NETWORK_BMS, &frame));
}

/*!
 * \brief Callback shared by every serializer task
 *
 * \details Builds the message of the task with the callback from \ref serializer_tasks, selected
 * by the task kind, and sends it on the network of that kind
 *
 * \param[in] task_id The ID of the executed task, a \ref SerializerTasksID
 */
EAGLETRT_STATIC void prv_serializer_tasks_run(uint8_t task_id) {
    if (task_id >= SERIALIZER_TASKS_COUNT) {
        return;
    }

    const struct SerializerTask *task = &serializer_tasks[task_id];

    switch (task->kind) {
        case SERIALIZER_TASK_KIND_MAINBOARD: {
            struct SerializerMessage message = { 0 };
            if (task->function.task_function(&message) == SERIALIZER_RC_OK) {
                prv_serializer_tasks_send_primary(&message);
            }
            break;
        }
        case SERIALIZER_TASK_KIND_CELLBOARD: {
            struct SerializerMessage message = { 0 };
            if (task->function.task_cellboard_function(task->arg, &message) == SERIALIZER_RC_OK) {
                prv_serializer_tasks_send_primary(&message);
            }
            break;
        }
        case SERIALIZER_TASK_KIND_BMS: {
            struct SerializerBmsMessage message = { 0 };
            if (task->function.task_bms_function(&message) == SERIALIZER_RC_OK) {
                prv_serializer_tasks_send_bms(&message);
            }
            break;
        }
        default:
            break;
    }
}

enum SerializerTasksReturnCode serializer_tasks_api_init(task_tick_callback get_tick) {
    if (get_tick == NULL) {
        return SERIALIZER_TASKS_RC_NULL_POINTER;
    }

    memset(&serializer_tasks_handler, 0, sizeof(serializer_tasks_handler));

    TaskList tasks_list = { 0 };
    for (uint8_t i = 0U; i < SERIALIZER_TASKS_COUNT; ++i) {
        // Every member of the union is a function pointer, so checking one of them is enough
        if (serializer_tasks[i].function.task_function == NULL) {
            return SERIALIZER_TASKS_RC_NULL_POINTER;
        }

        const uint32_t period_ms = serializer_tasks[i].period_ms;
        if (period_ms == 0U || period_ms > UINT16_MAX) {
            return SERIALIZER_TASKS_RC_INVALID_PERIOD;
        }

        tasks_list[i].task_id = i;
        tasks_list[i].state = TASKS_STATE_ENABLED;
        tasks_list[i].repeats = 0U; // Run forever
        tasks_list[i].function = prv_serializer_tasks_run;
        tasks_list[i].interval = (uint16_t)period_ms;
        tasks_list[i].start = i; // Spread the first executions over consecutive ticks
    }

    if (tasks_api_init(&serializer_tasks_handler, tasks_list, (uint8_t)SERIALIZER_TASKS_COUNT, get_tick) != TASKS_RC_OK) {
        return SERIALIZER_TASKS_RC_ERROR;
    }
    return SERIALIZER_TASKS_RC_OK;
}

enum SerializerTasksReturnCode serializer_tasks_api_routine(void) {
    if (tasks_api_routine(&serializer_tasks_handler) != TASKS_RC_OK) {
        return SERIALIZER_TASKS_RC_ERROR;
    }
    return SERIALIZER_TASKS_RC_OK;
}

enum SerializerTasksReturnCode serializer_tasks_api_suppress_comms(enum SerializerTasksPriority priority, bool suppress) {
    enum SerializerTasksReturnCode ret = SERIALIZER_TASKS_RC_OK;

    // Keep going on failure so a single task does not leave the others in the wrong state
    for (uint8_t i = 0U; i < SERIALIZER_TASKS_COUNT; ++i) {
        if (serializer_tasks[i].priority != priority) {
            continue;
        }
        const enum TasksReturnCode rc = suppress ? tasks_api_disable_task(&serializer_tasks_handler, i)
                                                 : tasks_api_enable_task(&serializer_tasks_handler, i);
        if (rc != TASKS_RC_OK) {
            ret = SERIALIZER_TASKS_RC_ERROR;
        }
    }
    return ret;
}
