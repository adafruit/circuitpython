// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Hassan Naeem
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/qspibus/QSPIBus.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"

// CO5300 start-up, as in Waveshare's ESP-IDF BSP for this board.
static uint8_t display_init_sequence[] = {
    0xFE, 0x01, 0x20,             // Page 0x20
    0x19, 0x01, 0x10,             // Vendor command
    0x1C, 0x01, 0xA0,             // Vendor command
    0xFE, 0x01, 0x00,             // Back to the user command page
    0xC4, 0x01, 0x80,             // QSPI interface
    0x3A, 0x01, 0x55,             // COLMOD: 16-bit RGB565
    0x35, 0x01, 0x00,             // Tearing effect line on
    0x53, 0x01, 0x20,             // Brightness control on
    0x51, 0x01, 0xFF,             // Brightness
    0x63, 0x01, 0xFF,             // High brightness mode level
    0x11, 0x80, 0xFF,             // Sleep out + 500ms
    0x29, 0x80, 0x0A,             // Display on + 10ms
};

void board_init(void) {
    qspibus_qspibus_obj_t *bus = &allocate_display_bus_or_raise()->qspi_bus;
    bus->base.type = &qspibus_qspibus_type;

    common_hal_qspibus_qspibus_construct(bus,
        CIRCUITPY_LCD_CLK,
        CIRCUITPY_LCD_D0,
        CIRCUITPY_LCD_D1,
        CIRCUITPY_LCD_D2,
        CIRCUITPY_LCD_D3,
        CIRCUITPY_LCD_CS,
        NULL,                 // dcx: QSPI encodes commands
        CIRCUITPY_LCD_RESET,
        40000000);

    // The 466x466 panel's first visible column is 6.
    busdisplay_busdisplay_obj_t *display = &allocate_display_or_raise()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        466,          // width
        466,          // height
        6,            // colstart
        0,            // rowstart
        0,            // rotation
        16,           // color_depth
        false,        // grayscale
        false,        // pixels_in_byte_share_row
        1,            // bytes_per_cell
        false,        // reverse_pixels_in_byte
        true,         // reverse_bytes_in_word: the CO5300 takes RGB565 high byte first
        MIPI_COMMAND_SET_COLUMN_ADDRESS,
        MIPI_COMMAND_SET_PAGE_ADDRESS,
        MIPI_COMMAND_WRITE_MEMORY_START,
        display_init_sequence,
        sizeof(display_init_sequence),
        NULL,         // no backlight pin on an AMOLED
        0x51,         // brightness_command
        1.0f,         // brightness
        false,        // single_byte_bounds
        false,        // data_as_commands
        true,         // auto_refresh
        60,           // native_frames_per_second
        true,         // backlight_on_high
        false,        // SH1107_addressing
        50000);       // backlight_pwm_frequency
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
