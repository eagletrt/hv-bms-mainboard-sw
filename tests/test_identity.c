#include "unity.h"
#include "identity.h"
#include <stdio.h>

extern _IdentityHandler hidentity;

void setUp() {}

void tearDown() {}

void test_identity_init_cellboard_id() {
    identity_init();
    for (CellboardId id = CELLBOARD_ID_0; id < CELLBOARD_ID_COUNT; ++id) {
        TEST_ASSERT_EQUAL(id, hidentity.cellboard_version_payload[id].cellboard_id);
    }
}

void test_identity_get_mainboard_version_payload_size() {
    identity_init();
    size_t size;

    identity_get_mainboard_version_payload(&size);
    TEST_ASSERT_EQUAL(sizeof(hidentity.mainboard_version_payload), size);
}

void test_identity_get_mainboard_version_payload_pointer() {
    identity_init();
    primary_hv_mainboard_version_converted_t *payload;

    payload = identity_get_mainboard_version_payload(NULL);
    TEST_ASSERT_EQUAL(&hidentity.mainboard_version_payload, payload);
}

void test_identity_get_cellboard_version_payload_size() {
    identity_init();
    size_t size;

    identity_get_cellboard_version_payload(CELLBOARD_ID_0, &size);
    TEST_ASSERT_EQUAL(sizeof(hidentity.cellboard_version_payload[0U]), size);
}

void test_identity_get_cellboard_version_payload_pointer() {
    identity_init();
    primary_hv_cellboard_version_converted_t *payload;

    payload = identity_get_cellboard_version_payload(CELLBOARD_ID_0, NULL);
    TEST_ASSERT_EQUAL(&hidentity.cellboard_version_payload[0U], payload);
}

void test_identity_get_cellboard_version_payload_pointer_invalid_id() {
    identity_init();
    primary_hv_cellboard_version_converted_t *payload;

    payload = identity_get_cellboard_version_payload(CELLBOARD_ID_COUNT, NULL);
    TEST_ASSERT_NULL(payload);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_identity_init_cellboard_id);

    RUN_TEST(test_identity_get_mainboard_version_payload_size);
    RUN_TEST(test_identity_get_mainboard_version_payload_pointer);

    RUN_TEST(test_identity_get_cellboard_version_payload_size);
    RUN_TEST(test_identity_get_cellboard_version_payload_pointer);
    RUN_TEST(test_identity_get_cellboard_version_payload_pointer_invalid_id);

    return UNITY_END();
}
