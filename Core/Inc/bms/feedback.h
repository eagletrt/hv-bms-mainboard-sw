/**
 * @file feedback.c
 * @date 2024-08-10
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Feedback management function
 */

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdbool.h>

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "primary_network.h"

/** @brief Alias for the total number of feedbacks */
#define FEEDBACK_COUNT (FEEDBACK_ID_COUNT)

/** @brief The period with which feedbacks are updated */
#define FEEDBACK_CYCLE_TIME_MS (1U)

/** @brief Voltage reference for the 5V to the MCU and the ShutDown */
#define FEEDBACK_VREF (3.3f)
#define FEEDBACK_5V_VREF (5.f)
#define FEEDBACK_SD_VREF (12.f)

/**
 * @brief Thresholds for the analog feedbacks in V
 *
 * @details If the voltage of a feedback is greater than the HIGH threshold it is considered
 * as logically high, if it is lower than the LOW threshold is considered low otherwise
 * it is in an implausible state and considered as error
 */
#define FEEDBACK_THRESHOLD_HIGH_V (1.9f)
#define FEEDBACK_THRESHOLD_LOW_V (0.7f)

/**
 * @brief Convert the feedback voltage to the 5V to MCU voltage in V
 *
 * @param value The feedback voltage
 *
 * @return volt_t The 5V voltage in V
 */
#define FEEDBACK_VOLTAGE_TO_5V_VOLT(value) ((value) * FEEDBACK_5V_VREF / FEEDBACK_VREF)

/**
 * @brief Convert the feedback voltage to the shutdown voltage in V
 *
 * @param value The shutdown feedback voltage
 *
 * @return volt_t The shutdown feedback voltage in V
 */
#define FEEDBACK_VOLTAGE_TO_SD_VOLT(value) ((value) * FEEDBACK_SD_VREF / FEEDBACK_VREF)

/**
 * @brief Feedbacks states needed when the miainboard is in the IDLE state
 * and has to go to the AIRN_CHECK state
 *
 * @details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * @details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_IDLE_TO_AIRN_CHECK_HIGH \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_MEC | \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC | \
        FEEDBACK_BIT_AIRP_OPEN_MEC | \
        FEEDBACK_BIT_TS_LESS_THAN_60V | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_TSAL_GREEN | \
        FEEDBACK_BIT_PROBING_3V3 | \
        FEEDBACK_BIT_SD_END | \
        FEEDBACK_BIT_V5_MCU \
    )
#define FEEDBACK_IDLE_TO_AIRN_CHECK_LOW \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_COM | \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_COM \
    )
#define FEEDBACK_IDLE_TO_AIRN_CHECK_MASK ((FEEDBACK_IDLE_TO_AIRN_CHECK_HIGH) | (FEEDBACK_IDLE_TO_AIRN_CHECK_LOW))

/**
 * @brief Feedbacks states needed when the miainboard is in the AIRN_CHECK state
 * and has to go to the PRECHARGE state
 *
 * @details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * @details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_HIGH \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_COM | \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC | \
        FEEDBACK_BIT_AIRP_OPEN_MEC | \
        FEEDBACK_BIT_TS_LESS_THAN_60V | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_PROBING_3V3 | \
        FEEDBACK_BIT_SD_END | \
        FEEDBACK_BIT_V5_MCU \
    )
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_LOW \
    ( \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_COM | \
        FEEDBACK_BIT_AIRN_OPEN_MEC | \
        FEEDBACK_BIT_TSAL_GREEN \
    )
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_MASK ((FEEDBACK_AIRN_CHECK_TO_PRECHARGE_HIGH) | (FEEDBACK_AIRN_CHECK_TO_PRECHARGE_LOW))

/**
 * @brief Feedbacks states needed when the miainboard is in the PRECHARGE state
 * and has to go to the AIRP_CHECK state
 *
 * @details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * @details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_HIGH \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_COM | \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_MEC | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_PROBING_3V3 | \
        FEEDBACK_BIT_SD_END | \
        FEEDBACK_BIT_V5_MCU \
    )
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_LOW \
    ( \
        FEEDBACK_BIT_AIRP_OPEN_COM | \
        FEEDBACK_BIT_AIRN_OPEN_MEC | \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC | \
        FEEDBACK_BIT_TS_LESS_THAN_60V | \
        FEEDBACK_BIT_TSAL_GREEN \
    )
#define FEEDBACK_PRECHARGE_TO_AIRP_CHECK_MASK ((FEEDBACK_PRECHARGE_TO_AIRP_CHECK_HIGH) | (FEEDBACK_PRECHARGE_TO_AIRP_CHECK_LOW))

/**
 * @brief Feedbacks states needed when the miainboard is in the AIRP_CHECK state
 * and has to go to the TS_ON state
 *
 * @details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * @details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_COM | \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_COM | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_PROBING_3V3 | \
        FEEDBACK_BIT_SD_END | \
        FEEDBACK_BIT_V5_MCU \
    )
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW \
    ( \
        FEEDBACK_BIT_AIRN_OPEN_MEC | \
        FEEDBACK_BIT_PRECHARGE_OPEN_MEC | \
        FEEDBACK_BIT_AIRP_OPEN_MEC | \
        FEEDBACK_BIT_TS_LESS_THAN_60V | \
        FEEDBACK_BIT_TSAL_GREEN \
    )
#define FEEDBACK_AIRP_CHECK_TO_TS_ON_MASK ((FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH) | (FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW))

/**
 * @brief Feedbacks states needed needed to keep the miainboard in the TS_ON state
 *
 * @details The mask is used to select the feedbacks that have to be checked
 * The high and low macros are the expected states for each feedback, for example if one
 * feedback is put inside the high macro its value is expected to be logically high
 * 
 * @details Those macros are bitflags where each bit represent the state of a single feedback
 */
#define FEEDBACK_TS_ON_HIGH (FEEDBACK_AIRP_CHECK_TO_TS_ON_HIGH)
#define FEEDBACK_TS_ON_LOW (FEEDBACK_AIRP_CHECK_TO_TS_ON_LOW)
#define FEEDBACK_TS_ON_MASK (FEEDBACK_AIRP_CHECK_TO_TS_ON_MASK)

/**
 * @brief Type definition for a callback function that should read all the digital feedbacks
 *
 * @return bit_flag32_t A bit flag with the status of each feedback
 */
typedef bit_flag32_t (* feedback_read_digital_all_callback_t)(void);

/**
 * @brief Type definition for a callback function that starts the ADC conversion to gather all the feedbacks data
 */
typedef void (* feedback_start_analog_conversion_callback_t)(void);

/**
 * @brief Return code for the feedback module functions
 *
 * @details
 *     - FEEDBACK_OK the function executed succesfully
 *     - FEEDBACK_NULL_POINTER a NULL pointer was given to a function
 *     - FEEDBACK_INVALID_INDEX if a given index is not valid
 */
typedef enum {
    FEEDBACK_OK,
    FEEDBACK_NULL_POINTER,
    FEEDBACK_INVALID_INDEX
} FeedbackReturnCode;

/**
 * @brief Type definition of the feedback identifiers
 *
 * @details
 *     - FEEDBACK_ID_AIRN_COM Opposite of the AIR- commanded state
 *     - FEEDBACK_ID_PRECHARGE_OPEN_COM Opposite of the PRECHARGE commanded state
 *     - FEEDBACK_ID_AIRP_COM Opposite of the AIR+ commanded state
 *     - FEEDBACK_ID_AIRN_MEC Mechanical status of the AIR-
 *     - FEEDBACK_ID_PRECHARGE_OPEN_MEC Mechanical status of the PRECHARGE relay
 *     - FEEDBACK_ID_AIRP_MEC Mechanical status of the AIR+
 *     - FEEDBACK_ID_SD_IMD_FB Shutdown IMD node feedback
 *     - FEEDBACK_ID_SD_BMS_FB Shutdown AMS node feedback
 *     - FEEDBACK_ID_TS_LESS_THAN_60V Less than 60V on the Tractive System
 *     - FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED Plausible state persisted value (Prefer this over the other two)
 *     - FEEDBACK_ID_PLAUSIBLE_STATE Raw plausible state value
 *     - FEEDBACK_ID_BMS_FAULT_COCKPIT_LED AMS cockpit LED status
 *     - FEEDBACK_ID_IMD_FAULT_COCKPIT_LED IMD cockpit LED status
 *     - FEEDBACK_ID_IMD_OK Status of the IMD
 *     - FEEDBACK_ID_INDICATOR_CONNECTED Voltage indicator connected
 *     - FEEDBACK_ID_LATCH_RESET All the latches are reset
 *     - FEEDBACK_ID_PLAUSIBLE_STATE_RC Plausible state after the RC circuit
 *     - FEEDBACK_ID_TSAL_GREEN Status of the TSAL
 *     - FEEDBACK_ID_PROBING_3V3 Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V)
 *     - FEEDBACK_ID_SD_OUT Shutdown out
 *     - FEEDBACK_ID_SD_IN Shutdown in
 *     - FEEDBACK_ID_SD_END Shutdown end
 *     - FEEDBACK_ID_V5_MCU Feedback on the 5V line
 *     - FEEDBACK_ID_UNKNOWN Feedback used for initialization or as return value
 */
typedef enum {
    FEEDBACK_ID_AIRN_OPEN_COM = 0,
    FEEDBACK_ID_PRECHARGE_OPEN_COM,
    FEEDBACK_ID_AIRP_OPEN_COM, 
    FEEDBACK_ID_AIRN_OPEN_MEC,
    FEEDBACK_ID_PRECHARGE_OPEN_MEC,
    FEEDBACK_ID_AIRP_OPEN_MEC, 
    FEEDBACK_ID_SD_IMD_FB,
    FEEDBACK_ID_SD_BMS_FB,
    FEEDBACK_ID_TS_LESS_THAN_60V,
    FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED,
    FEEDBACK_ID_PLAUSIBLE_STATE,
    FEEDBACK_ID_BMS_FAULT_COCKPIT_LED,
    FEEDBACK_ID_IMD_FAULT_COCKPIT_LED,
    FEEDBACK_ID_INDICATOR_CONNECTED,
    FEEDBACK_ID_LATCH_RESET,
    FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED,
    FEEDBACK_ID_BMS_FAULT_LATCHED,
    FEEDBACK_ID_IMD_FAULT_LATCHED,
    FEEDBACK_ID_EXT_FAULT_LATCHED,
    FEEDBACK_ID_IMD_OK,
    FEEDBACK_ID_PLAUSIBLE_STATE_RC,
    FEEDBACK_ID_TSAL_GREEN,
    FEEDBACK_ID_PROBING_3V3,
    FEEDBACK_ID_SD_OUT,
    FEEDBACK_ID_SD_IN,
    FEEDBACK_ID_SD_END,
    FEEDBACK_ID_V5_MCU,
    FEEDBACK_ID_COUNT,
    FEEDBACK_ID_UNKNOWN
} FeedbackId;

/**
 * @brief Type definition of the feedback bit representation
 *
 * @details Can be used to change or check bit flags, the feedback id is used as
 * the position of the bit
 *
 * @details
 *     - FEEDBACK_BIT_AIRN_COM Opposite of the AIR- commanded state
 *     - FEEDBACK_BIT_PRECHARGE_OPEN_COM Opposite of the PRECHARGE commanded state
 *     - FEEDBACK_BIT_AIRP_COM Opposite of the AIR+ commanded state
 *     - FEEDBACK_BIT_AIRN_MEC Mechanical status of the AIR-
 *     - FEEDBACK_BIT_PRECHARGE_OPEN_MEC Mechanical status of the PRECHARGE relay
 *     - FEEDBACK_BIT_AIRP_MEC Mechanical status of the AIR+
 *     - FEEDBACK_BIT_SD_IMD_FB Shutdown IMD node feedback
 *     - FEEDBACK_BIT_SD_BMS_FB Shutdown AMS node feedback
 *     - FEEDBACK_BIT_TS_LESS_THAN_60V Less than 60V on the Tractive System
 *     - FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED Plausible state persisted value (Prefer this over the other two)
 *     - FEEDBACK_BIT_PLAUSIBLE_STATE Raw plausible state value
 *     - FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED AMS cockpit LED status
 *     - FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED IMD cockpit LED status
 *     - FEEDBACK_BIT_IMD_OK Status of the IMD
 *     - FEEDBACK_BIT_INDICATOR_CONNECTED Voltage indicator connected
 *     - FEEDBACK_BIT_LATCH_RESET All the latches are reset
 *     - FEEDBACK_BIT_PLAUSIBLE_STATE_RC Plausible state after the RC circuit
 *     - FEEDBACK_BIT_TSAL_GREEN Status of the TSAL
 *     - FEEDBACK_BIT_PROBING_3V3 Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V)
 *     - FEEDBACK_BIT_SD_OUT Shutdown out
 *     - FEEDBACK_BIT_SD_IN Shutdown in
 *     - FEEDBACK_BIT_SD_END Shutdown end
 *     - FEEDBACK_BIT_V5_MCU Feedback on the 5V line
 *     - FEEDBACK_BIT_UNKNOWN Feedback used for initialization or as return value
 */
typedef enum {
    FEEDBACK_BIT_AIRN_OPEN_COM = (1U << FEEDBACK_ID_AIRN_OPEN_COM),
    FEEDBACK_BIT_PRECHARGE_OPEN_COM = (1U << FEEDBACK_ID_PRECHARGE_OPEN_COM),
    FEEDBACK_BIT_AIRP_OPEN_COM = (1U << FEEDBACK_ID_AIRP_OPEN_COM),
    FEEDBACK_BIT_AIRN_OPEN_MEC = (1U << FEEDBACK_ID_AIRN_OPEN_MEC),
    FEEDBACK_BIT_PRECHARGE_OPEN_MEC = (1U << FEEDBACK_ID_PRECHARGE_OPEN_MEC),
    FEEDBACK_BIT_AIRP_OPEN_MEC = (1U << FEEDBACK_ID_AIRP_OPEN_MEC),
    FEEDBACK_BIT_SD_IMD_FB = (1U << FEEDBACK_ID_SD_IMD_FB),
    FEEDBACK_BIT_SD_BMS_FB = (1U << FEEDBACK_ID_SD_BMS_FB),
    FEEDBACK_BIT_TS_LESS_THAN_60V = (1U << FEEDBACK_ID_TS_LESS_THAN_60V),
    FEEDBACK_BIT_PLAUSIBLE_STATE_PERSISTED = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED),
    FEEDBACK_BIT_PLAUSIBLE_STATE = (1U << FEEDBACK_ID_PLAUSIBLE_STATE),
    FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED = (1U << FEEDBACK_ID_BMS_FAULT_COCKPIT_LED),
    FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED = (1U << FEEDBACK_ID_IMD_FAULT_COCKPIT_LED),
    FEEDBACK_BIT_INDICATOR_CONNECTED = (1U << FEEDBACK_ID_INDICATOR_CONNECTED),
    FEEDBACK_BIT_LATCH_RESET = (1U << FEEDBACK_ID_LATCH_RESET),
    FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_LATCHED),
    FEEDBACK_BIT_BMS_FAULT_LATCHED = (1U << FEEDBACK_ID_BMS_FAULT_LATCHED),
    FEEDBACK_BIT_IMD_FAULT_LATCHED = (1U << FEEDBACK_ID_IMD_FAULT_LATCHED),
    FEEDBACK_BIT_EXT_FAULT_LATCHED = (1U << FEEDBACK_ID_EXT_FAULT_LATCHED),
    FEEDBACK_BIT_IMD_OK = (1U << FEEDBACK_ID_IMD_OK),
    FEEDBACK_BIT_PLAUSIBLE_STATE_RC = (1U << FEEDBACK_ID_PLAUSIBLE_STATE_RC),
    FEEDBACK_BIT_TSAL_GREEN = (1U << FEEDBACK_ID_TSAL_GREEN),
    FEEDBACK_BIT_PROBING_3V3 = (1U << FEEDBACK_ID_PROBING_3V3),
    FEEDBACK_BIT_SD_OUT = (1U << FEEDBACK_ID_SD_OUT),
    FEEDBACK_BIT_SD_IN = (1U << FEEDBACK_ID_SD_IN),
    FEEDBACK_BIT_SD_END = (1U << FEEDBACK_ID_SD_END),
    FEEDBACK_BIT_V5_MCU = (1U << FEEDBACK_ID_V5_MCU)
} FeedbackBit;

/**
 * @brief Bit position of the digital feedbacks inside the bit flag
 *
 * @details
 *     - FEEDBACK_DIGITAL_BIT_AIRN_COM Opposite of the AIR- commanded state
 *     - FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM Opposite of the PRECHARGE commanded state
 *     - FEEDBACK_DIGITAL_BIT_AIRP_COM Opposite of the AIR+ commanded state
 *     - FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC Mechanical status of the PRECHARGE relay
 *     - FEEDBACK_DIGITAL_BIT_SD_IMD_FB Shutdown IMD node feedback
 *     - FEEDBACK_DIGITAL_BIT_SD_BMS_FB Shutdown AMS node feedback
 *     - FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V Less than 60V on the Tractive System
 *     - FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED Plausible state persisted value (Prefer this over the other two)
 *     - FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE Raw plausible state value
 *     - FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED AMS cockpit LED status
 *     - FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED IMD cockpit LED status
 *     - FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED Voltage indicator connected
 *     - FEEDBACK_DIGITAL_BIT_LATCH_RESET All the latches are reset
 *     - FEEDBACK_DIGITAL_BIT_UNKNOWN Feedback used for initialization or as return value
 */
typedef enum {
    FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM = 0U,
    FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM,
    FEEDBACK_DIGITAL_BIT_SD_IMD_FB,
    FEEDBACK_DIGITAL_BIT_SD_BMS_FB,
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM,
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC,
    FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V,
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_PERSISTED,
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE,
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED,
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED,
    FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED,
    FEEDBACK_DIGITAL_BIT_LATCH_RESET,
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED,
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_COUNT,
    FEEDBACK_DIGITAL_BIT_UNKNOWN
} FeedbackDigitalBit;

/**
 * @brief Indices of the analog feedbacks
 *
 * @details
 *     - FEEDBACK_ANALOG_INDEX_AIRN_MEC Mechanical status of the AIR-
 *     - FEEDBACK_ANALOG_INDEX_AIRP_MEC Mechanical status of the AIR+
 *     - FEEDBACK_ANALOG_INDEX_IMD_OK Status of the IMD
 *     - FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC Plausible state after the RC circuit
 *     - FEEDBACK_ANALOG_INDEX_TSAL_GREEN Status of the TSAL
 *     - FEEDBACK_ANALOG_INDEX_PROBING_3V3 Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V)
 *     - FEEDBACK_ANALOG_INDEX_SD_OUT Shutdown out
 *     - FEEDBACK_ANALOG_INDEX_SD_IN Shutdown in
 *     - FEEDBACK_ANALOG_INDEX_SD_END Shutdown end
 *     - FEEDBACK_ANALOG_INDEX_V5_MCU Feedback on the 5V line
 *     - FEEDBACK_ANALOG_INDEX_UNKNOWN Feedback used for initialization or as return value
 */
typedef enum {
    FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC = 0U,
    FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC,
    FEEDBACK_ANALOG_INDEX_IMD_OK,
    FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC,
    FEEDBACK_ANALOG_INDEX_TSAL_GREEN,
    FEEDBACK_ANALOG_INDEX_PROBING_3V3,
    FEEDBACK_ANALOG_INDEX_SD_OUT,
    FEEDBACK_ANALOG_INDEX_SD_IN,
    FEEDBACK_ANALOG_INDEX_SD_END,
    FEEDBACK_ANALOG_INDEX_V5_MCU,
    FEEDBACK_ANALOG_INDEX_COUNT,
    FEEDBACK_ANALOG_INDEX_UNKNOWN
} FeedbackAnalogIndex;

/**
 * @brief Status of the feedbacks
 *
 * @details
 *     - FEEDBACK_STATUS_LOW The feedback value is considered as logically low
 *     - FEEDBACK_STATUS_ERROR The feedback value is not in a valid state 
 *     - FEEDBACK_STATUS_HIGH The feedback value is considered as logically high
 */
typedef enum {
    FEEDBACK_STATUS_LOW,
    FEEDBACK_STATUS_ERROR,
    FEEDBACK_STATUS_HIGH
} FeedbackStatus;

/**
 * @brief Type definition for the internal feedback handler structure
 *
 * @attention This struct should not be used outside of this module
 *
 * @param read_digital A pointer to the function used to read all the digital feedbacks
 * @param start_conversion A pointer to the function used to start the converison of the analog feedbacks
 * @param digital The bit flag where each bit represent a specific feedback state
 * @param analog An array of raw voltages of the analog feedbacks
 * @param status An array of all the feedbacks current status
 * @param status_can_payload The canlib payload of the feedbacks status
 * @param digital_can_payload The canlib payload of the digital feedbacks values
 * @param analog_can_payload The canlib payload of the analog feedbacks values
 * @param analog_sd_can_payload The canlib payload of the analog shutdown feedbacks values
 * @param enzomma_can_payload The canlib payload of the feedback that did not allow the BMS to go the TS ON state
 */
typedef struct {
    feedback_read_digital_all_callback_t read_digital;
    feedback_start_analog_conversion_callback_t start_conversion;

    bit_flag32_t digital; 
    volt_t analog[FEEDBACK_ANALOG_INDEX_COUNT];

    FeedbackStatus status[FEEDBACK_COUNT];

    primary_hv_feedback_status_converted_t status_can_payload;
    primary_hv_feedback_digital_converted_t digital_can_payload;
    primary_hv_feedback_analog_converted_t analog_can_payload;
    primary_hv_feedback_analog_sd_converted_t analog_sd_can_payload;
    primary_hv_feedback_enzomma_converted_t enzomma_can_payload;
} _FeedbackHandler;

#ifdef CONF_FEEDBACK_MODULE_ENABLE

/**
 * @brief Initialize the internal feedback handler structure
 *
 * @param read_all A pointer to the callback that should read all the digital feedbacks
 * @param start_conversion A pointer to the callback that should start the conversion of the analog feedbacks
 *
 * @return FeedbackReturnCode
 *     - FEEDBACK_NULL_POINTER if any of the parameters are NULL
 *     - FEEDBACK_OK otherwise
 */
FeedbackReturnCode feedback_init(const feedback_read_digital_all_callback_t read_all, const feedback_start_analog_conversion_callback_t start_conversion);

/**
 * @brief Update all the digital feedbacks
 *
 * @return FeedbackReturnCode
 *     - FEEDBACK_OK
 */
FeedbackReturnCode feedback_update_digital_feedback_all(void);

/**
 * @brief Start the conversion of the analog feedbacks
 *
 * @return FeedbackReturnCode
 *     - FEEDBACK_OK
 */
FeedbackReturnCode feedback_start_analog_conversion_all(void);

/**
 * @brief Update a single value of the analog feedbacks
 *
 * @param index The index of the analog feedback
 * @param value The voltage value of the feedback in V
 *
 * @return FeedbackReturnCode
 *     - FEEDBACK_INVALID_INDEX the given index is not valid
 *     - FEEDBACK_OK otherwise
 */
FeedbackReturnCode feedback_update_analog_feedback(const FeedbackAnalogIndex index, const volt_t value);

/**
 * @brief Update the status of all the feedbacks
 *
 * @return FeedbackReturnCode
 *      - FEEDBACK_OK
 */
FeedbackReturnCode feedback_update_status(void);

/**
 * @brief Get the value of a digital feedback
 *
 * @param bit The digital feedback to get the value from
 *
 * @return bool The value of the feedback
 */
bool feedback_get_digital(const FeedbackDigitalBit bit);

/**
 * @brief Get the value of an analog feedback
 *
 * @param index The index of the analog feedback to get the value from
 *
 * @return volt_t The feedbacks voltage values
 */
volt_t feedback_get_analog(const FeedbackAnalogIndex index);

/**
 * @brief Get the status of a single feedback
 *
 * @param id The identifier of the feedback
 *
 * @return FeedbackStatus The feedback status
 */
FeedbackStatus feedback_get_status(const FeedbackId id);

/**
 * @brief Check if the feedbacks specified in the mask are in the expected status
 * 
 * @param mask The mask used to select the feedbacks to check
 * @param value The expected values of the feedbacks
 * @param out[out] The identifer of the feedback that does not match the expected value
 * or FEEDBACK_ID_UNKNOWN if every feedback is ok (can be NULL)
 *
 * @return bool True if all the feedbacks match the expected value, false otherwise
 */
bool feedback_check_values(
    const bit_flag32_t mask,
    const bit_flag32_t value,
    FeedbackId * const out
);

/**
 * @brief Check if a feedback is digital or analog given its identifier
 *
 * @param id The identifier of the feedback
 *
 * @return bool True if the feedback is digital, false otherwise
 */
bool feedback_is_digital(const FeedbackId id);

/**
 * @brief Get the feedback digital bit position from its identifier
 *
 * @param id The identifier of the feedback
 *
 * @return FeedbackDigitalBit The bit position of the digital feedback
 */
FeedbackDigitalBit feedback_get_digital_bit_from_id(const FeedbackId id);

/**
 * @brief Get the feedback analog index from its identifier
 *
 * @param id The identifier of the feedback
 *
 * @return FeedbackAnalogIndex The index of the analog feedback
 */
FeedbackAnalogIndex feedback_get_analog_index_from_id(const FeedbackId id);

/**
 * @brief Get a pointer to the CAN payload structure of the feedbacks status
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_feedback_status_converted_t* A pointer to the payload
 */
primary_hv_feedback_status_converted_t * feedback_get_status_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload structure of the digital feedbacks values
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_feedback_digital_converted_t* A pointer to the payload
 */
primary_hv_feedback_digital_converted_t * feedback_get_digital_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload structure of the analog feedbacks values
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_feedback_analog_converted_t* A pointer to the payload
 */
primary_hv_feedback_analog_converted_t * feedback_get_analog_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload structure of the analog shutdown feedbacks values
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_feedback_analog_sd_converted_t* A pointer to the payload
 */
primary_hv_feedback_analog_sd_converted_t * feedback_get_analog_sd_payload(size_t * const byte_size);

/**
 * @brief Get a pointer to the CAN payload structure of the feedback that did not
 * allow the BMS to go to the TS ON state
 *
 * @param id The identifier of the feedback
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_feedback_enzomma_converted_t* A pointer to the payload
 */
primary_hv_feedback_enzomma_converted_t * feedback_get_enzomma_payload(const FeedbackId id, size_t * const byte_size);

#ifdef CONF_FEEDBACK_STRINGS_ENABLE

/**
 * @brief Get the name of the corresponding feedback identifier
 *
 * @param id The feedback identifier
 *
 * @return cont char* A pointer to the name of the feedback id
 */
const char * const feedback_get_feedback_id_name(const FeedbackId id);

#else  // CONF_FEEDBACK_STRINGS_ENABLE

#define feedback_get_feedback_id_name(id) ""

#endif // CONF_FEEDBACK_STRINGS_ENABLE

#else  // CONF_FEEDBACK_MODULE_ENABLE

#define feedback_init(read_all, start_conversion) (FEEDBACK_OK)
#define feedback_update_digital_feedback_all() (FEEDBACK_OK)
#define feedback_start_analog_conversion_all() (FEEDBACK_OK)
#define feedback_update_analog_feedback(index, value) (FEEDBACK_OK)
#define feedback_update_status() (FEEDBACK_OK)
#define feedback_get_digital(bit) (false)
#define feedback_get_analog(index) (0.f)
#define feedback_get_status(id) (FEEDBACK_STATUS_ERROR)
#define feedback_check_values(mask, value, out) (true)
#define feedback_is_digital(id) (true)
#define feedback_get_digital_bit_from_id(id) (FEEDBACK_DIGITAL_BIT_UNKNOWN)
#define feedback_get_analog_index_from_id(id) (FEEDBACK_ANALOG_INDEX_UNKNOWN)
#define feedback_get_status_payload(byte_size) (NULL)
#define feedback_get_digital_payload(byte_size) (NULL)
#define feedback_get_analog_payload(byte_size) (NULL)
#define feedback_get_analog_sd_payload(byte_size) (NULL)
#define feedback_get_enzomma_payload(id, byte_size) (NULL)

#endif // CONF_FEEDBACK_MODULE_ENABLE

#endif  // FEEDBACK_H
