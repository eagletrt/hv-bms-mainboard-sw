/*!
 * \file ir1553204-api.h
 * \date 2024-07-03
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 * 
 * \brief Driver for the IR155-3204 Insulation Monitoring Device (IMD)
 */

#ifndef IR1553204_API_H
#define IR1553204_API_H

#include "ir1553204.h"

/*!
 * \brief Initialization function for the IR1553204
 *
 * \param handler A pointer to the handler structure
 * \param start A pointer to the function callback that should start the PWM readings
 */
enum Ir1553204ReturnCode ir1553204_api_init(struct Ir1553204Handler *handler);

/*!
 * \brief Get the duty cycle percentage of the last PWM measurement
 *
 * \param handler A pointer to the structure handler
 *
 * \returns precise_percentage_t The duty cycle percentage from 0 to 1, or 0 on error
 */
precise_percentage_t ir1553204_api_get_duty_cycle(struct Ir1553204Handler *handler);

/*!
 * \brief Set the duty cycle of the IMD
 *
 * \param handler A pointer to the structure handler
 * \param precise_percentage_t The duty cycle
 *
 * \retval IR1153204_NULL_POINTER if any of the parameters is NULL
 * \retval IR1553204_RC_OK otherwise
 */
enum Ir1553204ReturnCode ir1553204_api_set_duty_cycle(struct Ir1553204Handler *handler, precise_percentage_t duty_cycle);

/*!
 * \brief Get the frequency of the last PWM measurement
 *
 * \param handler A pointer to the structure handler
 *
 * \returns hertz_t The frequency in Hz, or 0 on error
 */
hertz_t ir1553204_api_get_frequency(struct Ir1553204Handler *handler);

/*!
 * \brief Set the frequency of the last PWM measurement
 *
 * \param handler A pointer to the structure handler
 * \param frequency The frequency
 *
 * \retval IR1153204_NULL_POINTER if any of the parameters is NULL
 * \retval IR1553204_RC_OK otherwise
 */
enum Ir1553204ReturnCode ir1553204_api_set_frequency(struct Ir1553204Handler *handler, hertz_t frequency);

/*!
 * \brief Get the period of the last PWM measurement
 *
 * \param handler A pointer to the structure handler
 *
 * \returns milliseconds_t The period in ms, or 0 on error
 */
milliseconds_t ir1553204_api_get_period(struct Ir1553204Handler *handler);

/*!
 * \brief Get the status of the IR155-3204
 *
 * \param handler A pointer to the structure handler
 *
 * \returns enum Ir1553204Status The current status of the IR155-3204
 */
enum Ir1553204Status ir1553204_api_get_status(struct Ir1553204Handler *handler);

#endif // IR1553204_API_H
