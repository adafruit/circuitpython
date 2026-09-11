// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "py/misc.h"
#include "py/obj.h"

#include "psa/crypto.h"

typedef struct {
    mp_obj_base_t base;
    // The digest algorithm the HMAC is built on, e.g. PSA_ALG_SHA_256.
    psa_algorithm_t hash_alg;
    // Key material for a bytes key: an owned copy on the GC heap. NULL when the
    // key lives in hardware (see borrowed_key_id).
    const uint8_t *key;
    size_t key_len;
    // A PSA key id borrowed from a hardwarekey.HardwareKey; the key is owned by
    // that object, not this one. 0 (PSA_KEY_ID_NULL) for a bytes key.
    psa_key_id_t borrowed_key_id;
    // Every byte passed to update(), buffered so digest() can be a one-shot
    // psa_mac_compute() and copy()/repeated digest() stay CPython-compatible.
    vstr_t msg;
} hmac_hmac_obj_t;

// Maps a CPython digest name ("sha1", "sha256") to a PSA hash algorithm.
// Returns false for an unsupported name.
bool hmac_hash_alg_from_name(const char *name, psa_algorithm_t *hash_alg);

// Constant-time equality, matching hmac.compare_digest() / CPython's _tscmp.
bool common_hal_hmac_compare_digest(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len);
