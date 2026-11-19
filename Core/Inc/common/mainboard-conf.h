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

/*!* ######################### VERSION #################################### *!*/

/*!
 * \defgroups version-conf
 * \brief Version of the mainboard firmware
 * {\
 */

/*! \brief Major version of the mainboard firmware */
#define MAINBOARD_VERSION_MAJOR (0U)
/*! \brief Minor version of the mainboard firmware */
#define MAINBOARD_VERSION_MINOR (1U)
/*! \brief Patch version of the mainboard firmware */
#define MAINBOARD_VERSION_PATCH (0U)

/*! \brief Build date and time of the firmware, in the format of __DATE__ and __TIME__ */
#define MAINBOARD_BUILD_TIME_STR (__DATE__ " " __TIME__)

/*! \} */

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
#define FEEDBACK_THRESHOLD_HIGH_V (1.9F)
#define FEEDBACK_THRESHOLD_LOW_V (0.7F)

/*!
 * \brief Low threshold of the AIRs and IMD feedback in V
 *
 * \details Their feedback voltage is higher than expected when logically low,
 *          so the standard low threshold is raised
 */
#define FEEDBACK_THRESHOLD_LOW_AIR_IMD_V (1.6F)

/*!
 * \brief Window of the 3V3 probing feedback in V
 *
 * \details The 3V3 line comes out of a divider (about 1.6V), it is correct
 *          only when inside the window
 */
#define FEEDBACK_THRESHOLD_PROBING_3V3_LOW_V (0.7F)
#define FEEDBACK_THRESHOLD_PROBING_3V3_HIGH_V (1.9F)

/*!
 * \brief High threshold of the 5V MCU feedback when the handcart is connected
 *
 * \details The voltage divider is different when the handcart is connected,
 *          so the voltage at the pin is lower
 */
#define FEEDBACK_THRESHOLD_HIGH_HANDCART_V (1.0F)

/*! \} */

/*!* ######################### SAFETY #################################### *!*/

/*!
 * \defgroups phc-conf
 * \brief Configuration of the Pack Health Controller (PHC)
 * {\
 */

/*!
 * \brief Thresholds for the voltages PHC checks
 */
#define PHC_MIN_CELL_VOLTAGE (0)
#define PHC_MAX_CELL_VOLTAGE (0)

/*!
 * \brief Thresholds for the temperatures PHC checks
 */
#define PHC_MIN_CELL_TEMPERATURE (0)
#define PHC_MAX_CELL_TEMPERATURE (0)

/*!
 * \brief Thresholds for the discharge resistor temperature PHC checks
 */
#define PHC_MIN_DISCHARGE_TEMPERATURE (0)
#define PHC_MAX_DISCHARGE_TEMPERATURE (0)

/*!
 * \brief Thresholds for the cooling system temperature PHC checks
 */
#define PHC_MIN_COOLING_TEMPERATURE (0)
#define PHC_MAX_COOLING_TEMPERATURE (0)

/*!
 * \brief Thresholds for the linear regulator temperature PHC checks
 */
#define PHC_MIN_LINEAR_REGULATOR_TEMPERATURE (0)
#define PHC_MAX_LINEAR_REGULATOR_TEMPERATURE (0)

/*!
 * \brief Thresholds for the precharge temperature PHC checks
 */
#define PHC_MIN_PRECHARGE_TEMPERATURE (0)
#define PHC_MAX_PRECHARGE_TEMPERATURE (0)

/*!
 * \brief Thresholds for the current PHC checks
 */
#define PHC_MAX_CURRENT (0)

/*!
 * \brief Thresholds for the power PHC checks
 */
#define PHC_MAX_POWER (0)

/*!
 * \brief Maximum interval between two consecutive PHC checks in ms
 *
 * \details If a check is not called within this time, the corresponding fault
 *          is set
 */
#define PHC_MAX_CHECK_INTERVAL_MS (50U)
#define PHC_MAX_STARTUP_INTERVAL_MS (500U)

/*! \} */

#endif // MAINBOARD_CONF_H
