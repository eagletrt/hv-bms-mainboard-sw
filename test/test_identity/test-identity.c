/*!
 * \file test-identity.c
 * \date 2026-07-10
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Identity management function tests
 */
#include "can-version.h"
#include "mainboard-def.h"
#include "unity.h"
#include "identity-api.h"

#include <stdint.h>
#include <string.h>

extern struct IdentityHandler identity_handler;

void test_identity_api_init(void) {
    identity_api_init();

    TEST_ASSERT_EQUAL_UINT32(identity_handler.build_time, identity_api_get_build_time());
    TEST_ASSERT_EQUAL_UINT32(identity_handler.build_time, identity_handler.libcan_message_version_info.tsacmainboardversioninfo.buildtime);
    TEST_ASSERT_EQUAL_UINT32(can_version_major, identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.major);
    TEST_ASSERT_EQUAL_UINT32(can_version_minor, identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.minor);
    TEST_ASSERT_EQUAL_UINT32(can_version_patch, identity_handler.libcan_message_libcan_version.tsacmainboardlibcanversion.patch);
    TEST_ASSERT_EQUAL_UINT32(can_generation_time, identity_handler.libcan_message_libcan_version_info.tsacmainboardlibcanversioninfo.generationtime);
}

void test_identity_api_get_mainboard_version_payload(void) {
    size_t byte_size = 0U;
    union CanPrimaryMessages *payload = identity_api_get_mainboard_version_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_version, payload);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsacmainboardversion, byte_size);
}

void test_identity_api_get_mainboard_version_info_payload(void) {
    size_t byte_size = 0U;
    union CanPrimaryMessages *payload = identity_api_get_mainboard_version_info_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_version_info, payload);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsacmainboardversioninfo, byte_size);
}

void test_identity_api_get_mainboard_libcan_version_payload(void) {
    size_t byte_size = 0U;
    union CanPrimaryMessages *payload = identity_api_get_mainboard_libcan_version_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_libcan_version, payload);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsacmainboardlibcanversion, byte_size);
}

void test_identity_api_get_mainboard_libcan_version_info_payload(void) {
    size_t byte_size = 0U;
    union CanPrimaryMessages *payload = identity_api_get_mainboard_libcan_version_info_payload(&byte_size);

    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_libcan_version_info, payload);
    TEST_ASSERT_EQUAL_UINT32(can_primary_byte_size_tsacmainboardlibcanversioninfo, byte_size);
}

void test_identity_api_get_payload_null_byte_size(void) {
    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_version, identity_api_get_mainboard_version_payload(NULL));
    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_version_info, identity_api_get_mainboard_version_info_payload(NULL));
    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_libcan_version, identity_api_get_mainboard_libcan_version_payload(NULL));
    TEST_ASSERT_EQUAL_PTR(&identity_handler.libcan_message_libcan_version_info, identity_api_get_mainboard_libcan_version_info_payload(NULL));
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
    RUN_TEST(test_identity_api_get_mainboard_version_info_payload);
    RUN_TEST(test_identity_api_get_mainboard_libcan_version_payload);
    RUN_TEST(test_identity_api_get_mainboard_libcan_version_info_payload);
    RUN_TEST(test_identity_api_get_payload_null_byte_size);

    return UNITY_END();
}
