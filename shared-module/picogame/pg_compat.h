// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// pg_compat.h — the small CircuitPython core-API delta the picogame engine relies on,
// OWNED BY THE ENGINE (not injected by a build-level force-`-include`). On CircuitPython
// this is a pure pass-through (every symbol is native); on a bare MicroPython build it
// supplies the ~9-symbol delta CP added over MicroPython. This lets the SAME engine C
// compile on both without a per-build shim impersonating CircuitPython.
//
// Engine TUs `#include "shared-module/picogame/pg_compat.h"` where they use any of these
// (it replaces the former `#include "py/objproperty.h"` in the bindings TUs). The function
// bodies for the MicroPython branch live in pg_compat_mp.c (compiled ONLY on MicroPython).
#pragma once

#include "py/runtime.h"

#if defined(CIRCUITPY)
// CircuitPython: everything below is native — pure pass-through.
#include "py/objproperty.h"
#else
// MicroPython: the core-API delta the engine needs that bare MicroPython lacks.
#include "py/obj.h"

#ifndef RUN_BACKGROUND_TASKS
#define RUN_BACKGROUND_TASKS (mp_handle_pending(MP_HANDLE_PENDING_CALLBACKS_AND_EXCEPTIONS))
#endif
#ifndef m_malloc_without_collect
#define m_malloc_without_collect(n) m_malloc(n)
#endif
#ifndef mp_raise_RuntimeError
#define mp_raise_RuntimeError(msg) mp_raise_msg(&mp_type_RuntimeError, (msg))
#endif

mp_int_t mp_arg_validate_int_min(mp_int_t i, mp_int_t min, qstr arg_name);
mp_int_t mp_arg_validate_int_range(mp_int_t i, mp_int_t min, mp_int_t max, qstr arg_name);
mp_obj_t mp_arg_validate_type(mp_obj_t obj, const mp_obj_type_t *type, qstr arg_name);
NORETURN void mp_raise_ValueError_varg(mp_rom_error_text_t fmt, ...);
NORETURN void mp_raise_TypeError_varg(mp_rom_error_text_t fmt, ...);
mp_obj_t mp_obj_new_bytearray_of_zeros(size_t n);

// CircuitPython's MP_PROPERTY_GETTER / MP_PROPERTY_GETSET convenience macros. Bare
// MicroPython keeps the property object private to py/objproperty.c; redeclare the
// struct with MP's EXACT layout + the macros in the non-native form (identical to
// CircuitPython's own !MICROPY_PY_OBJ_PROPERTY_NATIVE branch), so the generated
// property objects are ABI-compatible with MP's mp_type_property.
#if MICROPY_PY_BUILTINS_PROPERTY
typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3];   // getter, setter, deleter
} mp_obj_property_getset_t;
#ifndef MP_PROPERTY_GETTER
#define MP_PROPERTY_GETTER(P, G) \
    const mp_obj_property_getset_t P = { .base.type = &mp_type_property, .proxy = {G, MP_ROM_NONE, MP_ROM_NONE} }
#endif
#ifndef MP_PROPERTY_GETSET
#define MP_PROPERTY_GETSET(P, G, S) \
    const mp_obj_property_getset_t P = { .base.type = &mp_type_property, .proxy = {G, S, MP_ROM_NONE} }
#endif
#endif // MICROPY_PY_BUILTINS_PROPERTY

#endif // CIRCUITPY
