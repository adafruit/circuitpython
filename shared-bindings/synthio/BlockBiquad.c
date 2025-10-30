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

//| class FilterMode:
//|     """The type of filter"""
//|
//|     LOW_PASS: FilterMode
//|     """A low-pass filter"""
//|     HIGH_PASS: FilterMode
//|     """A high-pass filter"""
//|     BAND_PASS: FilterMode
//|     """A band-pass filter"""
//|     NOTCH: FilterMode
//|     """A notch filter"""
//|     LOW_SHELF: FilterMode
//|     """A low shelf filter"""
//|     HIGH_SHELF: FilterMode
//|     """A high shelf filter"""
//|     PEAKING_EQ: FilterMode
//|     """A peaking equalizer filter"""
//|
//|

MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, LOW_PASS, SYNTHIO_LOW_PASS);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, HIGH_PASS, SYNTHIO_HIGH_PASS);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, BAND_PASS, SYNTHIO_BAND_PASS);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, NOTCH, SYNTHIO_NOTCH);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, LOW_SHELF, SYNTHIO_LOW_SHELF);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, HIGH_SHELF, SYNTHIO_HIGH_SHELF);
MAKE_ENUM_VALUE(synthio_filter_mode_type, mode, PEAKING_EQ, SYNTHIO_PEAKING_EQ);

MAKE_ENUM_MAP(synthio_filter_mode) {
    MAKE_ENUM_MAP_ENTRY(mode, LOW_PASS),
    MAKE_ENUM_MAP_ENTRY(mode, HIGH_PASS),
    MAKE_ENUM_MAP_ENTRY(mode, BAND_PASS),
    MAKE_ENUM_MAP_ENTRY(mode, NOTCH),
    MAKE_ENUM_MAP_ENTRY(mode, LOW_SHELF),
    MAKE_ENUM_MAP_ENTRY(mode, HIGH_SHELF),
    MAKE_ENUM_MAP_ENTRY(mode, PEAKING_EQ),
};

static MP_DEFINE_CONST_DICT(synthio_filter_mode_locals_dict, synthio_filter_mode_locals_table);

MAKE_PRINTER(synthio, synthio_filter_mode);

MAKE_ENUM_TYPE(synthio, FilterMode, synthio_filter_mode);

static synthio_filter_mode validate_synthio_filter_mode(mp_obj_t obj, qstr arg_name) {
    return cp_enum_value(&synthio_filter_mode_type, obj, arg_name);
}

//| class BlockBiquad:
//|     def __init__(
//|         self,
//|         mode: FilterMode,
//|         frequency: BlockInput,
//|         Q: BlockInput = 0.7071067811865475,
//|         A: BlockInput = None,
//|     ) -> None:
//|         """Construct a biquad filter object with given settings.
//|
//|         ``frequency`` gives the center frequency or corner frequency of the filter,
//|         depending on the mode.
//|
//|         ``Q`` gives the gain or sharpness of the filter.
//|
//|         ``A`` controls the gain of peaking and shelving filters according to the
//|         formula ``A = 10^(dBgain/40)``. For other filter types it is ignored.
//|
//|         Since ``frequency`` and ``Q`` are `BlockInput` objects, they can
//|         be varied dynamically. Internally, this is evaluated as "direct form 1"
//|         biquad filter.
//|
//|         The internal filter state x[] and y[] is not updated when the filter
//|         coefficients change, and there is no theoretical justification for why
//|         this should result in a stable filter output. However, in practice,
//|         slowly varying the filter's characteristic frequency and sharpness
//|         appears to work as you'd expect."""
//|

static const mp_arg_t block_biquad_properties[] = {
    { MP_QSTR_mode, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_frequency, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_Q, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL } },
    { MP_QSTR_A, MP_ARG_OBJ, {.u_obj = MP_ROM_NONE } },
};

static mp_obj_t synthio_block_biquad_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_mode, ARG_frequency, ARG_Q };

    mp_arg_val_t args[MP_ARRAY_SIZE(block_biquad_properties)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(block_biquad_properties), block_biquad_properties, args);

    if (args[ARG_Q].u_obj == MP_OBJ_NULL) {
        args[ARG_Q].u_obj = mp_obj_new_float(MICROPY_FLOAT_CONST(0.7071067811865475));
    }

    synthio_filter_mode mode = validate_synthio_filter_mode(args[ARG_mode].u_obj, MP_QSTR_mode);
    mp_obj_t result = common_hal_synthio_block_biquad_new(mode);
    properties_construct_helper(result, block_biquad_properties + 1, args + 1, MP_ARRAY_SIZE(block_biquad_properties) - 1);
    return result;
}

//|
//|     mode: FilterMode
//|     """The mode of filter (read-only)"""
static mp_obj_t synthio_block_biquad_get_mode(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&synthio_filter_mode_type, common_hal_synthio_block_biquad_get_mode(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_mode_obj, synthio_block_biquad_get_mode);

MP_PROPERTY_GETTER(synthio_block_biquad_mode_obj,
    (mp_obj_t)&synthio_block_biquad_get_mode_obj);

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
//|     Q: BlockInput
//|     """The sharpness (Q) of the filter"""
//|
static mp_obj_t synthio_block_biquad_get_Q(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_block_biquad_get_Q(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_Q_obj, synthio_block_biquad_get_Q);

static mp_obj_t synthio_block_biquad_set_Q(mp_obj_t self_in, mp_obj_t arg) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_block_biquad_set_Q(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_block_biquad_set_Q_obj, synthio_block_biquad_set_Q);
MP_PROPERTY_GETSET(synthio_block_biquad_Q_obj,
    (mp_obj_t)&synthio_block_biquad_get_Q_obj,
    (mp_obj_t)&synthio_block_biquad_set_Q_obj);

//|
//|     A: BlockInput
//|     """The gain (A) of the filter
//|
//|     This setting only has an effect for peaking and shelving EQ filters. It is related
//|     to the filter gain according to the formula ``A = 10^(dBgain/40)``.
//|     """
//|
//|
static mp_obj_t synthio_block_biquad_get_A(mp_obj_t self_in) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_synthio_block_biquad_get_A(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(synthio_block_biquad_get_A_obj, synthio_block_biquad_get_A);

static mp_obj_t synthio_block_biquad_set_A(mp_obj_t self_in, mp_obj_t arg) {
    synthio_block_biquad_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_synthio_block_biquad_set_A(self, arg);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(synthio_block_biquad_set_A_obj, synthio_block_biquad_set_A);
MP_PROPERTY_GETSET(synthio_block_biquad_A_obj,
    (mp_obj_t)&synthio_block_biquad_get_A_obj,
    (mp_obj_t)&synthio_block_biquad_set_A_obj);

static const mp_rom_map_elem_t synthio_block_biquad_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mode), MP_ROM_PTR(&synthio_block_biquad_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&synthio_block_biquad_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_Q), MP_ROM_PTR(&synthio_block_biquad_Q_obj) },
    { MP_ROM_QSTR(MP_QSTR_A), MP_ROM_PTR(&synthio_block_biquad_A_obj) },
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
