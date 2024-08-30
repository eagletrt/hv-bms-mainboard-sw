/**
 * @file identity.h
 * @date 2024-06-20
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Module that stores the information about the identity of the current board
 */

#ifndef IDENTITY_H
#define IDENTITY_H

#include <stddef.h>

#include "mainboard-def.h"

#include "primary_network.h"
#include "bms_network.h"

/**
 * @brief Definition of the string containing the latest build time
 *
 * @details The build time string is in the following format %b %d %Y %H:%M:%S
 * -> month day year hours:minutes:seconds (e.g. Apr 28 2024 11:45:15)
 */
#define IDENTITY_BUILD_TIME_STR (__DATE__" "__TIME__)

/**
 * @brief Identity module handler structure
 *
 * @warning This structure should never be used outside of this file
 *
 * @param build_time The unix timestamp of the latest build time
 * @param mainboard_version_payload The payload of the canlib message containing the mainboard version
 * @param cellboard_version_payload The payloads of the canlib message containing the cellboards version
 */
typedef struct {
    seconds_t build_time;

    primary_hv_mainboard_version_converted_t mainboard_version_payload;
    primary_hv_cellboard_version_converted_t cellboard_version_payload[CELLBOARD_ID_COUNT];
} _IdentityHandler;

/**
 * @brief Initialize all the info about the mainboard identity
 */
void identity_init(void);

/**
 * @brief Get the mainboard software build time in unix timestamp format (seconds)
 *
 * @return seconds_t The build time
 */
seconds_t identity_get_build_time(void);

/**
 * @brief Get a pointer to the canlib payload of the mainboard identity info
 *
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_mainboard_version_converted_t* A pointer to the payload
 */
primary_hv_mainboard_version_converted_t * identity_get_mainboard_version_payload(size_t * byte_size);

/**
 * @brief Get a pointer to the canlib payload of the cellboard identity info
 *
 * @param id The cellboard to get the payload from
 * @param byte_size[out] A pointer where the size of the payload in bytes is stored (can be NULL)
 *
 * @return primary_hv_cellboard_version_converted_t* A pointer to the payload or NULL if the id is not valid
 */
primary_hv_cellboard_version_converted_t * identity_get_cellboard_version_payload(CellboardId id, size_t * byte_size);

/**
 * @brief Handle the received cellboard version
 *
 * @param payload A pointer to the canlib payload
 */
void identity_cellboard_version_handle(bms_cellboard_version_converted_t * payload);

#endif  // IDENTITY_H
