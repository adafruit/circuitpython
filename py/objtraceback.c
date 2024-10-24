// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "py/objtraceback.h"

const mp_obj_traceback_t mp_const_empty_traceback_obj = {{&mp_type_traceback}, 0, 0, NULL};

static void mp_obj_traceback_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_traceback_t *o = MP_OBJ_TO_PTR(o_in);
    mp_printf(print, "<%q object at %p>", MP_QSTR_traceback, o);
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_traceback,
    MP_QSTR_traceback,
    MP_TYPE_FLAG_NONE,
    print, mp_obj_traceback_print
    );
