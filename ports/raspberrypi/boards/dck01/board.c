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

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.


#include "supervisor/board.h"
#include "supervisor/shared/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/displayio/FourWire.h"
// #include "shared-bindings/time/__init__.h"
#include "shared-module/displayio/__init__.h"
// #include "shared-module/displayio/mipi_constants.h"

displayio_fourwire_obj_t board_display_obj;

#define HEIGHT  200
#define WIDTH   200

#define DELAY   0x80

#define EPD_RAM_BW      0x10
#define EPD_RAM_RED 0x13
#define BUSY_WAIT       500

// These commands are the combination of SSD1608 and SSD1681 and not all commands are supported for each controller
#define SSD_DRIVER_CONTROL              0x01
#define SSD_GATE_VOLTAGE                0x03
#define SSD_SOURCE_VOLTAGE              0x04
#define SSD_DISPLAY_CONTROL             0x07
#define SSD_PROGOTP_INITIAL             0x08
#define SSD_PROGREG_INITIAL             0x09
#define SSD_READREG_INITIAL             0x0A
#define SSD_NON_OVERLAP                 0x0B
#define SSD_BOOST_SOFT_START    0x0C
#define SSD_DEEP_SLEEP                  0x10
#define SSD_DATA_MODE                   0x11
#define SSD_SW_RESET                    0x12
#define SSD1681_TEMP_CONTROL    0x18
#define SSD1608_TEMP_CONTROL    0x1C
#define SSD_TEMP_WRITE                  0x1A
#define SSD_TEMP_READ                   0x1B
#define SSD_MASTER_ACTIVATE             0x20
#define SSD_DISP_CTRL1                  0x21
#define SSD_DISP_CTRL2                  0x22
#define SSD_WRITE_RAM1                  0x24
#define SSD_READ_RAM1                   0x24
#define SSD_WRITE_RAM2                  0x26
#define SSD_READ_RAM2                   0x24
#define SSD_WRITE_VCOM                  0x2C
#define SSD_READ_OTP                    0x2D
#define SSD_READ_STATUS                 0x2F
#define SSD_WRITE_LUT                   0x32
#define SSD_WRITE_DUMMY                 0x3A
#define SSD1608_WRITE_GATELINE  0x3B
#define SSD_WRITE_BORDER                0x3C
#define SSD_SET_RAMXPOS                 0x44
#define SSD_SET_RAMYPOS                 0x45
#define SSD_SET_RAMXCOUNT               0x4E
#define SSD_SET_RAMYCOUNT               0x4F
#define SSD_NOP                                 0xFF


#if 1   // SSD1608

#define SSD_TEMP_CONTROL                SSD1608_TEMP_CONTROL

const uint8_t _start_sequence[] = {
    SSD_SW_RESET,                    0,                                         // soft reset
    SSD_NOP, 20,                                                                // busy wait 20ms
    SSD_DRIVER_CONTROL,          3, ((WIDTH - 1) & 0xFF), ((WIDTH - 1) >> 8), 0x00, // set display size
    SSD_WRITE_DUMMY,             1, 0x1b,                       // Set dummy line period
    SSD1608_WRITE_GATELINE,      1, 0x0b,                       // Set gate line width
    SSD_DATA_MODE,                       1, 0x03,                       // Data entry sequence
    SSD_SET_RAMXPOS,                 1, 0x00,
    SSD_SET_RAMYPOS,                 2, 0x00, 0x00,
    SSD_WRITE_VCOM,                      1, 0x70,                       // Vcom Voltage
    SSD_WRITE_LUT,                      30, 0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xb4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00,         // LUT
    SSD_DISP_CTRL2,                      1, 0xc7,                       // Set DISP only full refreshes
    SSD_NOP, 20,                                                                // busy wait
};

const uint8_t _stop_sequence[] = {
    SSD_DEEP_SLEEP,                  1, 0x01                            // Enter deep sleep
};

const uint8_t _refresh_sequence[] = {
    SSD_SW_RESET,                    0,                                         // soft reset
};

#else   // SD1681

#define SSD_TEMP_CONTROL                SSD1681_TEMP_CONTROL

const uint8_t _start_sequence[] = {
    SSD_SW_RESET,, 0x80, 20,                                    // soft reset and wait 20ms
    SSD_DATA_MODE,                       1, 0x03,                       // Data entry sequence
    SSD_SET_RAMXPOS,                 1, 0x00,
    SSD_SET_RAMYPOS,                 2, 0x00, 0x00,
    SSD_WRITE_BORDER,            1, 0x05                        // border color
    SSD_TEMP_CONTROL,                1, 0x80,                           // Temp control
    SSD_DRIVER_CONTROL, 3, ((WIDTH - 1) & 0xFF), ((WIDTH - 1) >> 8), 0x00       // set display size
    SSD_DISP_CTRL2,                      1, 0xc7,                       // Set DISP only full refreshes
};

const uint8_t _stop_sequence[] = {
    SSD_DEEP_SLEEP, 0x81,    1, 0x64                            // Enter deep sleep
};

const uint8_t _refresh_sequence[] = {
    SSD_SW_RESET,                    0,                                         // soft reset
};

#endif  // EPD choice


void board_init(void) {
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;

    common_hal_busio_spi_construct(spi, DEFAULT_SPI_BUS_SCK, DEFAULT_SPI_BUS_MOSI, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    // Set up the DisplayIO pin object
    bus->base.type = &displayio_fourwire_type;

    common_hal_displayio_fourwire_construct(
        bus,
        spi,
        DEFAULT_SPI_BUS_DC, // EPD_DC Command or data
        DEFAULT_SPI_BUS_CS, // EPD_CS Chip select
        DEFAULT_SPI_BUS_RESET, // EPD_RST Reset
        1200000, // Baudrate
        0, // Polarity
        0); // Phase

    // Set up the DisplayIO epaper object
    displayio_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &displayio_epaperdisplay_type;

    common_hal_displayio_epaperdisplay_construct(
        display,
        bus,
        _start_sequence,
        sizeof(_start_sequence),
        0, // start up time
        _stop_sequence,
        sizeof(_stop_sequence),
        WIDTH, // width
        HEIGHT, // height
        WIDTH, // RAM width
        HEIGHT, // RAM height
        0, // colstart
        0, // rowstart
        0, // rotation
        NO_COMMAND, // set_column_window_command
        NO_COMMAND, // set_row_window_command
        NO_COMMAND, // set_current_column_command
        NO_COMMAND, // set_current_row_command
        0x13, // write_black_ram_command
        false, // black_bits_inverted
        NO_COMMAND, // write_color_ram_command (can add this for grayscale eventually)
        false, // color_bits_inverted
        0x000000, // highlight_color
        _refresh_sequence, // refresh_display_sequence
        sizeof(_refresh_sequence),
        45, // refresh_time (seconds)
        DEFAULT_SPI_BUS_BUSY, // busy_pin
        false, // busy_state
        5, // seconds_per_frame (minimum and Adafruit recommends 180)
        false, // chip_select (don't always toggle chip select)
        false, // grayscale
        false, // acep
        false, // two_byte_sequence_length
        false // address_little_endian
        );
}


void board_deinit(void) {
    displayio_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &displayio_epaperdisplay_type) {
        while (common_hal_displayio_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
        }
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
