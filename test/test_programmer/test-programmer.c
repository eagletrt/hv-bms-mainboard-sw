/*!
 * \file test-programmer.c
 * \date 2026-05-22
 * \author Alessandro Giustina [giustinalessandro@gmail.com]
 *
 * \brief Tests for the Programmer API
 */

#include "unity.h"
#include "programmer-api.h"
#include "fsm.h"
#include "fff.h"
#include "timebase.h"
DEFINE_FFF_GLOBALS;

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();
    return UNITY_END();
}