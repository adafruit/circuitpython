// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

// The SDK expects the port to provide pico_localtime_r() and pico_mktime(); its
// weak defaults call newlib's localtime_r() and mktime(). CircuitPython has the
// same calendar arithmetic in shared/timeutils, so use that and keep newlib's
// time functions, and what they link in, out of the image.
//
// Newlib would run without a TZ setting here, so both compute UTC.

#include <time.h>

#include "pico/util/datetime.h"
#include "shared/timeutils/timeutils.h"

struct tm *pico_localtime_r(const time_t *time, struct tm *tm) {
    timeutils_struct_time_t t;
    timeutils_seconds_since_1970_to_struct_time((timeutils_timestamp_t)*time, &t);
    tm->tm_year = t.tm_year - 1900;
    tm->tm_mon = t.tm_mon - 1;
    tm->tm_mday = t.tm_mday;
    tm->tm_hour = t.tm_hour;
    tm->tm_min = t.tm_min;
    tm->tm_sec = t.tm_sec;
    tm->tm_wday = (t.tm_wday + 1) % 7;  // timeutils counts from Monday, struct tm from Sunday
    tm->tm_yday = t.tm_yday - 1;
    tm->tm_isdst = 0;
    return tm;
}

time_t pico_mktime(struct tm *tm) {
    time_t t = (time_t)timeutils_mktime_1970(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    pico_localtime_r(&t, tm);  // mktime() also normalises the fields it was given
    return t;
}
