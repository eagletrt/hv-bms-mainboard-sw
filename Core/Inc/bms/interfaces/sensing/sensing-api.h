/*!
 * \file sensing-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Low level sensing (ADC, GPIO, interrupts) that provides the physical values
 *
 * \details The module reads the hardware through the callbacks given at init,
 *          converts the readings and pushes them to the application modules:
 *          - GPIOs -> feedback (digital, raw pin level)
 *          - MCU ADCs -> feedback (analog, pin voltage) and internal
 *            temperatures (cooling, NTC converted)
 *          - external ADC (MAX22530) -> internal voltages (TS and pack)
 *          - IMD PWM (IR155-3204) -> decoded by the driver, read with the getters
 *
 *          Expected HAL glue:
 *          - HAL_ADC_ConvCpltCallback -> \ref sensing_api_adc_conversion_complete
 *          - HAL_TIM_IC_CaptureCallback (IMD timer) -> \ref sensing_api_imd_capture
 */

#ifndef SENSING_API_H
#define SENSING_API_H

#include "mainboard-def.h"
#include "sensing.h"

/*!
 * \brief Initialize the sensing module and its drivers, then start the IMD capture
 *
 * \details Initializes the freshness watchdogs, they are started by the first
 *          \ref sensing_api_routine
 *
 * \attention Must be called once, after \ref mainboard_watchdogs_api_init
 *
 * \param data The hardware callbacks
 *
 * \retval SENSING_RC_ERROR if the module is already initialized or a watchdog could not be initialized
 * \retval SENSING_RC_NULL_POINTER if data or any of its callbacks is NULL
 * \retval SENSING_RC_HARDWARE_ERROR if a driver could not be initialized
 * \retval SENSING_RC_OK otherwise
 */
enum SensingReturnCode sensing_api_init(const struct SensingInitData *data);

/*!
 * \brief Read all the sensors and push the values to the application modules
 *
 * \details Reads every GPIO, processes the MCU ADC samples of the last
 *          completed conversion and starts a new one, reads the external ADC.
 *          The first call starts the freshness watchdogs, every call pets the
 *          ones whose source delivered: a processed MCU ADC conversion, a
 *          successful external ADC read, a valid IMD capture since the last call.
 *          A watchdog that times out latches its STALE fault
 *
 * \attention The SPI transfer with the external ADC is blocking
 * \attention Must be called from the same context as \ref mainboard_watchdogs_api_routine
 *
 * \retval SENSING_RC_NOT_INITIALIZED if the module was not initialized
 * \retval SENSING_RC_HARDWARE_ERROR if an ADC conversion could not be started or the external ADC could not be read
 * \retval SENSING_RC_OK otherwise
 */
enum SensingReturnCode sensing_api_routine(void);

/*!
 * \brief Notify that the scan conversion of an MCU ADC is complete
 *
 * \details Meant to be called from interrupt context, the samples are
 *          processed in the next \ref sensing_api_routine
 *
 * \param adc The ADC whose conversion is complete
 *
 * \retval SENSING_RC_INVALID_INDEX if the ADC is not valid
 * \retval SENSING_RC_OK otherwise
 */
enum SensingReturnCode sensing_api_adc_conversion_complete(enum SensingAdc adc);

/*!
 * \brief Update the IMD PWM measurement with the captured timer values
 *
 * \details Meant to be called from interrupt context. A valid capture is
 *          flagged for the next \ref sensing_api_routine, which pets the IMD watchdog
 *
 * \param source_frequency The frequency of the timer counter in Hz
 * \param period_count The counter ticks of a whole PWM period
 * \param high_count The counter ticks while the PWM is high
 *
 * \retval SENSING_RC_INVALID_DATA if the period is 0 or shorter than the high time
 * \retval SENSING_RC_OK otherwise
 */
enum SensingReturnCode sensing_api_imd_capture(ticks source_frequency, ticks period_count, ticks high_count);

/*!
 * \brief Get the status of the IMD decoded from the PWM frequency
 *
 * \returns The IMD status, IR1553204_STATUS_UNKNOWN if not valid
 */
enum Ir1553204Status sensing_api_get_imd_status(void);

/*!
 * \brief Get the frequency of the IMD PWM
 *
 * \returns The frequency in Hz
 */
hertz sensing_api_get_imd_frequency(void);

/*!
 * \brief Get the duty cycle of the IMD PWM
 *
 * \returns The duty cycle from 0 to 1
 */
precise_percentage sensing_api_get_imd_duty_cycle(void);

#endif // SENSING_API_H
