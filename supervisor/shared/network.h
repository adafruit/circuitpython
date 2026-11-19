// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

// Formats the default network hostname, "cpy-<board>-<mac>", into buf.
//
// <board> is CIRCUITPY_BOARD_ID, with its start trimmed off so the result fits
// in buf_size bytes, including the terminating NUL. The start is usually the
// manufacturer, so the end is more likely to distinguish the board. <mac> is
// the six bytes of mac as lowercase hex.
//
// buf_size must be at least 18, the length of "cpy--", 12 hex digits, and the
// NUL. Always writes a NUL-terminated string to buf. Never allocates or raises.
//
// Returns the length of the string, not counting the NUL.
size_t network_format_default_hostname(char *buf, size_t buf_size, const uint8_t mac[6]);
