// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "py/objproperty.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "shared-bindings/crypto_primitives/__init__.h"
#include "shared-bindings/hardwarekey/DigitalSignatureKey.h"
#include "shared-bindings/hardwarekey/HardwareKey.h"

//| class DigitalSignatureKey:
//|     """An RSA private key usable for signing and decryption, backed by a
//|     Digital Signature hardware peripheral. The private key is never returned
//|     or exposed; every operation runs entirely inside the peripheral.
//|
//|     This class cannot be instantiated directly -- get one from
//|     `load_digital_signature_key()`.
//|
//|     `sign()` / `decrypt()` mirror the method shape of
//|     :py:class:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey`
//|     (``sign``, ``decrypt``, ``key_size``), but this is not a drop-in
//|     implementation of that interface: there is intentionally no
//|     ``public_key()``, ``private_numbers()``, or ``private_bytes()`` -- the key
//|     material never leaves the Digital Signature peripheral, so there is no
//|     honest way to implement key-material export.
//|
//|     Reusing one RSA key under more than one algorithm is a real cryptographic
//|     risk (arithmetic relations between operations under different algorithms
//|     can be exploitable), not just an inconvenience -- so a `DigitalSignatureKey`
//|     commits to whichever of `sign()` or `decrypt()` (and, for `decrypt()`, which
//|     padding) it's first called with, for the rest of its lifetime. A later call
//|     requesting a different algorithm raises `ValueError`; call
//|     `load_digital_signature_key()` again for a fresh key committed to nothing yet.
//|     """
//|

static void hardwarekey_digitalsignaturekey_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    hardwarekey_digitalsignaturekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    hardwarekey_hardwarekey_obj_t *source_key = MP_OBJ_TO_PTR(self->source_key);
    if (source_key->name != MP_QSTRnull) {
        mp_printf(print, "<DigitalSignatureKey %q>", source_key->name);
    } else {
        mp_printf(print, "<DigitalSignatureKey slot %d>", (int)source_key->key_slot);
    }
}

//|     key_size: int
//|     """The RSA modulus size in bits (e.g. 2048, 3072). Signatures are
//|     ``key_size // 8`` bytes long. Mirrors
//|     :py:attr:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey.key_size`.
//|     (read-only)"""
static mp_obj_t hardwarekey_digitalsignaturekey_get_key_size(mp_obj_t self_in) {
    hardwarekey_digitalsignaturekey_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_hardwarekey_digitalsignaturekey_get_key_size(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(hardwarekey_digitalsignaturekey_get_key_size_obj, hardwarekey_digitalsignaturekey_get_key_size);
MP_PROPERTY_GETTER(hardwarekey_digitalsignaturekey_key_size_obj, (mp_obj_t)&hardwarekey_digitalsignaturekey_get_key_size_obj);

//|     def sign(
//|         self,
//|         data: ReadableBuffer,
//|         padding: object,
//|         algorithm: object,
//|     ) -> bytes:
//|         """Sign ``data`` and return the signature (``key_size // 8`` bytes).
//|         Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey.sign`,
//|         with the peripheral's one supported combination.
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
static mp_obj_t hardwarekey_digitalsignaturekey_sign(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_padding, ARG_algorithm };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_padding, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_algorithm, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    hardwarekey_digitalsignaturekey_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (!mp_obj_is_type(args[ARG_padding].u_obj, &crypto_primitives_pkcs1v15_type) ||
        !mp_obj_is_type(args[ARG_algorithm].u_obj, &crypto_primitives_sha256_type)) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("Only %q supported"), MP_QSTR_PKCS1v15_space_and_space_SHA256);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    size_t sig_len = common_hal_hardwarekey_digitalsignaturekey_get_key_size(self) / 8;
    mp_obj_t result = mp_obj_new_bytes_of_zeros(sig_len);
    mp_obj_str_t *result_bytes = MP_OBJ_TO_PTR(result);

    common_hal_hardwarekey_digitalsignaturekey_sign(self, bufinfo.buf, bufinfo.len,
        (uint8_t *)result_bytes->data, sig_len);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(hardwarekey_digitalsignaturekey_sign_obj, 1, hardwarekey_digitalsignaturekey_sign);

//|     def decrypt(
//|         self,
//|         ciphertext: ReadableBuffer,
//|         padding: object,
//|     ) -> bytes:
//|         """Decrypt ``ciphertext`` (``key_size // 8`` bytes) and return the
//|         recovered plaintext. Mirrors
//|         :py:meth:`cryptography.hazmat.primitives.asymmetric.rsa.RSAPrivateKey.decrypt`.
//|
//|         Subject to the same one-algorithm-per-lifetime rule as `sign()` --
//|         using `crypto_primitives.PKCS1v15` here after using
//|         `crypto_primitives.OAEP` (or vice versa), or after calling `sign()`,
//|         raises `ValueError`.
//|
//|         :param ~circuitpython_typing.ReadableBuffer ciphertext: the ciphertext to decrypt
//|         :param crypto_primitives.PKCS1v15 padding: RSAES-PKCS1-v1_5 padding -- the
//|           older, legacy scheme; prefer `crypto_primitives.OAEP` for new designs
//|         :param crypto_primitives.OAEP padding: RSAES-OAEP padding. Only usable in a
//|           build with TLS 1.3 support enabled -- raises `NotImplementedError` otherwise
//|         """
//|         ...
static mp_obj_t hardwarekey_digitalsignaturekey_decrypt(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ciphertext, ARG_padding };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ciphertext, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_padding, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    hardwarekey_digitalsignaturekey_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

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

    size_t max_len = common_hal_hardwarekey_digitalsignaturekey_get_key_size(self) / 8;
    uint8_t *plaintext = m_new(uint8_t, max_len);
    size_t output_len = 0;
    common_hal_hardwarekey_digitalsignaturekey_decrypt(self, alg, bufinfo.buf, bufinfo.len,
        plaintext, max_len, &output_len);

    mp_obj_t result = mp_obj_new_bytes(plaintext, output_len);
    m_del(uint8_t, plaintext, max_len);
    return result;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(hardwarekey_digitalsignaturekey_decrypt_obj, 1, hardwarekey_digitalsignaturekey_decrypt);

static const mp_rom_map_elem_t hardwarekey_digitalsignaturekey_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_key_size), MP_ROM_PTR(&hardwarekey_digitalsignaturekey_key_size_obj) },
    { MP_ROM_QSTR(MP_QSTR_sign), MP_ROM_PTR(&hardwarekey_digitalsignaturekey_sign_obj) },
    { MP_ROM_QSTR(MP_QSTR_decrypt), MP_ROM_PTR(&hardwarekey_digitalsignaturekey_decrypt_obj) },
};
static MP_DEFINE_CONST_DICT(hardwarekey_digitalsignaturekey_locals_dict, hardwarekey_digitalsignaturekey_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    hardwarekey_digitalsignaturekey_type,
    MP_QSTR_DigitalSignatureKey,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    print, hardwarekey_digitalsignaturekey_print,
    locals_dict, &hardwarekey_digitalsignaturekey_locals_dict
    );

//| def load_digital_signature_key(
//|     key: HardwareKey, ds_params: ReadableBuffer
//| ) -> DigitalSignatureKey:
//|     """Make a Digital Signature hardware key usable for signing and
//|     decryption. Mirrors
//|     :py:func:`cryptography.hazmat.primitives.serialization.load_pem_private_key`:
//|     a blob of key material in, a ready-to-use private-key object out.
//|
//|     :param HardwareKey key: must have `HardwareKey.purpose` ==
//|       `hardwarekey.Purpose.HMAC_DOWN_DIGITAL_SIGNATURE`
//|     :param ~circuitpython_typing.ReadableBuffer ds_params: the encrypted Digital
//|       Signature parameter block for this RSA key, produced at provisioning
//|       time by vendor tooling (never by this module). On espressif this is the
//|       raw ``esp_ds_data_t`` structure. Not secret -- it is only usable
//|       together with this slot's eFuse key -- so it is fine to keep in a plain
//|       file, e.g. on the ``CIRCUITPY`` filesystem.
//|     :raises ValueError: if `key`'s purpose is wrong, or ``ds_params`` is malformed
//|     """
//|     ...
static mp_obj_t hardwarekey_load_digital_signature_key(mp_obj_t key_in, mp_obj_t ds_params_in) {
    hardwarekey_hardwarekey_obj_t *key =
        mp_arg_validate_type(key_in, &hardwarekey_hardwarekey_type, MP_QSTR_key);
    if (common_hal_hardwarekey_hardwarekey_get_purpose(key) != HARDWAREKEY_PURPOSE_DS) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("key does not have the expected %q purpose"),
            MP_QSTR_HMAC_DOWN_DIGITAL_SIGNATURE);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(ds_params_in, &bufinfo, MP_BUFFER_READ);

    hardwarekey_digitalsignaturekey_obj_t *self =
        mp_obj_malloc(hardwarekey_digitalsignaturekey_obj_t, &hardwarekey_digitalsignaturekey_type);
    self->source_key = key_in;
    common_hal_hardwarekey_digitalsignaturekey_construct(self, key, bufinfo.buf, bufinfo.len);
    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_2(hardwarekey_load_digital_signature_key_obj, hardwarekey_load_digital_signature_key);
