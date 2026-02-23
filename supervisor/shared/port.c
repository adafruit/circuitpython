// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include <string.h>

#include "py/runtime.h"
#include "py/gc.h"

#include "lib/tlsf/tlsf.h"

#ifdef CIRCUITPY_BOOT_BUTTON
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/time/__init__.h"
#endif

static tlsf_t heap;

MP_WEAK void port_wake_main_task(void) {
}

MP_WEAK void port_wake_main_task_from_isr(void) {
}

MP_WEAK void port_yield(void) {
}

MP_WEAK void port_boot_info(void) {
}

MP_WEAK void port_heap_init(void) {
    uint32_t *heap_bottom = port_heap_get_bottom();
    uint32_t *heap_top = port_heap_get_top();
    size_t size = (heap_top - heap_bottom) * sizeof(uint32_t);
    heap = tlsf_create_with_pool(heap_bottom, size, size);
}

MP_WEAK void *port_malloc(size_t size, bool dma_capable) {
    void *block = tlsf_malloc(heap, size);
    return block;
}

// Ensure allocated memory is zero.
MP_WEAK void *port_malloc_zero(size_t size, bool dma_capable) {
    void *ptr = port_malloc(size, dma_capable);
    if (ptr) {
        memset(ptr, 0, size);
    }
    return ptr;
}

MP_WEAK void port_free(void *ptr) {
    tlsf_free(heap, ptr);
}

// Safely free an object that may have been allocated from either the GC heap or port heap.
// If the pointer is on the GC heap, it will be freed by the GC automatically, so we do nothing.
// If the pointer is not on the GC heap (i.e., allocated with port_malloc), we free it with port_free.
// This is safe to call during shutdown when GC may not be available.
void circuitpy_free_obj(mp_obj_t obj) {
    if (obj == mp_const_none) {
        return;
    }
    void *ptr = MP_OBJ_TO_PTR(obj);
    if (gc_alloc_possible() && gc_ptr_on_heap(ptr)) {
        gc_free(ptr);
    } else {
        port_free(ptr);
    }
}

MP_WEAK void *port_realloc(void *ptr, size_t size, bool dma_capable) {
    return tlsf_realloc(heap, ptr, size);
}

static bool max_size_walker(void *ptr, size_t size, int used, void *user) {
    size_t *max_size = (size_t *)user;
    if (!used && *max_size < size) {
        *max_size = size;
    }
    return true;
}

MP_WEAK size_t port_heap_get_largest_free_size(void) {
    size_t max_size = 0;
    tlsf_walk_pool(tlsf_get_pool(heap), max_size_walker, &max_size);
    // IDF does this. Not sure why.
    return tlsf_fit_size(heap, max_size);
}

MP_WEAK bool port_boot_button_pressed(void) {
    #if defined(CIRCUITPY_BOOT_BUTTON)
    // Init/deinit the boot button every time in case it is used for LEDs.
    digitalio_digitalinout_obj_t boot_button;
    common_hal_digitalio_digitalinout_construct(&boot_button, CIRCUITPY_BOOT_BUTTON);
    common_hal_digitalio_digitalinout_switch_to_input(&boot_button, PULL_UP);
    common_hal_time_delay_ms(1);
    bool button_pressed = !common_hal_digitalio_digitalinout_get_value(&boot_button);
    common_hal_digitalio_digitalinout_deinit(&boot_button);
    return button_pressed;
    #else
    return false;
    #endif
}

// Ports may provide an implementation of this function if it is needed
MP_WEAK void port_gc_collect(void) {
}

// Allocates an object in the port heap, not the VM heap, and also sets type, for mp_obj_malloc{,_var} macros.
MP_NOINLINE void *mp_obj_port_malloc_helper(size_t num_bytes, const mp_obj_type_t *type) {
    mp_obj_base_t *base = (mp_obj_base_t *)port_malloc_zero(num_bytes, false);
    base->type = type;
    return base;
}

// Creates a tuple on the port heap, not the VM heap.
// Implementation copied from py/objtuple.c.
mp_obj_t mp_obj_new_port_tuple(size_t n, const mp_obj_t *items) {
    if (n == 0) {
        return mp_const_empty_tuple;
    }
    mp_obj_tuple_t *o = mp_obj_port_malloc_var(mp_obj_tuple_t, items, mp_obj_t, n, &mp_type_tuple);
    o->len = n;
    if (items) {
        for (size_t i = 0; i < n; i++) {
            o->items[i] = items[i];
        }
    }
    return MP_OBJ_FROM_PTR(o);
}
