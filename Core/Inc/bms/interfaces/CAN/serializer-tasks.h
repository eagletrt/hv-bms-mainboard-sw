/*!
 * \file serializer-tasks.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the CAN serializer tasks
 */

#ifndef SERIALIZER_TASKS_H
#define SERIALIZER_TASKS_H

#include <stdint.h>

#include "mainboard-def.h"
#include "serializer.h"

/*!
 * \brief Enum for the different serializer tasks
 */
enum SerializerTasksID : uint8_t {
    SERIALIZER_TASKS_SEND_STATUS = 0,                     /*!< Send the TSAC status message */
    SERIALIZER_TASKS_SEND_MAINBOARD_VERSION,              /*!< Send the mainboard version message */
    SERIALIZER_TASKS_SEND_MAINBOARD_VERSION_INFO,         /*!< Send the mainboard version info message */
    SERIALIZER_TASKS_SEND_MAINBOARD_LIBCAN_VERSION,       /*!< Send the mainboard libcan version message */
    SERIALIZER_TASKS_SEND_MAINBOARD_LIBCAN_VERSION_INFO,  /*!< Send the mainboard libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_VERSION,             /*!< Send the cellboard 1 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_VERSION,             /*!< Send the cellboard 2 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_VERSION,             /*!< Send the cellboard 3 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_VERSION,             /*!< Send the cellboard 4 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_VERSION,             /*!< Send the cellboard 5 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_VERSION,             /*!< Send the cellboard 6 version message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_VERSION_INFO,        /*!< Send the cellboard 1 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_VERSION_INFO,        /*!< Send the cellboard 2 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_VERSION_INFO,        /*!< Send the cellboard 3 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_VERSION_INFO,        /*!< Send the cellboard 4 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_VERSION_INFO,        /*!< Send the cellboard 5 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_VERSION_INFO,        /*!< Send the cellboard 6 version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_LIBCAN_VERSION,      /*!< Send the cellboard 1 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_LIBCAN_VERSION,      /*!< Send the cellboard 2 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_LIBCAN_VERSION,      /*!< Send the cellboard 3 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_LIBCAN_VERSION,      /*!< Send the cellboard 4 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_LIBCAN_VERSION,      /*!< Send the cellboard 5 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_LIBCAN_VERSION,      /*!< Send the cellboard 6 libcan version message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_LIBCAN_VERSION_INFO, /*!< Send the cellboard 1 libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_LIBCAN_VERSION_INFO, /*!< Send the cellboard 2 libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_LIBCAN_VERSION_INFO, /*!< Send the cellboard 3 libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_LIBCAN_VERSION_INFO, /*!< Send the cellboard 4 libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_LIBCAN_VERSION_INFO, /*!< Send the cellboard 5 libcan version info message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_LIBCAN_VERSION_INFO, /*!< Send the cellboard 6 libcan version info message */
    SERIALIZER_TASKS_SEND_CURRENT,                        /*!< Send the current message */
    SERIALIZER_TASKS_SEND_VOLTAGE_INFO,                   /*!< Send the internal voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_VOLTAGE,             /*!< Send the cellboard 1 voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_VOLTAGE,             /*!< Send the cellboard 2 voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_VOLTAGE,             /*!< Send the cellboard 3 voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_VOLTAGE,             /*!< Send the cellboard 4 voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_VOLTAGE,             /*!< Send the cellboard 5 voltages message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_VOLTAGE,             /*!< Send the cellboard 6 voltages message */
    SERIALIZER_TASKS_SEND_TEMPERATURE_INFO,               /*!< Send the internal temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_TEMPERATURE,         /*!< Send the cellboard 1 temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_TEMPERATURE,         /*!< Send the cellboard 2 temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_TEMPERATURE,         /*!< Send the cellboard 3 temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_TEMPERATURE,         /*!< Send the cellboard 4 temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_TEMPERATURE,         /*!< Send the cellboard 5 temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_TEMPERATURE,         /*!< Send the cellboard 6 temperatures message */
    SERIALIZER_TASKS_SEND_COOLING_TEMPERATURE1,           /*!< Send the first cooling temperatures message */
    SERIALIZER_TASKS_SEND_COOLING_TEMPERATURE2,           /*!< Send the second cooling temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_BALANCING,           /*!< Send the cellboard 1 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_BALANCING,           /*!< Send the cellboard 2 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_BALANCING,           /*!< Send the cellboard 3 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_BALANCING,           /*!< Send the cellboard 4 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_BALANCING,           /*!< Send the cellboard 5 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_BALANCING,           /*!< Send the cellboard 6 balancing status message */
    SERIALIZER_TASKS_SEND_CELLBOARD1_BAL_TEMPERATURE,     /*!< Send the cellboard 1 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD2_BAL_TEMPERATURE,     /*!< Send the cellboard 2 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD3_BAL_TEMPERATURE,     /*!< Send the cellboard 3 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD4_BAL_TEMPERATURE,     /*!< Send the cellboard 4 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD5_BAL_TEMPERATURE,     /*!< Send the cellboard 5 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_CELLBOARD6_BAL_TEMPERATURE,     /*!< Send the cellboard 6 discharge resistor temperatures message */
    SERIALIZER_TASKS_SEND_IMD_STATUS,                     /*!< Send the IMD status message */
    SERIALIZER_TASKS_SEND_FEEDBACK,                       /*!< Send the feedback message */
    SERIALIZER_TASKS_SEND_SHUTDOWN,                       /*!< Send the shutdown message */
    SERIALIZER_TASKS_SEND_FEEDBACK_SHUTDOWN,              /*!< Send the feedback shutdown message */
    SERIALIZER_TASKS_SEND_ERRORS,                         /*!< Send the mainboard errors message */
    SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_A,             /*!< Send the cellboard errors message A */
    SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_B,             /*!< Send the cellboard errors message B */
    SERIALIZER_TASKS_SEND_CELLBOARD_ERRORS_C,             /*!< Send the cellboard errors message C */
    SERIALIZER_TASKS_SEND_CELLBOARD_SET_BALANCING_STATUS, /*!< Send the cellboard set balancing status message */
    SERIALIZER_TASKS_COUNT                                /*!< Number of tasks */

};

/*!
 * \brief Priority of a serializer task
 *
 * \details The priority is used to determine the order in which tasks are executed.
 */
enum SerializerTasksPriority {
    SERIALIZER_TASKS_PRIORITY_LOW,  /*!< Low priority */
    SERIALIZER_TASKS_PRIORITY_HIGH, /*!< High priority */
};

/*!
 * \brief Return codes for the serializer tasks
 *
 * \details These are the possible return codes for the serializer tasks.
 */
enum SerializerTasksReturnCode {
    SERIALIZER_TASKS_RC_OK,            /*!< The operation was successful */
    SERIALIZER_TASKS_RC_ERROR,         /*!< An error occurred */
    SERIALIZER_TASKS_RC_NULL_POINTER,  /*!< A NULL pointer was passed to the function */
    SERIALIZER_TASKS_RC_INVALID_ID,    /*!< An invalid task ID was passed to the function */
    SERIALIZER_TASKS_RC_INVALID_PERIOD /*!< An invalid period was passed to the function */
};

/*!
 * \brief Function pointer type for serializer tasks
 *
 * \details This is a function pointer type that can be used to point to any function that takes a \ref SerializerMessage pointer as an argument and returns a \ref SerializerReturnCode.
 */
typedef enum SerializerReturnCode (*serializer_task_function)(struct SerializerMessage *message);

/*!
 * \brief Function pointer type for serializer tasks that require a cellboard ID or an index
 *
 * \details This is a function pointer type that can be used to point to any function that takes a \ref CellboardId
 * (compatible with uint8_t since its underlying type is fixed) or a uint8_t index and a \ref SerializerMessage pointer
 * as arguments and returns a \ref SerializerReturnCode.
 */
typedef enum SerializerReturnCode (*serializer_task_cellboard_function)(uint8_t arg, struct SerializerMessage *message);

/*!
 * \brief Function pointer type for serializer tasks that build a BMS network message
 *
 * \details This is a function pointer type that can be used to point to any function that takes a \ref SerializerBmsMessage pointer as an argument and returns a \ref SerializerReturnCode.
 */
typedef enum SerializerReturnCode (*serializer_task_bms_function)(struct SerializerBmsMessage *message);

/*!
 * \brief Kind of a serializer task, selects the active member of \ref SerializerTaskFunction and the network
 */
enum SerializerTaskKind : uint8_t {
    SERIALIZER_TASK_KIND_MAINBOARD, /*!< Uses task_function, sent on the primary network */
    SERIALIZER_TASK_KIND_CELLBOARD, /*!< Uses task_cellboard_function with the task arg, sent on the primary network */
    SERIALIZER_TASK_KIND_BMS,       /*!< Uses task_bms_function, sent on the BMS network */
};

/*!
 * \brief Callback of a serializer task, the active member is selected by \ref SerializerTaskKind
 */
union SerializerTaskFunction {
    serializer_task_function task_function;                     /*!< Function pointer for \ref SERIALIZER_TASK_KIND_MAINBOARD tasks */
    serializer_task_cellboard_function task_cellboard_function; /*!< Function pointer for \ref SERIALIZER_TASK_KIND_CELLBOARD tasks */
    serializer_task_bms_function task_bms_function;             /*!< Function pointer for \ref SERIALIZER_TASK_KIND_BMS tasks */
};

/*!
 * \brief Description of a periodic serializer task
 *
 * \details The tasks are stored in a table indexed by \ref SerializerTasksID, so the ID is not repeated here
 */
struct SerializerTask {
    enum SerializerTasksPriority priority; /*!< The task priority */
    enum SerializerTaskKind kind;          /*!< The task kind, selects the callback and the network */
    uint8_t arg;                           /*!< The cellboard ID or message index passed to the callback, used only by \ref SERIALIZER_TASK_KIND_CELLBOARD tasks */
    uint32_t period_ms;                    /*!< The task period in milliseconds */
    union SerializerTaskFunction function; /*!< The function to call to build the message */
};

/*! \brief Table of the serializer tasks, indexed by \ref SerializerTasksID */
extern const struct SerializerTask serializer_tasks[SERIALIZER_TASKS_COUNT];

#endif // SERIALIZER_TASKS_H