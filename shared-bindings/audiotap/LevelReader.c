// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared-bindings/audiotap/LevelReader.h"
#include "shared-bindings/audiotap/Tap.h"

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"

//| class LevelReader:
//|     """Measures the peak and RMS levels of the audio that passes through a `Tap`"""
//|
//|     def __init__(self, tap: Tap) -> None:
//|         """Create a LevelReader that measures the audio passing through ``tap``.
//|
//|         The levels are measured on all of the audio that passes through the
//|         tap, whether or not it is read with `Tap.readinto`, so a tap made
//|         with a ``capacity`` of 0 can be used for levels alone.
//|
//|         A tap can have one LevelReader at a time. `deinit` it to detach it
//|         from the tap.
//|
//|         :param Tap tap: The tap whose audio is measured.
//|
//|         """
//|         ...
//|
static mp_obj_t audiotap_levelreader_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_tap, };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tap, MP_ARG_OBJ | MP_ARG_REQUIRED, {} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    audiotap_tap_obj_t *tap = MP_OBJ_TO_PTR(mp_arg_validate_type(args[ARG_tap].u_obj, &audiotap_tap_type, MP_QSTR_tap));

    audiotap_levelreader_obj_t *self = mp_obj_malloc(audiotap_levelreader_obj_t, &audiotap_levelreader_type);
    common_hal_audiotap_levelreader_construct(self, tap);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Detaches the LevelReader from its tap, which stops measuring levels."""
//|         ...
//|
static mp_obj_t audiotap_levelreader_deinit(mp_obj_t self_in) {
    audiotap_levelreader_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiotap_levelreader_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiotap_levelreader_deinit_obj, audiotap_levelreader_deinit);

static void check_for_deinit(audiotap_levelreader_obj_t *self) {
    if (common_hal_audiotap_levelreader_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> LevelReader:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     peak: float
//|     """The largest magnitude of the audio that passed through the tap since
//|     this was last read, from 0.0 to 1.0 of full scale. Reading this starts a
//|     new measurement. 0.0 when no audio passed through. The channels of
//|     stereo audio are measured together. (read-only)"""
//|
static mp_obj_t audiotap_levelreader_obj_get_peak(mp_obj_t self_in) {
    audiotap_levelreader_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiotap_levelreader_get_peak(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_levelreader_get_peak_obj, audiotap_levelreader_obj_get_peak);

MP_PROPERTY_GETTER(audiotap_levelreader_peak_obj,
    (mp_obj_t)&audiotap_levelreader_get_peak_obj);

//|     rms: float
//|     """The RMS level of the audio that passed through the tap since this was
//|     last read, from 0.0 to 1.0 of full scale. Reading this starts a new
//|     measurement, separate from `peak`'s. 0.0 when no audio passed through.
//|     The channels of stereo audio are measured together. (read-only)"""
//|
//|
static mp_obj_t audiotap_levelreader_obj_get_rms(mp_obj_t self_in) {
    audiotap_levelreader_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_audiotap_levelreader_get_rms(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(audiotap_levelreader_get_rms_obj, audiotap_levelreader_obj_get_rms);

MP_PROPERTY_GETTER(audiotap_levelreader_rms_obj,
    (mp_obj_t)&audiotap_levelreader_get_rms_obj);

static const mp_rom_map_elem_t audiotap_levelreader_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiotap_levelreader_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_peak), MP_ROM_PTR(&audiotap_levelreader_peak_obj) },
    { MP_ROM_QSTR(MP_QSTR_rms), MP_ROM_PTR(&audiotap_levelreader_rms_obj) },
};
static MP_DEFINE_CONST_DICT(audiotap_levelreader_locals_dict, audiotap_levelreader_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    audiotap_levelreader_type,
    MP_QSTR_LevelReader,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiotap_levelreader_make_new,
    locals_dict, &audiotap_levelreader_locals_dict
    );
