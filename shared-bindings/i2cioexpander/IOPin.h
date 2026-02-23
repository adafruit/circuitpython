// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/i2cioexpander/IOPin.h"

extern const mp_obj_type_t i2cioexpander_iopin_type;

mp_negative_errno_t i2cioexpander_iopin_construct(
    i2cioexpander_iopin_obj_t *self,
    i2cioexpander_ioexpander_obj_t *expander,
    uint8_t pin_number);

void common_hal_i2cioexpander_iopin_deinit(i2cioexpander_iopin_obj_t *self);
bool common_hal_i2cioexpander_iopin_deinited(i2cioexpander_iopin_obj_t *self);

digitalinout_result_t common_hal_i2cioexpander_iopin_switch_to_input(
    i2cioexpander_iopin_obj_t *self,
    digitalio_pull_t pull);

digitalinout_result_t common_hal_i2cioexpander_iopin_switch_to_output(
    i2cioexpander_iopin_obj_t *self,
    bool value,
    digitalio_drive_mode_t drive_mode);

digitalio_direction_t common_hal_i2cioexpander_iopin_get_direction(i2cioexpander_iopin_obj_t *self);

mp_negative_errno_t common_hal_i2cioexpander_iopin_set_value(i2cioexpander_iopin_obj_t *self, bool value);
mp_negative_errno_t common_hal_i2cioexpander_iopin_get_value(i2cioexpander_iopin_obj_t *self, bool *value);

digitalinout_result_t common_hal_i2cioexpander_iopin_set_drive_mode(
    i2cioexpander_iopin_obj_t *self,
    digitalio_drive_mode_t drive_mode);

digitalio_drive_mode_t common_hal_i2cioexpander_iopin_get_drive_mode(i2cioexpander_iopin_obj_t *self);

digitalinout_result_t common_hal_i2cioexpander_iopin_set_pull(
    i2cioexpander_iopin_obj_t *self,
    digitalio_pull_t pull);

digitalio_pull_t common_hal_i2cioexpander_iopin_get_pull(i2cioexpander_iopin_obj_t *self);
