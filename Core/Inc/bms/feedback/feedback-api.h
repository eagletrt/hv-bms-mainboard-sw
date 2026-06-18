/*!
 * \file feedback-api.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */

#ifndef FEEDBACK_API_H
#define FEEDBACK_API_H

#include <stdbool.h>

#include "feedback.h"

#ifdef CONF_FEEDBACK_MODULE_ENABLE

/*!
 * \brief Initialize the internal feedback handler structure
 *
 * \param read_all A pointer to the callback that should read all the digital feedbacks
 * \param start_conversion A pointer to the callback that should start the conversion of the analog feedbacks
 *
 * \retval FEEDBACK_NULL_POINTER if any of the parameters are NULL
 * \retval FEEDBACK_OK otherwise
 */
enum FeedbackReturnCode feedback_api_init(feedback_read_digital_all_callback read_all, feedback_start_analog_conversion_callback start_conversion);

/*!
 * \brief Update all the digital feedbacks
 *
 * \retval FEEDBACK_OK
 */
enum FeedbackReturnCode feedback_api_update_digital_feedback_all(void);

/*!
 * \brief Start the conversion of the analog feedbacks
 *
 * \retval FEEDBACK_OK
 */
enum FeedbackReturnCode feedback_api_start_analog_conversion_all(void);

/*!
 * \brief Callback to update a single value of the analog feedbacks
 *
 * \param index The index of the analog feedback
 * \param value The voltage value of the feedback in V
 *
 * \retval FEEDBACK_INVALID_INDEX the given index is not valid
 * \retval FEEDBACK_OK otherwise
 */
enum FeedbackReturnCode feedback_api_update_analog_feedback(enum FeedbackAnalogIndex index, volt_t value);

/*!
 * \brief Update the status of all the feedbacks according to the internal values of the feedback handler,
 * this function does not read the feedbacks values but only update their status based on the current values and the defined thresholds
 *
 * \retval FEEDBACK_OK
 */
enum FeedbackReturnCode feedback_api_update_status(void);

/*!
 * \brief Get the value of a digital feedback
 *
 * \param bit The digital feedback to get the value from
 *
 * \returns bool The value of the feedback
 */
bool feedback_api_get_digital(enum FeedbackDigitalBit bit);

/*!
 * \brief Get the value of an analog feedback
 *
 * \param index The index of the analog feedback to get the value from
 *
 * \returns volt_t The feedbacks voltage values
 */
volt_t feedback_api_get_analog(enum FeedbackAnalogIndex index);

/*!
 * \brief Get the status of a single feedback
 *
 * \param id_fb The identifier of the feedback
 *
 * \return enum FeedbackStatus The feedback status
 */
enum FeedbackStatus feedback_api_get_status(enum FeedbackId id_fb);

/*!
 * \brief Check if the feedbacks specified in the mask are in the expected status
 * 
 * \param mask The mask used to select the feedbacks to check
 * \param value The expected values of the feedbacks
 * \param out[out] The identifer of the feedback that does not match the expected value
 * or FEEDBACK_ID_UNKNOWN if every feedback is ok (can be NULL)
 *
 * \returns bool True if all the feedbacks match the expected value, false otherwise
 */
bool feedback_api_check_values(bit_flag32_t mask, bit_flag32_t value, enum FeedbackId *out);

/*!
 * \brief Check if a feedback is digital or analog given its identifier
 *
 * \param id_fb The identifier of the feedback
 *
 * \returns bool True if the feedback is digital, false otherwise
 */
bool feedback_api_is_digital(enum FeedbackId id_fb);

/*!
 * \brief Get the feedback digital bit position from its identifier
 *
 * \param id_fb The identifier of the feedback
 *
 * \returns enum FeedbackDigitalBit The bit position of the digital feedback
 */
enum FeedbackDigitalBit feedback_api_get_digital_bit_from_id(enum FeedbackId id_fb);

/*!
 * \brief Get the feedback analog index from its identifier
 *
 * \param id_fb The identifier of the feedback
 *
 * \returns enum FeedbackAnalogIndex The index of the analog feedback
 */
enum FeedbackAnalogIndex feedback_api_get_analog_index_from_id(enum FeedbackId id_fb);

/*!
 * \brief Get a pointer to the CAN payload structure of the feedbacks status
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_feedback_status_converted_t* A pointer to the payload
 */
primary_hv_feedback_status_converted_t *feedback_api_get_status_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload structure of the digital feedbacks values
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_feedback_digital_converted_t* A pointer to the payload
 */
primary_hv_feedback_digital_converted_t *feedback_api_get_digital_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload structure of the analog feedbacks values
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_feedback_analog_converted_t* A pointer to the payload
 */
primary_hv_feedback_analog_converted_t *feedback_api_get_analog_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload structure of the analog shutdown feedbacks values
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_feedback_analog_sd_converted_t* A pointer to the payload
 */
primary_hv_feedback_analog_sd_converted_t *feedback_api_get_analog_sd_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the CAN payload structure of the feedback that did not
 * allow the BMS to go to the TS ON state
 *
 * \param id_fb The identifier of the feedback
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns primary_hv_feedback_enzomma_converted_t* A pointer to the payload
 */
primary_hv_feedback_enzomma_converted_t *feedback_api_get_enzomma_payload(enum FeedbackId id_fb, size_t *byte_size);

#ifdef CONF_FEEDBACK_STRINGS_ENABLE

/*!
 * \brief Get the name of the corresponding feedback identifier
 *
 * \param id The feedback identifier
 *
 * \returns const char* A pointer to the name of the feedback id
 */
const char *const feedback_api_get_feedback_id_name(enum FeedbackId id);

#else // CONF_FEEDBACK_STRINGS_ENABLE

#define feedback_api_get_feedback_id_name(id) ""

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#else // CONF_FEEDBACK_MODULE_ENABLE

#define feedback_api_init(read_all, start_conversion) (FEEDBACK_RC_OK)
#define feedback_api_update_digital_feedback_all() (FEEDBACK_RC_OK)
#define feedback_api_start_analog_conversion_all() (FEEDBACK_RC_OK)
#define feedback_api_update_analog_feedback(index, value) (FEEDBACK_RC_OK)
#define feedback_api_update_status() (FEEDBACK_RC_OK)
#define feedback_api_get_digital(bit) (false)
#define feedback_api_get_analog(index) (0.f)
#define feedback_api_get_status(id) (FEEDBACK_STATUS_ERROR)
#define feedback_api_check_values(mask, value, out) (true)
#define feedback_api_is_digital(id) (true)
#define feedback_api_get_digital_bit_from_id(id) (FEEDBACK_DIGITAL_BIT_UNKNOWN)
#define feedback_api_get_analog_index_from_id(id) (FEEDBACK_ANALOG_INDEX_UNKNOWN)
#define feedback_api_get_status_payload(byte_size) (NULL)
#define feedback_api_get_digital_payload(byte_size) (NULL)
#define feedback_api_get_analog_payload(byte_size) (NULL)
#define feedback_api_get_analog_sd_payload(byte_size) (NULL)
#define feedback_api_get_enzomma_payload(id, byte_size) (NULL)

#endif // CONF_FEEDBACK_MODULE_ENABLE

#endif // FEEDBACK_API_H
