// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objtuple.h"
#include "shared-bindings/busio/I2C.h"

#define NO_REGISTER (0x100)

typedef struct {
    mp_obj_base_t base;
    busio_i2c_obj_t *i2c;
    uint8_t address;
    uint8_t num_pins;
    uint8_t set_value_reg;
    uint8_t get_value_reg;
    uint8_t set_direction_reg;
    size_t output_value;
    size_t output_mask;
    bool has_set_value;
    bool has_get_value;
    bool has_set_direction;
    mp_obj_tuple_t *pins;
} i2cioexpander_ioexpander_obj_t;

void common_hal_i2cioexpander_ioexpander_construct(
    i2cioexpander_ioexpander_obj_t *self,
    mp_obj_t i2c,
    uint8_t address,
    uint8_t num_pins,
    uint16_t set_value_reg,
    uint16_t get_value_reg,
    uint16_t set_direction_reg);
