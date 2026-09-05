// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "bindings/esphmac/HMACKey.h"
#include "common-hal/esphmac/HMACKey.h"

#define HMAC_DIGEST_SIZE 32

//| class HMACKey:
//|     """A handle to one eFuse key block burned with the ``HMAC_UP``
//|     (application-triggered HMAC) purpose.
//|
//|     Construction fails closed: only a block whose eFuse purpose field is
//|     already set to ``HMAC_UP`` can be used. This means an `HMACKey` can
//|     never be pointed at a block reserved for flash encryption, secure
//|     boot, JTAG re-enable, or the Digital Signature peripheral, even by
//|     mistake.
//|     """
//|
//|     def __init__(self, key_block: int) -> None:
//|         """Bind to eFuse key block `key_block`.
//|
//|         :param key_block: Which eFuse key block to use (``0``-``5``,
//|             corresponding to ``BLOCK_KEY0``-``BLOCK_KEY5``).
//|         :raises ValueError: if `key_block` is out of range, or its eFuse
//|             purpose is not ``HMAC_UP``.
//|         """
//|         ...
static mp_obj_t esphmac_hmackey_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_key_block };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key_block, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    esphmac_hmackey_obj_t *self = mp_obj_malloc(esphmac_hmackey_obj_t, &esphmac_hmackey_type);
    common_hal_esphmac_hmackey_construct(self, args[ARG_key_block].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def hmac(self, data: ReadableBuffer) -> bytes:
//|         """Compute the HMAC-SHA256 of ``data`` using this block's key and
//|         return the 32-byte result. The key itself is never returned or
//|         exposed by this or any other method.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: the message to authenticate
//|         """
//|         ...
static mp_obj_t esphmac_hmackey_hmac(mp_obj_t self_in, mp_obj_t data_in) {
    esphmac_hmackey_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    mp_obj_t result = mp_obj_new_bytes_of_zeros(HMAC_DIGEST_SIZE);
    mp_obj_str_t *result_bytes = MP_OBJ_TO_PTR(result);

    common_hal_esphmac_hmackey_hmac(self, bufinfo.buf, bufinfo.len, (uint8_t *)result_bytes->data);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_2(esphmac_hmackey_hmac_obj, esphmac_hmackey_hmac);

//|     key_block: int
//|     """The eFuse key block index (0-5) this handle is bound to. (read-only)"""
static mp_obj_t esphmac_hmackey_get_key_block(mp_obj_t self_in) {
    esphmac_hmackey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_esphmac_hmackey_get_key_block(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(esphmac_hmackey_get_key_block_obj, esphmac_hmackey_get_key_block);
MP_PROPERTY_GETTER(esphmac_hmackey_key_block_obj, (mp_obj_t)&esphmac_hmackey_get_key_block_obj);

//|     read_protected: bool
//|     """True if this key block's ``RD_DIS`` bit is set, i.e. hardware
//|     enforces that the raw key can never be read back by software or JTAG.
//|     Informational only -- it does not gate whether `hmac` can be called.
//|     Meant for manufacturing-time self-test code to confirm a key block was
//|     burned as expected. (read-only)"""
static mp_obj_t esphmac_hmackey_get_read_protected(mp_obj_t self_in) {
    esphmac_hmackey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_esphmac_hmackey_get_read_protected(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(esphmac_hmackey_get_read_protected_obj, esphmac_hmackey_get_read_protected);
MP_PROPERTY_GETTER(esphmac_hmackey_read_protected_obj, (mp_obj_t)&esphmac_hmackey_get_read_protected_obj);

static const mp_rom_map_elem_t esphmac_hmackey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_hmac), MP_ROM_PTR(&esphmac_hmackey_hmac_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_block), MP_ROM_PTR(&esphmac_hmackey_key_block_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_protected), MP_ROM_PTR(&esphmac_hmackey_read_protected_obj) },
};
static MP_DEFINE_CONST_DICT(esphmac_hmackey_locals_dict, esphmac_hmackey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    esphmac_hmackey_type,
    MP_QSTR_HMACKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, esphmac_hmackey_make_new,
    locals_dict, &esphmac_hmackey_locals_dict
    );
