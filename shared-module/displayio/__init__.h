// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/displayio/Group.h"
#include "shared-module/displayio/legacy/__init__.h"

// Both compositors call this in addition to rooting their own port-heap state.
void displayio_gc_collect_primary_and_board(void);
