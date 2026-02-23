// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/i2cioexpander/IOExpander.h"

extern const mp_obj_type_t i2cioexpander_ioexpander_type;

void common_hal_i2cioexpander_ioexpander_construct(
    i2cioexpander_ioexpander_obj_t *self,
    mp_obj_t i2c,
    uint8_t address,
    uint8_t num_pins,
    uint16_t set_value_reg,
    uint16_t get_value_reg,
    uint16_t set_direction_reg);

void common_hal_i2cioexpander_ioexpander_deinit(i2cioexpander_ioexpander_obj_t *self);
bool common_hal_i2cioexpander_ioexpander_deinited(i2cioexpander_ioexpander_obj_t *self);

mp_negative_errno_t common_hal_i2cioexpander_ioexpander_get_input_value(i2cioexpander_ioexpander_obj_t *self, size_t *value);
// No error return because this returns a cached value.
void common_hal_i2cioexpander_ioexpander_get_output_value(i2cioexpander_ioexpander_obj_t *self, size_t *value);
mp_negative_errno_t common_hal_i2cioexpander_ioexpander_set_output_value(i2cioexpander_ioexpander_obj_t *self, size_t value);

void common_hal_i2cioexpander_ioexpander_get_output_mask(i2cioexpander_ioexpander_obj_t *self, size_t *mask);
mp_negative_errno_t common_hal_i2cioexpander_ioexpander_set_output_mask(i2cioexpander_ioexpander_obj_t *self, size_t mask);

mp_obj_t common_hal_i2cioexpander_ioexpander_get_pins(i2cioexpander_ioexpander_obj_t *self);
