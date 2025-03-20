// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "py/objstr.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#include "shared-bindings/os/__init__.h"

#include "lib/crypto-algorithms/sha256.h"

#include "hardware/structs/rosc.h"

#include <string.h>

// NIST Special Publication 800-90B (draft) recommends several extractors,
// including the SHA hash family and states that if the amount of entropy input
// is twice the number of bits output from them, that output can be considered
// essentially fully random.  If every RANDOM_SAFETY_MARGIN bits from
// `rosc_hw->randombit` have at least 1 bit of entropy, then this criterion is met.
//
// This works by seeding the `random_state` with plenty of random bits (SHA256
// as entropy harvesting function), then using that state it as a counter input
// (SHA256 as a CSPRNG), re-seeding at least every 256 blocks (8kB).
//
// In practice, `PractRand` doesn't detect any gross problems with the output
// random numbers on samples of 1 to 8 megabytes, no matter the setting of
// RANDOM_SAFETY_MARGIN.  (it does detect "unusual" results from time to time,
// as it will with any RNG)
#define RANDOM_SAFETY_MARGIN (4)

static BYTE random_state[SHA256_BLOCK_SIZE];
static void seed_random_bits(BYTE out[SHA256_BLOCK_SIZE]) {
    CRYAL_SHA256_CTX context;
    sha256_init(&context);
    for (int i = 0; i < 2 * RANDOM_SAFETY_MARGIN; i++) {
        for (int j = 0; j < SHA256_BLOCK_SIZE; j++) {
            out[j] = rosc_hw->randombit & 1;
            for (int k = 0; k < 8; k++) {
                out[j] = (out[j] << 1) ^ (rosc_hw->randombit & 1);
            }
        }
        sha256_update(&context, out, SHA256_BLOCK_SIZE);
    }
    sha256_final(&context, out);
}

static void get_random_bits(BYTE out[SHA256_BLOCK_SIZE]) {
    if (!random_state[0]++) {
        seed_random_bits(random_state);
    }
    CRYAL_SHA256_CTX context;
    sha256_init(&context);
    sha256_update(&context, random_state, SHA256_BLOCK_SIZE);
    sha256_final(&context, out);
}

bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length) {
#define ROSC_POWER_SAVE (1) // assume ROSC is not necessarily active all the time
    #if ROSC_POWER_SAVE
    uint32_t old_rosc_ctrl = rosc_hw->ctrl;
    rosc_hw->ctrl = (old_rosc_ctrl & ~ROSC_CTRL_ENABLE_BITS) | (ROSC_CTRL_ENABLE_VALUE_ENABLE << 12);
    #endif
    while (length) {
        size_t n = MIN(length, SHA256_BLOCK_SIZE);
        BYTE sha_buf[SHA256_BLOCK_SIZE];
        get_random_bits(sha_buf);
        memcpy(buffer, sha_buf, n);
        buffer += n;
        length -= n;
    }
    #if ROSC_POWER_SAVE
    rosc_hw->ctrl = old_rosc_ctrl;
    #endif
    return true;
}
