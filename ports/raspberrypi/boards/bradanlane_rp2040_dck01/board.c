/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Bradán Lane STUDIO
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
#include "supervisor/shared/board.h"

#define HEIGHT 200
#define WIDTH  200

#define DELAY_FLAG 0x80

#define EPD_RAM_BW      0x10
#define EPD_RAM_RED     0x13
// #define BUSY_WAIT	500

// These commands are the combination of SSD1608 and SSD1681 and not all commands are supported for each controller
#define SSD_DRIVER_CONTROL                      0x01
#define SSD_GATE_VOLTAGE                        0x03
#define SSD_SOURCE_VOLTAGE                      0x04
#define SSD_DISPLAY_CONTROL                     0x07
#define SSD_PROGOTP_INITIAL                     0x08
#define SSD_WRITEREG_INITIAL            0x09
#define SSD_READREG_INITIAL                     0x0A
#define SSD_NON_OVERLAP                         0x0B
#define SSD_BOOST_SOFT_START            0x0C
#define SSD_DEEP_SLEEP                          0x10
#define SSD_DATA_MODE                           0x11
#define SSD_SW_RESET                            0x12
#define SSD_HV_DETECT                           0x14
#define SSD_VCI_DETECT                          0x15
#define SSD1681_TEMP_CONTROL            0x18
#define SSD1608_TEMP_CONTROL            0x1C
#define SSD_TEMP_WRITE                          0x1A
#define SSD_TEMP_READ                           0x1B
#define SSD_TEMP_EXTERN                         0x1C
#define SSD_MASTER_ACTIVATE                     0x20
#define SSD_DISP_CTRL1                          0x21
#define SSD_DISP_CTRL2                          0x22
#define SSD_WRITE_RAM_BLK                       0x24
// #define SSD_READ_RAM_BLK			0x25
#define SSD_WRITE_RAM_RED                       0x26
// #define SSD_READ_RAM_RED			0x27
#define SSD_VCOM_SENSE                          0x28
// #define SSD_VCOM_DURRATION		0x29
// #define SSD_PROG_VCOM			0x2A
// #define SSD_CTRL_VCOM			0x2B
// #define SSD_WRITE_VCOM			0x2C
#define SSD_READ_OTP                            0x2D
#define SSD_READ_ID                                     0x2E
#define SSD_READ_STATUS                         0x2F
#define SSD_WRITE_LUT                           0x32
#define SSD_WRITE_DUMMY                         0x3A
#define SSD1608_WRITE_GATELINE          0x3B
#define SSD_WRITE_BORDER                        0x3C
#define SSD_SET_RAMXPOS                         0x44
#define SSD_SET_RAMYPOS                         0x45
#define SSD_SET_RAMXCOUNT                       0x4E
#define SSD_SET_RAMYCOUNT                       0x4F
#define SSD_NOP                                         0xFF

#define SSD_TEMP_CONTROL SSD1681_TEMP_CONTROL

const uint8_t _start_sequence[] = {
    SSD_SW_RESET,                   DELAY_FLAG + 0, 20,                                                                                                         // soft reset and wait 20ms
    SSD_DATA_MODE,                  1,              0x03,                                                                                                               // Data entry sequence
    SSD_WRITE_BORDER,               1,              0x05,                                                                                                               // border color
    SSD_TEMP_CONTROL,               1,              0x80,                                                                                                               // Temp control
    SSD_SET_RAMXCOUNT,              1,              0x00,
    SSD_SET_RAMYCOUNT,              2,              0x00, 0x00,
    SSD_DRIVER_CONTROL, 3,  ((WIDTH - 1) & 0xFF), (((WIDTH >> 8) - 1) & 0xFF), 0x00,                            // set display size
    SSD_DISP_CTRL2,                 1,              0xf7,                                                                                                               // Set DISP only full refreshes
};

const uint8_t _stop_sequence[] = {
    SSD_DEEP_SLEEP,                                 DELAY_FLAG + 1, 1, 0x64                                                                             // Enter deep sleep
};

const uint8_t _refresh_sequence[] = {
    SSD_MASTER_ACTIVATE, 0,
//	SSD_SW_RESET, 0,
};

void board_init(void) {
    // Set up the SPI object used to control the display
    displayio_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, DEFAULT_SPI_BUS_SCK, DEFAULT_SPI_BUS_MOSI, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    // Set up the DisplayIO pin object
    bus->base.type = &displayio_fourwire_type;
    common_hal_displayio_fourwire_construct(bus,
        spi,
        DEFAULT_SPI_BUS_DC,                                                                                                                                                     // EPD_DC Command or data
        DEFAULT_SPI_BUS_CS,                                                                                                                                                     // EPD_CS Chip select
        DEFAULT_SPI_BUS_RESET,                                                                                                                                          // EPD_RST Reset
        1000000,                                                                                                                                                                        // Baudrate
        0,                                                                                                                                                                                      // Polarity
        0);                                                                                                                                                                                     // Phase

    // Set up the DisplayIO epaper object
    displayio_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &displayio_epaperdisplay_type;

    common_hal_displayio_epaperdisplay_construct(
        display,
        bus,
        _start_sequence, sizeof(_start_sequence),
        1.0,                     // start up time
        _stop_sequence, sizeof(_stop_sequence),
        WIDTH,                                                                                                                                                                          // width
        HEIGHT,                                                                                                                                                                         // height
        WIDTH,                                                                                                                                                                          // ram_width
        HEIGHT + 0x60,                                                                                                                                                          // ram_height RAM is actually only 200 bits high but we use 296 to match the 9 bits
        0,                                                                                                                                                                                      // colstart
        0,                                                                                                                                                                                      // rowstart
        0,                                                                                                                                                                              // rotation
        SSD_SET_RAMXPOS,                                                                                                                                                        // set_column_window_command
        SSD_SET_RAMYPOS,                                                                                                                                                        // set_row_window_command
        SSD_SET_RAMXCOUNT,                                                                                                                                                      // set_current_column_command
        SSD_SET_RAMYCOUNT,                                                                                                                                                      // set_current_row_command
        SSD_WRITE_RAM_BLK,                                                                                                                                                      // write_black_ram_command
        false,                                                                                                                                                                          // black_bits_inverted
        SSD_WRITE_RAM_RED,                                                                                                                                                      // write_color_ram_command
        false,                                                                                                                                                                          // color_bits_inverted
        0xFF0000,                                                                                                                                                                       // highlight_color (RED for tri-color display)
        _refresh_sequence, sizeof(_refresh_sequence),                                                                                           // refresh_display_command
        15.0,                                                                                                                                                                           // refresh_time
        DEFAULT_SPI_BUS_BUSY,                                                                                                                                           // busy_pin
        true,                                                                                                                                                                           // busy_state
        20.0,                                                                                                                                                                           // seconds_per_frame (does not seem the user can change this)
        true,                                                                                                                                                                           // always_toggle_chip_select
        false,                                                                                                                                                                          // grayscale
        false,                                                                                                                                                                          // acep
        false,                                                                                                                                                                          // two_byte_sequence_length
        true);                                                                                                                                                                          // address_little_endian
}

void board_deinit(void) {
    #if 0
    displayio_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &displayio_epaperdisplay_type) {
        while (common_hal_displayio_epaperdisplay_get_busy(display)) {
            // RUN_BACKGROUND_TASKS;
        }
    }
    common_hal_displayio_release_displays();
    #endif
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
