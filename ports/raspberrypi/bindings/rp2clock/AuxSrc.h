/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Elliot Buller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "py/obj.h"
#include "hardware/regs/clocks.h"

// Output sources
typedef enum {
    AUXSRC_PLL_SYS    = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
    AUXSRC_GPIN0      = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLKSRC_GPIN0,
    AUXSRC_GPIN1      = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLKSRC_GPIN1,
    AUXSRC_PLL_USB    = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
    AUXSRC_PLL_ROSC   = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_ROSC_CLKSRC,
    AUXSRC_PLL_XOSC   = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_XOSC_CLKSRC,
    AUXSRC_SYS        = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS,
    AUXSRC_USB        = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_USB,
    AUXSRC_ADC        = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_ADC,
    AUXSRC_RTC        = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_RTC,
    AUXSRC_REF        = CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_REF,
} rp2clock_auxsrc_t;

extern const mp_obj_type_t rp2clock_auxsrc_type;
