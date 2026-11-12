/*!
 * \file feedback.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the feedback handling
 *
 * \details The indices below name the physical signals as they come out of
 *          the hardware, with their original polarity (e.g. AIRN_OPEN_MEC is
 *          high when the AIR- is open). The translation to the mainboard
 *          status conditions (e.g. AIRN_CLOSED) is done by the module checks
 */

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <stdbool.h>
#include <stddef.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/*! \brief Voltage reference of the MCU ADC in V */
#define FEEDBACK_VREF (3.3f)
/*! \brief Full scale of the 5V MCU feedback in V */
#define FEEDBACK_5V_VREF (5.f)
/*! \brief Full scale of the shutdown feedbacks in V */
#define FEEDBACK_SD_VREF (24.f)

/*!
 * \brief Convert the 5V MCU feedback voltage to the 5V line voltage
 *
 * \param VALUE The feedback voltage in V
 */
#define FEEDBACK_TO_5V_VOLT(VALUE) ((VALUE) * (FEEDBACK_5V_VREF) / (FEEDBACK_VREF))

/*!
 * \brief Convert a shutdown feedback voltage to the shutdown circuit voltage
 *
 * \param VALUE The feedback voltage in V
 */
#define FEEDBACK_TO_SD_VOLT(VALUE) ((VALUE) * (FEEDBACK_SD_VREF) / (FEEDBACK_VREF))

/*!
 * \brief Return code for the feedback module functions
 */
enum FeedbackReturnCode {
    FEEDBACK_RC_OK,           /*!< The function executed succesfully */
    FEEDBACK_RC_INVALID_INDEX /*!< The given feedback index is out of range */
};

/*!
 * \brief Indices of the digital feedback (read from GPIOs)
 */
enum FeedbackDigitalIndex {
    FEEDBACK_DIGITAL_INDEX_PRECHARGE_OPEN_MEC,        /*!< Mechanical status of the precharge relay */
    FEEDBACK_DIGITAL_INDEX_TS_LESS_THAN_60V,          /*!< Less than 60V on the Tractive System */
    FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED, /*!< Plausible state persisted value (prefer this over the other two) */
    FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE,           /*!< Raw plausible state value */
    FEEDBACK_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED,     /*!< AMS cockpit LED status */
    FEEDBACK_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED,     /*!< IMD cockpit LED status */
    FEEDBACK_DIGITAL_INDEX_INDICATOR_CONNECTED,       /*!< Voltage indicator connected */
    FEEDBACK_DIGITAL_INDEX_LATCH_RESET,               /*!< All the latches are reset */
    FEEDBACK_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED,   /*!< Plausible state latched */
    FEEDBACK_DIGITAL_INDEX_AMS_FAULT_LATCHED,         /*!< AMS fault latched */
    FEEDBACK_DIGITAL_INDEX_IMD_FAULT_LATCHED,         /*!< IMD fault latched */
    FEEDBACK_DIGITAL_INDEX_EXT_FAULT_LATCHED,         /*!< External fault latched */
    FEEDBACK_DIGITAL_INDEX_HANDCART_CONNECTED,        /*!< The mainboard is connected to the handcart */
    FEEDBACK_DIGITAL_INDEX_COUNT                      /*!< Total number of digital feedback */
};

/*!
 * \brief Indices of the analog feedback (read from the ADC)
 */
enum FeedbackAnalogIndex {
    FEEDBACK_ANALOG_INDEX_AIRN_OPEN_MEC,      /*!< Mechanical status of the AIR- */
    FEEDBACK_ANALOG_INDEX_AIRP_OPEN_MEC,      /*!< Mechanical status of the AIR+ */
    FEEDBACK_ANALOG_INDEX_IMD_OK,             /*!< Status of the IMD */
    FEEDBACK_ANALOG_INDEX_PLAUSIBLE_STATE_RC, /*!< Plausible state after the RC circuit */
    FEEDBACK_ANALOG_INDEX_TSAL_GREEN,         /*!< Status of the TSAL */
    FEEDBACK_ANALOG_INDEX_PROBING_3V3,        /*!< Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V) */
    FEEDBACK_ANALOG_INDEX_SD_IMD_FB,          /*!< Shutdown IMD node feedback */
    FEEDBACK_ANALOG_INDEX_SD_AMS_FB,          /*!< Shutdown AMS node feedback */
    FEEDBACK_ANALOG_INDEX_SD_OUT,             /*!< Shutdown out */
    FEEDBACK_ANALOG_INDEX_SD_IN,              /*!< Shutdown in */
    FEEDBACK_ANALOG_INDEX_SD_END,             /*!< Shutdown end */
    FEEDBACK_ANALOG_INDEX_V5_MCU,             /*!< Feedback on the 5V line */
    FEEDBACK_ANALOG_INDEX_COUNT               /*!< Total number of analog feedback */
};

/*!
 * \brief Logical status of an analog feedback, obtained from its filtered voltage
 */
enum FeedbackAnalogStatus {
    FEEDBACK_ANALOG_STATUS_LOW,   /*!< The value is logically low */
    FEEDBACK_ANALOG_STATUS_ERROR, /*!< The value is between the thresholds, so it is implausible */
    FEEDBACK_ANALOG_STATUS_HIGH   /*!< The value is logically high */
};

/*!
 * \brief Pipeline of a single digital feedback
 */
struct FeedbackDigitalValue {
    volatile bool buffer[FEEDBACK_BUFFER_SIZE]; /*!< Circular buffer of the raw samples */
    volatile size_t head;                       /*!< Index where the next sample is written */
    volatile bool ready;                        /*!< True once the buffer is full, false otherwise */

    bool filtered; /*!< Filtered value (majority of the samples) */
};

/*!
 * \brief Pipeline of a single analog feedback
 */
struct FeedbackAnalogValue {
    volatile volt buffer[FEEDBACK_BUFFER_SIZE]; /*!< Circular buffer of the raw samples */
    volatile size_t head;                       /*!< Index where the next sample is written */
    volatile bool ready;                        /*!< True once the buffer is full, false otherwise */

    volt filtered;                    /*!< Filtered value */
    enum FeedbackAnalogStatus status; /*!< Logical status of the filtered value */
};

/*!
 * \brief How the thresholds of an analog feedback are interpreted
 */
enum FeedbackAnalogThresholdMode {
    FEEDBACK_ANALOG_THRESHOLD_MODE_STEP,  /*!< value <= low is LOW, value >= high is HIGH, in between is ERROR */
    FEEDBACK_ANALOG_THRESHOLD_MODE_WINDOW /*!< low <= value <= high is HIGH, outside is ERROR */
};

/*!
 * \brief Threshold profile of an analog feedback
 *
 * \details The limits always satisfy low < high, the mode defines their meaning
 */
struct FeedbackAnalogThreshold {
    volt low;                              /*!< Low threshold in V */
    volt high;                             /*!< High threshold in V */
    enum FeedbackAnalogThresholdMode mode; /*!< Interpretation of the limits */
};

/*!
 * \brief Internal handler of the feedback module
 *
 * \warning This structure should never be used outside of this module
 */
struct FeedbackHandler {
    struct FeedbackDigitalValue digital[FEEDBACK_DIGITAL_INDEX_COUNT]; /*!< The digital feedback */
    struct FeedbackAnalogValue analog[FEEDBACK_ANALOG_INDEX_COUNT];    /*!< The analog feedback */

    bool is_ready; /*!< True once every feedback buffer is full, the filtered values are meaningful only then */
};

#endif // FEEDBACK_H
