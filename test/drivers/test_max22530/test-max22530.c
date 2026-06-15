/*!
 * \file test-max22530.c
 * \date 2024-08-02
 *
 * \brief Test functions for the MAX22530 ADC driver
 */

#include "unity.h"
#include "fff.h"
#include "max22530-api.h"
#include <string.h>
#include <stdint.h>

DEFINE_FFF_GLOBALS;

FAKE_VOID_FUNC(mock_send, SpiNetwork, uint8_t *, size_t);
FAKE_VOID_FUNC(mock_send_receive, const SpiNetwork, uint8_t *, uint8_t *, size_t, size_t);

static struct Max22530Handler max22530_handler;

static uint8_t mock_rx_buffer[MAX22530_BURST_BYTE_SIZE];
static uint16_t mock_rx_value;

static void mock_send_receive_custom_fake(SpiNetwork network, uint8_t *tx, uint8_t *rx, size_t tx_size, size_t rx_size) {
    (void)network;
    (void)tx;
    (void)tx_size;
    rx[0U] = (mock_rx_value >> 8U) & 0xFF;
    rx[1U] = mock_rx_value & 0xFF;
    for (size_t i = 2U; i < rx_size; ++i) {
        rx[i] = mock_rx_buffer[i];
    }
}

// --- max22530_api_init ---

void test_max22530_api_init_null_handler_returns_null_pointer() {
    enum Max22530ReturnCode rc = max22530_api_init(NULL, mock_send, mock_send_receive);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_NULL_POINTER, rc, "max22530_api_init() should return NULL_POINTER when handler is NULL");
}

void test_max22530_api_init_null_send_returns_null_pointer() {
    struct Max22530Handler handler;

    enum Max22530ReturnCode rc = max22530_api_init(&handler, NULL, mock_send_receive);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_NULL_POINTER, rc, "max22530_api_init() should return NULL_POINTER when send is NULL");
}

void test_max22530_api_init_null_send_receive_returns_null_pointer() {
    struct Max22530Handler handler;

    enum Max22530ReturnCode rc = max22530_api_init(&handler, mock_send, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_NULL_POINTER, rc, "max22530_api_init() should return NULL_POINTER when send_receive is NULL");
}

void test_max22530_api_init_ok() {
    struct Max22530Handler handler;
    memset(&handler, 0xFF, sizeof(handler));

    enum Max22530ReturnCode rc = max22530_api_init(&handler, mock_send, mock_send_receive);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_OK, rc, "max22530_api_init() should return MAX22530_RC_OK on valid input");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(mock_send, handler.send, "max22530_api_init() should store the send callback");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(mock_send_receive, handler.send_receive, "max22530_api_init() should store the send_receive callback");
}

// --- max22530_api_get_id ---

void test_max22530_api_get_id_null_handler_returns_error() {
    max22530_id id = max22530_api_get_id(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(-1, id, "max22530_api_get_id() should return -1 when handler is NULL");
}

void test_max22530_api_get_id_returns_upper_byte() {
    mock_rx_value = 0xAB00U;

    max22530_id id = max22530_api_get_id(&max22530_handler);

    TEST_ASSERT_EQUAL_MESSAGE((int8_t)0xAB, id, "max22530_api_get_id() should return the upper byte of the ID register");
}

// --- max22530_api_get_power_on_reset ---

void test_max22530_api_get_power_on_reset_null_handler_returns_error() {
    enum Max22530PowerOnReset por = max22530_api_get_power_on_reset(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(-1, por, "max22530_api_get_power_on_reset() should return -1 when handler is NULL");
}

void test_max22530_api_get_power_on_reset_bit_set() {
    mock_rx_value = 0x0080U;

    enum Max22530PowerOnReset por = max22530_api_get_power_on_reset(&max22530_handler);

    TEST_ASSERT_EQUAL_MESSAGE(1, por, "max22530_api_get_power_on_reset() should return 1 when POR bit is set");
}

void test_max22530_api_get_power_on_reset_bit_clear() {
    mock_rx_value = 0x0000U;

    enum Max22530PowerOnReset por = max22530_api_get_power_on_reset(&max22530_handler);

    TEST_ASSERT_EQUAL_MESSAGE(0, por, "max22530_api_get_power_on_reset() should return 0 when POR bit is clear");
}

// --- max22530_api_get_revision ---

void test_max22530_api_get_revision_null_handler_returns_error() {
    max22530_revision rev = max22530_api_get_revision(NULL);

    TEST_ASSERT_EQUAL_MESSAGE(-1, rev, "max22530_api_get_revision() should return -1 when handler is NULL");
}

void test_max22530_api_get_revision_returns_lower_7_bits() {
    mock_rx_value = 0x002AU;

    max22530_revision rev = max22530_api_get_revision(&max22530_handler);

    TEST_ASSERT_EQUAL_MESSAGE((int8_t)0x2A, rev, "max22530_api_get_revision() should return the lower 7 bits of the ID register");
}

// --- max22530_api_read_channel ---

void test_max22530_api_read_channel_null_handler_returns_zero() {
    volt_t result = max22530_api_read_channel(NULL, MAX22530_CHANNEL_1, false);

    TEST_ASSERT_EQUAL_MESSAGE(0U, result, "max22530_api_read_channel() should return 0 when handler is NULL");
}

void test_max22530_api_read_channel_zero_raw_returns_zero_volts() {
    mock_rx_value = 0x0000U;

    volt_t result = max22530_api_read_channel(&max22530_handler, MAX22530_CHANNEL_1, false);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RAW_VALUE_TO_VOLT(0x0000U), result, "max22530_api_read_channel() should convert 0 raw to 0V");
}

void test_max22530_api_read_channel_max_raw_returns_max_volts() {
    mock_rx_value = 0x0FFFU;

    volt_t result = max22530_api_read_channel(&max22530_handler, MAX22530_CHANNEL_1, false);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RAW_VALUE_TO_VOLT(0x0FFFU), result, "max22530_api_read_channel() should convert full-scale raw to max voltage");
}

// --- max22530_api_read_channels_all ---

void test_max22530_api_read_channels_all_null_handler_returns_null_pointer() {
    volt_t out[MAX22530_CHANNEL_COUNT];

    enum Max22530ReturnCode rc = max22530_api_read_channels_all(NULL, false, out, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_NULL_POINTER, rc, "max22530_api_read_channels_all() should return NULL_POINTER when handler is NULL");
}

void test_max22530_api_read_channels_all_null_out_returns_null_pointer() {
    enum Max22530ReturnCode rc = max22530_api_read_channels_all(&max22530_handler, false, NULL, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_NULL_POINTER, rc, "max22530_api_read_channels_all() should return NULL_POINTER when out is NULL");
}

void test_max22530_api_read_channels_all_ok() {
    volt_t out[MAX22530_CHANNEL_COUNT];

    enum Max22530ReturnCode rc = max22530_api_read_channels_all(&max22530_handler, false, out, NULL);

    TEST_ASSERT_EQUAL_MESSAGE(MAX22530_RC_OK, rc, "max22530_api_read_channels_all() should return MAX22530_RC_OK on valid inputs");
}

void test_max22530_api_read_channels_all_writes_interrupt_status() {
    volt_t out[MAX22530_CHANNEL_COUNT];
    uint16_t interrupt_status = 0xDEADU;
    memset(mock_rx_buffer, 0x00, sizeof(mock_rx_buffer));
    mock_rx_buffer[(MAX22530_CHANNEL_COUNT * 2U) - 1U] = 0xBE;
    mock_rx_buffer[(MAX22530_CHANNEL_COUNT * 2U)] = 0xEF;

    max22530_api_read_channels_all(&max22530_handler, false, out, &interrupt_status);

    TEST_ASSERT_NOT_EQUAL_MESSAGE(0xDEADU, interrupt_status, "max22530_api_read_channels_all() should overwrite interrupt_status with burst data");
}

void setUp() {
    RESET_FAKE(mock_send);
    RESET_FAKE(mock_send_receive);
    FFF_RESET_HISTORY();

    memset(mock_rx_buffer, 0x00, sizeof(mock_rx_buffer));
    mock_rx_value = 0x0000U;
    mock_send_receive_fake.custom_fake = mock_send_receive_custom_fake;

    max22530_api_init(&max22530_handler, mock_send, mock_send_receive);
}

void tearDown() {
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_max22530_api_init_null_handler_returns_null_pointer);
    RUN_TEST(test_max22530_api_init_null_send_returns_null_pointer);
    RUN_TEST(test_max22530_api_init_null_send_receive_returns_null_pointer);
    RUN_TEST(test_max22530_api_init_ok);
    RUN_TEST(test_max22530_api_get_id_null_handler_returns_error);
    RUN_TEST(test_max22530_api_get_id_returns_upper_byte);

    RUN_TEST(test_max22530_api_get_power_on_reset_null_handler_returns_error);
    RUN_TEST(test_max22530_api_get_power_on_reset_bit_set);
    RUN_TEST(test_max22530_api_get_power_on_reset_bit_clear);

    RUN_TEST(test_max22530_api_get_revision_null_handler_returns_error);
    RUN_TEST(test_max22530_api_get_revision_returns_lower_7_bits);

    RUN_TEST(test_max22530_api_read_channel_null_handler_returns_zero);
    RUN_TEST(test_max22530_api_read_channel_zero_raw_returns_zero_volts);
    RUN_TEST(test_max22530_api_read_channel_max_raw_returns_max_volts);

    RUN_TEST(test_max22530_api_read_channels_all_null_handler_returns_null_pointer);
    RUN_TEST(test_max22530_api_read_channels_all_null_out_returns_null_pointer);
    RUN_TEST(test_max22530_api_read_channels_all_ok);
    RUN_TEST(test_max22530_api_read_channels_all_writes_interrupt_status);

    return UNITY_END();
}