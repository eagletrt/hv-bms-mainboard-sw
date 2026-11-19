/*!
 * \file internal-temperatures-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal temperatures measurement and control
 *
 * \details Temperatures of the linear regulator mosfet, of the precharge
 *          resistors and of the cooling liquid (inlet and outlets),
 *          provided already converted by the interface layer
 */

#ifndef INTERNAL_TEMPERATURES_API_H
#define INTERNAL_TEMPERATURES_API_H

#include "internal-temperatures.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the internal temperatures module
 */
void internal_temperatures_api_init(void);

/*!
 * \brief Set a single internal temperature value
 *
 * \param[in] index The index of the temperature to update
 * \param[in] temperature The temperature value
 *
 * \retval INTERNAL_TEMPERATURES_RC_INVALID_INDEX if the given index is out of range
 * \retval INTERNAL_TEMPERATURES_RC_OK otherwise
 */
enum InternalTemperaturesReturnCode internal_temperatures_api_set_value(enum InternalTemperaturesIndex index, celsius temperature);

/*!
 * \brief Reset the updated flag of the internal temperatures
 */
void internal_temperatures_api_reset_temperatures_updated_flag(void);

/*!
 * \brief Get a single internal temperature value
 *
 * \param[in] index The index of the temperature
 *
 * \return The temperature value, 0 if the index is out of range
 */
celsius internal_temperatures_api_get_value(enum InternalTemperaturesIndex index);

/*!
 * \brief Check if every internal temperature has been received at least once
 *
 * \retval true if every internal temperature has been received
 * \retval false otherwise
 */
bool internal_temperatures_api_is_ready(void);

#endif // INTERNAL_TEMPERATURES_API_H
