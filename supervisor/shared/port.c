// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/port.h"

#include <string.h>

#include "py/runtime.h"

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

MP_WEAK void port_free(void *ptr) {
    tlsf_free(heap, ptr);
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
