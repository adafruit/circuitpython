/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"

#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "cmsis5/CMSIS/Core/Include/cmsis_compiler.h"


STATIC void gpio_port_init(gpio_port_obj_t *ports, const size_t num) {
    for (size_t n = 0; n < num; ++n) {
        ports[n].used_pin_mask = 0U;
    }

    return;
}

void reset_all_pins(void) {
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);
    gpio_port_init(gpio_ports, port_cnt);

    for (size_t port_idx = 0U; port_idx < port_cnt; port_idx++) {
        uint32_t available_pin_mask = gpio_ports[port_idx].available_pin_mask;
        const uint32_t reserved_pin_mask = gpio_ports[port_idx].reserved_pin_mask;

        available_pin_mask &= ~reserved_pin_mask;

        uint32_t pin_idx = (31U - __CLZ(available_pin_mask));
        uint32_t pin_mask = (1U << pin_idx);
        while (available_pin_mask & pin_mask) {
            reset_pin_number(port_idx, pin_idx);
            available_pin_mask &= ~pin_mask;
            pin_idx = (31U - __CLZ(available_pin_mask));
            pin_mask = (1U << pin_idx);
        }
    }

    return;
}

void never_reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);

    if ((pin_port < port_cnt) && (pin_number < 32U)) {
        const uint32_t available_pin_mask = gpio_ports[pin_port].available_pin_mask;
        uint32_t pin_mask = (1 << pin_number);

        if (pin_mask & available_pin_mask) {
            gpio_ports[pin_port].reserved_pin_mask |= pin_mask;
        }
    }

    return;
}

void reset_pin_number(uint8_t pin_port, uint8_t pin_number) {
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);
    if ((pin_port < port_cnt) && (pin_number < 32U)) {
        uint32_t pin_mask = ~(1U << pin_number);
        gpio_ports[pin_port].reserved_pin_mask &= pin_mask;
        gpio_ports[pin_port].used_pin_mask &= pin_mask;

        gpio_pin_config_t config;
        config.input = true;
        config.outputLogic = true;
        config.pinMode = GPIO_Mode_PullNone;
        gpio_pin_init(pin_port, pin_number, &config);
    }

    return;
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->port, pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    reset_pin_number(pin->port, pin->number);
}

void claim_pin(uint8_t pin_port, uint8_t pin_number) {
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);

    if ((pin_port < port_cnt) && (pin_number < 32U)) {
        uint32_t pin_mask = (1U << pin_number);
        gpio_ports[pin_port].used_pin_mask |= pin_mask;
    }

    return;
}

bool pin_number_is_free(uint8_t pin_port, uint8_t pin_number) {
    bool is_free = true;
    const size_t port_cnt = sizeof(gpio_ports) / sizeof(gpio_ports[0U]);

    if ((pin_port < port_cnt) && (pin_number < 32U)) {
        uint32_t pin_mask = (1U << pin_number);
        is_free = is_free && !(gpio_ports[pin_port].used_pin_mask & pin_mask);
        is_free = is_free && !(gpio_ports[pin_port].reserved_pin_mask & pin_mask);
    }

    return is_free;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->port, pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    uint8_t pin_designator = (pin->port) << 5;
    pin_designator |= (pin->number);

    return pin_designator;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    return claim_pin(pin->port, pin->number);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    uint8_t port = (uint8_t)(pin_no >> 5U);
    uint8_t pin = (uint8_t)(pin_no & 0x1FU);

    reset_pin_number(port, pin);

    return;
}
