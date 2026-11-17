// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// LVGL-based displayio Group implementation. Each Group is mirrored into a plain lv_obj
// container; members are appended as children so the lv object tree's child order matches
// the displayio z-order (later members draw on top). Group scale is applied by scaling
// child positions and images directly. That keeps displayio's integer nearest-neighbor
// scaling and per-child invalidation; LVGL transform styles use an ARGB8888 transform
// layer and bilinear filtering instead.
//
// The compositor pushes every state change into the LVGL mirror synchronously, under the
// port's LVGL lock: LVGL itself renders on an RTOS thread (e.g. a Zephyr workqueue), and
// this keeps tree mutation off the render thread.

#include "shared-bindings/displayio/Group.h"

#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/objlist.h"
#include "shared-bindings/displayio/TileGrid.h"
#include "shared-bindings/displayio/__init__.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "shared-module/displayio/lvgl/lvgl_protocol.h"

#if CIRCUITPY_VIDEOCORE
#include "shared-module/videocore/VideoBuffer.h"
#endif

#if CIRCUITPY_DISPLAYIO_LVGL

// Resolve a member's protocol and, in place, the object it implements the protocol on. Layers
// mirror themselves into the LVGL tree through protocol_lvgl, so nothing here has to know a
// member's type. See lvgl_protocol.h.
static const lvgl_protocol_t *_layer_protocol(mp_obj_t *layer) {
    const lvgl_protocol_t *protocol = mp_proto_get(MP_QSTR_protocol_lvgl, *layer);
    if (protocol != NULL && protocol->protocol_self != NULL) {
        *layer = protocol->protocol_self(*layer);
        protocol = mp_proto_get(MP_QSTR_protocol_lvgl, *layer);
    }
    return protocol;
}

static displayio_group_t *cp_lvgl_root_group = NULL;
static bool cp_lvgl_inited = false;

// The port lock is recursive. Keeping the recursion at the port instead of counting here matters
// once LVGL renders on its own task: a shared depth counter would let the render task believe it
// owns a lock that the CircuitPython task is holding.
void displayio_lvgl_lock(void) {
    displayio_lvgl_port_lock();
}

void displayio_lvgl_unlock(void) {
    displayio_lvgl_port_unlock();
}

static void _unlock_on_exception(void *ctx) {
    (void)ctx;
    displayio_lvgl_unlock();
}

void displayio_lvgl_lock_with_cleanup(nlr_jump_callback_node_t *cleanup) {
    displayio_lvgl_lock();
    nlr_push_jump_callback(cleanup, _unlock_on_exception);
}

void displayio_lvgl_unlock_with_cleanup(void) {
    nlr_pop_jump_callback(true);
}

// Groups are mirrored as transparent, screen sized LVGL objects at the screen origin and
// their members are positioned in screen coordinates (lv_abs_x/y is the group's origin on
// the screen). A group's LVGL object never moves or resizes, so moving a member only
// redraws the member's old and new area. (A group sized to its content would be redrawn
// from its origin to its farthest member whenever a member moves.)
//
// Recompute members' inherited scale and screen positions after this group's scale or
// position changed. Returns false when a member rejects the transform; some members may
// already have taken it then, so callers restore the group's own transform and push again.
static bool _update_children(displayio_group_t *self) {
    uint16_t child_scale = self->lv_scale * self->scale;
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer = self->members->items[i];
        const lvgl_protocol_t *protocol = _layer_protocol(&layer);
        if (protocol == NULL || protocol->set_transform == NULL) {
            continue;
        }
        if (!protocol->set_transform(layer, self->lv_abs_x, self->lv_abs_y, child_scale)) {
            return false;
        }
    }
    return true;
}

static void check_readonly(displayio_group_t *self) {
    if (self->readonly) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Read-only"));
    }
}

// Bitmap writers mark the dirty area before writing, and TileGrid writes only set bits, so the
// composed buffers are brought up to date here: at the start of a refresh, after the writes and
// before anything is drawn. The lock is held, and LVGL doesn't render while this runs.
static void refresh_start_cb(lv_event_t *e) {
    (void)e;
    // Take any video frames that landed since the last background tick, so a
    // manual display.refresh() shows them too.
    #if CIRCUITPY_VIDEOCORE
    displayio_lvgl_video_background();
    #endif
    displayio_lvgl_tilegrid_refresh_all();
}

void displayio_lvgl_register_display(lv_display_t *display) {
    if (display == NULL) {
        return;
    }
    displayio_lvgl_lock();
    // Match the legacy compositor, where undrawn pixels are zero (black). LVGL draws its bottom
    // layer -- white, out of the box -- wherever nothing else covers an area, so the screen has to
    // be opaque black. This belongs to each display rather than the one-time init: displays are
    // deleted and re-created on every soft reload (and by the display classes themselves), and a
    // screen without this style comes up white.
    lv_obj_t *screen = lv_display_get_screen_active(display);
    if (screen != NULL) {
        lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
        lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    }
    lv_display_add_event_cb(display, refresh_start_cb, LV_EVENT_REFR_START, NULL);
    displayio_lvgl_unlock();
}

// Whether any active display mirrored into LVGL still wants automatic repaints. The display slots
// are the shared registry (the same ones the legacy compositor uses), so this works for both
// display classes the compositor supports.
bool displayio_lvgl_auto_refresh(void) {
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t display_type = displays[i].display_base.type;
        if (display_type == NULL || display_type == &mp_type_NoneType) {
            continue;
        }
        #if CIRCUITPY_BUSDISPLAY
        if (display_type == &busdisplay_busdisplay_type) {
            if (displays[i].display.auto_refresh) {
                return true;
            }
            continue;
        }
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        if (display_type == &framebufferio_framebufferdisplay_type) {
            if (displays[i].framebuffer_display.auto_refresh) {
                return true;
            }
            continue;
        }
        #endif
    }
    // No display yet: pumping LVGL is harmless and keeps its own timers running.
    return true;
}

// Render the displays that can't run on the render task. The guard keeps the proxy from
// re-entering LVGL rendering if a background callback is processed while a refresh is already
// active on the CircuitPython thread.
void displayio_lvgl_refresh_unsafe_displays(void) {
    #if CIRCUITPY_BUSDISPLAY
    static bool refreshing = false;
    if (refreshing) {
        return;
    }
    refreshing = true;
    displayio_lvgl_lock();
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t display_type = displays[i].display_base.type;
        if (display_type != &busdisplay_busdisplay_type) {
            continue;
        }
        busdisplay_busdisplay_obj_t *busdisplay = &displays[i].display;
        if (busdisplay->auto_refresh && busdisplay->lv_display != NULL) {
            lv_refr_now(busdisplay->lv_display);
        }
    }
    displayio_lvgl_unlock();
    refreshing = false;
    #endif
}

// Render the framebuffer displays that pace themselves from a frame boundary. Like the BusDisplay
// proxy, this runs on the CircuitPython thread and takes the LVGL lock; a frame callback may be
// running on the render task at the same time, so the lock serializes them.
void displayio_lvgl_refresh_paced_displays(void) {
    #if CIRCUITPY_FRAMEBUFFERIO
    static bool refreshing = false;
    if (refreshing) {
        return;
    }
    refreshing = true;
    displayio_lvgl_lock();
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        mp_const_obj_t display_type = displays[i].display_base.type;
        if (display_type != &framebufferio_framebufferdisplay_type) {
            continue;
        }
        framebufferio_framebufferdisplay_obj_t *display = &displays[i].framebuffer_display;
        if (display->lv_paced && display->auto_refresh && display->lv_display != NULL) {
            lv_refr_now(display->lv_display);
        }
    }
    displayio_lvgl_unlock();
    refreshing = false;
    #endif
}

void displayio_lvgl_init(void) {
    if (cp_lvgl_inited) {
        return;
    }
    displayio_lvgl_lock();
    // The OnDiskBitmap decoder has to be in place before any TileGrid attaches. Once a display
    // exists the compositor is in use; until then leave this unset so the decoder is initialized
    // by the display that does show up.
    if (lv_screen_active() != NULL) {
        displayio_lvgl_ondiskbitmap_decoder_init();
        cp_lvgl_inited = true;
    }
    displayio_lvgl_unlock();
}

void displayio_group_construct(displayio_group_t *self, mp_obj_list_t *members, uint32_t scale, mp_int_t x, mp_int_t y) {
    self->x = x;
    self->y = y;
    self->members = members;
    self->item_removed = false;
    self->scale = scale;
    self->in_group = false;
    self->readonly = false;
    self->lv_obj = NULL;
    self->lv_scale = 1;
    self->lv_abs_x = x;
    self->lv_abs_y = y;
}

void common_hal_displayio_group_construct(displayio_group_t *self, uint32_t scale, mp_int_t x, mp_int_t y) {
    mp_obj_list_t *members = mp_obj_new_list(0, NULL);
    displayio_group_construct(self, members, scale, x, y);
}

bool common_hal_displayio_group_get_hidden(displayio_group_t *self) {
    return self->hidden;
}

// Update the mirror object for a group's effective visibility.
static void _update_lvgl_visible(displayio_group_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    if (self->hidden || self->hidden_by_parent) {
        lv_obj_add_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void _propagate_hidden(displayio_group_t *self, bool hidden) {
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer = self->members->items[i];
        const lvgl_protocol_t *protocol = _layer_protocol(&layer);
        if (protocol != NULL && protocol->set_hidden_by_parent != NULL) {
            protocol->set_hidden_by_parent(layer, hidden);
        }
    }
}

void common_hal_displayio_group_set_hidden(displayio_group_t *self, bool hidden) {
    if (self->hidden == hidden) {
        return;
    }
    check_readonly(self);
    self->hidden = hidden;
    if (self->hidden_by_parent) {
        return;
    }
    _propagate_hidden(self, hidden);
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    _update_lvgl_visible(self);
    displayio_lvgl_unlock_with_cleanup();
}

void displayio_group_set_hidden_by_parent(displayio_group_t *self, bool hidden) {
    if (self->hidden_by_parent == hidden) {
        return;
    }
    check_readonly(self);
    self->hidden_by_parent = hidden;
    if (self->hidden) {
        return;
    }
    _propagate_hidden(self, hidden);
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    _update_lvgl_visible(self);
    displayio_lvgl_unlock_with_cleanup();
}

uint32_t common_hal_displayio_group_get_scale(displayio_group_t *self) {
    return self->scale;
}

void common_hal_displayio_group_set_scale(displayio_group_t *self, uint32_t scale) {
    if (self->scale == scale) {
        return;
    }
    check_readonly(self);
    if (self->lv_obj == NULL) {
        self->scale = scale;
        return;
    }
    uint16_t previous_scale = self->scale;
    self->scale = scale;
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    bool accepted = _update_children(self);
    if (!accepted) {
        // A member can't be shown at this scale. Put the group back the way it was, push that
        // to the members that already took the new scale, and report it.
        self->scale = previous_scale;
        _update_children(self);
    }
    displayio_lvgl_unlock_with_cleanup();
    if (!accepted) {
        mp_raise_ValueError(MP_ERROR_TEXT("Layer does not support scaling"));
    }
}

mp_int_t common_hal_displayio_group_get_x(displayio_group_t *self) {
    return self->x;
}

void common_hal_displayio_group_set_x(displayio_group_t *self, mp_int_t x) {
    if (self->x == x) {
        return;
    }
    check_readonly(self);
    // The origin is in screen pixels, so the move is scaled by the parent chain.
    self->lv_abs_x += (x - self->x) * self->lv_scale;
    self->x = x;
    if (self->lv_obj != NULL) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        // Every layer can be translated; only set_scale() can be rejected.
        (void)_update_children(self);
        displayio_lvgl_unlock_with_cleanup();
    }
}

mp_int_t common_hal_displayio_group_get_y(displayio_group_t *self) {
    return self->y;
}

void common_hal_displayio_group_set_y(displayio_group_t *self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    check_readonly(self);
    // The origin is in screen pixels, so the move is scaled by the parent chain.
    self->lv_abs_y += (y - self->y) * self->lv_scale;
    self->y = y;
    if (self->lv_obj != NULL) {
        nlr_jump_callback_node_t cleanup;
        displayio_lvgl_lock_with_cleanup(&cleanup);
        // Every layer can be translated; only set_scale() can be rejected.
        (void)_update_children(self);
        displayio_lvgl_unlock_with_cleanup();
    }
}

// Resolve a group member to its mirror object, or NULL if it has none yet.
static lv_obj_t *_member_mirror(mp_obj_t layer) {
    const lvgl_protocol_t *protocol = _layer_protocol(&layer);
    return protocol == NULL ? NULL : protocol->root_obj(layer);
}

// Create the mirror for a member that doesn't have one yet. Unsupported layers stay NULL.
static lv_obj_t *_attach_member(displayio_group_t *self, mp_obj_t layer) {
    const lvgl_protocol_t *protocol = _layer_protocol(&layer);
    if (protocol == NULL) {
        return NULL;
    }
    protocol->attach(layer, self->lv_obj);
    return protocol->root_obj(layer);
}

// Adopt a member: validate that it can join, update Python-visible state, and push it into
// the mirror if the group is mirrored.
static void _adopt_member(displayio_group_t *self, mp_obj_t layer) {
    const lvgl_protocol_t *protocol = _layer_protocol(&layer);
    if (protocol == NULL || protocol->set_in_group == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("Layer must be a Group, TileGrid, vector shape or VideoBuffer"));
    }
    if (protocol->set_in_group(layer, true)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Layer already in a group"));
    }
    // Transform/visibility updates can raise too (for example when resuming video).
    // A rejected adoption must not leave an otherwise unused layer marked in_group.
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (protocol->set_transform != NULL &&
            !protocol->set_transform(layer, self->lv_abs_x, self->lv_abs_y,
                self->lv_scale * self->scale)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Layer does not support scaling"));
        }
        if (protocol->set_hidden_by_parent != NULL) {
            protocol->set_hidden_by_parent(layer, self->hidden || self->hidden_by_parent);
        }
        nlr_pop();
    } else {
        protocol->set_in_group(layer, false);
        nlr_jump(nlr.ret_val);
    }
}

// Release a member and remove its mirror. Must be called under the displayio lock.
static void _release_member(displayio_group_t *self, mp_obj_t layer) {
    const lvgl_protocol_t *protocol = _layer_protocol(&layer);
    if (protocol == NULL) {
        return;
    }
    if (protocol->set_in_group != NULL) {
        protocol->set_in_group(layer, false);
    }
    protocol->detach(layer);
}

// Push the mirror children into the same order as the members list. Newly added members
// get mirrors here; children that no longer belong to a member are deleted. Must be
// called under the displayio lock.
static void _sync_members(displayio_group_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    size_t child_index = 0;
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer = self->members->items[i];
        lv_obj_t *obj = _member_mirror(layer);
        if (obj == NULL) {
            // Not mirrored yet: try to attach it now (unsupported layers stay NULL).
            obj = _attach_member(self, layer);
        }
        if (obj == NULL) {
            continue;
        }
        if (lv_obj_get_parent(obj) != self->lv_obj) {
            lv_obj_set_parent(obj, self->lv_obj);
        }
        if (lv_obj_get_index(obj) != (int32_t)child_index) {
            lv_obj_move_to_index(obj, child_index);
        }
        child_index++;
    }
    // Delete mirrors of members that were removed from the group.
    while ((uint32_t)lv_obj_get_child_count(self->lv_obj) > child_index) {
        lv_obj_t *child = lv_obj_get_child(self->lv_obj, (int32_t)(lv_obj_get_child_count(self->lv_obj) - 1));
        lv_obj_delete(child);
    }
    // Newly attached members need their position, scale and absolute offset pushed in. Every
    // member already accepted this transform in _adopt_member().
    (void)_update_children(self);
}

void common_hal_displayio_group_insert(displayio_group_t *self, size_t index, mp_obj_t layer) {
    check_readonly(self);
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    _adopt_member(self, layer);
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_list_insert(self->members, index, layer);
        nlr_pop();
    } else {
        mp_obj_t native = layer;
        const lvgl_protocol_t *protocol = _layer_protocol(&native);
        protocol->set_in_group(native, false);
        nlr_jump(nlr.ret_val);
    }
    if (self->lv_obj != NULL) {
        _sync_members(self);
    }
    displayio_lvgl_unlock_with_cleanup();
}

mp_obj_t common_hal_displayio_group_pop(displayio_group_t *self, size_t index) {
    check_readonly(self);
    mp_obj_t layer = self->members->items[index];
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    _release_member(self, layer);
    mp_obj_list_pop(self->members, index);
    if (self->lv_obj != NULL) {
        _sync_members(self);
    }
    displayio_lvgl_unlock_with_cleanup();
    return layer;
}

mp_int_t common_hal_displayio_group_index(displayio_group_t *self, mp_obj_t layer) {
    mp_obj_t args[] = {self->members, layer};
    mp_obj_t *index = mp_seq_index_obj(self->members->items, self->members->len, 2, args);
    return MP_OBJ_SMALL_INT_VALUE(index);
}

size_t common_hal_displayio_group_get_len(displayio_group_t *self) {
    return self->members->len;
}

mp_obj_t common_hal_displayio_group_get(displayio_group_t *self, size_t index) {
    return self->members->items[index];
}

void common_hal_displayio_group_set(displayio_group_t *self, size_t index, mp_obj_t layer) {
    check_readonly(self);
    mp_obj_t previous = self->members->items[index];
    if (previous == layer) {
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    // Validate the replacement before detaching the current member. Failed assignment
    // must leave both the Python list and its LVGL mirrors intact.
    _adopt_member(self, layer);
    _release_member(self, previous);
    mp_obj_list_store(self->members, MP_OBJ_NEW_SMALL_INT(index), layer);
    if (self->lv_obj != NULL) {
        _sync_members(self);
    }
    displayio_lvgl_unlock_with_cleanup();
}

// protocol_lvgl implementation. A Group is a member of another Group like any other layer;
// its own position and scale are inherited, so it recomputes its origin from the parent's.
//
// A Python subclass of Group (PageLayout, a Tile, ...) is an mp_obj_instance_t that holds the
// native Group in subobj[0], so every function below has to see that native object instead of
// the instance. Resolving it here keeps the type slot on the native type.
static mp_obj_t _group_protocol_self(mp_obj_t layer) {
    mp_obj_t native = mp_obj_cast_to_native_base(layer, MP_OBJ_FROM_PTR(&displayio_group_type));
    if (native == MP_OBJ_NULL) {
        return layer;
    }
    mp_obj_assert_native_inited(native);
    return native;
}

static lv_obj_t *_group_protocol_root_obj(mp_obj_t layer) {
    return ((displayio_group_t *)MP_OBJ_TO_PTR(layer))->lv_obj;
}

static void _group_protocol_attach(mp_obj_t layer, lv_obj_t *parent) {
    displayio_lvgl_group_attach(MP_OBJ_TO_PTR(layer), parent);
}

static void _group_protocol_detach(mp_obj_t layer) {
    displayio_lvgl_group_detach(MP_OBJ_TO_PTR(layer));
}

static bool _group_protocol_set_transform(mp_obj_t layer, int32_t abs_x, int32_t abs_y,
    uint16_t scale) {
    displayio_group_t *self = MP_OBJ_TO_PTR(layer);
    self->lv_scale = scale;
    // abs_x/y is where this group's parent origin is on the screen; shift by this group's
    // own position, scaled the same way as its members are.
    self->lv_abs_x = abs_x + self->x * scale;
    self->lv_abs_y = abs_y + self->y * scale;
    if (self->lv_obj == NULL) {
        return true;
    }
    return _update_children(self);
}

static void _group_protocol_set_hidden_by_parent(mp_obj_t layer, bool hidden) {
    displayio_group_set_hidden_by_parent(MP_OBJ_TO_PTR(layer), hidden);
}

static bool _group_protocol_set_in_group(mp_obj_t layer, bool in_group) {
    displayio_group_t *self = MP_OBJ_TO_PTR(layer);
    bool was_in_group = self->in_group;
    self->in_group = in_group;
    return was_in_group;
}

const lvgl_protocol_t displayio_lvgl_group_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_lvgl)
    .protocol_self = _group_protocol_self,
    .root_obj = _group_protocol_root_obj,
    .attach = _group_protocol_attach,
    .detach = _group_protocol_detach,
    .set_transform = _group_protocol_set_transform,
    .set_hidden_by_parent = _group_protocol_set_hidden_by_parent,
    .set_in_group = _group_protocol_set_in_group,
};

void displayio_lvgl_group_attach(displayio_group_t *self, lv_obj_t *parent) {
    if (self->lv_obj == NULL) {
        self->lv_obj = lv_obj_create(parent);
        if (self->lv_obj == NULL) {
            return;
        }
        // Plain, transparent, screen sized container at the screen origin. See
        // _update_children() for why it doesn't follow the group's position.
        lv_obj_remove_style_all(self->lv_obj);
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(self->lv_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(self->lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
        lv_obj_set_size(self->lv_obj, lv_pct(100), lv_pct(100));
        lv_obj_set_pos(self->lv_obj, 0, 0);
        _update_lvgl_visible(self);
    } else if (lv_obj_get_parent(self->lv_obj) != parent) {
        lv_obj_set_parent(self->lv_obj, parent);
    }
    _sync_members(self);
}

void displayio_lvgl_group_detach(displayio_group_t *self) {
    if (self->lv_obj == NULL) {
        return;
    }
    // Detach the members first so their object pointers don't dangle when the lv tree
    // is deleted below us.
    for (size_t i = 0; i < self->members->len; i++) {
        mp_obj_t layer = self->members->items[i];
        const lvgl_protocol_t *protocol = _layer_protocol(&layer);
        if (protocol != NULL) {
            protocol->detach(layer);
        }
    }
    lv_obj_delete(self->lv_obj);
    self->lv_obj = NULL;
}

void displayio_lvgl_set_root_group(displayio_group_t *group) {
    displayio_lvgl_init();
    if (!cp_lvgl_inited) {
        // The port never created the LVGL screen (no display); nothing to mirror.
        return;
    }
    nlr_jump_callback_node_t cleanup;
    displayio_lvgl_lock_with_cleanup(&cleanup);
    if (cp_lvgl_root_group == group) {
        displayio_lvgl_unlock_with_cleanup();
        return;
    }
    if (cp_lvgl_root_group != NULL) {
        displayio_lvgl_group_detach(cp_lvgl_root_group);
    }
    cp_lvgl_root_group = group;
    if (cp_lvgl_root_group != NULL) {
        // The root group's origin is its own position.
        group->lv_scale = 1;
        group->lv_abs_x = group->x;
        group->lv_abs_y = group->y;
        displayio_lvgl_group_attach(cp_lvgl_root_group, lv_screen_active());
    }
    // Repaint the whole screen. The incoming tree may not cover everything the outgoing one did
    // (the terminal leaves the top row for its status bar and logo), so the screen background has
    // to replace the old content there instead of leaving it on the display.
    lv_obj_t *screen = lv_screen_active();
    if (screen != NULL) {
        lv_obj_invalidate(screen);
    }
    displayio_lvgl_unlock_with_cleanup();
}

void displayio_gc_collect(void) {
    void *root_ptrs[1] = { cp_lvgl_root_group };
    if (root_ptrs[0] != NULL) {
        gc_collect_root(root_ptrs, 1);
    }
    // The shader registry holds vectorio pointers in port memory; root them explicitly.
    displayio_lvgl_mirror_gc();
    // So does the compositor's list of attached TileGrids.
    displayio_lvgl_tilegrid_gc_collect();
    // Displays live in the static slot registry, which the GC can't see.
    displayio_display_slots_gc_collect();
    #if CIRCUITPY_VIDEOCORE
    // So does the compositor's list of attached VideoBuffer mirrors.
    displayio_lvgl_video_gc_collect();
    #endif
}

#endif  // CIRCUITPY_DISPLAYIO_LVGL
