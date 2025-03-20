// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// An object that has a `deinit` method can use `default___enter___obj` and
// `default___exit___obj` to define the `__enter__` and `__exit__` members in
// its object table.
//
// `__enter__` returns the object itself, and `__exit__` calls its `deinit`.
//
// If enter/exit do anything else, such as taking & releasing a lock, these are
// not suitable.
#define default___enter___obj (mp_identity_obj)
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(default___exit___obj);
