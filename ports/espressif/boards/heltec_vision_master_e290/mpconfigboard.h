// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Heltec Vison Master E290"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOOT_BUTTON (&pin_GPIO0)

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_GPIO2, .mosi = &pin_GPIO1, .miso = NULL}, \
        {.clock = &pin_GPIO9, .mosi = &pin_GPIO10, .miso = &pin_GPIO11}, \
}

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     { \
        {.scl = &pin_GPIO38, .sda = &pin_GPIO39}, \
}

#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
