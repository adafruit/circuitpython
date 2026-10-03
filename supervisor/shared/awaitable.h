// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Awaitable functions implemented in C.
//
// CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(obj_name, n_args_min, fun_name) defines obj_name, a callable
// that takes the same arguments as an MP_DEFINE_CONST_FUN_OBJ_KW function but returns an
// awaitable. A call only checks the number of positional arguments; as with a coroutine in
// CPython, nothing runs and the arguments are parsed only when the awaitable is awaited. Then:
//
//   void *fun_name(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args,
//       mp_map_t *kw_args)
//       parses the arguments, starts the operation and returns a context. The operation sets
//       *done, possibly from an interrupt, once it has finished.
//   mp_obj_t common_hal_<fun_name>_end(void *context, circuitpy_async_flag_t *done)
//       finishes the operation once *done is set and returns the result of the await.
//   void common_hal_<fun_name>_cancel(void *context, circuitpy_async_flag_t *done)
//       stops the operation, or finishes it if *done is already set. It must not allocate: it
//       may run during GC. done tells which operation is meant, if the context has started
//       another one since.
//
// While an operation runs, the awaiting asyncio task waits on asyncio's I/O queue: the awaitable
// is a stream that polls readable once *done is set.

#pragma once

#include "py/obj.h"
#include "supervisor/shared/async_flag.h"

#if MICROPY_PY_ASYNC_AWAIT

typedef void *(*circuitpy_awaitable_start_fn)(circuitpy_async_flag_t *done, size_t n_args,
    const mp_obj_t *pos_args, mp_map_t *kw_args);
typedef mp_obj_t (*circuitpy_awaitable_end_fn)(void *context, circuitpy_async_flag_t *done);
typedef void (*circuitpy_awaitable_cancel_fn)(void *context, circuitpy_async_flag_t *done);

// An async function: calling it returns a circuitpy_awaitable_obj_t.
typedef struct {
    mp_obj_base_t base;
    uint32_t sig;                       // as made by MP_OBJ_FUN_MAKE_SIG
    circuitpy_awaitable_start_fn start;
    circuitpy_awaitable_end_fn end;
    circuitpy_awaitable_cancel_fn cancel;
} circuitpy_async_fun_obj_t;

extern const mp_obj_type_t circuitpy_async_fun_type;

typedef struct {
    mp_obj_base_t base;
    const circuitpy_async_fun_obj_t *fun;
    void *context;                  // returned by start()
    circuitpy_async_flag_t done;
    uint8_t state;                  // CIRCUITPY_AWAITABLE_NEW, _RUNNING or _FINISHED
    uint16_t n_args;
    uint16_t n_kw;
    mp_obj_t args[];                // positional arguments, then keyword name/value pairs
} circuitpy_awaitable_obj_t;

extern const mp_obj_type_t circuitpy_awaitable_type;

enum { CIRCUITPY_AWAITABLE_NEW, CIRCUITPY_AWAITABLE_RUNNING, CIRCUITPY_AWAITABLE_FINISHED };

#define CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(obj_name, n_args_min, fun_name) \
    const circuitpy_async_fun_obj_t obj_name = { \
        {&circuitpy_async_fun_type}, MP_OBJ_FUN_MAKE_SIG(n_args_min, MP_OBJ_FUN_ARGS_MAX, true), \
        fun_name, common_hal_##fun_name##_end, common_hal_##fun_name##_cancel \
    }

#endif // MICROPY_PY_ASYNC_AWAIT
