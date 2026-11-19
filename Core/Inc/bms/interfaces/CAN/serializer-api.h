/*!
 * \file serializer-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief CAN serializer: gets the updated values and builds the packets to send
 */

#ifndef SERIALIZER_API_H
#define SERIALIZER_API_H

#include "serializer.h"

/*!
 * \brief Initialize the serializer
 *
 * \details The cellboard voltages and temperatures restart from their first group
 * and the firmware build time is computed
 */
void serializer_api_init(void);

/*!
 * \brief Suppress or resume the periodic messages
 *
 * \details Used during a flash procedure, when the mainboard must stay quiet on the CAN networks;
 * the message builders still work, the senders must check \ref serializer_api_is_comms_suppressed
 *
 * \param[in] suppress true to stop sending the periodic messages, false to resume them
 */
void serializer_api_suppress_comms(bool suppress);

/*!
 * \brief Check if the periodic messages are suppressed
 *
 * \retval true if the periodic messages must not be sent
 * \retval false otherwise
 */
bool serializer_api_is_comms_suppressed(void);

/*!
 * \brief Build the TSAC status message (TsacStatus)
 *
 * \details Carries the state of the mainboard FSM and of every cellboard
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_status_message(struct SerializerMessage *message);

/*!
 * \brief Build the balancing set message sent to the cellboards on the BMS network (TsacMainboardBalancingSet)
 *
 * \details Carries whether the balancing is active, the target voltage and the threshold
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_balancing_set_message(struct SerializerBmsMessage *message);

/*!
 * \brief Build the balancing status message of a cellboard (TsacCellboardNBalancing)
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_balancing_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the first cooling temperatures message (TsacMainboardCoolingTemperature1)
 *
 * \details Carries the inlet and the outlets from 1 to 3
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cooling_temperatures_1_message(struct SerializerMessage *message);

/*!
 * \brief Build the second cooling temperatures message (TsacMainboardCoolingTemperature2)
 *
 * \details Carries the outlets from 4 to 6
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cooling_temperatures_2_message(struct SerializerMessage *message);

/*!
 * \brief Build the current message (TsacMainboardCurrentInfo)
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_current_message(struct SerializerMessage *message);

/*!
 * \brief Build the mainboard errors message (TsacMainboardError)
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_mainboard_errors_message(struct SerializerMessage *message);

/*!
 * \brief Build one of the cellboard errors messages (TsacCellboardErrorA, B or C)
 *
 * \details Message N carries the errors of cellboards 2N and 2N + 1
 *
 * \param[in] index The message index, from 0 (A) to SERIALIZER_CELLBOARD_ERRORS_MESSAGE_COUNT - 1
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_INDEX if the given index is out of range
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_errors_message(uint8_t index, struct SerializerMessage *message);

/*!
 * \brief Build the feedback message (TsacMainboardFeedback)
 *
 * \details Carries the commanded and mechanical state of the relays, the plausibility check,
 * the cockpit LEDs and the TSAL; every signal keeps the polarity of the physical feedback
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_feedback_message(struct SerializerMessage *message);

/*!
 * \brief Build the IMD message (TsacMainboardImd)
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_imd_message(struct SerializerMessage *message);

/*!
 * \brief Build the internal voltages message (TsacMainboardVoltageInfo)
 *
 * \details Carries the TS and pack voltages and the sum, average, minimum and maximum of the cell voltages
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_internal_voltages_message(struct SerializerMessage *message);

/*!
 * \brief Build the shutdown message (TsacMainboardShutdown)
 *
 * \details Carries the voltages of the shutdown circuit nodes owned by the mainboard and the IMD status
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_shutdown_message(struct SerializerMessage *message);

/*!
 * \brief Build the shutdown feedback message (TsacMainboardFeedbackShutdown)
 *
 * \details Carries the voltages at the shutdown input and at the end of the shutdown circuit
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_feedback_shutdown_message(struct SerializerMessage *message);

/*!
 * \brief Build the temperatures info message (TsacMainboardTemperatureInfo)
 *
 * \details Carries the minimum, maximum and average cell temperature of the pack
 * and the precharge and linear regulator temperatures
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_temperatures_info_message(struct SerializerMessage *message);

/*!
 * \brief Build the next cell temperatures message of a cellboard (TsacCellboardNTemperature)
 *
 * \details Each call sends the next group of temperatures, wrapping around after the last one
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_temperatures_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the discharge resistor temperatures message of a cellboard (TsacCellboardNBalancingTemperature)
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_balancing_temperatures_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the mainboard firmware version message (TsacMainboardVersion)
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_mainboard_version_message(struct SerializerMessage *message);

/*!
 * \brief Build the mainboard firmware version info message (TsacMainboardVersionInfo)
 *
 * \details Carries the build time, the commit hash and whether the working tree was dirty
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_mainboard_version_info_message(struct SerializerMessage *message);

/*!
 * \brief Build the libcan version message of the mainboard (TsacMainboardLibcanVersion)
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_libcan_version_message(struct SerializerMessage *message);

/*!
 * \brief Build the libcan version info message of the mainboard (TsacMainboardLibcanVersionInfo)
 *
 * \details Carries the generation time, the commit hash and whether the working tree was dirty
 *
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_libcan_version_info_message(struct SerializerMessage *message);

/*!
 * \brief Build the firmware version message of a cellboard (TsacCellboardNVersion)
 *
 * \details Relays the version received from the cellboard, all zeros until it is received
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_version_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the firmware version info message of a cellboard (TsacCellboardNVersionInfo)
 *
 * \details Relays the version info received from the cellboard, all zeros until it is received
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_version_info_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the libcan version message of a cellboard (TsacCellboardNLibcanVersion)
 *
 * \details Relays the libcan version received from the cellboard, all zeros until it is received
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_libcan_version_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the libcan version info message of a cellboard (TsacCellboardNLibcanVersionInfo)
 *
 * \details Relays the libcan version info received from the cellboard, all zeros until it is received
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_libcan_version_info_message(enum CellboardId cellboard, struct SerializerMessage *message);

/*!
 * \brief Build the next cell voltages message of a cellboard (TsacCellboardNVoltage)
 *
 * \details Each call sends the next group of voltages, wrapping around after the last one
 *
 * \param[in] cellboard The cellboard id
 * \param[out] message The message to fill
 *
 * \retval SERIALIZER_RC_NULL_POINTER if message is NULL
 * \retval SERIALIZER_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval SERIALIZER_RC_OK otherwise
 */
enum SerializerReturnCode serializer_api_get_cellboard_voltages_message(enum CellboardId cellboard, struct SerializerMessage *message);

#endif // SERIALIZER_API_H
