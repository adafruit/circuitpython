// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#include <py/runtime.h>

#include "__init__.h"
#include "Text.h"

//| class Text:
//|     """Keep information about a single grid of text"""
//|
//|     def __init__(
//|         self,
//|         width: int,
//|         height: int,
//|         font: ReadableBuffer,
//|         palette: ReadableBuffer,
//|         chars: ReadableBuffer,
//|     ) -> None:
//|         """Keep internal information about a grid of text
//|         in a format suitable for fast rendering
//|         with the ``render()`` function.
//|
//|         :param int width: The width of the grid in tiles, or 1 for sprites.
//|         :param int height: The height of the grid in tiles, or 1 for sprites.
//|         :param ~circuitpython_typing.ReadableBuffer font: The font data of the characters.
//|         :param ~circuitpython_typing.ReadableBuffer palette: The color palette to be used.
//|         :param ~circuitpython_typing.ReadableBuffer chars: The contents of the character grid.
//|
//|         This class is intended for internal use in the ``stage`` library and
//|         it shouldn't be used on its own."""
//|         ...
//|
static mp_obj_t text_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 5, 5, false);

    mp_uint_t width = mp_arg_validate_int_min(mp_obj_get_int(args[0]), 0, MP_QSTR_width);
    mp_uint_t height = mp_arg_validate_int_min(mp_obj_get_int(args[1]), 0, MP_QSTR_height);

    mp_buffer_info_t font_bufinfo;
    mp_get_buffer_raise(args[2], &font_bufinfo, MP_BUFFER_READ);
    mp_arg_validate_length(font_bufinfo.len, 2048, MP_QSTR_font);

    mp_buffer_info_t palette_bufinfo;
    mp_get_buffer_raise(args[3], &palette_bufinfo, MP_BUFFER_READ);
    mp_arg_validate_length(palette_bufinfo.len, 32, MP_QSTR_palette);

    mp_buffer_info_t chars_bufinfo;
    mp_get_buffer_raise(args[4], &chars_bufinfo, MP_BUFFER_READ);
    if (chars_bufinfo.len < width * height) {
        mp_raise_ValueError(MP_ERROR_TEXT("chars buffer too small"));
    }

    text_obj_t *self = mp_obj_malloc(text_obj_t, type);
    self->width = width;
    self->height = height;
    self->x = 0;
    self->y = 0;
    self->font = font_bufinfo.buf;
    self->palette = palette_bufinfo.buf;
    self->chars = chars_bufinfo.buf;

    return MP_OBJ_FROM_PTR(self);
}

//|     def move(self, x: int, y: int) -> None:
//|         """Set the offset of the text to the specified values."""
//|         ...
//|
//|
static mp_obj_t text_move(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    text_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->x = mp_obj_get_int(x_in);
    self->y = mp_obj_get_int(y_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(text_move_obj, text_move);


static const mp_rom_map_elem_t text_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&text_move_obj) },
};
static MP_DEFINE_CONST_DICT(text_locals_dict, text_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_text,
    MP_QSTR_Text,
    MP_TYPE_FLAG_NONE,
    make_new, text_make_new,
    locals_dict, &text_locals_dict
    );
