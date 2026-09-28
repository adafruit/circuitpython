// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Waveshare ESP32-S3-Touch-AMOLED-2.41"
#define MICROPY_HW_MCU_NAME   "ESP32S3"

// I2C bus - Disabled on boot to avoid conflicts. User must manually initialize I2C.
#define CIRCUITPY_BOARD_I2C         (0)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO48, .sda = &pin_GPIO47}}

// AMOLED Display (displayio + qspibus path) - initialized in board_init()
#define CIRCUITPY_BOARD_DISPLAY      (1)
#define CIRCUITPY_LCD_CS             (&pin_GPIO9)
#define CIRCUITPY_LCD_CLK            (&pin_GPIO10)
#define CIRCUITPY_LCD_D0             (&pin_GPIO11)
#define CIRCUITPY_LCD_D1             (&pin_GPIO12)
#define CIRCUITPY_LCD_D2             (&pin_GPIO13)
#define CIRCUITPY_LCD_D3             (&pin_GPIO14)
#define CIRCUITPY_LCD_RESET          (&pin_GPIO21)
#define CIRCUITPY_LCD_POWER          (&pin_GPIO16)

// No default SPI bus — SD card uses SDIO, display uses QSPI.
#define CIRCUITPY_BOARD_SPI         (0)

// Default UART bus
#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)

// Reduce wifi.radio.tx_power due to the antenna design of this board
#define CIRCUITPY_WIFI_DEFAULT_TX_POWER   (15)
