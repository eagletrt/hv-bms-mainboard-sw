#include "unity.h"
#include "identity.h"

#include <stdint.h>
#include <string.h>

void test_identity_build_time_matches_mainboard_payload_component_build_time() {
    size_t byte_size = 0U;
    seconds_t build_time = identity_get_build_time();

    primary_hv_mainboard_version_converted_t *payload = identity_get_mainboard_version_payload(&byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard payload pointer should not be NULL");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)build_time, (uint32_t)payload->component_build_time, "Mainboard component build time must match identity build time");
}

void test_identity_get_mainboard_version_payload_sets_byte_size() {
    size_t byte_size = 0U;

    primary_hv_mainboard_version_converted_t *payload = identity_get_mainboard_version_payload(&byte_size);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard payload pointer should not be NULL");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(sizeof(primary_hv_mainboard_version_converted_t), byte_size, "Mainboard payload byte size mismatch");
}

void test_identity_get_mainboard_version_payload_canlib_build_time_matches_macro() {
    primary_hv_mainboard_version_converted_t *payload = identity_get_mainboard_version_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard payload pointer should not be NULL");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE((uint32_t)CANLIB_BUILD_TIME, (uint32_t)payload->canlib_build_time, "Mainboard canlib build time should match CANLIB_BUILD_TIME");
}

void test_identity_get_mainboard_version_payload_null_size_ptr_ok() {
    primary_hv_mainboard_version_converted_t *payload = identity_get_mainboard_version_payload(NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Mainboard payload pointer should not be NULL when byte_size is NULL");
}

void test_identity_get_cellboard_version_payload_valid_ids_return_non_null() {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(id, NULL);

        TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard payload pointer should not be NULL for valid id");
    }
}

void test_identity_get_cellboard_version_payload_valid_ids_set_expected_byte_size() {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        size_t byte_size = 0U;
        primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(id, &byte_size);

        TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard payload pointer should not be NULL for valid id");
        TEST_ASSERT_EQUAL_UINT_MESSAGE(sizeof(primary_hv_cellboard_version_converted_t), byte_size, "Cellboard payload byte size mismatch");
    }
}

void test_identity_get_cellboard_version_payload_valid_ids_set_cellboard_id_field() {
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(id, NULL);

        TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard payload pointer should not be NULL for valid id");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE((uint8_t)id, (uint8_t)payload->cellboard_id, "Cellboard id field should be initialized to its index");
    }
}

void test_identity_get_cellboard_version_payload_invalid_id_returns_null() {
    size_t byte_size = 0xA5A5U;

    primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload((CellboardId)CELLBOARD_ID_COUNT, &byte_size);

    TEST_ASSERT_NULL_MESSAGE(payload, "Invalid cellboard id should return NULL");
}

void test_identity_get_cellboard_version_payload_invalid_id_keeps_byte_size_unchanged() {
    size_t byte_size = 0xA5A5U;

    primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload((CellboardId)CELLBOARD_ID_COUNT, &byte_size);

    TEST_ASSERT_NULL_MESSAGE(payload, "Invalid cellboard id should return NULL");
    TEST_ASSERT_EQUAL_UINT_MESSAGE(0xA5A5U, byte_size, "byte_size should remain unchanged on invalid id");
}

void test_identity_get_cellboard_version_payload_null_size_ptr_ok() {
    primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(CELLBOARD_ID_0, NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard payload pointer should not be NULL when byte_size is NULL");
}

void test_identity_get_cellboard_version_payload_null_size_ptr_preserves_cellboard_id() {
    primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(CELLBOARD_ID_0, NULL);

    TEST_ASSERT_NOT_NULL_MESSAGE(payload, "Cellboard payload pointer should not be NULL when byte_size is NULL");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE((uint8_t)CELLBOARD_ID_0, (uint8_t)payload->cellboard_id, "Cellboard id should be correctly initialized");
}

void test_identity_cellboard_version_handle_null_payload_no_changes() {
    primary_hv_cellboard_version_converted_t before[CELLBOARD_ID_COUNT];
    primary_hv_cellboard_version_converted_t after[CELLBOARD_ID_COUNT];

    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(id, NULL);
        TEST_ASSERT_NOT_NULL(payload);
        memcpy(&before[id], payload, sizeof(before[id]));
    }

    identity_cellboard_version_handle(NULL);

    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        primary_hv_cellboard_version_converted_t *payload = identity_get_cellboard_version_payload(id, NULL);
        TEST_ASSERT_NOT_NULL(payload);
        memcpy(&after[id], payload, sizeof(after[id]));
        TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&before[id], &after[id], sizeof(before[id]), "State should not change when payload is NULL");
    }
}

#ifdef IDENTITY_TESTS

void setUp() {
    identity_init();
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_identity_build_time_matches_mainboard_payload_component_build_time);
    RUN_TEST(test_identity_get_mainboard_version_payload_sets_byte_size);
    RUN_TEST(test_identity_get_mainboard_version_payload_canlib_build_time_matches_macro);
    RUN_TEST(test_identity_get_mainboard_version_payload_null_size_ptr_ok);

    RUN_TEST(test_identity_get_cellboard_version_payload_valid_ids_return_non_null);
    RUN_TEST(test_identity_get_cellboard_version_payload_valid_ids_set_expected_byte_size);
    RUN_TEST(test_identity_get_cellboard_version_payload_valid_ids_set_cellboard_id_field);
    RUN_TEST(test_identity_get_cellboard_version_payload_invalid_id_returns_null);
    RUN_TEST(test_identity_get_cellboard_version_payload_invalid_id_keeps_byte_size_unchanged);
    RUN_TEST(test_identity_get_cellboard_version_payload_null_size_ptr_ok);
    RUN_TEST(test_identity_get_cellboard_version_payload_null_size_ptr_preserves_cellboard_id);

    RUN_TEST(test_identity_cellboard_version_handle_null_payload_no_changes);

    return UNITY_END();
}

#endif // IDENTITY_TESTS
