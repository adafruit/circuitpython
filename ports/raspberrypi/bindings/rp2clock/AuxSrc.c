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
#include "bindings/rp2clock/AuxSrc.h"

//| class AuxSrc:
//|     """Defines the input clock for GPOUT on RP2040.
//|     Used with rp2clock.OutputPin instantiation.
//|     """
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the clock src."""
//|     PLL_SYS: object
//|     """PLL used to derive SYS clock."""
//|
//|     GPIN0: object
//|     """Input clock on GP20."""
//|
//|     GPIN1: object
//|     """Input clock on GP22."""
//|
//|     PLL_USB: object
//|     """Generates 48MHz USB reference clock."""
//|
//|     PLL_ROSC: object
//|     """Ring oscillator clock. 1.8-12MHz on boot depending on PVT."""
//|
//|     PLL_XOSC: object
//|     """External oscillator clock."""
//|
//|     SYS: object
//|     """Derived system clock."""
//|
//|     USB: object
//|     """Derived USB clock after PLL_USB divider, 48MHz."""
//|
//|     ADC: object
//|     """Current ADC selected clock, 48MHz."""
//|
//|     RTC: object
//|     """Current RTC selected clock."""
//|
//|     REF: object
//|     """Current reference clock for watchdog and timers."""
//|
const mp_obj_type_t rp2clock_auxsrc_type;

MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, PLL_SYS, AUXSRC_PLL_SYS);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, GPIN0, AUXSRC_GPIN0);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, GPIN1, AUXSRC_GPIN1);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, PLL_USB, AUXSRC_PLL_USB);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, PLL_ROSC, AUXSRC_PLL_ROSC);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, PLL_XOSC, AUXSRC_PLL_XOSC);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, SYS, AUXSRC_SYS);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, USB, AUXSRC_USB);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, ADC, AUXSRC_ADC);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, RTC, AUXSRC_RTC);
MAKE_ENUM_VALUE(rp2clock_auxsrc_type, rp2clock_auxsrc, REF, AUXSRC_REF);

MAKE_ENUM_MAP(rp2clock_auxsrc) {
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, PLL_SYS),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, GPIN0),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, GPIN1),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, PLL_USB),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, PLL_ROSC),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, PLL_XOSC),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, SYS),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, USB),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, ADC),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, RTC),
    MAKE_ENUM_MAP_ENTRY(rp2clock_auxsrc, REF),
};

STATIC MP_DEFINE_CONST_DICT(rp2clock_auxsrc_locals_dict, rp2clock_auxsrc_locals_table);
MAKE_PRINTER(rp2clock, rp2clock_auxsrc);
MAKE_ENUM_TYPE(rp2clock, AuxSrc, rp2clock_auxsrc);
