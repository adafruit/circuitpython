// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "hardware/gpio.h"

// Display pins from Pimoroni Explorer parallel bus: {cs=27, dc=28, wr=30, rd=31, d0=32, bl=26}
#define LCD_BACKLIGHT_PIN 26
#define LCD_CS_PIN 27
#define LCD_DC_PIN 28
#define LCD_WR_PIN 30
#define LCD_RD_PIN 31
#define LCD_D0_PIN 32  // Data pins are GPIO32-39 (8 consecutive pins)

#define DELAY 0x80

// ST7789V display init sequence for 320x240 parallel bus
// Based on Pimoroni's pimoroni-pico ST7789 driver configuration
uint8_t display_init_sequence[] = {
    // Software reset
    0x01, 0 | DELAY, 150,
    // Sleep out
    0x11, 0 | DELAY, 255,
    // Tearing effect line on (frame sync)
    0x35, 1, 0x00,
    // COLMOD: 16-bit color (5-6-5 RGB)
    0x3A, 1, 0x55,
    // Porch control (PORCTRL)
    0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
    // Gate control (GCTRL) - VGH=13.26V, VGL=-10.43V
    0xB7, 1, 0x35,
    // VCOM setting (VCOMS)
    0xBB, 1, 0x1F,
    // LCM control (LCMCTRL)
    0xC0, 1, 0x2C,
    // VDV and VRH command enable (VDVVRHEN)
    0xC2, 1, 0x01,
    // VRH set (VRHS)
    0xC3, 1, 0x12,
    // VDV set (VDVS)
    0xC4, 1, 0x20,
    // Frame rate control (FRCTRL2)
    0xC6, 1, 0x0F,
    // Power control 1 (PWCTRL1)
    0xD0, 2, 0xA4, 0xA1,
    // RAM control (RAMCTRL) - for proper endianness
    0xB0, 2, 0x00, 0xC0,
    // Positive gamma correction
    0xE0, 14, 0xD0, 0x08, 0x11, 0x08, 0x0C, 0x15, 0x39, 0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2D,
    // Negative gamma correction
    0xE1, 14, 0xD0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39, 0x44, 0x51, 0x0B, 0x16, 0x14, 0x2F, 0x31,
    // Inversion on
    0x21, 0,
    // Normal display mode on
    0x13, 0 | DELAY, 10,
    // MADCTL: MX=0, MY=1, MV=1, ML=1 (COL_ORDER | SWAP_XY | SCAN_ORDER) = 0x70
    // This configures the 320x240 display in landscape orientation
    0x36, 1, 0x70,
    // Display on
    0x29, 0 | DELAY, 100,
};

static void display_init(void) {
    paralleldisplaybus_parallelbus_obj_t *bus = &allocate_display_bus()->parallel_bus;
    bus->base.type = &paralleldisplaybus_parallelbus_type;

    common_hal_paralleldisplaybus_parallelbus_construct(bus,
        &pin_GPIO32,  // Data0 (D0) - data pins are sequential GPIO32-39
        &pin_GPIO28,  // Command/Data (DC)
        &pin_GPIO27,  // Chip select (CS)
        &pin_GPIO30,  // Write (WR)
        &pin_GPIO31,  // Read (RD)
        NULL,         // Reset (directly connected to board reset)
        15000000);    // Frequency - ST7789 min clock cycle ~66ns = ~15MHz

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;

    common_hal_busdisplay_busdisplay_construct(display,
        bus,
        320, // Width
        240, // Height
        0,   // column start
        0,   // row start
        0,   // rotation
        16,  // Color depth
        false, // grayscale
        false, // pixels_in_byte_share_row
        1,   // bytes per cell
        false, // reverse_pixels_in_byte
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO26, // Backlight pin (BL)
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true,  // auto_refresh
        60,    // native_frames_per_second
        true,  // backlight_on_high
        false, // SH1107_addressing
        50000  // backlight pwm frequency
        );
}

void board_init(void) {
    // Ensure backlight is on before display init
    board_reset_pin_number(LCD_BACKLIGHT_PIN);
    display_init();
}

// Prevent the backlight pin from being reset, keeping display visible across soft resets
bool board_reset_pin_number(uint8_t pin_number) {
    if (pin_number == LCD_BACKLIGHT_PIN) {
        // Keep backlight on - set high output without glitching
        gpio_put(pin_number, 1);
        gpio_set_dir(pin_number, GPIO_OUT);
        gpio_set_function(pin_number, GPIO_FUNC_SIO);
        return true;
    }
    return false;
}

void reset_board(void) {
    // Keep backlight on during reset
    board_reset_pin_number(LCD_BACKLIGHT_PIN);
}

void board_deinit(void) {
    // Backlight will be handled by board_reset_pin_number
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
