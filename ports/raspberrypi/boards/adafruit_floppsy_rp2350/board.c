// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "hardware/gpio.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"


uint8_t display_init_sequence[] = {
    0x01, 0x80, 0x80,  // Software reset then delay 0x80 (128ms)
    0xEF, 0x03, 0x03, 0x80, 0x02,
    0xCF, 0x03, 0x00, 0xC1, 0x30,
    0xED, 0x04, 0x64, 0x03, 0x12, 0x81,
    0xE8, 0x03, 0x85, 0x00, 0x78,
    0xCB, 0x05, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 0x01, 0x20,
    0xEA, 0x02, 0x00, 0x00,
    0xc0, 0x01, 0x23,  // Power control VRH[5:0]
    0xc1, 0x01, 0x10,  // Power control SAP[2:0];BT[3:0]
    0xc5, 0x02, 0x3e, 0x28,  // VCM control
    0xc7, 0x01, 0x86,  // VCM control2
    0x36, 0x01, 0x28,  // Memory Access Control
    0x37, 0x01, 0x00,  // Vertical scroll zero
    0x3a, 0x01, 0x55,  // COLMOD: Pixel Format Set
    0xb1, 0x02, 0x00, 0x18,  // Frame Rate Control (In Normal Mode/Full Colors)
    0xb6, 0x03, 0x08, 0x82, 0x27,  // Display Function Control
    0xF2, 0x01, 0x00,  // 3Gamma Function Disable
    0x26, 0x01, 0x01,  // Gamma curve selected
    0xe0, 0x0f, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07,
    0x10, 0x03, 0x0E, 0x09, 0x00,  // Set Gamma
    0xe1, 0x0f, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08,
    0x0F, 0x0C, 0x31, 0x36, 0x0F,  // Set Gamma
    0x11, 0x80, 0x78,  // Exit Sleep then delay 0x78 (120ms)
    0x29, 0x80, 0x78,  // Display on then delay 0x78 (120ms)
};

// Keep cable drivers isolated and write/motor/select signals inactive after reset.
// Python can claim these GPIOs normally; deinit returns them to this safe state.
bool board_reset_pin_number(uint8_t pin_number) {
    bool high = pin_number == 10 || pin_number == 20 || pin_number == 22 ||
        pin_number == 25 || pin_number == 26 || pin_number == 30 || pin_number == 31;
    if (high || pin_number == 0) {
        gpio_put(pin_number, high);
        gpio_set_dir(pin_number, GPIO_OUT);
        gpio_set_function(pin_number, GPIO_FUNC_SIO);
        return true;
    }
    return false;
}

void board_init(void) {
    const uint8_t safe_pins[] = {10, 20, 22, 25, 26, 30, 31, 0};
    for (size_t i = 0; i < sizeof(safe_pins); i++) {
        board_reset_pin_number(safe_pins[i]);
    }
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_TFT_DC),
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_TFT_CS),
        MP_OBJ_FROM_PTR(CIRCUITPY_BOARD_TFT_RESET), // TFT reset
        24000000, // Baudrate
        0, // Polarity
        0); // Phase

    busdisplay_busdisplay_obj_t *display = &allocate_display()->display;
    display->base.type = &busdisplay_busdisplay_type;
    common_hal_busdisplay_busdisplay_construct(
        display,
        bus,
        320, // Width (after rotation)
        240, // Height (after rotation)
        0, // column start
        0, // row start
        0, // rotation
        16, // Color depth
        false, // Grayscale
        false, // Pixels in a byte share a row. Only used for depth < 8
        1, // bytes per cell. Only valid for depths < 8
        false, // reverse_pixels_in_byte. Only valid for depths < 8
        true, // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // Set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS, // Set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // Write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        CIRCUITPY_BOARD_TFT_BACKLIGHT,
        NO_BRIGHTNESS_COMMAND,
        1.0f, // brightness
        false, // single_byte_bounds
        false, // data_as_commands
        true, // auto_refresh
        60, // native_frames_per_second
        true, // backlight_on_high
        false, // not SH1107
        50000); // backlight pwm frequency
}

void board_deinit(void) {
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
