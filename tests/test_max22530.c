#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "max22530.h"
#include "unity.h"

typedef uint8_t max22530_address_t;

Max22530ReturnCode _max22530_write(Max22530Handler* handler,
    max22530_address_t address,
    uint16_t data);

int16_t _max22530_read(Max22530Handler* handler, max22530_address_t address);

Max22530ReturnCode _max22530_burst(Max22530Handler * handler, bool filtered, uint16_t * out);


struct
{
    SpiNetwork network;
    uint8_t* data;
    size_t size;
} send_params;

struct
{
    SpiNetwork network;
    uint8_t* data;
    uint8_t* out;
    size_t size;
    size_t out_size;
} send_receive_params;

void setUp()
{
    memset(&send_params, 0, sizeof(send_params));
    memset(&send_receive_params, 0, sizeof(send_receive_params));
}

void tearDown()
{
    if (send_params.data != 0) {
        memset(send_params.data, 0, send_params.size);
        free(send_params.data);
    }

    if (send_receive_params.data != 0) {
        memset(send_receive_params.data, 0, send_receive_params.size);
        free(send_receive_params.data);
    }

    if (send_receive_params.out != 0) {
        memset(send_receive_params.out, 0, send_receive_params.out_size);
        free(send_receive_params.out);
    }

}

void send(SpiNetwork network, uint8_t* data, size_t size)
{
    send_params.network = network;
    send_params.data = malloc(size);

    memcpy(send_params.data, data, size);
    send_params.size = size;
}

void send_receive(SpiNetwork network,
    uint8_t* data,
    uint8_t* out,
    size_t size,
    size_t out_size)
{
    send_receive_params.network = network;

    send_receive_params.data = malloc(size);
    memcpy(send_receive_params.data, data, size);

    send_receive_params.out = malloc(out_size);
    for (size_t i = 0; i < out_size; i++) {
        send_receive_params.out[i] = 0xFF - i;
    }
    memcpy(out, send_receive_params.out, out_size);

    send_receive_params.size = size;
    send_receive_params.out_size = out_size;
}

Max22530Handler handler = {
    .send = send,
    .send_receive = send_receive,
};



void test_max22530_write_ok(){
    TEST_ASSERT_EQUAL(MAX22530_OK, _max22530_write(&handler, 0x55, 0xDEAD));
}
void test_max22530_write_cmd_size(){
    _max22530_write(&handler, 0x55, 0xDEAD);
    TEST_ASSERT_EQUAL_size_t(3, send_params.size);
}
void test_max22530_write_cmd()
{
    _max22530_write(&handler, 0x55, 0xDEAD);

    uint8_t expected[3] = { 0x56, 0xDE, 0xAD };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, send_params.data, sizeof(expected));
}


void test_max22530_read_cmd_size(){
    _max22530_read(&handler, 0xAB);
    TEST_ASSERT_EQUAL_size_t(1, send_receive_params.size);

}
void test_max22530_read_cmd()
{
    _max22530_read(&handler, 0xAB);
    TEST_ASSERT_EQUAL_UINT8(0xAE, send_receive_params.data[0]);
}
void test_max22530_read_resp_size(){
    _max22530_read(&handler, 0xAB);
    TEST_ASSERT_EQUAL_size_t(2, send_receive_params.out_size);
}
void test_max22530_read_resp()
{
    uint16_t expected = 0xFFFE;
    uint16_t ret = _max22530_read(&handler, 0xAB);
    TEST_ASSERT_EQUAL_UINT16(expected, ret);
}


void test_max22530_burst_ok(){
    uint16_t out[4];
    TEST_ASSERT_EQUAL(MAX22530_OK, _max22530_burst(&handler, false, out));
}
void test_max22530_burst_cmd_size(){
    uint16_t out[4];
    _max22530_burst(&handler, false, out);
    TEST_ASSERT_EQUAL_size_t(1, send_receive_params.size);
}
void test_max22530_burst_cmd_filt_on(){
    uint16_t out[4];
    _max22530_burst(&handler, true, out);
    TEST_ASSERT_EQUAL_UINT8(0x17, send_receive_params.data[0]);
}
void test_max22530_burst_cmd_filt_off(){
    uint16_t out[4];
    _max22530_burst(&handler, false, out);
    TEST_ASSERT_EQUAL_UINT8(0x07, send_receive_params.data[0]);
}
void test_max22530_burst_resp_size(){
    uint16_t out[4];
    _max22530_burst(&handler, false, out);
    TEST_ASSERT_EQUAL_size_t(10, send_receive_params.out_size);
}
void test_max22530_burst_resp(){
    uint16_t out[4];
    _max22530_burst(&handler, false, out);

    uint8_t expected[10] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, send_receive_params.out, 10);
}
void test_max22530_burst_out(){
    uint16_t out[4];
    _max22530_burst(&handler, false, out);

    uint16_t expected[4] = {0x0FFE, 0x0DFC, 0x0BFA, 0x09F8};
    TEST_ASSERT_EQUAL_UINT16_ARRAY(expected, out, 4);
}


void test_max22530_init_null_handler(){
    TEST_ASSERT_EQUAL(MAX22530_NULL_POINTER, max22530_init(NULL, handler.send, handler.send_receive));
}
void test_max22530_init_null_send(){
    Max22530Handler test_handler;
    TEST_ASSERT_EQUAL(MAX22530_NULL_POINTER, max22530_init(&test_handler, NULL, handler.send_receive));
}
void test_max22530_init_null_send_receive(){
    Max22530Handler test_handler;
    TEST_ASSERT_EQUAL(MAX22530_NULL_POINTER, max22530_init(&test_handler, handler.send, NULL));
}
void test_max22530_init_ok(){
    Max22530Handler test_handler;
    TEST_ASSERT_EQUAL(MAX22530_OK, max22530_init(&test_handler, handler.send, handler.send_receive));
}
void test_max22530_init(){
    Max22530Handler test_handler;
    max22530_init(&test_handler, handler.send, handler.send_receive);
    TEST_ASSERT_EQUAL_MEMORY(&handler, &test_handler, sizeof(Max22530Handler));
}
void test_max22530_init_retval(){
    Max22530Handler test_handler;
    max22530_init(&test_handler, handler.send, handler.send_receive);
    
    uint8_t expected[3] = {0x52, 0x00, 0x06};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, send_params.data, 3);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_max22530_write_ok);
    RUN_TEST(test_max22530_write_cmd_size);
    RUN_TEST(test_max22530_write_cmd);
    
    RUN_TEST(test_max22530_read_cmd_size);
    RUN_TEST(test_max22530_read_cmd);
    RUN_TEST(test_max22530_read_resp_size);
    RUN_TEST(test_max22530_read_resp);

    RUN_TEST(test_max22530_burst_ok);
    RUN_TEST(test_max22530_burst_cmd_size);
    RUN_TEST(test_max22530_burst_cmd_filt_on);
    RUN_TEST(test_max22530_burst_cmd_filt_off);
    RUN_TEST(test_max22530_burst_resp_size);
    RUN_TEST(test_max22530_burst_resp);
    RUN_TEST(test_max22530_burst_out);

    RUN_TEST(test_max22530_init_null_handler);
    RUN_TEST(test_max22530_init_null_send);
    RUN_TEST(test_max22530_init_null_send_receive);
    RUN_TEST(test_max22530_init_ok);
    RUN_TEST(test_max22530_init);
    RUN_TEST(test_max22530_init_retval);


    return UNITY_END();
}