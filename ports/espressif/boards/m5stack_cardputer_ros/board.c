// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "mpconfigboard.h"
#include "supervisor/board.h"
#include "supervisor/shared/serial.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/mipi_constants.h"
#include "shared-bindings/board/__init__.h"
#include "py/runtime.h"
#include "py/ringbuf.h"
#include "shared/runtime/interrupt_char.h"


#define DELAY 0x80

uint8_t display_init_sequence[] = {
    // SWRESET and Delay 140ms
    0x01, 0 | DELAY, 140,
    // SLPOUT and Delay 10ms
    0x11, 0 | DELAY, 10,
    // COLMOD 65k colors and 16 bit 5-6-5
    0x3A, 1, 0x55,
    // INVON Iiversion on
    0x21, 0,
    // NORON normal operation (full update)
    0x13, 0,
    // MADCTL columns RTL, page/column reverse order
    0x36, 1, 0x60,
    // RAMCTRL color word little endian
    0xB0, 2, 0x00, 0xF8,
    // DIPON display on
    0x29, 0,
};


// Overrides the weakly linked function from supervisor/shared/board.c
void board_init(void) {
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    bus->base.type = &fourwire_fourwire_type;

    // see here for inspiration: https://github.com/m5stack/M5GFX/blob/33d7d3135e816a86a008fae8ab3757938cee95d2/src/M5GFX.cpp#L1350
    common_hal_fourwire_fourwire_construct(
        bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO34),    // DC
        MP_OBJ_FROM_PTR(&pin_GPIO37),     // CS
        MP_OBJ_FROM_PTR(&pin_GPIO33),    // RST
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
        135,            // height (after rotation)
        40,             // column start
        53,             // row start
        0,              // rotation
        16,             // color depth
        false,          // grayscale
        false,          // pixels in a byte share a row. Only valid for depths < 8
        1,              // bytes per cell. Only valid for depths < 8
        false,          // reverse_pixels_in_byte. Only valid for depths < 8
        false,          // reverse_pixels_in_word
        MIPI_COMMAND_SET_COLUMN_ADDRESS, // set column command
        MIPI_COMMAND_SET_PAGE_ADDRESS,   // set row command
        MIPI_COMMAND_WRITE_MEMORY_START, // write memory command
        display_init_sequence,
        sizeof(display_init_sequence),
        &pin_GPIO38,    // backlight pin
        NO_BRIGHTNESS_COMMAND,
        1.0f,           // brightness
        false,          // single_byte_bounds
        false,          // data_as_commands
        true,           // auto_refresh
        60,             // native_frames_per_second
        true,           // backlight_on_high
        false,          // SH1107_addressing
        350             // backlight pwm frequency
        );
}

// TODO: Should we turn off the display when asleep, in board_deinit() ?
