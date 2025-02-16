// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Heltec Wireless Paper"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOOT_BUTTON (&pin_GPIO0)

#define CIRCUITPY_BOARD_SPI         (2)
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_GPIO3, .mosi = &pin_GPIO2, .miso = NULL}, \
        {.clock = &pin_GPIO9, .mosi = &pin_GPIO10, .miso = &pin_GPIO11}, \
}

// UART definition for UART connected to the CP210x
#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)

// Serial over UART
#define CIRCUITPY_CONSOLE_UART_RX               DEFAULT_UART_BUS_RX
#define CIRCUITPY_CONSOLE_UART_TX               DEFAULT_UART_BUS_TX
