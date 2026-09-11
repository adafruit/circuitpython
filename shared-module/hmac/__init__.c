// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "mbedtls/constant_time.h"
#include "shared-module/hmac/__init__.h"

bool hmac_hash_alg_from_name(const char *name, psa_algorithm_t *hash_alg) {
    if (strcmp(name, "sha256") == 0) {
        *hash_alg = PSA_ALG_SHA_256;
    } else if (strcmp(name, "sha1") == 0) {
        *hash_alg = PSA_ALG_SHA_1;
    } else {
        return false;
    }
    return true;
}

bool common_hal_hmac_compare_digest(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len) {
    // Same shape as CPython's _tscmp: the running time depends only on len(a),
    // never on where (or whether) the two inputs first differ. The byte
    // comparison itself is mbedtls_ct_memcmp(), which is hardened (volatile
    // accesses, no early-exit branch) against being optimized into a
    // variable-time comparison.
    const uint8_t *right = b;
    int mismatch = 0;

    if (a_len != b_len) {
        // Compare a against itself so the call still does a_len bytes of work,
        // then force a mismatch.
        right = a;
        mismatch = 1;
    }

    mismatch |= mbedtls_ct_memcmp(a, right, a_len);

    return mismatch == 0;
}
