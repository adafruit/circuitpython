// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/Palette.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-module/displayio/ColorConverter.h"
#include "shared-module/displayio/lvgl/Bitmap.h"
#include "shared-module/displayio/lvgl/OnDiskBitmap.h"
#include "shared-module/displayio/lvgl/Palette.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"

#if CIRCUITPY_VECTORIO
#include "shared-module/vectorio/VectorShape.h"
#endif

// --- The shader registry -----------------------------------------------------
//
// Bitmaps keep their views in the Bitmap itself, but Palettes are shared and may be `const`
// statistics, so whoever draws with one registers in this port-memory registry keyed by the
// shader object. A Palette change then walks the list and updates each mirror.

typedef struct {
    void *shader; // displayio Palette or ColorConverter object.
    displayio_lvgl_mirror_user_t *users;
    // LVGL's view of a Palette's colors, shared by every image drawing with it. NULL for
    // ColorConverters and until an indexed image needs it.
    lv_color32_t *colors;
    struct displayio_lvgl_mirror_entry *next; // as displayio_lvgl_mirror_entry_t
} displayio_lvgl_mirror_entry_t;

static displayio_lvgl_mirror_entry_t *mirror_registry = NULL;

static displayio_lvgl_mirror_entry_t *_mirror_entry(void *shader) {
    for (displayio_lvgl_mirror_entry_t *entry = mirror_registry; entry != NULL; entry = (displayio_lvgl_mirror_entry_t *)entry->next) {
        if (entry->shader == shader) {
            return entry;
        }
    }
    return NULL;
}

void displayio_lvgl_shader_register_user(mp_obj_t pixel_shader, void *user, uint8_t kind) {
    void *shader = MP_OBJ_TO_PTR(pixel_shader);
    displayio_lvgl_mirror_entry_t *entry = _mirror_entry(shader);
    if (entry == NULL) {
        entry = lv_malloc(sizeof(displayio_lvgl_mirror_entry_t));
        if (entry == NULL) {
            return;
        }
        entry->shader = shader;
        entry->users = NULL;
        entry->colors = NULL;
        entry->next = NULL;
        displayio_lvgl_mirror_entry_t **link = (displayio_lvgl_mirror_entry_t **)&mirror_registry;
        while (*link != NULL) {
            link = (displayio_lvgl_mirror_entry_t **)&(*link)->next;
        }
        *link = entry;
    }
    for (displayio_lvgl_mirror_user_t *existing = entry->users; existing != NULL; existing = existing->next) {
        if (existing->user == user) {
            return;
        }
    }
    displayio_lvgl_mirror_user_t *new_user = lv_malloc(sizeof(displayio_lvgl_mirror_user_t));
    if (new_user == NULL) {
        return;
    }
    new_user->user = user;
    new_user->kind = kind;
    new_user->next = entry->users;
    entry->users = new_user;
}

void displayio_lvgl_shader_unregister_user(mp_obj_t pixel_shader, void *user) {
    displayio_lvgl_mirror_entry_t *entry = _mirror_entry(MP_OBJ_TO_PTR(pixel_shader));
    if (entry == NULL) {
        return;
    }
    displayio_lvgl_mirror_user_t **link = &entry->users;
    while (*link != NULL) {
        if ((*link)->user == user) {
            displayio_lvgl_mirror_user_t *found = *link;
            *link = found->next;
            lv_free(found);
            break;
        }
        link = &(*link)->next;
    }
    if (entry->users == NULL) {
        // Walk with entry-typed links to remove the entry itself.
        displayio_lvgl_mirror_entry_t **prev_entry_link = &mirror_registry;
        while (*prev_entry_link != entry) {
            prev_entry_link = (displayio_lvgl_mirror_entry_t **)&(*prev_entry_link)->next;
        }
        *prev_entry_link = (displayio_lvgl_mirror_entry_t *)entry->next;
        lv_free(entry->colors);
        lv_free(entry);
    }
}

void displayio_lvgl_shader_notify(mp_obj_t pixel_shader) {
    displayio_lvgl_mirror_entry_t *entry = _mirror_entry(MP_OBJ_TO_PTR(pixel_shader));
    if (entry == NULL) {
        return;
    }
    displayio_lvgl_lock();
    if (entry->colors != NULL && mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
        displayio_lvgl_palette_fill(MP_OBJ_TO_PTR(pixel_shader), entry->colors,
            LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I8));
    }
    for (displayio_lvgl_mirror_user_t *user = entry->users; user != NULL; user = user->next) {
        switch (user->kind) {
            case DISPLAYIO_LVGL_USER_TILEGRID:
                displayio_lvgl_tilegrid_shader_changed(user->user);
                break;
            case DISPLAYIO_LVGL_USER_ONDISKBITMAP:
                displayio_ondiskbitmap_lv_palette_changed(user->user);
                break;
            #if CIRCUITPY_VECTORIO
            case DISPLAYIO_LVGL_USER_VECTOR_SHAPE:
                displayio_lvgl_vector_shape_shader_changed(user->user);
                break;
            #endif
            default:
                break;
        }
    }
    displayio_lvgl_unlock();
}

void displayio_lvgl_mirror_gc(void) {
    for (displayio_lvgl_mirror_entry_t *entry = mirror_registry; entry != NULL; entry = (displayio_lvgl_mirror_entry_t *)entry->next) {
        for (displayio_lvgl_mirror_user_t *user = entry->users; user != NULL; user = user->next) {
            if (user->kind != DISPLAYIO_LVGL_USER_VECTOR_SHAPE &&
                user->kind != DISPLAYIO_LVGL_USER_TILEGRID) {
                // OnDiskBitmaps live in port memory and hold no Python objects of their own.
                continue;
            }
            gc_collect_ptr(user->user);
        }
    }
}

// --- Palette -----------------------------------------------------------------

void displayio_lvgl_palette_fill(displayio_palette_t *palette, lv_color32_t *colors,
    uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (i >= palette->color_count || palette->colors[i].transparent) {
            colors[i] = (lv_color32_t) {0};
            continue;
        }
        uint32_t rgb888 = palette->colors[i].rgb888;
        colors[i] = lv_color32_make((rgb888 >> 16) & 0xff, (rgb888 >> 8) & 0xff, rgb888 & 0xff,
            LV_OPA_COVER);
    }
}

const lv_color32_t *displayio_lvgl_shader_palette(mp_obj_t pixel_shader) {
    displayio_lvgl_mirror_entry_t *entry = _mirror_entry(MP_OBJ_TO_PTR(pixel_shader));
    if (entry == NULL || !mp_obj_is_type(pixel_shader, &displayio_palette_type)) {
        return NULL;
    }
    if (entry->colors == NULL) {
        // 256 entries (the largest indexed palette) so the address stays stable when the
        // same Palette is used with bitmaps of different depths.
        entry->colors = lv_malloc(LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I8) * sizeof(lv_color32_t));
        if (entry->colors == NULL) {
            return NULL;
        }
        displayio_lvgl_palette_fill(MP_OBJ_TO_PTR(pixel_shader), entry->colors,
            LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I8));
    }
    return entry->colors;
}

void common_hal_displayio_palette_construct(displayio_palette_t *self, uint16_t color_count, bool dither) {
    self->color_count = color_count;
    self->colors = (_displayio_color_t *)m_malloc_without_collect(color_count * sizeof(_displayio_color_t));
    self->dither = dither;
}

void common_hal_displayio_palette_set_dither(displayio_palette_t *self, bool dither) {
    self->dither = dither;
}

bool common_hal_displayio_palette_get_dither(displayio_palette_t *self) {
    return self->dither;
}

void common_hal_displayio_palette_make_opaque(displayio_palette_t *self, uint32_t palette_index) {
    self->colors[palette_index].transparent = false;
    self->needs_refresh = true;
    displayio_lvgl_shader_notify(MP_OBJ_FROM_PTR(self));
}

void common_hal_displayio_palette_make_transparent(displayio_palette_t *self, uint32_t palette_index) {
    self->colors[palette_index].transparent = true;
    self->needs_refresh = true;
    displayio_lvgl_shader_notify(MP_OBJ_FROM_PTR(self));
}

bool common_hal_displayio_palette_is_transparent(displayio_palette_t *self, uint32_t palette_index) {
    return self->colors[palette_index].transparent;
}

uint32_t common_hal_displayio_palette_get_len(displayio_palette_t *self) {
    return self->color_count;
}

void common_hal_displayio_palette_set_color(displayio_palette_t *self, uint32_t palette_index, uint32_t color) {
    if (self->colors[palette_index].rgb888 == color) {
        return;
    }
    self->colors[palette_index].rgb888 = color;
    self->colors[palette_index].cached_colorspace = NULL;
    self->needs_refresh = true;
    displayio_lvgl_shader_notify(MP_OBJ_FROM_PTR(self));
}

uint32_t common_hal_displayio_palette_get_color(displayio_palette_t *self, uint32_t palette_index) {
    return self->colors[palette_index].rgb888;
}

bool displayio_palette_needs_refresh(displayio_palette_t *self) {
    return self->needs_refresh;
}

void displayio_palette_finish_refresh(displayio_palette_t *self) {
    self->needs_refresh = false;
}
