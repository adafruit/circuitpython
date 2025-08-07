// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"

#include "mpconfigboard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/board.h"

#include "esp_log.h"

static const char *TAG = "board";

#define DELAY 0x80

// This is an ILO373 control chip. The display is a 2.9" grayscale EInk.

const uint8_t il0373_display_start_sequence[] = {
    0x01, 5, 0x03, 0x00, 0x2b, 0x2b, 0x13, // power setting
    0x06, 3, 0x17, 0x17, 0x17, // booster soft start
    0x04, DELAY, 200, // power on and wait 200 ms
    0x00, 1, 0x7f, // panel setting
    0x50, 1, 0x97, // CDI setting
    0x30, 1, 0x3c, // PLL set to 50 Hx (M = 7, N = 4)
    0x61, 3, 0x80, 0x01, 0x28, // Resolution
    0x82, DELAY | 1, 0x12, 50, // VCM DC and delay 50ms

    // Look up tables for voltage sequence for pixel transition
    // Common voltage
    0x20, 0x2a,
    0x00, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x60, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x13, 0x0a, 0x01, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // White to white
    0x21, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x10, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0xa0, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to white
    0x22, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x99, 0x0c, 0x01, 0x03, 0x04, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // White to black
    0x23, 0x2a,
    0x40, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x00, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x99, 0x0b, 0x04, 0x04, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // Black to black
    0x24, 0x2a,
    0x80, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x90, 0x14, 0x14, 0x00, 0x00, 0x01,
    0x20, 0x14, 0x0a, 0x00, 0x00, 0x01,
    0x50, 0x13, 0x01, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t il0373_display_stop_sequence[] = {
    0x50, 0x01, 0x17,  // CDI Setting
    0x82, 0x01, 0x00,  // VCM DC to -0.1V
    0x02, 0x00  // Power off
};

const uint8_t il0373_display_refresh_sequence[] = {
    0x12, 0x00
};


// This is an SSD1680 control chip. The display is a 2.9" grayscale EInk.
const uint8_t ssd1680_display_start_sequence[] = {
    0x12, DELAY, 0x00, 0x14, // soft reset and wait 20ms
    0x11, 0x00, 0x01, 0x03, // Ram data entry mode
    0x3c, 0x00, 0x01, 0x03, // border color
    0x2c, 0x00, 0x01, 0x28, // Set vcom voltage
    0x03, 0x00, 0x01, 0x17, // Set gate voltage
    0x04, 0x00, 0x03, 0x41, 0xae, 0x32, // Set source voltage
    0x4e, 0x00, 0x01, 0x01, // ram x count
    0x4f, 0x00, 0x02, 0x00, 0x00, // ram y count
    0x01, 0x00, 0x03, 0x27, 0x01, 0x00, // set display size
    0x32, 0x00, 0x99, // Update waveforms
    0x2a, 0x60, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L0
    0x20, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L1
    0x28, 0x60, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L2
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L3
    0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // VS L4
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,     // TP, SR, RP of Group0
    0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x01,     // TP, SR, RP of Group1
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,     // TP, SR, RP of Group2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // TP, SR, RP of Group11
    0x24, 0x22, 0x22, 0x22, 0x23, 0x32, 0x00, 0x00, 0x00,     // FR, XON
    0x22, 0x00, 0x01, 0xc7 // display update mode
};

const uint8_t ssd1680_display_stop_sequence[] = {
    0x10, DELAY, 0x01, 0x01, 0x64
};

const uint8_t ssd1680_display_refresh_sequence[] = {
    0x20, 0x00, 0x00
};

static bool detect_ssd1680(void) {
    // Bitbang 4-wire SPI with a bidirectional data line to read register 0x71.
    // On the IL0373 it will return 0x13 or similar. On the SSD1680 it is
    // unsupported and will be 0xff.
    digitalio_digitalinout_obj_t data;
    digitalio_digitalinout_obj_t clock;
    digitalio_digitalinout_obj_t chip_select;
    digitalio_digitalinout_obj_t data_command;
    digitalio_digitalinout_obj_t reset;
    data.base.type = &digitalio_digitalinout_type;
    clock.base.type = &digitalio_digitalinout_type;
    chip_select.base.type = &digitalio_digitalinout_type;
    data_command.base.type = &digitalio_digitalinout_type;
    reset.base.type = &digitalio_digitalinout_type;

    common_hal_digitalio_digitalinout_construct(&data, &pin_GPIO35);
    common_hal_digitalio_digitalinout_construct(&clock, &pin_GPIO36);
    common_hal_digitalio_digitalinout_construct(&chip_select, &pin_GPIO8);
    common_hal_digitalio_digitalinout_construct(&data_command, &pin_GPIO7);
    common_hal_digitalio_digitalinout_construct(&reset, &pin_GPIO6);

    // Set CS and DC low
    common_hal_digitalio_digitalinout_switch_to_output(&chip_select, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&data_command, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&data, false, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&reset, true, DRIVE_MODE_PUSH_PULL);
    common_hal_digitalio_digitalinout_switch_to_output(&clock, false, DRIVE_MODE_PUSH_PULL);

    uint8_t status_read = 0x71;
    for (int i = 0; i < 8; i++) {
        common_hal_digitalio_digitalinout_set_value(&data, (status_read & (1 << (7 - i))) != 0);
        common_hal_digitalio_digitalinout_set_value(&clock, true);
        common_hal_digitalio_digitalinout_set_value(&clock, false);
    }

    // Set DC high for data and switch to input with pull-up in case the SSD1680 doesn't send any
    // data back (as it should.)
    common_hal_digitalio_digitalinout_switch_to_input(&data, PULL_UP);
    common_hal_digitalio_digitalinout_set_value(&data_command, true);
    uint8_t status = 0;
    for (int bit = 0; bit < 8; bit++) {
        status <<= 1;
        if (common_hal_digitalio_digitalinout_get_value(&data)) {
            status |= 1;
        }
        common_hal_digitalio_digitalinout_set_value(&clock, true);
        common_hal_digitalio_digitalinout_set_value(&clock, false);
    }

    // Set CS high
    common_hal_digitalio_digitalinout_set_value(&chip_select, true);

    common_hal_digitalio_digitalinout_deinit(&data);
    common_hal_digitalio_digitalinout_deinit(&clock);
    common_hal_digitalio_digitalinout_deinit(&chip_select);
    common_hal_digitalio_digitalinout_deinit(&data_command);
    common_hal_digitalio_digitalinout_deinit(&reset);
    return status == 0xff;
}

void board_init(void) {
    bool is_ssd1680 = detect_ssd1680();

    fourwire_fourwire_obj_t *bus = &allocate_display_bus()->fourwire_bus;
    busio_spi_obj_t *spi = &bus->inline_bus;
    common_hal_busio_spi_construct(spi, &pin_GPIO36, &pin_GPIO35, NULL, false);
    common_hal_busio_spi_never_reset(spi);

    bus->base.type = &fourwire_fourwire_type;
    common_hal_fourwire_fourwire_construct(bus,
        spi,
        &pin_GPIO7, // EPD_DC Command or data
        &pin_GPIO8, // EPD_CS Chip select
        &pin_GPIO6, // EPD_RST Reset
        4000000, // Baudrate
        0, // Polarity
        0); // Phase

    epaperdisplay_epaperdisplay_obj_t *display = &allocate_display()->epaper_display;
    display->base.type = &epaperdisplay_epaperdisplay_type;

    if (is_ssd1680) {
        common_hal_epaperdisplay_epaperdisplay_construct(
            display,
            bus,
            ssd1680_display_start_sequence, sizeof(ssd1680_display_start_sequence),
            0, // start up time
            ssd1680_display_stop_sequence, sizeof(ssd1680_display_stop_sequence),
            296,  // width
            128,  // height
            250,  // ram_width
            296,  // ram_height
            0,  // colstart
            0,  // rowstart
            270,  // rotation
            0x44,  // set_column_window_command
            0x45,  // set_row_window_command
            0x4e,  // set_current_column_command
            0x4f,  // set_current_row_command
            0x24,  // write_black_ram_command
            false,  // black_bits_inverted
            0x26,  // write_color_ram_command
            false,  // color_bits_inverted
            0x000000,  // highlight_color
            0x000000,  // highlight_color2
            ssd1680_display_refresh_sequence, sizeof(ssd1680_display_refresh_sequence),
            1.0,  // refresh_time
            &pin_GPIO5,  // busy_pin
            true,  // busy_state
            5.0, // seconds_per_frame
            false,  // always_toggle_chip_select
            true, // grayscale
            false, // acep
            false, // spectra6
            true,  // two_byte_sequence_length
            true); // address_little_endian
    } else {
        common_hal_epaperdisplay_epaperdisplay_construct(
            display,
            bus,
            il0373_display_start_sequence, sizeof(il0373_display_start_sequence),
            0, // start up time
            il0373_display_stop_sequence, sizeof(il0373_display_stop_sequence),
            296,  // width
            128,  // height
            160,  // ram_width
            296,  // ram_height
            0,  // colstart
            0,  // rowstart
            270,  // rotation
            NO_COMMAND,  // set_column_window_command
            NO_COMMAND,  // set_row_window_command
            NO_COMMAND,  // set_current_column_command
            NO_COMMAND,  // set_current_row_command
            0x10,  // write_black_ram_command
            false,  // black_bits_inverted
            0x13,  // write_color_ram_command
            false,  // color_bits_inverted
            0x000000,  // highlight_color
            0x000000,  // highlight_color2
            il0373_display_refresh_sequence, sizeof(il0373_display_refresh_sequence),
            1.0,  // refresh_time
            &pin_GPIO5,  // busy_pin
            false,  // busy_state
            5.0, // seconds_per_frame
            false,  // always_toggle_chip_select
            true, // grayscale
            false, // acep
            false, // spectra6
            false,  // two_byte_sequence_length
            false); // address_little_endian
    }
}

bool espressif_board_reset_pin_number(gpio_num_t pin_number) {
    // Pin 16 is speaker enable and it's pulled down on the board. We don't want
    // to pull it high because then we'll compete with the external pull down.
    // So, reset without any pulls internally.
    if (pin_number == 16) {
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(16),
            .mode = GPIO_MODE_DISABLE,
            // The pin is externally pulled down, so we don't need to pull it.
            .pull_up_en = false,
            .pull_down_en = false,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        return true;
    }
    // Pin 4 is used for voltage monitoring, so don't reset
    if (pin_number == 4) {
        return true;
    }
    return false;
}

void board_deinit(void) {
    epaperdisplay_epaperdisplay_obj_t *display = &displays[0].epaper_display;
    if (display->base.type == &epaperdisplay_epaperdisplay_type) {
        size_t i = 0;
        while (common_hal_epaperdisplay_epaperdisplay_get_busy(display)) {
            RUN_BACKGROUND_TASKS;
            i++;
        }
        ESP_LOGI(TAG, "waited %d iterations for display", i);
    } else {
        ESP_LOGI(TAG, "didn't wait for display");
    }
    common_hal_displayio_release_displays();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
