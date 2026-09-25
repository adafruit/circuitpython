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
