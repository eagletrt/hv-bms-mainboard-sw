/*!
 * \file identity.c
 * \date 2026-05-22
 * \author Antonio Gelain [antonio.gelain2@gmail.com]
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Module that stores the information about the identity of the current board
 */

#include "identity-api.h"

#include <time.h>
#include <string.h>
#include "can-primary.h"
#include "eagletrt.h"
#include "mainboard-def.h"
#include "can-version.h"

#ifdef CONF_IDENTITY_MODULE_ENABLE

EAGLETRT_STATIC struct IdentityHandler identity_handler;

void identity_api_init(void) {
    memset(&identity_handler, 0U, sizeof(identity_handler));

    // Get build time
    struct tm time = { 0 };
    if (strptime(IDENTITY_BUILD_TIME_STR, "%b %d %Y %H:%M:%S", &time) != NULL) {
        identity_handler.build_time = mktime(&time);
    }

    // Update canlib payload info
    identity_handler.libcan_message_version.tsacmainboardversion.major = 0;
    identity_handler.libcan_message_version.tsacmainboardversion.minor = 1;
    identity_handler.libcan_message_version.tsacmainboardversion.patch = 0;
    identity_handler.libcan_message_version_info.tsacmainboardversioninfo.buildtime = identity_handler.build_time;
    identity_handler.libcan_message_version_info.tsacmainboardversioninfo.commithash = 0;
    identity_handler.libcan_message_version_info.tsacmainboardversioninfo.dirty = 0;

    identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.major = can_version_major;
    identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.minor = can_version_minor;
    identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.patch = can_version_patch;
    identity_handler.libcan_message_libcan_version_info.tsacmainboardlibcanversioninfo.generationtime = can_generation_time;
    identity_handler.libcan_message_libcan_version_info.tsacmainboardlibcanversioninfo.commithash = 0;
    identity_handler.libcan_message_libcan_version_info.tsacmainboardlibcanversioninfo.dirty = 0;
}

seconds_t identity_api_get_build_time(void) {
    return identity_handler.build_time;
}

union CanPrimaryMessages *identity_api_get_mainboard_version_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardversion;
    }
    return &identity_handler.libcan_message_version;
}

union CanPrimaryMessages *identity_api_get_mainboard_version_info_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardversioninfo;
    }
    return &identity_handler.libcan_message_version_info;
}

union CanPrimaryMessages *identity_api_get_mainboard_libcan_version_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardlibcanversion;
    }
    return &identity_handler.libcan_message_libcan_version;
}

union CanPrimaryMessages *identity_api_get_mainboard_libcan_version_info_payload(size_t *byte_size) {
    if (byte_size != NULL) {
        *byte_size = can_primary_byte_size_tsacmainboardlibcanversioninfo;
    }
    return &identity_handler.libcan_message_libcan_version_info;
}

// TODO: Send Libcan Cellboard versions info

#ifdef CONF_IDENTITY_STRINGS_ENABLE

EAGLETRT_STATIC char *identity_module_name = "identity";

#endif // CONF_IDENTITY_STRINGS_ENABLE

#endif // CONF_IDENTITY_MODULE_ENABLE
