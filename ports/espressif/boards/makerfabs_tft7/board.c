// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "shared-bindings/board/__init__.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/dotclockframebuffer/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/settings.h"

static const mcu_pin_obj_t *blue_pins[] = {
    &pin_GPIO8,
    &pin_GPIO3,
    &pin_GPIO46,
    &pin_GPIO9,
    &pin_GPIO1
};
static const mcu_pin_obj_t *green_pins[] = {
    &pin_GPIO5,
    &pin_GPIO6,
    &pin_GPIO7,
    &pin_GPIO15,
    &pin_GPIO16,
    &pin_GPIO4
};
static const mcu_pin_obj_t *red_pins[] = {
    &pin_GPIO45,
    &pin_GPIO48,
    &pin_GPIO47,
    &pin_GPIO21,
    &pin_GPIO14
};

static void display_init(void) {

    mp_int_t height = 0, width = 0, frequency = 0;
    settings_err_t result;

    result = settings_get_int("CIRCUITPY_DISPLAY_WIDTH", &width);
    if (result == SETTINGS_OK && width == 800) {
        width = 800;
        height = 480;
        frequency = 6500000;
    } else if (result == SETTINGS_OK && width == 1024) {
        width = 1024;
        height = 600;
        frequency = 10000000;
    }

    if (height == 0) {
        width = 800;
        height = 480;
        frequency = 6500000;
    }

    dotclockframebuffer_framebuffer_obj_t *framebuffer = &allocate_display_bus_or_raise()->dotclock;
    framebuffer->base.type = &dotclockframebuffer_framebuffer_type;

    common_hal_dotclockframebuffer_framebuffer_construct(
        framebuffer,
        &pin_GPIO40,    // de
        &pin_GPIO41,    // vsync
        &pin_GPIO39,    // hsync
        &pin_GPIO42,    // pclk
        red_pins, MP_ARRAY_SIZE(red_pins),
        green_pins, MP_ARRAY_SIZE(green_pins),
        blue_pins, MP_ARRAY_SIZE(blue_pins),
        frequency,        // Frequency
        width,            // width
        height,            // height
        30, 16, 210, false, // horiz: pulse, back porch, front porch, idle low
        13, 10, 22, false,  // vert: pulse, back porch, front porch, idle low
        false,          // DE idle high
        false,          // pclk active high
        false,          // pclk idle high
        0               // overscan left
        );

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display_or_raise()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        framebuffer,
        0,    // rotation
        true  // auto-refresh
        );
}

void board_init(void) {
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
