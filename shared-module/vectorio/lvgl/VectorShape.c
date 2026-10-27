// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// LVGL implementation of the vectorio VectorShape wrapper. See lvgl/VectorShape.h for the
// design. In short: a shape is one lv_obj, LVGL does all drawing and invalidation, and none
// of the scanline compositor's area/mask/dirty-area machinery exists here.

#include "shared-bindings/vectorio/VectorShape.h"

#include <stdint.h>

#include "py/runtime.h"

#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-bindings/vectorio/Circle.h"
#include "shared-bindings/vectorio/Polygon.h"
#include "shared-bindings/vectorio/Rectangle.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "shared-module/vectorio/__init__.h"

#if CIRCUITPY_VECTORIO && CIRCUITPY_DISPLAYIO_LVGL

#define COVERAGE_OPAQUE 0xff

static bool _is_polygon(vectorio_vector_shape_t *self) {
    return mp_obj_is_type(self->ishape.shape, &vectorio_polygon_type);
}

// The shape's color_index is biased by one so that 0 can always mean "not covered".
static uint16_t _biased_color_index(vectorio_vector_shape_t *self) {
    if (mp_obj_is_type(self->ishape.shape, &vectorio_rectangle_type)) {
        return ((vectorio_rectangle_t *)MP_OBJ_TO_PTR(self->ishape.shape))->color_index;
    }
    if (mp_obj_is_type(self->ishape.shape, &vectorio_circle_type)) {
        return ((vectorio_circle_t *)MP_OBJ_TO_PTR(self->ishape.shape))->color_index;
    }
    return ((vectorio_polygon_t *)MP_OBJ_TO_PTR(self->ishape.shape))->color_index;
}

// Resolve the pixel shader to the color to paint. Returns false when nothing should be
// drawn at all, which is what the "not covered" color index, a transparent palette entry
// and a ColorConverter's transparent color all mean.
static bool _resolve_color(mp_obj_t pixel_shader, uint16_t biased_index, lv_color_t *color) {
    if (biased_index == 0 || pixel_shader == mp_const_none) {
        return false;
    }
    uint32_t index = (uint32_t)biased_index - 1;
    if (mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
        displayio_palette_t *palette = MP_OBJ_TO_PTR(pixel_shader);
        if (index >= common_hal_displayio_palette_get_len(palette) ||
            common_hal_displayio_palette_is_transparent(palette, index)) {
            return false;
        }
        *color = lv_color_hex(common_hal_displayio_palette_get_color(palette, index));
        return true;
    }
    displayio_colorconverter_t *converter = MP_OBJ_TO_PTR(pixel_shader);
    if (index == converter->transparent_color) {
        return false;
    }
    // A ColorConverter maps a pixel in its input colorspace onto the display's output
    // colorspace. LVGL does the output conversion itself while blending, so all that is
    // needed here is the rgb888 value of the input pixel.
    *color = lv_color_hex(displayio_colorconverter_convert_pixel(
        (displayio_colorspace_t)converter->input_colorspace, index));
    return true;
}

// The shape's extent in its own coordinate space. Rectangles start at the origin, circles
// are centered on it, and polygons use the bounds of their points. This is the same
// geometry the legacy compositor gets from get_area(), just computed where it is needed
// instead of being part of the shape's contract.
//
// The bounds are inclusive of both ends, matching the pixel range the legacy compositor
// asks get_pixel() about: the rectangle covers [0, width) x [0, height), the circle covers
// [-radius, radius] on both axes and the polygon covers the bounding box of its points.
static void _shape_bounds(vectorio_vector_shape_t *self, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2) {
    if (mp_obj_is_type(self->ishape.shape, &vectorio_rectangle_type)) {
        vectorio_rectangle_t *rectangle = MP_OBJ_TO_PTR(self->ishape.shape);
        *x1 = 0;
        *y1 = 0;
        *x2 = rectangle->width - 1;
        *y2 = rectangle->height - 1;
        return;
    }
    if (mp_obj_is_type(self->ishape.shape, &vectorio_circle_type)) {
        vectorio_circle_t *circle = MP_OBJ_TO_PTR(self->ishape.shape);
        *x1 = -(int32_t)circle->radius;
        *y1 = -(int32_t)circle->radius;
        *x2 = (int32_t)circle->radius;
        *y2 = (int32_t)circle->radius;
        return;
    }
    vectorio_polygon_t *polygon = MP_OBJ_TO_PTR(self->ishape.shape);
    if (polygon->points_list == NULL || polygon->len < 2) {
        *x1 = 0;
        *y1 = 0;
        *x2 = 0;
        *y2 = 0;
        return;
    }
    int32_t min_x = polygon->points_list[0];
    int32_t max_x = min_x;
    int32_t min_y = polygon->points_list[1];
    int32_t max_y = min_y;
    for (uint16_t i = 2; i + 1 < polygon->len; i += 2) {
        int32_t x = polygon->points_list[i];
        int32_t y = polygon->points_list[i + 1];
        if (x < min_x) {
            min_x = x;
        }
        if (x > max_x) {
            max_x = x;
        }
        if (y < min_y) {
            min_y = y;
        }
        if (y > max_y) {
            max_y = y;
        }
    }
    *x1 = min_x;
    *y1 = min_y;
    *x2 = max_x;
    *y2 = max_y;
}

static void _update_visible(vectorio_vector_shape_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    if (self->hidden || self->hidden_by_parent) {
        lv_obj_add_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
    }
}

// Rebuild the polygon coverage mask. LVGL treats an A8 image as a mask and blends it with
// the image recolor color, so the mask only needs to say where the shape is, not what color
// it is. Group scale is baked into the mask resolution, which reproduces displayio's
// nearest-neighbor sampling of the shape.
static void _refresh_mask(vectorio_vector_shape_t *self, int32_t x1, int32_t y1, int32_t w,
    int32_t h, uint16_t scale) {
    if (self->mask == NULL || (int32_t)self->mask->header.w != w || (int32_t)self->mask->header.h != h) {
        // The image borrows the mask. Clear its source before freeing the old buffer:
        // if the replacement allocation fails it must draw nothing, not freed memory.
        lv_image_set_src(self->lv_obj, NULL);
        if (self->mask != NULL) {
            lv_draw_buf_destroy(self->mask);
            self->mask = NULL;
        }
        self->mask = lv_draw_buf_create(w, h, LV_COLOR_FORMAT_A8, 0);
        if (self->mask == NULL) {
            return;
        }
        // Hand the new buffer to the image. The bin decoder wraps a variable A8 image
        // instead of copying it, so rewriting the pixels below is visible without dropping
        // any cache entry.
        lv_image_set_src(self->lv_obj, self->mask);
    }
    uint32_t stride = self->mask->header.stride;
    for (int32_t y = 0; y < h; y++) {
        uint8_t *row = self->mask->data + (uint32_t)y * stride;
        int16_t shape_y = y1 + y / scale;
        for (int32_t x = 0; x < w; x++) {
            row[x] = self->ishape.get_pixel(self->ishape.shape, x1 + x / scale, shape_y) == 0
                ? 0
                : COVERAGE_OPAQUE;
        }
    }
}

// lv_obj_set_pos() only records a style that a later layout pass applies, and lv_obj_move_to()
// leaves that style behind, so an unrelated size change would snap the object back. Set both: the
// coordinates are right immediately and nothing can undo them.
static void _place(lv_obj_t *obj, int32_t x, int32_t y) {
    lv_obj_set_style_x(obj, x, 0);
    lv_obj_set_style_y(obj, y, 0);
    lv_obj_move_to(obj, x, y);
}

// Push every Python-visible property of the shape into its mirror. Called under the LVGL
// lock from everything that can change the shape. `rebuild_mask` is false for a pure move,
// which is the one change that can't affect the polygon's coverage.
static void _sync_mirror(vectorio_vector_shape_t *self, bool rebuild_mask) {
    if (self->lv_obj == NULL) {
        return;
    }
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    _shape_bounds(self, &x1, &y1, &x2, &y2);
    uint16_t scale = self->abs_scale == 0 ? 1 : self->abs_scale;
    // The bounds are inclusive, so the extent is one more than the difference.
    int32_t w = (x2 - x1 + 1) * scale;
    int32_t h = (y2 - y1 + 1) * scale;
    // A shape with an empty extent (a rectangle with no width or height) covers no
    // pixels at all, so it must not be painted.
    bool empty = w < 1 || h < 1;
    if (w < 1) {
        w = 1;
    }
    if (h < 1) {
        h = 1;
    }

    bool polygon = _is_polygon(self);
    // abs_x/y is the parent group's origin on the screen; groups are screen sized
    // objects at the screen origin, so members are positioned in screen coordinates.
    _place(self->lv_obj, self->abs_x + (self->x + x1) * scale,
        self->abs_y + (self->y + y1) * scale);
    if (lv_obj_get_width(self->lv_obj) != w || lv_obj_get_height(self->lv_obj) != h) {
        lv_obj_set_size(self->lv_obj, w, h);
        // A new size means a new mask buffer too.
        rebuild_mask = true;
    }
    if (polygon && rebuild_mask) {
        _refresh_mask(self, x1, y1, w, h, scale);
    } else if (!polygon) {
        bool circle = mp_obj_is_type(self->ishape.shape, &vectorio_circle_type);
        lv_obj_set_style_radius(self->lv_obj, circle ? LV_RADIUS_CIRCLE : 0, 0);
    }

    lv_color_t color;
    bool painted = !empty && _resolve_color(self->pixel_shader, _biased_color_index(self), &color);
    if (polygon) {
        if (painted) {
            lv_obj_set_style_image_recolor(self->lv_obj, color, 0);
        }
        lv_obj_set_style_image_recolor_opa(self->lv_obj, painted ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    } else {
        if (painted) {
            lv_obj_set_style_bg_color(self->lv_obj, color, 0);
        }
        lv_obj_set_style_bg_opa(self->lv_obj, painted ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    }
}

// A move changes nothing but where the shape is, so the polygon mask survives it.
static void _set_dirty(vectorio_vector_shape_t *self, bool rebuild_mask) {
    if (self->lv_obj == NULL) {
        return;
    }
    displayio_lvgl_lock();
    // The shape's old area has to be redrawn too, and batching below would drop that
    // invalidation once the move has already happened.
    lv_obj_invalidate(self->lv_obj);
    // Batch: the sync writes several styles and only the final invalidation matters.
    lv_display_t *display = lv_obj_get_display(self->lv_obj);
    if (display != NULL) {
        lv_display_enable_invalidation(display, false);
    }
    _sync_mirror(self, rebuild_mask);
    if (display != NULL) {
        lv_display_enable_invalidation(display, true);
    }
    lv_obj_invalidate(self->lv_obj);
    displayio_lvgl_unlock();
}

void common_hal_vectorio_vector_shape_construct(vectorio_vector_shape_t *self,
    vectorio_ishape_t ishape, mp_obj_t pixel_shader, int32_t x, int32_t y) {
    x = mp_arg_validate_int_range(x, SHRT_MIN, SHRT_MAX, MP_QSTR_x);
    y = mp_arg_validate_int_range(y, SHRT_MIN, SHRT_MAX, MP_QSTR_y);
    self->x = x;
    self->y = y;
    self->pixel_shader = pixel_shader;
    self->ishape = ishape;
    self->abs_x = 0;
    self->abs_y = 0;
    self->abs_scale = 1;
    self->lv_obj = NULL;
    self->mask = NULL;
    self->hidden = false;
    self->hidden_by_parent = false;
    self->in_group = false;
}

// This must be invoked after each time a shape changes its position, shape or appearance
// in any way.
void common_hal_vectorio_vector_shape_set_dirty(void *vector_shape) {
    // The shape itself changed (points, size, color index), so its coverage has to be redone.
    _set_dirty(vector_shape, true);
}

static int32_t _screen_to_shape(int32_t screen, int32_t origin, uint16_t scale) {
    int32_t relative = screen - origin;
    if (relative < 0) {
        // Floor division so that negative coordinates map to the pixel that contains them.
        return -((-relative + scale - 1) / scale);
    }
    return relative / scale;
}

bool common_hal_vectorio_vector_shape_contains(vectorio_vector_shape_t *self, mp_int_t x, mp_int_t y) {
    x = mp_arg_validate_int_range(x, SHRT_MIN, SHRT_MAX, MP_QSTR_x);
    y = mp_arg_validate_int_range(y, SHRT_MIN, SHRT_MAX, MP_QSTR_y);
    uint16_t scale = self->abs_scale == 0 ? 1 : self->abs_scale;
    int16_t shape_x = (int16_t)(_screen_to_shape(x, self->abs_x, scale) - self->x);
    int16_t shape_y = (int16_t)(_screen_to_shape(y, self->abs_y, scale) - self->y);
    return self->ishape.get_pixel(self->ishape.shape, shape_x, shape_y) != 0;
}

mp_int_t common_hal_vectorio_vector_shape_get_x(vectorio_vector_shape_t *self) {
    return self->x;
}

void common_hal_vectorio_vector_shape_set_x(vectorio_vector_shape_t *self, mp_int_t x) {
    if (self->x == x) {
        return;
    }
    self->x = mp_arg_validate_int_range(x, SHRT_MIN, SHRT_MAX, MP_QSTR_x);
    _set_dirty(self, false);
}

mp_int_t common_hal_vectorio_vector_shape_get_y(vectorio_vector_shape_t *self) {
    return self->y;
}

void common_hal_vectorio_vector_shape_set_y(vectorio_vector_shape_t *self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    self->y = mp_arg_validate_int_range(y, SHRT_MIN, SHRT_MAX, MP_QSTR_y);
    _set_dirty(self, false);
}

mp_obj_tuple_t *common_hal_vectorio_vector_shape_get_location(vectorio_vector_shape_t *self) {
    mp_obj_tuple_t *pair = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    pair->items[0] = mp_obj_new_int((mp_int_t)self->x);
    pair->items[1] = mp_obj_new_int((mp_int_t)self->y);
    return pair;
}

void common_hal_vectorio_vector_shape_set_location(vectorio_vector_shape_t *self, mp_obj_t xy) {
    size_t tuple_len = 0;
    mp_obj_t *tuple_items;
    mp_obj_tuple_get(xy, &tuple_len, &tuple_items);
    mp_arg_validate_length(tuple_len, 2, MP_QSTR_location);

    mp_int_t x = mp_arg_validate_type_int(tuple_items[0], MP_QSTR_x);
    mp_int_t y = mp_arg_validate_type_int(tuple_items[1], MP_QSTR_y);
    x = mp_arg_validate_int_range(x, SHRT_MIN, SHRT_MAX, MP_QSTR_x);
    y = mp_arg_validate_int_range(y, SHRT_MIN, SHRT_MAX, MP_QSTR_y);
    if (self->x == x && self->y == y) {
        return;
    }
    self->x = x;
    self->y = y;
    _set_dirty(self, false);
}

mp_int_t common_hal_vectorio_vector_shape_get_hidden(vectorio_vector_shape_t *self) {
    return self->hidden;
}

void common_hal_vectorio_vector_shape_set_hidden(vectorio_vector_shape_t *self, bool hidden) {
    self->hidden = hidden;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _update_visible(self);
        displayio_lvgl_unlock();
    }
    _set_dirty(self, false);
}

mp_obj_t common_hal_vectorio_vector_shape_get_pixel_shader(vectorio_vector_shape_t *self) {
    return self->pixel_shader;
}

void common_hal_vectorio_vector_shape_set_pixel_shader(vectorio_vector_shape_t *self, mp_obj_t pixel_shader) {
    if (self->pixel_shader != pixel_shader) {
        displayio_lvgl_shader_unregister_user(self->pixel_shader, self);
        self->pixel_shader = pixel_shader;
        if (self->lv_obj != NULL) {
            displayio_lvgl_shader_register_user(pixel_shader,
                self, DISPLAYIO_LVGL_USER_VECTOR_SHAPE);
        }
    }
    // A different shader is a different color, not different coverage.
    _set_dirty(self, false);
}

bool vectorio_vector_shape_set_in_group(vectorio_vector_shape_t *self, bool in_group) {
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

void displayio_lvgl_vector_shape_attach(vectorio_vector_shape_t *self, lv_obj_t *parent) {
    if (parent == NULL || !self->in_group) {
        return;
    }
    displayio_lvgl_lock();
    if (self->lv_obj == NULL) {
        bool polygon = _is_polygon(self);
        self->lv_obj = polygon ? lv_image_create(parent) : lv_obj_create(parent);
        if (self->lv_obj == NULL) {
            displayio_lvgl_unlock();
            return;
        }
        // Take the theme out of the picture so the shape is exactly what we style it to be.
        lv_obj_remove_style_all(self->lv_obj);
        lv_obj_set_style_pad_all(self->lv_obj, 0, 0);
        lv_obj_set_style_border_width(self->lv_obj, 0, 0);
        lv_obj_set_style_radius(self->lv_obj, 0, 0);
        lv_obj_set_style_bg_opa(self->lv_obj, LV_OPA_TRANSP, 0);
        lv_obj_set_style_opa(self->lv_obj, LV_OPA_COVER, 0);
        if (polygon) {
            lv_obj_set_style_image_opa(self->lv_obj, LV_OPA_COVER, 0);
        }
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_SCROLLABLE);
        _update_visible(self);
        _sync_mirror(self, true);
    } else if (lv_obj_get_parent(self->lv_obj) != parent) {
        lv_obj_set_parent(self->lv_obj, parent);
    }
    displayio_lvgl_shader_register_user(self->pixel_shader,
        self, DISPLAYIO_LVGL_USER_VECTOR_SHAPE);
    displayio_lvgl_unlock();
}

void displayio_lvgl_vector_shape_detach(vectorio_vector_shape_t *self) {
    displayio_lvgl_shader_unregister_user(self->pixel_shader, self);
    if (self->lv_obj == NULL) {
        return;
    }
    displayio_lvgl_lock();
    lv_obj_delete(self->lv_obj);
    self->lv_obj = NULL;
    if (self->mask != NULL) {
        lv_draw_buf_destroy(self->mask);
        self->mask = NULL;
    }
    displayio_lvgl_unlock();
}

void displayio_lvgl_vector_shape_set_transform(vectorio_vector_shape_t *self, int32_t abs_x,
    int32_t abs_y, uint16_t scale) {
    if (self->abs_x == abs_x && self->abs_y == abs_y && self->abs_scale == scale) {
        return;
    }
    self->abs_x = abs_x;
    self->abs_y = abs_y;
    uint16_t previous_scale = self->abs_scale;
    self->abs_scale = scale == 0 ? 1 : scale;
    // The mask is rasterized at the scaled resolution, so a scale change rebuilds it.
    _set_dirty(self, self->abs_scale != previous_scale);
}

void displayio_lvgl_vector_shape_set_hidden_by_parent(vectorio_vector_shape_t *self, bool hidden) {
    self->hidden_by_parent = hidden;
    if (self->lv_obj != NULL) {
        displayio_lvgl_lock();
        _update_visible(self);
        lv_obj_invalidate(self->lv_obj);
        displayio_lvgl_unlock();
    }
}

void displayio_lvgl_vector_shape_shader_changed(vectorio_vector_shape_t *self) {
    // Called with the LVGL lock held by the shader's notify path.
    _sync_mirror(self, false);
}

// protocol_lvgl implementation. Group owns the tree; these only translate its calls into the
// shape mirror operations above.
static lv_obj_t *_vector_shape_protocol_root_obj(mp_obj_t layer) {
    return ((vectorio_vector_shape_t *)MP_OBJ_TO_PTR(layer))->lv_obj;
}

static void _vector_shape_protocol_attach(mp_obj_t layer, lv_obj_t *parent) {
    displayio_lvgl_vector_shape_attach(MP_OBJ_TO_PTR(layer), parent);
}

static void _vector_shape_protocol_detach(mp_obj_t layer) {
    displayio_lvgl_vector_shape_detach(MP_OBJ_TO_PTR(layer));
}

static bool _vector_shape_protocol_set_transform(mp_obj_t layer, int32_t abs_x,
    int32_t abs_y, uint16_t scale) {
    displayio_lvgl_vector_shape_set_transform(MP_OBJ_TO_PTR(layer), abs_x, abs_y, scale);
    return true;
}

static void _vector_shape_protocol_set_hidden_by_parent(mp_obj_t layer, bool hidden) {
    displayio_lvgl_vector_shape_set_hidden_by_parent(MP_OBJ_TO_PTR(layer), hidden);
}

static bool _vector_shape_protocol_set_in_group(mp_obj_t layer, bool in_group) {
    return vectorio_vector_shape_set_in_group(MP_OBJ_TO_PTR(layer), in_group);
}

const lvgl_protocol_t displayio_lvgl_vector_shape_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_lvgl)
    .root_obj = _vector_shape_protocol_root_obj,
    .attach = _vector_shape_protocol_attach,
    .detach = _vector_shape_protocol_detach,
    .set_transform = _vector_shape_protocol_set_transform,
    .set_hidden_by_parent = _vector_shape_protocol_set_hidden_by_parent,
    .set_in_group = _vector_shape_protocol_set_in_group,
};

#endif  // CIRCUITPY_VECTORIO && CIRCUITPY_DISPLAYIO_LVGL
