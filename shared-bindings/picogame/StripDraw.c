// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame.StripDraw: an immediate-mode draw layer that holds no pixel buffer.

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-bindings/picogame/__init__.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-bindings/picogame/Sprite.h"
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"   // fast DMA backend; absent on portable ports
#include "common-hal/picogame/Display.h"        // its struct (pg_get_display unwraps the wrapper)
#endif
#include "shared-bindings/picogame/Scene.h"
#include "shared-bindings/picogame/Tilemap.h"
#include "shared-bindings/picogame/Particles.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-bindings/picogame/Framebuffer.h"
#include "shared-bindings/picogame/StripDraw.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame/Bitmap.h"
#include "shared-module/picogame/Sprite.h"

// ---------------------------------------------------------------------------
// StripDraw  (immediate-mode draw layer; struct in shared-module/picogame/__init__.h)
// ---------------------------------------------------------------------------

//| class StripDraw:
//|     """An immediate-mode draw layer that holds NO pixel buffer. Each refresh, for
//|     every render strip overlapping its rect, ``callback(view, vx, vy, vw, vh)`` is
//|     called with a :py:class:`Canvas` ``view`` pointing straight at the live strip
//|     buffer - so you draw primitives directly into the frame (zero RAM, vs a Canvas
//|     which costs width*height*2 bytes). The view's local (0, 0) is screen pixel
//|     (vx, vy); (vw, vh) is the strip size. Draw only the rows in [vy, vy+vh) for
//|     speed (anything outside the view is clipped anyway). The rect is repainted every
//|     frame, so use it for animated / scanline content (pseudo-3D, gradients,
//|     procedural backgrounds), not static art (use Canvas for that).
//|
//|     COORDINATE CONTRACT: ``vx`` is the RENDER REGION's origin (NOT this layer's x), and the
//|     view spans the FULL region WIDTH (the layer's rect only gates which ROWS run). So draw at
//|     ABSOLUTE screen coords minus (vx, vy), and fill only your own rect with ``fill_rect`` -
//|     ``view.clear()`` fills the whole region width. (When you render a StripDraw via
//|     ``picogame.render([sd], buf, x,y,x+w,y+h)`` the region == the rect, so vx == x.)
//|     Text via ``Canvas.text`` is ASCII (the built-in font); non-ASCII has no glyph."""
//|
//|     def __init__(
//|         self,
//|         callback: Callable[[Canvas, int, int, int, int], None],
//|         x: int = 0,
//|         y: int = 0,
//|         width: int = 0,
//|         height: int = 0,
//|     ) -> None: ...
//|
static mp_obj_t picogame_stripdraw_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_callback, ARG_x, ARG_y, ARG_width, ARG_height, ARG_always_dirty };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_callback, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_width, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_always_dirty, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    picogame_stripdraw_obj_t *self = mp_obj_malloc(picogame_stripdraw_obj_t, type);
    self->callback = args[ARG_callback].u_obj;
    self->x = args[ARG_x].u_int;
    self->y = args[ARG_y].u_int;
    self->w = args[ARG_width].u_int;
    self->h = args[ARG_height].u_int;
    self->faulted = false;
    self->always_dirty = args[ARG_always_dirty].u_bool;
    picogame_dirty_reset(&self->dx1);        // render once on first refresh (even when always_dirty=False)
    picogame_dirty_union(&self->dx1, self->x, self->y, self->x + self->w, self->y + self->h);
    // A buffer-less Canvas reused as the per-strip drawing view: its `data` is
    // repointed at the live strip each blit, so no surface RAM is allocated here.
    picogame_canvas_obj_t *view = mp_obj_malloc(picogame_canvas_obj_t, &picogame_canvas_type);
    view->data = NULL;
    view->data_obj = MP_OBJ_NULL;
    view->w = 0;
    view->h = 0;
    view->x = 0;
    view->y = 0;
    view->transparent = 0;
    view->has_transparent = false;
    picogame_canvas_dirty_reset(view);
    self->view = MP_OBJ_FROM_PTR(view);
    return MP_OBJ_FROM_PTR(self);
}

//|
//|     x: int
//|     y: int
//|     width: int
//|     height: int
//|     """The screen rect repainted each refresh (read/write). Move or resize the layer
//|     by assigning these. Shrinking the rect leaves stale pixels behind - follow a
//|     shrink with ``scene.invalidate()`` for a clean repaint (as the fx helpers do)."""
static mp_obj_t sd_get_x(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_x_obj, sd_get_x);
static mp_obj_t sd_set_x(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->x = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_x_obj, sd_set_x);
MP_PROPERTY_GETSET(sd_x_obj, (mp_obj_t)&sd_get_x_obj, (mp_obj_t)&sd_set_x_obj);

static mp_obj_t sd_get_y(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->y);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_y_obj, sd_get_y);
static mp_obj_t sd_set_y(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->y = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_y_obj, sd_set_y);
MP_PROPERTY_GETSET(sd_y_obj, (mp_obj_t)&sd_get_y_obj, (mp_obj_t)&sd_set_y_obj);

static mp_obj_t sd_get_width(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->w);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_width_obj, sd_get_width);
static mp_obj_t sd_set_width(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->w = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_width_obj, sd_set_width);
MP_PROPERTY_GETSET(sd_width_obj, (mp_obj_t)&sd_get_width_obj, (mp_obj_t)&sd_set_width_obj);

static mp_obj_t sd_get_height(mp_obj_t self_in) {
    return MP_OBJ_NEW_SMALL_INT(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->h);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_height_obj, sd_get_height);
static mp_obj_t sd_set_height(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->h = mp_obj_get_int(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_height_obj, sd_set_height);
MP_PROPERTY_GETSET(sd_height_obj, (mp_obj_t)&sd_get_height_obj, (mp_obj_t)&sd_set_height_obj);

//|     always_dirty: bool
//|     """True (default): repaint every frame - for animated content (pseudo-3D, gradients). False:
//|     repaint only after an ``invalidate()`` call (or when overlapped by another dirty layer) - for on-change UI,
//|     so a static panel doesn't re-rasterize+re-push every frame. With False you MUST invalidate() on
//|     every content/visibility change (it's invisible until you do)."""
//|
static mp_obj_t sd_get_always_dirty(mp_obj_t self_in) {
    return mp_obj_new_bool(((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->always_dirty);
}
static MP_DEFINE_CONST_FUN_OBJ_1(sd_get_always_dirty_obj, sd_get_always_dirty);
static mp_obj_t sd_set_always_dirty(mp_obj_t self_in, mp_obj_t v) {
    ((picogame_stripdraw_obj_t *)MP_OBJ_TO_PTR(self_in))->always_dirty = mp_obj_is_true(v);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(sd_set_always_dirty_obj, sd_set_always_dirty);
MP_PROPERTY_GETSET(sd_always_dirty_obj, (mp_obj_t)&sd_get_always_dirty_obj, (mp_obj_t)&sd_set_always_dirty_obj);

//|     def invalidate(self, x: int = 0, y: int = 0, w: int = 0, h: int = 0) -> None:
//|         """Mark dirty so the layer repaints on the next refresh (only needed when
//|         ``always_dirty=False``). With no args, the whole layer repaints. Pass a rect in
//|         VIEW-LOCAL coordinates (the same (0,0)-at-``(vx, vy)`` space the draw callback uses) to
//|         repaint only that region - like Canvas/Tilemap, the Scene then recomposites and pushes just
//|         those rows. Repeated calls union; the rect is clamped to the layer."""
//|
//|
static mp_obj_t sd_invalidate(size_t n_args, const mp_obj_t *args) {
    picogame_stripdraw_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args >= 5) {                       // (x, y, w, h) in view-local coords -> clamped scene rect
        int x1 = self->x + mp_obj_get_int(args[1]);
        int y1 = self->y + mp_obj_get_int(args[2]);
        int x2 = x1 + mp_obj_get_int(args[3]);
        int y2 = y1 + mp_obj_get_int(args[4]);
        if (x1 < self->x) {
            x1 = self->x;
        }
        if (y1 < self->y) {
            y1 = self->y;
        }
        if (x2 > self->x + self->w) {
            x2 = self->x + self->w;
        }
        if (y2 > self->y + self->h) {
            y2 = self->y + self->h;
        }
        if (x2 > x1 && y2 > y1) {
            picogame_dirty_union(&self->dx1, x1, y1, x2, y2);
        }
    } else {                                 // whole layer
        picogame_dirty_union(&self->dx1, self->x, self->y, self->x + self->w, self->y + self->h);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sd_invalidate_obj, 1, 5, sd_invalidate);

static const mp_rom_map_elem_t picogame_stripdraw_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&sd_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&sd_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&sd_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&sd_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_always_dirty), MP_ROM_PTR(&sd_always_dirty_obj) },
    { MP_ROM_QSTR(MP_QSTR_invalidate), MP_ROM_PTR(&sd_invalidate_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_stripdraw_locals_dict, picogame_stripdraw_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_stripdraw_type,
    MP_QSTR_StripDraw,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, picogame_stripdraw_make_new,
    locals_dict, &picogame_stripdraw_locals_dict
    );
