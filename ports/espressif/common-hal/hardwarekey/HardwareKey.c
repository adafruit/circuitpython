// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

// The one port-specific step: turn an eFuse key block into a PSA key id.
// Everything after that -- hmac.new()'s use of the key id, sign() -- lives in
// shared-module/hardwarekey/HardwareKey.c.

#include <string.h>

#include "py/runtime.h"

#include "common-hal/hardwarekey/__init__.h"
#include "common-hal/hardwarekey/board.h"

#include "shared-module/hardwarekey/HardwareKey.h"

#include "esp_efuse.h"
#include "esp_heap_caps.h"

// board.h hardcodes the slot count (enum values can't be used in #if); make sure
// it still matches this chip's eFuse layout.
_Static_assert(HARDWAREKEY_EFUSE_SLOT_COUNT == EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0,
    "eFuse key block count changed; update common-hal/hardwarekey/board.h");

// Pulls in MBEDTLS_CONFIG_FILE (esp_config.h), which is what defines
// ESP_HMAC_OPAQUE_DRIVER_ENABLED / ESP_RSA_DS_DRIVER_ENABLED on chips that
// have those peripherals. Including only <psa/crypto.h> goes through the
// tf-psa-crypto config path and does NOT define either, so the opaque-driver
// headers below would compile to nothing.
#include "mbedtls/build_info.h"
#include "psa/crypto.h"
// Public header of the ESP-IDF mbedtls component's PSA opaque-key driver for
// eFuse HMAC keys (components/mbedtls/port/psa_driver/include/).
#include "psa_crypto_driver_esp_hmac_opaque.h"

#if !defined(ESP_HMAC_OPAQUE_DRIVER_ENABLED)
#error "hardwarekey requires the ESP-IDF PSA opaque HMAC driver (SOC_HMAC_SUPPORTED targets only)"
#endif

// The Digital Signature peripheral driver, by contrast, is genuinely optional:
// CIRCUITPY_HARDWAREKEY is on for every HMAC-capable chip, but not every one
// of those also has SOC_DIG_SIGN_SUPPORTED. Where it's absent,
// ESP_RSA_DS_DRIVER_ENABLED is undefined and DS purpose is simply never
// reported by hardwarekey_efuse_slot_load() below -- no build-time #error.
#if defined(ESP_RSA_DS_DRIVER_ENABLED)
#include "esp_ds.h"
#include "psa_crypto_driver_esp_rsa_ds.h"
#include "psa_crypto_driver_esp_rsa_ds_contexts.h"
#endif

// The ESP HMAC peripheral consumes a 256-bit eFuse key.
#define HMAC_KEY_BITS 256

// One PSA key is imported per eFuse block. The imports are volatile references
// (no key material) and survive a CircuitPython soft reset -- ESP-IDF initializes
// PSA once at boot and never frees it -- so this only runs once per block.
static psa_key_id_t import_efuse_hmac_key(mp_int_t slot) {
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attr, HMAC_KEY_BITS);
    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

    // Import data is a *reference* to the eFuse block, not key material. The
    // driver independently re-checks the HMAC_UP purpose and refuses anything else.
    esp_hmac_opaque_key_t keyref = { .efuse_key_id = (uint8_t)slot };

    psa_key_id_t key_id = 0;
    if (psa_import_key(&attr, (const uint8_t *)&keyref, sizeof(keyref), &key_id) != PSA_SUCCESS) {
        return 0;
    }
    return key_id;
}

bool hardwarekey_efuse_slot_load(mp_int_t slot, hardwarekey_hardwarekey_obj_t *key) {
    key->key_slot = slot;
    key->key_id = 0;
    key->purpose = HARDWAREKEY_PURPOSE_UNUSED;
    key->exportable = false;
    key->rsa_key_bits = 0;

    esp_efuse_block_t block = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + slot);
    esp_efuse_purpose_t block_purpose = esp_efuse_get_key_purpose(block);

    if (block_purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_DOWN_DIGITAL_SIGNATURE) {
        // No PSA import yet: unlike HMAC_UP, this purpose alone doesn't name a
        // full key -- the caller still has to supply ds_params via
        // load_ds_params(). Just record that the slot is provisioned for it.
        #if defined(ESP_RSA_DS_DRIVER_ENABLED)
        key->purpose = HARDWAREKEY_PURPOSE_DS;
        key->exportable = !esp_efuse_get_key_dis_read(block);
        return true;
        #else
        return false;
        #endif
    }

    if (block_purpose != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        return false;
    }

    // PSA is already initialized by ssl / hashlib, but psa_crypto_init() is
    // idempotent and keeps hardwarekey working on a build with neither.
    if (psa_crypto_init() != PSA_SUCCESS) {
        return false;
    }

    psa_key_id_t key_id = import_efuse_hmac_key(slot);
    if (key_id == 0) {
        return false;
    }

    key->key_id = key_id;
    key->purpose = HARDWAREKEY_PURPOSE_HMAC;
    key->exportable = !esp_efuse_get_key_dis_read(block);
    return true;
}

#if defined(ESP_RSA_DS_DRIVER_ENABLED)

// Persistent (non-GC) storage for a DS slot's imported key. Allocated lazily
// on first load_ds_params() and reused (overwritten in place) on a later
// call for the same slot -- the PSA RSA-DS driver only supports volatile
// keys (IDF-15427), so there is no psa_destroy_key() to pair a replacement
// with; the old PSA key id is simply abandoned along with its one HMAC-key
// eFuse block's worth of state.
typedef struct {
    esp_ds_data_t *data;
    esp_ds_data_ctx_t *ctx;
    esp_rsa_ds_opaque_key_t *opaque_key;
} ds_slot_cache_t;
static ds_slot_cache_t ds_slot_cache[HARDWAREKEY_EFUSE_SLOT_COUNT];

void common_hal_hardwarekey_hardwarekey_load_ds_params(hardwarekey_hardwarekey_obj_t *self,
    const uint8_t *ds_params, size_t ds_params_len) {
    if (ds_params_len != sizeof(esp_ds_data_t)) {
        mp_raise_ValueError(MP_ERROR_TEXT("ds_params has the wrong length"));
    }

    ds_slot_cache_t *cache = &ds_slot_cache[self->key_slot];
    if (cache->data == NULL) {
        cache->data = heap_caps_malloc(sizeof(esp_ds_data_t), MALLOC_CAP_8BIT);
        cache->ctx = heap_caps_malloc(sizeof(esp_ds_data_ctx_t), MALLOC_CAP_8BIT);
        cache->opaque_key = heap_caps_malloc(sizeof(esp_rsa_ds_opaque_key_t), MALLOC_CAP_8BIT);
        if (cache->data == NULL || cache->ctx == NULL || cache->opaque_key == NULL) {
            m_malloc_fail(sizeof(esp_ds_data_t));
        }
    }
    memcpy(cache->data, ds_params, sizeof(esp_ds_data_t));

    // rsa_length is stored as (bits / 32) - 1 (see esp_digital_signature_length_t).
    mp_int_t rsa_bits = ((mp_int_t)cache->data->rsa_length + 1) * 32;

    *cache->ctx = (esp_ds_data_ctx_t) {
        .esp_ds_data = cache->data,
        .efuse_key_id = (uint8_t)self->key_slot,
        .rsa_length_bits = (uint16_t)rsa_bits,
    };
    *cache->opaque_key = (esp_rsa_ds_opaque_key_t) {
        .ds_data_ctx = cache->ctx,
    };

    // The actual PSA import is deferred to ensure_algorithm(), on the first
    // sign()/decrypt() call -- see its declaration in shared-module for why.
    self->key_id = 0;
    self->committed_alg = PSA_ALG_NONE;
    self->rsa_key_bits = rsa_bits;
}

void common_hal_hardwarekey_hardwarekey_ensure_algorithm(hardwarekey_hardwarekey_obj_t *self,
    psa_algorithm_t alg, psa_key_usage_t usage) {
    if (self->key_id != 0) {
        if (self->committed_alg != alg) {
            mp_raise_ValueError(MP_ERROR_TEXT(
                "This key already committed to a different algorithm; call load_ds_params() again to use a different one"));
        }
        return;
    }

    ds_slot_cache_t *cache = &ds_slot_cache[self->key_slot];
    if (cache->opaque_key == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("load_ds_params() has not been called on this key"));
    }

    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_type(&attr, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attr, self->rsa_key_bits);
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_algorithm(&attr, alg);
    psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_RSA_DS_VOLATILE);

    psa_key_id_t key_id = 0;
    psa_status_t status = psa_import_key(&attr,
        (const uint8_t *)cache->opaque_key, sizeof(*cache->opaque_key), &key_id);
    if (status != PSA_SUCCESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("ds_params is invalid for this key slot"));
    }

    self->key_id = key_id;
    self->committed_alg = alg;
}

#else

void common_hal_hardwarekey_hardwarekey_load_ds_params(hardwarekey_hardwarekey_obj_t *self,
    const uint8_t *ds_params, size_t ds_params_len) {
    // Unreachable in practice: a Digital Signature purpose is never reported
    // by hardwarekey_efuse_slot_load() on a chip without the driver, and
    // shared-bindings checks `purpose` before calling this. Kept as a body
    // (not a build error) so this file still compiles on those chips.
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Digital Signature peripheral not available on this chip"));
}

void common_hal_hardwarekey_hardwarekey_ensure_algorithm(hardwarekey_hardwarekey_obj_t *self,
    psa_algorithm_t alg, psa_key_usage_t usage) {
    // Also unreachable: load_ds_params() above always raises first.
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Digital Signature peripheral not available on this chip"));
}

#endif
