// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Bill Sideris, independently providing these changes
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"


#define DELAY 0x80

// Display init sequence according to LILYGO factory firmware
uint8_t display_init_sequence[] = {
    // sw reset
    0x01, 0 | DELAY, 150,
    // sleep out
    0x11, 0 | DELAY, 255,
    // normal display mode on
    0x13, 0,
    // display and color format settings
    0x36, 1, 0x68,
    0xB6, 2, 0x0A, 0x82,
    0x3A, 1 | DELAY,  0x55, 10,
    // ST7789V frame rate setting
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
    // voltages: VGH / VGL
    0xB7, 1, 0x35,
    // ST7789V power setting
    0xBB, 1, 0x28,
    0xC0, 1, 0x0C,
    0xC2, 2, 0x01, 0xFF,
    0xC3, 1, 0x10,
    0xC4, 1, 0x20,
    0xC6, 1, 0x0F,
    0xD0, 2, 0xA4, 0xA1,
    // ST7789V gamma setting
    0xE0, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x32, 0x44, 0x42, 0x06, 0x0E, 0x12, 0x14, 0x17,
    0xE1, 14, 0xD0, 0x00, 0x02, 0x07, 0x0A, 0x28, 0x31, 0x54, 0x47, 0x0E, 0x1C, 0x17, 0x1B, 0x1E,
    0x21, 0,
    // display on
    0x29, 0 | DELAY, 255,
};

#define AXP2101_I2C_ADDRESS 0x34

static void write_register8(busio_i2c_obj_t *i2c, uint8_t reg, uint8_t value) {
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = value;
    common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, buffer, 2);
}

static void set_bit_in_register(busio_i2c_obj_t *i2c, uint8_t reg, uint8_t bitmask) {
    uint8_t buffer[2];
    buffer[0] = reg;
    buffer[1] = 0;
    common_hal_busio_i2c_write_read(i2c, AXP2101_I2C_ADDRESS, &buffer[0], 1, &buffer[1], 1);
    buffer[1] |= bitmask;
    common_hal_busio_i2c_write(i2c, AXP2101_I2C_ADDRESS, buffer, 2);
}

static void enable_ldo(busio_i2c_obj_t *i2c, uint8_t ldo) {
    write_register8(i2c, ldo + 0x92, 0x1C); // 3300mV
    set_bit_in_register(i2c, 0x90, 1 << ldo);
}

static void enable_dldo(busio_i2c_obj_t *i2c, uint8_t ldo) {
    if (ldo == 1) {
        write_register8(i2c, 0x99, 0x1C); // 3300mV
        set_bit_in_register(i2c, 0x90, 0x80);
    }
}

// Init the AXP2101 by hand as to not include XPOWERS lib.
static void pmic_init(busio_i2c_obj_t *i2c) {
    enable_ldo(i2c, 0); // _aldo1
    enable_ldo(i2c, 1); // _aldo2
    enable_ldo(i2c, 2); // _aldo3
    enable_ldo(i2c, 3); // _aldo4
    enable_ldo(i2c, 5); // _bldo2
    enable_dldo(i2c, 1); // _dldo1
    write_register8(i2c, 0x18, 0x0F); // Enable charging of main Bat and Coin cell
    write_register8(i2c, 0x27, 0x1F); // 2s on time + 10s off time
    write_register8(i2c, 0x62, 0x0B); // 500mA Current limit
    write_register8(i2c, 0x16, 0x04); // 1.5A INcurr limit
    write_register8(i2c, 0x61, 0x06); // 150mA Precharge limit
    write_register8(i2c, 0x64, 0x03); // 4.2V Voltage target
    write_register8(i2c, 0x63, 0x11); // 25mA Charging termination current
}


void board_init(void) {
    busio_i2c_obj_t *internal_i2c = common_hal_board_create_i2c(0);
    pmic_init(internal_i2c);

    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO38),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO12),    // CS
        NULL,           // RST
        40000000,       // baudrate
        0,              // polarity
        0               // phase
        );
    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        240,            // width (after rotation)
        240,            // height (after rotation)
        0,              // column start
        0,              // row start
        90,             // rotation
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
        &pin_GPIO45,    // backlight pin
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

// TODO: Should we turn off the display when asleep, in board_deinit() ?
