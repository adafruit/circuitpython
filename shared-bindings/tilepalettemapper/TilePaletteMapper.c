// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include <stdint.h>
#include <stdlib.h>
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/util.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/tilepalettemapper/TilePaletteMapper.h"

//| class TilePaletteMapper:
//|     """Remaps color indices from the source bitmap to alternate indices on a
//|     per-tile basis. This allows for altering coloring of tiles based on
//|     their tilegrid location. It also allows for using a limited color
//|     bitmap with a wider array of colors."""
//|
//|     def __init__(
//|         self, palette: displayio.Palette, input_color_count: int, width: int, height: int
//|     ) -> None:
//|         """Create a TilePaletteMApper object to store a set of color mappings for tiles.
//|
//|         :param Union[displayio.Palette, displayio.ColorConverter] pixel_shader:
//|           The palette or ColorConverter to get mapped colors from.
//|         :param int input_color_count: The number of colors in in the input bitmap.
//|         :param int width: The width of the grid in tiles.
//|         :param int height: The height of the grid in tiles."""
//|

static mp_obj_t tilepalettemapper_tilepalettemapper_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_pixel_shader, ARG_input_color_count, ARG_width, ARG_height };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pixel_shader, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_input_color_count, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_REQUIRED },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t pixel_shader = args[ARG_pixel_shader].u_obj;
    if (!mp_obj_is_type(pixel_shader, &displayio_palette_type) && !mp_obj_is_type(pixel_shader, &displayio_colorconverter_type)) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("unsupported %q type"), MP_QSTR_pixel_shader);
    }
    tilepalettemapper_tilepalettemapper_t *self = mp_obj_malloc(tilepalettemapper_tilepalettemapper_t, &tilepalettemapper_tilepalettemapper_type);
    common_hal_tilepalettemapper_tilepalettemapper_construct(self, pixel_shader, args[ARG_input_color_count].u_int, args[ARG_width].u_int, args[ARG_height].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     width: int
//|     """Width of the tile palette mapper in tiles."""
static mp_obj_t tilepalettemapper_tilepalettemapper_obj_get_width(mp_obj_t self_in) {
    tilepalettemapper_tilepalettemapper_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_tilepalettemapper_tilepalettemapper_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(tilepalettemapper_tilepalettemapper_get_width_obj, tilepalettemapper_tilepalettemapper_obj_get_width);

MP_PROPERTY_GETTER(tilepalettemapper_tilepalettemapper_width_obj,
    (mp_obj_t)&tilepalettemapper_tilepalettemapper_get_width_obj);

//|     height: int
//|     """Height of the tile palette mapper in tiles."""
static mp_obj_t tilepalettemapper_tilepalettemapper_obj_get_height(mp_obj_t self_in) {
    tilepalettemapper_tilepalettemapper_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_tilepalettemapper_tilepalettemapper_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(tilepalettemapper_tilepalettemapper_get_height_obj, tilepalettemapper_tilepalettemapper_obj_get_height);

MP_PROPERTY_GETTER(tilepalettemapper_tilepalettemapper_height_obj,
    (mp_obj_t)&tilepalettemapper_tilepalettemapper_get_height_obj);


//|     pixel_shader: Union[displayio.Palette, displayio.ColorConverter]
//|     """The palette or ColorConverter that the mapper uses."""
//|
static mp_obj_t tilepalettemapper_tilepalettemapper_obj_get_pixel_shader(mp_obj_t self_in) {
    tilepalettemapper_tilepalettemapper_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_tilepalettemapper_tilepalettemapper_get_pixel_shader(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(tilepalettemapper_tilepalettemapper_get_pixel_shader_obj, tilepalettemapper_tilepalettemapper_obj_get_pixel_shader);

MP_PROPERTY_GETTER(tilepalettemapper_tilepalettemapper_palette_obj,
    (mp_obj_t)&tilepalettemapper_tilepalettemapper_get_pixel_shader_obj);


//|     def __getitem__(self, index: Union[Tuple[int, int], int]) -> Tuple[int]:
//|         """Returns the mapping for the given index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           print(tpm[0])"""
//|         ...
//|
//|     def __setitem__(self, index: Union[Tuple[int, int], int], value: List[int]) -> None:
//|         """Sets the mapping at the given tile index. The index can either be an x,y tuple or an int equal
//|         to ``y * width + x``.
//|
//|         This allows you to::
//|
//|           tpm[0] = [1,0]
//|
//|         or::
//|
//|           tpm[0,0] = [1,0]"""
//|         ...
//|
//|
static mp_obj_t tilepalettemapper_subscr(mp_obj_t self_in, mp_obj_t index_obj, mp_obj_t value_obj) {
    tilepalettemapper_tilepalettemapper_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_type(index_obj, &mp_type_slice)) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("Slices not supported"));
    } else {
        uint16_t x = 0;
        uint16_t y = 0;
        if (mp_obj_is_small_int(index_obj)) {
            mp_int_t i = MP_OBJ_SMALL_INT_VALUE(index_obj);
            uint16_t width = common_hal_tilepalettemapper_tilepalettemapper_get_width(self);
            x = i % width;
            y = i / width;
        } else {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(index_obj, 2, &items);
            x = mp_obj_get_int(items[0]);
            y = mp_obj_get_int(items[1]);
        }
        if (x >= common_hal_tilepalettemapper_tilepalettemapper_get_width(self) ||
            y >= common_hal_tilepalettemapper_tilepalettemapper_get_height(self)) {
            mp_raise_IndexError(MP_ERROR_TEXT("Tile index out of bounds"));
        }

        if (value_obj == MP_OBJ_SENTINEL) {
            // load
            return common_hal_tilepalettemapper_tilepalettemapper_get_mapping(self, x, y);
        } else if (value_obj == mp_const_none) {
            return MP_OBJ_NULL; // op not supported
        } else {
            size_t len = 0;
            mp_obj_t *items;
            mp_obj_list_get(value_obj, &len, &items);
            mp_arg_validate_int_range(len, 0, self->input_color_count, MP_QSTR_mapping_length);
            common_hal_tilepalettemapper_tilepalettemapper_set_mapping(self, x, y, len, items);
        }
    }
    return mp_const_none;
}


static const mp_rom_map_elem_t tilepalettemapper_tilepalettemapper_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&tilepalettemapper_tilepalettemapper_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&tilepalettemapper_tilepalettemapper_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_palette), MP_ROM_PTR(&tilepalettemapper_tilepalettemapper_palette_obj) },
};
static MP_DEFINE_CONST_DICT(tilepalettemapper_tilepalettemapper_locals_dict, tilepalettemapper_tilepalettemapper_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    tilepalettemapper_tilepalettemapper_type,
    MP_QSTR_TilePaletteMapper,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, tilepalettemapper_tilepalettemapper_make_new,
    locals_dict, &tilepalettemapper_tilepalettemapper_locals_dict,
    subscr, tilepalettemapper_subscr,
    iter, mp_obj_generic_subscript_getiter
    );
