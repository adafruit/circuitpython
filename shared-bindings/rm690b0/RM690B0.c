// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/rm690b0/RM690B0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/time/__init__.h"
#include "py/objproperty.h"
#include "py/objstr.h"

//| class RM690B0:
//|     """RM690B0 driver for AMOLED displays
//|
//|     This is a basic driver for the RM690B0 AMOLED display controller.
//|     It provides low-level drawing primitives (pixels, lines, rectangles, circles,
//|     blitting bitmaps) and a small built-in text API with several fixed fonts.
//|     """
//|
//|     def __init__(self) -> None:
//|         """Initialize the RM690B0 display driver
//|
//|         Initializes the panel and internal framebuffers. Call `init_display()`
//|         before performing any drawing operations.
//|         """
//|         ...
//|

static mp_obj_t rm690b0_rm690b0_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // For now, accept no arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    rm690b0_rm690b0_obj_t *self = mp_obj_malloc(rm690b0_rm690b0_obj_t, &rm690b0_rm690b0_type);

    common_hal_rm690b0_rm690b0_construct(self);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialize the display driver
//|
//|         Can be called as instance method or static method:
//|
//|         Instance method::
//|
//|             d = rm690b0.RM690B0()
//|             d.deinit()  # Cleans up this specific instance
//|
//|         Static method (REPL convenience)::
//|
//|             import rm690b0
//|             # If display is stuck/initialized without a reference
//|             rm690b0.RM690B0.deinit()  # Cleans up any active instance
//|             # Now you can create a new instance
//|             d = rm690b0.RM690B0()
//|         """
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_deinit(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // Called as static method: RM690B0.deinit()
        common_hal_rm690b0_rm690b0_deinit_all();
    } else {
        // Called as instance method: d.deinit()
        rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
        common_hal_rm690b0_rm690b0_deinit(self);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_deinit_obj, 0, 1, rm690b0_rm690b0_deinit);

//|     def init_display(self) -> None:
//|         """Initialize the display with default settings"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_init_display(mp_obj_t self_in) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rm690b0_rm690b0_init_display(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_rm690b0_init_display_obj, rm690b0_rm690b0_init_display);

//|     # --- Built-in font and text API ---
//|
//|     def set_font(self, font: int) -> None:
//|         """Select one of the built-in fonts for subsequent text drawing.
//|
//|         :param int font: Font identifier. Available fonts:
//|             0 = 8x8 monospace (smallest)
//|             1 = 16x16 monospace (Liberation Sans)
//|             2 = 16x24 monospace (Liberation Mono Bold)
//|             3 = 24x24 monospace
//|             4 = 24x32 monospace
//|             5 = 32x32 monospace
//|             6 = 32x48 monospace (largest)
//|
//|         This affects `text()` only and does not interact with LVGL or Tiny TTF.
//|         """
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_set_font(mp_obj_t self_in, mp_obj_t font_obj) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t font_id = mp_obj_get_int(font_obj);
    common_hal_rm690b0_rm690b0_set_font(self, font_id);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(rm690b0_rm690b0_set_font_obj, rm690b0_rm690b0_set_font);

//|     def text(self, x: int, y: int, text: str, color: int = 0xFFFF, background: int | None = None) -> None:
//|         """Draw a UTF-8 string using the currently selected built-in font.
//|
//|         :param int x: Left coordinate (in pixels)
//|         :param int y: Top coordinate (in pixels)
//|         :param str text: Text to draw (UTF-8; unsupported glyphs may be replaced)
//|         :param int color: Foreground RGB565 color (default: white)
//|         :param int|None background: Optional background RGB565 color. If None, text is drawn transparent over existing pixels.
//|
//|         This is a lightweight, non-LVGL text API intended for labels, debug info
//|         and simple UIs. For full-featured text layout and TTF fonts, use `rm690b0_lvgl`.
//|         """
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_text(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_x, ARG_y, ARG_text, ARG_color, ARG_background };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_text, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_color, MP_ARG_INT, {.u_int = 0xFFFF} },
        { MP_QSTR_background, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t x = args[ARG_x].u_int;
    mp_int_t y = args[ARG_y].u_int;

    // Accept any object that can be converted to str
    mp_obj_t text_obj = args[ARG_text].u_obj;
    GET_STR_DATA_LEN(text_obj, text_data, text_len);

    uint16_t fg = (uint16_t)(args[ARG_color].u_int & 0xFFFF);
    bool has_bg = args[ARG_background].u_obj != mp_const_none;
    uint16_t bg = 0;
    if (has_bg) {
        bg = (uint16_t)(mp_obj_get_int(args[ARG_background].u_obj) & 0xFFFF);
    }

    common_hal_rm690b0_rm690b0_text(self, x, y,
        (const char *)text_data, (size_t)text_len,
        fg, has_bg, bg);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(rm690b0_rm690b0_text_obj, 1, rm690b0_rm690b0_text);

static mp_obj_t rm690b0_rm690b0_fill_color(mp_obj_t self_in, mp_obj_t color_obj) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint16_t color = mp_obj_get_int(color_obj) & 0xFFFF;
    common_hal_rm690b0_rm690b0_fill_color(self, color);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(rm690b0_rm690b0_fill_color_obj, rm690b0_rm690b0_fill_color);

//|     def pixel(self, x: int, y: int, color: int) -> None:
//|         """Draw a single pixel"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_pixel(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);
    common_hal_rm690b0_rm690b0_pixel(self, x, y, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_pixel_obj, 4, 4, rm690b0_rm690b0_pixel);

//|     def fill_rect(self, x: int, y: int, width: int, height: int, color: int) -> None:
//|         """Draw a filled rectangle"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_fill_rect(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t height = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    common_hal_rm690b0_rm690b0_fill_rect(self, x, y, width, height, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_fill_rect_obj, 6, 6, rm690b0_rm690b0_fill_rect);

//|     def hline(self, x: int, y: int, width: int, color: int) -> None:
//|         """Draw a horizontal line"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_hline(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);
    common_hal_rm690b0_rm690b0_hline(self, x, y, width, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_hline_obj, 5, 5, rm690b0_rm690b0_hline);

//|     def vline(self, x: int, y: int, height: int, color: int) -> None:
//|         """Draw a vertical line"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_vline(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t height = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);
    common_hal_rm690b0_rm690b0_vline(self, x, y, height, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_vline_obj, 5, 5, rm690b0_rm690b0_vline);

//|     def line(self, x0: int, y0: int, x1: int, y1: int, color: int) -> None:
//|         """Draw a line"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_line(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x0 = mp_obj_get_int(args[1]);
    mp_int_t y0 = mp_obj_get_int(args[2]);
    mp_int_t x1 = mp_obj_get_int(args[3]);
    mp_int_t y1 = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    common_hal_rm690b0_rm690b0_line(self, x0, y0, x1, y1, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_line_obj, 6, 6, rm690b0_rm690b0_line);

//|     def rect(self, x: int, y: int, width: int, height: int, color: int) -> None:
//|         """Draw a rectangle outline"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_rect(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t width = mp_obj_get_int(args[3]);
    mp_int_t height = mp_obj_get_int(args[4]);
    mp_int_t color = mp_obj_get_int(args[5]);
    common_hal_rm690b0_rm690b0_rect(self, x, y, width, height, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_rect_obj, 6, 6, rm690b0_rm690b0_rect);

//|     def circle(self, x: int, y: int, radius: int, color: int) -> None:
//|         """Draw a circle outline"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_circle(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t radius = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);
    common_hal_rm690b0_rm690b0_circle(self, x, y, radius, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_circle_obj, 5, 5, rm690b0_rm690b0_circle);

//|     def fill_circle(self, x: int, y: int, radius: int, color: int) -> None:
//|         """Draw a filled circle"""
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_fill_circle(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_int_t radius = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);
    common_hal_rm690b0_rm690b0_fill_circle(self, x, y, radius, color);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_fill_circle_obj, 5, 5, rm690b0_rm690b0_fill_circle);




//|     def blit_buffer(self, x: int, y: int, width: int, height: int, bitmap_data: bytearray) -> None:
//|         """Draw a bitmap"""
//|         ...
//|
//|     def blit_bmp(self, x: int, y: int, bmp_data: readablebuffer) -> None:
//|         """Draw a BMP image from memory.
//|
//|         :param int x: X coordinate of the top-left corner
//|         :param int y: Y coordinate of the top-left corner
//|         :param readablebuffer bmp_data: BMP image data"""
//|         ...
static mp_obj_t rm690b0_rm690b0_blit_bmp(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_obj_t bmp_data = args[3];
    common_hal_rm690b0_rm690b0_blit_bmp(self, x, y, bmp_data);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_blit_bmp_obj, 4, 4, rm690b0_rm690b0_blit_bmp);

//|     def blit_jpeg(self, x: int, y: int, jpeg_data: readablebuffer) -> None:
//|         """Draw a JPEG image from memory.
//|
//|         :param int x: X coordinate of the top-left corner
//|         :param int y: Y coordinate of the top-left corner
//|         :param readablebuffer jpeg_data: JPEG image data"""
//|         ...
static mp_obj_t rm690b0_rm690b0_blit_jpeg(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    mp_obj_t jpeg_data = args[3];
    common_hal_rm690b0_rm690b0_blit_jpeg(self, x, y, jpeg_data);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_blit_jpeg_obj, 4, 4, rm690b0_rm690b0_blit_jpeg);

//|     def convert_bmp(self, src_data: readablebuffer, dest_bitmap: displayio.Bitmap) -> None:
//|         """Convert a BMP file (internal buffer) to a displayio.Bitmap in RGB565 format (RGB/BGR swapped for display).
//|
//|         :param readablebuffer src_data: The full BMP file data
//|         :param displayio.Bitmap dest_bitmap: The destination bitmap (must be correct size)"""
//|         ...
static mp_obj_t rm690b0_rm690b0_convert_bmp(size_t n_args, const mp_obj_t *args) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t src_data = args[1];
    mp_obj_t dest_bitmap = args[2];
    common_hal_rm690b0_rm690b0_convert_bmp(self, src_data, dest_bitmap);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rm690b0_rm690b0_convert_bmp_obj, 3, 3, rm690b0_rm690b0_convert_bmp);


static mp_obj_t rm690b0_rm690b0_blit_buffer(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_x, ARG_y, ARG_width, ARG_height, ARG_bitmap_data, ARG_dest_is_swapped };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_x, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bitmap_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_dest_is_swapped, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
        MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_rm690b0_rm690b0_blit_buffer(self,
        args[ARG_x].u_int,
        args[ARG_y].u_int,
        args[ARG_width].u_int,
        args[ARG_height].u_int,
        args[ARG_bitmap_data].u_obj,
        args[ARG_dest_is_swapped].u_bool
    );
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rm690b0_rm690b0_blit_buffer_obj, 1, rm690b0_rm690b0_blit_buffer);

//|     def swap_buffers(self, copy: bool = True) -> None:
//|         """Swap the front and back framebuffers and display the result
//|
//|         If double-buffering is enabled (two framebuffers allocated), this atomically
//|         swaps the buffer pointers and then flushes the new front buffer to the display.
//|         This provides tear-free rendering for animations and graphics.
//|
//|         If double-buffering is not enabled (single framebuffer), this simply flushes
//|         the current framebuffer to the display, equivalent to a refresh operation.
//|
//|         :param bool copy: If True (default), copies front buffer to back buffer after swap.
//|                           This allows incremental drawing on top of existing content.
//|                           If False, back buffer is left unchanged (better performance for
//|                           full redraws like animations that clear the screen each frame).
//|         """
//|         ...
//|
static mp_obj_t rm690b0_rm690b0_swap_buffers(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_copy };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_copy, MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool copy = args[ARG_copy].u_bool;
    common_hal_rm690b0_rm690b0_swap_buffers(self, copy);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rm690b0_rm690b0_swap_buffers_obj, 1, rm690b0_rm690b0_swap_buffers);

//|     rotation: int
//|     """The rotation of the display as an int in degrees."""
static mp_obj_t rm690b0_rm690b0_get_rotation_prop(mp_obj_t self_in) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(common_hal_rm690b0_rm690b0_get_rotation(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_rm690b0_get_rotation_prop_obj, rm690b0_rm690b0_get_rotation_prop);

static mp_obj_t rm690b0_rm690b0_set_rotation_prop(mp_obj_t self_in, mp_obj_t rotation_obj) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t rotation = mp_obj_get_int(rotation_obj);
    common_hal_rm690b0_rm690b0_set_rotation(self, rotation);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rm690b0_rm690b0_set_rotation_prop_obj, rm690b0_rm690b0_set_rotation_prop);

MP_PROPERTY_GETSET(rm690b0_rm690b0_rotation_obj,
    (mp_obj_t)&rm690b0_rm690b0_get_rotation_prop_obj,
    (mp_obj_t)&rm690b0_rm690b0_set_rotation_prop_obj);




//|     brightness: float
//|     """The brightness of the display, from 0.0 to 1.0"""
static mp_obj_t rm690b0_rm690b0_get_brightness_prop(mp_obj_t self_in) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_rm690b0_rm690b0_get_brightness(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_rm690b0_get_brightness_prop_obj, rm690b0_rm690b0_get_brightness_prop);

static mp_obj_t rm690b0_rm690b0_set_brightness_prop(mp_obj_t self_in, mp_obj_t brightness_obj) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_float_t brightness = mp_obj_get_float(brightness_obj);
    common_hal_rm690b0_rm690b0_set_brightness(self, brightness);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rm690b0_rm690b0_set_brightness_prop_obj, rm690b0_rm690b0_set_brightness_prop);

MP_PROPERTY_GETSET(rm690b0_rm690b0_brightness_obj,
    (mp_obj_t)&rm690b0_rm690b0_get_brightness_prop_obj,
    (mp_obj_t)&rm690b0_rm690b0_set_brightness_prop_obj);

//|     width: int
//|     """Gets the width of the display"""
static mp_obj_t rm690b0_rm690b0_get_width(mp_obj_t self_in) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->width);
}
MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_rm690b0_get_width_obj, rm690b0_rm690b0_get_width);

MP_PROPERTY_GETTER(rm690b0_rm690b0_width_obj,
    (mp_obj_t)&rm690b0_rm690b0_get_width_obj);

//|     height: int
//|     """Gets the height of the display"""
static mp_obj_t rm690b0_rm690b0_get_height(mp_obj_t self_in) {
    rm690b0_rm690b0_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->height);
}
MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_rm690b0_get_height_obj, rm690b0_rm690b0_get_height);

MP_PROPERTY_GETTER(rm690b0_rm690b0_height_obj,
    (mp_obj_t)&rm690b0_rm690b0_get_height_obj);

static const mp_rom_map_elem_t rm690b0_rm690b0_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rm690b0_rm690b0_deinit_obj) },

    { MP_ROM_QSTR(MP_QSTR_init_display), MP_ROM_PTR(&rm690b0_rm690b0_init_display_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_color), MP_ROM_PTR(&rm690b0_rm690b0_fill_color_obj) },

    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&rm690b0_rm690b0_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&rm690b0_rm690b0_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&rm690b0_rm690b0_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_hline), MP_ROM_PTR(&rm690b0_rm690b0_hline_obj) },
    { MP_ROM_QSTR(MP_QSTR_vline), MP_ROM_PTR(&rm690b0_rm690b0_vline_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&rm690b0_rm690b0_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&rm690b0_rm690b0_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&rm690b0_rm690b0_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_circle), MP_ROM_PTR(&rm690b0_rm690b0_fill_circle_obj) },

    // Built-in text and font API
    { MP_ROM_QSTR(MP_QSTR_set_font), MP_ROM_PTR(&rm690b0_rm690b0_set_font_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&rm690b0_rm690b0_text_obj) },

    { MP_ROM_QSTR(MP_QSTR_rotation), MP_ROM_PTR(&rm690b0_rm690b0_rotation_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit_buffer), MP_ROM_PTR(&rm690b0_rm690b0_blit_buffer_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit_bmp), MP_ROM_PTR(&rm690b0_rm690b0_blit_bmp_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit_jpeg), MP_ROM_PTR(&rm690b0_rm690b0_blit_jpeg_obj) },
    { MP_ROM_QSTR(MP_QSTR_convert_bmp), MP_ROM_PTR(&rm690b0_rm690b0_convert_bmp_obj) },
    { MP_ROM_QSTR(MP_QSTR_swap_buffers), MP_ROM_PTR(&rm690b0_rm690b0_swap_buffers_obj) },

    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&rm690b0_rm690b0_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&rm690b0_rm690b0_height_obj) },
};
static MP_DEFINE_CONST_DICT(rm690b0_rm690b0_locals_dict, rm690b0_rm690b0_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    rm690b0_rm690b0_type,
    MP_QSTR_RM690B0,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, rm690b0_rm690b0_make_new,
    locals_dict, &rm690b0_rm690b0_locals_dict
    );
