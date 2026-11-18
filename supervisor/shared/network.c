// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>

#include "py/misc.h"
#include "supervisor/shared/network.h"

// Bytes used by everything except the board part: "cpy-", "-", 12 hex digits,
// and the NUL.
#define DEFAULT_HOSTNAME_OVERHEAD (4 + 1 + 12 + 1)

size_t network_format_default_hostname(char *buf, size_t buf_size, const uint8_t mac[6]) {
    const size_t board_id_len = strlen(CIRCUITPY_BOARD_ID);
    size_t board_len = MIN(buf_size - DEFAULT_HOSTNAME_OVERHEAD, board_id_len);
    size_t board_trim = board_id_len - board_len;
    // If trimming leaves a leading "_", drop it too, so the name doesn't start
    // with "cpy-_".
    if (CIRCUITPY_BOARD_ID[board_trim] == '_') {
        board_trim++;
    }
    int len = snprintf(buf, buf_size, "cpy-%s-%02x%02x%02x%02x%02x%02x",
        CIRCUITPY_BOARD_ID + board_trim,
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return len < 0 ? 0 : (size_t)len;
}
