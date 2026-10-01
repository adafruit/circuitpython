// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lib/svox/lib/picoapi.h"

typedef struct {
    mp_obj_base_t base;
    void *memory;            // engine working memory, not scanned by the GC
    size_t memory_size;
    pico_System system;
    pico_Engine engine;
    mp_obj_t ta;             // voice buffers, read in place by the engine, kept alive here
    mp_obj_t sg;
    mp_obj_t text;           // str being spoken, kept alive while the engine reads it
    const char *text_data;
    size_t text_len;         // including the NUL that flushes the engine
    size_t text_pos;
    // pico_getData fails if one output item does not fit, and items are up to 255 bytes. When
    // less room than that is left in the caller's buffer, output goes here first.
    int16_t carry[128];
    uint16_t carry_len;
    uint16_t carry_pos;
} picotts_engine_obj_t;
