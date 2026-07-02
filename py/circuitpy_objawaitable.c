// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/circuitpy_objawaitable.h"
#include "py/mpprint.h"
#include "py/runtime.h"

#if MICROPY_PY_ASYNC_AWAIT

#if MICROPY_PY_ASYNCIO
extern mp_obj_t mp_asyncio_context;
#endif

static mp_obj_t awaitable_iternext(mp_obj_t self_in) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->started) {
        self->started = true;
        CIRCUITPY_ASYNC_FLAG_INIT(&self->flag);
        self->context = self->start(&self->flag, self->data);
    }
    if (!CIRCUITPY_ASYNC_FLAG_IS_SET(&self->flag)) {
        // asyncio's run_until_complete does NOT auto-reschedule after send() returns —
        // the iterator must push the current task back onto _task_queue itself, just
        // like SingletonGenerator does for asyncio.sleep().  Without this the task
        // disappears from the queue and the await never completes.
        #if MICROPY_PY_ASYNCIO
        if (mp_asyncio_context != MP_OBJ_NULL) {
            mp_obj_t _task_queue = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR__task_queue));
            mp_obj_t cur_task = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
            if (_task_queue != MP_OBJ_NULL && cur_task != MP_OBJ_NULL && cur_task != mp_const_none) {
                mp_obj_t dest[3];
                mp_load_method(_task_queue, MP_QSTR_push, dest);
                dest[2] = cur_task;
                mp_call_method_n_kw(1, 0, dest);
            }
        }
        #endif
        return mp_const_none; // yield — not ready yet
    }
    // Done — collect result and clean up.
    mp_obj_t result = mp_const_none;
    if (self->end) {
        self->finished = true;
        result = self->end(self->context);
    }
    self->finished = true;
    MP_STATE_THREAD(stop_iteration_arg) = result;
    return MP_OBJ_STOP_ITERATION;
}

// __await__() returns self — this object IS its own iterator.
static mp_obj_t awaitable_await(mp_obj_t self_in) {
    return self_in;
}
static MP_DEFINE_CONST_FUN_OBJ_1(awaitable_await_obj, awaitable_await);

// send(value): only None is accepted.
static mp_obj_t awaitable_send(mp_obj_t self_in, mp_obj_t value) {
    if (value != mp_const_none) {
        mp_raise_TypeError(MP_ERROR_TEXT("can't send non-None value"));
    }
    mp_obj_t ret = awaitable_iternext(self_in);
    if (ret == MP_OBJ_STOP_ITERATION) {
        mp_raise_StopIteration(MP_STATE_THREAD(stop_iteration_arg));
    }
    return ret;
}
static MP_DEFINE_CONST_FUN_OBJ_2(awaitable_send_obj, awaitable_send);

// throw(exc): cancel the operation and propagate the exception.
static mp_obj_t awaitable_throw(mp_obj_t self_in, mp_obj_t exc_in) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->started && !self->finished) {
        if (self->cancel) {
            self->cancel(self->context);
        }
        if (self->end) {
            self->finished = true;
            self->end(self->context);
        }
        self->finished = true;
    }
    nlr_raise(mp_make_raise_obj(exc_in));
}
static MP_DEFINE_CONST_FUN_OBJ_2(awaitable_throw_obj, awaitable_throw);

// close(): cancel the operation gracefully.
static mp_obj_t awaitable_close(mp_obj_t self_in) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->started && !self->finished) {
        if (self->cancel) {
            self->cancel(self->context);
        }
        if (self->end) {
            self->finished = true;
            self->end(self->context);
        }
        self->finished = true;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(awaitable_close_obj, awaitable_close);

// __del__: runs during GC — no allocations allowed.
// Cancel hardware but don't call end (which may allocate a result).
static mp_obj_t awaitable_del(mp_obj_t self_in) {
    circuitpy_awaitable_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (!self->started) {
        mp_printf(MICROPY_ERROR_PRINTER, "RuntimeWarning: awaitable was never awaited\n");
    } else if (!self->finished) {
        if (self->cancel) {
            self->cancel(self->context);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(awaitable_del_obj, awaitable_del);

static const mp_rom_map_elem_t awaitable_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___await__), MP_ROM_PTR(&awaitable_await_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&awaitable_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&awaitable_throw_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&awaitable_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&awaitable_del_obj) },
};
static MP_DEFINE_CONST_DICT(awaitable_locals_dict, awaitable_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    circuitpy_awaitable_type,
    MP_QSTR_Awaitable,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, awaitable_iternext,
    locals_dict, &awaitable_locals_dict
    );

mp_obj_t circuitpy_awaitable_new(
    circuitpy_awaitable_start_fn start,
    circuitpy_awaitable_end_fn end,
    circuitpy_awaitable_cancel_fn cancel,
    mp_obj_t data) {
    circuitpy_awaitable_obj_t *self =
        mp_obj_malloc_with_finaliser(circuitpy_awaitable_obj_t, &circuitpy_awaitable_type);
    self->start = start;
    self->end = end;
    self->cancel = cancel;
    self->context = NULL;
    self->data = data;
    self->started = false;
    self->finished = false;
    return MP_OBJ_FROM_PTR(self);
}

#endif // MICROPY_PY_ASYNC_AWAIT
