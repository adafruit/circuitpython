// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#include "common-hal/esphmac/HMACKey.h"

#include "py/runtime.h"

#include "esp_efuse.h"
#include "esp_hmac.h"

// esp_hmac_calculate() itself does not check that key_block's eFuse purpose is
// actually HMAC_UP -- it will run against whatever key block number it's given,
// including ones burned for flash encryption, secure boot, or the DS
// peripheral. We fail closed here instead: an HMACKey can only ever be
// constructed for a block already burned for this specific purpose.
void common_hal_esphmac_hmackey_construct(esphmac_hmackey_obj_t *self, mp_int_t key_block) {
    if (key_block < 0 || key_block > 5) {
        mp_raise_ValueError(MP_ERROR_TEXT("key_block must be 0-5"));
    }

    esp_efuse_block_t block = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + key_block);
    if (esp_efuse_get_key_purpose(block) != ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        mp_raise_ValueError(MP_ERROR_TEXT("key_block is not configured for HMAC use"));
    }

    self->key_block = (uint8_t)key_block;
}

void common_hal_esphmac_hmackey_hmac(esphmac_hmackey_obj_t *self, const uint8_t *data, size_t data_len, uint8_t *out_hmac) {
    esp_err_t err = esp_hmac_calculate((hmac_key_id_t)self->key_block, data, data_len, out_hmac);
    if (err != ESP_OK) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("HMAC calculation failed"));
    }
}

mp_int_t common_hal_esphmac_hmackey_get_key_block(esphmac_hmackey_obj_t *self) {
    return self->key_block;
}

bool common_hal_esphmac_hmackey_get_read_protected(esphmac_hmackey_obj_t *self) {
    esp_efuse_block_t block = (esp_efuse_block_t)(EFUSE_BLK_KEY0 + self->key_block);
    return esp_efuse_get_key_dis_read(block);
}
