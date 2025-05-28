// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"


uint8_t display_init_sequence[] = {
    0xAE, 0, // DISPLAY_OFF
    0x20, 1, 0x00, // Set memory addressing to horizontal mode.
    0x81, 1, 0xcf, // set contrast control
    0xA1, 0, // Column 127 is segment 0
    0xA6, 0, // Normal display
    0xc8, 0, // Normal display
    0xA8, 1, 0x3f, // Mux ratio is 1/64
    0xd5, 1, 0x80, // Set divide ratio
    0xd9, 1, 0xf1, // Set pre-charge period
    0xda, 1, 0x12, // Set com configuration
    0xdb, 1, 0x40, // Set vcom configuration
    0x8d, 1, 0x14, // Enable charge pump
    0xAF, 0, // DISPLAY_ON
};

void board_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        CIRCUITPY_BOARD_OLED_DC, // Command or data
        CIRCUITPY_BOARD_OLED_CS, // Chip select
        CIRCUITPY_BOARD_OLED_RESET, // Reset
        10000000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        72, // Width (after rotation)
        40, // Height (after rotation)
        28, // column start
        28, // row start
        0, // rotation
        1, // Color depth
        true, // grayscale
        false, // pixels in byte share row. only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        0x21, // Set column command
        0x22, // Set row command
        44, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,  // backlight pin
        0x81,
        1.0f, // brightness
        true, // single_byte_bounds
        true, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        0); // backlight pwm frequency
}

void reset_board(void) {
}
