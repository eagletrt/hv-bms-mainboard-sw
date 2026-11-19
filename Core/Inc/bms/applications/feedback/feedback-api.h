/*!
 * \file feedback-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback handling (SD, IMD, AIRs, precharge, ...)
 *
 * \details Value pipeline: the sensing interface pushes the raw samples into
 *          a circular buffer per feedback, then \ref feedback_api_routine filters
 *          them, checks the filtered values and updates the mainboard status
 *          (conditions and faults owned by this module)
 */

#ifndef FEEDBACK_API_H
#define FEEDBACK_API_H

#include <stdbool.h>

#include "feedback.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the feedback API
 */
void feedback_api_init(void);

/*!
 * \brief Set the value of a digital feedback
 *
 * \param index The index of the digital feedback
 * \param value The value to set
 * \retval FEEDBACK_RC_OK if the operation was successful
 * \retval FEEDBACK_RC_INVALID_INDEX if the index is invalid
 * \retval FEEDBACK_RC_ERROR for other errors
 */
enum FeedbackReturnCode feedback_api_set_digital_value(enum FeedbackDigitalIndex index, bool value);

/*!
 * \brief Set the value of an analog feedback
 *
 * \param index The index of the analog feedback
 * \param value The value to set
 * \retval FEEDBACK_RC_OK if the operation was successful
 * \retval FEEDBACK_RC_INVALID_INDEX if the index is invalid
 * \retval FEEDBACK_RC_ERROR for other errors
 */
enum FeedbackReturnCode feedback_api_set_analog_value(enum FeedbackAnalogIndex index, volt value);

/*!
 * \brief Get the filtered value of a digital feedback
 *
 * \details The value keeps the polarity of the physical signal (see \ref FeedbackDigitalIndex)
 *
 * \param index The index of the digital feedback
 * \returns The filtered value, false if the index is invalid or the feedback is not ready
 */
bool feedback_api_get_digital_value(enum FeedbackDigitalIndex index);

/*!
 * \brief Get the filtered voltage of an analog feedback
 *
 * \details The voltage is the one at the MCU pin, use the FEEDBACK_TO_*_VOLT macros
 *          to get the voltage of the measured line
 *
 * \param index The index of the analog feedback
 * \returns The filtered voltage, 0 if the index is invalid or the feedback is not ready
 */
volt feedback_api_get_analog_value(enum FeedbackAnalogIndex index);

/*!
 * \brief Return the readyness of the module
 * 
 * \returns true once all the feedbacks are ready
 */
bool feedback_api_is_ready(void);

/*!
 * \brief Run the feedback API routine, this function runs the filtering and updates the mainboard status accordingly
 *
 * \retval FEEDBACK_RC_OK if the routine executed successfully
 */
enum FeedbackReturnCode feedback_api_routine(void);

#endif // FEEDBACK_API_H
