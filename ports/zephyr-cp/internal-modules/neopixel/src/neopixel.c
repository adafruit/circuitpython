// neopixel: SoC-agnostic core.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// The transmit functions are SoC-specific and live in src/<vendor>/<soc>/;
// the -ENOSYS stubs below stand in for SoCs with no implementation.

#include <errno.h>

#include <neopixel/neopixel.h>

#if !defined(CONFIG_SOC_FAMILY_NORDIC_NRF)

int neopixel_send(package_pin_t pin, const uint8_t *pixels, size_t num_bytes,
    void *pattern_buffer, size_t pattern_buffer_size) {
    (void)pin;
    (void)pixels;
    (void)num_bytes;
    (void)pattern_buffer;
    (void)pattern_buffer_size;
    return -ENOSYS;
}

#endif // !CONFIG_SOC_FAMILY_NORDIC_NRF
