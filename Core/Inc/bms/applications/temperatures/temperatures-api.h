/*!
 * \file temperatures-api.h
 * \date 2026-11-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Cells temperature measurement and control
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
 * \brief Set a single temperature value
 *
 * \param[in] cellboard The cellboard id
 * \param[in] index The index of the temperature to update
 * \param[in] temperature The temperature value
 */
void temperatures_api_set_value(enum CellboardId cellboard, uint8_t index, celsius temperature);

/*!
 * \brief Reset the updated flags for the temperatures of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 */
void temperatures_api_reset_temperatures_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Reset the updated flags for the temperature information of a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 */
void temperatures_api_reset_info_updated_flags(enum CellboardId cellboard);

/*!
 * \brief Set the temperature information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 * \param[in] min The minimum temperature
 * \param[in] max The maximum temperature
 * \param[in] average The average temperature
 */
void temperatures_api_set_cellboard_temperature_info(enum CellboardId cellboard, celsius min, celsius max, celsius average);

/*!
 * \brief Get the temperature information for a specific cellboard
 *
 * \param[in] cellboard The cellboard id
 *
 * \return The temperature information of the cellboard, all zeros if the cellboard does not exist
 */
struct TemperaturesInfo temperatures_api_get_cellboard_info(enum CellboardId cellboard);

/*!
 * \brief Get the temperature information for the entire pack
 *
 * \return The minimum, maximum and average temperature of the pack
 */
struct TemperaturesInfo temperatures_api_get_pack_info(void);

#endif // TEMPERATURES_API_H
