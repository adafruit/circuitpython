// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/watchdog/__init__.h"

#include "shared-bindings/watchdog/WatchDogMode.h"
#include "shared-bindings/watchdog/WatchDogTimer.h"

// The watchdog device is selected by the devicetree `watchdog0` alias, the
// same selection Zephyr's own watchdog sample uses. The module is only
// enabled (CIRCUITPY_WATCHDOG=1) when the alias exists and CONFIG_WATCHDOG is
// on (see cptools/zephyr2cp.py), so DEVICE_DT_GET(DT_ALIAS(watchdog0)) in
// WatchDogTimer.c always resolves.
struct _watchdog_watchdogtimer_obj_t {
    mp_obj_base_t base;
    mp_float_t timeout;
    watchdog_watchdogmode_t mode;
    // The timeout channel installed with wdt_install_timeout(), or -1 when
    // the watchdog isn't running.
    int channel_id;
};
