// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "shared/timeutils/timeutils.h"

extern mp_obj_t struct_time_from_tm(timeutils_struct_time_t *tm);
extern void struct_time_to_tm(mp_obj_t t, timeutils_struct_time_t *tm);
extern void struct_time_validate(mp_int_t year, mp_int_t mon, mp_int_t mday, mp_int_t hour,
    mp_int_t min, mp_int_t sec);

extern uint64_t common_hal_time_monotonic_ms(void);
extern uint64_t common_hal_time_monotonic_ns(void);
extern void common_hal_time_delay_ms(uint32_t);
