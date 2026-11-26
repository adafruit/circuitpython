// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"


#define DELAY 0x80

uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 150, // SWRESET
    0x11, 0 | DELAY, 255, // SLPOUT
    0x36, 1, 0x00,        // _MADCTL bottom to top refresh in vsync aligned order.
    0x3a, 1, 0x55, // COLMOD - 16bit color
    0x38, 0,                 // Idle mode off
    0x21, 0,                 // _INVON
    0x13, 0,                 // _NORON
    0x29, 0 | DELAY, 255                // _DISPON
};

static fourwire_fourwire_obj_t left_display_bus;
static fourwire_fourwire_obj_t right_display_bus;
busdisplay_busdisplay_obj_t board_display;
busdisplay_busdisplay_obj_t board_right_display;

static void init_display(fourwire_fourwire_obj_t *bus, busdisplay_busdisplay_obj_t *display,
    const mcu_pin_obj_t *sck, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *dc, const mcu_pin_obj_t *cs,
    mp_obj_t reset, const mcu_pin_obj_t *backlight) {
    displayio_native_init(bus, &fourwire_fourwire_protocol, &fourwire_fourwire_type, 0, MP_OBJ_NULL, true);
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, sck, mosi, NULL, false);

    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(dc), // TFT_DC Command or data
        MP_OBJ_FROM_PTR(cs), // TFT_CS Chip select
        reset,
        60000000, // Baudrate
        0, // Polarity
        0); // Phase

    displayio_native_init(display, &busdisplay_busdisplay_protocol, &busdisplay_busdisplay_type, 0, MP_OBJ_FROM_PTR(bus), true);
    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        240, // Width (after rotation)
        240, // Height (after rotation)
        0, // column start
        0, // row start
        180, // rotation
        16, // Color depth
        false, // Grayscale
        false, // pixels in a byte share a row. Only valid for depths < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        backlight,
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // SH1107_addressing
        50000); // backlight pwm frequency
}

void board_init(void) {
    // The left display has no directly connected reset or backlight pin.
    init_display(&left_display_bus, &board_display,
        &pin_PB03, &pin_PB02, &pin_PB22, &pin_PB23, mp_const_none, NULL);
    init_display(&right_display_bus, &board_right_display,
        &pin_PA13, &pin_PA12, &pin_PA07, &pin_PA06, MP_OBJ_FROM_PTR(&pin_PA04), &pin_PA23);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
