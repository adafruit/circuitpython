// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/cpu_regs.h"

#ifdef __arm__
.syntax unified
.thumb
.text
.align  2

@ uint cpu_get_regs_and_sp(r0=uint regs[SAVED_REGISTER_COUNT])
.global cpu_get_regs_and_sp
.thumb
.thumb_func
.type   cpu_get_regs_and_sp, %function
cpu_get_regs_and_sp:
#if __ARM_ARCH_ISA_THUMB == 2
@ store registers into given array
#ifdef __arm__
stmia   r0!, {r4-r11}
#endif
#if defined(__aarch64__) && __aarch64__ == 1
#error "aarch64 not supported"
stmia   r0!, {x19-x28}
#endif
#ifdef __ARM_FP
#ifdef __arm__
vstmia   r0!, {s16-s31}
#endif
#if defined(__aarch64__) && __aarch64__ == 1
vst1.64 {d8-d15}, [r0], #16
#endif
#endif
#endif
// Thumb 1 can only store directly from R0-R7. This is M0 and M23 mostly.
#if __ARM_ARCH_ISA_THUMB == 1
str     r4, [r0, #0]
str     r5, [r0, #4]
str     r6, [r0, #8]
str     r7, [r0, #12]
push {r1}
mov     r1, r8
str     r1, [r0, #16]
mov     r1, r9
str     r1, [r0, #20]
mov     r1, r10
str     r1, [r0, #24]
mov     r1, r11
str     r1, [r0, #28]
mov     r1, r12
str     r1, [r0, #32]
mov     r1, r13
str     r1, [r0, #36]
pop {r1}
#endif

@ return the sp
mov     r0, sp
bx      lr
#endif

#ifdef __riscv
#if __riscv_xlen == 32
.global cpu_get_regs_and_sp
.type   cpu_get_regs_and_sp, %function
cpu_get_regs_and_sp:
sw s0, 0(a0)
sw s1, 4(a0)
sw s2, 8(a0)
sw s3, 12(a0)
sw s4, 16(a0)
sw s5, 20(a0)
sw s6, 24(a0)
sw s7, 28(a0)
sw s8, 32(a0)
sw s9, 36(a0)
sw s10, 40(a0)
sw s11, 44(a0)
#ifdef __riscv_vector
sw fs0, 48(a0)
sw fs1, 52(a0)
sw fs2, 56(a0)
sw fs3, 60(a0)
sw fs4, 64(a0)
sw fs5, 68(a0)
sw fs6, 72(a0)
sw fs7, 76(a0)
sw fs8, 80(a0)
sw fs9, 84(a0)
sw fs10, 88(a0)
sw fs11, 92(a0)
#endif
move a0, sp
ret
#else
#error "Unsupported RISC-V bit length"
#endif
#endif
