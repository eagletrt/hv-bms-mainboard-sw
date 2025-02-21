/**
 * @file imd.h
 * @date 2024-07-04
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Insulation Monitoring Device (IMD) handing functions
 */

#ifndef IMD_H
#define IMD_H

#include "mainboard-def.h"
#include "mainboard-conf.h"

#include "primary_network.h"

#include "ir1553204.h"

/** @brief Type definition for the callback function that should start the PWM measurements */
typedef void (* imd_pwm_start_callback_t)(void);

/**
 * @brief Return code for the IMD module functions
 *
 * @details
 *     - IMD_OK the function executed succesfully
 *     - IMD_NULL_POINTER a NULL pointer was given to a function
 *     - IMD_INVALID_DATA the given data is not valid
 */
typedef enum {
    IMD_OK = IR1553204_OK,
    IMD_NULL_POINTER = IR1553204_NULL_POINTER,
    IMD_INVALID_DATA
} ImdReturnCode;

/**
 * @brief Type definition for the IMD status
 *
 * @details
 *     - IMD_STATUS_UNKNOWN any other uncertain state
 *     - IMD_STATUS_SHORT_CIRCUIT a short circuit is detected
 *     - IMD_STATUS_NORMAL this is the normal state of the IMD
 *     - IMD_STATUS_UNDER_VOLTAGE detected voltage is too low
 *     - IMD_STATUS_START_MEASURE the IMD has started the measure
 *     - IMD_STATUS_DEVICE_ERROR general device error
 *     - IMD_STATUS_EARTH_FAULT fault detected on the heart connection
 */
typedef enum {
    IMD_STATUS_UNKNOWN = IR1553204_STATUS_UNKNOWN,
    IMD_STATUS_SHORT_CIRCUIT = IR1553204_STATUS_SHORT_CIRCUIT,
    IMD_STATUS_NORMAL = IR1553204_STATUS_NORMAL,
    IMD_STATUS_UNDER_VOLTAGE = IR1553204_STATUS_UNDER_VOLTAGE,
    IMD_STATUS_START_MEASURE = IR1553204_STATUS_START_MEASURE,
    IMD_STATUS_DEVICE_ERROR = IR1553204_STATUS_DEVICE_ERROR,
    IMD_STATUS_EARTH_FAULT = IR1553204_STATUS_EARTH_FAULT,
    IMD_STATUS_COUNT = IR1553204_STATUS_COUNT
} ImdStatus;

/**
 * @brief Type definition for the IMD handler structure
 *
 * @attention Do not use this struct outside this module
 *
 * @param start A pointer to the callback used to start the PWM to read from the IMD
 * @param ir41153204 Handler structure of the IMD driver
 * @param status_can_payload The canlib payload used to send the IMD status
 */
typedef struct {
    imd_pwm_start_callback_t start;

    Ir1553204Handler ir1153204;

    primary_hv_imd_status_converted_t status_can_payload;
} _ImdHandler;

#ifdef CONF_IMD_MODULE_ENABLE

/**
 * @brief initialization function for the IMD internal structure handler
 *
 * @param start A pointer to the PWM start callback function
 *
 * @return ImdReturnCode
 *     - IMD_NULL_POINTER if any of the given parameter is NULL
 *     - IMD_OK otherwise
 */
ImdReturnCode imd_init(const imd_pwm_start_callback_t start);

/**
 * @brief Get the duty cycle percentage of the IMD signal
 *
 * @return precise_percentage_t The duty cycle percentage from 0 to 1, or 0 on error
 */
precise_percentage_t imd_get_duty_cycle(void);

/**
 * @brief Get the frequency of the IMD signal
 *
 * @return hertz_t The frequency in Hz, or 0 on error
 */
hertz_t imd_get_frequency(void);

/**
 * @brief Get the period of the IMD signal
 *
 * @return milliseconds_t The period in ms, or 0 on error
 */
milliseconds_t imd_get_period(void);

/**
 * @brief Get the status of the IMD
 *
 * @return ImdStatus The current status of the IMD
 */
ImdStatus imd_get_status(void);

/**
 * @brief Update the internal handler with a new PWM measurement
 *
 * @param source_frequency The sampling frequency of the signal
 * @param period_count Number of samples of the signal period
 * @param high_count Number of logical high samples of the signal
 *
 * @return ImdReturnCode
 *     - IMD_INVALID_DATA if the parameters are not valid
 *     - IMD_OK otherwise
 */
ImdReturnCode imd_update(
    const ticks_t source_frequency,
    const ticks_t period_count,
    const ticks_t high_count
);

/**
 * @brief Get a pointer to the CAN payload of the IMD
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_imd_status_converted_t* A pointer to the payload
 */
primary_hv_imd_status_converted_t * imd_get_status_canlib_payload(size_t * const byte_size);

#ifdef CONF_IMD_STRINGS_ENABLE

const char * const imd_get_imd_status_name(const ImdStatus status);

#else  // CONF_IMD_STRINGS_ENABLE

#define imd_get_imd_status_name(status) ""

#endif // CONF_IMD_STRINGS_ENABLE

#else  // CONF_IMD_MODULE_ENABLE

#define imd_init(start) (IMD_OK)
#define imd_get_duty_cycle() (0U)
#define imd_get_frequency() (0U)
#define imd_get_period() (0U)
#define imd_get_status() (IMD_STATUS_NORMAL)
#define imd_update(source_frequency, period_count, high_count) (IMD_OK)
#define imd_get_status_canlib_payload(byte_size) (NULL)

#endif // CONF_IMD_MODULE_ENABLE

#endif  // IMD_H
