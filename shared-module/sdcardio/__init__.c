// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-module/sdcardio/__init__.h"

#include "extmod/vfs_fat.h"

#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/sdcardio/SDCard.h"

#include "supervisor/filesystem.h"

#ifdef DEFAULT_SD_CARD_DETECT
static digitalio_digitalinout_obj_t sd_card_detect_pin;
static sdcardio_sdcard_obj_t sdcard;

static mp_vfs_mount_t _sdcard_vfs;
fs_user_mount_t _sdcard_usermount;

static bool _init_error = false;
static bool _mounted = false;

#ifdef DEFAULT_SD_MOSI
static busio_spi_obj_t busio_spi_obj;
#else
#include "shared-bindings/board/__init__.h"
#endif
#endif

void sdcardio_init(void) {
    #ifdef DEFAULT_SD_CARD_DETECT
    sd_card_detect_pin.base.type = &digitalio_digitalinout_type;
    common_hal_digitalio_digitalinout_construct(&sd_card_detect_pin, DEFAULT_SD_CARD_DETECT);
    common_hal_digitalio_digitalinout_switch_to_input(&sd_card_detect_pin, PULL_UP);
    common_hal_digitalio_digitalinout_never_reset(&sd_card_detect_pin);
    #endif
}

void automount_sd_card(void) {
    #ifdef DEFAULT_SD_CARD_DETECT
    if (common_hal_digitalio_digitalinout_get_value(&sd_card_detect_pin) != DEFAULT_SD_CARD_INSERTED) {
        // No card.
        _init_error = false;
        if (_mounted) {
            // Unmount the card.
            mp_vfs_mount_t *cur = MP_STATE_VM(vfs_mount_table);
            if (cur == &_sdcard_vfs) {
                MP_STATE_VM(vfs_mount_table) = cur->next;
            } else {
                while (cur->next != &_sdcard_vfs && cur != NULL) {
                    cur = cur->next;
                }
                if (cur != NULL) {
                    cur->next = _sdcard_vfs.next;
                }
            }
            _sdcard_vfs.next = NULL;

            #ifdef DEFAULT_SD_MOSI
            common_hal_busio_spi_deinit(&busio_spi_obj);
            #endif
            _mounted = false;
        }
        return;
    } else if (_init_error || _mounted) {
        // We've already tried and failed to init the card. Don't try again.
        return;
    }

    busio_spi_obj_t *spi_obj;
    #ifndef DEFAULT_SD_MOSI
    spi_obj = MP_OBJ_TO_PTR(common_hal_board_create_spi(0));
    #else
    spi_obj = &busio_spi_obj;
    spi_obj->base.type = &busio_spi_type;
    common_hal_busio_spi_construct(spi_obj, DEFAULT_SD_SCK, DEFAULT_SD_MOSI, DEFAULT_SD_MISO, false);
    common_hal_busio_spi_never_reset(spi_obj);
    #endif
    sdcard.base.type = &sdcardio_SDCard_type;
    mp_rom_error_text_t error = sdcardio_sdcard_construct(&sdcard, spi_obj, DEFAULT_SD_CS, 25000000);
    if (error != NULL) {
        // Failed to communicate with the card.
        _mounted = false;
        _init_error = true;
        #ifdef DEFAULT_SD_MOSI
        common_hal_busio_spi_deinit(spi_obj);
        #endif
        return;
    }
    common_hal_digitalio_digitalinout_never_reset(&sdcard.cs);

    fs_user_mount_t *vfs = &_sdcard_usermount;
    vfs->base.type = &mp_fat_vfs_type;
    vfs->fatfs.drv = vfs;

    // Initialise underlying block device
    vfs->blockdev.block_size = FF_MIN_SS; // default, will be populated by call to MP_BLOCKDEV_IOCTL_BLOCK_SIZE
    mp_vfs_blockdev_init(&vfs->blockdev, &sdcard);

    // mount the block device so the VFS methods can be used
    FRESULT res = f_mount(&vfs->fatfs);
    if (res != FR_OK) {
        _mounted = false;
        _init_error = true;
        common_hal_sdcardio_sdcard_deinit(&sdcard);
        #ifdef DEFAULT_SD_MOSI
        common_hal_busio_spi_deinit(spi_obj);
        #endif
        return;
    }

    filesystem_set_concurrent_write_protection(vfs, true);
    filesystem_set_writable_by_usb(vfs, false);

    mp_vfs_mount_t *sdcard_vfs = &_sdcard_vfs;
    sdcard_vfs->str = "/sd";
    sdcard_vfs->len = 3;
    sdcard_vfs->obj = MP_OBJ_FROM_PTR(&_sdcard_usermount);
    sdcard_vfs->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = sdcard_vfs;
    _mounted = true;
    #endif // DEFAULT_SD_CARD_DETECT
}
