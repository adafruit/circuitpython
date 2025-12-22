// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"

typedef struct {
    mp_obj_base_t base;
    union {
        mbedtls_sha1_context sha1;
        mbedtls_sha256_context sha256;
    };
    // Of MBEDTLS_SSL_HASH_*
    uint8_t hash_type;
} hashlib_hash_obj_t;
