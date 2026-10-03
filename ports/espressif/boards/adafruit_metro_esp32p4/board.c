// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "esp_ldo_regulator.h"

static esp_ldo_channel_handle_t io_ldo;

void board_init(void) {
    // The SD card and C6 control pins (GPIO39-48) require the VO4 3.3 V rail.
    const esp_ldo_channel_config_t config = {
        .chan_id = 4,
        .voltage_mv = 3300,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&config, &io_ldo));
}
