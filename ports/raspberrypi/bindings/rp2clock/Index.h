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
typedef enum _rp2clock_index_t {
    INDEX_GPOUT0  = clk_gpout0,
    INDEX_GPOUT1  = clk_gpout1,
    INDEX_GPOUT2  = clk_gpout2,
    INDEX_GPOUT3  = clk_gpout3,
    INDEX_REF     = clk_ref,
    INDEX_SYS     = clk_sys,
    INDEX_PERI    = clk_peri,
    INDEX_USB     = clk_usb,
    INDEX_ADC     = clk_adc,
    INDEX_RTC     = clk_rtc,
    INDEX_NONE = CLK_COUNT
} rp2clock_index_t;

extern const mp_obj_type_t rp2clock_index_type;

typedef struct {
    mp_obj_base_t base;
} rp2clock_index_obj_t;

extern const rp2clock_index_obj_t rp2clock_index_gpout0_obj;
extern const rp2clock_index_obj_t rp2clock_index_gpout1_obj;
extern const rp2clock_index_obj_t rp2clock_index_gpout2_obj;
extern const rp2clock_index_obj_t rp2clock_index_gpout3_obj;
extern const rp2clock_index_obj_t rp2clock_index_ref_obj;
extern const rp2clock_index_obj_t rp2clock_index_sys_obj;
extern const rp2clock_index_obj_t rp2clock_index_peri_obj;
extern const rp2clock_index_obj_t rp2clock_index_usb_obj;
extern const rp2clock_index_obj_t rp2clock_index_adc_obj;
extern const rp2clock_index_obj_t rp2clock_index_rtc_obj;

rp2clock_index_t validate_index(mp_rom_obj_t obj, qstr arg_name);
mp_obj_t index_get_obj(rp2clock_index_t type);
