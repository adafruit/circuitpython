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

#ifdef HAS_RP2350_TRNG
#include "hardware/structs/trng.h"
#include "hardware/sync.h"
#endif

// NIST Special Publication 800-90B (draft) recommends several extractors,
// including the SHA hash family and states that if the amount of entropy input
// is twice the number of bits output from them, that output can be considered
// essentially fully random.
//
// This works by seeding `random_state` with entropy from hardware sources
// (SHA-256 as the conditioning function), then using that state as a counter
// input (SHA-256 as a CSPRNG), re-seeding at least every 256 blocks (8kB).
//
// On RP2350, entropy comes from both the dedicated TRNG peripheral and the
// ROSC. On RP2040, the ROSC is the only available source.
//
// In practice, `PractRand` doesn't detect any gross problems with the output
// random numbers on samples of 1 to 8 megabytes, no matter the setting of
// ROSC_SAFETY_MARGIN.  (it does detect "unusual" results from time to time,
// as it will with any RNG)

// Number of ROSC collection rounds on RP2040. Each round feeds
// SHA256_BLOCK_SIZE bytes into the hash; we do 2*N rounds so the
// raw-to-output ratio satisfies 800-90B's 2:1 minimum.
#define ROSC_SAFETY_MARGIN (4)

static BYTE random_state[SHA256_BLOCK_SIZE];

// Collect `count` bytes from the ROSC, one bit per read.
static void rosc_random_bytes(BYTE *buf, size_t count) {
    for (size_t i = 0; i < count; i++) {
        buf[i] = rosc_hw->randombit & 1;
        for (int k = 0; k < 8; k++) {
            buf[i] = (buf[i] << 1) ^ (rosc_hw->randombit & 1);
        }
    }
}

#ifdef HAS_RP2350_TRNG

// TRNG_DEBUG_CONTROL bypass bits:
//
//   bit 1  VNC_BYPASS             Von Neumann corrector
//   bit 2  TRNG_CRNGT_BYPASS      Continuous Random Number Generator Test
//   bit 3  AUTO_CORRELATE_BYPASS   Autocorrelation test
//
// We bypass Von Neumann and autocorrelation but keep CRNGT.
//
//   Von Neumann (bypassed): ~4x throughput cost for bias removal.
//     Redundant here because SHA-256 conditioning already handles
//     biased input -- that's what the 2:1 oversampling ratio is for.
//
//   Autocorrelation (bypassed): has a non-trivial false-positive rate
//     at high sampling speeds and halts the TRNG until SW reset on
//     failure. SHA-256 is not bothered by correlated input. ARM's own
//     TZ-TRNG 90B reference configuration also bypasses it (0x0A).
//
//   CRNGT (kept): compares consecutive 192-bit EHR outputs. Flags if
//     identical -- false-positive rate 2^-192, throughput cost zero.
//     This is our early warning for a stuck oscillator or a successful
//     injection lock to a fixed state.
#define TRNG_BYPASS_BITS \
    (TRNG_TRNG_DEBUG_CONTROL_VNC_BYPASS_BITS | \
    TRNG_TRNG_DEBUG_CONTROL_AUTO_CORRELATE_BYPASS_BITS)

// Collect 192 raw bits (6 x 32-bit words) from the TRNG.
// Returns false on CRNGT failure (consecutive identical EHR outputs).
//
// Holds PICO_SPINLOCK_ID_RAND (the SDK's lock for this peripheral)
// with interrupts disabled for the duration of the collection, which
// takes ~192 ROSC cycles (~24us at 8MHz).
static bool trng_collect_192(uint32_t out[6]) {
    spin_lock_t *lock = spin_lock_instance(PICO_SPINLOCK_ID_RAND);
    uint32_t save = spin_lock_blocking(lock);

    trng_hw->trng_debug_control = TRNG_BYPASS_BITS;
    // One rng_clk cycle between samples. The SDK uses 0 here, but it
    // also sets debug_control = -1u (full bypass). The behavior of
    // sample_cnt1 = 0 with health tests still active is undocumented,
    // so we use 1 to be safe.
    trng_hw->sample_cnt1 = 1;
    trng_hw->rnd_source_enable = 1;
    trng_hw->rng_icr = 0xFFFFFFFF;

    while (trng_hw->trng_busy) {
    }

    if (trng_hw->rng_isr & TRNG_RNG_ISR_CRNGT_ERR_BITS) {
        // Drain ehr_data so the hardware starts a fresh collection.
        // (Reading the last word clears the valid flag.)
        for (int i = 0; i < 6; i++) {
            (void)trng_hw->ehr_data[i];
        }
        trng_hw->rng_icr = TRNG_RNG_ISR_CRNGT_ERR_BITS;
        spin_unlock(lock, save);
        return false;
    }

    for (int i = 0; i < 6; i++) {
        out[i] = trng_hw->ehr_data[i];
    }

    // Switch the inverter chain length for the next collection, using
    // bits from the sample we just read. Only bits [1:0] matter -- they
    // select one of four chain lengths, changing the ROSC frequency.
    // This is borrowed from pico_rand's injection-locking countermeasure.
    // (The SDK uses its PRNG state here instead of raw output; either
    // works since the real defense is SHA-256 conditioning, not this.)
    trng_hw->trng_config = out[0];

    spin_unlock(lock, save);
    return true;
}

#endif // HAS_RP2350_TRNG

static void seed_random_bits(BYTE out[SHA256_BLOCK_SIZE]) {
    CRYAL_SHA256_CTX context;
    sha256_init(&context);

    #ifdef HAS_RP2350_TRNG
    // 384 bits from TRNG + 384 bits from ROSC = 768 bits into the hash,
    // giving a 3:1 ratio over the 256-bit output (800-90B wants >= 2:1).
    // Two independent sources so a failure in one doesn't zero the input.

    // TRNG: 2 x 192 bits.
    for (int i = 0; i < 2; i++) {
        uint32_t trng_buf[6] = {0};
        for (int attempt = 0; attempt < 3; attempt++) {
            if (trng_collect_192(trng_buf)) {
                break;
            }
            // CRNGT failure. If all 3 retries fail, trng_buf stays zeroed
            // and we rely entirely on the ROSC contribution below.
        }
        sha256_update(&context, (const BYTE *)trng_buf, sizeof(trng_buf));
    }

    // ROSC: 2 x 24 bytes = 384 bits.
    for (int i = 0; i < 2; i++) {
        BYTE rosc_buf[24];
        rosc_random_bytes(rosc_buf, sizeof(rosc_buf));
        sha256_update(&context, rosc_buf, sizeof(rosc_buf));
    }
    #else
    // RP2040: ROSC is the only entropy source.
    for (int i = 0; i < 2 * ROSC_SAFETY_MARGIN; i++) {
        rosc_random_bytes(out, SHA256_BLOCK_SIZE);
        sha256_update(&context, out, SHA256_BLOCK_SIZE);
    }
    #endif

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
    rosc_hw->ctrl = (old_rosc_ctrl & ~ROSC_CTRL_ENABLE_BITS)
        | (ROSC_CTRL_ENABLE_VALUE_ENABLE << ROSC_CTRL_ENABLE_LSB);
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
