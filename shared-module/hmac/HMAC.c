// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/runtime.h"

#include "shared-bindings/hmac/HMAC.h"
#include "shared-module/hmac/__init__.h"

#include "psa/crypto.h"

#define HMAC_ALG(self) (PSA_ALG_HMAC((self)->hash_alg))

void common_hal_hmac_new(hmac_hmac_obj_t *self, const uint8_t *key, size_t key_len,
    psa_key_id_t borrowed_key_id, psa_algorithm_t hash_alg) {
    self->hash_alg = hash_alg;
    self->borrowed_key_id = borrowed_key_id;
    self->key = NULL;
    self->key_len = 0;
    if (borrowed_key_id == 0) {
        // Keep our own copy of the caller's key bytes; digest() imports it into
        // PSA on demand and destroys the import immediately afterward.
        uint8_t *copy = m_malloc(key_len == 0 ? 1 : key_len);
        memcpy(copy, key, key_len);
        self->key = copy;
        self->key_len = key_len;
    }
    vstr_init(&self->msg, 0);
}

void common_hal_hmac_update(hmac_hmac_obj_t *self, const uint8_t *data, size_t data_len) {
    vstr_add_strn(&self->msg, (const char *)data, data_len);
}

static void check_psa(psa_status_t status) {
    if (status != PSA_SUCCESS) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("HMAC operation failed"));
    }
}

void common_hal_hmac_digest(hmac_hmac_obj_t *self, uint8_t *out, size_t out_len) {
    check_psa(psa_crypto_init());

    psa_key_id_t key_id = self->borrowed_key_id;
    bool imported = false;
    if (key_id == 0) {
        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
        psa_set_key_bits(&attr, self->key_len * 8);
        psa_set_key_algorithm(&attr, HMAC_ALG(self));
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);
        psa_status_t status = psa_import_key(&attr, self->key, self->key_len, &key_id);
        check_psa(status);
        imported = true;
    }

    size_t mac_len = 0;
    psa_status_t status = psa_mac_compute(key_id, HMAC_ALG(self),
        (const uint8_t *)self->msg.buf, self->msg.len,
        out, out_len, &mac_len);

    if (imported) {
        psa_destroy_key(key_id);
    }
    check_psa(status);
}

void common_hal_hmac_copy(hmac_hmac_obj_t *self, hmac_hmac_obj_t *dest) {
    common_hal_hmac_new(dest, self->key, self->key_len, self->borrowed_key_id, self->hash_alg);
    vstr_add_strn(&dest->msg, self->msg.buf, self->msg.len);
}

size_t common_hal_hmac_get_digest_size(hmac_hmac_obj_t *self) {
    return PSA_HASH_LENGTH(self->hash_alg);
}

size_t common_hal_hmac_get_block_size(hmac_hmac_obj_t *self) {
    return PSA_HASH_BLOCK_LENGTH(self->hash_alg);
}

const char *common_hal_hmac_get_name(hmac_hmac_obj_t *self) {
    if (self->hash_alg == PSA_ALG_SHA_1) {
        return "hmac-sha1";
    }
    return "hmac-sha256";
}
