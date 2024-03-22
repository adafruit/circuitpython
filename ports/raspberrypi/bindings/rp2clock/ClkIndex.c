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
#include "bindings/rp2clock/ClkIndex.h"

//| class ClkIndex:
//|     """Defines the internal clock index to drive GPIN from an external pin."""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the internal clock index."""
//|
const mp_obj_type_t clkio_clkindex_type;

const clkio_clkindex_obj_t clkio_clkindex_gpout0_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_gpout1_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_gpout2_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_gpout3_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_ref_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_sys_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_peri_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_usb_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_adc_obj = {
    { &clkio_clkindex_type },
};
const clkio_clkindex_obj_t clkio_clkindex_rtc_obj = {
    { &clkio_clkindex_type },
};


STATIC const mp_rom_map_elem_t clkio_clkindex_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_GPOUT0),   MP_ROM_PTR(&clkio_clkindex_gpout0_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT1),   MP_ROM_PTR(&clkio_clkindex_gpout1_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT2),   MP_ROM_PTR(&clkio_clkindex_gpout2_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT3),   MP_ROM_PTR(&clkio_clkindex_gpout3_obj) },
    { MP_ROM_QSTR(MP_QSTR_REF),      MP_ROM_PTR(&clkio_clkindex_ref_obj) },
    { MP_ROM_QSTR(MP_QSTR_SYS),      MP_ROM_PTR(&clkio_clkindex_sys_obj) },
    { MP_ROM_QSTR(MP_QSTR_PERI),     MP_ROM_PTR(&clkio_clkindex_peri_obj) },
    { MP_ROM_QSTR(MP_QSTR_USB),      MP_ROM_PTR(&clkio_clkindex_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_ADC),      MP_ROM_PTR(&clkio_clkindex_adc_obj) },
    { MP_ROM_QSTR(MP_QSTR_RTC),      MP_ROM_PTR(&clkio_clkindex_rtc_obj) },
};
STATIC MP_DEFINE_CONST_DICT(clkio_clkindex_locals_dict, clkio_clkindex_locals_dict_table);

STATIC void clkio_clkindex_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr clk = MP_QSTR_INVALID;
    if (self_in == MP_ROM_PTR(&clkio_clkindex_gpout0_obj)) {
        clk = MP_QSTR_GPOUT0;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_gpout1_obj)) {
        clk = MP_QSTR_GPOUT1;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_gpout2_obj)) {
        clk = MP_QSTR_GPOUT2;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_gpout3_obj)) {
        clk = MP_QSTR_GPOUT3;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_ref_obj)) {
        clk = MP_QSTR_REF;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_sys_obj)) {
        clk = MP_QSTR_SYS;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_peri_obj)) {
        clk = MP_QSTR_PERI;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_usb_obj)) {
        clk = MP_QSTR_USB;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_adc_obj)) {
        clk = MP_QSTR_ADC;
    } else if (self_in == MP_ROM_PTR(&clkio_clkindex_rtc_obj)) {
        clk = MP_QSTR_RTC;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_clkio, MP_QSTR_ClkIndex, clk);
}

MP_DEFINE_CONST_OBJ_TYPE(
    clkio_clkindex_type,
    MP_QSTR_ClkIndex,
    MP_TYPE_FLAG_NONE,
    print, clkio_clkindex_print,
    locals_dict, &clkio_clkindex_locals_dict
    );

mp_obj_t clkindex_get_obj(clkio_clkindex_t type) {
    if (type == CLKINDEX_GPOUT0) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_gpout0_obj);
    } else if (type == CLKINDEX_GPOUT1) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_gpout1_obj);
    } else if (type == CLKINDEX_GPOUT2) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_gpout2_obj);
    } else if (type == CLKINDEX_GPOUT3) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_gpout3_obj);
    } else if (type == CLKINDEX_REF) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_ref_obj);
    } else if (type == CLKINDEX_SYS) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_sys_obj);
    } else if (type == CLKINDEX_PERI) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_peri_obj);
    } else if (type == CLKINDEX_USB) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_usb_obj);
    } else if (type == CLKINDEX_ADC) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_adc_obj);
    } else if (type == CLKINDEX_RTC) {
        return MP_OBJ_FROM_PTR(&clkio_clkindex_rtc_obj);
    } else {
        return MP_ROM_NONE;
    }
}

clkio_clkindex_t validate_clkindex(mp_rom_obj_t obj, qstr arg_name) {
    if (obj == MP_ROM_PTR(&clkio_clkindex_gpout0_obj)) {
        return CLKINDEX_GPOUT0;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_gpout1_obj)) {
        return CLKINDEX_GPOUT1;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_gpout2_obj)) {
        return CLKINDEX_GPOUT2;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_gpout3_obj)) {
        return CLKINDEX_GPOUT3;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_ref_obj)) {
        return CLKINDEX_REF;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_sys_obj)) {
        return CLKINDEX_SYS;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_peri_obj)) {
        return CLKINDEX_PERI;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_usb_obj)) {
        return CLKINDEX_USB;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_adc_obj)) {
        return CLKINDEX_ADC;
    } else if (obj == MP_ROM_PTR(&clkio_clkindex_rtc_obj)) {
        return CLKINDEX_RTC;
    } else if (obj == MP_ROM_NONE) {
        return CLKINDEX_NONE;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), arg_name, MP_QSTR_ClkIndex, MP_QSTR_None, mp_obj_get_type(obj)->name);
}
