// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/__init__.h"
#include "shared-module/hashlib/__init__.h"

#include "psa/crypto.h"

bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm) {
    if (strcmp(algorithm, "md5") == 0) {
        self->hash_alg = PSA_ALG_MD5;
    } else if (strcmp(algorithm, "sha1") == 0) {
        self->hash_alg = PSA_ALG_SHA_1;
    } else if (strcmp(algorithm, "sha256") == 0) {
        self->hash_alg = PSA_ALG_SHA_256;
    } else {
        return false;
    }
    // PSA has to be initialized before any of it is used. OK to psa_crypto_init()
    // multiple times. On espressif, ESP-IDF has already done this during its own system
    // init and this call is a no-op.
    if (psa_crypto_init() != PSA_SUCCESS) {
        return false;
    }
    self->hash_op = psa_hash_operation_init();
    return psa_hash_setup(&self->hash_op, self->hash_alg) == PSA_SUCCESS;
}
