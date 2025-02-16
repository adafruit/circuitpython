// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared/runtime/context_manager_helpers.h"

#include "py/obj.h"
#include "py/runtime.h"

static mp_obj_t default___exit__(size_t n_args, const mp_obj_t *args) {
    mp_obj_t dest[2];
    mp_load_method(args[0], MP_QSTR_deinit, dest);
    mp_call_method_n_kw(0, 0, dest);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(default___exit___obj, 4, 4, default___exit__);
