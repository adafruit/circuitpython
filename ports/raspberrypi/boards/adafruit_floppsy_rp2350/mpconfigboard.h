// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Adafruit Floppsy RP2350"
#define MICROPY_HW_MCU_NAME "rp2350b"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO18)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO29)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO28)

#define CIRCUITPY_BOARD_TFT_DC (&pin_GPIO5)
#define CIRCUITPY_BOARD_TFT_CS (&pin_GPIO8)
#define CIRCUITPY_BOARD_TFT_RESET (&pin_GPIO4)
#define CIRCUITPY_BOARD_TFT_BACKLIGHT (&pin_GPIO3)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO6)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO7)
#define DEFAULT_SPI_BUS_MISO NULL

// Use sdioio with the board.SD_* pins. Automatic SPI SD mounting would claim
// SPI0 before the TFT, whose separate GPIOs use the same hardware peripheral.

#define CIRCUITPY_PSRAM_CHIP_SELECT (&pin_GPIO47)
// Sustained flux DMA into PSRAM was verified at 40 MHz on the prototype.
#define CIRCUITPY_PSRAM_MAX_SCK_HZ (40000000)
