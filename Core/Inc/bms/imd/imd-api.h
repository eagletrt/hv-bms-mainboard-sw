/*!
 * \file imd-api.h
 * \date 2024-07-04
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Insulation Monitoring Device (IMD) handing functions
 */

#ifndef IMD_API_H
#define IMD_API_H

#include "imd.h"

#ifdef CONF_IMD_MODULE_ENABLE

/*!
 * \brief initialization function for the IMD internal structure handler
 *
 * \param start A pointer to the PWM start callback function
 *
 * \retval IMD_RC_NULL_POINTER if any of the given parameter is NULL
 * \retval IMD_RC_OK otherwise
 */
enum ImdReturnCode imd_init(imd_pwm_start_callback start);

/*!
 * \brief Get the duty cycle percentage of the IMD signal
 *
 * \returns precise_percentage_t The duty cycle percentage from 0 to 1, or 0 on error
 */
precise_percentage_t imd_get_duty_cycle(void);

/*!
 * \brief Get the frequency of the IMD signal
 *
 * \returns hertz_t The frequency in Hz, or 0 on error
 */
hertz_t imd_get_frequency(void);

/*!
 * \brief Get the period of the IMD signal
 *
 * \returns milliseconds_t The period in ms, or 0 on error
 */
milliseconds_t imd_get_period(void);

/*!
 * \brief Get the status of the IMD
 *
 * \returns enum ImdStatus The current status of the IMD
 */
enum ImdStatus imd_get_status(void);

/*!
 * \brief Update the internal handler with a new PWM measurement
 *
 * \param source_frequency The sampling frequency of the signal
 * \param period_count Number of samples of the signal period
 * \param high_count Number of logical high samples of the signal
 *
 * \retval IMD_RC_INVALID_DATA if the parameters are not valid
 * \retval IMD_RC_OK otherwise
 */
enum ImdReturnCode imd_update(ticks_t source_frequency, ticks_t period_count, ticks_t high_count);

/*!
 * \brief Get a pointer to the CAN payload of the IMD
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return primary_hv_imd_status_converted_t* A pointer to the payload
 */
primary_hv_imd_status_converted_t *imd_get_status_canlib_payload(size_t *byte_size);

#ifdef CONF_IMD_STRINGS_ENABLE

const char *const imd_get_imd_status_name(const enum ImdStatus status);

#else // CONF_IMD_STRINGS_ENABLE

#define imd_get_imd_status_name(status) ""

#endif // CONF_IMD_STRINGS_ENABLE

#else // CONF_IMD_MODULE_ENABLE

#define imd_init(start) (IMD_RC_OK)
#define imd_get_duty_cycle() (0U)
#define imd_get_frequency() (0U)
#define imd_get_period() (0U)
#define imd_get_status() (IMD_STATUS_NORMAL)
#define imd_update(source_frequency, period_count, high_count) (IMD_RC_OK)
#define imd_get_status_canlib_payload(byte_size) (NULL)

#endif // CONF_IMD_MODULE_ENABLE

#endif // IMD_API_H
