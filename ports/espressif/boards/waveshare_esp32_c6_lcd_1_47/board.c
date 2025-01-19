// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Benjamin Shockley
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

#define DELAY 0x80

// Driver is ST7789V3
// Display Panel is LBS147TC-IF15
// 172 X 320 Pixels RGB 18-bit

uint8_t display_init_sequence[] = {
    // (0x01) SW Reset
    // The display module performs a software reset, registers are written with their SW reset default values. No parameters.
    // ST7789V3 requirement is to wait 120msec before sending sleep out command.
    0x01, 0 | DELAY, 120,
    // (0x11) Sleep Out
    // This command turns off sleep mode.
    // ST7789V3 requirement is to wait 120msec before sending sleep in command and wait 5msec before sending any new commands. No parameters.
    0x11, 0 | DELAY, 120,
    // (0x13) Normal Display Mode On
    // This command turns the display to normal mode. No parameters.
    0x13, 0,
    // Display and Color Format Settings
    //
    // (0x36) Memory Data Access Control
    // LBS147TC-IF15 as Mounted on the board is in Horizontal Mode
    // This command defines read/ write scanning direction of frame memory. 1 parameter.
    0x36, 1, 0x00,
    // (0x3A) Interface Pixel Format
    // This command is used to define the format of RGB picture data. 1 parameter. Value of 0x05 is setting the display to 16-bit (5,6,5). Could do 18-bit
    // but may not really see the benefit and costs much more memory.
    0x3A, 1 | DELAY,  0x05, 10,
    // (0xB2) Porch Setting
    // These 5 parameters front and backporch buffer sizes as well as enabling separate front and back control. Default value provided.
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
    // (0xB7) Gate Voltage Control
    // VGHS and VGLS settings.  1 paramter contains both. Per LBS147TC-IF15 spec; 12.2 < VGH < 14.97 (13.58) and12.5 < VGL <7.16 9.83
    0xB7, 1, 0x35,
    // (0xBB) VCOMS Setting
    // VCOMS is used for feed through voltage compensation. Default value provided.
    0xBB, 1, 0x20,
    // (0xC0) LCM Control
    // Various bits to control modes.  1 paramter.  Value taken from Waveshare's own Arduino example LCD init code.
    0xC0, 1, 0x2C,
    // (0xC2) VDV and VRH Command Enable
    // VDV and VRH command write enable. 2 paramters. Default values provided.
    0xC2, 2, 0x01, 0xFF,
    // (0xC3) VRH Set
    // 1 parameter. Value taken from Waveshare's own Arduino example LCD init code.
    0xC3, 1, 0x13,
    // (0xC4) VDV Set
    // 1 parameter. Value taken from Waveshare's own Arduino example LCD init code.    
    0xC4, 1, 0x20,
    // (0xC6) Frame Rate Control in Normal Mode
    // Frame rate value betwee 119 and 39 Hz. Default value is 60 Hz - uses values from 0xB2 as well. See manual. Default value provided.
    0xC6, 1, 0x0F,
    // (0xD0) Power Control 1
    // Sets AVDD, AVCL, and VDS voltages. 2 parameters. Values taken from Waveshare's own Arduino example LCD init code.
    0xD0, 2, 0xA4, 0xA1,
    // ST7789V gamma setting
    // (0xE0) Positive Voltage Gamma Control
    // 14 parameters. Values taken from Waveshare's own Arduino example LCD init code.
    0xE0, 14, 0xF0, 0x00, 0x04, 0x04, 0x04, 0x05, 0x29, 0x33, 0x3E, 0x38, 0x12, 0x12, 0x28, 0x30,
    // (0xE1) Negative Voltage Gamma Control
    // 14 parameters. Values taken from Waveshare's own Arduino example LCD init code.
    0xE1, 14, 0xF0, 0x07, 0x0A, 0x0D, 0x0B, 0x07, 0x28, 0x33, 0x3E, 0x36, 0x14, 0x14, 0x29, 0x32,
    // (0x21) Display Inversion On
    // This command is used to recover from display inversion mode. No parameters.
    0x21, 0,
    // (0x29) Display On
    // This command is used to recover from DISPLAY OFF mode. No parameters.
    0x29, 0 | DELAY, 255,
};

static void display_init(void) {
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;

    common_hal_busio_spi_construct(
        spi,
        &pin_GPIO7,    // CLK
        &pin_GPIO6,    // MOSI
        NULL,           // MISO not connected
        false);         // Not half-duplex

    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO15,    // DC
        &pin_GPIO14,    // CS
        &pin_GPIO21,    // RST
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
        &pin_GPIO22,    // backlight pin
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
