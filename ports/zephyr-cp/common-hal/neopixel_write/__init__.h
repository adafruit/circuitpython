// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Called from reset_port() to drop any buffer held across writes.
void neopixel_write_reset(void);
