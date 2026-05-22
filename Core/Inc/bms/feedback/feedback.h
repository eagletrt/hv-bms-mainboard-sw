/*!
 * \file feedback.h
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Feedback management function
 */

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdbool.h>
#include <stdint.h>

#include "mainboard-def.h"
#include "mainboard-conf.h"
#include "eagletrt-api.h"

#include "primary_network.h"

/*! \brief Alias for the total number of feedbacks */
#define FEEDBACK_COUNT (FEEDBACK_ID_COUNT)

/*! \brief The period with which feedbacks are updated */
#define FEEDBACK_CYCLE_TIME_MS (1U)

/*! \brief Voltage reference for the 5V to the MCU and the ShutDown */
#define FEEDBACK_VREF (3.3f)
#define FEEDBACK_5V_VREF (5.f)
#define FEEDBACK_SD_VREF (12.f)

/*!
 * \brief Thresholds for the analog feedbacks in V
 *
 * \details If the voltage of a feedback is greater than the HIGH threshold it is considered
 * as logically high, if it is lower than the LOW threshold is considered low otherwise
 * it is in an implausible state and considered as error
 */
#define FEEDBACK_THRESHOLD_HIGH_V (1.9f)
#define FEEDBACK_THRESHOLD_LOW_V (0.7f)

/*!
 * \brief Convert the feedback voltage to the 5V to MCU voltage in V
 *
 * \param value The feedback voltage
 *
 * \returns volt_t The 5V voltage in V
 */
#define FEEDBACK_VOLTAGE_TO_5V_VOLT(value) ((value) * FEEDBACK_5V_VREF / FEEDBACK_VREF)

/*!
 * \brief Convert the feedback voltage to the shutdown voltage in V
 *
 * \param value The shutdown feedback voltage
 *
 * \returns volt_t The shutdown feedback voltage in V
 */
#define FEEDBACK_VOLTAGE_TO_SD_VOLT(value) ((value) * FEEDBACK_SD_VREF / FEEDBACK_VREF)

/*!
 * \brief Feedbacks states needed when the miainboard is in the IDLE state
 * and has to go to the AIRN_CHECK state
 *
 * \details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * \details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_IDLE_TO_AIRN_CHECK_HIGH         \
    (                                            \
        FEEDBACK_BIT_AIRN_OPEN_MEC |             \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC |        \
        FEEDBACK_BIT_AIRP_OPEN_MEC |             \
        FEEDBACK_BIT_TS_LESS_THAN_60V |          \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_INDICATOR_CONNECTED |       \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED |   \
        FEEDBACK_BIT_BMS_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_OK |                    \
        FEEDBACK_BIT_TSAL_GREEN |                \
        FEEDBACK_BIT_PROBING_3V3 |               \
        FEEDBACK_BIT_SD_END |                    \
        FEEDBACK_BIT_V5_MCU)

#define FEEDBACK_IDLE_TO_AIRN_CHECK_LOW   \
    (                                     \
        FEEDBACK_BIT_AIRN_OPEN_COM |      \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_COM)

/*!
 * \brief Mask for the feedbacks states needed when the miainboard is in the IDLE state
 * and has to go to the AIRN_CHECK state
 */
#define FEEDBACK_IDLE_TO_AIRN_CHECK_MASK ((FEEDBACK_IDLE_TO_AIRN_CHECK_HIGH) | (FEEDBACK_IDLE_TO_AIRN_CHECK_LOW))

/*!
 * \brief Feedbacks states needed when the miainboard is in the AIRN_CHECK state
 * and has to go to the PRECHARGE state
 *
 * \details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * \details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_HIGH    \
    (                                            \
        FEEDBACK_BIT_AIRN_OPEN_COM |             \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC |        \
        FEEDBACK_BIT_AIRP_OPEN_MEC |             \
        FEEDBACK_BIT_TS_LESS_THAN_60V |          \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_INDICATOR_CONNECTED |       \
        FEEDBACK_BIT_BMS_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_FAULT_LATCHED |         \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED |   \
        FEEDBACK_BIT_IMD_OK |                    \
        FEEDBACK_BIT_PROBING_3V3 |               \
        FEEDBACK_BIT_SD_END |                    \
        FEEDBACK_BIT_V5_MCU)
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_LOW \
    (                                        \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM |    \
        FEEDBACK_BIT_AIRP_OPEN_COM |         \
        FEEDBACK_BIT_AIRN_OPEN_MEC |         \
        FEEDBACK_BIT_TSAL_GREEN)
/*!
 * \brief Mask for the feedbacks states needed when the mainboard is in the AIRN_CHECK state
 * and has to go to the PRECHARGE state
 */
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_MASK ((FEEDBACK_AIRN_CHECK_TO_PRECHARGE_HIGH) | (FEEDBACK_AIRN_CHECK_TO_PRECHARGE_LOW))

/*!
 * \brief Feedbacks states needed when the miainboard is in the PRECHARGE state
 * and has to go to the AIRP_CHECK state
 *
 * \details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * \details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_HIGH    \
    (                                            \
        FEEDBACK_BIT_AIRN_OPEN_COM |             \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM |        \
        FEEDBACK_BIT_AIRP_OPEN_MEC |             \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_INDICATOR_CONNECTED |       \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED |   \
        FEEDBACK_BIT_BMS_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_OK |                    \
        FEEDBACK_BIT_PROBING_3V3 |               \
        FEEDBACK_BIT_SD_END |                    \
        FEEDBACK_BIT_V5_MCU)
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_LOW \
    (                                        \
        FEEDBACK_BIT_AIRP_OPEN_COM |         \
        FEEDBACK_BIT_AIRN_OPEN_MEC |         \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC |    \
        FEEDBACK_BIT_TS_LESS_THAN_60V |      \
        FEEDBACK_BIT_TSAL_GREEN)
/*!
 * \brief Mask for the feedbacks states needed when the mainboard is in the PRECHARGE state
 * and has to go to the AIRP_CHECK state
 */
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_MASK ((FEEDBACK_PRECHARGE_TO_AIRP_CHECK_HIGH) | (FEEDBACK_PRECHARGE_TO_AIRP_CHECK_LOW))

/*!
 * \brief Feedbacks states needed when the miainboard is in the AIRP_CHECK state
 * and has to go to the TS_ON state
 *
 * \details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * \details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH        \
    (                                            \
        FEEDBACK_BIT_AIRN_OPEN_COM |             \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM |        \
        FEEDBACK_BIT_AIRP_OPEN_COM |             \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED |     \
        FEEDBACK_BIT_INDICATOR_CONNECTED |       \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED |   \
        FEEDBACK_BIT_BMS_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_FAULT_LATCHED |         \
        FEEDBACK_BIT_IMD_OK |                    \
        FEEDBACK_BIT_PROBING_3V3 |               \
        FEEDBACK_BIT_SD_END |                    \
        FEEDBACK_BIT_V5_MCU)
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW  \
    (                                     \
        FEEDBACK_BIT_AIRN_OPEN_MEC |      \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC | \
        FEEDBACK_BIT_AIRP_OPEN_MEC |      \
        FEEDBACK_BIT_TS_LESS_THAN_60V |   \
        FEEDBACK_BIT_TSAL_GREEN)

/*!
 * \brief Mask for the feedbacks states needed when the mainboard is in the AIRP_CHECK state
 * and has to go to the TS_ON state
 */
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_MASK ((FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH) | (FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW))

/*!
 * \brief Feedbacks states needed needed to keep the miainboard in the TS_ON state
 *
 * \details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * \details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_TS_ON_HIGH (FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH)
#define FEEDBACK_TS_ON_LOW (FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW)
#define FEEDBACK_TS_ON_MASK (FEEDBACK_AIRP_CHECK_TO_TS_ON_MASK)

/*!
 * \brief Type definition for a callback function that should read all the digital feedbacks
 *
 * \return bit_flag32_t A bit flag with the status of each feedback
 */
typedef bit_flag32_t (*feedback_read_digital_all_callback)(void);

/*!
 * \brief Type definition for a callback function that starts the ADC conversion to gather all the feedbacks data
 */
typedef void (*feedback_start_analog_conversion_callback)(void);

/*!
 * \brief Return code for the feedback module functions
 */
enum FeedbackReturnCode {
    FEEDBACK_RC_OK,           /*!< Function executed successfully */
    FEEDBACK_RC_NULL_POINTER, /*!< A NULL pointer was given to a function */
    FEEDBACK_RC_INVALID_INDEX /*!< A given index is not valid */
};

/*!
 * \brief Type definition of the feedback identifiers
 */
enum FeedbackId : int8_t {
    FEEDBACK_ID_AIRN_OPEN_COM = 0,         /*!< Opposite of the AIR- commanded state */
    FEEDBACK_ID_PRECHARGE_OPEN_COM,        /*!< Opposite of the PRECHARGE commanded state */
    FEEDBACK_ID_AIRP_OPEN_COM,             /*!< Opposite of the AIR+ commanded state */
    FEEDBACK_ID_AIRN_OPEN_MEC,             /*!< Mechanical status of the AIR- */
    FEEDBACK_ID_PRECHARGE_OPEN_MEC,        /*!< Mechanical status of the PRECHARGE relay */
    FEEDBACK_ID_AIRP_OPEN_MEC,             /*!< Mechanical status of the AIR+ */
    FEEDBACK_ID_SD_IMD_FB,                 /*!< Shutdown IMD node feedback */
    FEEDBACK_ID_SD_BMS_FB,                 /*!< Shutdown AMS node feedback */
    FEEDBACK_ID_TS_LESS_THAN_60V,          /*!< Less than 60V on the Tractive System */
    FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED, /*!< Plausible state persisted value (Prefer this over the other two) */
    FEEDBACK_ID_PLAUSIBLE_STATE,           /*!< Raw plausible state value */
    FEEDBACK_ID_BMS_FAULT_COCKPIT_LED,     /*!< AMS cockpit LED status */
    FEEDBACK_ID_IMD_FAULT_COCKPIT_LED,     /*!< IMD cockpit LED status */
    FEEDBACK_ID_INDICATOR_CONNECTED,       /*!< Voltage indicator connected */
    FEEDBACK_ID_LATCH_RESET,               /*!< All the latches are reset */
    FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED,   /*!< Plausible state latched */
    FEEDBACK_ID_BMS_FAULT_LATCHED,         /*!< AMS fault latched */
    FEEDBACK_ID_IMD_FAULT_LATCHED,         /*!< IMD fault latched */
    FEEDBACK_ID_EXT_FAULT_LATCHED,         /*!< External fault latched */
    FEEDBACK_ID_IMD_OK,                    /*!< IMD status */
    FEEDBACK_ID_PLAUSIBLE_STATE_RC,        /*!< Plausible state after RC circuit */
    FEEDBACK_ID_TSAL_GREEN,                /*!< TSAL status */
    FEEDBACK_ID_PROBING_3V3,               /*!< 3V3 probing feedback */
    FEEDBACK_ID_SD_OUT,                    /*!< Shutdown out */
    FEEDBACK_ID_SD_IN,                     /*!< Shutdown in */
    FEEDBACK_ID_SD_END,                    /*!< Shutdown end */
    FEEDBACK_ID_V5_MCU,                    /*!< 5V line feedback */
    FEEDBACK_ID_COUNT,                     /*!< Total number of feedbacks */
    FEEDBACK_ID_UNKNOWN                    /*!< Feedback used for initialization or as return value */
};

/*!
 * \brief Type definition of the feedback bit representation
 *
 * \details Can be used to change or check bit flags, the feedback id is used as
 * the position of the bit
 */
enum FeedbackBit : int32_t {
    FEEDBACK_BIT_AIRN_OPEN_COM = (1U << FEEDBACK_ID_AIRN_OPEN_COM),                         /*!< Opposite of the AIR- commanded state (bit definition)*/
    FEEDBACK_BIT_PRECHARGE_OPEN_COM = (1U << FEEDBACK_ID_PRECHARGE_OPEN_COM),               /*!< Opposite of the PRECHARGE commanded state (bit definition)*/
    FEEDBACK_BIT_AIRP_OPEN_COM = (1U << FEEDBACK_ID_AIRP_OPEN_COM),                         /*!< Opposite of the AIR+ commanded state (bit definition)*/
    FEEDBACK_BIT_AIRN_OPEN_MEC = (1U << FEEDBACK_ID_AIRN_OPEN_MEC),                         /*!< Mechanical status of the AIR- (bit definition)*/
    FEEDBACK_BIT_PRECHARGE_OPEN_MEC = (1U << FEEDBACK_ID_PRECHARGE_OPEN_MEC),               /*!< Mechanical status of the PRECHARGE relay (bit definition)*/
    FEEDBACK_BIT_AIRP_OPEN_MEC = (1U << FEEDBACK_ID_AIRP_OPEN_MEC),                         /*!< Mechanical status of the AIR+ (bit definition)*/
    FEEDBACK_BIT_SD_IMD_FB = (1U << FEEDBACK_ID_SD_IMD_FB),                                 /*!< Shutdown IMD node feedback (bit definition)*/
    FEEDBACK_BIT_SD_BMS_FB = (1U << FEEDBACK_ID_SD_BMS_FB),                                 /*!< Shutdown AMS node feedback (bit definition)*/
    FEEDBACK_BIT_TS_LESS_THAN_60V = (1U << FEEDBACK_ID_TS_LESS_THAN_60V),                   /*!< Less than 60V on the Tractive System (bit definition)*/
    FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED), /*!< Plausible state persisted value (bit definition)*/
    FEEDBACK_BIT_PLAUSIBLE_STATE = (1U << FEEDBACK_ID_PLAUSIBLE_STATE),                     /*!< Raw plausible state value (bit definition)*/
    FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED = (1U << FEEDBACK_ID_BMS_FAULT_COCKPIT_LED),         /*!< AMS cockpit LED status (bit definition)*/
    FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED = (1U << FEEDBACK_ID_IMD_FAULT_COCKPIT_LED),         /*!< IMD cockpit LED status (bit definition)*/
    FEEDBACK_BIT_INDICATOR_CONNECTED = (1U << FEEDBACK_ID_INDICATOR_CONNECTED),             /*!< Voltage indicator connected (bit definition)*/
    FEEDBACK_BIT_LATCH_RESET = (1U << FEEDBACK_ID_LATCH_RESET),                             /*!< All the latches are reset (bit definition)*/
    FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED),     /*!< Plausible state latched (bit definition)*/
    FEEDBACK_BIT_BMS_FAULT_LATCHED = (1U << FEEDBACK_ID_BMS_FAULT_LATCHED),                 /*!< AMS fault latched (bit definition)*/
    FEEDBACK_BIT_IMD_FAULT_LATCHED = (1U << FEEDBACK_ID_IMD_FAULT_LATCHED),                 /*!< IMD fault latched (bit definition)*/
    FEEDBACK_BIT_EXT_FAULT_LATCHED = (1U << FEEDBACK_ID_EXT_FAULT_LATCHED),                 /*!< External fault latched (bit definition)*/
    FEEDBACK_BIT_IMD_OK = (1U << FEEDBACK_ID_IMD_OK),                                       /*!< IMD status (bit definition)*/
    FEEDBACK_BIT_PLAUSIBLE_STATE_RC = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_RC),               /*!< Plausible state after RC circuit (bit definition)*/
    FEEDBACK_BIT_TSAL_GREEN = (1U << FEEDBACK_ID_TSAL_GREEN),                               /*!< TSAL status (bit definition)*/
    FEEDBACK_BIT_PROBING_3V3 = (1U << FEEDBACK_ID_PROBING_3V3),                             /*!< 3V3 probing feedback (bit definition)*/
    FEEDBACK_BIT_SD_OUT = (1U << FEEDBACK_ID_SD_OUT),                                       /*!< Shutdown out (bit definition)*/
    FEEDBACK_BIT_SD_IN = (1U << FEEDBACK_ID_SD_IN),                                         /*!< Shutdown in (bit definition)*/
    FEEDBACK_BIT_SD_END = (1U << FEEDBACK_ID_SD_END),                                       /*!< Shutdown end (bit definition)*/
    FEEDBACK_BIT_V5_MCU = (1U << FEEDBACK_ID_V5_MCU),                                       /*!< 5V line feedback (bit definition)*/
};

/*!
 * \brief Bit position of the digital feedbacks inside the bit flag
 */
enum FeedbackDigitalBit : int8_t {
    FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM = 0U,        /*!< Opposite of the AIR- commanded state */
    FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM,             /*!< Opposite of the AIR+ commanded state */
    FEEDBACK_DIGITAL_BIT_SD_IMD_FB,                 /*!< Shutdown IMD node feedback */
    FEEDBACK_DIGITAL_BIT_SD_BMS_FB,                 /*!< Shutdown AMS node feedback */
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM,        /*!< Opposite of the PRECHARGE commanded state */
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC,        /*!< Mechanical status of the PRECHARGE relay */
    FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V,          /*!< Less than 60V on the Tractive System */
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED, /*!< Plausible state persisted value (Prefer this over the other two) */
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE,           /*!< Raw plausible state value */
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED,     /*!< AMS cockpit LED status */
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED,     /*!< IMD cockpit LED status */
    FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED,       /*!< Voltage indicator connected */
    FEEDBACK_DIGITAL_BIT_LATCH_RESET,               /*!< All the latches are reset */
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED,   /*!< Plausible state latched */
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED,         /*!< AMS fault latched */
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED,         /*!< IMD fault latched */
    FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED,         /*!< External fault latched */
    FEEDBACK_DIGITAL_BIT_COUNT,                     /*!< Total number of digital feedbacks */
    FEEDBACK_DIGITAL_BIT_UNKNOWN                    /*!< Feedback used for initialization or as return value */
};

/*!
 * \brief Indices of the analog feedbacks
 */
enum FeedbackAnalogIndex : int8_t {
    FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC = 0U, /*!< Mechanical status of the AIR- */
    FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC,      /*!< Mechanical status of the AIR+ */
    FEEDBACK_ANALOG_INDEX_IMD_OK,             /*!< Status of the IMD */
    FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC, /*!< Plausible state after the RC circuit */
    FEEDBACK_ANALOG_INDEX_TSAL_GREEN,         /*!< Status of the TSAL */
    FEEDBACK_ANALOG_INDEX_PROBING_3V3,        /*!< Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V) */
    FEEDBACK_ANALOG_INDEX_SD_OUT,             /*!< Shutdown out */
    FEEDBACK_ANALOG_INDEX_SD_IN,              /*!< Shutdown in */
    FEEDBACK_ANALOG_INDEX_SD_END,             /*!< Shutdown end */
    FEEDBACK_ANALOG_INDEX_V5_MCU,             /*!< Feedback on the 5V line */
    FEEDBACK_ANALOG_INDEX_COUNT,              /*!< Total number of analog feedbacks */
    FEEDBACK_ANALOG_INDEX_UNKNOWN             /*!< Feedback used for initialization or as return value */
};

/*!
 * \brief Status of the feedbacks
 */
enum FeedbackStatus : int8_t {
    FEEDBACK_STATUS_LOW,   /*!< The feedback value is considered as logically low */
    FEEDBACK_STATUS_ERROR, /*!< The feedback value is not in a valid state */
    FEEDBACK_STATUS_HIGH   /*!< The feedback value is considered as logically high */
};

/*!
 * \brief Type definition for the internal feedback handler structure
 *
 * \attention This struct should not be used outside of this module
 */
struct FeedbackHandler {
    feedback_read_digital_all_callback read_digital;            /*!< Pointer to the function used to read all the digital feedbacks */
    feedback_start_analog_conversion_callback start_conversion; /*!< Pointer to the function used to start the converison of the analog feedbacks */

    bit_flag32_t digital;                       /*!< Bit flag where each bit represent a specific feedback state */
    volt_t analog[FEEDBACK_ANALOG_INDEX_COUNT]; /*!< Array of raw voltages of the analog feedbacks */

    enum FeedbackStatus status[FEEDBACK_COUNT]; /*!< Array of all the feedbacks current status */

    primary_hv_feedback_status_converted_t status_can_payload;       /*!< CAN payload of the feedbacks status */
    primary_hv_feedback_digital_converted_t digital_can_payload;     /*!< CAN payload of the digital feedbacks values */
    primary_hv_feedback_analog_converted_t analog_can_payload;       /*!< CAN payload of the analog feedbacks values */
    primary_hv_feedback_analog_sd_converted_t analog_sd_can_payload; /*!< CAN payload of the analog shutdown feedbacks values */
    primary_hv_feedback_enzomma_converted_t enzomma_can_payload;     /*!< CAN payload of the feedback that did not allow the BMS to go the TS ON state */
};

#endif // FEEDBACK_H
