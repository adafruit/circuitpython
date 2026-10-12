// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include <zephyr/device.h>

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *pin;
    // PWM device and channel from iobroker_pwm_channel_allocate(); dev is
    // NULL when deinited.
    const struct device *dev;
    uint32_t channel;
    // Period in the device's PWM clock cycles, from the frequency. Every
    // pwm_set_cycles() call passes exactly this value.
    uint32_t period_cycles;
    uint16_t duty_cycle;
    bool variable_frequency;
} pwmio_pwmout_obj_t;
