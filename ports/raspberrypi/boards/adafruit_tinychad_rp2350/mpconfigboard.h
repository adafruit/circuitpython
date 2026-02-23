// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Adafruit Tinychad RP2350"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO17)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO19)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO18)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO10)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO11)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO12)

#define DEFAULT_UART_BUS_RX (&pin_GPIO1)
#define DEFAULT_UART_BUS_TX (&pin_GPIO0)
