// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/hashlib/Hash.h"
#include "shared-module/hashlib/__init__.h"

#include "mbedtls/version.h"

#if MBEDTLS_VERSION_MAJOR >= 4

#include "psa/crypto.h"

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen) {
    psa_hash_update(&self->hash_op, data, datalen);
}

void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen) {
    if (datalen < common_hal_hashlib_hash_get_digest_size(self)) {
        return;
    }
    // Clone the operation so we can continue to update or get digest again.
    psa_hash_operation_t clone = PSA_HASH_OPERATION_INIT;
    psa_hash_clone(&self->hash_op, &clone);
    size_t hash_len;
    psa_hash_finish(&clone, data, datalen, &hash_len);
}

size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self) {
    return PSA_HASH_LENGTH(self->hash_alg);
}

#else

#include "mbedtls/ssl.h"

// In mbedtls 2.x, the _ret suffix functions are the recommended API.
// In mbedtls 3.x, the _ret suffix was removed and the base names return int.
#if MBEDTLS_VERSION_MAJOR < 3
#define SHA1_UPDATE mbedtls_sha1_update_ret
#define SHA1_FINISH mbedtls_sha1_finish_ret
#define SHA256_UPDATE mbedtls_sha256_update_ret
#define SHA256_FINISH mbedtls_sha256_finish_ret
#else
#define SHA1_UPDATE mbedtls_sha1_update
#define SHA1_FINISH mbedtls_sha1_finish
#define SHA256_UPDATE mbedtls_sha256_update
#define SHA256_FINISH mbedtls_sha256_finish
#endif

void common_hal_hashlib_hash_update(hashlib_hash_obj_t *self, const uint8_t *data, size_t datalen) {
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        SHA1_UPDATE(&self->sha1, data, datalen);
        return;
    } else if (self->hash_type == MBEDTLS_SSL_HASH_SHA256) {
        SHA256_UPDATE(&self->sha256, data, datalen);
        return;
    }
}

void common_hal_hashlib_hash_digest(hashlib_hash_obj_t *self, uint8_t *data, size_t datalen) {
    if (datalen < common_hal_hashlib_hash_get_digest_size(self)) {
        return;
    }
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        // We copy the sha1 state so we can continue to update if needed or get
        // the digest a second time.
        mbedtls_sha1_context copy;
        mbedtls_sha1_clone(&copy, &self->sha1);
        SHA1_FINISH(&self->sha1, data);
        mbedtls_sha1_clone(&self->sha1, &copy);
    } else if (self->hash_type == MBEDTLS_SSL_HASH_SHA256) {
        mbedtls_sha256_context copy;
        mbedtls_sha256_clone(&copy, &self->sha256);
        SHA256_FINISH(&self->sha256, data);
        mbedtls_sha256_clone(&self->sha256, &copy);
    }
}

size_t common_hal_hashlib_hash_get_digest_size(hashlib_hash_obj_t *self) {
    if (self->hash_type == MBEDTLS_SSL_HASH_SHA1) {
        return 20;
    } else if (self->hash_type == MBEDTLS_SSL_HASH_SHA256) {
        return 32;
    }
    return 0;
}

#endif
