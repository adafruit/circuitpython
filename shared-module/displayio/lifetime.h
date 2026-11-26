// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// These links follow mp_obj_base_t in each native object. The lists are not
// scanned by the GC: only the current VM's proxies hold Python references.
//
// Every display and bus shares this protocol. Display-only methods are NULL
// for buses; background also identifies a native object as a display.
typedef struct {
    // Permanently release the hardware. Called from finalizers and from VM
    // teardown for objects that do not survive. It may wait, but it must not
    // propagate an exception.
    void (*deinit)(void *self);
    // Whether VM-owned dependencies can move between native and VM storage.
    // Displays and their native buses do not move themselves. Displays use
    // this for resources such as backlights; buses use it for wrapped buses.
    bool (*is_moveable)(void *self);
    // Allocate VM-owned references for a new VM.
    void *(*start_vm)(void *self);
    // Stop the current VM for an object that survives: detach VM-owned
    // references (root groups become None, or the console for the primary)
    // and return false if the object can no longer be used.
    bool (*stop_vm)(void *self);
    // Background refresh work. NULL for buses.
    void (*background)(void *self);
} displayio_protocol_t;

// Displays with no legacy background work still set background to this so the
// protocol can distinguish displays from buses.
void displayio_display_background_noop(void *self);

typedef struct {
    mp_obj_t next;
    #if CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
    bool deinited;
    #else
    mp_obj_t proxy;
    #endif
    const displayio_protocol_t *protocol;
    bool static_allocation;
} displayio_native_link_t;

typedef struct {
    displayio_native_link_t native;
    mp_obj_t bus;
    #if CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
    // VM-created displays are GC objects, so the group reference is scanned
    // directly. Board displays are static and have it rooted explicitly.
    mp_obj_t root_group;
    #endif
    bool board;
} displayio_display_link_t;

typedef displayio_native_link_t displayio_bus_lifetime_t;

#if !CIRCUITPY_DISPLAY_LIFETIME_SIMPLE
// Python sees one shared, same-type proxy for every display and bus. native is
// MP_OBJ_NULL after release, making finalization and deinitialization
// idempotent. root_group keeps a user-assigned Group alive for this VM only.
// vm is an opaque, implementation-owned VM-heap object containing the native
// object's other GC-visible references.
typedef struct {
    mp_obj_base_t base;
    mp_obj_t native;
    mp_obj_t bus;
    mp_obj_t root_group;
    void *vm;
} displayio_proxy_t;
#endif

// Prepare a native object and its lifetime link. If native is NULL, allocate
// size bytes from the port heap. Displays pass their bus; buses pass
// MP_OBJ_NULL. Constructors call displayio_register() as their last step.
// It is idempotent so bindings can use it to retrieve the Python proxy.
void *displayio_native_init(void *native, const displayio_protocol_t *protocol,
    const mp_obj_type_t *type, size_t size, mp_obj_t bus, bool board);
mp_obj_t displayio_register(mp_obj_t native);
// At teardown release non-survivors and detach the surviving VM proxies.
void displayio_start_vm(void);
void displayio_stop_vm(void);

// Shared proxy conversion and lazy proxy creation. Public bindings unwrap with
// the expected native type; native code uses the typed pointers directly.
void *displayio_proxy_native(mp_obj_t obj, const mp_obj_type_t *type);
// Like displayio_proxy_native() but returns MP_OBJ_NULL when the object has
// been deinitialized. Works with both lifetime implementations.
mp_obj_t displayio_native_obj(mp_obj_t obj, const mp_obj_type_t *type);
mp_obj_t displayio_bus_native(mp_obj_t obj);
mp_obj_t displayio_display_proxy(mp_obj_t native);
mp_obj_t displayio_bus_proxy(mp_obj_t native);
void displayio_display_set_root_group_reference(mp_obj_t native, mp_obj_t root_group);

// Native lifetime operations. Explicit display release can raise during
// e-paper preparation; proxy finalizers use the non-raising variant.
void displayio_display_deinit(mp_obj_t native);
void displayio_construct_failed(mp_obj_t native);
void displayio_display_proxy_finalise(void *obj);
void displayio_bus_deinit(mp_obj_t native_bus);
void displayio_bus_deinit_obj(mp_obj_t obj, const mp_obj_type_t *type);
void displayio_bus_proxy_finalise(void *obj);

bool displayio_is_primary_display(const void *native);
mp_obj_t displayio_primary_display_native(void);
mp_obj_t displayio_display_bus_native(mp_obj_t native);
// Iterate registered bus/framebuffer objects. Pass MP_OBJ_NULL to start.
mp_obj_t displayio_bus_next(mp_obj_t bus);
mp_obj_t displayio_board_display_native(void);

void displayio_background(void);
void displayio_gc_collect(void);
void displayio_gc_collect_primary_and_board(void);
