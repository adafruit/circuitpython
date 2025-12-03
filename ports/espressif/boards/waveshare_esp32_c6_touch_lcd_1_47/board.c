// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Benjamin Shockley
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
// Board used for this commit : https://www.waveshare.com/wiki/ESP32-C6-Touch-LCD-1.47

// Driver is ST7789V3     https://www.waveshare.com/w/upload/a/ad/ST7789VW.pdf
// Display Panel is LBS147TC-IF15
// 172 X 320 Pixels RGB 18-bit

// Init Sequence format
// Addr, seq length, data
uint8_t display_init_sequence[] = {
    0x01, 0 | DELAY, 120,			// Software Reset - SWRESET
    0x11, 0 | DELAY, 120,			// Sleep Out - SLPOUT
    0x13, 0,					// Normal Display Mode ON - NORON
    0x36, 1, 0x88,                              // Memory Data Access Control - MADCTL
    //0x3A, 1 | DELAY,  0x05, 10,			// Interface Pixel Format - COLMOD    0x05= 16bits/pixel   ,0x10   
    0x3A, 1 | DELAY,  0x55, 10,			// Interface Pixel Format - COLMOD    0x05= 16bits/pixel   ,0x10   
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,	// Porch Setting - PORCTRL
    0xB7, 1, 0x35,				// Gate Control - GCTRL
    0xBB, 1, 0x20,				// VCOM Setting - VCOMS
    0xC0, 1, 0x2C,				// LCM Control - LCMCTRL
    0xC2, 2, 0x01, 0xFF,			// VDV and VRH Command Enable - VDVVRHEN
    0xC3, 1, 0x13,				// VRH Set - VRHS
    0xC4, 1, 0x20,				// VDV Set - VDVS
    0xC6, 1, 0x0F,				// Frame Rate CTRL in Normal Mode - FRCTRL2
    0xD0, 2, 0xA4, 0xA1,			// Power Control 1 - PWCTRL1
    0xE0, 14, 0xF0, 0x00, 0x04, 0x04, 0x04, 0x05, 0x29, 0x33, 0x3E, 0x38, 0x12, 0x12, 0x28, 0x30,	// Positive Voltage Gamma Control
    0xE1, 14, 0xF0, 0x07, 0x0A, 0x0D, 0x0B, 0x07, 0x28, 0x33, 0x3E, 0x36, 0x14, 0x14, 0x29, 0x32,	// Negative Voltage Gamma Control
    0x21, 0,					// Display Inversion On - INVON
    0x29, 0 | DELAY, 255,			// Display ON - DISPON
};
// MADCTL       D7->D2 =   MY,MX,MV,ML,RGB,MH,0,0
//	MY = PAge Address ORder  0=Top to Bottom   1= Bottom to Top
//	MX = Column Address order  0 = left to right 1 = right to left
//	MV = Page column order	   0 = Normal 1=Reverse
//	ML = Line Address Order    0=Top-Bottom  1=Bottom-top
//	RGB  0=RGB 1=BGR
//      MH = Display Data Latch Data Order    0=Left2Right 1=Right2LEft
//      0x0C   1100
//
static void display_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        &pin_GPIO15,    // DC
        &pin_GPIO14,    // CS
        &pin_GPIO22,    // RST changed from 21 to 22 for TOUCH version
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
        34,              // column start
        0,              // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        false,           // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command    0x2A
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command       0x2B
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command  0x2C
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO23,    // backlight pin from 22 to 23 for TOUCH version
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
