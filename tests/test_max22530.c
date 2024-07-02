#include "unity.h"
#include <stdio.h>
#include "max22530.h"
#include <string.h>
#include <malloc.h>

typedef uint8_t max22530_address_t;

Max22530ReturnCode _max22530_write(Max22530Handler *handler, max22530_address_t address, uint16_t data);

static struct {
    SpiNetwork network;
    uint8_t data[3];
    size_t size;
} sendparams;

void setUp() {
    memset(&sendparams, 0, sizeof(sendparams));
}

void tearDown() {}

void send(SpiNetwork network, uint8_t *data, size_t size) {
    sendparams.network = network;
    memcpy(sendparams.data, data, size);
    sendparams.size = size;
}

Max22530Handler handler = {
    .send = send,
    .send_receive = NULL,
};

void test_max22530_write() {
    _max22530_write(&handler, 0x55, 0xDEAD);
    uint8_t exp[3] = {0x56, 0xDE, 0xAD};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(exp, sendparams.data, sizeof(exp));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_max22530_write);
    return UNITY_END();
}