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
#include "bindings/rp2clock/Index.h"

//| class Index:
//|     """Defines the internal clock index to drive GPIN from an external pin."""
//|
//|     def __init__(self) -> None:
//|         """Enum-like class to define the internal clock index."""
//|
const mp_obj_type_t rp2clock_index_type;

const rp2clock_index_obj_t rp2clock_index_gpout0_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_gpout1_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_gpout2_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_gpout3_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_ref_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_sys_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_peri_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_usb_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_adc_obj = {
    { &rp2clock_index_type },
};
const rp2clock_index_obj_t rp2clock_index_rtc_obj = {
    { &rp2clock_index_type },
};


STATIC const mp_rom_map_elem_t rp2clock_index_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_GPOUT0),   MP_ROM_PTR(&rp2clock_index_gpout0_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT1),   MP_ROM_PTR(&rp2clock_index_gpout1_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT2),   MP_ROM_PTR(&rp2clock_index_gpout2_obj) },
    { MP_ROM_QSTR(MP_QSTR_GPOUT3),   MP_ROM_PTR(&rp2clock_index_gpout3_obj) },
    { MP_ROM_QSTR(MP_QSTR_REF),      MP_ROM_PTR(&rp2clock_index_ref_obj) },
    { MP_ROM_QSTR(MP_QSTR_SYS),      MP_ROM_PTR(&rp2clock_index_sys_obj) },
    { MP_ROM_QSTR(MP_QSTR_PERI),     MP_ROM_PTR(&rp2clock_index_peri_obj) },
    { MP_ROM_QSTR(MP_QSTR_USB),      MP_ROM_PTR(&rp2clock_index_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_ADC),      MP_ROM_PTR(&rp2clock_index_adc_obj) },
    { MP_ROM_QSTR(MP_QSTR_RTC),      MP_ROM_PTR(&rp2clock_index_rtc_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2clock_index_locals_dict, rp2clock_index_locals_dict_table);

STATIC void rp2clock_index_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr clk = MP_QSTR_INVALID;
    if (self_in == MP_ROM_PTR(&rp2clock_index_gpout0_obj)) {
        clk = MP_QSTR_GPOUT0;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_gpout1_obj)) {
        clk = MP_QSTR_GPOUT1;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_gpout2_obj)) {
        clk = MP_QSTR_GPOUT2;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_gpout3_obj)) {
        clk = MP_QSTR_GPOUT3;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_ref_obj)) {
        clk = MP_QSTR_REF;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_sys_obj)) {
        clk = MP_QSTR_SYS;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_peri_obj)) {
        clk = MP_QSTR_PERI;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_usb_obj)) {
        clk = MP_QSTR_USB;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_adc_obj)) {
        clk = MP_QSTR_ADC;
    } else if (self_in == MP_ROM_PTR(&rp2clock_index_rtc_obj)) {
        clk = MP_QSTR_RTC;
    }
    mp_printf(print, "%q.%q.%q", MP_QSTR_rp2clock, MP_QSTR_Index, clk);
}

MP_DEFINE_CONST_OBJ_TYPE(
    rp2clock_index_type,
    MP_QSTR_Index,
    MP_TYPE_FLAG_NONE,
    print, rp2clock_index_print,
    locals_dict, &rp2clock_index_locals_dict
    );

mp_obj_t index_get_obj(rp2clock_index_t type) {
    if (type == INDEX_GPOUT0) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_gpout0_obj);
    } else if (type == INDEX_GPOUT1) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_gpout1_obj);
    } else if (type == INDEX_GPOUT2) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_gpout2_obj);
    } else if (type == INDEX_GPOUT3) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_gpout3_obj);
    } else if (type == INDEX_REF) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_ref_obj);
    } else if (type == INDEX_SYS) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_sys_obj);
    } else if (type == INDEX_PERI) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_peri_obj);
    } else if (type == INDEX_USB) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_usb_obj);
    } else if (type == INDEX_ADC) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_adc_obj);
    } else if (type == INDEX_RTC) {
        return MP_OBJ_FROM_PTR(&rp2clock_index_rtc_obj);
    } else {
        return MP_ROM_NONE;
    }
}

rp2clock_index_t validate_index(mp_rom_obj_t obj, qstr arg_name) {
    if (obj == MP_ROM_PTR(&rp2clock_index_gpout0_obj)) {
        return INDEX_GPOUT0;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_gpout1_obj)) {
        return INDEX_GPOUT1;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_gpout2_obj)) {
        return INDEX_GPOUT2;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_gpout3_obj)) {
        return INDEX_GPOUT3;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_ref_obj)) {
        return INDEX_REF;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_sys_obj)) {
        return INDEX_SYS;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_peri_obj)) {
        return INDEX_PERI;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_usb_obj)) {
        return INDEX_USB;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_adc_obj)) {
        return INDEX_ADC;
    } else if (obj == MP_ROM_PTR(&rp2clock_index_rtc_obj)) {
        return INDEX_RTC;
    } else if (obj == MP_ROM_NONE) {
        return INDEX_NONE;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), arg_name, MP_QSTR_Index, MP_QSTR_None, mp_obj_get_type(obj)->name);
}
