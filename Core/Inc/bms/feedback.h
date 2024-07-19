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

/** @brief Alias for the total number of feedbacks */
#define FEEDBACK_COUNT (FEEDBACK_ID_COUNT)

/** @brief The period with which feedbacks are updated */
#define FEEDBACK_CYCLE_TIME_MS ((milliseconds_t)1U)

/** @brief Feedbacks reference voltage in mV */
#define FEEDBACK_VREF ((millivolt_t)3300.f)

/** @brief Feedbacks ADC resolution in bits */
#define FEEDBACK_ADC_RESOLUTION (12U)

/**
 * @brief Thresholds for the analog feedbacks in mV
 *
 * @details If the voltage of a feedback is greater than the HIGH threshold it is considered
 * as logically high, if it is lower than the LOW threshold is considered low otherwise
 * it is in an implausible state and considered as error
 */
#define FEEDBACK_THRESHOLD_HIGH_MILLIVOLT ((millivolt_t)1900.f)
#define FEEDBACK_THRESHOLD_LOW_MILLIVOLT ((millivolt_t)700.f)

/**
 * @brief Threshold for the analog feedbacks as raw values
 */
#define FEEDBACK_THRESHOLD_HIGH ( \
    MAINBOARD_MILLIVOLT_TO_ADC_VALUE( \
        FEEDBACK_THRESHOLD_HIGH_MILLIVOLT, \
        FEEDBACK_VREF, \
        FEEDBACK_ADC_RESOLUTION \
    ) \
)
#define FEEDBACK_THRESHOLD_LOW ( \
    MAINBOARD_MILLIVOLT_TO_ADC_VALUE( \
        FEEDBACK_THRESHOLD_LOW_MILLIVOLT, \
        FEEDBACK_VREF, \
        FEEDBACK_ADC_RESOLUTION \
    ) \
)

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
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
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
        FEEDBACK_BIT_AIRP_OPEN_COM | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED \
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
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_TSAL_GREEN | \
        FEEDBACK_BIT_PROBING_3V3 | \
        FEEDBACK_BIT_SD_END | \
        FEEDBACK_BIT_V5_MCU \
    )
#define FEEDBACK_AIRN_CHECK_TO_PRECHARGE_LOW \
    ( \
        FEEDBACK_BIT_PRECHARGE_OPEN_COM | \
        FEEDBACK_BIT_AIRP_OPEN_COM | \
        FEEDBACK_BIT_AIRN_OPEN_MEC | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED \
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
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
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
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED | \
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
        FEEDBACK_BIT_INDICATOR_CONNECTED | \
        FEEDBACK_BIT_PLAUSIBLE_STATE_LATCHED | \
        FEEDBACK_BIT_IMD_OK | \
        FEEDBACK_BIT_TSAL_GREEN | \
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
        FEEDBACK_BIT_BMS_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_IMD_FAULT_COCKPIT_LED | \
        FEEDBACK_BIT_BMS_FAULT_LATCHED | \
        FEEDBACK_BIT_IMD_FAULT_LATCHED \
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

/** @brief Type definition of the feedback identifiers
 *
 * @details
 *     - FEEDBACK_ID_AIRN_COM Opposite of the AIR- commanded state
 *     - FEEDBACK_ID_PRECHARGE_OPEN_COM Opposite of the PRECHARGE commanded state
 *     - FEEDBACK_ID_AIRP_COM Opposite of the AIR+ commanded state
 *     - FEEDBACK_ID_AIRN_MEC Status of the AIR-
 *     - FEEDBACK_ID_PRECHARGE_OPEN_MEC Status of the PRECHARGE
 *     - FEEDBACK_ID_AIRP_MEC Status of the AIR+
 *     - FEEDBACK_ID_SD_IMD_FB 
 *     - FEEDBACK_ID_SD_BMS_FB
 *     - FEEDBACK_ID_TS_LESS_THAN_60V Feedback on the TS+ voltage
 *     - FEEDBACK_ID_PLAUSIBLE_STATE_PERSISTED Plausible state of the feedbacks (Prefer this over the other two)
 *     - FEEDBACK_ID_PLAUSIBLE_STATE Plausible state of the feedbacks
 *     - FEEDBACK_ID_BMS_FAULT_COCKPIT_LED
 *     - FEEDBACK_ID_IMD_FAULT_COCKPIT_LED
 *     - FEEDBACK_ID_IMD_OK The IMD is working correclty
 *     - FEEDBACK_ID_INDICATOR_CONNECTED
 *     - FEEDBACK_ID_LATCH_RESET
 *     - FEEDBACK_ID_PLAUSIBLE_STATE_RC Plausible state of the feedbacks
 */
typedef enum {
    FEEDBACK_ID_AIRN_OPEN_COM,
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
    FEEDBACK_ID_COUNT
} FeedbackId;

/** @brief Type definition of the feedback identifiers
 *
 * @details
 *     - FEEDBACK_ID_AIRN_COM 
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
 *     - FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM
 */
typedef enum {
    FEEDBACK_DIGITAL_BIT_AIRN_OPEN_COM = 0U,
    FEEDBACK_DIGITAL_BIT_AIRP_OPEN_COM,
    FEEDBACK_DIGITAL_BIT_SD_IMD_FB,
    FEEDBACK_DIGITAL_BIT_SD_BMS_FB,
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_COM,
    FEEDBACK_DIGITAL_BIT_PRECHARGE_OPEN_MEC,
    FEEDBACK_DIGITAL_BIT_TS_LESS_THAN_60V,
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE,
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_COCKPIT_LED,
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_COCKPIT_LED,
    FEEDBACK_DIGITAL_BIT_INDICATOR_CONNECTED,
    FEEDBACK_DIGITAL_BIT_LATCH_RESET,
    FEEDBACK_DIGITAL_BIT_PLAUSIBLE_STATE_LATCHED,
    FEEDBACK_DIGITAL_BIT_BMS_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_IMD_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_EXT_FAULT_LATCHED,
    FEEDBACK_DIGITAL_BIT_COUNT
} FeedbackDigitalBit;

/**
 * @brief Indices of the analog feedbacks
 *
 * @details
 *     - 
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
    FEEDBACK_ANALOG_INDEX_COUNT
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
 */
typedef struct {
    feedback_read_digital_all_callback_t read_digital;
    feedback_start_analog_conversion_callback_t start_conversion;

    bit_flag32_t digital; 
    raw_volt_t analog[FEEDBACK_ANALOG_INDEX_COUNT];

    FeedbackStatus status[FEEDBACK_COUNT];
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
FeedbackReturnCode feedback_init(
    feedback_read_digital_all_callback_t read_all,
    feedback_start_analog_conversion_callback_t start_conversion
);

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
 * @return FeedbackReturnCode
 *     - FEEDBACK_INVALID_INDEX the given index is not valid
 *     - FEEDBACK_OK otherwise
 */
FeedbackReturnCode feedback_update_analog_feedback(FeedbackAnalogIndex index, raw_volt_t value);

/**
 * @brief Update the status of all the feedbacks
 *
 * @return FeedbackReturnCode
 *      - FEEDBACK_OK
 */
FeedbackReturnCode feedback_update_status(void);

/**
 * @brief Check if the feedbacks specified in the mask are in the expected status
 * 
 * @param mask The mask used to select the feedbacks to check
 * @param value The expected values of the feedbacks
 *
 * @return bool True if all the feedbacks match the expected value, false Otherwise
 */
bool feedback_check_values(bit_flag32_t mask, bit_flag32_t value);

#else  // CONF_FEEDBACK_MODULE_ENABLE

#define feedback_init(read_all, start_conversion) (FEEDBACK_OK)
#define feedback_update_digital_feedback_all() (FEEDBACK_OK)
#define feedback_start_analog_conversion_all() (FEEDBACK_OK)
#define feedback_update_analog_conversion(index, value) (FEEDBACK_OK)
#define feedback_update_status() (FEEDBACK_OK)
#define feedback_check_values(mask, value) (true)

#endif // CONF_FEEDBACK_MODULE_ENABLE

#endif  // FEEDBACK_H
