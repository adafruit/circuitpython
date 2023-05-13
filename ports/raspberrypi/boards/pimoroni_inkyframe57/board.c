/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"
#include "inky-shared.h"

#define DELAY 0x80

digitalio_digitalinout_obj_t enable_pin_obj;

enum reg {
    PSR = 0x00,
    PWR = 0x01,
    POF = 0x02,
    PFS = 0x03,
    PON = 0x04,
    BTST = 0x06,
    DSLP = 0x07,
    DTM1 = 0x10,
    DSP = 0x11,
    DRF = 0x12,
    IPC = 0x13,
    PLL = 0x30,
    TSC = 0x40,
    TSE = 0x41,
    TSW = 0x42,
    TSR = 0x43,
    CDI = 0x50,
    LPD = 0x51,
    TCON = 0x60,
    TRES = 0x61,
    DAM = 0x65,
    REV = 0x70,
    FLG = 0x71,
    AMV = 0x80,
    VV = 0x81,
    VDCS = 0x82,
    PWS = 0xE3,
    TSSET = 0xE5
};

const uint8_t display_start_sequence[] = {
    PWR, 0x04, 0x37, 0x00, 0x23, 0x23,
    PSR, 0x02, 0xE3, 0x08,                //  0xE3=no rotation
    PFS, 0x01, 0x00,
    BTST,0x03, 0xC7, 0xC7, 0x1D,
    PLL, 0x01, 0x3C,
    TSE, 0x01, 0x00,
    CDI, 0x01, 0x37,
    TCON,0x01, 0x22,
    TRES,0x04, 0x02, 0x58, 0x01, 0xC0,  // 0x0258: 600, 0x01C0: 448
    PWS, 0x01, 0xAA,                    // 0x64=100ms delay (Pimoroni)
    PON, DELAY,0xC8
};

const uint8_t display_stop_sequence[] = {
    POF, 0x01, 0x00,  // Power off
    DSLP,0x01, 0xA5   // deep-sleep
};

const uint8_t refresh_sequence[] = {
    DRF, 0x00
};

void board_init(void) {
    // Drive the EN_3V3 pin high so the board stays awake on battery power
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO2);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&enable_pin_obj);

    // Set up the SPI object used to control the display
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    common_hal_busio_spi_never_reset(spi);

    // Set up the DisplayIO pin object
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        &pin_GPIO28, // EPD_DC Command or data
        &pin_GPIO17, // EPD_CS Chip select
        &pin_GPIO27, // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    // Set up the DisplayIO epaper object
    displayio_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &displayio_epaperdisplay_type;
    common_hal_displayio_epaperdisplay_construct(
        display,
        bus,
        display_start_sequence, sizeof(display_start_sequence),
        1, // start up time
        display_stop_sequence, sizeof(display_stop_sequence),
        600,  // width
        448,  // height
        640,  // ram_width
        480,  // ram_height
        0,  // colstart
        0,  // rowstart
        0,  // rotation
        NO_COMMAND,  // set_column_window_command
        NO_COMMAND,  // set_row_window_command
        NO_COMMAND,  // set_current_column_command
        NO_COMMAND,  // set_current_row_command
        DTM1,  // write_black_ram_command
        false,  // black_bits_inverted
        NO_COMMAND,  // write_color_ram_command
        false,  // color_bits_inverted
        0x000000,  // highlight_color
        refresh_sequence, sizeof(refresh_sequence),  // refresh_display_command
        2.0,   // refresh_time
        NULL,  // busy_pin
        false,  // busy_state
        40,      // seconds_per_frame (update-interval)
        false, // always_toggle_chip_select
        false, // grayscale
        true,  // acep
        false,  // two_byte_sequence_length
        false); // address_little_endian
}

void board_deinit(void) {
    displayio_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &displayio_epaperdisplay_type) {
        size_t i = 0;
        while (common_hal_displayio_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
            i++;
        }
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
