// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Pontus Oldberg, Invector Labs
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Challenger RP2040 NFC"
#define MICROPY_HW_MCU_NAME "rp2040"

// The on-board PN7150 sits on GPIO10/11 with no external pull-up
// resistors: iLabs' Arduino core relies on the RP2040's internal
// pull-ups instead (TwoWire::begin() calls gpio_pull_up() on both pins).
#define CIRCUITPY_I2C_ALLOW_INTERNAL_PULL_UP (1)

#define MICROPY_HW_LED_STATUS (&pin_GPIO24)
#define MICROPY_HW_NEOPIXEL   (&pin_GPIO14)

#define DEFAULT_UART_BUS_TX   (&pin_GPIO16)
#define DEFAULT_UART_BUS_RX   (&pin_GPIO17)
#define DEFAULT_I2C_BUS_SDA   (&pin_GPIO0)
#define DEFAULT_I2C_BUS_SCL   (&pin_GPIO1)
#define DEFAULT_SPI_BUS_SCK   (&pin_GPIO22)
#define DEFAULT_SPI_BUS_MOSI  (&pin_GPIO23)
#define DEFAULT_SPI_BUS_MISO  (&pin_GPIO20)
