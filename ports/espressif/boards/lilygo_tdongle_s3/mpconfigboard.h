// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "LILYGO T-Dongle S3"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define MICROPY_HW_APA102_MOSI   (&pin_GPIO40)
#define MICROPY_HW_APA102_SCK    (&pin_GPIO39)

#define DEFAULT_I2C_BUS_SCL  (&pin_GPIO44)
#define DEFAULT_I2C_BUS_SDA  (&pin_GPIO43)
