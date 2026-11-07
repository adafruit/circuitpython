// neopixel: NeoPixel (WS2812-style) LED strip transmit on a runtime-chosen pin.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// Send a buffer of pixel bytes, already in the byte order the strip expects
// (GRB or GRBW), as a NeoPixel waveform on a package pin. The SoC
// implementation chooses the transmit hardware per call and allocates it,
// with the pin, from the iobroker module for the frame: on nRF a PWM
// instance routed to the pin, with a bit-bang fallback on the pin as a GPIO
// when every instance is busy or none can reach the pin (nRF54L PWMs drive
// only their own power domain's pads), except where the CPU's GPIO access is
// too slow to bit-bang (nRF54L P0). The pin must not be claimed in iobroker
// during the call; a caller that holds it as a GPIO releases it first and
// drives it low again afterwards, so that the strip latches the data when
// the line stays low, which the caller paces.

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <iobroker/iobroker.h>

// Bytes of pattern buffer the SoC implementation needs to send num_bytes of
// pixel data: the waveform it builds from the pixel bytes for its transmit
// hardware. The caller provides the buffer to neopixel_send(), 4-byte
// aligned, so that it decides where the memory lives (stack for small
// strips, heap for long ones). A constant expression, so that a caller can
// size a static or stack buffer with it.
#if defined(CONFIG_SOC_FAMILY_NORDIC_NRF)
// nRF PWM sequence: one 16-bit entry per bit plus two end-of-sequence entries.
#define NEOPIXEL_PATTERN_BUFFER_SIZE(num_bytes) \
    ((num_bytes) * 8 * sizeof(uint16_t) + 2 * sizeof(uint16_t))
#else
// No transmit implementation: nothing to build.
#define NEOPIXEL_PATTERN_BUFFER_SIZE(num_bytes) ((size_t)0)
#endif

// Send num_bytes of pixel data on the package pin. pattern_buffer must hold
// at least NEOPIXEL_PATTERN_BUFFER_SIZE(num_bytes) bytes. Returns 0, or a
// negative errno:
//   -ENOSYS: no transmit implementation for this SoC
//   -EINVAL: the pin is not in the package pin map, no transmit hardware
//            can drive it (on nRF: an nRF54L P0 pad), or the pattern buffer
//            is too small or misaligned
//   other negative values: the transfer could not be completed
int neopixel_send(package_pin_t pin, const uint8_t *pixels, size_t num_bytes,
    void *pattern_buffer, size_t pattern_buffer_size);
