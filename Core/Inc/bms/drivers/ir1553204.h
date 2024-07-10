/**
 * @file ir1553204.h
 * @date 2024-07-03
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Driver for the IR155-3204 Insulation Monitoring Device (IMD)
 */

#ifndef IR1553204_H
#define IR1553204_H

#include <stdint.h>

#include "mainboard-def.h"

/**
 * @brief Return code for the IR1553204 module functions
 * 
 * @details
 *     - IR1553204_OK the function executed succesfully
 *     - IR1553204_NULL_POINTER a NULL pointer was given to a function
 */
typedef enum {
    IR1553204_OK,
    IR1553204_NULL_POINTER
} Ir1553204ReturnCode;

/**
 * @brief Type definition for the IR1553204 status
 *
 * @details
 *     - IR1553204_STATUS_UNKNOWN any other uncertain state
 *     - IR1553204_STATUS_SHORT_CIRCUIT a short circuit is detected
 *     - IR1553204_STATUS_NORMAL this is the normal state of the IMD
 *     - IR1553204_STATUS_UNDER_VOLTAGE detected voltage is too low
 *     - IR1553204_STATUS_START_MEASURE the IMD has started the measure
 *     - IR1553204_STATUS_DEVICE_ERROR general device error
 *     - IR1553204_STATUS_EARTH_FAULT fault detected on the heart connection
 */
typedef enum {
    IR1553204_STATUS_UNKNOWN = -1,
    IR1553204_STATUS_SHORT_CIRCUIT,
    IR1553204_STATUS_NORMAL,
    IR1553204_STATUS_UNDER_VOLTAGE,
    IR1553204_STATUS_START_MEASURE,
    IR1553204_STATUS_DEVICE_ERROR,
    IR1553204_STATUS_EARTH_FAULT,
    IR1553204_STATUS_COUNT
} Ir1553204Status;

/**
 * @brief IR155-3204 handler structure definition
 *
 * @param start A pointer to the function callback used to start the PWM measurements
 * @param frequency The frequency of the signal
 * @param duty_cycle The duty cycle of the signal
 */
typedef struct {
    hertz_t frequency;
    precise_percentage_t duty_cycle;
} Ir1553204Handler;


/**
 * @brief Initialization function for the IR1553204
 *
 * @param handler A pointer to the handler structure
 * @param start A pointer to the function callback that should start the PWM readings
 */
Ir1553204ReturnCode ir1553204_init(Ir1553204Handler * handler);

/**
 * @brief Get the duty cycle percentage of the last PWM measurement
 *
 * @param handler A pointer to the structure handler
 *
 * @return precise_percentage_t The duty cycle percentage from 0 to 100, or 0 on error
 */
precise_percentage_t ir1553204_get_duty_cycle(Ir1553204Handler * handler);

/**
 * @brief Set the duty cycle of the IMD
 *
 * @param handler A pointer to the structure handler
 * @param precise_percentage_t The duty cycle
 *
 * @return Ir1553204ReturnCode
 *     - IR1153204_NULL_POINTER if any of the parameters is NULL
 *     - IR1553204_OK otherwise
 */
Ir1553204ReturnCode ir1553204_set_duty_cycle(Ir1553204Handler * handler, precise_percentage_t duty_cycle);

/**
 * @brief Get the frequency of the last PWM measurement
 *
 * @param handler A pointer to the structure handler
 *
 * @return hertz_t The frequency in Hz, or 0 on error
 */
hertz_t ir1553204_get_frequency(Ir1553204Handler * handler);

/**
 * @brief Set the frequency of the last PWM measurement
 *
 * @param handler A pointer to the structure handler
 * @param frequency The frequency
 *
 * @return Ir1553204ReturnCode
 *     - IR1153204_NULL_POINTER if any of the parameters is NULL
 *     - IR1553204_OK otherwise
 */
Ir1553204ReturnCode ir1553204_set_frequency(Ir1553204Handler * handler, hertz_t frequency);

/**
 * @brief Get the period of the last PWM measurement
 *
 * @param handler A pointer to the structure handler
 *
 * @return milliseconds_t The period in ms, or 0 on error
 */
milliseconds_t ir1553204_get_period(Ir1553204Handler * handler);

/**
 * @brief Get the status of the IR155-3204
 *
 * @param handler A pointer to the structure handler
 *
 * @return Ir1553204Status The current status of the IR155-3204
 */
Ir1553204Status ir1553204_get_status(Ir1553204Handler * handler);

#endif  // IR1553204_H
