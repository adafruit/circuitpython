// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Challenger+ RP2350 BConnect"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO22)

#define DEFAULT_I2C_BUS_SCL (&pin_GPIO21)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO20)

#define DEFAULT_SPI_BUS_SS (&pin_GPIO17)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO16)

#define DEFAULT_UART_BUS_RX (&pin_GPIO13)
#define DEFAULT_UART_BUS_TX (&pin_GPIO12)

#define CIRCUITPY_PSRAM_CHIP_SELECT (&pin_GPIO0)
