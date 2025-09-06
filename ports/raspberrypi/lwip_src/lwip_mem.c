// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Bob Abeles
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>
#include "lib/tlsf/tlsf.h"
#include "lwip_mem.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/port_heap.h"

void *lwip_heap_malloc(size_t size) {
    common_hal_mcu_disable_interrupts();
    void *ptr = port_malloc(size, true);
    common_hal_mcu_enable_interrupts();
    return ptr;
}

void lwip_heap_free(void *ptr) {
    common_hal_mcu_disable_interrupts();
    port_free(ptr);
    common_hal_mcu_enable_interrupts();
}

void *lwip_heap_calloc(size_t num, size_t size) {
    void *ptr = lwip_heap_malloc(num * size);
    if (ptr != NULL) {
        memset(ptr, 0, num * size);
    }
    return ptr;
}
