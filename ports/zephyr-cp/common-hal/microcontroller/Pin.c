// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"

#include "py/mphal.h"

#include <iobroker/iobroker.h>

// Pin claims and pad reset live in the iobroker module: the objects that use
// a pin (busio buses through iobroker_*_allocate(), digitalio and rotaryio
// through iobroker_gpio_allocate()) own the claim and give it up again when
// they deinit, so the port keeps no claim table of its own.
// iobroker_pin_in_use() answers whether a pin is taken.

void reset_all_pins(void) {
    // Nothing to do: pins belong to the objects that allocated them through
    // iobroker, and those release their claims (and reset the pads) when they
    // deinit.
}

void never_reset_pin_number(uint8_t pin_number) {
    // Deprecated single-byte pin number API; not used by this port.
    (void)pin_number;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    // Nothing to mark: reset_all_pins() leaves pins alone, so there is no
    // reset to opt out of.
    (void)pin;
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    // iobroker resets the pads when the object holding them releases its
    // claim; a pin on its own has nothing to reset here.
    (void)pin;
}

bool pin_number_is_free(uint8_t pin_number) {
    // Deprecated single-byte pin number API; not used by this port.
    (void)pin_number;
    return true;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return true;
    }
    return !iobroker_pin_in_use(pin->package_pin);
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    // iobroker records the claim when the object using the pin allocates it;
    // a bare claim has nothing to record and nobody to release it.
    (void)pin;
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return (uint8_t)pin->number;
}

void common_hal_mcu_pin_claim_number(uint8_t pin_no) {
    (void)pin_no;
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    (void)pin_no;
}
