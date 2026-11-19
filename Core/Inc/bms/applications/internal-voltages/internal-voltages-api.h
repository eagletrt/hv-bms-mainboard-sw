/*!
 * \file internal-voltages-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages (TS, pack) and current measurement and control
 *
 * \details The TS and pack voltages and the current are provided already
 *          converted by the interface layer; the power is computed from the
 *          current and the TS voltage
 */

#ifndef INTERNAL_VOLTAGES_API_H
#define INTERNAL_VOLTAGES_API_H

#include "internal-voltages.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the internal voltages module
 */
void internal_voltages_api_init(void);

/*!
 * \brief Set a single internal voltage value
 *
 * \param[in] index The index of the voltage to update
 * \param[in] voltage The voltage value
 *
 * \retval INTERNAL_VOLTAGES_RC_INVALID_INDEX if the given index is out of range
 * \retval INTERNAL_VOLTAGES_RC_OK otherwise
 */
enum InternalVoltagesReturnCode internal_voltages_api_set_value(enum InternalVoltagesIndex index, volt voltage);

/*!
 * \brief Set the current supplied by the pack
 *
 * \param[in] current The current value
 */
void internal_voltages_api_set_current(ampere current);

/*!
 * \brief Reset the updated flag of the internal voltages
 */
void internal_voltages_api_reset_voltages_updated_flag(void);

/*!
 * \brief Reset the updated flag of the current
 */
void internal_voltages_api_reset_current_updated_flag(void);

/*!
 * \brief Get a single internal voltage value
 *
 * \param[in] index The index of the voltage
 *
 * \return The voltage value, 0 if the index is out of range
 */
volt internal_voltages_api_get_value(enum InternalVoltagesIndex index);

/*!
 * \brief Get the current supplied by the pack
 *
 * \return The current value
 */
ampere internal_voltages_api_get_current(void);

/*!
 * \brief Get the power supplied at the TSAC outlet
 *
 * \return The current times the TS voltage, in kW
 */
kilowatt internal_voltages_api_get_power(void);

/*!
 * \brief Get all the internal electrical information
 *
 * \return The TS and pack voltages, the current and the power
 */
struct InternalVoltagesInfo internal_voltages_api_get_info(void);

/*!
 * \brief Check if every internal voltage has been received at least once
 *
 * \retval true if every internal voltage has been received
 * \retval false otherwise
 */
bool internal_voltages_api_is_voltages_ready(void);

/*!
 * \brief Check if the current has been received at least once
 *
 * \retval true if the current has been received
 * \retval false otherwise
 */
bool internal_voltages_api_is_current_ready(void);

#endif // INTERNAL_VOLTAGES_API_H
