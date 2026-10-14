// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

// Flag that marks an operation as done, possibly set from an interrupt. Ports may override these
// before this header is included.
#ifndef CIRCUITPY_ASYNC_FLAG_SET
typedef volatile bool circuitpy_async_flag_t;
#define CIRCUITPY_ASYNC_FLAG_INIT(flag) (*(flag) = false)
#define CIRCUITPY_ASYNC_FLAG_SET(flag) (*(flag) = true)
#define CIRCUITPY_ASYNC_FLAG_IS_SET(flag) (*(flag))
#endif
