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
#include "hardware/clocks.h"

// Input sources
typedef enum _clkio_clkindex_t {
    CLKINDEX_GPOUT0  = clk_gpout0,
    CLKINDEX_GPOUT1  = clk_gpout1,
    CLKINDEX_GPOUT2  = clk_gpout2,
    CLKINDEX_GPOUT3  = clk_gpout3,
    CLKINDEX_REF     = clk_ref,
    CLKINDEX_SYS     = clk_sys,
    CLKINDEX_PERI    = clk_peri,
    CLKINDEX_USB     = clk_usb,
    CLKINDEX_ADC     = clk_adc,
    CLKINDEX_RTC     = clk_rtc,
    CLKINDEX_NONE = CLK_COUNT
} clkio_clkindex_t;

extern const mp_obj_type_t clkio_clkindex_type;

typedef struct {
    mp_obj_base_t base;
} clkio_clkindex_obj_t;

extern const clkio_clkindex_obj_t clkio_clkindex_gpout0_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_gpout1_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_gpout2_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_gpout3_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_ref_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_sys_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_peri_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_usb_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_adc_obj;
extern const clkio_clkindex_obj_t clkio_clkindex_rtc_obj;

clkio_clkindex_t validate_clkindex(mp_rom_obj_t obj, qstr arg_name);
mp_obj_t clkindex_get_obj(clkio_clkindex_t type);
