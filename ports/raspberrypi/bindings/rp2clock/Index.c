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

#include "py/runtime.h"
#include "py/enum.h"
#include "bindings/rp2clock/Index.h"

//| class Index:
//|     """Defines the internal clock to be driven from GPIN external pin."""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the internal clock index."""
//|     GPOUT0: object
//|     """Clock routed to GPOUT0 (GP21)"""
//|
//|     GPOUT1: object
//|     """Clock routed to GPOUT1 (GP23)"""
//|
//|     GPOUT2: object
//|     """Clock routed to GPOUT2 (GP24)"""
//|
//|     GPOUT3: object
//|     """Clock routed to GPOUT3 (GP25)"""
//|
//|     REF: object
//|     """Reference clock for watchdog and timers."""
//|
//|     SYS: object
//|     """Main system clock for processors."""
//|
//|     PERI: object
//|     """Peripheral clock: UART, SPI, etc. 12-125MHz"""
//|
//|     USB: object
//|     """USB clock: Must be 48MHz."""
//|
//|     ADC: object
//|     """ADC clock: Must be 48MHz."""
//|
//|     RTC: object
//|     """RTC clock: Nominally 46875 for 1 second ticks."""
//|
const mp_obj_type_t rp2clock_index_type;

MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, GPOUT0, INDEX_GPOUT0);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, GPOUT1, INDEX_GPOUT1);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, GPOUT2, INDEX_GPOUT2);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, GPOUT3, INDEX_GPOUT3);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, REF, INDEX_REF);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, SYS, INDEX_SYS);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, PERI, INDEX_PERI);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, USB, INDEX_USB);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, ADC, INDEX_ADC);
MAKE_ENUM_VALUE(rp2clock_index_type, rp2clock_index, RTC, INDEX_RTC);

MAKE_ENUM_MAP(rp2clock_index) {
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, GPOUT0),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, GPOUT1),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, GPOUT2),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, GPOUT3),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, REF),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, SYS),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, PERI),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, USB),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, ADC),
    MAKE_ENUM_MAP_ENTRY(rp2clock_index, RTC),
};

STATIC MP_DEFINE_CONST_DICT(rp2clock_index_locals_dict, rp2clock_index_locals_table);
MAKE_PRINTER(rp2clock, rp2clock_index);
MAKE_ENUM_TYPE(rp2clock, Index, rp2clock_index);
