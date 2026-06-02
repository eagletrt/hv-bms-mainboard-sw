/*!
 * \file test-internal-voltage.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Internal voltages measurements and controls
 */

#include "internal-voltage.h"
#include "unity.h"
#include "internal-voltage-api.h"
#include "mainboard-def.h"
#include "volt.h"

#include <string.h>
#include <fff.h>
DEFINE_FFF_GLOBALS;

extern struct InternalVoltageHandler internal_volt_handler;

FAKE_VOID_FUNC(spi_send, const SpiNetwork, uint8_t *const, const size_t);
FAKE_VOID_FUNC(spi_send_receive, const SpiNetwork, uint8_t *const, uint8_t *const, const size_t, const size_t);

void test_internal_voltage_init_ok() {
    struct InternalVoltageHandler expected_handler;
    memset(&expected_handler, 0U, sizeof(expected_handler));
    (void)max22530_init(&expected_handler.max22530, spi_send, spi_send_receive);

    enum InternalVoltageReturnCode rc = internal_voltage_api_init(spi_send, spi_send_receive);

    TEST_ASSERT_EQUAL_MESSAGE(INTERNAL_VOLTAGE_RC_OK, rc, "internal_voltage_init() failed to return INTERNAL_VOLTAGE_OK");
    TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&expected_handler, &internal_volt_handler, sizeof(expected_handler), "internal_voltage_init() did not properly initialize the internal voltage handler");
}

void test_internal_voltage_init_null_pointer_send() {
    enum InternalVoltageReturnCode rc = internal_voltage_api_init(NULL, spi_send_receive);

    TEST_ASSERT_EQUAL_MESSAGE(INTERNAL_VOLTAGE_RC_NULL_POINTER, rc, "internal_voltage_init() should return INTERNAL_VOLTAGE_NULL_POINTER if send callback is NULL");
}

void test_internal_voltage_init_null_pointer_send_receive() {
    enum InternalVoltageReturnCode rc = internal_voltage_api_init(spi_send, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(INTERNAL_VOLTAGE_RC_NULL_POINTER, rc, "internal_voltage_init() should return INTERNAL_VOLTAGE_NULL_POINTER if send_receive callback is NULL");
}

void test_internal_voltage_read_all_ok() {

    enum InternalVoltageReturnCode rc = internal_voltage_api_read_all();

    TEST_ASSERT_EQUAL_MESSAGE(INTERNAL_VOLTAGE_RC_OK, rc, "internal_voltage_read_all() failed to return INTERNAL_VOLTAGE_OK");

    // The content of the handler is not checked as it is just a conversion of the max22530 read function, which is (going to be) tested separately.
}

void test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size() {
    size_t byte_size = 0U;
    primary_hv_ts_voltage_converted_t *payload = internal_voltage_api_get_ts_voltage_canlib_payload(&byte_size);

    TEST_ASSERT_EQUAL_MESSAGE(&internal_volt_handler.ts_voltage_can_payload, payload, "Returned payload pointer does not match internal handler");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof(internal_volt_handler.ts_voltage_can_payload), byte_size, "Returned payload size mismatch");
}

void test_internal_voltage_get_ts_voltage_canlib_payload_null_byte_size() {
    primary_hv_ts_voltage_converted_t *payload = internal_voltage_api_get_ts_voltage_canlib_payload(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(&internal_volt_handler.ts_voltage_can_payload, payload, "Returned payload pointer does not match internal handler");
}

void setUp() {
    RESET_FAKE(spi_send);
    RESET_FAKE(spi_send_receive);
    volt_init();
    internal_voltage_api_init(spi_send, spi_send_receive);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_internal_voltage_init_ok);
    RUN_TEST(test_internal_voltage_init_null_pointer_send);
    RUN_TEST(test_internal_voltage_init_null_pointer_send_receive);
    RUN_TEST(test_internal_voltage_read_all_ok);
    RUN_TEST(test_internal_voltage_get_ts_voltage_canlib_payload_pointer_and_size);
    RUN_TEST(test_internal_voltage_get_ts_voltage_canlib_payload_null_byte_size);

    return UNITY_END();
}