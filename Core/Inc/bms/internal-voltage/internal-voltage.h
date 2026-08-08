/*!
 * \file internal-voltage.h
 * \date 2024-07-19
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages measurements and controls
 */

#ifndef INTERNAL_VOLTAGE_H
#define INTERNAL_VOLTAGE_H

#include "can-primary.h"
#include "mainboard-def.h"

#include "max22530.h"

/*! \brief The period with which internal voltages are updated in ms */
#define INTERNAL_VOLTAGE_CYCLE_TIME_MS (8U)

/*! \brief Maximum allowed voltage difference between the pack voltage and the cells voltage sum in V */
#define INTERNAL_VOLTAGE_MAX_DELTA_V (25.f)

/*! \brief Divider ratio of the internal voltages */
#define INTERNAL_VOLTAGE_DIVIDER_RATIO (0.002914f)

/*!
 * \brief Conversion from the voltage read from the ADC to the real voltage in V
 *
 * \param value The voltage read from the ADC
 *
 * \returns volt_t The converted value in V
 */
#define INTERNAL_VOLTAGE_ADC_VOLTAGE_TO_VOLT(value) ((value) / INTERNAL_VOLTAGE_DIVIDER_RATIO)

/*!
 * \brief Return code for the internal voltage module functions
 */
enum InternalVoltageReturnCode {
    INTERNAL_VOLTAGE_RC_OK,           /*!< Function executed successfully */
    INTERNAL_VOLTAGE_RC_NULL_POINTER, /*!< NULL pointer was given to a function */
    INTERNAL_VOLTAGE_RC_DRIVER_ERROR  /*!< The driver was not able to parse the input data */
};

/*!
 * \brief Internal voltage channels of the external ADC
 */
enum InternalVoltageChannel {
    INTERNAL_VOLTAGE_CHANNEL_TS_VOLTAGE = MAX22530_CHANNEL_1,                   /*!< The voltage of the TS */
    INTERNAL_VOLTAGE_CHANNEL_PACK_VOLTAGE = MAX22530_CHANNEL_2,                 /*!< The voltage of the battery pack */
    INTERNAL_VOLTAGE_CHANNEL_LINEAR_REGULATOR_TEMPERATURE = MAX22530_CHANNEL_3, /*!< Temperature of the linear regulator mosfet */
    INTERNAL_VOLTAGE_CHANNEL_PRECHARGE_TEMPERATURE = MAX22530_CHANNEL_4,        /*!< Temperature of the precharge resistors heatsink */
    INTERNAL_VOLTAGE_CHANNEL_COUNT = MAX22530_CHANNEL_COUNT                     /*!< The number of channels used for internal voltage measurements */
};

/*!
 * \brief Type definition for the internal voltage handler structure
 *
 * \attention This structure should not be used outside of this module
 */
struct InternalVoltageHandler {
    struct Max22530Handler max22530; /*!< Handler structure of the external ADC */

    volt_t ts;   /*!< The voltage of the Tractive System */
    volt_t pack; /*!< The voltage of the battery pack */

    union CanPrimaryMessages libcan_message_voltage; /*!< The TS voltage info canlib payload */
};

#endif // INTERNAL_VOLTAGE_H
