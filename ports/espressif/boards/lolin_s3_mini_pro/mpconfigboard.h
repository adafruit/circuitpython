// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 David Sullivan
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LOLIN S3 MINI PRO 4MB Flash 2MB PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO8)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO11)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO12)

#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO38)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO39)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO40)

#define DEFAULT_UART_BUS_RX (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX (&pin_GPIO43)
