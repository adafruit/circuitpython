// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "bindings/picodvi/Framebuffer.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"

static picodvi_framebuffer_obj_t board_display_bus;
framebufferio_framebufferdisplay_obj_t board_display;

void board_init(void) {
    picodvi_framebuffer_obj_t *fb = displayio_native_init(&board_display_bus, &picodvi_framebuffer_protocol, &picodvi_framebuffer_type, 0, MP_OBJ_NULL, true);
    if (fb == NULL) {
        return;
    }
    fb->base.type = &picodvi_framebuffer_type;
    common_hal_picodvi_framebuffer_construct(fb, 320, 240,
        &pin_GPIO17, &pin_GPIO16,
        &pin_GPIO19, &pin_GPIO18,
        &pin_GPIO21, &pin_GPIO20,
        &pin_GPIO23, &pin_GPIO22,
        8);

    framebufferio_framebufferdisplay_obj_t *display = displayio_native_init(&board_display, &framebufferio_framebufferdisplay_protocol, &framebufferio_framebufferdisplay_type, 0, MP_OBJ_FROM_PTR(fb), true);

    if (display == NULL) {
        return;
    }
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        MP_OBJ_FROM_PTR(fb),
        0,
        true);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
