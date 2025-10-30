// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "mpconfigboard.h"
#include "pins.h"

#include "max32_port.h"

#include "common-hal/microcontroller/Pin.h"

static uint32_t claimed_pins[NUM_GPIO_PORTS];

// defined in board.c
extern mxc_gpio_regs_t *gpio_ports[NUM_GPIO_PORTS];

static uint32_t never_reset_pins[NUM_GPIO_PORTS];

#define INVALID_PIN 0xFF // id for invalid pin

void reset_all_pins(void) {
    // reset all pins except for never_reset_pins
    for (int i = 0; i < NUM_GPIO_PORTS; i++) {
        for (int j = 0; j < 32; j++) {
            if (!(never_reset_pins[i] & (1 << j))) {
                reset_pin_number(i, j);
            }
        }
        // set claimed pins to never_reset pins
        claimed_pins[i] = never_reset_pins[i];
    }
}

void reset_pin_number(uint8_t pin_port, uint8_t pin_pad) {
    if (pin_port == INVALID_PIN || pin_port > NUM_GPIO_PORTS) {
        return;
    }

    uint32_t mask = 1 << (pin_pad);

    /** START: RESET LOGIC for GPIOs */
    // Switch to I/O mode first
    gpio_ports[pin_port]->en0_set = mask;

    // set GPIO configuration enable bits to I/O
    gpio_ports[pin_port]->en0_clr = mask;
    gpio_ports[pin_port]->en1_clr = mask;
    gpio_ports[pin_port]->en2_clr = mask;

    // enable input mode GPIOn_INEN.pin = 1
    gpio_ports[pin_port]->inen |= mask;

    // High Impedance mode enable (GPIOn_PADCTRL1 = 0, _PADCTRL0 = 0), pu/pd disable
    gpio_ports[pin_port]->padctrl0 &= ~mask;
    gpio_ports[pin_port]->padctrl1 &= ~mask;

    // Output mode disable GPIOn_OUTEN = 0
    gpio_ports[pin_port]->outen |= mask;

    // Interrupt disable GPIOn_INTEN = 0
    gpio_ports[pin_port]->inten &= ~mask;
    /** END: RESET LOGIC for GPIOs */
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return INVALID_PIN;
    }

    // most max32 gpio ports have 32 pins
    // todo (low prior.): encode # of pins for each port, since some GPIO ports differ
    return pin->port * 32 + pin->mask;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return true;
    }
    return !(claimed_pins[pin->port] & (pin->mask));
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    if ((pin != NULL) && (pin->mask != INVALID_PIN)) {
        never_reset_pins[pin->port] |= (1 << pin->mask);

        // any never reset pin must also be claimed
        claimed_pins[pin->port] |= (1 << pin->mask);
    }
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }

    reset_pin_number(pin->port, pin->mask);
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    claimed_pins[pin->port] |= (1 << pin->mask);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no / 32, pin_no & 32);
}
