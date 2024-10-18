// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mphal.h"

#include "peripherals/pins.h"

void reset_all_pins(void);
// reset_pin_number takes the pin number instead of the pointer so that objects don't
// need to store a full pointer.
void reset_pin_number(uint8_t pin_port, uint8_t pin_pad);
