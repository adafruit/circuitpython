// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"   // brings in the board config (CIRCUITPY_PICOGAME_FAST_DISPLAY)

// The fast DMA Display backend is port-specific (needs a common-hal). Ports without it
// (e.g. ESP32) build picogame with this whole type compiled out and use Scene's portable
// bus.send renderer instead - so picogame stays buildable on any CircuitPython port.
#if CIRCUITPY_PICOGAME_FAST_DISPLAY
#include "shared-bindings/picogame/Display.h"
#include "shared-bindings/picogame/Sprite.h"
#include "shared-bindings/busdisplay/BusDisplay.h"
#include "shared-module/picogame/__init__.h"
#include "common-hal/picogame/Display.h"

//| class Display:
//|     """Fast display backend: wraps an existing ``busdisplay.BusDisplay`` and
//|     pushes pixels with asynchronous double-buffered DMA, overlapping the CPU
//|     blit of the next strip with the SPI transfer of the current one.
//|
//|     Controller- and resolution-agnostic: reuses the busdisplay's SPI bus,
//|     window commands and dimensions."""
//|
//|     def __init__(self, display: busdisplay.BusDisplay, *, rgb444: bool = False) -> None:
//|         """rgb444=True drives the panel in 12-bit RGB444 instead of 16-bit RGB565: ~25% less
//|         SPI traffic (and thus more FPS on full-screen / scrolling, transfer-bound scenes), at
//|         4096 colours instead of 65536 - which PAL8 art doesn't notice. The panel controller
//|         must support COLMOD 12-bit (ST7789/ST7735 do; ILI9341 does NOT)."""
//|         ...
//|
static mp_obj_t picogame_display_make_new(const mp_obj_type_t *type, size_t n_args,
    size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_display, ARG_rgb444 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_display, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rgb444, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t native = mp_obj_cast_to_native_base(args[ARG_display].u_obj, &busdisplay_busdisplay_type);
    if (!mp_obj_is_type(native, &busdisplay_busdisplay_type)) {
        mp_arg_validate_type(native, &busdisplay_busdisplay_type, MP_QSTR_display);
    }
    picogame_display_obj_t *self = mp_obj_malloc(picogame_display_obj_t, type);
    common_hal_picogame_display_construct(self, MP_OBJ_TO_PTR(native), args[ARG_rgb444].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

//|     def render(
//|         self,
//|         sprites: List[Sprite],
//|         buffer_a: WriteableBuffer,
//|         buffer_b: WriteableBuffer,
//|         x0: int,
//|         y0: int,
//|         x1: int,
//|         y1: int,
//|         *,
//|         background: int = 0,
//|     ) -> None:
//|         """Render ``sprites`` into region [x0,x1) x [y0,y1) and push via async
//|         DMA. ``buffer_a``/``buffer_b`` are two equal strip buffers used for
//|         double buffering (each >= region_width*2 bytes).
//|
//|         SPRITES ONLY (unlike module-level ``picogame.render()``, which also accepts
//|         StripDraw/Canvas/Tilemap/Particles): this is the low-level double-buffered
//|         sprite push. For mixed layer kinds use a ``Scene`` or ``picogame.render()``."""
//|         ...
//|
//|
static mp_obj_t picogame_display_render(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sprites, ARG_buffer_a, ARG_buffer_b, ARG_x0, ARG_y0, ARG_x1, ARG_y1, ARG_background };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sprites, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_a, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_buffer_b, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_x0, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y0, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_x1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_y1, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_background, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };
    picogame_display_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t n = 0;
    mp_obj_t *items;
    mp_obj_get_array(args[ARG_sprites].u_obj, &n, &items);
    for (size_t i = 0; i < n; i++) {
        mp_arg_validate_type(items[i], &picogame_sprite_type, MP_QSTR_sprite);
    }

    mp_buffer_info_t ba, bb;
    mp_get_buffer_raise(args[ARG_buffer_a].u_obj, &ba, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[ARG_buffer_b].u_obj, &bb, MP_BUFFER_WRITE);
    size_t buf_pixels = (ba.len < bb.len ? ba.len : bb.len) / 2;

    // kinds == NULL: every item is a sprite (the layered path handles this).
    common_hal_picogame_display_render(self, items, NULL, n,
        (uint16_t *)ba.buf, (uint16_t *)bb.buf, buf_pixels,
        args[ARG_x0].u_int, args[ARG_y0].u_int, args[ARG_x1].u_int, args[ARG_y1].u_int,
        args[ARG_background].u_int, 0, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(picogame_display_render_obj, 8, picogame_display_render);

static const mp_rom_map_elem_t picogame_display_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_render), MP_ROM_PTR(&picogame_display_render_obj) },
};
static MP_DEFINE_CONST_DICT(picogame_display_locals_dict, picogame_display_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    picogame_display_type,
    MP_QSTR_Display,
    MP_TYPE_FLAG_NONE,
    make_new, picogame_display_make_new,
    locals_dict, &picogame_display_locals_dict
    );

#endif // CIRCUITPY_PICOGAME_FAST_DISPLAY
