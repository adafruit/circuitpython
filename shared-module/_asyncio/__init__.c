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

#include "shared-module/_asyncio/__init__.h"
#include "py/objgenerator.h"

// TODO: move this to mp_state_thread_t
STATIC mp_obj_t common_hal__asyncio_running_loop_obj = mp_const_none;

mp_obj_t *common_hal__asyncio_running_loop() {
    return &common_hal__asyncio_running_loop_obj;
}

void common_hal__asyncio_reset() {
    *common_hal__asyncio_running_loop() = mp_const_none;
}

bool common_hal__asyncio_iscoroutine(mp_const_obj_t obj) {
    if (mp_obj_get_type(obj) == &mp_type_gen_instance) {
        mp_obj_gen_instance_t *self = MP_OBJ_TO_PTR(obj);
        return self->coroutine_generator;
    }
    if (mp_obj_get_type(obj) == &mp_type_gen_wrap) {
        mp_obj_gen_wrap_t *self = MP_OBJ_TO_PTR(obj);
        return self->coroutine_generator;
    }
    return false;
}
