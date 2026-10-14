// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Benjamin Shockley
// SPDX-FileCopyrightText: Copyright (c) 2026 David Glaude
//
// SPDX-License-Identifier: MIT
//
// Modified from waveshare_esp32_c6_lcd_1_47 to support waveshare_esp32_c5_lcd_1_47

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME "Waveshare ESP32-C5 LCD 1.47"
#define MICROPY_HW_MCU_NAME "ESP32-C5"

// For entering safe mode, use BOOT button
#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO28)

// Waveshare onboard NeoPixel on GPIO8
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO8)
#define MICROPY_HW_NEOPIXEL (&pin_GPIO8)
#define MICROPY_HW_NEOPIXEL_COUNT (1)

// Default SPI bus definitions (shared between LCD and TF card)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO7)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO6)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO5)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the BOOT button at start up.")

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
