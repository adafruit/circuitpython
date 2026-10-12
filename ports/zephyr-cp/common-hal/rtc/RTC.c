// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <errno.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "shared/timeutils/timeutils.h"
#include "shared-bindings/rtc/RTC.h"
#include "common-hal/rtc/RTC.h"

#include "bindings/zephyr_kernel/__init__.h"

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>

// The RTC driver already initializes its device at Zephyr boot time, so there
// is nothing for the port to start up: time is simply read from and written
// to the devicetree `rtc` device.
static const struct device *const rtc_dev = DEVICE_DT_GET(DT_ALIAS(rtc));

// Zephyr stores calibration in parts per billion. CircuitPython's calibration
// unit is approximately one part per million.
#define CALIBRATION_PPM_TO_ZEPHYR (1000)

void common_hal_rtc_get_time(timeutils_struct_time_t *tm) {
    struct rtc_time zephyr_tm;
    int ret = rtc_get_time(rtc_dev, &zephyr_tm);
    if (ret == -ENODATA) {
        // The RTC has never been set (no battery-backed hardware and no
        // seeded time). Report the start of the CircuitPython epoch,
        // 2000-01-01, instead of failing; users set it with
        // rtc.RTC().datetime.
        timeutils_seconds_since_2000_to_struct_time(0, tm);
        return;
    }
    CHECK_ZEPHYR_RESULT(ret);

    tm->tm_year = zephyr_tm.tm_year + 1900;
    tm->tm_mon = zephyr_tm.tm_mon + 1;
    tm->tm_mday = zephyr_tm.tm_mday;
    tm->tm_hour = zephyr_tm.tm_hour;
    tm->tm_min = zephyr_tm.tm_min;
    tm->tm_sec = zephyr_tm.tm_sec;
    // tm_wday and tm_yday are filled in by struct_time_from_tm.
}

void common_hal_rtc_set_time(timeutils_struct_time_t *tm) {
    struct rtc_time zephyr_tm = {
        .tm_year = tm->tm_year - 1900,
        .tm_mon = tm->tm_mon - 1,
        .tm_mday = tm->tm_mday,
        .tm_hour = tm->tm_hour,
        .tm_min = tm->tm_min,
        .tm_sec = tm->tm_sec,
    };
    CHECK_ZEPHYR_RESULT(rtc_set_time(rtc_dev, &zephyr_tm));
}

int common_hal_rtc_get_calibration(void) {
    #ifdef CONFIG_RTC_CALIBRATION
    int32_t zephyr_calibration;
    int ret = rtc_get_calibration(rtc_dev, &zephyr_calibration);
    if (ret == -ENOSYS || ret == -ENOTSUP) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
    }
    CHECK_ZEPHYR_RESULT(ret);
    return zephyr_calibration / CALIBRATION_PPM_TO_ZEPHYR;
    #else
    // The Zephyr RTC calibration syscall only exists with
    // CONFIG_RTC_CALIBRATION, which is off unless a driver needs it.
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
    #endif
}

void common_hal_rtc_set_calibration(int calibration) {
    #ifdef CONFIG_RTC_CALIBRATION
    int ret = rtc_set_calibration(rtc_dev, (int32_t)calibration * CALIBRATION_PPM_TO_ZEPHYR);
    if (ret == -ENOSYS || ret == -ENOTSUP) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
    }
    CHECK_ZEPHYR_RESULT(ret);
    #else
    mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_calibration);
    #endif
}
