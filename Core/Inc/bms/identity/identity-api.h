/*!
 * \file identity-api.h
 * \date 2024-06-20
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Module that stores the information about the identity of the current board
 */

#ifndef IDENTITY_API_H
#define IDENTITY_API_H

#include "identity.h"

#ifdef CONF_IDENTITY_MODULE_ENABLE

/*!
 * \brief Initialize all the info about the mainboard identity
 */
void identity_api_init(void);

/*!
 * \brief Get the mainboard software build time in unix timestamp format (seconds)
 *
 * \return seconds_t The build time
 */
seconds_t identity_api_get_build_time(void);

/*!
 * \brief Get a pointer to the canlib payload of the mainboard identity info
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return primary_hv_mainboard_version_converted_t* A pointer to the payload
 */
primary_hv_mainboard_version_converted_t *identity_api_get_mainboard_version_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the canlib payload of the cellboard identity info
 *
 * \param cellboard_id The cellboard to get the payload from
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \return primary_hv_cellboard_version_converted_t* A pointer to the payload or NULL if the id is not valid
 */
primary_hv_cellboard_version_converted_t *identity_api_get_cellboard_version_payload(CellboardId cellboard_id, size_t *byte_size);

/*!
 * \brief Handle the received cellboard version
 *
 * \param payload A pointer to the canlib payload
 */
void identity_api_cellboard_version_handle(bms_cellboard_version_converted_t *payload);

#else // CONF_IDENTITY_MODULE_ENABLE

#define identity_api_init(id) CELLBOARD_NOPE()
#define identity_api_get_build_time() (0U)
#define identity_api_get_mainboard_version_payload(byte_size) (NULL)
#define identity_api_get_cellboard_version_payload(byte_size) (NULL)
#define identity_api_cellboard_version_handle(payload) MAINBOARD_NOPE()

#endif // CONF_IDENTITY_MODULE_ENABLE

#endif // IDENTITY_API_H
