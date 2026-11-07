// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/microcontroller/Pin.h"
#include "lib/cyw43-driver/src/cyw43_ll.h"

extern const mp_obj_type_t cyw43_pin_type;
const mcu_pin_obj_t *validate_obj_is_free_pin_including_cyw43(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_free_pin_or_gpio29(mp_obj_t obj, qstr arg_name);
const mcu_pin_obj_t *validate_obj_is_pin_including_cyw43(mp_obj_t obj, qstr arg_name);

// This is equivalent to the code in cyw43.h, except that the values are computed at compile time.
// A `CONST_` prefix has been added to the computation function (expressed as a macro) and the values.

#define CONST_cyw43_pm_value(pm_mode, pm2_sleep_ret_ms, li_beacon_period, li_dtim_period, li_assoc) \
    (li_assoc << 20 | /* listen interval sent to ap */ \
        li_dtim_period << 16 | \
        li_beacon_period << 12 | \
        (pm2_sleep_ret_ms / 10) << 4 | /* cyw43_ll_wifi_pm multiplies this by 10 */ \
        pm_mode /* CYW43_PM2_POWERSAVE_MODE etc */)

#define CONST_CYW43_DEFAULT_PM (CONST_CYW43_PERFORMANCE_PM)

#define CONST_CYW43_NONE_PM (CONST_cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 10, 0, 0, 0))

#define CONST_CYW43_AGGRESSIVE_PM (CONST_cyw43_pm_value(CYW43_PM1_POWERSAVE_MODE, 10, 0, 0, 0))

#define CONST_CYW43_PERFORMANCE_PM (CONST_cyw43_pm_value(CYW43_PM2_POWERSAVE_MODE, 200, 1, 1, 10))

extern uint32_t cyw43_get_power_management_value(void);
extern void cyw43_set_power_management_value(uint32_t value);
extern void bindings_cyw43_wifi_enforce_pm(void);
void cyw43_enter_deep_sleep(void);
