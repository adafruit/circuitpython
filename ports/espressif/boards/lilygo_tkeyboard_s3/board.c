// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"

#define DELAY 0x80

digitalio_digitalinout_obj_t board_tft_dc = {
    .base.type = &digitalio_digitalinout_type,
};

// GC9107 initialization sequence from LilyGO's Arduino_GFX driver.
uint8_t display_init_sequence[] = {
    0xEF, 0x00,
    0xEB, 0x01, 0x14,
    0xFE, 0x00,
    0xEF, 0x00,
    0xEB, 0x01, 0x14,
    0x84, 0x01, 0x40,
    0x85, 0x01, 0xFF,
    0x86, 0x01, 0xFF,
    0x87, 0x01, 0xFF,
    0x88, 0x01, 0x0A,
    0x89, 0x01, 0x21,
    0x8A, 0x01, 0x00,
    0x8B, 0x01, 0x80,
    0x8C, 0x01, 0x01,
    0x8D, 0x01, 0x01,
    0x8E, 0x01, 0xFF,
    0x8F, 0x01, 0xFF,
    0xB6, 0x02, 0x00, 0x20,
    0x3A, 0x01, 0x05, // COLMOD: 16 bits per pixel
    0x90, 0x04, 0x08, 0x08, 0x08, 0x08,
    0xBD, 0x01, 0x06,
    0xBC, 0x01, 0x00,
    0xFF, 0x03, 0x60, 0x01, 0x04,
    0xC3, 0x01, 0x13,
    0xC4, 0x01, 0x13,
    0xC9, 0x01, 0x22,
    0xBE, 0x01, 0x11,
    0xE1, 0x02, 0x10, 0x0E,
    0xDF, 0x03, 0x21, 0x0C, 0x02,
    0xF0, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    0xF1, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    0xF2, 0x06, 0x45, 0x09, 0x08, 0x08, 0x26, 0x2A,
    0xF3, 0x06, 0x43, 0x70, 0x72, 0x36, 0x37, 0x6F,
    0xED, 0x02, 0x1B, 0x0B,
    0xAE, 0x01, 0x77,
    0xCD, 0x01, 0x63,
    0x70, 0x09, 0x07, 0x07, 0x04, 0x0E, 0x0F, 0x09, 0x07, 0x08, 0x03,
    0xE8, 0x01, 0x34,
    0x62, 0x0C, 0x18, 0x0D, 0x71, 0xED, 0x70, 0x70, 0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70,
    0x63, 0x0C, 0x18, 0x11, 0x71, 0xF1, 0x70, 0x70, 0x18, 0x13, 0x71, 0xF3, 0x70, 0x70,
    0x64, 0x07, 0x28, 0x29, 0xF1, 0x01, 0xF1, 0x00, 0x07,
    0x66, 0x0A, 0x3C, 0x00, 0xCD, 0x67, 0x45, 0x45, 0x10, 0x00, 0x00, 0x00,
    0x67, 0x0A, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x01, 0x54, 0x10, 0x32, 0x98,
    0x74, 0x07, 0x10, 0x85, 0x80, 0x00, 0x00, 0x4E, 0x00,
    0x98, 0x02, 0x3E, 0x07,
    0x35, 0x00, // Tearing effect line on
    0x36, 0x01, 0xC8, // MADCTL: MY | MX | BGR
    0x21, 0x00, // Display inversion on
    0x11, 0x00 | DELAY, 120, // Sleep out
    0x29, 0x00 | DELAY, 20, // Display on
};

static void deselect_other_displays(void) {
    config_pin_as_output_with_level(13, true);
    config_pin_as_output_with_level(14, true);
    config_pin_as_output_with_level(21, true);
}

static fourwire_fourwire_obj_t board_display_bus;
busdisplay_busdisplay_obj_t board_display;

void board_init(void) {
    deselect_other_displays();

    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = displayio_native_init(&board_display_bus, &fourwire_fourwire_protocol, &fourwire_fourwire_type, 0, MP_OBJ_NULL, true);
    if (bus == NULL) {
        return;
    }
    bus->base.type = &fourwire_fourwire_type;

    // All four displays share this DC object. Keep it board-owned so additional
    // FourWire instances can borrow it without claiming or deinitializing it.
    common_hal_digitalio_digitalinout_construct(&board_tft_dc, &pin_GPIO45);

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&board_tft_dc), // Shared DC
        MP_OBJ_FROM_PTR(&pin_GPIO12), // First display CS
        MP_OBJ_FROM_PTR(&pin_GPIO38), // Shared reset
        40000000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = displayio_native_init(&board_display, &busdisplay_busdisplay_protocol, &busdisplay_busdisplay_type, 0, MP_OBJ_FROM_PTR(bus), true);
    if (display == NULL) {
        return;
    }
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        128, // Width
        128, // Height
        2, // Column start
        1, // Row start
        0, // Rotation
        16, // Color depth
        false, // Grayscale
        false, // Pixels in a byte share a row. Only valid for depths < 8
        1, // Bytes per cell. Only valid for depths < 8
        false, // Reverse pixels in byte
        true, // Reverse pixels in word
        MIPI_COMMAND_SET_COLUMN_ADDRESS,
        MIPI_COMMAND_SET_PAGE_ADDRESS,
        MIPI_COMMAND_WRITE_MEMORY_START,
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO39, // Shared backlight
        NO_BRIGHTNESS_COMMAND,
        1.0f, // Brightness
        false, // Single-byte bounds
        false, // Data as commands
        true, // Auto refresh
        60, // Native frames per second
        true, // Backlight on high
        false, // SH1107 addressing
        50000); // Backlight PWM frequency
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    if (pin_number == 13 || pin_number == 14 || pin_number == 21) {
        config_pin_as_output_with_level(pin_number, true);
        return true;
    }
    return false;
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
