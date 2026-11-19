/*!
 * \file temperatures-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells and discharge resistors temperature measurement and control
 *
 * \warning The old code also had a map for the temperatures and their positions, if needed it can be reintroduced in the future
 */

#ifndef TEMPERATURES_API_H
#define TEMPERATURES_API_H

#include "temperatures.h"
#include "mainboard-def.h"

/*!
 * \brief Initialize the temperature module
 */
void temperatures_api_init(void);

/*!
 * \brief Set a single cell temperature value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the cell temperature to update
 * \param[in] temperature The temperature value
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_INVALID_INDEX if the given index is out of range
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_set_cell_temperature(enum CellboardId cellboard, uint8_t index, celsius temperature);

/*!
 * \brief Reset the updated flags for the cell temperatures of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_reset_cell_temperatures_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Reset the updated flags for the cell temperature information of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_reset_cell_temperatures_info_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Set the cell temperature information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] min The minimum cell temperature
 * \param[in] max The maximum cell temperature
 * \param[in] average The average cell temperature
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_set_cell_temperatures_info(enum CellboardId cellboard, celsius min, celsius max, celsius average);

/*!
 * \brief Get the temperature of a single cell sensor
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the sensor in the cellboard
 *
 * \return The cell temperature, 0 if the cellboard or the index does not exist
 */
celsius temperatures_api_get_cell_temperature(enum CellboardId cellboard, uint8_t index);

/*!
 * \brief Get the cell temperature information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The cell temperature information of the cellboard, all zeros if the cellboard does not exist
 */
struct TemperaturesInfo temperatures_api_get_cell_temperatures_info(enum CellboardId cellboard);

/*!
 * \brief Get the cell temperature information for the entire pack
 *
 * \return The minimum, maximum and average cell temperature of the pack
 */
struct TemperaturesInfo temperatures_api_get_pack_cell_temperatures_info(void);

/*!
 * \brief Set a single discharge resistor temperature value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the discharge resistor temperature to update
 * \param[in] temperature The temperature value
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_INVALID_INDEX if the given index is out of range
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_set_discharge_temperature(enum CellboardId cellboard, uint8_t index, celsius temperature);

/*!
 * \brief Reset the updated flags for the discharge resistor temperatures of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \retval TEMPERATURES_RC_INVALID_CELLBOARD if the given cellboard does not exist
 * \retval TEMPERATURES_RC_OK otherwise
 */
enum TemperaturesReturnCode temperatures_api_reset_discharge_temperatures_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Get the temperature of a single discharge resistor
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the discharge resistor in the cellboard
 *
 * \return The discharge resistor temperature, 0 if the cellboard or the index does not exist
 */
celsius temperatures_api_get_discharge_temperature(enum CellboardId cellboard, uint8_t index);

/*!
 * \brief Get the discharge resistor temperature information for a specific cellboard
 *
 * \details The cellboards do not send this information, it is computed from the single values
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The discharge resistor temperature information of the cellboard, all zeros if the cellboard does not exist
 */
struct TemperaturesInfo temperatures_api_get_discharge_temperatures_info(enum CellboardId cellboard);

/*!
 * \brief Get the discharge resistor temperature information for the entire pack
 *
 * \return The minimum, maximum and average discharge resistor temperature of the pack
 */
struct TemperaturesInfo temperatures_api_get_pack_discharge_temperatures_info(void);

#endif // TEMPERATURES_API_H
