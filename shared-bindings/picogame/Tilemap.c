// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-module/picogame/Tilemap.h"

//| class Tilemap:
//|     """A grid of tile indices into a tileset Bitmap (each frame = one tile).
//|     Add it to a Scene as a background layer; setting tiles or moving the map
//|     marks only the affected area dirty."""
//|
//|     def __init__(self, tileset: Bitmap, cols: int, rows: int) -> None:
//|         """A map ``cols`` tiles wide by ``rows`` tiles tall (each cell indexes a
//|         frame of ``tileset``)."""
//|         ...
//|
static mp_obj_t picogame_tilemap_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_tileset, ARG_cols, ARG_rows };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tileset, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_cols, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_rows, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    mp_obj_t tileset_obj = mp_arg_validate_type(args[ARG_tileset].u_obj, &picogame_bitmap_type, MP_QSTR_tileset);
    // Cap cols/rows (not just floor): an unbounded cols*rows overflows the size_t map/orient
    // allocation on a 32-bit port, desyncing alloc size from the index space (same family as the
    // Bitmap width*frames guard). 1024 each -> product <= 1M, far below SIZE_MAX; oversize maps
    // MemoryError at alloc (safe).
    mp_int_t map_w = mp_arg_validate_int_range(args[ARG_cols].u_int, 1, 1024, MP_QSTR_cols);
    mp_int_t map_h = mp_arg_validate_int_range(args[ARG_rows].u_int, 1, 1024, MP_QSTR_rows);

    mp_obj_t map_obj = mp_obj_new_bytearray_of_zeros((size_t)map_w * map_h);
    mp_buffer_info_t mi;
    mp_get_buffer_raise(map_obj, &mi, MP_BUFFER_RW);

    picogame_tilemap_obj_t *self = mp_obj_malloc(picogame_tilemap_obj_t, type);
    self->tileset = MP_OBJ_TO_PTR(tileset_obj);
    self->tileset_obj = tileset_obj;
    self->map = mi.buf;
    self->map_obj = map_obj;
    self->orient = NULL;            // orientation plane allocated lazily on first flipped/rotated tile
    self->orient_obj = mp_const_none;
    self->map_w = map_w;
    self->map_h = map_h;
    self->x = 0;
    self->y = 0;
    picogame_tilemap_dirty_reset(self);
    return MP_OBJ_FROM_PTR(self);
}

//|     def tile(
//|         self,
//|         tx: int,
//|         ty: int,
//|         value: Optional[int] = None,
//|         *,
//|         flip_x: bool = False,
//|         flip_y: bool = False,
//|         transpose: bool = False,
//|     ) -> Optional[int]:
//|         """Get the tile at (tx, ty) -> int; with ``value``, set it (and mark dirty) -> None.
//|         The optional keyword ``flip_x``/``flip_y``/``transpose`` flags orient the tile - together
//|         they give all 8 orientations (4 rotations x mirror) for free at draw time; use them
//|         with a deduplicated tileset (png2picogame --dedup REMAP). Out-of-range reads as 0,
//|         ignores writes."""
//|         ...
//|
static mp_obj_t picogame_tilemap_tile(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_tx, ARG_ty, ARG_value, ARG_flip_x, ARG_flip_y, ARG_transpose };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tx, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_ty, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_value, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_flip_x, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_flip_y, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_transpose, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    picogame_tilemap_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    int tx = args[ARG_tx].u_int;
    int ty = args[ARG_ty].u_int;
    bool oob = (tx < 0 || ty < 0 || tx >= self->map_w || ty >= self->map_h);
    if (args[ARG_value].u_obj != mp_const_none) {
        if (!oob) {
            size_t off = (size_t)ty * self->map_w + tx;
            uint8_t v = mp_obj_get_int(args[ARG_value].u_obj) & 0xff;
            uint8_t o = 0;
            if (args[ARG_flip_x].u_bool) {
                o |= 1;
            }
            if (args[ARG_flip_y].u_bool) {
                o |= 2;
            }
            if (args[ARG_transpose].u_bool) {
                o |= 4;
            }
            // Allocate the orientation plane lazily - only maps that actually use flips/rotation
            // pay the RAM (1 byte/cell).
            if (o != 0 && self->orient == NULL) {
                mp_obj_t ob = mp_obj_new_bytearray_of_zeros((size_t)self->map_w * self->map_h);
                mp_buffer_info_t oi;
                mp_get_buffer_raise(ob, &oi, MP_BUFFER_RW);
                self->orient = oi.buf;
                self->orient_obj = ob;
            }
            uint8_t old_o = self->orient ? self->orient[off] : 0;
            if (self->map[off] != v || old_o != o) {
                self->map[off] = v;
                if (self->orient) {
                    self->orient[off] = o;
                }
                int tw = self->tileset ? self->tileset->width : 0;
                int th = self->tileset ? self->tileset->height : 0;
                int sx = self->x + tx * tw;
                int sy = self->y + ty * th;
                picogame_tilemap_dirty_union(self, sx, sy, sx + tw, sy + th);
            }
        }
        return mp_const_none;
    }
    if (oob) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }
    return MP_OBJ_NEW_SMALL_INT(self->map[(size_t)ty * self->map_w + tx]);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_tilemap_tile_obj, 3, picogame_tilemap_tile);

//|     def move(self, x: int, y: int) -> None:
//|         """Move the whole map to pixel (x, y)."""
//|         ...
//|
static mp_obj_t picogame_tilemap_move(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    picogame_tilemap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int nx = mp_obj_get_int(x_in), ny = mp_obj_get_int(y_in);
    if (nx == self->x && ny == self->y) {
        return mp_const_none;                        // unchanged -> avoid a full-tilemap repaint
    }
    int ox1, oy1, ox2, oy2;
    picogame_tilemap_extent(self, &ox1, &oy1, &ox2, &oy2);
    self->x = nx;
    self->y = ny;
    int nx1, ny1, nx2, ny2;
    picogame_tilemap_extent(self, &nx1, &ny1, &nx2, &ny2);
    picogame_tilemap_dirty_union(self, ox1, oy1, ox2, oy2);
    picogame_tilemap_dirty_union(self, nx1, ny1, nx2, ny2);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(picogame_tilemap_move_obj, picogame_tilemap_move);

//|     def fill(self, value: int) -> None:
//|         """Set every tile to ``value``."""
//|         ...
//|
static mp_obj_t picogame_tilemap_fill(mp_obj_t self_in, mp_obj_t value_in) {
    picogame_tilemap_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t v = mp_obj_get_int(value_in) & 0xff;
    size_t total = (size_t)self->map_w * self->map_h;
    for (size_t i = 0; i < total; i++) {
        self->map[i] = v;
        if (self->orient) {
            self->orient[i] = 0;       // a plain fill clears any per-cell orientation
        }
    }
    int x1, y1, x2, y2;
    picogame_tilemap_extent(self, &x1, &y1, &x2, &y2);
    picogame_tilemap_dirty_union(self, x1, y1, x2, y2);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(picogame_tilemap_fill_obj, picogame_tilemap_fill);

//|     x: int
//|     y: int
//|     """Current pixel position of the map's top-left (read-only; set with move())."""
//|     cols: int
//|     rows: int
//|     """Map size in tiles (read-only)."""
//|
//|
static mp_obj_t tilemap_get_x(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_tilemap_obj_t *)MP_OBJ_TO_PTR(self_in))->x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tilemap_get_x_obj, tilemap_get_x);
MP_PROPERTY_GETTER(tilemap_x_obj, (mp_obj_t)&tilemap_get_x_obj);

static mp_obj_t tilemap_get_y(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_tilemap_obj_t *)MP_OBJ_TO_PTR(self_in))->y);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tilemap_get_y_obj, tilemap_get_y);
MP_PROPERTY_GETTER(tilemap_y_obj, (mp_obj_t)&tilemap_get_y_obj);

static mp_obj_t tilemap_get_cols(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_tilemap_obj_t *)MP_OBJ_TO_PTR(self_in))->map_w);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tilemap_get_cols_obj, tilemap_get_cols);
MP_PROPERTY_GETTER(tilemap_cols_obj, (mp_obj_t)&tilemap_get_cols_obj);

static mp_obj_t tilemap_get_rows(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_tilemap_obj_t *)MP_OBJ_TO_PTR(self_in))->map_h);
}
static MP_DEFINE_CONST_FUN_OBJ_1(tilemap_get_rows_obj, tilemap_get_rows);
MP_PROPERTY_GETTER(tilemap_rows_obj, (mp_obj_t)&tilemap_get_rows_obj);

static const mp_rom_map_elem_t picogame_tilemap_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_tile), MP_ROM_PTR(&picogame_tilemap_tile_obj) },
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&picogame_tilemap_move_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&picogame_tilemap_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&tilemap_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&tilemap_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_cols), MP_ROM_PTR(&tilemap_cols_obj) },
    { MP_ROM_QSTR(MP_QSTR_rows), MP_ROM_PTR(&tilemap_rows_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_tilemap_locals_dict, picogame_tilemap_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_tilemap_type,
    MP_QSTR_Tilemap,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_tilemap_make_new,
    locals_dict, &picogame_tilemap_locals_dict
    );
