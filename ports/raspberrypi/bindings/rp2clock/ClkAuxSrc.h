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

// Output sources
typedef enum _clkio_clkauxsrc_t {
    CLKAUXSRC_PLL_SYS    = 0,
    CLKAUXSRC_GPIN0      = 1,
    CLKAUXSRC_GPIN1      = 2,
    CLKAUXSRC_PLL_USB    = 3,
    CLKAUXSRC_PLL_ROSC   = 4,
    CLKAUXSRC_PLL_XOSC   = 5,
    CLKAUXSRC_SYS        = 6,
    CLKAUXSRC_USB        = 7,
    CLKAUXSRC_ADC        = 8,
    CLKAUXSRC_RTC        = 9,
    CLKAUXSRC_REF        = 10,
    CLKAUXSRC_NONE       = 11,
} clkio_clkauxsrc_t;

extern const mp_obj_type_t clkio_clkauxsrc_type;

typedef struct {
    mp_obj_base_t base;
} clkio_clkauxsrc_obj_t;

extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_sys_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_gpin0_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_gpin1_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_usb_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_rosc_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_xosc_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_sys_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_usb_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_adc_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_rtc_obj;
extern const clkio_clkauxsrc_obj_t clkio_clkauxsrc_ref_obj;

clkio_clkauxsrc_t validate_clkauxsrc(mp_rom_obj_t obj, qstr arg_name);
mp_obj_t clkauxsrc_get_obj(clkio_clkauxsrc_t type);
