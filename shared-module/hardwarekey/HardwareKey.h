// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"

#include "psa/crypto.h"

// What a hardware key slot is provisioned for. UNUSED means no key has been
// burned into the slot (or it is burned for something this module does not
// expose); the slot is present in `board` but not usable.
typedef enum {
    HARDWAREKEY_PURPOSE_UNUSED = 0,
    HARDWAREKEY_PURPOSE_HMAC,
    // A Digital Signature (RSA) key. Unlike HMAC, the key material isn't
    // fully in the eFuse block itself -- the slot only holds the AES key
    // that decrypts an externally-supplied, already-encrypted RSA private
    // key (`ds_params`) inside the DS peripheral. So a DS-purpose slot is
    // usable for `sign()` only after `load_ds_params()` has been called;
    // key_id and rsa_key_bits are 0 until then.
    HARDWAREKEY_PURPOSE_DS,
} hardwarekey_purpose_t;

// The handle is portable: it holds a PSA key id. How that id gets created --
// which hardware key store, which slot -- is the one port-specific step, done
// when the port populates its per-slot HardwareKey objects at startup.
typedef struct {
    mp_obj_base_t base;
    psa_key_id_t key_id;
    mp_int_t key_slot;
    hardwarekey_purpose_t purpose;
    bool exportable;
    // Name this key is exposed under in `board` (e.g. MP_QSTR_EFUSE_KEY0), for
    // repr(). MP_QSTRnull if the object was not placed in `board`.
    qstr name;
    // DS purpose only: the RSA modulus size in bits, set by load_ds_params().
    // 0 for an HMAC key, or a DS key before load_ds_params() succeeds.
    mp_int_t rsa_key_bits;
    // DS purpose only: the PSA algorithm key_id was imported under, once
    // ensure_algorithm() has run for the first time since the last
    // load_ds_params(). PSA_ALG_NONE (0) until then. A single RSA key must
    // not be used under more than one algorithm for its lifetime -- reusing
    // it for both signing and decryption (or two different decrypt paddings)
    // is a real cryptographic risk, not just a PSA API nicety; see the
    // warning on psa_set_key_enrollment_algorithm() in the PSA Crypto API.
    // So this key commits to the first algorithm it's actually used with,
    // and later calls under a different algorithm are refused.
    psa_algorithm_t committed_alg;
} hardwarekey_hardwarekey_obj_t;

// HardwareKey objects are created by the port at startup, one per hardware key
// slot, and placed in `board`; application code never constructs them. The
// per-port startup code fills in key_id, key_slot, purpose, exportable and name.
// An HMAC key is used by passing the object to hmac.new(); a DS key is used by
// calling load_ds_params() once, then sign()/decrypt() repeatedly (with a single
// algorithm -- see committed_alg above).

mp_int_t common_hal_hardwarekey_hardwarekey_get_key_slot(hardwarekey_hardwarekey_obj_t *self);
hardwarekey_purpose_t common_hal_hardwarekey_hardwarekey_get_purpose(hardwarekey_hardwarekey_obj_t *self);
bool common_hal_hardwarekey_hardwarekey_get_exportable(hardwarekey_hardwarekey_obj_t *self);
mp_int_t common_hal_hardwarekey_hardwarekey_get_rsa_key_bits(hardwarekey_hardwarekey_obj_t *self);
// The PSA key id, for hmac.new(). 0 if the slot is unused, or a DS slot whose
// ds_params has not been loaded yet.
psa_key_id_t common_hal_hardwarekey_hardwarekey_get_key_id(hardwarekey_hardwarekey_obj_t *self);

// DS purpose only. Port-specific: caches ds_params and computes rsa_key_bits, but
// does NOT import a PSA key yet -- see ensure_algorithm() below for why. Raises
// ValueError on a malformed blob or a non-DS-purpose key. Safe to call more than
// once: replaces the cached blob and clears any committed algorithm, so the key
// can be recommitted to a (possibly different) algorithm afterward.
void common_hal_hardwarekey_hardwarekey_load_ds_params(hardwarekey_hardwarekey_obj_t *self, const uint8_t *ds_params, size_t ds_params_len);

// DS purpose only, after load_ds_params(). Port-specific: on the first call since
// load_ds_params(), imports self->key_id under exactly `alg` and `usage` (only the
// port knows how to turn the cached ds_params into a PSA key reference). On a later
// call, either confirms `alg` matches what was already committed (no-op) or raises
// ValueError -- this key already committed to a different algorithm. Callers
// (sign(), decrypt() below) call this before their PSA operation so a HardwareKey
// is never used under two different algorithms over its lifetime.
void common_hal_hardwarekey_hardwarekey_ensure_algorithm(hardwarekey_hardwarekey_obj_t *self, psa_algorithm_t alg, psa_key_usage_t usage);

// DS purpose only, and only after load_ds_params(). Portable: psa_sign_message()
// against self->key_id. sig_out must be rsa_key_bits / 8 bytes.
void common_hal_hardwarekey_hardwarekey_sign(hardwarekey_hardwarekey_obj_t *self, const uint8_t *data, size_t data_len, uint8_t *sig_out, size_t sig_out_len);

// DS purpose only, and only after load_ds_params(). Portable: psa_asymmetric_decrypt()
// against self->key_id. plaintext_out must be at least rsa_key_bits / 8 bytes; the
// actual plaintext length (after padding removal) is returned via *output_len.
void common_hal_hardwarekey_hardwarekey_decrypt(hardwarekey_hardwarekey_obj_t *self,
    psa_algorithm_t alg, const uint8_t *ciphertext, size_t ciphertext_len,
    uint8_t *plaintext_out, size_t plaintext_out_size, size_t *output_len);
