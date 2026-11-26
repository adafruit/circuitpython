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

typedef enum {
    BUSDISPLAY_BACKLIGHT_NONE,
    BUSDISPLAY_BACKLIGHT_DIGITAL,
    BUSDISPLAY_BACKLIGHT_PWM,
} busdisplay_backlight_kind_t;

typedef union {
    digitalio_digitalinout_obj_t inout;
    #if CIRCUITPY_PWMIO
    pwmio_pwmout_obj_t pwm;
    #endif
} busdisplay_backlight_native_t;

// The primary display adopts a borrowed VM backlight before VM teardown.
// Board displays use statically allocated backlight objects.
typedef struct {
    busdisplay_backlight_native_t native;
    mp_obj_t object;
    busdisplay_backlight_kind_t kind;
    #if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
    size_t size;
    #endif
    bool owns;
    bool adopted;
} busdisplay_backlight_t;

#if CIRCUITPY_FULL_BUILD
void busdisplay_backlight_validate(mp_obj_t backlight);
#endif
void busdisplay_backlight_construct(busdisplay_backlight_t *self, mp_obj_t backlight,
    uint16_t pwm_frequency);
#if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
bool busdisplay_backlight_is_moveable(busdisplay_backlight_t *self);
void busdisplay_backlight_adopt(busdisplay_backlight_t *self);
mp_obj_t busdisplay_backlight_start_vm(busdisplay_backlight_t *self);
#endif
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
