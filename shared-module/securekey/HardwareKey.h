// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include "psa/crypto.h"

#define SECUREKEY_HMAC_SHA256_DIGEST_SIZE 32

// The handle is portable: it holds a PSA key id. How that id gets created --
// which hardware key store, which slot -- is the one port-specific step, done
// by common_hal_securekey_hardwarekey_construct() in each port's common-hal.
typedef struct {
    mp_obj_base_t base;
    psa_key_id_t key_id;
    mp_int_t key_slot;
    bool exportable;
} securekey_hardwarekey_obj_t;

// Implemented per-port in common-hal/securekey/HardwareKey.c. Must fail closed:
// a slot that does not name a key usable for HMAC-SHA256 is an error, never a
// silent success. Fills in key_id, key_slot and exportable on success.
void common_hal_securekey_hardwarekey_construct(securekey_hardwarekey_obj_t *self, mp_int_t key_slot);

// Implemented once in shared-module/securekey/HardwareKey.c on top of PSA.
void common_hal_securekey_hardwarekey_hmac_sha256(securekey_hardwarekey_obj_t *self,
    const uint8_t *data, size_t data_len, uint8_t *mac_out, size_t mac_out_len);
bool common_hal_securekey_hardwarekey_verify_hmac_sha256(securekey_hardwarekey_obj_t *self,
    const uint8_t *data, size_t data_len, const uint8_t *mac, size_t mac_len);
mp_int_t common_hal_securekey_hardwarekey_get_key_slot(securekey_hardwarekey_obj_t *self);
bool common_hal_securekey_hardwarekey_get_exportable(securekey_hardwarekey_obj_t *self);
