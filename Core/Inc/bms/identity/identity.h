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

#include "can-primary.h"
#include "mainboard-def.h"

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
    seconds_t build_time; /*!< The unix timestamp of the latest build time */

    union CanPrimaryMessages libcan_message_version;
    union CanPrimaryMessages libcan_message_version_info;
    union CanPrimaryMessages libcan_message_libcan_version;
    union CanPrimaryMessages libcan_message_libcan_version_info;

    union CanPrimaryMessages libcan_message_cellboard_version[CELLBOARD_COUNT];
    union CanPrimaryMessages libcan_message_cellboard_version_info[CELLBOARD_COUNT];
    union CanPrimaryMessages libcan_message_cellboard_libcan_version[CELLBOARD_COUNT];
    union CanPrimaryMessages libcan_message_cellboard_libcan_version_info[CELLBOARD_COUNT];
};

#endif // IDENTITY_H
