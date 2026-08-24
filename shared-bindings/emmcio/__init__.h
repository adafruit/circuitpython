// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Clear module state on every VM reset. The card's rail has already been cut
// by then, so the state must not pretend to survive.
void emmcio_reset(void);
