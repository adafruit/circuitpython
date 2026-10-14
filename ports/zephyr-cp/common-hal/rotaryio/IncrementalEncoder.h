// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include <zephyr/drivers/gpio.h>

#include "common-hal/microcontroller/Pin.h"
#include "py/obj.h"

typedef struct rotaryio_incrementalencoder_obj rotaryio_incrementalencoder_obj_t;

typedef struct {
    struct gpio_callback callback;
    rotaryio_incrementalencoder_obj_t *encoder;
} rotaryio_incrementalencoder_gpio_callback_t;

struct rotaryio_incrementalencoder_obj {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin_a;
    const mcu_pin_obj_t *pin_b;
    // GPIO controller devices and pin numbers within them, resolved from the
    // pins' global numbers by the gpio allocate calls at construct time.
    const struct device *port_a;
    gpio_pin_t number_a;
    const struct device *port_b;
    gpio_pin_t number_b;
    rotaryio_incrementalencoder_gpio_callback_t callback_a;
    rotaryio_incrementalencoder_gpio_callback_t callback_b;
    uint8_t state; // <old A><old B>
    int8_t sub_count; // count intermediate transitions between detents
    int8_t divisor; // Number of quadrature edges required per count
    mp_int_t position;
};
