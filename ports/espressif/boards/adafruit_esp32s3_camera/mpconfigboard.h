// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "Adafruit Camera"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_NEOPIXEL (&pin_GPIO1)
#define MICROPY_HW_NEOPIXEL_COUNT (1)

#define DEFAULT_I2C_BUS_SDA (&pin_GPIO34)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO33)

#define DEFAULT_SPI_BUS_MOSI (&pin_GPIO35)
#define DEFAULT_SPI_BUS_SCK (&pin_GPIO36)
#define DEFAULT_SPI_BUS_MISO (&pin_GPIO37)

#define DOUBLE_TAP_PIN (&pin_GPIO42)

// Turn off presenting SD card to USB by default. Changes settings.toml default for this setting.
// Workaround for https://github.com/adafruit/circuitpython/issues/11109.
#define CIRCUITPY_SDCARD_USB (false)
