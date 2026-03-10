#!/usr/bin/env bash

echo "Running full test suite..."

pio test -e balancing_tests
pio test -e can_comm_tests
pio test -e current_tests
pio test -e display_tests
pio test -e feedback_tests
pio test -e identity_tests
pio test -e imd_tests
pio test -e led_tests
pio test -e internal_voltage_tests
pio test -e pcu_tests
pio test -e tasks_tests
pio test -e temp_tests
pio test -e timebase_tests
pio test -e volt_tests
