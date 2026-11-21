// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Daniel Schaefer for Framework Computer Inc
//
// SPDX-License-Identifier: MIT

#pragma once

#define MICROPY_HW_BOARD_NAME "Framework Laptop 16 Input Module"
#define MICROPY_HW_MCU_NAME "rp2040"

// I2C to the IS31FL37xx LED controller(s)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO27)
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO26)
