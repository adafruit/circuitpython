// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "lib/oofatfs/ff.h"

void override_fattime(DWORD time);

// Current time in nanoseconds past 1970/1/1, read from the same RTC source
// that get_fattime() uses. When there is no RTC, this is the same fixed
// fallback timestamp get_fattime() falls back to, so every filesystem kind
// stamps files with the same "current time".
uint64_t get_fattime_ns(void);
