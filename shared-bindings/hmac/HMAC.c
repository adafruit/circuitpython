// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared-bindings/hmac/HMAC.h"

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

//| class HMAC:
//|     """An HMAC object, in progress. Created by `hmac.new()`; it has no user-visible
//|     constructor."""
//|

//|     def update(self, msg: ReadableBuffer) -> None:
//|         """Feed more data into the HMAC."""
//|         ...
mp_obj_t hmac_hmac_update(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    common_hal_hmac_update(self, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(hmac_hmac_update_obj, hmac_hmac_update);

//|     def digest(self) -> bytes:
//|         """Return the HMAC of the data fed so far, as ``digest_size`` bytes.
//|
//|         The object can still be updated after this call."""
//|         ...
static mp_obj_t hmac_hmac_digest(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    size_t size = common_hal_hmac_get_digest_size(self);
    mp_obj_t obj = mp_obj_new_bytes_of_zeros(size);
    mp_obj_str_t *o = MP_OBJ_TO_PTR(obj);

    common_hal_hmac_digest(self, (uint8_t *)o->data, size);
    return obj;
}
static MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_digest_obj, hmac_hmac_digest);

//|     def hexdigest(self) -> str:
//|         """Like `digest()` but returns the MAC as a string of hexadecimal digits."""
//|         ...
static mp_obj_t hmac_hmac_hexdigest(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    size_t size = common_hal_hmac_get_digest_size(self);
    uint8_t digest[PSA_HASH_MAX_SIZE];
    common_hal_hmac_digest(self, digest, size);

    vstr_t vstr;
    vstr_init_len(&vstr, size * 2);
    for (size_t i = 0; i < size; i++) {
        vstr.buf[i * 2] = nibble_to_hex_lower[digest[i] >> 4];
        vstr.buf[i * 2 + 1] = nibble_to_hex_lower[digest[i] & 0xf];
    }
    return mp_obj_new_str_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_hexdigest_obj, hmac_hmac_hexdigest);

//|     def copy(self) -> HMAC:
//|         """Return a copy of this HMAC object, with the same key and data fed so far."""
//|         ...
static mp_obj_t hmac_hmac_copy(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hmac_hmac_obj_t *other = mp_obj_malloc(hmac_hmac_obj_t, &hmac_hmac_type);
    common_hal_hmac_copy(self, other);
    return MP_OBJ_FROM_PTR(other);
}
static MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_copy_obj, hmac_hmac_copy);

//|     digest_size: int
//|     """The size of the MAC in bytes (32 for sha256, 20 for sha1). (read-only)"""
static mp_obj_t hmac_hmac_get_digest_size(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hmac_get_digest_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_get_digest_size_obj, hmac_hmac_get_digest_size);
MP_PROPERTY_GETTER(hmac_hmac_digest_size_obj, (mp_obj_t)&hmac_hmac_get_digest_size_obj);

//|     block_size: int
//|     """The internal block size of the hash algorithm in bytes. (read-only)"""
static mp_obj_t hmac_hmac_get_block_size(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hmac_get_block_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_get_block_size_obj, hmac_hmac_get_block_size);
MP_PROPERTY_GETTER(hmac_hmac_block_size_obj, (mp_obj_t)&hmac_hmac_get_block_size_obj);

//|     name: str
//|     """The canonical name of this HMAC, e.g. ``"hmac-sha256"``. (read-only)"""
//|
static mp_obj_t hmac_hmac_get_name(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &hmac_hmac_type));
    hmac_hmac_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const char *name = common_hal_hmac_get_name(self);
    return mp_obj_new_str(name, strlen(name));
}
MP_DEFINE_CONST_FUN_OBJ_1(hmac_hmac_get_name_obj, hmac_hmac_get_name);
MP_PROPERTY_GETTER(hmac_hmac_name_obj, (mp_obj_t)&hmac_hmac_get_name_obj);

static const mp_rom_map_elem_t hmac_hmac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&hmac_hmac_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&hmac_hmac_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR_hexdigest), MP_ROM_PTR(&hmac_hmac_hexdigest_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&hmac_hmac_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_ROM_PTR(&hmac_hmac_digest_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_ROM_PTR(&hmac_hmac_block_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_name), MP_ROM_PTR(&hmac_hmac_name_obj) },
};
static MP_DEFINE_CONST_DICT(hmac_hmac_locals_dict, hmac_hmac_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hmac_hmac_type,
    MP_QSTR_HMAC,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &hmac_hmac_locals_dict
    );
