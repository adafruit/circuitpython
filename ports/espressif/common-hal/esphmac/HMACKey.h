// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t key_block;
} esphmac_hmackey_obj_t;

void common_hal_esphmac_hmackey_construct(esphmac_hmackey_obj_t *self, mp_int_t key_block);
void common_hal_esphmac_hmackey_hmac(esphmac_hmackey_obj_t *self, const uint8_t *data, size_t data_len, uint8_t *out_hmac);
mp_int_t common_hal_esphmac_hmackey_get_key_block(esphmac_hmackey_obj_t *self);
bool common_hal_esphmac_hmackey_get_read_protected(esphmac_hmackey_obj_t *self);
