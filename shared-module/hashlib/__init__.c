// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/__init__.h"
#include "shared-module/hashlib/__init__.h"

#include "mbedtls/version.h"

#if MBEDTLS_VERSION_MAJOR >= 4

#include "psa/crypto.h"

bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm) {
    if (strcmp(algorithm, "sha1") == 0) {
        self->hash_alg = PSA_ALG_SHA_1;
    } else if (strcmp(algorithm, "sha256") == 0) {
        self->hash_alg = PSA_ALG_SHA_256;
    } else {
        return false;
    }
    self->hash_op = psa_hash_operation_init();
    psa_hash_setup(&self->hash_op, self->hash_alg);
    return true;
}

#else

#include "mbedtls/ssl.h"

// In mbedtls 2.x, the _ret suffix functions are the recommended API.
// In mbedtls 3.x, the _ret suffix was removed and the base names return int.
#if MBEDTLS_VERSION_MAJOR < 3
#define SHA1_STARTS mbedtls_sha1_starts_ret
#define SHA256_STARTS mbedtls_sha256_starts_ret
#else
#define SHA1_STARTS mbedtls_sha1_starts
#define SHA256_STARTS mbedtls_sha256_starts
#endif

bool common_hal_hashlib_new(hashlib_hash_obj_t *self, const char *algorithm) {
    if (strcmp(algorithm, "sha1") == 0) {
        self->hash_type = MBEDTLS_SSL_HASH_SHA1;
        mbedtls_sha1_init(&self->sha1);
        SHA1_STARTS(&self->sha1);
        return true;
    } else if (strcmp(algorithm, "sha256") == 0) {
        self->hash_type = MBEDTLS_SSL_HASH_SHA256;
        mbedtls_sha256_init(&self->sha256);
        SHA256_STARTS(&self->sha256, 0);
        return true;
    }
    return false;
}

#endif
