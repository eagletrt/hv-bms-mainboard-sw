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

#include "can-primary.h"
#include "identity.h"
#include "mainboard-conf.h"

#ifdef CONF_IDENTITY_MODULE_ENABLE

/*!
 * \brief Initialize all the info about the mainboard identity
 */
void identity_api_init(void);

/*!
 * \brief Get the mainboard software build time in unix timestamp format (seconds)
 *
 * \returns seconds_t The build time
 */
seconds_t identity_api_get_build_time(void);

/*!
 * \brief Get a pointer to the payload of the mainboard version
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *identity_api_get_mainboard_version_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the payload of the mainboard version info
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *identity_api_get_mainboard_version_info_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the payload of the mainboard libcan version
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *identity_api_get_mainboard_libcan_version_payload(size_t *byte_size);

/*!
 * \brief Get a pointer to the canlib payload of the mainboard identity version info
 *
 * \param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * \returns Pointer to the payload
 */
union CanPrimaryMessages *identity_api_get_mainboard_libcan_version_info_payload(size_t *byte_size);

#else // CONF_IDENTITY_MODULE_ENABLE

#define identity_api_init(id) EAGLETRT_API_NOP()
#define identity_api_get_build_time() (0U)
#define identity_api_get_mainboard_version_payload(byte_size) (NULL)
#define identity_api_get_mainboard_version_info_payload(byte_size) (NULL)
#define identity_api_get_mainboard_libcan_version_payload(byte_size) (NULL)
#define identity_api_get_mainboard_libcan_version_info_payload(byte_size) (NULL)
#define identity_api_get_cellboard_version_payload(byte_size) (NULL)
#define identity_api_cellboard_version_handle(payload) EAGLETRT_API_NOP()

#endif // CONF_IDENTITY_MODULE_ENABLE

#endif // IDENTITY_API_H
