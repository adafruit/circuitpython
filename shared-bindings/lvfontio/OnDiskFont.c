// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/lvfontio/OnDiskFont.h"

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class OnDiskFont:
//|     """A font built into CircuitPython for use with LVGL.
//|
//|        There is an in-browser converter here: https://lvgl.io/tools/fontconverter
//|
//|        The format is documented here: https://github.com/lvgl/lv_font_conv/tree/master/doc"""
//|
//|     def __init__(self, file_path: str, max_glyphs: int = 100) -> None:
//|         """Create a OnDiskFont by loading an LVGL font file from the filesystem.
//|
//|         :param str file_path: The path to the font file
//|         :param int max_glyphs: Maximum number of glyphs to cache at once
//|         """
//|         ...
//|

//|     bitmap: displayio.Bitmap
//|     """Bitmap containing all font glyphs starting with ASCII and followed by unicode. This is useful for use with LVGL."""
//|
static mp_obj_t lvfontio_ondiskfont_obj_get_bitmap(mp_obj_t self_in) {
    lvfontio_ondiskfont_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_lvfontio_ondiskfont_get_bitmap(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(lvfontio_ondiskfont_get_bitmap_obj, lvfontio_ondiskfont_obj_get_bitmap);

MP_PROPERTY_GETTER(lvfontio_ondiskfont_bitmap_obj,
    (mp_obj_t)&lvfontio_ondiskfont_get_bitmap_obj);

//|     def get_bounding_box(self) -> Tuple[int, int]:
//|         """Returns the maximum bounds of all glyphs in the font in a tuple of two values: width, height."""
//|         ...
//|
//|
static mp_obj_t lvfontio_ondiskfont_obj_get_bounding_box(mp_obj_t self_in) {
    lvfontio_ondiskfont_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_lvfontio_ondiskfont_get_bounding_box(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(lvfontio_ondiskfont_get_bounding_box_obj, lvfontio_ondiskfont_obj_get_bounding_box);

static const mp_rom_map_elem_t lvfontio_ondiskfont_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bitmap), MP_ROM_PTR(&lvfontio_ondiskfont_bitmap_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_bounding_box), MP_ROM_PTR(&lvfontio_ondiskfont_get_bounding_box_obj) },
};
static MP_DEFINE_CONST_DICT(lvfontio_ondiskfont_locals_dict, lvfontio_ondiskfont_locals_dict_table);

static mp_obj_t lvfontio_ondiskfont_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_file_path, ARG_max_glyphs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file_path, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_max_glyphs, MP_ARG_INT, {.u_int = 100} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Allocate the BuiltinFont object
    lvfontio_ondiskfont_t *self = m_new_obj(lvfontio_ondiskfont_t);
    self->base.type = &lvfontio_ondiskfont_type;

    // Extract arguments
    mp_obj_t file_path_obj = args[ARG_file_path].u_obj;
    mp_uint_t max_glyphs = args[ARG_max_glyphs].u_int;

    // Get the C string from the Python string
    const char *file_path = mp_obj_str_get_str(file_path_obj);

    // Always use GC allocator for Python-created objects
    common_hal_lvfontio_ondiskfont_construct(self, file_path, max_glyphs, true);

    return MP_OBJ_FROM_PTR(self);
}

MP_DEFINE_CONST_OBJ_TYPE(
    lvfontio_ondiskfont_type,
    MP_QSTR_OnDiskFont,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, lvfontio_ondiskfont_make_new,
    locals_dict, &lvfontio_ondiskfont_locals_dict
    );
