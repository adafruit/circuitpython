// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2019 Nick Moore for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include <stdio.h>
#include <string.h>

#include "nrfx_nvmc.h"

#include "nrf/wdt.h"
#include "supervisor/shared/safe_mode.h"

#define FLASH_PAGE_SIZE (4096)

// The only two regions of internal flash CircuitPython owns: the nvm byte array
// and the internal-flash filesystem.
static bool flash_page_is_ours(uint32_t page_addr) {
    if ((page_addr & (FLASH_PAGE_SIZE - 1)) != 0) {
        return false;
    }
    #if CIRCUITPY_INTERNAL_NVM_SIZE > 0
    if (page_addr >= CIRCUITPY_INTERNAL_NVM_START_ADDR &&
        page_addr < CIRCUITPY_INTERNAL_NVM_START_ADDR + CIRCUITPY_INTERNAL_NVM_SIZE) {
        return true;
    }
    #endif
    #if CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE > 0
    if (page_addr >= CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR &&
        page_addr < CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR + CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE) {
        return true;
    }
    #endif
    return false;
}

#ifdef BLUETOOTH_SD
#include "ble_drv.h"
#include "nrf_sdm.h"

static bool sd_is_enabled(void) {
    uint8_t sd_en = 0;
    if (__get_PRIMASK()) {
        return false;
    }
    (void)sd_softdevice_is_enabled(&sd_en);
    return sd_en;
}

static void sd_flash_operation_start(void) {
    sd_flash_operation_status = SD_FLASH_OPERATION_IN_PROGRESS;
}

static sd_flash_operation_status_t sd_flash_operation_wait_until_done(void) {
    // If the SD is not enabled, no events are generated, so just return immediately.
    if (sd_is_enabled()) {
        while (sd_flash_operation_status == SD_FLASH_OPERATION_IN_PROGRESS) {
            sd_app_evt_wait();
        }
    } else {
        sd_flash_operation_status = SD_FLASH_OPERATION_DONE;
    }
    return sd_flash_operation_status;

}

bool sd_flash_page_erase_sync(uint32_t page_number) {
    sd_flash_operation_start();
    if (sd_flash_page_erase(page_number) != NRF_SUCCESS) {
        return false;
    }
    if (sd_flash_operation_wait_until_done() == SD_FLASH_OPERATION_ERROR) {
        return false;
    }
    return true;
}

bool sd_flash_write_sync(uint32_t *dest_words, uint32_t *src_words, uint32_t num_words) {
    sd_flash_operation_start();
    if (sd_flash_write(dest_words, src_words, num_words) != NRF_SUCCESS) {
        return false;
    }
    if (sd_flash_operation_wait_until_done() == SD_FLASH_OPERATION_ERROR) {
        return false;
    }
    return true;
}

#endif

void nrf_nvm_protect_init(void) {
    #if CIRCUITPY_NRF_FLASH_PROTECT
    const struct {
        uint32_t addr;
        uint32_t size;
    } regions[] = {
        { 0, CIRCUITPY_BLE_CONFIG_START_ADDR },
        { BOOTLOADER_START_ADDR, FLASH_SIZE - BOOTLOADER_START_ADDR },
    };

    for (size_t i = 0; i < MP_ARRAY_SIZE(regions); i++) {
        if (regions[i].size == 0) {
            continue;
        }
        NRF_ACL->ACL[i].ADDR = regions[i].addr;
        NRF_ACL->ACL[i].SIZE = regions[i].size;
        NRF_ACL->ACL[i].PERM = ACL_ACL_PERM_WRITE_Disable << ACL_ACL_PERM_WRITE_Pos;
    }
    #endif
}

// The nRF52840 datasheet specifies a maximum of two writes to a flash
// location before an erase is necessary, even if the write is all
// ones (erased state).  So we can't avoid erases even if the page
// appears to be already erased (all ones), unless we keep track of
// writes to a page.

bool nrf_nvm_safe_flash_page_write(uint32_t page_addr, uint8_t *data) {
    if (!flash_page_is_ours(page_addr)) {
        // Out of bounds write that should never have been asked for,
        // reset into safe mode
        reset_into_safe_mode(SAFE_MODE_FLASH_WRITE_FAIL);
    }

    #ifdef BLUETOOTH_SD
    if (sd_is_enabled()) {
        uint32_t err_code;
        sd_flash_operation_status_t status;

        sd_flash_operation_start();
        err_code = sd_flash_page_erase(page_addr / FLASH_PAGE_SIZE);
        if (err_code != NRF_SUCCESS) {
            return false;
        }
        status = sd_flash_operation_wait_until_done();
        if (status == SD_FLASH_OPERATION_ERROR) {
            return false;
        }

        // Divide a full page into parts, because writing a full page causes an assertion failure.
        // See https://devzone.nordicsemi.com/f/nordic-q-a/40088/sd_flash_write-cause-nrf_fault_id_sd_assert/
        const size_t BLOCK_PARTS = 2;
        size_t words_to_write = FLASH_PAGE_SIZE / sizeof(uint32_t) / BLOCK_PARTS;
        for (size_t i = 0; i < BLOCK_PARTS; i++) {
            sd_flash_operation_start();
            err_code = sd_flash_write(((uint32_t *)page_addr) + i * words_to_write,
                (uint32_t *)data + i * words_to_write,
                words_to_write);
            if (err_code != NRF_SUCCESS) {
                return false;
            }
            status = sd_flash_operation_wait_until_done();
            if (status == SD_FLASH_OPERATION_ERROR) {
                return false;
            }
        }

        return true;
    }
    #endif

    // feed bootloader watchdog per page write
    bootloader_wdt_feed();

    nrfx_nvmc_page_erase(page_addr);
    nrfx_nvmc_bytes_write(page_addr, data, FLASH_PAGE_SIZE);
    return true;
}
