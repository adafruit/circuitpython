// iobroker internal helpers shared between the SoC-agnostic core and the
// vendor/SoC implementations in src/<vendor>/<soc>/. Not part of the public
// API; see include/iobroker/iobroker.h for that.
//
// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include <iobroker/iobroker.h>

// Resolve a package pin to the SoC pad it is bonded to (defined by the core).
// IOBROKER_NO_PIN passes through unchanged so that disconnected optional
// signals stay disconnected. Returns 0, or -EINVAL when the pin is not in
// the map.
int iobroker_package_pin_soc_pad(package_pin_t pin, uint16_t *soc_pad_out);

// Resolve a SoC pad to the global GPIO number it is bonded to (port index *
// 32 + pin). Returns 0, or -EINVAL when the pad has no GPIO controller (an
// analog-only pad).
int iobroker_pad_gpio(uint16_t soc_pad, uint16_t *gpio_pad_out);

// Analog claim registry helpers, shared by the analog implementations in
// src/<vendor>/<soc>/ and src/emul/. The registry is what
// iobroker_pin_in_use() reports, so bus allocate() calls and GPIO
// allocations refuse analog-claimed pads.

// Returns true when the channel slot is currently taken by an analog
// allocation on the given device.
bool iobroker_analog_channel_in_use(const struct device *dev, uint8_t channel);

// Record an analog claim: the pad is validated (nothing else may hold it)
// and its SoC pad stored alongside the device and channel. Call after the
// implementation has resolved the pad and picked a free channel slot.
// Returns 0, or -EBUSY (pad already claimed), -EINVAL (pin has no SoC pad)
// or -ENOMEM (claim registry full).
int iobroker_analog_claim_add(package_pin_t pin, const struct device *dev,
    uint8_t channel);

// Drop the analog claim for the given device and channel. *soc_pad_out
// receives the claimed pad's SoC pad number for quiescing. Returns true when
// a claim was held.
bool iobroker_analog_claim_remove(const struct device *dev, uint8_t channel,
    uint16_t *soc_pad_out);

// Return a pad to a quiescent state (disconnected from any peripheral
// routing, input buffer and driver off, no pulls). Analog-only pads, which no
// GPIO controller covers, are left alone.
void iobroker_gpio_pad_quiesce(uint16_t soc_pad);
