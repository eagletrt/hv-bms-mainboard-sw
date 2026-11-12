/*!
 * \file sensors-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Sensors handling (SD, IMD, AIRs, precharge, ...)
 *
 * \details Value pipeline: the sensing interface pushes the raw samples into
 *          a circular buffer per sensor, then \ref sensors_api_routine filters
 *          them, checks the filtered values and updates the mainboard status
 *          (conditions and faults owned by this module)
 */

#ifndef SENSORS_API_H
#define SENSORS_API_H

#include <stdbool.h>

#include "sensors.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the sensors API
 */
void sensors_api_init(void);

/*!
 * \brief Set the value of a digital sensor
 *
 * \param index The index of the digital sensor
 * \param value The value to set
 * \return The result of the operation
 */
enum SensorsReturnCode sensors_api_set_digital_value(enum SensorsDigitalIndex index, bool value);

/*!
 * \brief Set the value of an analog sensor
 *
 * \param index The index of the analog sensor
 * \param value The value to set
 * \return The result of the operation
 */
enum SensorsReturnCode sensors_api_set_analog_value(enum SensorsAnalogIndex index, volt value);

/*!
 * \brief Run the sensors API routine, this function runs the filtering and updates the mainboard status accordingly
 *
 * \retval SENSORS_RC_OK if the routine executed successfully
 */
enum SensorsReturnCode sensors_api_routine(void);

#endif // SENSORS_API_H
