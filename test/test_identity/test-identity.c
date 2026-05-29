#include "mainboard-def.h"
#include "primary_network.h"
#include "unity.h"
#include "identity-api.h"

#include <stdint.h>
#include <string.h>

extern struct IdentityHandler identity_handler;

void test_identity_api_init(void) {
    struct IdentityHandler expected_handler = { 0 };

    expected_handler.build_time = identity_api_get_build_time();
    expected_handler.mainboard_version_payload.component_build_time = expected_handler.build_time;
    expected_handler.mainboard_version_payload.canlib_build_time = CANLIB_BUILD_TIME;
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        expected_handler.cellboard_version_payload[id].cellboard_id = (primary_hv_cellboard_version_cellboard_id)id;
    }

    identity_api_init();

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &identity_handler, sizeof(expected_handler), "Identity handler not initialized correctly");
}

void test_identity_api_get_mainboard_version_payload(void) {
    size_t byte_size = 0U;
    primary_hv_mainboard_version_converted_t *payload = identity_api_get_mainboard_version_payload(&byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard version payload pointer is NULL");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(identity_handler.mainboard_version_payload), byte_size, "Mainboard version payload byte size is incorrect");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&identity_handler.mainboard_version_payload, payload, sizeof(identity_handler.mainboard_version_payload), "Mainboard version payload content is incorrect");
}

void test_identity_api_get_cellboard_version_payload(void) {
    size_t byte_size = 0U;
    primary_hv_cellboard_version_converted_t *payload = identity_api_get_cellboard_version_payload(CELLBOARD_ID_0, &byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard version payload pointer is NULL");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sizeof(identity_handler.cellboard_version_payload[CELLBOARD_ID_0]), byte_size, "Cellboard version payload byte size is incorrect");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&identity_handler.cellboard_version_payload[CELLBOARD_ID_0], payload, sizeof(identity_handler.cellboard_version_payload[CELLBOARD_ID_0]), "Cellboard version payload content is incorrect");
}

void test_identity_api_get_cellboard_version_payload_invalid_id(void) {
    size_t byte_size = 0U;
    primary_hv_cellboard_version_converted_t *payload = identity_api_get_cellboard_version_payload(CELLBOARD_ID_COUNT, &byte_size);

    TEST_ASSERT_NULL_MESSAGE(payload, "Cellboard version payload pointer should be NULL for invalid id");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0U, byte_size, "Cellboard version payload byte size should be 0 for invalid id");
}

void test_identity_api_cellboard_version_handle(void) {
    bms_cellboard_version_converted_t payload = { 0 };
    payload.cellboard_id = CELLBOARD_ID_0;
    payload.canlib_build_time = 123456789U;
    payload.component_build_time = 987654321U;

    identity_api_cellboard_version_handle(&payload);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&payload.canlib_build_time, &identity_handler.cellboard_version_payload[payload.cellboard_id].canlib_build_time, sizeof(payload.canlib_build_time), "Cellboard version canlib build time not updated correctly");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&payload.component_build_time, &identity_handler.cellboard_version_payload[payload.cellboard_id].component_build_time, sizeof(payload.component_build_time), "Cellboard version component build time not updated correctly");
}

void test_identity_api_cellboard_version_handle_invalid_payload(void) {
    // Save a copy of the current handler to check that it is not modified
    struct IdentityHandler expected_handler = identity_handler;

    identity_api_cellboard_version_handle(NULL);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &identity_handler, sizeof(expected_handler), "Identity handler should not be modified when handling NULL payload");

    bms_cellboard_version_converted_t invalid_payload = { 0 };
    invalid_payload.cellboard_id = CELLBOARD_ID_COUNT; // Invalid id

    identity_api_cellboard_version_handle(&invalid_payload);

    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &identity_handler, sizeof(expected_handler), "Identity handler should not be modified when handling payload with invalid id");
}

void test_get_mainboard_version_payload_null_byte_size(void) {
    primary_hv_mainboard_version_converted_t *payload = identity_api_get_mainboard_version_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard version payload pointer is NULL when byte_size is NULL");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&identity_handler.mainboard_version_payload, payload, sizeof(identity_handler.mainboard_version_payload), "Mainboard version payload content is incorrect when byte_size is NULL");
}

void test_get_cellboard_version_payload_null_byte_size(void) {
    primary_hv_cellboard_version_converted_t *payload = identity_api_get_cellboard_version_payload(CELLBOARD_ID_0, NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard version payload pointer is NULL when byte_size is NULL");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&identity_handler.cellboard_version_payload[CELLBOARD_ID_0], payload, sizeof(identity_handler.cellboard_version_payload[CELLBOARD_ID_0]), "Cellboard version payload content is incorrect when byte_size is NULL");
}

void setUp() {
    identity_api_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_identity_api_init);
    RUN_TEST(test_identity_api_get_mainboard_version_payload);
    RUN_TEST(test_identity_api_get_cellboard_version_payload);
    RUN_TEST(test_identity_api_get_cellboard_version_payload_invalid_id);
    RUN_TEST(test_identity_api_cellboard_version_handle);
    RUN_TEST(test_identity_api_cellboard_version_handle_invalid_payload);
    RUN_TEST(test_get_mainboard_version_payload_null_byte_size);
    RUN_TEST(test_get_cellboard_version_payload_null_byte_size);

    return UNITY_END();
}
