// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 20212 Matthew McGowan for Blues Wireless Inc
//
// SPDX-License-Identifier: MIT

#include <stdio.h>

#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/RTC.h"
#include "peripherals/rtc.h"


void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    // The calendar holds only the last two digits of the year, and it stores the other fields as
    // given, even an impossible date or time.
    mp_arg_validate_int_range(tm->tm_year, 2000, 2099, MP_QSTR_tm_year);
    mp_arg_validate_int_range(tm->tm_mon, 1, 12, MP_QSTR_tm_mon);
    mp_arg_validate_int_range(tm->tm_mday, 1, timeutils_days_in_month(tm->tm_year, tm->tm_mon), MP_QSTR_tm_mday);
    mp_arg_validate_int_range(tm->tm_hour, 0, 23, MP_QSTR_tm_hour);
    mp_arg_validate_int_range(tm->tm_min, 0, 59, MP_QSTR_tm_min);
    mp_arg_validate_int_range(tm->tm_sec, 0, 59, MP_QSTR_tm_sec);
    stm32_peripherals_rtc_set_time(tm);
}

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    stm32_peripherals_rtc_get_time(tm);
}

int common_hal_rtc_get_calibration(void) {
    return 0;
}

void common_hal_rtc_set_calibration(int calibration) {
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
}
