// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "common-hal/nvm/ByteArray.h"
#include "shared-bindings/nvm/ByteArray.h"

#include <string.h>

#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

#define NVM_PARTITION nvm_partition

#if FIXED_PARTITION_EXISTS(NVM_PARTITION)

static const struct flash_area *nvm_area = NULL;
static size_t nvm_erase_size = 0;

static bool ensure_nvm_open(void) {
    if (nvm_area != NULL) {
        return true;
    }
    int rc = flash_area_open(FIXED_PARTITION_ID(NVM_PARTITION), &nvm_area);
    if (rc != 0) {
        return false;
    }

    const struct device *dev = flash_area_get_device(nvm_area);
    struct flash_pages_info info;
    flash_get_page_info_by_offs(dev, nvm_area->fa_off, &info);
    nvm_erase_size = info.size;

    return true;
}

uint32_t common_hal_nvm_bytearray_get_length(const nvm_bytearray_obj_t *self) {
    if (!ensure_nvm_open()) {
        return 0;
    }
    return nvm_area->fa_size;
}

bool common_hal_nvm_bytearray_set_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {
    if (!ensure_nvm_open()) {
        return false;
    }

    uint32_t address = start_index;
    while (len > 0) {
        uint32_t page_offset = address % nvm_erase_size;
        uint32_t page_start = address - page_offset;
        uint32_t write_len = MIN(len, nvm_erase_size - page_offset);

        uint8_t *buffer = m_malloc(nvm_erase_size);
        if (buffer == NULL) {
            return false;
        }

        // Read the full erase page.
        int rc = flash_area_read(nvm_area, page_start, buffer, nvm_erase_size);
        if (rc != 0) {
            m_free(buffer);
            return false;
        }

        // Modify the relevant bytes.
        memcpy(buffer + page_offset, values, write_len);

        // Erase the page.
        rc = flash_area_erase(nvm_area, page_start, nvm_erase_size);
        if (rc != 0) {
            m_free(buffer);
            return false;
        }

        // Write the page back.
        rc = flash_area_write(nvm_area, page_start, buffer, nvm_erase_size);
        m_free(buffer);
        if (rc != 0) {
            return false;
        }

        address += write_len;
        values += write_len;
        len -= write_len;
    }
    return true;
}

void common_hal_nvm_bytearray_get_bytes(const nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    if (!ensure_nvm_open()) {
        return;
    }
    flash_area_read(nvm_area, start_index, values, len);
}

#endif // FIXED_PARTITION_EXISTS(NVM_PARTITION)
