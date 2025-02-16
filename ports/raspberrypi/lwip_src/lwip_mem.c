// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Bob Abeles
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <string.h>
#include "lib/tlsf/tlsf.h"
#include "lwip_mem.h"
#include "supervisor/port_heap.h"

void *lwip_heap_malloc(size_t size) {
    return port_malloc(size, true);
}

void lwip_heap_free(void *ptr) {
    port_free(ptr);
}

void *lwip_heap_calloc(size_t num, size_t size) {
    void *ptr = lwip_heap_malloc(num * size);
    if (ptr != NULL) {
        memset(ptr, 0, num * size);
    }
    return ptr;
}
