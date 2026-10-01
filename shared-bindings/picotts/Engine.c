// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared/runtime/context_manager_helpers.h"

#include "shared-bindings/picotts/Engine.h"
#include "shared-bindings/util.h"

//| class Engine:
//|     """SVOX Pico text to speech engine.
//|
//|     Renders 16 bit signed mono samples at `sample_rate`. The voice is two files from the
//|     SVOX Pico lingware, read into memory by your code and passed in. The engine reads them
//|     in place, so they stay allocated for as long as the engine exists.
//|
//|     Example::
//|
//|         import array
//|         import picotts
//|
//|         with open("/voices/en-US_ta.bin", "rb") as f:
//|             ta = f.read()
//|         with open("/voices/en-US_lh0_sg.bin", "rb") as f:
//|             sg = f.read()
//|         engine = picotts.Engine(ta, sg)
//|         buffer = array.array("h", [0]) * 1024
//|         engine.start("Hello from Circuit Python.")
//|         while engine.speaking:
//|             n = engine.render(buffer)
//|             # play buffer[:n]
//|     """
//|
//|     def __init__(
//|         self, ta: ReadableBuffer, sg: ReadableBuffer, *, memory_size: int = 1100000
//|     ) -> None:
//|         """Create the engine.
//|
//|         :param ReadableBuffer ta: Contents of the text analysis file, such as ``en-US_ta.bin``
//|         :param ReadableBuffer sg: Contents of the signal generation file, such as
//|             ``en-US_lh0_sg.bin``
//|         :param int memory_size: Bytes of working memory for the engine. The engine keeps a
//|             fixed 1,000,000 byte block inside it; the rest is system overhead."""
//|         ...
//|
static mp_obj_t picotts_engine_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_ta, ARG_sg, ARG_memory_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ta, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sg, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_memory_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1100000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_int_t memory_size = mp_arg_validate_int_min(args[ARG_memory_size].u_int, 1, MP_QSTR_memory_size);
    mp_buffer_info_t ta;
    mp_get_buffer_raise(args[ARG_ta].u_obj, &ta, MP_BUFFER_READ);
    mp_buffer_info_t sg;
    mp_get_buffer_raise(args[ARG_sg].u_obj, &sg, MP_BUFFER_READ);

    picotts_engine_obj_t *self = mp_obj_malloc_with_finaliser(picotts_engine_obj_t, &picotts_engine_type);
    common_hal_picotts_engine_construct(self, args[ARG_ta].u_obj, ta.buf, args[ARG_sg].u_obj, sg.buf,
        (size_t)memory_size);
    return MP_OBJ_FROM_PTR(self);
}

static void check_for_deinit(picotts_engine_obj_t *self) {
    if (common_hal_picotts_engine_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Release the engine's working memory."""
//|         ...
//|
static mp_obj_t picotts_engine_deinit(mp_obj_t self_in) {
    picotts_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_picotts_engine_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picotts_engine_deinit_obj, picotts_engine_deinit);

//|     def __enter__(self) -> Engine:
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

//|     def start(self, text: str) -> None:
//|         """Start speaking ``text``. Any text not yet rendered is dropped. Pico markup such as
//|         ``<speed level="120">`` is accepted inside ``text``."""
//|         ...
//|
static mp_obj_t picotts_engine_start(mp_obj_t self_in, mp_obj_t text) {
    picotts_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (!mp_obj_is_str(text)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q, not %q"), MP_QSTR_text, MP_QSTR_str, mp_obj_get_type(text)->name);
    }
    common_hal_picotts_engine_start(self, text);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(picotts_engine_start_obj, picotts_engine_start);

//|     def stop(self) -> None:
//|         """Drop any text not yet rendered."""
//|         ...
//|
static mp_obj_t picotts_engine_stop(mp_obj_t self_in) {
    picotts_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_picotts_engine_stop(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(picotts_engine_stop_obj, picotts_engine_stop);

//|     def render(self, buffer: WriteableBuffer) -> int:
//|         """Render the next samples of the current text into ``buffer``, an
//|         ``array.array("h")`` or other buffer of 16 bit samples.
//|
//|         Returns the number of samples written. It is less than the buffer holds only when
//|         the text is finished, and 0 when there is nothing to speak."""
//|         ...
//|
static mp_obj_t picotts_engine_render(mp_obj_t self_in, mp_obj_t buffer) {
    picotts_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buffer, &bufinfo, MP_BUFFER_WRITE);
    size_t n = common_hal_picotts_engine_render(self, (int16_t *)bufinfo.buf, bufinfo.len / sizeof(int16_t));
    return MP_OBJ_NEW_SMALL_INT(n);
}
static MP_DEFINE_CONST_FUN_OBJ_2(picotts_engine_render_obj, picotts_engine_render);

//|     speaking: bool
//|     """True until `render` has returned all of the samples for the current text. (read-only)"""
//|
static mp_obj_t picotts_engine_obj_get_speaking(mp_obj_t self_in) {
    picotts_engine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_picotts_engine_get_speaking(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(picotts_engine_get_speaking_obj, picotts_engine_obj_get_speaking);

MP_PROPERTY_GETTER(picotts_engine_speaking_obj,
    (mp_obj_t)&picotts_engine_get_speaking_obj);

//|     sample_rate: int
//|     """Samples per second of the rendered audio, always 16000. (read-only)"""
//|
static mp_obj_t picotts_engine_obj_get_sample_rate(mp_obj_t self_in) {
    (void)self_in;
    return MP_OBJ_NEW_SMALL_INT(16000);
}
MP_DEFINE_CONST_FUN_OBJ_1(picotts_engine_get_sample_rate_obj, picotts_engine_obj_get_sample_rate);

MP_PROPERTY_GETTER(picotts_engine_sample_rate_obj,
    (mp_obj_t)&picotts_engine_get_sample_rate_obj);

static const mp_rom_map_elem_t picotts_engine_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&picotts_engine_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&picotts_engine_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&picotts_engine_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&picotts_engine_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&picotts_engine_render_obj) },
    { MP_ROM_QSTR(MP_QSTR_speaking), MP_ROM_PTR(&picotts_engine_speaking_obj) },
    { MP_ROM_QSTR(MP_QSTR_sample_rate), MP_ROM_PTR(&picotts_engine_sample_rate_obj) },
};
static MP_DEFINE_CONST_DICT(picotts_engine_locals_dict, picotts_engine_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picotts_engine_type,
    MP_QSTR_Engine,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picotts_engine_make_new,
    locals_dict, &picotts_engine_locals_dict
    );
