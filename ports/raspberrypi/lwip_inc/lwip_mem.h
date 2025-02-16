// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Bob Abeles
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>

void *lwip_heap_malloc(size_t size);
void lwip_heap_free(void *ptr);
void *lwip_heap_calloc(size_t num, size_t size);
