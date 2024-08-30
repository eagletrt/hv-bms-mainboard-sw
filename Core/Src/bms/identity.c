/**
 * @file identity.c
 * @date 2024-06-20
 * @author Antonio Gelain [antonio.gelain2@gmail.com]
 *
 * @brief Module that stores the information about the identity of the current board
 */

#include "identity.h"

#include <time.h>
#include <string.h>

_STATIC _IdentityHandler hidentity;

void identity_init(void) {
    memset(&hidentity, 0U, sizeof(hidentity));

    // TODO: Build time at compile time
    // Get build time
    struct tm tm = { 0 };
    // Ignore warnings from this line
    if (strptime(IDENTITY_BUILD_TIME_STR, "%b %d %Y %H:%M:%S", &tm) != NULL)
        hidentity.build_time = mktime(&tm);

    // Update canlib payload info
    hidentity.mainboard_version_payload.component_build_time = hidentity.build_time;
    hidentity.mainboard_version_payload.canlib_build_time = CANLIB_BUILD_TIME;
    
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        hidentity.cellboard_version_payload[id].cellboard_id = id;
    }
}

seconds_t identity_get_build_time(void) {
    return hidentity.build_time;
}

primary_hv_mainboard_version_converted_t * identity_get_mainboard_version_payload(size_t * byte_size) {
    if (byte_size != NULL)
        *byte_size = sizeof(hidentity.mainboard_version_payload);
    return &hidentity.mainboard_version_payload;
}

primary_hv_cellboard_version_converted_t * identity_get_cellboard_version_payload(CellboardId id, size_t * byte_size) {
    if (id >= CELLBOARD_ID_COUNT)
        return NULL;
    if (byte_size != NULL)
        *byte_size = sizeof(hidentity.cellboard_version_payload[0U]);
    return &hidentity.cellboard_version_payload[id];
}

void identity_cellboard_version_handle(bms_cellboard_version_converted_t * payload) {
    if (payload == NULL || (CellboardId)payload->cellboard_id >= CELLBOARD_ID_COUNT)
        return;
    // Copy version data
    hidentity.cellboard_version_payload[payload->cellboard_id].canlib_build_time = payload->canlib_build_time;
    hidentity.cellboard_version_payload[payload->cellboard_id].component_build_time = payload->component_build_time;
}
