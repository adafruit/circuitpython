// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "NHB Systems JL401-S3 4MB Flash 2MB PSRAM"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO1)
#define CIRCUITPY_STATUS_LED_POWER (&pin_GPIO2)

// #define MICROPY_HW_LED_STATUS (&pin_GPIO13)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO48)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO47)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO12)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO13)

#define DEFAULT_UART_BUS_RX (&pin_GPIO18)
#define DEFAULT_UART_BUS_TX (&pin_GPIO17)

#define DOUBLE_TAP_PIN (&pin_GPIO38)

#define DEFAULT_SD_SCK (&pin_GPIO12)
#define DEFAULT_SD_MOSI (&pin_GPIO11)
#define DEFAULT_SD_MISO (&pin_GPIO13)
#define DEFAULT_SD_CS (&pin_GPIO10)
#define DEFAULT_SD_CARD_DETECT (&pin_GPIO9)
#define DEFAULT_SD_CARD_INSERTED true
