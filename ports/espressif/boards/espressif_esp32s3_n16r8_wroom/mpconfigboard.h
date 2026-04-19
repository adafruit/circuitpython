// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

// board name printed on the board (sometimes wroom is missing)
#define MICROPY_HW_BOARD_NAME       "ESP32-S3-N16R8-Wroom"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// Camera SCCB/I2C
// used SIOD = CAM_SDA and SIOC for CAM_SCL
#define CAM_PIN_SIOD_SDA  (&pin_GPIO4)  // replace with actual GPIO
#define CAM_PIN_SIOC_SCL  (&pin_GPIO5)  // replace with actual GPIO

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = CAM_PIN_SIOC_SCL, .sda = CAM_PIN_SIOD_SDA}}

#define DEFAULT_I2C_BUS_SDA (&pin_GPIO4)  // your CAM_SDA pin
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO5)  // your CAM_SCL pin

// WS2812
#define MICROPY_HW_NEOPIXEL         (&pin_GPIO48)

// also the pins for the RX and TX LED
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

// Status LED
#define MICROPY_HW_LED_STATUS       (&pin_GPIO2)

#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)
