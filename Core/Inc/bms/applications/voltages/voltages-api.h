/*!
 * \file voltages-api.h
 * \date 2026-11-10
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells voltage measurement and control
 */

#ifndef VOLTAGES_API_H
#define VOLTAGES_API_H

#include "voltages.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the voltage module
 */
void voltages_api_init(void);

/*!
 * \brief Set a single voltage value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the voltage to update
 * \param[in] voltage The voltage value
 */
void voltages_api_set_value(enum CellboardId cellboard, uint8_t index, volt voltage);
/*!
 * \brief Reset the updated flags for the voltages of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 */
void voltages_api_reset_voltages_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Reset the updated flags for the global voltage information of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 */
void voltages_api_reset_info_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Set the voltage information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] min The minimum voltage
 * \param[in] max The maximum voltage
 * \param[in] average The average voltage
 * \param[in] sum The sum of the voltages
 */
void voltages_api_set_cellboard_voltage_info(enum CellboardId cellboard, volt min, volt max, volt average, volt sum);

/*!
 * \brief Get the voltage information for a specific cellboard
 *
 * \return The minimum voltage value
 */
struct VoltagesInfo voltages_api_get_cellboard_info(enum CellboardId cellboard);

/*!
 * \brief Get the the voltage information for the entire pack
 *
 * \return The minimum voltage value
 */
struct VoltagesInfo voltages_api_get_pack_info(void);