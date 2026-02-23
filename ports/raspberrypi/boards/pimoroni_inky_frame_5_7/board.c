// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "shared-bindings/board/__init__.h"
#include "supervisor/shared/board.h"
#include "inky-shared.h"

#define DELAY 0x80

digitalio_digitalinout_obj_t enable_pin_obj;

// This is an SPD1656 control chip. The display is a 5.7" ACeP EInk.

const uint8_t display_start_sequence[] = {
    0x01, 4, 0x37, 0x00, 0x23, 0x23, // power setting
    0x00, 2, 0xe3, 0x08, // panel setting (PSR, 0xe3: no rotation)
    0x03, 1, 0x00, // PFS
    0x06, 3, 0xc7, 0xc7, 0x1d, // booster
    0x30, 1, 0x3c, // PLL setting
    0x41, 1, 0x00, // TSE
    0x50, 1, 0x37, // vcom and data interval setting
    0x60, 1, 0x22, // tcon setting
    0x61, 4, 0x02, 0x58, 0x01, 0xc0, // tres
    0xe3, 1, 0xaa, // PWS
    0x04, DELAY | 0, 0xc8, // VCM DC and delay 200ms
};

const uint8_t display_stop_sequence[] = {
    0x02, 1, 0x00, // power off
    0x07, 1, 0xa5 // deep sleep
};

const uint8_t refresh_sequence[] = {
    0x12, 0x00
};

void board_init(void) {
    // Drive the EN_3V3 pin high so the board stays awake on battery power
    enable_pin_obj.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&enable_pin_obj, &pin_GPIO2);
    common_hal_digitalio_digitalinout_switch_to_output(&enable_pin_obj, true, DRIVE_MODE_PUSH_PULL);

    // Never reset
    common_hal_digitalio_digitalinout_never_reset(&enable_pin_obj);
    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = common_hal_board_create_spi(0);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        MP_OBJ_FROM_PTR(&pin_GPIO28), // EPD_DC Command or data
        MP_OBJ_FROM_PTR(&pin_GPIO17), // EPD_CS Chip select
        MP_OBJ_FROM_PTR(&pin_GPIO27), // EPD_RST Reset
        1000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    epaperdisplay_construct_args_t args = EPAPERDISPLAY_CONSTRUCT_ARGS_DEFAULTS;
    args.bus = bus;
    args.start_sequence = display_start_sequence;
    args.start_sequence_len = sizeof(display_start_sequence);
    args.start_up_time = 1.0;
    args.stop_sequence = display_stop_sequence;
    args.stop_sequence_len = sizeof(display_stop_sequence);
    args.width = 600;
    args.height = 448;
    args.ram_width = 640;
    args.ram_height = 480;
    args.write_black_ram_command = 0x10;
    args.refresh_sequence = refresh_sequence;
    args.refresh_sequence_len = sizeof(refresh_sequence);
    args.refresh_time = 28.0;
    args.busy_pin = NULL;
    args.seconds_per_frame = 40.0;
    args.acep = true;
    common_hal_epaperdisplay_epaperdisplay_construct(display, &args);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
