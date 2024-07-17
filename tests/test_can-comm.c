#include "unity.h"
#include "can-comm.h"
#include <stdio.h>

extern _CanCommHandler hcan_comm;


bool _send_called = false;
const uint8_t *_data;
size_t _size;

CanCommReturnCode _can_comm_send(CanNetwork network, can_id_t id, CanFrameType type, const uint8_t * data, size_t size) {
    _send_called = true;
    _data = data;
    _size = size;
    return CAN_COMM_OK;
}

void setUp() {
    can_comm_init(_can_comm_send);
    _send_called = false;
    _data = NULL;
    _size = 0;
}

void tearDown() {}

void test_can_comm_init_ok() {
    CanCommReturnCode ret = can_comm_init(_can_comm_send);
    TEST_ASSERT_EQUAL(CAN_COMM_OK, ret);
}

void test_can_comm_init_null() {
    CanCommReturnCode ret = can_comm_init(NULL);
    TEST_ASSERT_EQUAL(CAN_COMM_NULL_POINTER, ret);
}

void test_can_comm_init_send() {
    TEST_ASSERT_EQUAL(_can_comm_send, hcan_comm.send);
}

void test_can_comm_init_disabled() {
    TEST_ASSERT_FALSE(hcan_comm.enabled);
}

void test_can_comm_enable_all() {
    can_comm_enable_all();
    TEST_ASSERT_EQUAL(CAN_COMM_ENABLED_ALL_MASK & hcan_comm.enabled,
            CAN_COMM_ENABLED_ALL_MASK);
}

void test_can_comm_is_enabled_all() {
    can_comm_enable_all();
    TEST_ASSERT_TRUE(can_comm_is_enabled_all());
}

void test_can_comm_enable_invalid() {
    bit_flag8_t enabled = hcan_comm.enabled;
    can_comm_enable(8);
    TEST_ASSERT_EQUAL(enabled, hcan_comm.enabled);
}

void test_can_comm_enable() {
    can_comm_enable(0);
    TEST_ASSERT_EQUAL(1, hcan_comm.enabled);
}

void test_can_comm_disable_invalid() {
    bit_flag8_t enabled = hcan_comm.enabled;
    can_comm_disable(8);
    TEST_ASSERT_EQUAL(enabled, hcan_comm.enabled);
}

void test_can_comm_disable() {
    can_comm_enable(0);
    can_comm_disable(0);
    TEST_ASSERT_EQUAL(0, hcan_comm.enabled);
}

void test_can_comm_is_enabled_invalid() {
    TEST_ASSERT_FALSE(can_comm_is_enabled(8));
}

void test_can_comm_is_enabled() {
    can_comm_enable(0);
    TEST_ASSERT_TRUE(can_comm_is_enabled(0));
}

void test_can_comm_send_immediate_disabled() {
    can_comm_disable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_DISABLED, ret); 
}

void test_can_comm_send_immediate_invalid_network() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_COUNT, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_NETWORK, ret); 
}

void test_can_comm_send_immediate_invalid_index_bms() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, bms_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_send_immediate_invalid_index_primary() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_PRIMARY, primary_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_send_immediate_invalid_frame_type() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_COUNT, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_FRAME_TYPE, ret); 
}

void test_can_comm_send_immediate_null() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_NULL_POINTER, ret); 
}

void test_can_comm_send_immediate_sended() {
    uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    can_comm_enable_all();
    can_comm_send_immediate(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, sizeof(data));
    TEST_ASSERT_TRUE(_send_called);
}

void test_can_comm_tx_add_disabled() {
    can_comm_disable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_DISABLED, ret); 
}

void test_can_comm_tx_add_invalid_network() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_COUNT, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_NETWORK, ret); 
}

void test_can_comm_tx_add_invalid_index_bms() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, bms_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_tx_add_invalid_index_primary() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_PRIMARY, primary_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_tx_add_invalid_frame_type() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_COUNT, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_FRAME_TYPE, ret); 
}

void test_can_comm_tx_add_null() {
    can_comm_enable(CAN_COMM_TX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_NULL_POINTER, ret); 
}

void test_can_comm_tx_add_ok() {
    uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, sizeof(data));
    TEST_ASSERT_EQUAL(CAN_COMM_OK, ret);
}

void test_can_comm_tx_add_added() {
    uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    can_comm_enable_all();
    can_comm_tx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, sizeof(data));

    CanMessage tx_msg;
    ring_buffer_pop_front(&hcan_comm.tx_buf, &tx_msg);
    
    TEST_ASSERT_EQUAL_MEMORY(data, tx_msg.payload.tx, sizeof(data));
}

void test_can_comm_rx_add_disabled() {
    can_comm_disable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_DISABLED, ret); 
}

void test_can_comm_rx_add_invalid_network() {
    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_COUNT, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_NETWORK, ret); 
}

void test_can_comm_rx_add_invalid_index_bms() {
    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, bms_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_rx_add_invalid_index_primary() {
    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_PRIMARY, primary_MESSAGE_COUNT, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_INDEX, ret); 
}

void test_can_comm_rx_add_invalid_frame_type() {
    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_COUNT, (void*)1, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_FRAME_TYPE, ret); 
}

void test_can_comm_rx_add_invalid_payload_size() {

    uint8_t data[CAN_COMM_INVALID_PAYLOAD_SIZE+1];

    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, CAN_COMM_MAX_PAYLOAD_BYTE_SIZE+1);
    TEST_ASSERT_EQUAL(CAN_COMM_INVALID_PAYLOAD_SIZE, ret); 
}

void test_can_comm_rx_add_null() {
    can_comm_enable(CAN_COMM_RX_ENABLE_BIT);
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, NULL, 0);
    TEST_ASSERT_EQUAL(CAN_COMM_NULL_POINTER, ret); 
}

void test_can_comm_rx_add_ok() {
    uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    can_comm_enable_all();
    CanCommReturnCode ret = can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, sizeof(data));
    TEST_ASSERT_EQUAL(CAN_COMM_OK, ret);
}

void test_can_comm_rx_add_added() {
    uint8_t data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    can_comm_enable_all();
    can_comm_rx_add(CAN_NETWORK_BMS, 0, CAN_FRAME_TYPE_DATA, data, sizeof(data));

    CanMessage rx_msg;
    ring_buffer_pop_front(&hcan_comm.rx_buf, &rx_msg);
    
    TEST_ASSERT_EQUAL_MEMORY(data, rx_msg.payload.rx, sizeof(data));
}

// TODO: can_comm_routine

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_can_comm_init_ok);
    RUN_TEST(test_can_comm_init_null);
    RUN_TEST(test_can_comm_init_send);
    RUN_TEST(test_can_comm_init_disabled);

    RUN_TEST(test_can_comm_enable_all);

    RUN_TEST(test_can_comm_is_enabled_all);

    RUN_TEST(test_can_comm_enable_invalid);
    RUN_TEST(test_can_comm_enable);

    RUN_TEST(test_can_comm_disable_invalid);
    RUN_TEST(test_can_comm_disable);

    RUN_TEST(test_can_comm_is_enabled_invalid);
    RUN_TEST(test_can_comm_is_enabled);

    RUN_TEST(test_can_comm_send_immediate_disabled);
    RUN_TEST(test_can_comm_send_immediate_invalid_network);
    RUN_TEST(test_can_comm_send_immediate_invalid_index_bms);
    RUN_TEST(test_can_comm_send_immediate_invalid_index_primary);
    RUN_TEST(test_can_comm_send_immediate_invalid_frame_type);
    RUN_TEST(test_can_comm_send_immediate_null);
    RUN_TEST(test_can_comm_send_immediate_sended);

    RUN_TEST(test_can_comm_tx_add_disabled);
    RUN_TEST(test_can_comm_tx_add_invalid_network);
    RUN_TEST(test_can_comm_tx_add_invalid_index_bms);
    RUN_TEST(test_can_comm_tx_add_invalid_index_primary);
    RUN_TEST(test_can_comm_tx_add_invalid_frame_type);
    RUN_TEST(test_can_comm_tx_add_null);
    RUN_TEST(test_can_comm_tx_add_ok);
    RUN_TEST(test_can_comm_tx_add_added);
    
    RUN_TEST(test_can_comm_rx_add_disabled);
    RUN_TEST(test_can_comm_rx_add_invalid_network);
    RUN_TEST(test_can_comm_rx_add_invalid_index_bms);
    RUN_TEST(test_can_comm_rx_add_invalid_index_primary);
    RUN_TEST(test_can_comm_rx_add_invalid_frame_type);
    RUN_TEST(test_can_comm_rx_add_null);
    RUN_TEST(test_can_comm_rx_add_ok);
    RUN_TEST(test_can_comm_rx_add_added);
    RUN_TEST(test_can_comm_rx_add_invalid_payload_size);

    return UNITY_END();
}
