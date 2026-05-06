// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

extern digitalio_digitalinout_obj_t i2c_power_en_pin_obj;
extern const mp_obj_fun_builtin_fixed_t set_update_speed_obj;
extern const mp_obj_fun_builtin_fixed_t get_reset_state_obj;
extern const mp_obj_fun_builtin_fixed_t on_reset_pressed_obj;
