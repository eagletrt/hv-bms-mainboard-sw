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
 * \brief Initialize the voltages API
 * 
 * \retval VOLTAGES_RC_OK if the initialization was successful
 */
enum VoltagesReturnCode voltages_api_init(void);

/*!
 * \brief Set a single voltage value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the voltage to update
 * \param[in] voltage The voltage value
 *
 * \retval VOLTAGES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval VOLTAGES_RC_INVALID_INDEX if the given index is out of range
 * \retval VOLTAGES_RC_OK otherwise
 */
enum VoltagesReturnCode voltages_api_set_value(enum CellboardId cellboard, uint8_t index, volt voltage);

/*!
 * \brief Set the voltage information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] min The minimum voltage
 * \param[in] max The maximum voltage
 * \param[in] average The average voltage
 * \param[in] sum The sum of the voltages
 *
 * \retval VOLTAGES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval VOLTAGES_RC_OK otherwise
 */
enum VoltagesReturnCode voltages_api_set_cellboard_voltage_info(enum CellboardId cellboard, volt min, volt max, volt average, volt sum);

/*!
 * \brief Get the voltage of a single cell
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the cell in the cellboard
 *
 * \return The cell voltage, 0 if the cellboard or the index does not exist
 */
volt voltages_api_get_value(enum CellboardId cellboard, uint8_t index);

/*!
 * \brief Get the voltage information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The voltage information of the cellboard, all zeros if the cellboard does not exist
 */
struct VoltagesInfo voltages_api_get_cellboard_info(enum CellboardId cellboard);

/*!
 * \brief Get the voltage information for the entire pack
 *
 * \return The minimum, maximum, average and total voltage of the pack
 */
struct VoltagesInfo voltages_api_get_pack_info(void);

/*!
 * \brief Check if the voltage information of every cellboard has been received at least once
 *
 * \details The pack information is meaningful only then: before, the cellboards that
 *          did not report yet count as 0V
 *
 * \retval true if every cellboard has sent its voltage information
 * \retval false otherwise
 */
bool voltages_api_is_ready(void);

#endif // VOLTAGES_API_H
