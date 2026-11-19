// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

#include "lvgl.h"

// Everything an LVGL-backed displayio layer has to provide to displayio.Group, independent
// of its type. This is the LVGL counterpart of the scanline compositor's protocol_draw (see
// shared-bindings/vectorio/__init__.h): Group mirrors its members into the LVGL object tree
// through these calls and nothing else in the compositor needs to know what a member is.
//
// The layer owns its mirror. Once attached, LVGL owns the lv_obj, so a layer never sets the
// mirror's parent or z-order itself; Group does that through root_obj().
//
// A type has a single protocol slot, so a layer implements either this (LVGL builds) or
// protocol_draw (legacy builds), never both. Each backend's dispatcher header picks the
// right object with a *_LVGL_PROTOCOL macro; it is NULL in legacy builds.
//
// Group calls every method above with the LVGL lock held, and passes the object protocol_self
// resolved. Implementations may take the lock again (it is depth counted).
typedef struct _lvgl_protocol_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_lvgl

    // The object the rest of the calls are made on, or the layer itself when the protocol is
    // implemented on the layer. This must resolve two things: vectorio's shapes (the
    // Python-visible Rectangle, Circle or Polygon owns the wrapper that holds the shape's
    // compositor state, so it forwards to that wrapper) and Python subclasses of a layer (a
    // PageLayout, Tile or Label is an mp_obj_instance_t whose native base is in subobj[0]).
    // May be NULL.
    mp_obj_t (*protocol_self)(mp_obj_t self);

    // The layer's root lv_obj, or NULL before attach(). Group uses it for z-order and
    // reparenting.
    lv_obj_t *(*root_obj)(mp_obj_t self);

    // Create the mirror under parent if needed, reparent it, and push every Python-visible
    // property into it.
    void (*attach)(mp_obj_t self, lv_obj_t *parent);

    // Delete the mirror and any port allocations behind it, and clear root_obj. Must leave
    // the layer reusable: a later attach() rebuilds the mirror. Does not touch group state.
    void (*detach)(mp_obj_t self);

    // Push the inherited transform. abs_x/abs_y is the parent group's origin in screen
    // pixels and scale is the composed scale of the whole ancestor chain; this layer's own
    // position and scale are not applied here. Return false when the layer can't be shown
    // that way (a VideoBuffer can't be scaled, for example); Group then restores its own
    // transform, pushes that again, and raises ValueError.
    bool (*set_transform)(mp_obj_t self, int32_t abs_x, int32_t abs_y, uint16_t scale);

    // Propagate an ancestor's hidden state. NULL means always drawn while attached.
    void (*set_hidden_by_parent)(mp_obj_t self, bool hidden);

    // Set or clear group membership, returning the previous value so Group can reject a
    // layer that is already in one. NULL means the layer can't be a group member.
    bool (*set_in_group)(mp_obj_t self, bool in_group);
} lvgl_protocol_t;
