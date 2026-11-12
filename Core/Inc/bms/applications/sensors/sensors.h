/*!
 * \file sensors.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Data types and structures for the sensors handling
 *
 * \details The indices below name the physical signals as they come out of
 *          the hardware, with their original polarity (e.g. AIRN_OPEN_MEC is
 *          high when the AIR- is open). The translation to the mainboard
 *          status conditions (e.g. AIRN_CLOSED) is done by the module checks
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>
#include <stddef.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

/*! \brief Voltage reference of the MCU ADC in V */
#define SENSORS_VREF (3.3f)
/*! \brief Full scale of the 5V MCU feedback in V */
#define SENSORS_5V_VREF (5.f)
/*! \brief Full scale of the shutdown feedbacks in V */
#define SENSORS_SD_VREF (24.f)

/*!
 * \brief Convert the 5V MCU feedback voltage to the 5V line voltage
 *
 * \param VALUE The feedback voltage in V
 */
#define SENSORS_FEEDBACK_TO_5V_VOLT(VALUE) ((VALUE) * (SENSORS_5V_VREF) / (SENSORS_VREF))

/*!
 * \brief Convert a shutdown feedback voltage to the shutdown circuit voltage
 *
 * \param VALUE The feedback voltage in V
 */
#define SENSORS_FEEDBACK_TO_SD_VOLT(VALUE) ((VALUE) * (SENSORS_SD_VREF) / (SENSORS_VREF))

/*!
 * \brief Return code for the sensors module functions
 */
enum SensorsReturnCode {
    SENSORS_RC_OK,           /*!< The function executed succesfully */
    SENSORS_RC_INVALID_INDEX /*!< The given sensor index is out of range */
};

/*!
 * \brief Indices of the digital sensors (read from GPIOs)
 */
enum SensorsDigitalIndex {
    SENSORS_DIGITAL_INDEX_PRECHARGE_OPEN_MEC,        /*!< Mechanical status of the precharge relay */
    SENSORS_DIGITAL_INDEX_TS_LESS_THAN_60V,          /*!< Less than 60V on the Tractive System */
    SENSORS_DIGITAL_INDEX_PLAUSIBLE_STATE_PERSISTED, /*!< Plausible state persisted value (prefer this over the other two) */
    SENSORS_DIGITAL_INDEX_PLAUSIBLE_STATE,           /*!< Raw plausible state value */
    SENSORS_DIGITAL_INDEX_AMS_FAULT_COCKPIT_LED,     /*!< AMS cockpit LED status */
    SENSORS_DIGITAL_INDEX_IMD_FAULT_COCKPIT_LED,     /*!< IMD cockpit LED status */
    SENSORS_DIGITAL_INDEX_INDICATOR_CONNECTED,       /*!< Voltage indicator connected */
    SENSORS_DIGITAL_INDEX_LATCH_RESET,               /*!< All the latches are reset */
    SENSORS_DIGITAL_INDEX_PLAUSIBLE_STATE_LATCHED,   /*!< Plausible state latched */
    SENSORS_DIGITAL_INDEX_AMS_FAULT_LATCHED,         /*!< AMS fault latched */
    SENSORS_DIGITAL_INDEX_IMD_FAULT_LATCHED,         /*!< IMD fault latched */
    SENSORS_DIGITAL_INDEX_EXT_FAULT_LATCHED,         /*!< External fault latched */
    SENSORS_DIGITAL_INDEX_HANDCART_CONNECTED,        /*!< The mainboard is connected to the handcart */
    SENSORS_DIGITAL_INDEX_COUNT                      /*!< Total number of digital sensors */
};

/*!
 * \brief Indices of the analog sensors (read from the ADC)
 */
enum SensorsAnalogIndex {
    SENSORS_ANALOG_INDEX_AIRN_OPEN_MEC,      /*!< Mechanical status of the AIR- */
    SENSORS_ANALOG_INDEX_AIRP_OPEN_MEC,      /*!< Mechanical status of the AIR+ */
    SENSORS_ANALOG_INDEX_IMD_OK,             /*!< Status of the IMD */
    SENSORS_ANALOG_INDEX_PLAUSIBLE_STATE_RC, /*!< Plausible state after the RC circuit */
    SENSORS_ANALOG_INDEX_TSAL_GREEN,         /*!< Status of the TSAL */
    SENSORS_ANALOG_INDEX_PROBING_3V3,        /*!< Feedback on the 3V3 line (comes out of a divider so it should be around 1.6V) */
    SENSORS_ANALOG_INDEX_SD_IMD_FB,          /*!< Shutdown IMD node feedback */
    SENSORS_ANALOG_INDEX_SD_AMS_FB,          /*!< Shutdown AMS node feedback */
    SENSORS_ANALOG_INDEX_SD_OUT,             /*!< Shutdown out */
    SENSORS_ANALOG_INDEX_SD_IN,              /*!< Shutdown in */
    SENSORS_ANALOG_INDEX_SD_END,             /*!< Shutdown end */
    SENSORS_ANALOG_INDEX_V5_MCU,             /*!< Feedback on the 5V line */
    SENSORS_ANALOG_INDEX_COUNT               /*!< Total number of analog sensors */
};

/*!
 * \brief Logical status of an analog sensor, obtained from its filtered voltage
 */
enum SensorsAnalogStatus {
    SENSORS_ANALOG_STATUS_LOW,   /*!< The value is logically low */
    SENSORS_ANALOG_STATUS_ERROR, /*!< The value is between the thresholds, so it is implausible */
    SENSORS_ANALOG_STATUS_HIGH   /*!< The value is logically high */
};

/*!
 * \brief Pipeline of a single digital sensor
 */
struct SensorsDigitalValue {
    volatile bool buffer[SENSORS_BUFFER_SIZE]; /*!< Circular buffer of the raw samples */
    volatile size_t head;                      /*!< Index where the next sample is written */
    volatile bool ready;                       /*!< True once the buffer is full, false otherwise */

    bool filtered; /*!< Filtered value (majority of the samples) */
};

/*!
 * \brief Pipeline of a single analog sensor
 */
struct SensorsAnalogValue {
    volatile volt buffer[SENSORS_BUFFER_SIZE]; /*!< Circular buffer of the raw samples */
    volatile size_t head;                      /*!< Index where the next sample is written */
    volatile bool ready;                       /*!< True once the buffer is full, false otherwise */

    volt filtered;                   /*!< Filtered value (average of the samples) */
    enum SensorsAnalogStatus status; /*!< Logical status of the filtered value */
};

/*!
 * \brief Internal handler of the sensors module
 *
 * \warning This structure should never be used outside of this module
 */
struct SensorsHandler {
    struct SensorsDigitalValue digital[SENSORS_DIGITAL_INDEX_COUNT]; /*!< The digital sensors */
    struct SensorsAnalogValue analog[SENSORS_ANALOG_INDEX_COUNT];    /*!< The analog sensors */

    bool is_ready; /*!< True once every sensor buffer is full, the filtered values are meaningful only then */
};

#endif // SENSORS_H
