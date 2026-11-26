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
#define FEEDBACK_BUFFER_SIZE (30U)

/*!
 * \brief Thresholds for the analog feedback in V
 *
 * \details Above HIGH the feedback is logically high, below LOW it is logically
 *          low, in between it is in an implausible (error) state
 */
#define FEEDBACK_THRESHOLD_HIGH_V (1.0F)
#define FEEDBACK_THRESHOLD_LOW_V (0.8F)

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

/*!
 * \brief Time in ms an analog feedback can stay in error before the fault is set
 *
 * \details A feedback crosses the error band at every edge (e.g. the SD closing
 *          or sagging when the AIRs close), only an error that lasts longer is a fault.
 *          The status turns to error after the majority of FEEDBACK_BUFFER_SIZE samples,
 *          so the total delay is about half the buffer plus this time
 */
#define FEEDBACK_MAX_ERROR_TIME_MS (100U)

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
#define PHC_MIN_CELL_VOLTAGE_V (2.6F)
#define PHC_MAX_CELL_VOLTAGE_V (4.2F)

/*!
 * \brief Thresholds for the temperatures PHC checks
 *
 * \details Same as TEMP_MIN_C and TEMP_MAX_C of the old firmware
 */
#define PHC_MIN_CELL_TEMPERATURE_C (-10.F)
#define PHC_MAX_CELL_TEMPERATURE_C (56.F)

/*!
 * \brief Thresholds for the discharge resistor temperature PHC checks
 *
 * \todo No reference value: the old firmware and the cellboards never checked
 *       these temperatures. Confirm the maximum with the cellboard hardware
 */
#define PHC_MIN_DISCHARGE_TEMPERATURE_C (-10.F)
#define PHC_MAX_DISCHARGE_TEMPERATURE_C (100.F)

/*!
 * \brief Thresholds for the cooling system temperature PHC checks
 *
 * \details Same as COOLING_TEMP_MIN_C and COOLING_TEMP_MAX_C of the old firmware
 *          (where the check was disabled). An unplugged NTC reads about -12.6 C
 *          and a shorted one about 163 C, so both latch the fault
 */
#define PHC_MIN_COOLING_TEMPERATURE_C (-10.F)
#define PHC_MAX_COOLING_TEMPERATURE_C (60.F)

/*!
 * \brief Thresholds for the linear regulator temperature PHC checks
 *
 * \todo No reference value, and the sensing conversion is still a placeholder
 *       (SENSING_LINEAR_REGULATOR_TEMP_GAIN_C_PER_V), so the check is not effective yet
 */
#define PHC_MIN_LINEAR_REGULATOR_TEMPERATURE_C (-10.F)
#define PHC_MAX_LINEAR_REGULATOR_TEMPERATURE_C (100.F)

/*!
 * \brief Thresholds for the precharge temperature PHC checks
 *
 * \todo No reference value, and the sensing conversion is still a placeholder
 *       (SENSING_PRECHARGE_TEMP_GAIN_C_PER_V), so the check is not effective yet
 */
#define PHC_MIN_PRECHARGE_TEMPERATURE_C (-10.F)
#define PHC_MAX_PRECHARGE_TEMPERATURE_C (100.F)

/*!
 * \brief Thresholds for the current PHC checks
 * \details The current is signed, the minimum is the limit in the opposite direction.
 *          Same as CURRENT_MIN_A and CURRENT_MAX_A of the old firmware
 */
#define PHC_MIN_CURRENT_A (-22.F)
#define PHC_MAX_CURRENT_A (130.F)

/*!
 * \brief Thresholds for the power PHC checks, in kW like internal_voltages_api_get_power
 *
 * \details Rules limit, same as CURRENT_MAX_POWER_KW of the old firmware
 */
#define PHC_MAX_POWER_KW (80.F)

/*!
 * \brief Threshold for the precharge complete PHC check, as a ratio of TS voltage over pack voltage
 *
 * \details Same as PCU_PRECHARGE_THRESHOLD_PERCENT of the old firmware
 */
#define PHC_PRECHARGE_THRESHOLD_RATIO (0.95F)

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
 *          the PHC fault is set. Must cover the boot of the cellboards and of the
 *          IVT plus the slowest cycle time of the data the checks need (500 ms)
 */
#define PHC_MAX_STARTUP_INTERVAL_MS (2000U)

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
/*! \brief 50 * 10 ms = 500 ms, same as CURRENT_SENSOR_COMMUNICATION_TIMEOUT_MS of the old firmware */
#define CAN_MAX_IVT_DROPPED (50U)
/*! \brief 6 * 500 ms = 3 s for the balancing message, same as BAL_TIMEOUT_MS of the old firmware */
#define CAN_MAX_PRIMARY_DROPPED (6U)

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

/*!* ######################### CAN WATCHDOGS ############################# *!*/

/*!
 * \defgroup can-watchdogs-conf
 * \brief Enable the monitoring of the dropped messages, one switch per message group
 *
 * \details 1 to monitor the group, 0 to skip it. A monitored message is timed
 *          from its first reception: after the maximum number of consecutive
 *          drops its fault is set (BMS network) or its request is cleared
 *          (primary network). A group that is never sent must be disabled,
 *          or its fault would be set as soon as it is dropped
 * \{
 */

/*! \brief IVT current, sets STALE_CURRENT */
#define IVT_CURRENT_ENABLE_WATCHDOGS (1)

/*! \brief Cellboard cell voltages and their info, set STALE_CELL_VOLTAGES */
#define CELLBOARD_VOLTAGES_ENABLE_WATCHDOGS (1)
#define CELLBOARD_VOLTAGES_INFO_ENABLE_WATCHDOGS (1)

/*! \brief Cellboard cell temperatures and their info, set STALE_CELL_TEMPERATURES */
#define CELLBOARD_TEMPERATURES_ENABLE_WATCHDOGS (1)
#define CELLBOARD_TEMPERATURES_INFO_ENABLE_WATCHDOGS (1)

/*!
 * \brief Cellboard discharge resistor temperatures (BALANCINGTEMPERATURE), set STALE_CELL_TEMPERATURES
 *
 * \details Disabled: not sent by the cellboard firmware yet
 */
#define CELLBOARD_DISCHARGE_TEMPERATURES_ENABLE_WATCHDOGS (0)

/*! \brief Cellboard balancing status, FSM state and errors, set STALE_CELLBOARD_STATUS */
#define CELLBOARD_BALANCING_STATUS_ENABLE_WATCHDOGS (1)
#define CELLBOARD_FSM_ENABLE_WATCHDOGS (1)
#define CELLBOARD_ERRORS_ENABLE_WATCHDOGS (1)

/*!
 * \brief Cellboard firmware and libcan versions, set STALE_CELLBOARD_STATUS
 *
 * \details Disabled: not sent by the cellboard firmware yet
 */
#define CELLBOARD_VERSION_ENABLE_WATCHDOGS (0)
#define CELLBOARD_VERSION_INFO_ENABLE_WATCHDOGS (0)
#define CELLBOARD_LIBCAN_VERSION_ENABLE_WATCHDOGS (0)
#define CELLBOARD_LIBCAN_VERSION_INFO_ENABLE_WATCHDOGS (0)

/*! \brief TS request from the primary network, the TS off request is applied when dropped */
#define PRIMARY_BMS_SET_ENABLE_WATCHDOGS (0)

/*! \brief Balancing request from the primary network, the balancing stop request is applied when dropped */
#define PRIMARY_BALANCING_SET_ENABLE_WATCHDOGS (1)

/*! \} */

/*!* ######################### BALANCING ################################# *!*/

/*!
 * \defgroup bal-control-conf
 * \brief Configuration of the balancing control
 * \{
 */

/*!
 * \brief Range in V the requested balancing threshold is clamped to
 *
 * \details Same as BAL_THRESHOLD_MIN_V and BAL_THRESHOLD_MAX_V of the old firmware
 */
#define BAL_CONTROL_THRESHOLD_MIN_V (0.005F)
#define BAL_CONTROL_THRESHOLD_MAX_V (0.200F)

/*!
 * \brief Range in V the balancing target (minimum cell voltage) is clamped to
 *
 * \details No cell is discharged below the minimum, same as BAL_TARGET_MIN_V
 *          and BAL_TARGET_MAX_V of the old firmware
 */
#define BAL_CONTROL_TARGET_MIN_V (2.8F)
#define BAL_CONTROL_TARGET_MAX_V (4.2F)

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

/*!* ######################### FSM ####################################### *!*/

/*!
 * \defgroup fsm-conf
 * \brief Configuration of the mainboard FSM
 * \{
 */

/*!
 * \brief Maximum time in ms allowed for the checks before closing the AIR- to pass
 *
 * \details Counted from the entry in WAIT_OK. If the keep TS on request is not
 *          received or the conditions do not match within this time the FSM
 *          goes to DISCHARGE_TS
 */
#define MAINBOARD_FSM_WAIT_OK_TIMEOUT_MS (1000U)

/*!
 * \brief TEMPORARY: let WAIT_OK continue on the TS on request instead of waiting for keep TS on
 *
 * \details The BmsSet message of libcan only carries on/off, so the keep TS on
 *          request is never received and WAIT_OK always times out. With this
 *          enabled WAIT_OK only checks the conditions, like the old firmware did
 *          in IDLE. Set to 0 once libcan has the keep state
 */
#define MAINBOARD_FSM_WAIT_OK_ACCEPT_TS_ON (1)

/*!
 * \brief Maximum time in ms allowed for the AIR- to close
 *
 * \details Counted from the entry in AIRN_CHECK. If the AIR- is not closed
 *          within this time the FSM goes to DISCHARGE_TS
 */
#define MAINBOARD_FSM_AIRN_TIMEOUT_MS (1000U)

/*!
 * \brief Maximum time in ms allowed for the precharge to complete
 *
 * \details Counted from the entry in PRECHARGE_CHECK. If the precharge is not
 *          complete within this time the FSM goes to DISCHARGE_TS
 */
#define MAINBOARD_FSM_PRECHARGE_TIMEOUT_MS (15000U)

/*!
 * \brief Maximum time in ms allowed for the AIR+ to close
 *
 * \details Counted from the entry in AIRP_CHECK. If the AIR+ is not closed and
 *          the precharge relay is not open within this time the FSM goes to DISCHARGE_TS
 */
#define MAINBOARD_FSM_AIRP_TIMEOUT_MS (1000U)

/*!
 * \brief Maximum time in ms allowed for the TS to discharge
 *
 * \details Counted from the entry in DISCHARGE_TS. If the relays are not open
 *          and the TS is not below 60V within this time the FSM goes to FATAL
 *          (the rules require the TS below 60V within 5s from the AIRs opening)
 */
#define MAINBOARD_FSM_DISCHARGE_TIMEOUT_MS (5000U)

/*!
 * \brief The time that the mainboard gives to the cells to settle
 * after a discharge cycle before reading the voltages again, in ms
 */
#define MAINBOARD_FSM_BALANCING_READ_TIMEOUT_MS (2000U)

/*!
 * \brief The time that the mainboard gives to the cells to discharge
 * before reading the voltages again, in ms
 */
#define MAINBOARD_FSM_BALANCING_DISCHARGE_TIMEOUT_MS (10000U)

/*! \} */

/*!* ######################### TASKS ##################################### *!*/

/*!
 * \defgroup tasks-conf
 * \brief Configuration of the mainboard tasks
 * \{
 */

/*!
 * \brief Period in ms of the sensing acquisition
 *
 * \warning Must be smaller than SENSING_MAX_ADC_CONVERSION_TIME_MS and
 *          SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS
 */
#define MAINBOARD_TASKS_SENSING_PERIOD_MS (1U)

/*!
 * \brief Period in ms of the update of the conditions from the feedbacks
 */
#define MAINBOARD_TASKS_FEEDBACK_PERIOD_MS (1U)

/*!
 * \brief Period in ms of the mainboard FSM run
 *
 * \details Adds to the resolution of every FSM timeout
 */
#define MAINBOARD_TASKS_FSM_PERIOD_MS (1U)

/*!
 * \brief Period in ms of the processing of the CAN TX and RX queues
 *
 * \warning Must be smaller than the shortest cycle time (CAN_IVT_CYCLE_TIME_MS)
 */
#define MAINBOARD_TASKS_CAN_PERIOD_MS (1U)

/*!
 * \brief Period in ms of the CAN transceivers monitor, which logs their state changes and errors
 *
 * \details The bus errors are sampled at this period, a shorter one sees more of them
 */
#define MAINBOARD_TASKS_CAN_MONITOR_PERIOD_MS (10U)

/*!
 * \brief Minimum time in ms between two logs of the bus state of a CAN network
 *
 * \details A transceiver that keeps going bus-off and recovering changes state every few ms,
 *          the changes that come too close to the last log are summarized at the next one
 */
#define MAINBOARD_TASKS_CAN_STATE_LOG_INTERVAL_MS (1000U)

/*!
 * \brief Period in ms of every PHC check
 *
 * \warning Must be smaller than PHC_MAX_CHECK_INTERVAL_MS
 */
#define MAINBOARD_TASKS_PHC_PERIOD_MS (10U)

/*!
 * \brief Period in ms of the heartbeat LED toggle
 */
#define MAINBOARD_TASKS_HEARTBEAT_PERIOD_MS (1000U)

/*!
 * \brief Period in ms of the display update
 *
 * \warning Must be smaller than MAINBOARD_TASKS_DISPLAY_FRAME_MS, or frames are skipped
 */
#define MAINBOARD_TASKS_DISPLAY_PERIOD_MS (10U)

/*!
 * \brief Duration in ms of a single frame of the display animations
 */
#define MAINBOARD_TASKS_DISPLAY_FRAME_MS (100U)

/*!
 * \brief Period in ms of the logger console (commands and reports)
 *
 * \details At most MAINBOARD_LOGGER_REPORT_LINES_PER_RUN report lines are sent per run
 */
#define MAINBOARD_TASKS_LOGGER_PERIOD_MS (5U)

static_assert(MAINBOARD_TASKS_SENSING_PERIOD_MS < SENSING_MAX_ADC_CONVERSION_TIME_MS, "The ADCs must be handled within SENSING_MAX_ADC_CONVERSION_TIME_MS");
static_assert(MAINBOARD_TASKS_SENSING_PERIOD_MS < SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS, "The external ADC must be read within SENSING_MAX_EXTERNAL_ADC_READ_INTERVAL_MS");
static_assert(MAINBOARD_TASKS_DISPLAY_PERIOD_MS < MAINBOARD_TASKS_DISPLAY_FRAME_MS, "The display must be updated at least once per frame");
static_assert(MAINBOARD_TASKS_CAN_STATE_LOG_INTERVAL_MS % MAINBOARD_TASKS_CAN_MONITOR_PERIOD_MS == 0U, "The CAN state log interval must be a multiple of the monitor period");
static_assert(MAINBOARD_TASKS_CAN_PERIOD_MS < CAN_IVT_CYCLE_TIME_MS, "The CAN queues must be processed faster than the shortest cycle time");
static_assert(MAINBOARD_TASKS_PHC_PERIOD_MS < PHC_MAX_CHECK_INTERVAL_MS, "The PHC checks must run within PHC_MAX_CHECK_INTERVAL_MS");
static_assert(MAINBOARD_TASKS_FEEDBACK_PERIOD_MS < FEEDBACK_MAX_ERROR_TIME_MS, "The feedback errors must be checked within FEEDBACK_MAX_ERROR_TIME_MS");

/*! \} */

/*!* ######################### LOGGER #################################### *!*/

/*!
 * \defgroup logger-conf
 * \brief Configuration of the logger and of its serial console
 * \{
 */

/*!
 * \brief Enable the console commands that act on the car (TS on/off, balancing, reset)
 *
 * \details They go through the request control like the CAN requests, so the FSM
 *          checks still apply. Set to 0 to have a read-only console
 */
#define MAINBOARD_LOGGER_ENABLE_ACTIONS (1)

/*!
 * \brief Space in bytes left free in the UART TX buffer while printing a report
 *
 * \details The reports are paced line by line so that the live logs are never
 *          dropped because a report filled the buffer
 */
#define MAINBOARD_LOGGER_REPORT_TX_HEADROOM (256U)

/*!
 * \brief Maximum number of report lines sent in a single run of the console
 */
#define MAINBOARD_LOGGER_REPORT_LINES_PER_RUN (4U)

/*!
 * \brief Balancing threshold in V requested by the balancing console command
 *
 * \details Clamped by the balancing control between BAL_CONTROL_THRESHOLD_MIN_V and BAL_CONTROL_THRESHOLD_MAX_V
 */
#define MAINBOARD_LOGGER_BALANCING_THRESHOLD_V (BAL_CONTROL_THRESHOLD_MIN_V)

/*!
 * \brief Time in ms within which the reset key must be pressed again to confirm the reset
 */
#define MAINBOARD_LOGGER_RESET_CONFIRM_MS (3000U)

/*!
 * \brief Delay in ms between the confirmed reset and the actual reset
 *
 * \details Leaves the time to send the last logs over the UART
 */
#define MAINBOARD_LOGGER_RESET_DELAY_MS (100U)

/*! \} */

#endif // MAINBOARD_CONF_H
