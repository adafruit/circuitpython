// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpconfig.h"
#include "py/obj.h"

#include "common-hal/zephyr_kernel/__init__.h"

void raise_zephyr_error(int err);
#define CHECK_ZEPHYR_RESULT(x) do { int res = (x); if (res < 0) raise_zephyr_error(res); } while (0)
