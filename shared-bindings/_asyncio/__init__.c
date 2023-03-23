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
#include "shared-module/_asyncio/__init__.h"
#include "shared-module/_asyncio/Loop.h"
#include "py/runtime.h"


//| def set_running_loop(loop: BaseLoop) -> None:
//|     """Set loop as the running event loop used by CircuitPython."""
//|     ...
//|
STATIC mp_obj_t _asyncio_set_running_loop(mp_obj_t loop_obj) {
    if (loop_obj != mp_const_none) {
        _asyncio_get_native_loop(loop_obj);
    }
    *common_hal__asyncio_running_loop() = loop_obj;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(_asyncio_set_running_loop_obj, _asyncio_set_running_loop);


//| def get_running_loop() -> BaseLoop:
//|     """Returns the running event loop used by CircuitPython."""
//|     ...
//|
STATIC mp_obj_t _asyncio_get_running_loop(void) {
    return *common_hal__asyncio_running_loop();
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(_asyncio_get_running_loop_obj, _asyncio_get_running_loop);


//| def iscoroutine(obj) -> bool:
//|     """Return True if the object is a coroutine created by an async def function."""
//|     ...
//|
STATIC mp_obj_t _asyncio_iscoroutine(mp_obj_t obj) {
    return mp_obj_new_bool(common_hal__asyncio_iscoroutine(obj));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(_asyncio_iscoroutine_obj, _asyncio_iscoroutine);

STATIC const mp_rom_map_elem_t _asyncio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__asyncio) },
    { MP_ROM_QSTR(MP_QSTR_BaseLoop), MP_ROM_PTR(&_asyncio_loop_type) },
    { MP_ROM_QSTR(MP_QSTR_set_running_loop), MP_ROM_PTR(&_asyncio_set_running_loop_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_running_loop), MP_ROM_PTR(&_asyncio_get_running_loop_obj) },
    { MP_ROM_QSTR(MP_QSTR_iscoroutine), MP_ROM_PTR(&_asyncio_iscoroutine_obj) },
};
STATIC MP_DEFINE_CONST_DICT(_asyncio_module_globals, _asyncio_module_globals_table);

const mp_obj_module_t _asyncio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&_asyncio_module_globals,
};
MP_REGISTER_MODULE(MP_QSTR__asyncio, _asyncio_module, CIRCUITPY__ASYNCIO);
