// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/digitalio/DigitalInOut.h"
#if CIRCUITPY_PWMIO
#include "shared-bindings/pwmio/PWMOut.h"
#endif

// Backlight state shared by both BusDisplay backends.
typedef union {
    digitalio_digitalinout_obj_t inout;
    #if CIRCUITPY_PWMIO
    pwmio_pwmout_obj_t pwm;
    #endif
} busdisplay_backlight_t;

void busdisplay_backlight_construct(busdisplay_backlight_t *self, const mcu_pin_obj_t *pin,
    uint16_t pwm_frequency);
bool busdisplay_backlight_set(busdisplay_backlight_t *self, mp_float_t brightness);
void busdisplay_backlight_deinit(busdisplay_backlight_t *self);

// The BusDisplay implementation has two backends selected at build time with
// CIRCUITPY_DISPLAYIO_LVGL:
//
//   legacy/ - the original scanline compositor, built on display_core/bus_core.
//   lvgl/   - an LVGL display with its own draw buffers and flush callback.
#if CIRCUITPY_DISPLAYIO_LVGL
#include "shared-module/busdisplay/lvgl/BusDisplay.h"
#else
#include "shared-module/busdisplay/legacy/BusDisplay.h"
#endif
