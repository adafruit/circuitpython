// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/synthio/BlockBiquad.h"
#include "shared-bindings/util.h"

//| class FilterType:
//|     """The type of filter"""
//|
//|     LOW_PASS: FilterType
//|     """A low-pass filter"""
//|     HIGH_PASS: FilterType
//|     """A high-pass filter"""
//|     BAND_PASS: FilterType
//|     """A band-pass filter"""
//|

MAKE_ENUM_VALUE(synthio_filter_type, kind, LOW_PASS, SYNTHIO_LOW_PASS);
MAKE_ENUM_VALUE(synthio_filter_type, kind, HIGH_PASS, SYNTHIO_HIGH_PASS);
MAKE_ENUM_VALUE(synthio_filter_type, kind, BAND_PASS, SYNTHIO_BAND_PASS);

MAKE_ENUM_MAP(synthio_filter) {
    MAKE_ENUM_MAP_ENTRY(kind, LOW_PASS),
    MAKE_ENUM_MAP_ENTRY(kind, HIGH_PASS),
    MAKE_ENUM_MAP_ENTRY(kind, BAND_PASS),
};

static MP_DEFINE_CONST_DICT(synthio_filter_locals_dict, synthio_filter_locals_table);

MAKE_PRINTER(synthio, synthio_filter);

MAKE_ENUM_TYPE(synthio, FilterType, synthio_filter);

static synthio_filter_e validate_synthio_filter(mp_obj_t obj, qstr arg_name) {
    return cp_enum_value(&synthio_filter_type, obj, arg_name);
}

//| class BlockBiquad:
//|     def __init__(
//|         self,
//|         kind: FilterType,
//|         frequency: BlockInput,
//|         q_factor: BlockInput = 0.7071067811865475,
//|     ) -> None:
//|         """Construct a biquad filter object with dynamic center frequency & q factor
//|
//|         Since ``frequency`` and ``q_factor`` are `BlockInput` objects, they can
//|         be varied dynamically. Internally, this is evaluated as "direct form 1"
//|         biquad filter.
//|
//|         The internal filter state x[] and y[] is not updated when the filter
//|         coefficients change, and there is no theoretical justification for why
//|         this should result in a stable filter output. However, in practice,
//|         slowly varying the filter's characteristic frequency and sharpness
//|         appears to work as you'd expect."""

static const mp_arg_t block_biquad_properties[] = {
    { MP_QSTR_kind, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_frequency, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_Q, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL } },
};

static mp_obj_t synthio_block_biquad_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_kind, ARG_frequency, ARG_Q };

    mp_arg_val_t args[MP_ARRAY_SIZE(block_biquad_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(block_biquad_properties), block_biquad_properties, args);

    if (args[ARG_Q].u_obj == MP_OBJ_NULL) {
        args[ARG_Q].u_obj = mp_obj_new_float(MICROPY_FLOAT_CONST(0.7071067811865475));
    }

    synthio_filter_e kind = validate_synthio_filter(args[ARG_kind].u_obj, MP_QSTR_kind);
    return common_hal_synthio_block_biquad_new(kind, args[ARG_frequency].u_obj, args[ARG_Q].u_obj);
}

//|
//|     kind: FilterType
//|     """The kind of filter (read-only)"""
static mp_obj_t synthio_block_biquad_get_kind(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&synthio_filter_type, common_hal_synthio_block_biquad_get_kind(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_kind_obj, synthio_block_biquad_get_kind);

MP_PROPERTY_GETTER(synthio_block_biquad_kind_obj,
    (mp_obj_t)&synthio_block_biquad_get_kind_obj);

//|
//|     frequency: BlockInput
//|     """The central frequency (in Hz) of the filter"""
static mp_obj_t synthio_block_biquad_get_frequency(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_block_biquad_get_frequency(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_frequency_obj, synthio_block_biquad_get_frequency);

static mp_obj_t synthio_block_biquad_set_frequency(mp_obj_t self_in, mp_obj_t arg) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_block_biquad_set_frequency(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_block_biquad_set_frequency_obj, synthio_block_biquad_set_frequency);
MP_PROPERTY_GETSET(synthio_block_biquad_frequency_obj,
    (mp_obj_t)&synthio_block_biquad_get_frequency_obj,
    (mp_obj_t)&synthio_block_biquad_set_frequency_obj);


//|
//|     q_factor: BlockInput
//|     """The sharpness (q_factor) of the filter"""
//|
static mp_obj_t synthio_block_biquad_get_q_factor(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_block_biquad_get_q_factor(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_q_factor_obj, synthio_block_biquad_get_q_factor);

static mp_obj_t synthio_block_biquad_set_q_factor(mp_obj_t self_in, mp_obj_t arg) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_block_biquad_set_q_factor(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_block_biquad_set_q_factor_obj, synthio_block_biquad_set_q_factor);
MP_PROPERTY_GETSET(synthio_block_biquad_q_factor_obj,
    (mp_obj_t)&synthio_block_biquad_get_q_factor_obj,
    (mp_obj_t)&synthio_block_biquad_set_q_factor_obj);

static const mp_rom_map_elem_t synthio_block_biquad_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_kind), MP_ROM_PTR(&synthio_block_biquad_kind_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&synthio_block_biquad_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_q_factor), MP_ROM_PTR(&synthio_block_biquad_q_factor_obj) },
};
static MP_DEFINE_CONST_DICT(synthio_block_biquad_locals_dict, synthio_block_biquad_locals_dict_table);

static void block_biquad_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    properties_print_helper(print, self_in, block_biquad_properties, MP_ARRAY_SIZE(block_biquad_properties));
}

MP_DEFINE_CONST_OBJ_TYPE(
    synthio_block_biquad_type_obj,
    MP_QSTR_BlockBiquad,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, synthio_block_biquad_make_new,
    locals_dict, &synthio_block_biquad_locals_dict,
    print, block_biquad_print
    );
