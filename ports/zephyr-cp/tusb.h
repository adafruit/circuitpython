// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

// Zephyr USB does not use TinyUSB. Shared code (shared-module/storage) asks
// tud_connected() whether the host is attached, so answer from the Zephyr USB state.

#include <stdbool.h>

#include "supervisor/usb.h"

static inline bool tud_connected(void) {
    return usb_connected();
}
