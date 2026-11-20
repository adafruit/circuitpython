// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-module/displayio/__init__.h"
#include "shared-bindings/displayio/__init__.h"

#include "py/gc.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "shared-bindings/util.h"
#include "supervisor/port.h"
#include "supervisor/shared/display.h"
#include "supervisor/shared/reload.h"

static mp_obj_t displays;
static mp_obj_t buses;
static mp_obj_t primary_display;
static mp_obj_t primary_display_identity;
static bool primary_display_set_by_user;
static bool vm_active;
static bool keep_registry_alive;

// A driver may run background tasks or collect while waiting for hardware.
// Keep the registry stable for operations that traverse it while waiting.
typedef struct {
    nlr_jump_callback_node_t node;
    bool previous;
} registry_guard_t;

static void restore_registry_guard(void *ctx) {
    registry_guard_t *guard = ctx;
    keep_registry_alive = guard->previous;
}

static void guard_registry(registry_guard_t *guard) {
    guard->previous = keep_registry_alive;
    keep_registry_alive = true;
    nlr_push_jump_callback(&guard->node, restore_registry_guard);
}

typedef struct {
    mp_obj_base_t base;
    displayio_native_link_t link;
} native_with_link_t;

static displayio_native_link_t *native_link(mp_obj_t native) {
    return &((native_with_link_t *)MP_OBJ_TO_PTR(native))->link;
}

static displayio_display_link_t *display_link(mp_obj_t native) {
    return (displayio_display_link_t *)native_link(native);
}

static void list_append(mp_obj_t *head, mp_obj_t native) {
    while (*head != MP_OBJ_NULL) {
        head = &native_link(*head)->next;
    }
    *head = native;
}

static void list_remove(mp_obj_t *head, mp_obj_t native) {
    while (*head != MP_OBJ_NULL) {
        displayio_native_link_t *link = native_link(*head);
        if (*head == native) {
            *head = link->next;
            link->next = MP_OBJ_NULL;
            return;
        }
        head = &link->next;
    }
}

static bool list_contains(mp_obj_t head, mp_obj_t native) {
    for (mp_obj_t entry = head; entry != MP_OBJ_NULL; entry = native_link(entry)->next) {
        if (entry == native) {
            return true;
        }
    }
    return false;
}

static bool display_survives(mp_obj_t native) {
    return native == primary_display || display_link(native)->board;
}

static bool protocol_is_bus(const displayio_protocol_t *protocol) {
    return protocol->background == NULL;
}

void displayio_display_background_noop(void *self) {
    (void)self;
}

static bool bus_has_display(mp_obj_t bus, bool surviving_only) {
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        if (display_link(native)->bus == bus && (!surviving_only || display_survives(native))) {
            return true;
        }
    }
    return false;
}

void *displayio_native_init(void *native, const displayio_protocol_t *protocol,
    const mp_obj_type_t *type, size_t size, mp_obj_t bus, bool board) {
    bool static_allocation = native != NULL;
    if (native == NULL) {
        native = port_malloc_zero(size, false);
        if (native == NULL) {
            mp_raise_msg(&mp_type_MemoryError, NULL);
        }
    }
    mp_obj_t object = MP_OBJ_FROM_PTR(native);
    ((mp_obj_base_t *)native)->type = type;
    displayio_native_link_t *link = native_link(object);
    *link = (displayio_native_link_t) {
        .protocol = protocol,
        .static_allocation = static_allocation,
    };

    if (protocol_is_bus(protocol)) {
        return native;
    }

    displayio_display_link_t *display = display_link(object);
    display->bus = bus;
    display->board = board;
    return native;
}

static bool native_is_moveable(mp_obj_t native) {
    const displayio_protocol_t *protocol = native_link(native)->protocol;
    return protocol->is_moveable == NULL || protocol->is_moveable(MP_OBJ_TO_PTR(native));
}

static bool display_is_moveable(mp_obj_t native) {
    if (!native_is_moveable(native)) {
        return false;
    }
    mp_obj_t bus = display_link(native)->bus;
    return bus == MP_OBJ_NULL || native_is_moveable(bus);
}

static void register_display(mp_obj_t native) {
    if (primary_display == MP_OBJ_NULL && !display_is_moveable(native)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Display resources cannot move between VMs"));
    }
    list_append(&displays, native);
    if (primary_display == MP_OBJ_NULL) {
        primary_display = native;
    }
}

mp_obj_t displayio_register(mp_obj_t native) {
    if (protocol_is_bus(native_link(native)->protocol)) {
        if (list_contains(buses, native)) {
            return displayio_bus_proxy(native);
        }
        mp_obj_t proxy = displayio_bus_proxy(native);
        list_append(&buses, native);
        return proxy;
    }
    if (list_contains(displays, native)) {
        return displayio_display_proxy(native);
    }
    mp_obj_t proxy = displayio_display_proxy(native);
    register_display(native);
    return proxy;
}

void *displayio_proxy_native(mp_obj_t obj, const mp_obj_type_t *type) {
    mp_obj_t base = mp_obj_cast_to_native_base(obj, MP_OBJ_FROM_PTR(type));
    if (base == MP_OBJ_NULL) {
        mp_raise_TypeError_varg(MP_ERROR_TEXT("expected %q"), type->name);
    }
    displayio_proxy_t *proxy = MP_OBJ_TO_PTR(base);
    if (!gc_ptr_on_heap(proxy)) {
        // Board dictionaries may expose their statically allocated native
        // display directly instead of a VM-heap proxy.
        return proxy;
    }
    if (proxy->native == MP_OBJ_NULL) {
        raise_deinited_error();
    }
    return MP_OBJ_TO_PTR(proxy->native);
}

static mp_obj_t find_bus(mp_obj_t head, mp_obj_t obj) {
    for (mp_obj_t native = head; native != MP_OBJ_NULL; native = native_link(native)->next) {
        const mp_obj_type_t *type = mp_obj_get_type(native);
        mp_obj_t cast = mp_obj_cast_to_native_base(obj, MP_OBJ_FROM_PTR(type));
        if (cast != MP_OBJ_NULL &&
            (cast == native || cast == native_link(native)->proxy)) {
            return native;
        }
    }
    return MP_OBJ_NULL;
}

mp_obj_t displayio_bus_native(mp_obj_t obj) {
    if (obj == MP_OBJ_NULL) {
        return MP_OBJ_NULL;
    }
    return find_bus(buses, obj);
}

static mp_obj_t get_proxy(mp_obj_t native, mp_obj_t owner) {
    displayio_native_link_t *link = native_link(native);
    if (link->proxy != MP_OBJ_NULL || !vm_active) {
        return link->proxy;
    }
    // Keep the proxy and the bus visible to the conservative GC while the
    // driver's VM state is allocated. Do not publish a half-created proxy.
    displayio_proxy_t *volatile proxy = mp_obj_malloc_with_finaliser(displayio_proxy_t, mp_obj_get_type(native));
    proxy->native = MP_OBJ_NULL;
    proxy->bus = owner;
    proxy->root_group = MP_OBJ_NULL;
    proxy->vm = link->protocol->start_vm == NULL ? NULL : link->protocol->start_vm(MP_OBJ_TO_PTR(native));
    proxy->native = native;
    link->proxy = MP_OBJ_FROM_PTR(proxy);
    return link->proxy;
}

mp_obj_t displayio_bus_proxy(mp_obj_t native) {
    return native == MP_OBJ_NULL ? mp_const_none : get_proxy(native, MP_OBJ_NULL);
}

mp_obj_t displayio_display_proxy(mp_obj_t native) {
    if (native == MP_OBJ_NULL) {
        return mp_const_none;
    }
    if (native_link(native)->proxy != MP_OBJ_NULL || !vm_active) {
        return native_link(native)->proxy;
    }
    return get_proxy(native, displayio_bus_proxy(display_link(native)->bus));
}

void displayio_start_vm(void) {
    // Materialize every surviving display so directly exposed static board
    // objects also have VM storage for their root groups and dependencies.
    vm_active = true;
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        if (display_survives(native)) {
            displayio_display_proxy(native);
        }
    }
}

void displayio_display_set_root_group_reference(mp_obj_t native, mp_obj_t root_group) {
    displayio_native_link_t *link = native_link(native);
    if (link->proxy != MP_OBJ_NULL) {
        ((displayio_proxy_t *)MP_OBJ_TO_PTR(link->proxy))->root_group = root_group;
    }
}

bool displayio_is_primary_display(const void *native) {
    mp_obj_t display = MP_OBJ_FROM_PTR(native);
    // During construction, size the terminal for the prospective primary,
    // but don't publish it until construction succeeds.
    return primary_display == display ||
           (primary_display == MP_OBJ_NULL && !list_contains(displays, display));
}

mp_obj_t displayio_primary_display_native(void) {
    return primary_display;
}

mp_obj_t displayio_display_bus_native(mp_obj_t native) {
    return native == MP_OBJ_NULL ? MP_OBJ_NULL : display_link(native)->bus;
}

mp_obj_t displayio_bus_next(mp_obj_t bus) {
    return bus == MP_OBJ_NULL ? buses : native_link(bus)->next;
}

mp_obj_t displayio_board_display_native(void) {
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        if (display_link(native)->board) {
            return native;
        }
    }
    return MP_OBJ_NULL;
}

static void invalidate_proxy(mp_obj_t native) {
    displayio_native_link_t *link = native_link(native);
    if (link->proxy != MP_OBJ_NULL) {
        displayio_proxy_t *proxy = MP_OBJ_TO_PTR(link->proxy);
        proxy->native = MP_OBJ_NULL;
        proxy->bus = MP_OBJ_NULL;
        proxy->root_group = MP_OBJ_NULL;
        proxy->vm = NULL;
        link->proxy = MP_OBJ_NULL;
    }
}

static void deinit_native(mp_obj_t native) {
    displayio_native_link_t *link = native_link(native);
    link->protocol->deinit(MP_OBJ_TO_PTR(native));
    invalidate_proxy(native);
    if (link->static_allocation) {
        ((mp_obj_base_t *)MP_OBJ_TO_PTR(native))->type = &mp_type_NoneType;
    } else {
        port_free(MP_OBJ_TO_PTR(native));
    }
}

static void bus_deinit(mp_obj_t bus);

static void display_deinit(mp_obj_t native, bool release_empty_bus) {
    mp_obj_t bus = display_link(native)->bus;
    // Keep the display proxy and its dependencies marked if release runs GC.
    volatile mp_obj_t proxy = native_link(native)->proxy;
    (void)proxy;
    list_remove(&displays, native);
    if (native == primary_display) {
        primary_display = MP_OBJ_NULL;
        primary_display_identity = MP_OBJ_NULL;
        primary_display_set_by_user = false;
        supervisor_stop_terminal();
    }
    deinit_native(native);
    if (release_empty_bus && bus != MP_OBJ_NULL && !bus_has_display(bus, false)) {
        bus_deinit(bus);
    }
}

void displayio_display_deinit(mp_obj_t native) {
    if (native == MP_OBJ_NULL) {
        return;
    }
    display_deinit(native, true);
}

void displayio_construct_failed(mp_obj_t native) {
    deinit_native(native);
}

static void bus_deinit(mp_obj_t bus) {
    volatile mp_obj_t proxy = native_link(bus)->proxy;
    (void)proxy;
    for (mp_obj_t *cursor = &displays; *cursor != MP_OBJ_NULL;) {
        mp_obj_t native = *cursor;
        if (display_link(native)->bus == bus) {
            display_deinit(native, false);
        } else {
            cursor = &native_link(native)->next;
        }
    }
    list_remove(&buses, bus);
    deinit_native(bus);
}

void displayio_bus_deinit(mp_obj_t native) {
    if (native == MP_OBJ_NULL) {
        return;
    }
    registry_guard_t guard;
    guard_registry(&guard);
    bus_deinit(native);
    nlr_pop_jump_callback(true);
}

void displayio_display_proxy_finalise(displayio_proxy_t *proxy) {
    if (proxy->native == MP_OBJ_NULL) {
        return;
    }
    if (display_survives(proxy->native)) {
        // The primary and board displays survive the VM. Stop them gracefully
        // instead of tearing down their hardware from a finalizer.
        const displayio_protocol_t *protocol = native_link(proxy->native)->protocol;
        if (protocol->stop_vm != NULL) {
            protocol->stop_vm(MP_OBJ_TO_PTR(proxy->native));
        }
        invalidate_proxy(proxy->native);
    } else {
        // Deinit may wait, but it must not raise.
        display_deinit(proxy->native, false);
    }
}

void displayio_bus_proxy_finalise(displayio_proxy_t *proxy) {
    if (proxy->native != MP_OBJ_NULL && !bus_has_display(proxy->native, false)) {
        bus_deinit(proxy->native);
    }
}

void displayio_bus_deinit_obj(mp_obj_t obj, const mp_obj_type_t *type) {
    mp_obj_t base = mp_obj_cast_to_native_base(obj, MP_OBJ_FROM_PTR(type));
    if (base != MP_OBJ_NULL) {
        displayio_proxy_t *proxy = MP_OBJ_TO_PTR(base);
        displayio_bus_deinit(proxy->native);
    }
}

void common_hal_displayio_release_displays(void) {
    registry_guard_t guard;
    guard_registry(&guard);
    mp_obj_t native = primary_display == MP_OBJ_NULL ? displayio_board_display_native() : primary_display;
    while (native != MP_OBJ_NULL) {
        mp_obj_t bus = display_link(native)->bus;
        if (bus != MP_OBJ_NULL) {
            bus_deinit(bus);
        } else {
            display_deinit(native, false);
        }
        native = displayio_board_display_native();
    }
    nlr_pop_jump_callback(true);
}

static bool stop_vm_native(mp_obj_t native) {
    const displayio_protocol_t *protocol = native_link(native)->protocol;
    return protocol->stop_vm == NULL || protocol->stop_vm(MP_OBJ_TO_PTR(native));
}

void displayio_stop_vm(void) {
    registry_guard_t guard;
    guard_registry(&guard);
    // Finish non-surviving displays before moving or releasing their buses.
    for (mp_obj_t *cursor = &displays; *cursor != MP_OBJ_NULL;) {
        mp_obj_t native = *cursor;
        if (!display_survives(native)) {
            display_deinit(native, false);
        } else {
            cursor = &native_link(native)->next;
        }
    }
    for (mp_obj_t *cursor = &buses; *cursor != MP_OBJ_NULL;) {
        mp_obj_t native = *cursor;
        if (!bus_has_display(native, false) || !stop_vm_native(native)) {
            bus_deinit(native);
        } else {
            cursor = &native_link(native)->next;
        }
    }
    // Detach old root groups from non-primary displays before putting the
    // terminal splash on the primary.
    for (mp_obj_t *cursor = &displays; *cursor != MP_OBJ_NULL;) {
        mp_obj_t native = *cursor;
        if (native == primary_display) {
            cursor = &native_link(native)->next;
        } else if (stop_vm_native(native)) {
            invalidate_proxy(native);
            cursor = &native_link(native)->next;
        } else {
            display_deinit(native, true);
        }
    }
    if (primary_display != MP_OBJ_NULL) {
        if (stop_vm_native(primary_display)) {
            invalidate_proxy(primary_display);
        } else {
            display_deinit(primary_display, true);
        }
    }
    for (mp_obj_t bus = buses; bus != MP_OBJ_NULL; bus = native_link(bus)->next) {
        invalidate_proxy(bus);
    }
    primary_display_identity = MP_OBJ_NULL;
    vm_active = false;
    nlr_pop_jump_callback(true);
}

void displayio_gc_collect_primary_and_board(void) {
    gc_collect_root(&primary_display_identity, 1);
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        if (keep_registry_alive || display_survives(native)) {
            gc_collect_root(&native_link(native)->proxy, 1);
        }
        // Keep bus finalization ordered after attached display finalization,
        // even when both proxies are otherwise unreachable.
        mp_obj_t bus = display_link(native)->bus;
        if (!keep_registry_alive && bus != MP_OBJ_NULL) {
            gc_collect_root(&native_link(bus)->proxy, 1);
        }
    }
    if (keep_registry_alive) {
        for (mp_obj_t bus = buses; bus != MP_OBJ_NULL; bus = native_link(bus)->next) {
            gc_collect_root(&native_link(bus)->proxy, 1);
        }
    }
}

#if !CIRCUITPY_DISPLAYIO_LVGL
void displayio_gc_collect(void) {
    displayio_gc_collect_primary_and_board();
}
#endif

mp_obj_t common_hal_displayio_get_primary_display(void) {
    return primary_display_identity != MP_OBJ_NULL ?
           primary_display_identity : displayio_display_proxy(primary_display);
}

static mp_obj_t find_display(mp_obj_t obj) {
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        mp_obj_t proxy = native_link(native)->proxy;
        if (obj == native || obj == proxy) {
            return native;
        }
        const mp_obj_type_t *type = mp_obj_get_type(native);
        mp_obj_t cast = mp_obj_cast_to_native_base(obj, MP_OBJ_FROM_PTR(type));
        if (cast != MP_OBJ_NULL && (cast == native || cast == proxy)) {
            return native;
        }
    }
    return MP_OBJ_NULL;
}

void common_hal_displayio_set_primary_display(mp_obj_t new_primary) {
    if (new_primary == mp_const_none) {
        primary_display = MP_OBJ_NULL;
        primary_display_identity = MP_OBJ_NULL;
        primary_display_set_by_user = false;
        return;
    }
    mp_obj_t native = find_display(new_primary);
    if (native != MP_OBJ_NULL) {
        if (!display_is_moveable(native)) {
            mp_raise_ValueError(MP_ERROR_TEXT("Display resources cannot move between VMs"));
        }
        primary_display = native;
        primary_display_identity = new_primary;
        primary_display_set_by_user = true;
        return;
    }
    mp_raise_TypeError_varg(MP_ERROR_TEXT("%q must be of type %q or %q, not %q"), MP_QSTR_Display, MP_QSTR_AnyDisplay, MP_QSTR_None, mp_obj_get_type(new_primary)->name);
}

bool common_hal_displayio_primary_display_set_by_user(void) {
    return primary_display_set_by_user;
}

void displayio_background(void) {
    if (mp_hal_is_interrupted() || autoreload_ready()) {
        return;
    }
    for (mp_obj_t native = displays; native != MP_OBJ_NULL; native = native_link(native)->next) {
        displayio_native_link_t *link = native_link(native);
        const displayio_protocol_t *protocol = link->protocol;
        if (protocol->background != NULL) {
            // A background driver may collect; pin this node until it returns.
            volatile mp_obj_t proxy = link->proxy;
            (void)proxy;
            protocol->background(MP_OBJ_TO_PTR(native));
        }
    }
}
