// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Djair Guilherme
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#define DELAY 0x80

// ILI9341 init sequence (standard)
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0 | DELAY, 150,
    // sleep out
    0x11, 0 | DELAY, 255,
    // normal display mode on
    0x13, 0,
    // pixel format: 16-bit RGB565
    0x3A, 1, 0x55,
    // frame rate control
    0xB1, 2, 0x00, 0x1B,
    // display function control
    0xB6, 4, 0x0A, 0x82, 0x27, 0x00,
    // power control 1
    0xC0, 1, 0x23,
    // power control 2
    0xC1, 1, 0x10,
    // VCOM control 1
    0xC5, 2, 0x3E, 0x28,
    // VCOM control 2
    0xC7, 1, 0x86,
    // memory access control (MV=0, MX=0, MY=0, ML=0, BGR=1)
    0x36, 1, 0x88,
    // column address set (0..239)
    0x2A, 4, 0x00, 0x00, 0x00, 0xEF,
    // page address set (0..319)
    0x2B, 4, 0x00, 0x00, 0x01, 0x3F,
    // positive gamma
    0xE0, 15, 0x00, 0x07, 0x10, 0x09, 0x17, 0x0B, 0x41, 0x89, 0x4B, 0x0A, 0x0C, 0x0E, 0x18, 0x1A, 0x0F,
    // negative gamma
    0xE1, 15, 0x00, 0x17, 0x1D, 0x02, 0x0C, 0x05, 0x23, 0x75, 0x4B, 0x05, 0x0E, 0x11, 0x27, 0x38, 0x0F,
    // display on
    0x29, 0 | DELAY, 100,
};

void board_init(void) {
    // Backlight is controlled by PMIC via init_board() in libunihiker_k10.a
    // No direct GPIO backlight control needed

    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO13),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO14),    // CS
        mp_const_none,           // RST (tied to 3.3V)
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width
        320,            // height
        0,              // column start
        0,              // row start
        0,              // rotation (0=default from MADCTL)
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row
        1,              // bytes per cell
        false,          // reverse_pixels_in_byte
        true,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS,
        MIPI_COMMAND_SET_PAGE_ADDRESS,
        MIPI_COMMAND_WRITE_MEMORY_START,
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,           // backlight pin (PMIC controlled)
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        50000           // backlight pwm frequency
        );
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
