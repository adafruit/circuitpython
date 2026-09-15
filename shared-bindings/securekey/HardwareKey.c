// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/securekey/HardwareKey.h"

#define HMAC_SHA256_DIGEST_SIZE SECUREKEY_HMAC_SHA256_DIGEST_SIZE

//| class HardwareKey:
//|     """A key held in a hardware key store, usable but not readable.
//|
//|     The constructor argument that selects the key is **port-defined**:
//|
//|     * **espressif**: ``key_slot`` is the eFuse key block index (``0`` -
//|       ``5``, i.e. ``BLOCK_KEY0`` - ``BLOCK_KEY5``). The block must already
//|       be burned with purpose ``HMAC_UP``; construction fails otherwise, so
//|       a `HardwareKey` can never be pointed at a block reserved for flash
//|       encryption, secure boot, or the Digital Signature peripheral.
//|     """
//|
//|     def __init__(self, key_slot: int) -> None:
//|         """Bind to the hardware key identified by ``key_slot``.
//|
//|         :param int key_slot: port-defined identifier for the hardware key
//|         :raises ValueError: if ``key_slot`` does not name a usable key
//|         """
//|         ...
static mp_obj_t securekey_hardwarekey_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_key_slot };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_key_slot, MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    securekey_hardwarekey_obj_t *self = mp_obj_malloc(securekey_hardwarekey_obj_t, &securekey_hardwarekey_type);
    common_hal_securekey_hardwarekey_construct(self, args[ARG_key_slot].u_int);

    return MP_OBJ_FROM_PTR(self);
}

//|     def hmac_sha256(self, data: ReadableBuffer) -> bytes:
//|         """Compute the HMAC-SHA256 of ``data`` with this key and return the
//|         32-byte result. The key is never returned or exposed.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: the message to authenticate
//|         """
//|         ...
static mp_obj_t securekey_hardwarekey_hmac_sha256(mp_obj_t self_in, mp_obj_t data_in) {
    securekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    mp_obj_t result = mp_obj_new_bytes_of_zeros(HMAC_SHA256_DIGEST_SIZE);
    mp_obj_str_t *result_bytes = MP_OBJ_TO_PTR(result);

    common_hal_securekey_hardwarekey_hmac_sha256(self, bufinfo.buf, bufinfo.len,
        (uint8_t *)result_bytes->data, HMAC_SHA256_DIGEST_SIZE);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_2(securekey_hardwarekey_hmac_sha256_obj, securekey_hardwarekey_hmac_sha256);

//|     def verify_hmac_sha256(self, data: ReadableBuffer, mac: ReadableBuffer) -> bool:
//|         """Return ``True`` if ``mac`` is the correct HMAC-SHA256 of ``data``
//|         for this key. The comparison is constant-time.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: the message that was authenticated
//|         :param ~circuitpython_typing.ReadableBuffer mac: the MAC to check
//|         """
//|         ...
static mp_obj_t securekey_hardwarekey_verify_hmac_sha256(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t mac_in) {
    securekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t data_info;
    mp_get_buffer_raise(data_in, &data_info, MP_BUFFER_READ);
    mp_buffer_info_t mac_info;
    mp_get_buffer_raise(mac_in, &mac_info, MP_BUFFER_READ);

    bool ok = common_hal_securekey_hardwarekey_verify_hmac_sha256(self,
        data_info.buf, data_info.len, mac_info.buf, mac_info.len);
    return mp_obj_new_bool(ok);
}
static MP_DEFINE_CONST_FUN_OBJ_3(securekey_hardwarekey_verify_hmac_sha256_obj, securekey_hardwarekey_verify_hmac_sha256);

//|     key_slot: int
//|     """The port-defined key identifier this handle is bound to. (read-only)"""
static mp_obj_t securekey_hardwarekey_get_key_slot(mp_obj_t self_in) {
    securekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_securekey_hardwarekey_get_key_slot(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(securekey_hardwarekey_get_key_slot_obj, securekey_hardwarekey_get_key_slot);
MP_PROPERTY_GETTER(securekey_hardwarekey_key_slot_obj, (mp_obj_t)&securekey_hardwarekey_get_key_slot_obj);

//|     exportable: bool
//|     """Whether the raw key bytes can ever leave the hardware. Always
//|     informational -- it does not gate `hmac_sha256`.
//|
//|     On espressif this is ``False`` once the key block's ``RD_DIS`` eFuse
//|     bit is set (which ``espefuse.py`` does by default). It is meant for
//|     manufacturing-time self-test code to confirm a key block was burned as
//|     expected. (read-only)"""
static mp_obj_t securekey_hardwarekey_get_exportable(mp_obj_t self_in) {
    securekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_securekey_hardwarekey_get_exportable(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(securekey_hardwarekey_get_exportable_obj, securekey_hardwarekey_get_exportable);
MP_PROPERTY_GETTER(securekey_hardwarekey_exportable_obj, (mp_obj_t)&securekey_hardwarekey_get_exportable_obj);

static const mp_rom_map_elem_t securekey_hardwarekey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_hmac_sha256), MP_ROM_PTR(&securekey_hardwarekey_hmac_sha256_obj) },
    { MP_ROM_QSTR(MP_QSTR_verify_hmac_sha256), MP_ROM_PTR(&securekey_hardwarekey_verify_hmac_sha256_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_slot), MP_ROM_PTR(&securekey_hardwarekey_key_slot_obj) },
    { MP_ROM_QSTR(MP_QSTR_exportable), MP_ROM_PTR(&securekey_hardwarekey_exportable_obj) },
};
static MP_DEFINE_CONST_DICT(securekey_hardwarekey_locals_dict, securekey_hardwarekey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    securekey_hardwarekey_type,
    MP_QSTR_HardwareKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, securekey_hardwarekey_make_new,
    locals_dict, &securekey_hardwarekey_locals_dict
    );
