// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// The RTC device is selected by the devicetree `rtc` alias, following
// Zephyr's own RTC sample (samples/drivers/rtc). The rtc module is only
// enabled (CIRCUITPY_RTC=1) when the alias exists and CONFIG_RTC is on, so
// common-hal/rtc/RTC.c can always DEVICE_DT_GET(DT_ALIAS(rtc)).
