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

#include "shared-bindings/_asyncio/Loop.h"
#include "shared-module/_asyncio/Loop.h"
#include "py/runtime.h"

_asyncio_loop_obj_t *_asyncio_get_native_loop(mp_obj_t loop_obj) {
    _asyncio_loop_obj_t *native_loop = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(loop_obj, &_asyncio_loop_type));
    if (!native_loop) {
        mp_raise_TypeError(MP_ERROR_TEXT("object is not a loop"));
    }
    return native_loop;
}

//| class BaseLoop:
//|     """A native base class used by implementations of asyncio Loop.
//|
//|     Implementations use this base class to access the queue of callbacks scheduled from hardware interrupt handlers.
//|     The implementation is expected to implement a subset of the CPython Loop interface (https://docs.python.org/3/library/asyncio-eventloop.html),
//|     specifically these methods:
//|       - call_soon
//|       - create_future
//|     """
//|
//|     def __init__(self) -> None:
//|         """Initializes the loop."""
//|         ...
STATIC mp_obj_t _asyncio_loop_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _asyncio_loop_obj_t *self = m_new_obj(_asyncio_loop_obj_t);
    common_hal__asyncio_loop_init(self, type);
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    return MP_OBJ_FROM_PTR(self);
}

//|     def poll_isr(self) -> None:
//|         """Drains the queue of callbacks from interrupt handlers and queues them onto the regular loop queue using the Python method `call_soon`."""
//|         ...
//|
STATIC mp_obj_t _asyncio_loop_poll_isr(mp_obj_t self_obj) {
    _asyncio_loop_obj_t *native_loop = _asyncio_get_native_loop(self_obj);
    common_hal__asyncio_loop_poll_isr(native_loop, self_obj);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(_asyncio_loop_poll_isr_obj, _asyncio_loop_poll_isr);

STATIC const mp_rom_map_elem_t _asyncio_loop_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_poll_isr), MP_ROM_PTR(&_asyncio_loop_poll_isr_obj) },
};
STATIC MP_DEFINE_CONST_DICT(_asyncio_loop_locals_dict, _asyncio_loop_locals_dict_table);

const mp_obj_type_t _asyncio_loop_type = {
    { &mp_type_type },
    .name = MP_QSTR_BaseLoop,
    .make_new = _asyncio_loop_make_new,
    .locals_dict = (mp_obj_dict_t *)&_asyncio_loop_locals_dict,
};
