// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Adafruit Feather M4 Express"
#define MICROPY_HW_MCU_NAME "samd51j19"

#define CIRCUITPY_MCU_FAMILY samd51

// Rev E

#define MICROPY_HW_LED_STATUS   (&pin_PA23)
#define MICROPY_HW_NEOPIXEL (&pin_PB03)

#define BOARD_HAS_CRYSTAL 1

#define DEFAULT_I2C_BUS_SCL (&pin_PA13)
#define DEFAULT_I2C_BUS_SDA (&pin_PA12)

#define DEFAULT_SPI_BUS_SCK (&pin_PA17)
#define DEFAULT_SPI_BUS_MOSI (&pin_PB23)
#define DEFAULT_SPI_BUS_MISO (&pin_PB22)

#define DEFAULT_UART_BUS_RX (&pin_PB17)
#define DEFAULT_UART_BUS_TX (&pin_PB16)

// Used for 32 kHZ crystal
#define IGNORE_PIN_PA00     1
#define IGNORE_PIN_PA01     1

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

// Not connected
#define IGNORE_PIN_PA15     1
#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PB00     1
#define IGNORE_PIN_PB04     1
#define IGNORE_PIN_PB05     1
#define IGNORE_PIN_PB06     1
#define IGNORE_PIN_PB07     1
#define IGNORE_PIN_PB30     1
#define IGNORE_PIN_PB31     1
