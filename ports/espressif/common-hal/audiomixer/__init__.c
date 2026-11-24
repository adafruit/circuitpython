// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

#include "shared-module/audiomixer/Mixer.h"

// ESP32-S3 PIE (SIMD), 8 signed 16-bit samples at a time. It gives the same result as
// mult16signed() and add16signed() in Mixer.c: EE.VMUL.S16 arithmetic-shifts each 32-bit
// product right by SAR and keeps the low 16 bits, and EE.VADDS.S16 saturates (ESP32-S3
// TRM, chapter 1 "Processor Instruction Extensions").
uint32_t audiomixer_port_mix_signed_16_bit(uint32_t *dst, const uint32_t *src, uint32_t n,
    int32_t lomul, int32_t himul, bool accumulate) {
    // A level of 1.0 is 32768, which does not fit in an int16 lane. Since
    // (x * m) >> 15 == (x * (m >> k)) >> (15 - k) when m is a multiple of 1 << k,
    // drop the common factors of two from both levels and shift by less.
    int32_t both_levels = lomul | himul;
    int32_t factors_of_two = both_levels ? __builtin_ctz(both_levels) : 15;
    int32_t lo_level = lomul >> factors_of_two;
    int32_t hi_level = himul >> factors_of_two;
    if (lo_level > INT16_MAX || hi_level > INT16_MAX) {
        // One channel at exactly 1.0 and the other at an odd level.
        return 0;
    }

    const uint32_t *source = src;
    uint32_t *destination = dst;
    uint32_t vectors = n / 4;
    // src may be unaligned: each step loads the next aligned 16 bytes and EE.SRC.Q.QUP
    // shifts the pair by SAR_BYTE, so it loads one block more than the vectors it
    // produces. When src is unaligned, that last block still holds the last source word,
    // and the load never leaves it. When src is aligned, it would be a whole block past
    // the end, so do one vector less.
    if (((uintptr_t)source & 15) == 0 && vectors > 0) {
        vectors--;
    }
    if (vectors == 0) {
        return 0;
    }
    uint32_t done = vectors * 4;

    int16_t levels[8] __attribute__((aligned(16))) = {
        lo_level, hi_level, lo_level, hi_level, lo_level, hi_level, lo_level, hi_level
    };
    int32_t shift = 15 - factors_of_two;
    // SAR is not restored afterwards. GCC sets SAR before every shift it emits.
    if (accumulate) {
        __asm__ volatile (
            "wsr.sar %[shift]\n"
            "ee.vld.128.ip q7, %[levels], 0\n"
            "ee.ld.128.usar.ip q0, %[source], 16\n"
            "1:\n"
            "ee.ld.128.usar.ip q1, %[source], 16\n"
            "ee.src.q.qup q2, q0, q1\n"
            "ee.vmul.s16 q3, q2, q7\n"
            "ee.vld.128.ip q4, %[destination], 0\n"
            "ee.vadds.s16 q4, q4, q3\n"
            "ee.vst.128.ip q4, %[destination], 16\n"
            "addi %[vectors], %[vectors], -1\n"
            "bnez %[vectors], 1b\n"
            : [source] "+r" (source), [destination] "+r" (destination), [vectors] "+r" (vectors)
            : [levels] "r" (levels), [shift] "r" (shift)
            : "memory");
    } else {
        __asm__ volatile (
            "wsr.sar %[shift]\n"
            "ee.vld.128.ip q7, %[levels], 0\n"
            "ee.ld.128.usar.ip q0, %[source], 16\n"
            "1:\n"
            "ee.ld.128.usar.ip q1, %[source], 16\n"
            "ee.src.q.qup q2, q0, q1\n"
            "ee.vmul.s16 q3, q2, q7\n"
            "ee.vst.128.ip q3, %[destination], 16\n"
            "addi %[vectors], %[vectors], -1\n"
            "bnez %[vectors], 1b\n"
            : [source] "+r" (source), [destination] "+r" (destination), [vectors] "+r" (vectors)
            : [levels] "r" (levels), [shift] "r" (shift)
            : "memory");
    }
    return done;
}
