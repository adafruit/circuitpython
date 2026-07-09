// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Generic awaitable object for C-level async operations.
//
// Usage with CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_1:
//
//   // shared-bindings — parse args, validate, call common_hal:
//   CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_1(my_func, my_func_obj,
//       common_hal_my_end, common_hal_my_cancel) {
//       mp_int_t val = mp_obj_get_int(data);
//       return common_hal_my_start(flag, val);
//   }
//
//   // common-hal — allocate context, start hardware:
//   void *common_hal_my_start(circuitpy_async_flag_t *flag, mp_int_t val) {
//       my_context_t *ctx = m_new_obj(my_context_t);
//       ctx->flag = flag;
//       // start hardware, ISR calls CIRCUITPY_ASYNC_FLAG_SET(ctx->flag) ...
//       return ctx;
//   }

#pragma once

#include "py/obj.h"

#if MICROPY_PY_ASYNC_AWAIT

// Async completion flag — ports may override by defining these in
// mpconfigport.h before this header is included.
//
// Default: volatile bool.  Ports can substitute an RTOS primitive
// (e.g. k_event on Zephyr, EventGroupHandle_t on ESP-IDF).
#ifndef CIRCUITPY_ASYNC_FLAG_SET
typedef volatile bool circuitpy_async_flag_t;
#define CIRCUITPY_ASYNC_FLAG_INIT(flag) (*(flag) = false)
#define CIRCUITPY_ASYNC_FLAG_SET(flag) (*(flag) = true)
#define CIRCUITPY_ASYNC_FLAG_IS_SET(flag) (*(flag))
#endif

// start: called on first await with a pre-initialised flag.
//        Parse data, allocate context, begin operation.
//        Store the flag pointer in context so the completion
//        callback can call CIRCUITPY_ASYNC_FLAG_SET(flag).
typedef void *(*circuitpy_awaitable_start_fn)(
    circuitpy_async_flag_t *flag, mp_obj_t data);
// end: collect result and clean up.
typedef mp_obj_t (*circuitpy_awaitable_end_fn)(void *context);
// cancel: abort the operation (GC-safe, no allocations).
typedef void (*circuitpy_awaitable_cancel_fn)(void *context);

typedef struct {
    mp_obj_base_t base;
    circuitpy_awaitable_start_fn start;
    circuitpy_awaitable_end_fn end;
    circuitpy_awaitable_cancel_fn cancel;
    void *context;                        // returned by start()
    mp_obj_t data;                        // arg(s) passed through to start()
    circuitpy_async_flag_t flag;          // owned by the awaitable
    bool started;
    bool finished;
} circuitpy_awaitable_obj_t;

extern const mp_obj_type_t circuitpy_awaitable_type;

// Create a new awaitable. Used by the macros below.
mp_obj_t circuitpy_awaitable_new(
    circuitpy_awaitable_start_fn start,
    circuitpy_awaitable_end_fn end,
    circuitpy_awaitable_cancel_fn cancel,
    mp_obj_t data);

// Define an async function that takes 1 positional argument.
// Generates the MP function, the function object, and a start wrapper.
// Follow the macro with { body } to provide the start implementation.
// Inside the body, `flag` is a `circuitpy_async_flag_t *` and `data`
// is the mp_obj_t argument. The body must return `void *` (context).
//
// Example:
//
//   CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_1(my_func, my_func_obj,
//       common_hal_my_end, common_hal_my_cancel) {
//       mp_int_t ms = mp_obj_get_int(data);
//       if (ms < 0) mp_raise_ValueError(...);
//       return common_hal_my_start(flag, ms);
//   }
//
#define CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_1( \
    fn_name, obj_name, end, cancel) \
    static void *fn_name##_start( \
    circuitpy_async_flag_t * flag, mp_obj_t data); \
    static mp_obj_t fn_name(mp_obj_t arg) { \
        return circuitpy_awaitable_new( \
    fn_name##_start, (end), (cancel), arg); \
    } \
    static MP_DEFINE_CONST_FUN_OBJ_1(obj_name, fn_name); \
    static void *fn_name##_start( \
    circuitpy_async_flag_t * flag, mp_obj_t data)

#endif // MICROPY_PY_ASYNC_AWAIT
