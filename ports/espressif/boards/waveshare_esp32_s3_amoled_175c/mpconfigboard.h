// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Hassan Naeem
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Waveshare ESP32-S3-Touch-AMOLED-1.75C"
#define MICROPY_HW_MCU_NAME   "ESP32S3"

// Touch (CST9217), PMU (AXP2101), codecs (ES8311, ES7210) and IMU (QMI8658)
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO14, .sda = &pin_GPIO15}}

#define CIRCUITPY_BOARD_DISPLAY      (1)
#define CIRCUITPY_LCD_CS             (&pin_GPIO12)
#define CIRCUITPY_LCD_CLK            (&pin_GPIO38)
#define CIRCUITPY_LCD_D0             (&pin_GPIO4)
#define CIRCUITPY_LCD_D1             (&pin_GPIO5)
#define CIRCUITPY_LCD_D2             (&pin_GPIO6)
#define CIRCUITPY_LCD_D3             (&pin_GPIO7)
#define CIRCUITPY_LCD_RESET          (&pin_GPIO1)

#define CIRCUITPY_BOARD_SPI         (0)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)
