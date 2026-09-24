// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/crypto_primitives/__init__.h"
#include "shared-bindings/hardwarekey/__init__.h"
#include "shared-bindings/hardwarekey/HardwareKey.h"

static void check_purpose(hardwarekey_hardwarekey_obj_t *self, hardwarekey_purpose_t purpose) {
    if (common_hal_hardwarekey_hardwarekey_get_purpose(self) != purpose) {
        mp_raise_ValueError(MP_ERROR_TEXT("Operation or feature not supported"));
    }
}

//| class HardwareKey:
//|     """A key held in a hardware key store, usable but not readable.
//|
//|     This class cannot be instantiated. Every hardware key slot the board has
//|     is exposed as a fixed `HardwareKey` in :mod:`board` -- for example
//|     ``board.EFUSE_KEY0`` -- just like pins. A slot with no key burned into it
//|     still has a `HardwareKey` object; its `purpose` is `hardwarekey.Purpose.UNUSED`.
//|
//|     Compute a MAC with a key by passing it to `hmac.new()` in place of a
//|     ``bytes`` key, or sign/decrypt with a Digital Signature key by calling
//|     `sign()` / `decrypt()`.
//|
//|     On espressif the slots are the eFuse key blocks (``BLOCK_KEY0`` -
//|     ``BLOCK_KEY5``); a slot is usable only if its block was burned with
//|     purpose ``HMAC_UP`` or ``HMAC_DOWN_DIGITAL_SIGNATURE``."""
//|

static void hardwarekey_hardwarekey_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->name != MP_QSTRnull) {
        mp_printf(print, "<HardwareKey %q>", self->name);
    } else {
        mp_printf(print, "<HardwareKey slot %d>", (int)self->key_slot);
    }
}

//|     def __bool__(self) -> bool:
//|         """``False`` if `purpose` is `hardwarekey.Purpose.UNUSED`, ``True`` otherwise.
//|         This lets you write ``if key:`` to check whether a slot is usable."""
//|         ...
static mp_obj_t hardwarekey_hardwarekey_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(common_hal_hardwarekey_hardwarekey_get_purpose(self) != HARDWAREKEY_PURPOSE_UNUSED);
        default:
            return MP_OBJ_NULL; // op not supported
    }
}

//|     key_slot: int
//|     """The port-defined key identifier this handle is bound to. On espressif,
//|     the eFuse key block index. (read-only)"""
static mp_obj_t hardwarekey_hardwarekey_get_key_slot(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hardwarekey_hardwarekey_get_key_slot(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_key_slot_obj, hardwarekey_hardwarekey_get_key_slot);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_key_slot_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_key_slot_obj);

//|     purpose: Purpose
//|     """What this key slot is provisioned for -- `hardwarekey.Purpose.HMAC_UP`,
//|     `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`, or `hardwarekey.Purpose.UNUSED`.
//|     (read-only)"""
static mp_obj_t hardwarekey_hardwarekey_get_purpose(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return cp_enum_find(&hardwarekey_purpose_type, common_hal_hardwarekey_hardwarekey_get_purpose(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_purpose_obj, hardwarekey_hardwarekey_get_purpose);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_purpose_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_purpose_obj);

//|     exportable: bool
//|     """Whether the raw key bytes can ever leave the hardware. Always
//|     informational.
//|
//|     On espressif this is ``False`` once the key block's ``RD_DIS`` eFuse
//|     bit is set (which ``espefuse.py`` does by default). It is meant for
//|     manufacturing-time self-test code to confirm a key block was burned as
//|     expected. (read-only)"""
//|
static mp_obj_t hardwarekey_hardwarekey_get_exportable(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_hardwarekey_hardwarekey_get_exportable(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_exportable_obj, hardwarekey_hardwarekey_get_exportable);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_exportable_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_exportable_obj);

//|     rsa_key_bits: int
//|     """The RSA modulus size in bits for a Digital Signature key (e.g. 2048,
//|     3072), or ``0`` for an HMAC key, or a Digital Signature key before
//|     `load_ds_params()` has been called. Signatures are ``rsa_key_bits // 8``
//|     bytes long. (read-only)"""
static mp_obj_t hardwarekey_hardwarekey_get_rsa_key_bits(mp_obj_t self_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hardwarekey_hardwarekey_get_rsa_key_bits(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_hardwarekey_get_rsa_key_bits_obj, hardwarekey_hardwarekey_get_rsa_key_bits);
MP_PROPERTY_GETTER(hardwarekey_hardwarekey_rsa_key_bits_obj, (mp_obj_t)&hardwarekey_hardwarekey_get_rsa_key_bits_obj);

//|     def load_ds_params(self, ds_params: ReadableBuffer) -> None:
//|         """Make this Digital Signature key usable with `sign()` / `decrypt()`.
//|
//|         :param ~circuitpython_typing.ReadableBuffer ds_params: the encrypted
//|           Digital Signature parameter block for this RSA key, produced at
//|           provisioning time by vendor tooling (never by this module). On
//|           espressif this is the raw ``esp_ds_data_t`` structure. Not secret --
//|           it is only usable together with this slot's eFuse key -- so it is
//|           fine to keep in a plain file, e.g. on the ``CIRCUITPY`` filesystem.
//|
//|         Only for a key whose `purpose` is `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`.
//|         Safe to call more than once; a later call replaces the loaded key and clears
//|         which algorithm it's committed to (see `sign()`).
//|
//|         :raises ValueError: if this key's `purpose` is not
//|           `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`, or ``ds_params`` is malformed
//|         """
//|         ...
static mp_obj_t hardwarekey_hardwarekey_load_ds_params(mp_obj_t self_in, mp_obj_t ds_params_in) {
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_purpose(self, HARDWAREKEY_PURPOSE_DS);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(ds_params_in, &bufinfo, MP_BUFFER_READ);

    common_hal_hardwarekey_hardwarekey_load_ds_params(self, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(hardwarekey_hardwarekey_load_ds_params_obj, hardwarekey_hardwarekey_load_ds_params);

//|     def sign(
//|         self,
//|         data: ReadableBuffer,
//|         padding: object,
//|         algorithm: object,
//|     ) -> bytes:
//|         """Sign ``data`` with this Digital Signature key and return the
//|         signature (``rsa_key_bits // 8`` bytes). The private key is never
//|         returned or exposed; the exponentiation runs entirely inside the
//|         Digital Signature peripheral. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey.sign`,
//|         with the peripheral's one supported combination.
//|
//|         Only for a key whose `purpose` is `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`,
//|         after `load_ds_params()` has been called.
//|
//|         Reusing one RSA key under more than one algorithm is a real cryptographic
//|         risk (arithmetic relations between operations under different algorithms
//|         can be exploitable), not just an inconvenience -- so this key commits to
//|         whichever of `sign()` or `decrypt()` (and, for `decrypt()`, which padding)
//|         it's first called with after `load_ds_params()`, for as long as that
//|         ``ds_params`` stays loaded. A later call requesting a different algorithm
//|         raises `ValueError`; call `load_ds_params()` again to start over.
//|
//|         :param ~circuitpython_typing.ReadableBuffer data: the message to sign
//|         :param crypto_primitives.PKCS1v15 padding: must be
//|           `crypto_primitives.PKCS1v15` -- the only padding the Digital
//|           Signature peripheral supports
//|         :param crypto_primitives.SHA256 algorithm: must be
//|           `crypto_primitives.SHA256` -- the only hash algorithm the Digital
//|           Signature peripheral supports
//|         """
//|         ...
static mp_obj_t hardwarekey_hardwarekey_sign(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_padding, ARG_algorithm };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_padding, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_algorithm, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_purpose(self, HARDWAREKEY_PURPOSE_DS);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_padding].u_obj, &crypto_primitives_pkcs1v15_type) ||
        !mp_obj_is_type(args[ARG_algorithm].u_obj, &crypto_primitives_sha256_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_PKCS1v15_space_and_space_SHA256);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    size_t sig_len = common_hal_hardwarekey_hardwarekey_get_rsa_key_bits(self) / 8;
    mp_obj_t result = mp_obj_new_bytes_of_zeros(sig_len);
    mp_obj_str_t *result_bytes = MP_OBJ_TO_PTR(result);

    common_hal_hardwarekey_hardwarekey_sign(self, bufinfo.buf, bufinfo.len,
        (uint8_t *)result_bytes->data, sig_len);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(hardwarekey_hardwarekey_sign_obj, 1, hardwarekey_hardwarekey_sign);

//|     def decrypt(
//|         self,
//|         ciphertext: ReadableBuffer,
//|         padding: object,
//|     ) -> bytes:
//|         """Decrypt ``ciphertext`` (``rsa_key_bits // 8`` bytes) with this Digital
//|         Signature key and return the recovered plaintext. The private key is
//|         never returned or exposed; the exponentiation runs entirely inside the
//|         Digital Signature peripheral. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey.decrypt`.
//|
//|         Only for a key whose `purpose` is `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`,
//|         after `load_ds_params()` has been called. Subject to the same one-algorithm-
//|         per-loaded-key rule as `sign()` -- using `crypto_primitives.PKCS1v15` here
//|         after using `crypto_primitives.OAEP` (or vice versa), or after calling
//|         `sign()`, raises `ValueError`.
//|
//|         :param ~circuitpython_typing.ReadableBuffer ciphertext: the ciphertext to decrypt
//|         :param crypto_primitives.PKCS1v15 padding: RSAES-PKCS1-v1_5 padding -- the
//|           older, legacy scheme; prefer `crypto_primitives.OAEP` for new designs
//|         :param crypto_primitives.OAEP padding: RSAES-OAEP padding. Only usable in a
//|           build with TLS 1.3 support enabled -- raises `NotImplementedError` otherwise
//|         """
//|         ...
static mp_obj_t hardwarekey_hardwarekey_decrypt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ciphertext, ARG_padding };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ciphertext, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_padding, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    hardwarekey_hardwarekey_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_purpose(self, HARDWAREKEY_PURPOSE_DS);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    psa_algorithm_t alg;
    if (mp_obj_is_type(args[ARG_padding].u_obj, &crypto_primitives_pkcs1v15_type)) {
        alg = PSA_ALG_RSA_PKCS1V15_CRYPT;
    } else if (mp_obj_is_type(args[ARG_padding].u_obj, &crypto_primitives_oaep_type)) {
        alg = PSA_ALG_RSA_OAEP(PSA_ALG_SHA_256);
    } else {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_PKCS1v15_space_or_space_OAEP);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_ciphertext].u_obj, &bufinfo, MP_BUFFER_READ);

    size_t max_len = common_hal_hardwarekey_hardwarekey_get_rsa_key_bits(self) / 8;
    uint8_t *plaintext = m_new(uint8_t, max_len);
    size_t output_len = 0;
    common_hal_hardwarekey_hardwarekey_decrypt(self, alg, bufinfo.buf, bufinfo.len,
        plaintext, max_len, &output_len);

    mp_obj_t result = mp_obj_new_bytes(plaintext, output_len);
    m_del(uint8_t, plaintext, max_len);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(hardwarekey_hardwarekey_decrypt_obj, 1, hardwarekey_hardwarekey_decrypt);

static const mp_rom_map_elem_t hardwarekey_hardwarekey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_key_slot), MP_ROM_PTR(&hardwarekey_hardwarekey_key_slot_obj) },
    { MP_ROM_QSTR(MP_QSTR_purpose), MP_ROM_PTR(&hardwarekey_hardwarekey_purpose_obj) },
    { MP_ROM_QSTR(MP_QSTR_exportable), MP_ROM_PTR(&hardwarekey_hardwarekey_exportable_obj) },
    { MP_ROM_QSTR(MP_QSTR_rsa_key_bits), MP_ROM_PTR(&hardwarekey_hardwarekey_rsa_key_bits_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_ds_params), MP_ROM_PTR(&hardwarekey_hardwarekey_load_ds_params_obj) },
    { MP_ROM_QSTR(MP_QSTR_sign), MP_ROM_PTR(&hardwarekey_hardwarekey_sign_obj) },
    { MP_ROM_QSTR(MP_QSTR_decrypt), MP_ROM_PTR(&hardwarekey_hardwarekey_decrypt_obj) },
};
static MP_DEFINE_CONST_DICT(hardwarekey_hardwarekey_locals_dict, hardwarekey_hardwarekey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hardwarekey_hardwarekey_type,
    MP_QSTR_HardwareKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    print, hardwarekey_hardwarekey_print,
    unary_op, hardwarekey_hardwarekey_unary_op,
    locals_dict, &hardwarekey_hardwarekey_locals_dict
    );
