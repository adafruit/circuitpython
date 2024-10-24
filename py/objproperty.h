// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// CIRCUITPY-CHANGE: MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS marks classes with
// properties, descriptors, __delattr__ or __setattr___.
// When defining native classes that use properties, you *must* set the
// MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS flag. Otherwise, the property will be
// ignored.

#if MICROPY_PY_BUILTINS_PROPERTY

typedef struct _mp_obj_property_t {
    mp_obj_base_t base;
    mp_obj_t proxy[3]; // getter, setter, deleter
} mp_obj_property_t;

#if MICROPY_PY_OPTIMIZE_PROPERTY_FLASH_SIZE
typedef struct _mp_obj_property_getter_t {
    mp_obj_base_t base;
    mp_obj_t proxy[1]; // getter
} mp_obj_property_getter_t;

typedef struct _mp_obj_property_getset_t {
    mp_obj_base_t base;
    mp_obj_t proxy[2]; // getter, setter
} mp_obj_property_getset_t;

#define MP_PROPERTY_GETTER(P, G) const mp_obj_property_getter_t P __attribute((section(".property_getter"))) = {.base.type = &mp_type_property, .proxy = {G}}
#define MP_PROPERTY_GETSET(P, G, S) const mp_obj_property_getset_t P __attribute((section(".property_getset"))) = {.base.type = &mp_type_property, .proxy = {G, S}}

#else
typedef struct _mp_obj_property_t mp_obj_property_getter_t;
typedef struct _mp_obj_property_t mp_obj_property_getset_t;

#define MP_PROPERTY_GETTER(P, G) const mp_obj_property_t P = {.base.type = &mp_type_property, .proxy = {G, MP_ROM_NONE, MP_ROM_NONE}}
#define MP_PROPERTY_GETSET(P, G, S) const mp_obj_property_t P = {.base.type = &mp_type_property, .proxy = {G, S, MP_ROM_NONE}}
#endif

#endif  // MICROPY_PY_BUILTINS_PROPERTY
