// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 natheihei
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#define DELAY 0x80

// Driver is JD9853
// 172 X 320 Pixels RGB 18-bit
// Init sequence converted from Arduino example

uint8_t display_init_sequence[] = {
    // Command: 0x11 (SLPOUT: Sleep Out)
    // Description: Exits sleep mode. A 120ms delay is added for the power supply and clock circuits to stabilize.
    0x11, 0 | DELAY, 120,

    0xDF, 2, 0x98, 0x53,
    0xB2, 1, 0x23,

    0xB7, 4, 0x00, 0x47, 0x00, 0x6F,
    0xBB, 6, 0x1C, 0x1A, 0x55, 0x73, 0x63, 0xF0,
    0xC0, 2, 0x44, 0xA4,
    0xC1, 1, 0x16,
    0xC3, 8, 0x7D, 0x07, 0x14, 0x06, 0xCF, 0x71, 0x72, 0x77,
    0xC4, 12, 0x00, 0x00, 0xA0, 0x79, 0x0B, 0x0A, 0x16, 0x79, 0x0B, 0x0A, 0x16, 0x82,

    0xC8, 32, 0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,
    0x3F, 0x32, 0x29, 0x29, 0x27, 0x2B, 0x27, 0x28, 0x28, 0x26, 0x25, 0x17, 0x12, 0x0D, 0x04, 0x00,

    0xD0, 5, 0x04, 0x06, 0x6B, 0x0F, 0x00,
    0xD7, 2, 0x00, 0x30,
    0xE6, 1, 0x14,
    0xDE, 1, 0x01,

    0xB7, 5, 0x03, 0x13, 0xEF, 0x35, 0x35,
    0xC1, 3, 0x14, 0x15, 0xC0,
    0xC2, 2, 0x06, 0x3A,
    0xC4, 2, 0x72, 0x12,
    0xBE, 1, 0x00,
    0xDE, 1, 0x02,

    0xE5, 3, 0x00, 0x02, 0x00,
    0xE5, 3, 0x01, 0x02, 0x00,

    0xDE, 1, 0x00,

    // Command: 0x35 (TEON: Tearing Effect Line ON)
    // Description: Turns on the Tearing Effect output signal.
    0x35, 1, 0x00,

    // Command: 0x3A (COLMOD: Pixel Format Set)
    // Description: Sets the pixel format for the MCU interface.
    0x3A, 1, 0x05,

    // Command: 0x2A (CASET: Column Address Set)
    // Description: Defines the accessible column range in frame memory.
    0x2A, 4, 0x00, 0x22, 0x00, 0xCD,

    // Command: 0x2B (PASET: Page Address Set)
    // Description: Defines the accessible page (row) range.
    0x2B, 4, 0x00, 0x00, 0x01, 0x3F,

    0xDE, 1, 0x02,
    0xE5, 3, 0x00, 0x02, 0x00,
    0xDE, 1, 0x00,

    // Command: 0x36 (MADCTL: Memory Access Control)
    // Description: Sets the read/write scanning direction of the frame memory.
    0x36, 1, 0x00,

    // Command: 0x21 (INVON: Display Inversion ON)
    // 0x21, 0 | DELAY, 10,

    // Command: 0x29 (DISPON: Display ON)
    // Description: Turns the display on by enabling output from the frame memory.
    0x29, 0,
};

static void display_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO45),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO21),    // CS
        MP_OBJ_FROM_PTR(&pin_GPIO40),    // RST
        80000000,       // baudrate
        0,              // polarity
        0               // phase
        );

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        172,            // width (after rotation)
        320,            // height (after rotation)
        34,             // column start
        0,              // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        true,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO46,    // backlight pin
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

void board_init(void) {
    // Display
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
