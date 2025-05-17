// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#ifdef __arm__
#define INTEGER_REGS 10
#ifdef __ARM_FP
#define FLOATING_POINT_REGS 16
#endif
#endif

#ifdef __aarch64__
#define INTEGER_REGS 10
#ifdef __ARM_FP
#define FLOATING_POINT_REGS 8
#endif
#endif

#ifdef __riscv
#define INTEGER_REGS 12
#ifdef __riscv_vector
#define FLOATING_POINT_REGS 12
#endif
#endif

#ifndef INTEGER_REGS
#define INTEGER_REGS 0
#endif

#ifndef FLOATING_POINT_REGS
#define FLOATING_POINT_REGS 0
#endif

#define SAVED_REGISTER_COUNT (INTEGER_REGS + FLOATING_POINT_REGS)
