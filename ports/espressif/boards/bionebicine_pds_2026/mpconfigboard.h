// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "Bionebicine PDS 2026"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// No MICROPY_HW_NEOPIXEL - this board has no onboard addressable LED.

#define DEFAULT_UART_BUS_RX         (&pin_GPIO44)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO43)
