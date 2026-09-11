// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

// The only port-specific step: turn a hardware key slot (here, an eFuse key
// block index) into a PSA key id. Everything after that -- hmac_sha256(),
// verify_hmac_sha256() -- lives in shared-module/securekey/HardwareKey.c.

#include "shared-module/securekey/HardwareKey.h"

#include "py/runtime.h"

#include "esp_efuse.h"

// Pulls in MBEDTLS_CONFIG_FILE (esp_config.h), which is what defines
// ESP_HMAC_OPAQUE_DRIVER_ENABLED on HMAC-capable chips. Including only
// <psa/crypto.h> goes through the tf-psa-crypto config path and does NOT
// define it, so the opaque-driver header below would compile to nothing.
#include "mbedtls/build_info.h"
#include "psa/crypto.h"
// Public header of the ESP-IDF mbedtls component's PSA opaque-key driver for
// eFuse HMAC keys (components/mbedtls/port/psa_driver/include/).
#include "psa_crypto_driver_esp_hmac_opaque.h"

#if !defined(ESP_HMAC_OPAQUE_DRIVER_ENABLED)
#error "securekey requires the ESP-IDF PSA opaque HMAC driver (SOC_HMAC_SUPPORTED targets only)"
#endif

// ESP32-S3 has BLOCK_KEY0..BLOCK_KEY5; other HMAC-capable chips match. Python
// key_slot 0-5 maps to EFUSE_BLK_KEY0 + key_slot.
#define EFUSE_KEY_BLOCK_COUNT 6

// The ESP HMAC peripheral consumes a 256-bit eFuse key.
#define HMAC_KEY_BITS 256

// One PSA key is imported per eFuse block on first use and reused thereafter, so
// repeated HardwareKey() construction does not accumulate PSA key slots. The
// keys are volatile references (no key material); at most EFUSE_KEY_BLOCK_COUNT
// are ever imported. On espressif this cache is safe across a CircuitPython soft
// reset because ESP-IDF initializes PSA once at boot and never frees it (see the
// raspberrypi port's reset path for the contrasting case).
static psa_key_id_t imported_key[EFUSE_KEY_BLOCK_COUNT];

void common_hal_securekey_hardwarekey_construct(securekey_hardwarekey_obj_t *self, mp_int_t key_slot) {
    if (key_slot < 0 || key_slot >= EFUSE_KEY_BLOCK_COUNT) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be %d-%d"),
            MP_QSTR_key_slot, 0, EFUSE_KEY_BLOCK_COUNT - 1);
    }

    esp_efuse_block_t block = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + key_slot);
    if (esp_efuse_get_key_purpose(block) != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        mp_raise_ValueError(MP_ERROR_TEXT("key_slot is not configured for HMAC use"));
    }

    if (imported_key[key_slot] == 0) {
        // PSA is already initialized by ssl / hashlib, but psa_crypto_init() is
        // idempotent and this keeps securekey usable on its own.
        if (psa_crypto_init() != PSA_SUCCESS) {
            mp_raise_RuntimeError(MP_ERROR_TEXT("crypto init failed"));
        }

        psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
        psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);
        psa_set_key_bits(&attr, HMAC_KEY_BITS);
        psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
        psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE);
        psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_ESP_HMAC_VOLATILE);

        // Import data is a *reference* to the eFuse block, not key material. The
        // driver independently re-checks the HMAC_UP purpose and refuses
        // anything else.
        esp_hmac_opaque_key_t keyref = { .efuse_key_id = (uint8_t)key_slot };

        psa_key_id_t key_id = 0;
        psa_status_t status = psa_import_key(&attr, (const uint8_t *)&keyref, sizeof(keyref), &key_id);
        if (status != PSA_SUCCESS) {
            mp_raise_ValueError(MP_ERROR_TEXT("key_slot is not configured for HMAC use"));
        }
        imported_key[key_slot] = key_id;
    }

    self->key_id = imported_key[key_slot];
    self->key_slot = key_slot;
    self->exportable = !esp_efuse_get_key_dis_read(block);
}
