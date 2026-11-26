// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "background.h"
#include "mpconfigboard.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/sharpdisplay/SharpMemoryFramebuffer.h"
#include "shared-module/displayio/__init__.h"

digitalio_digitalinout_obj_t extcomin;
digitalio_digitalinout_obj_t display_on;

uint32_t last_down_ticks_ms;

static sharpdisplay_framebuffer_obj_t board_display_bus;
framebufferio_framebufferdisplay_obj_t board_display;

void board_init(void) {
    common_hal_digitalio_digitalinout_construct(&extcomin, &pin_P0_06);
    common_hal_digitalio_digitalinout_switch_to_output(&extcomin, true, DRIVE_MODE_PUSH_PULL);

    common_hal_digitalio_digitalinout_construct(&display_on, &pin_P0_07);
    common_hal_digitalio_digitalinout_switch_to_output(&display_on, true, DRIVE_MODE_PUSH_PULL);

    sharpdisplay_framebuffer_obj_t *fb = displayio_native_init(&board_display_bus, &sharpdisplay_framebuffer_protocol, &sharpdisplay_framebuffer_type, 0, MP_OBJ_NULL, true);

    if (fb == NULL) {
        return;
    }
    fb->base.type = &sharpdisplay_framebuffer_type;

    busio_spi_obj_t *spi = &fb->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_P0_26, &pin_P0_27, NULL, false);

    common_hal_sharpdisplay_framebuffer_construct(fb, spi, &pin_P0_05, 500000, 176, 176, true);

    framebufferio_framebufferdisplay_obj_t *self = displayio_native_init(&board_display, &framebufferio_framebufferdisplay_protocol, &framebufferio_framebufferdisplay_type, 0, MP_OBJ_FROM_PTR(fb), true);
    if (self == NULL) {
        return;
    }
    self->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(self, fb, 0, true);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    nrf_gpio_cfg_input(17, NRF_GPIO_PIN_PULLUP);
}

void board_deinit(void) {
    // common_hal_displayio_release_displays();
}

void board_background_task(void) {
    if (!nrf_gpio_pin_read(17)) {
        if (last_down_ticks_ms == 0) {
            last_down_ticks_ms = supervisor_ticks_ms32();
        }
    } else {
        last_down_ticks_ms = 0;
    }
    // If the button isn't pressed, then feed the watchdog.
    if (last_down_ticks_ms == 0) {
        NRF_WDT->RR[0] = 0x6E524635;
        return;
    }
    // if the button has been pressed less than 5 seconds, then feed the watchdog.
    uint32_t now = supervisor_ticks_ms32();
    if (now - last_down_ticks_ms < 5000) {
        NRF_WDT->RR[0] = 0x6E524635;
    }
    // Don't feed the watchdog so that it'll expire and kick us to the bootloader.
}
