/**
 * Copyright (c) 2011-2025 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * \file
 * \brief SysCall class
 *
 * CircuitPython vendored, trimmed copy. The upstream SysCall.h pulls in
 * SdFatConfig.h (and through it Arduino.h / avr/io.h) plus PrintBasic.h. The
 * PIO SDIO card driver only needs the Sector_t typedef, nullptr handling, and
 * the SD_MAX_INIT_RATE_KHZ tuning constant, so we provide just those here and
 * leave the Arduino/Print machinery out of the build.
 */
#pragma once
#include <stddef.h>
#include <stdint.h>

#if __cplusplus < 201103
#warning nullptr defined
/** Define nullptr if not C++11 */
#define nullptr NULL
#endif  // __cplusplus < 201103
// ------------------------------------------------------------------------------
/** Type for FsBlockDevice sector */
typedef uint32_t Sector_t;
// ------------------------------------------------------------------------------
// SdCardInfo.h declares (but, in this vendored build, never defines) two error
// printing helpers that take a print_t*. Forward declare the type so those
// pointer-only declarations parse without dragging in PrintBasic / Arduino.
class print_t;
// ------------------------------------------------------------------------------
// Normally supplied by SdFatConfig.h.
#ifndef SD_MAX_INIT_RATE_KHZ
#define SD_MAX_INIT_RATE_KHZ 400
#endif  // SD_MAX_INIT_RATE_KHZ
