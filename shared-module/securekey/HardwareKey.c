// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "shared-module/securekey/HardwareKey.h"

#include "py/runtime.h"

#include "psa/crypto.h"

#define HMAC_SHA256_DIGEST_SIZE SECUREKEY_HMAC_SHA256_DIGEST_SIZE
#define SECUREKEY_ALG (PSA_ALG_HMAC(PSA_ALG_SHA_256))

// The operations here are port-independent: they act on self->key_id, which the
// port's common-hal construct() resolved from the hardware key slot. Any port
// with a PSA Crypto backend (Espressif today, a future Zephyr port, ...) reuses
// this file unchanged.

void common_hal_securekey_hardwarekey_hmac_sha256(securekey_hardwarekey_obj_t *self,
    const uint8_t *data, size_t data_len, uint8_t *mac_out, size_t mac_out_len) {
    size_t mac_len = 0;
    psa_status_t status = psa_mac_compute(self->key_id, SECUREKEY_ALG,
        data, data_len, mac_out, mac_out_len, &mac_len);
    if (status != PSA_SUCCESS || mac_len != HMAC_SHA256_DIGEST_SIZE) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("HMAC calculation failed"));
    }
}

bool common_hal_securekey_hardwarekey_verify_hmac_sha256(securekey_hardwarekey_obj_t *self,
    const uint8_t *data, size_t data_len, const uint8_t *mac, size_t mac_len) {
    if (mac_len != HMAC_SHA256_DIGEST_SIZE) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q length must be %d"), MP_QSTR_mac, HMAC_SHA256_DIGEST_SIZE);
    }
    psa_status_t status = psa_mac_verify(self->key_id, SECUREKEY_ALG,
        data, data_len, mac, mac_len);
    switch (status) {
        case PSA_SUCCESS:
            return true;
        case PSA_ERROR_INVALID_SIGNATURE:
            return false;
        default:
            mp_raise_RuntimeError(MP_ERROR_TEXT("HMAC calculation failed"));
            return false;
    }
}

mp_int_t common_hal_securekey_hardwarekey_get_key_slot(securekey_hardwarekey_obj_t *self) {
    return self->key_slot;
}

bool common_hal_securekey_hardwarekey_get_exportable(securekey_hardwarekey_obj_t *self) {
    return self->exportable;
}
