/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-module/_asyncio/Loop.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "py/runtime.h"


void common_hal__asyncio_loop_init(_asyncio_loop_obj_t *native_loop, const mp_obj_type_t *type) {
    native_loop->base.type = type;
    native_loop->call_soon_list_head = NULL;
    native_loop->call_soon_list_tail = &native_loop->call_soon_list_head;
}

_asyncio_loop_call_soon_entry_t *common_hal__asyncio_loop_call_soon_entry_alloc(_asyncio_loop_obj_t *native_loop, mp_obj_t loop_obj, mp_obj_t fun_obj, size_t n_args, mp_obj_t *args) {
    _asyncio_loop_call_soon_entry_t *entry = m_new_obj(_asyncio_loop_call_soon_entry_t);
    entry->next = NULL;
    entry->native_loop = native_loop;
    entry->n_args = n_args + 2;
    entry->args = m_new(mp_obj_t, entry->n_args);
    entry->args[0] = loop_obj;
    entry->args[1] = fun_obj;
    for (size_t i = 0; i < n_args; i++) {
        entry->args[i + 2] = args[i];
    }
    return entry;
}

void common_hal__asyncio_loop_call_soon_entry_free(_asyncio_loop_call_soon_entry_t *entry) {
    m_free(entry->args);
    m_free(entry);
}

void common_hal__asyncio_loop_call_soon_isrsafe(_asyncio_loop_call_soon_entry_t *entry) {
    _asyncio_loop_call_soon_entry_t **tail = entry->native_loop->call_soon_list_tail;
    *tail = entry;
    tail = &entry->next;
}

void common_hal__asyncio_loop_poll_isr(_asyncio_loop_obj_t *native_loop, mp_obj_t loop_obj) {
    mp_obj_t dest[2];
    mp_load_method(loop_obj, MP_QSTR_call_soon, dest);

    common_hal_mcu_disable_interrupts();
    _asyncio_loop_call_soon_entry_t *entry = native_loop->call_soon_list_head;
    native_loop->call_soon_list_head = NULL;
    native_loop->call_soon_list_tail = &native_loop->call_soon_list_head;
    common_hal_mcu_enable_interrupts();

    while (entry) {
        mp_call_function_n_kw(dest[0], entry->n_args, 0, entry->args);
        entry = entry->next;
    }
}
