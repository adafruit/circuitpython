// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objlist.h"
#include "shared-module/displayio/Palette.h"
#include "shared-module/displayio/lvgl/lvgl_protocol.h"

#include "lvgl.h"

// LVGL-based version of displayio Group. The Python-visible state is mirrored into an
// LVGL object tree: a Group maps to a plain lv_obj container whose children are the
// mirrors of its TileGrid and subgroup members. Only the fields below are used by this
// implementation; the scanline compositor's transform/area state lives in
// shared-module/displayio/legacy/Group.h.

typedef struct {
    mp_obj_base_t base;
    mp_obj_list_t *members;
    int16_t x;
    int16_t y;
    uint16_t scale;
    bool item_removed : 1;
    bool in_group : 1;
    bool hidden : 1;
    bool hidden_by_parent : 1;
    bool readonly : 1;
    uint8_t padding : 3;
    // LVGL mirror state.
    lv_obj_t *lv_obj;    // Plain lv_obj container created when the group is attached.
    uint16_t lv_scale;   // Scale inherited from the parent group for child positioning.
    // Absolute position of this group's coordinate origin. Children use it to keep their
    // own absolute position (needed by vectorio's contains()) in sync.
    int32_t lv_abs_x;
    int32_t lv_abs_y;
} displayio_group_t;

void displayio_group_construct(displayio_group_t *self, mp_obj_list_t *members, uint32_t scale, mp_int_t x, mp_int_t y);
void displayio_group_set_hidden_by_parent(displayio_group_t *self, bool hidden);

// Shared LVGL compositor API. Implemented in shared-module/displayio/lvgl/Group.c.
void displayio_lvgl_init(void);
void displayio_lvgl_tick(void); // Run LVGL timers from the background task.
void displayio_lvgl_set_root_group(displayio_group_t *group); // NULL detaches.

// Group mirror lifecycle. Implemented in Group.c and called by the compositor.
void displayio_lvgl_group_attach(displayio_group_t *self, lv_obj_t *parent);
void displayio_lvgl_group_detach(displayio_group_t *self);

// How a parent Group attaches, positions and hides this Group. See lvgl_protocol.h.
extern const lvgl_protocol_t displayio_lvgl_group_protocol;
