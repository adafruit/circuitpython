// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

// STD includes
#include <stdint.h>
#include <stdbool.h>

// CktPy includes
#include "py/obj.h"

// HAL includes
#include "gpio.h"
#include "gpio_regs.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t port;
    uint32_t mask; // the pad # target e.g. P0.01 is Port=0, Mask=1
    mxc_gpio_vssel_t level;
} mcu_pin_obj_t;

extern const mp_obj_type_t mcu_pin_type;

#define PIN(pin_port, pin_mask) { {&mcu_pin_type}, .port = pin_port, .mask = 1UL << pin_mask, .level = MXC_GPIO_VSSEL_VDDIO }

// for non-connected pins
#define NO_PIN 0xFF

#ifdef MAX32690
#include "max32690/pins.h"
#endif
