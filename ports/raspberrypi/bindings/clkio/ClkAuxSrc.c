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
#include "supervisor/shared/translate/translate.h"
#include "bindings/clkio/ClkAuxSrc.h"

//| class ClkAuxSrc:
//|     """Defines the input clock for GPOUT on RP2040"""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the clock src."""
//|
const mp_obj_type_t clkio_clkauxsrc_type;

const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_sys_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_gpin0_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_gpin1_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_usb_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_rosc_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_pll_xosc_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_sys_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_usb_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_adc_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_rtc_obj = {
    { &clkio_clkauxsrc_type },
};
const clkio_clkauxsrc_obj_t clkio_clkauxsrc_ref_obj = {
    { &clkio_clkauxsrc_type },
};


STATIC const mp_rom_map_elem_t clkio_clkauxsrc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PLL_SYS),  MP_ROM_PTR(&clkio_clkauxsrc_pll_sys_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPIN0),    MP_ROM_PTR(&clkio_clkauxsrc_gpin0_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPIN1),    MP_ROM_PTR(&clkio_clkauxsrc_gpin1_obj) },
    { MP_ROM_QSTR(MP_QSTR_PLL_USB),  MP_ROM_PTR(&clkio_clkauxsrc_pll_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_PLL_ROSC), MP_ROM_PTR(&clkio_clkauxsrc_pll_rosc_obj) },
    { MP_ROM_QSTR(MP_QSTR_PLL_XOSC), MP_ROM_PTR(&clkio_clkauxsrc_pll_xosc_obj) },
    { MP_ROM_QSTR(MP_QSTR_SYS),      MP_ROM_PTR(&clkio_clkauxsrc_sys_obj) },
    { MP_ROM_QSTR(MP_QSTR_USB),      MP_ROM_PTR(&clkio_clkauxsrc_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_ADC),      MP_ROM_PTR(&clkio_clkauxsrc_adc_obj) },
    { MP_ROM_QSTR(MP_QSTR_RTC),      MP_ROM_PTR(&clkio_clkauxsrc_rtc_obj) },
    { MP_ROM_QSTR(MP_QSTR_REF),      MP_ROM_PTR(&clkio_clkauxsrc_ref_obj) },
};
STATIC MP_DEFINE_CONST_DICT(clkio_clkauxsrc_locals_dict, clkio_clkauxsrc_locals_dict_table);

STATIC void clkio_clkauxsrc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr clk = MP_QSTR_INVALID;
    if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_pll_sys_obj)) {
        clk = MP_QSTR_PLL_SYS;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_gpin0_obj)) {
        clk = MP_QSTR_GPIN0;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_gpin1_obj)) {
        clk = MP_QSTR_GPIN1;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_pll_usb_obj)) {
        clk = MP_QSTR_PLL_USB;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_pll_rosc_obj)) {
        clk = MP_QSTR_PLL_ROSC;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_pll_xosc_obj)) {
        clk = MP_QSTR_PLL_XOSC;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_sys_obj)) {
        clk = MP_QSTR_SYS;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_usb_obj)) {
        clk = MP_QSTR_USB;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_adc_obj)) {
        clk = MP_QSTR_ADC;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_rtc_obj)) {
        clk = MP_QSTR_RTC;
    }
    else if (self_in == MP_ROM_PTR(&clkio_clkauxsrc_ref_obj)) {
        clk = MP_QSTR_REF;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_clkio, MP_QSTR_ClkAuxSrc, clk);
}

MP_DEFINE_CONST_OBJ_TYPE(
    clkio_clkauxsrc_type,
    MP_QSTR_ClkAuxSrc,
    MP_TYPE_FLAG_NONE,
    print, clkio_clkauxsrc_print,
    locals_dict, &clkio_clkauxsrc_locals_dict
    );

mp_obj_t clkauxsrc_get_obj (clkio_clkauxsrc_t type)
{
    if (type == CLKAUXSRC_PLL_SYS)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_pll_sys_obj);
    else if (type == CLKAUXSRC_GPIN0)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_gpin0_obj);
    else if (type == CLKAUXSRC_GPIN1)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_gpin1_obj);
    else if (type == CLKAUXSRC_PLL_USB)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_pll_usb_obj);
    else if (type == CLKAUXSRC_PLL_ROSC)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_pll_rosc_obj);
    else if (type == CLKAUXSRC_PLL_XOSC)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_pll_xosc_obj);
    else if (type == CLKAUXSRC_SYS)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_sys_obj);
    else if (type == CLKAUXSRC_USB)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_usb_obj);
    else if (type == CLKAUXSRC_ADC)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_adc_obj);
    else if (type == CLKAUXSRC_RTC)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_rtc_obj);
    else if (type == CLKAUXSRC_REF)
        return MP_OBJ_FROM_PTR(&clkio_clkauxsrc_ref_obj);
    else
        return MP_ROM_NONE;
}
clkio_clkauxsrc_t validate_clkauxsrc(mp_rom_obj_t obj, qstr arg_name) {
    if (obj == MP_ROM_PTR(&clkio_clkauxsrc_pll_sys_obj)) {
        return CLKAUXSRC_PLL_SYS;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_gpin0_obj)) {
        return CLKAUXSRC_GPIN0;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_gpin1_obj)) {
        return CLKAUXSRC_GPIN1;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_pll_usb_obj)) {
        return CLKAUXSRC_PLL_USB;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_pll_rosc_obj)) {
        return CLKAUXSRC_PLL_ROSC;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_pll_xosc_obj)) {
        return CLKAUXSRC_PLL_XOSC;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_sys_obj)) {
        return CLKAUXSRC_SYS;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_usb_obj)) {
        return CLKAUXSRC_USB;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_adc_obj)) {
        return CLKAUXSRC_ADC;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_rtc_obj)) {
        return CLKAUXSRC_RTC;
    }
    else if (obj == MP_ROM_PTR(&clkio_clkauxsrc_ref_obj)) {
        return CLKAUXSRC_REF;
    }
    else if (obj == MP_ROM_NONE) {
        return CLKAUXSRC_NONE;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), arg_name, MP_QSTR_ClkAuxSrc, MP_QSTR_None, mp_obj_get_type(obj)->name);
}
