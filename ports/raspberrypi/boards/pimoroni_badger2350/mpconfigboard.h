// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Bernhard Bablok
//
// SPDX-License-Identifier: MIT

#define MICROPY_HW_BOARD_NAME "Pimoroni Badger 2350"
#define MICROPY_HW_MCU_NAME "rp2350a"

#define CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL (1)
#define CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE (1)

#define MICROPY_HW_LED_STATUS   (&pin_GPIO0)

#define DEFAULT_I2C_BUS_SDA (&pin_GPIO4)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO5)

#define DEFAULT_SPI_BUS_SCK (&pin_GPIO18)
#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO19)

#define CIRCUITPY_PSRAM_CHIP_SELECT (&pin_GPIO8)
