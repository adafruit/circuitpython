// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Bitmap.h"
#include "shared-module/displayio/Palette.h"

#include "lvgl.h"

// Palette changes notify the LVGL images that draw with it. Those mirrors register here;
// Bitmaps keep pointers to their views themselves. The objects may be `const` statistics (the
// supervisor's generated splash resources), so the registry lives in port memory rather
// than in them.

// What a registered user is, which decides how a Palette change reaches it.
typedef enum {
    DISPLAYIO_LVGL_USER_ONDISKBITMAP = 0, // user is a displayio_ondiskbitmap_t.
    DISPLAYIO_LVGL_USER_TILEGRID,         // user is a displayio_tilegrid_t.
    DISPLAYIO_LVGL_USER_VECTOR_SHAPE,     // user is a vectorio_vector_shape_t.
} displayio_lvgl_user_kind_t;

typedef struct displayio_lvgl_mirror_user {
    void *user;
    uint8_t kind; // displayio_lvgl_user_kind_t
    struct displayio_lvgl_mirror_user *next;
} displayio_lvgl_mirror_user_t;

void displayio_lvgl_shader_register_user(mp_obj_t pixel_shader, void *user, uint8_t kind);
void displayio_lvgl_shader_unregister_user(mp_obj_t pixel_shader, void *user);
// The pixel shader's content changed: every mirror that draws with it has to update.
void displayio_lvgl_shader_notify(mp_obj_t pixel_shader);
// Root the registry's user pointers during GC.
void displayio_lvgl_mirror_gc(void);

// Write count LVGL palette entries for palette. Entries past its end and transparent ones
// are fully transparent, which leaves those pixels to the layers below.
void displayio_lvgl_palette_fill(displayio_palette_t *palette, lv_color32_t *colors,
    uint32_t count);

// The shared LVGL palette of `pixel_shader`, allocated and filled on first use. It lives in
// the shader's registry entry, so it is shared by every image drawing with the Palette and
// released when the last of them unregisters. Returns NULL for a non-Palette shader or when
// there is no memory. Call after registering a user.
const lv_color32_t *displayio_lvgl_shader_palette(mp_obj_t pixel_shader);
