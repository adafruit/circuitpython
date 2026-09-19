// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-module/hardwarekey/HardwareKey.h"

// These accessors are port-independent. The one port-specific step -- turning a
// hardware key slot into self->key_id -- happens when the port fills in its
// per-slot HardwareKey objects at startup.

mp_int_t common_hal_hardwarekey_hardwarekey_get_key_slot(hardwarekey_hardwarekey_obj_t *self) {
    return self->key_slot;
}

hardwarekey_purpose_t common_hal_hardwarekey_hardwarekey_get_purpose(hardwarekey_hardwarekey_obj_t *self) {
    return self->purpose;
}

bool common_hal_hardwarekey_hardwarekey_get_exportable(hardwarekey_hardwarekey_obj_t *self) {
    return self->exportable;
}

mp_int_t common_hal_hardwarekey_hardwarekey_get_rsa_key_bits(hardwarekey_hardwarekey_obj_t *self) {
    return self->rsa_key_bits;
}

psa_key_id_t common_hal_hardwarekey_hardwarekey_get_key_id(hardwarekey_hardwarekey_obj_t *self) {
    return self->key_id;
}

// Portable: once ensure_algorithm() (port-specific) has produced a PSA key id
// for the DS peripheral, signing is the same PSA call as any other opaque
// RSA key. The DS peripheral hashes+pads internally when driven this way, so
// callers never see a raw RSA exponentiation primitive.
void common_hal_hardwarekey_hardwarekey_sign(hardwarekey_hardwarekey_obj_t *self,
    const uint8_t *data, size_t data_len, uint8_t *sig_out, size_t sig_out_len) {
    common_hal_hardwarekey_hardwarekey_ensure_algorithm(self,
        PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
        PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_SIGN_HASH);

    size_t sig_len = 0;
    psa_status_t status = psa_sign_message(self->key_id,
        PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_SHA_256),
        data, data_len, sig_out, sig_out_len, &sig_len);
    if (status != PSA_SUCCESS) {
        mp_raise_RuntimeError(NULL);
    }
}

// Portable: once ensure_algorithm() (port-specific) has produced a PSA key id
// committed to `alg`, decryption is the same PSA call as any other opaque RSA
// key. The DS peripheral removes the padding internally, so *output_len is the
// recovered plaintext length, not rsa_key_bits / 8.
void common_hal_hardwarekey_hardwarekey_decrypt(hardwarekey_hardwarekey_obj_t *self,
    psa_algorithm_t alg, const uint8_t *ciphertext, size_t ciphertext_len,
    uint8_t *plaintext_out, size_t plaintext_out_size, size_t *output_len) {
    common_hal_hardwarekey_hardwarekey_ensure_algorithm(self, alg, PSA_KEY_USAGE_DECRYPT);

    psa_status_t status = psa_asymmetric_decrypt(self->key_id, alg,
        ciphertext, ciphertext_len, NULL, 0, plaintext_out, plaintext_out_size, output_len);
    if (status == PSA_ERROR_NOT_SUPPORTED && PSA_ALG_IS_RSA_OAEP(alg)) {
        mp_raise_NotImplementedError(
            MP_ERROR_TEXT("OAEP requires this build to enable TLS 1.3 support"));
    }
    if (status != PSA_SUCCESS) {
        mp_raise_RuntimeError(NULL);
    }
}
