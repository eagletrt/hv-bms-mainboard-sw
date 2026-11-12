/*!
 * \file mainboard-conf.h
 * \date 2024-06-19
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Main configuration for the mainboard project
 *
 * \details Tunable settings (feature toggles, thresholds, periods, ...)
 * Fixed definitions go in mainboard-def.h instead
 *
 * \attention Do not touch unless you known what you are doing
 */

#ifndef MAINBOARD_CONF_H
#define MAINBOARD_CONF_H

#include "mainboard-def.h"

/*!* ######################### FEEDBACK ################################### *!*/

/*!
 * \defgroups feedback-conf
 * \brief Configuration of the feedback module (SD, IMD, AIRs, precharge, ...)
 * {\
 */

/*!
 * \brief Number of samples kept for every feedback
 *
 * \details The filter works on the whole buffer, so the filtered values (and
 *          every check made on them) are available only after this many samples
 */
#define FEEDBACK_BUFFER_SIZE (8U)

/*!
 * \brief Thresholds for the analog feedback in V
 *
 * \details Above HIGH the feedback is logically high, below LOW it is logically
 *          low, in between it is in an implausible (error) state
 */
#define FEEDBACK_THRESHOLD_HIGH_V (1.9f)
#define FEEDBACK_THRESHOLD_LOW_V (0.7f)

/*!
 * \brief High threshold of the 5V MCU feedback when the handcart is connected
 *
 * \details The voltage divider is different when the handcart is connected,
 *          so the voltage at the pin is lower
 */
#define FEEDBACK_THRESHOLD_HIGH_HANDCART_V (1.0f)

/*! \} */

#endif // MAINBOARD_CONF_H
