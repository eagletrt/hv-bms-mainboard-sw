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
 * \defgroup phc-conf
 * \brief Configuration of the Pack Health Controller (PHC)
 * \{
 */

/*!
 * \brief Thresholds for the voltages PHC checks
 */
#define PHC_MIN_CELL_VOLTAGE_V (0)
#define PHC_MAX_CELL_VOLTAGE_V (0)

/*!
 * \brief Thresholds for the temperatures PHC checks
 */
#define PHC_MIN_CELL_TEMPERATURE_C (0)
#define PHC_MAX_CELL_TEMPERATURE_C (0)

/*!
 * \brief Thresholds for the discharge resistor temperature PHC checks
 */
#define PHC_MIN_DISCHARGE_TEMPERATURE_C (0)
#define PHC_MAX_DISCHARGE_TEMPERATURE_C (0)

/*!
 * \brief Thresholds for the cooling system temperature PHC checks
 */
#define PHC_MIN_COOLING_TEMPERATURE_C (0)
#define PHC_MAX_COOLING_TEMPERATURE_C (0)

/*!
 * \brief Thresholds for the linear regulator temperature PHC checks
 */
#define PHC_MIN_LINEAR_REGULATOR_TEMPERATURE_C (0)
#define PHC_MAX_LINEAR_REGULATOR_TEMPERATURE_C (0)

/*!
 * \brief Thresholds for the precharge temperature PHC checks
 */
#define PHC_MIN_PRECHARGE_TEMPERATURE_C (0)
#define PHC_MAX_PRECHARGE_TEMPERATURE_C (0)

/*!
 * \brief Thresholds for the current PHC checks
 * \details The current is signed, the minimum is the limit in the opposite direction
 */
#define PHC_MIN_CURRENT_A (0)
#define PHC_MAX_CURRENT_A (0)

/*!
 * \brief Thresholds for the power PHC checks, in kW like internal_voltages_api_get_power
 */
#define PHC_MAX_POWER_KW (0)

/*! 
 * \brief Thresholds for the precharge complete PHC check
 */
#define PHC_PRECHARGE_VOLTAGE_V (0)

/*!
 * \brief Maximum interval between two consecutive runs of a PHC check in ms
 *
 * \details If a check does not run within this time after its last run, the
 *          PHC fault is set
 */
#define PHC_MAX_CHECK_INTERVAL_MS (50U)

/*!
 * \brief Time from the PHC init within which every PHC check must have run once, in ms
 *
 * \details If a check has not run within this time (e.g. its data never arrived),
 *          the PHC fault is set
 */
#define PHC_MAX_STARTUP_INTERVAL_MS (500U)

/*! \} */

/*!* ######################### SENSING ################################### *!*/

/*!
 * \defgroup sensing-conf
 * \brief Configuration of the sensing interface
 * \{
 */

/*!
 * \brief Maximum interval in ms between two consecutive processed conversions of an MCU ADC
 *
 * \details Measured from the processing of the samples of a conversion to the
 *          processing of the next one, so it covers both a conversion that is
 *          never completed (e.g. the DMA complete interrupt never fired) and a
 *          conversion that cannot be started. When it elapses the data of that
 *          ADC is considered stale.
 *          The routine also reads the external ADC with a blocking SPI transfer
 *          that can stall up to ~55 ms (1 + 10 bytes at 5 ms each), the timeout
 *          must stay above it so that a stalled transfer does not latch this fault
 */
#define SENSING_MAX_ADC_CONVERSION_TIME_MS (100U)

/*!
 * \brief Maximum interval in ms between two consecutive successful reads of the external ADC
 *
 * \details The external ADC is read at every sensing routine, the timeout
 *          tolerates a few transient SPI failures and one stalled blocking
 *          transfer (~55 ms). If no read succeeds within this time the external
 *          ADC data is considered stale
 */
#define SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS (100U)

/*!
 * \brief Maximum interval in ms between two consecutive IMD PWM captures
 *
 * \details The slowest valid IMD signal is 10 Hz (100 ms period), the timeout
 *          allows a few missed periods. The watchdog is started by the first
 *          capture, so the startup of the IMD is not timed. Once started, if no
 *          capture arrives within this time (e.g. the signal is cut or the IMD
 *          is dead) the IMD data is considered stale
 */
#define SENSING_MAX_IMD_UPDATE_MS (300U)

/*! \} */

/*!* ######################### CAN ####################################### *!*/

/*!
 * \defgroup can-conf
 * \brief Configuration of the CAN communication
 * \{
 */

/*!
 * \brief Maximum number of consecutive messages that can be dropped
 *
 * \details CELLBOARD applies to the messages sent by the cellboards, IVT to
 *          the ones sent by the IVT, PRIMARY to the ones received on the
 *          primary network
 */
#define CAN_MAX_CELLBOARD_DROPPED (3U)
#define CAN_MAX_IVT_DROPPED (3U)
#define CAN_MAX_PRIMARY_DROPPED (3U)

/*!
 * \brief Maximum number of system critical messages that can be dropped consecutively
 */
#define CAN_MAX_CELLBOARD_DROPPED_SCS (2U)
#define CAN_MAX_IVT_DROPPED_SCS (2U)
#define CAN_MAX_PRIMARY_DROPPED_SCS (2U)

/*!
 * \brief Cycle time of the IVT current message in ms
 *
 * \details libcan marks the IVT messages as one-shot, the actual period is
 *          the one configured on the IVT
 */
#define CAN_IVT_CYCLE_TIME_MS (10U)

/*!
 * \brief Wiggle room in ms added to the cycle time of every received message
 *
 * \details Gives some leeway to account for jitter on the bus and on the sender
 *          before a message is considered dropped
 *
 * \warning Must be smaller than the shortest cycle time (CAN_IVT_CYCLE_TIME_MS)
 */
#define CAN_CYCLE_TIME_WIGGLE_ROOM_MS (5U)

/*!
 * \brief Length in ms of the timeframe over which the dropped messages are counted
 */
#define CAN_TIMEFRAME (1000U)

/*! \} */

/*!* ######################### FLASH ##################################### *!*/

/*!
 * \defgroup flash-conf
 * \brief Configuration of the flash procedure control
 * \{
 */

/*!
 * \brief Maximum interval in ms between two consecutive flashing messages
 *
 * \details Every flashing message restarts the timeout. If no flashing message
 *          is received within this time the flash request is dropped (target set
 *          to none).
 */
#define FLASH_CONTROL_MAX_MESSAGE_INTERVAL_MS (1000U)

/*! \} */

#endif // MAINBOARD_CONF_H
