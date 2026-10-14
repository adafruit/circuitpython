// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Brian Nelson
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME       "Waveshare ESP32-S3-Touch-LCD-1.69"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO10, .sda = &pin_GPIO11}}
