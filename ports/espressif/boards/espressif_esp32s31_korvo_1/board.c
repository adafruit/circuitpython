// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "common-hal/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"

// The 4.3 inch 800x480 RGB panel on the LCD connector (ESP32-S3-LCD-EV-Board SUB3,
// ST7262E43). It needs no init commands, only the RGB timing below.

// LCD data lines DB0-DB15, least significant bit first within each color.
static const mcu_pin_obj_t *blue_pins[] = {
    &pin_GPIO8, &pin_GPIO9, &pin_GPIO10, &pin_GPIO11, &pin_GPIO12
};
static const mcu_pin_obj_t *green_pins[] = {
    &pin_GPIO13, &pin_GPIO14, &pin_GPIO15, &pin_GPIO16, &pin_GPIO17, &pin_GPIO18
};
static const mcu_pin_obj_t *red_pins[] = {
    &pin_GPIO19, &pin_GPIO33, &pin_GPIO34, &pin_GPIO35, &pin_GPIO36
};

// Display enable on the LCD connector.
#define LCD_DISP_PIN (38)

void board_init(void) {
    config_pin_as_output_with_level(LCD_DISP_PIN, true);

    dotclockframebuffer_framebuffer_obj_t *framebuffer = &allocate_display_bus_or_raise()->dotclock;
    framebuffer->base.type = &dotclockframebuffer_framebuffer_type;

    common_hal_dotclockframebuffer_framebuffer_construct(
        framebuffer,
        /* de */ &pin_GPIO43,
        /* vsync */ &pin_GPIO45,
        /* hsync */ &pin_GPIO44,
        /* dclk */ &pin_GPIO40,
        /* data */ red_pins, MP_ARRAY_SIZE(red_pins), green_pins, MP_ARRAY_SIZE(green_pins), blue_pins, MP_ARRAY_SIZE(blue_pins),
        /* frequency */ 18000000,
        /* width x height */ 800, 480,
        /* horizontal: pulse, back & front porch, idle */ 40, 40, 48, false,
        /* vertical: pulse, back & front porch, idle */ 23, 32, 13, false,
        /* de_idle_high */ false,
        /* pclk_active_high */ false,
        /* pclk_idle_high */ false,
        /* overscan_left */ 0
        );

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display_or_raise()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        framebuffer,
        0,
        true
        );
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Keep the display enabled across resets.
    if (pin_number == LCD_DISP_PIN) {
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
