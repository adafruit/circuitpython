// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#include "py/mphal.h"

// Bit mask of claimed pins on each of up to two ports. nrf52832 has one port; nrf52840 has two.
// static uint32_t claimed_pins[GPIO_COUNT];

// Mark pin as free and return it to a quiescent state.
void reset_pin(const mcu_pin_obj_t *pin) {

    // Clear claimed bit.
    // claimed_pins[nrf_pin_port(pin_number)] &= ~(1 << nrf_relative_pin_number(pin_number));
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin(pin);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    // Set bit in claimed_pins bitmask.
    // claimed_pins[nrf_pin_port(pin->number)] |= 1 << nrf_relative_pin_number(pin->number);
}


bool pin_number_is_free(uint8_t pin_number) {
    return false; // !(claimed_pins[nrf_pin_port(pin_number)] & (1 << nrf_relative_pin_number(pin_number)));
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return true;

}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}
