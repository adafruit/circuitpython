// This file is part of the CircuitPython project: https://circuitpython.org
// SPDX-FileCopyrightText: Copyright (c) 2026 Djair Guilherme
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "DFRobot UNIHIKER K10"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// I2C bus - sensors (AHT20, LTR303ALS, SC7A20H)
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO48, .sda = &pin_GPIO47}}

// SPI bus - Display ILI9341
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO12, .mosi = &pin_GPIO21, .miso = &pin_GPIO41}}

// Display (ILI9341 via SPI)
#define MICROPY_HW_DISPLAY_SPI_CS       (&pin_GPIO14)
#define MICROPY_HW_DISPLAY_SPI_DC       (&pin_GPIO13)
//#define MICROPY_HW_DISPLAY_SPI_RST      (NULL)  // tied to 3.3V, not used - RST handled in board.c
#define MICROPY_HW_DISPLAY_WIDTH        (240)
#define MICROPY_HW_DISPLAY_HEIGHT       (320)

// Backlight - controlled via PMIC (not direct GPIO)
// eLCD_BLK is handled by init_board() in libunihiker_k10.a

// NeoPixel (3x WS2812)
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO46)

// Status LED
#define MICROPY_HW_LED_STATUS       (&pin_GPIO46)

// Default UART
//#define DEFAULT_UART_BUS_RX (NULL)  // No UART on default pins
//#define DEFAULT_UART_BUS_TX (NULL)  // No UART on default pins

// Camera (GC2145) - pins from who_camera.h
#define CIRCUITPY_ESPCAMERA_PWDN   (-1)
#define CIRCUITPY_ESPCAMERA_RESET  (-1)
#define CIRCUITPY_ESPCAMERA_XCLK   (7)
#define CIRCUITPY_ESPCAMERA_SIOD   (47)
#define CIRCUITPY_ESPCAMERA_SIOC   (48)
#define CIRCUITPY_ESPCAMERA_D7     (6)
#define CIRCUITPY_ESPCAMERA_D6     (15)
#define CIRCUITPY_ESPCAMERA_D5     (16)
#define CIRCUITPY_ESPCAMERA_D4     (18)
#define CIRCUITPY_ESPCAMERA_D3     (9)
#define CIRCUITPY_ESPCAMERA_D2     (11)
#define CIRCUITPY_ESPCAMERA_D1     (10)
#define CIRCUITPY_ESPCAMERA_D0     (8)
#define CIRCUITPY_ESPCAMERA_VSYNC  (4)
#define CIRCUITPY_ESPCAMERA_HREF   (5)
#define CIRCUITPY_ESPCAMERA_PCLK   (17)

// I2S Audio
#define CIRCUITPY_I2S_BCLK         (&pin_GPIO0)
#define CIRCUITPY_I2S_LRCLK        (&pin_GPIO38)
#define CIRCUITPY_I2S_DIN          (&pin_GPIO39)
#define CIRCUITPY_I2S_DOUT         (&pin_GPIO45)
#define CIRCUITPY_I2S_MCLK         (&pin_GPIO3)
