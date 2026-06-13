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
#include "eagletrt-api.h"

#ifdef CONF_IDENTITY_MODULE_ENABLE

EAGLETRT_STATIC struct IdentityHandler identity_handler;

void identity_api_init(void) {
    memset(&identity_handler, 0U, sizeof(identity_handler));

    // TODO: Build time at compile time
    // Get build time
    struct tm time = { 0 };
    // Ignore warnings from this line
    if (strptime(IDENTITY_BUILD_TIME_STR, "%b %d %Y %H:%M:%S", &time) != NULL) {
        identity_handler.build_time = mktime(&time);
    }

    // Update canlib payload info
    identity_handler.mainboard_version_payload.component_build_time = identity_handler.build_time;
    identity_handler.mainboard_version_payload.canlib_build_time = CANLIB_BUILD_TIME;

    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        identity_handler.cellboard_version_payload[id].cellboard_id = (primary_hv_cellboard_version_cellboard_id)id;
    }
}

seconds_t identity_api_get_build_time(void) {
    return identity_handler.build_time;
}

primary_hv_mainboard_version_converted_t *identity_api_get_mainboard_version_payload(size_t *const byte_size) {
    if (byte_size != NULL) {
        *byte_size = sizeof(identity_handler.mainboard_version_payload);
    }
    return &identity_handler.mainboard_version_payload;
}

primary_hv_cellboard_version_converted_t *identity_api_get_cellboard_version_payload(const CellboardId cellboard_id, size_t *const byte_size) {
    if (cellboard_id >= CELLBOARD_ID_COUNT) {
        return NULL;
    }
    if (byte_size != NULL) {
        *byte_size = sizeof(identity_handler.cellboard_version_payload[0U]);
    }
    return &identity_handler.cellboard_version_payload[cellboard_id];
}

void identity_api_cellboard_version_handle(bms_cellboard_version_converted_t *const payload) {
    if (payload == NULL || (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT) {
        return;
    }
    // Copy version data
    identity_handler.cellboard_version_payload[payload->cellboard_id].canlib_build_time = payload->canlib_build_time;
    identity_handler.cellboard_version_payload[payload->cellboard_id].component_build_time = payload->component_build_time;
}

#ifdef CONF_IDENTITY_STRINGS_ENABLE

EAGLETRT_STATIC char *identity_module_name = "identity";

#endif // CONF_IDENTITY_STRINGS_ENABLE

#endif // CONF_IDENTITY_MODULE_ENABLE
