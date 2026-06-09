/*!
 * \file identity.h
 * \date 2024-06-20
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Module that stores the information about the identity of the current board
 */

#ifndef IDENTITY_H
#define IDENTITY_H

#include <stddef.h>

#include "mainboard-conf.h"
#include "mainboard-def.h"

#include "primary_network.h"
#include "bms_network.h"

/*!
 * \brief Definition of the string containing the latest build time
 *
 * \details The build time string is in the following format %b %d %Y %H:%M:%S
 * -> month day year hours:minutes:seconds (e.g. Apr 28 2024 11:45:15)
 */
#define IDENTITY_BUILD_TIME_STR (__DATE__ " "__TIME__)

/*!
 * \brief Identity module handler structure
 *
 * \warning This structure should never be used outside of this file
 */
struct IdentityHandler {
    seconds_t build_time;                                                                   /*!< The unix timestamp of the latest build time */
    primary_hv_mainboard_version_converted_t mainboard_version_payload;                     /*!< The payload of the canlib message containing the mainboard version */
    primary_hv_cellboard_version_converted_t cellboard_version_payload[CELLBOARD_ID_COUNT]; /*!< The payloads of the canlib message containing the cellboards version */
};

#endif // IDENTITY_H
