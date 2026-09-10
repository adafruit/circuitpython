// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/hmac/__init__.h"

extern const mp_obj_type_t hmac_hmac_type;

// Shared with __init__.c so hmac.new() can feed the initial msg argument.
mp_obj_t hmac_hmac_update(mp_obj_t self_in, mp_obj_t buf_in);

void common_hal_hmac_new(hmac_hmac_obj_t *self, const uint8_t *key, size_t key_len,
    psa_key_id_t borrowed_key_id, psa_algorithm_t hash_alg);
void common_hal_hmac_update(hmac_hmac_obj_t *self, const uint8_t *data, size_t data_len);
void common_hal_hmac_digest(hmac_hmac_obj_t *self, uint8_t *out, size_t out_len);
void common_hal_hmac_copy(hmac_hmac_obj_t *self, hmac_hmac_obj_t *dest);
size_t common_hal_hmac_get_digest_size(hmac_hmac_obj_t *self);
size_t common_hal_hmac_get_block_size(hmac_hmac_obj_t *self);
// Returns "hmac-sha256" / "hmac-sha1" for the .name property (CPython format).
const char *common_hal_hmac_get_name(hmac_hmac_obj_t *self);
