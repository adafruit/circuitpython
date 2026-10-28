// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Micropython setup

#define MICROPY_HW_BOARD_NAME       "ESP32-C6-DevKitC-1-N8"
#define MICROPY_HW_MCU_NAME         "ESP32C6"

#define MICROPY_HW_NEOPIXEL         (&pin_GPIO8)

#define DEFAULT_UART_BUS_RX         (&pin_GPIO17)
#define DEFAULT_UART_BUS_TX         (&pin_GPIO16)

// Bench finding: 32 KB of IDF headroom is not enough for a softAP that also
// serves TLS -- handshakes hit MemoryError and DHCP replies fail while the
// Python heap still reports free space. 40 KB is what the C6 hub needed.
#define CIRCUITPY_ESP_RADIO_HEAP_RESERVE (40 * 1024)
