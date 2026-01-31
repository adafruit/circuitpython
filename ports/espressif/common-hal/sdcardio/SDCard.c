// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

// ESP-specific sdcardio implementation using ESP-IDF's sdspi_host driver.
// This provides significantly better performance (~2x) compared to the generic
// shared-module implementation by using hardware-optimized SD protocol handling.
//
// The key design decision is to "borrow" the busio.SPI object passed in,
// extract its pins, deinitialize it, and then initialize sdspi_host with
// those same pins. This is necessary because ESP-IDF's SD host driver and
// CircuitPython's busio.SPI cannot share the same SPI peripheral.

#include "common-hal/sdcardio/SDCard.h"
#include "shared-bindings/sdcardio/SDCard.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "common-hal/busio/SPI.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"

#include "sdmmc_cmd.h"
#include "sd_protocol_defs.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_log.h"

#include <string.h>

static const char *TAG = "sdcardio";

// Singleton tracker for cleanup during reset
static sdcardio_sdcard_obj_t *sdcardio_singleton = NULL;

// Forward declarations
static void check_for_deinit(sdcardio_sdcard_obj_t *self);

void common_hal_sdcardio_sdcard_construct(sdcardio_sdcard_obj_t *self,
    busio_spi_obj_t *bus, const mcu_pin_obj_t *cs, int baudrate) {

    // Extract pin information from busio.SPI before we deinitialize it
    if (common_hal_busio_spi_deinited(bus)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI bus already deinitialized"));
    }

    // Get pin numbers from the busio.SPI object
    const mcu_pin_obj_t *mosi_pin = bus->MOSI;
    const mcu_pin_obj_t *miso_pin = bus->MISO;
    const mcu_pin_obj_t *clk_pin = bus->clock;
    spi_host_device_t spi_host = bus->host_id;

    if (mosi_pin == NULL || miso_pin == NULL || clk_pin == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("SPI bus must have MOSI, MISO, and CLK pins"));
    }

    // Store pin numbers
    self->mosi_pin = mosi_pin->number;
    self->miso_pin = miso_pin->number;
    self->clk_pin = clk_pin->number;
    self->cs_pin = cs->number;
    self->spi_host = spi_host;
    self->slot = -1;
    self->mounted = false;
    self->spi_bus_initialized = false;
    self->card = NULL;

    // Deinitialize the busio.SPI to release the SPI bus
    // This is necessary because ESP-IDF's sdspi_host needs exclusive access
    ESP_LOGI(TAG, "Taking over SPI bus from busio.SPI (host=%d)", spi_host);
    common_hal_busio_spi_deinit(bus);

    // Configure SPI bus for SD card
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = self->mosi_pin,
        .miso_io_num = self->miso_pin,
        .sclk_io_num = self->clk_pin,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 65536,  // 64KB for optimal performance
    };

    // Initialize SPI bus with DMA
    esp_err_t ret = spi_bus_initialize(spi_host, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("SPI bus init failed: %d"), ret);
        return;
    }
    // Track if we initialized the bus (not if it was already initialized by something else)
    self->spi_bus_initialized = (ret == ESP_OK);

    // Configure SD card slot
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = self->cs_pin;
    slot_config.host_id = spi_host;

    // Configure host - use the provided baudrate
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = spi_host;
    // Convert baudrate to kHz for ESP-IDF
    host.max_freq_khz = baudrate / 1000;
    if (host.max_freq_khz > SDMMC_FREQ_DEFAULT) {
        host.max_freq_khz = SDMMC_FREQ_DEFAULT;  // Cap at safe default
    }

    // Allocate card structure
    self->card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (self->card == NULL) {
        if (self->spi_bus_initialized) {
            spi_bus_free(spi_host);
            self->spi_bus_initialized = false;
        }
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No memory for SD card structure"));
        return;
    }

    // Initialize the SD SPI device
    ret = sdspi_host_init_device(&slot_config, &self->slot);
    if (ret != ESP_OK) {
        free(self->card);
        self->card = NULL;
        if (self->spi_bus_initialized) {
            spi_bus_free(spi_host);
            self->spi_bus_initialized = false;
        }
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("SD host init failed: %d"), ret);
        return;
    }

    // Probe and initialize the card
    ret = sdmmc_card_init(&host, self->card);
    if (ret != ESP_OK) {
        if (self->slot >= 0) {
            sdspi_host_remove_device(self->slot);
            self->slot = -1;
        }
        free(self->card);
        self->card = NULL;
        if (self->spi_bus_initialized) {
            spi_bus_free(spi_host);
            self->spi_bus_initialized = false;
        }
        if (ret == ESP_ERR_TIMEOUT) {
            mp_raise_OSError_msg(MP_ERROR_TEXT("no SD card"));
        } else if (ret == ESP_ERR_NOT_FOUND) {
            mp_raise_OSError_msg(MP_ERROR_TEXT("no SD card"));
        } else {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("SD card init failed: %d"), ret);
        }
        return;
    }

    // Mark as mounted
    self->mounted = true;

    // Claim the CS pin (SPI pins were already released by busio.SPI deinit,
    // but we need to claim them for our use)
    claim_pin(cs);

    // Set singleton for cleanup
    sdcardio_singleton = self;

    ESP_LOGI(TAG, "SD card initialized successfully [NATIVE BLOCKDEV v2]");
    if (self->card != NULL) {
        ESP_LOGI(TAG, "Card: %s, capacity: %llu sectors",
            self->card->cid.name,
            (unsigned long long)self->card->csd.capacity);
    }
}

void common_hal_sdcardio_sdcard_deinit(sdcardio_sdcard_obj_t *self) {
    if (!self->mounted) {
        return;
    }

    // Remove device from SPI slot
    if (self->slot >= 0) {
        sdspi_host_remove_device(self->slot);
        self->slot = -1;
    }

    // Free card structure
    if (self->card != NULL) {
        free(self->card);
        self->card = NULL;
    }

    // Free SPI bus only if we initialized it
    if (self->spi_bus_initialized) {
        (void)spi_bus_free((spi_host_device_t)self->spi_host);
        self->spi_bus_initialized = false;
    }

    // Release CS pin
    reset_pin_number(self->cs_pin);

    self->mounted = false;

    // Clear singleton if this was it
    if (sdcardio_singleton == self) {
        sdcardio_singleton = NULL;
    }

    ESP_LOGI(TAG, "SD card deinitialized");
}

static void check_for_deinit(sdcardio_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        raise_deinited_error();
    }
}

void common_hal_sdcardio_sdcard_check_for_deinit(sdcardio_sdcard_obj_t *self) {
    check_for_deinit(self);
}

int common_hal_sdcardio_sdcard_get_blockcount(sdcardio_sdcard_obj_t *self) {
    check_for_deinit(self);
    return (int)self->card->csd.capacity;
}

int common_hal_sdcardio_sdcard_readblocks(sdcardio_sdcard_obj_t *self,
    uint32_t start_block, mp_buffer_info_t *buf) {

    check_for_deinit(self);

    if (buf->len % 512 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer must be a multiple of 512 bytes"));
    }

    uint32_t block_count = buf->len / 512;

    esp_err_t err = sdmmc_read_sectors(self->card, buf->buf, start_block, block_count);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Read failed at block %lu, count %lu: %d",
            (unsigned long)start_block, (unsigned long)block_count, err);
        return -MP_EIO;
    }

    return 0;
}

int common_hal_sdcardio_sdcard_writeblocks(sdcardio_sdcard_obj_t *self,
    uint32_t start_block, mp_buffer_info_t *buf) {

    check_for_deinit(self);

    if (buf->len % 512 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer must be a multiple of 512 bytes"));
    }

    uint32_t block_count = buf->len / 512;

    esp_err_t err = sdmmc_write_sectors(self->card, buf->buf, start_block, block_count);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Write failed at block %lu, count %lu: %d",
            (unsigned long)start_block, (unsigned long)block_count, err);
        return -MP_EIO;
    }

    return 0;
}

int common_hal_sdcardio_sdcard_sync(sdcardio_sdcard_obj_t *self) {
    // ESP-IDF handles synchronization internally
    // Just verify the card is still mounted
    if (!self->mounted) {
        return -MP_ENODEV;
    }
    return 0;
}

// VFS block device interface functions

mp_uint_t sdcardio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {

    sdcardio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->mounted || self->card == NULL) {
        return MP_ENODEV;
    }

    esp_err_t err = sdmmc_read_sectors(self->card, buf, start_block, nblocks);
    if (err != ESP_OK) {
        return MP_EIO;
    }

    return 0;
}

mp_uint_t sdcardio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks) {

    sdcardio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (!self->mounted || self->card == NULL) {
        return MP_ENODEV;
    }

    esp_err_t err = sdmmc_write_sectors(self->card, buf, start_block, nblocks);
    if (err != ESP_OK) {
        return MP_EIO;
    }

    return 0;
}

bool sdcardio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg, mp_int_t *out_value) {
    sdcardio_sdcard_obj_t *self = MP_OBJ_TO_PTR(self_in);
    *out_value = 0;

    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_DEINIT:
            // Sync is handled by ESP-IDF internally
            break;
        case MP_BLOCKDEV_IOCTL_SYNC:
            // ESP-IDF handles sync internally
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            if (self->mounted && self->card != NULL) {
                *out_value = (mp_int_t)self->card->csd.capacity;
            }
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = 512;
            break;
        default:
            return false;
    }
    return true;
}

// Module reset function
void sdcardio_reset(void) {
    if (sdcardio_singleton != NULL) {
        common_hal_sdcardio_sdcard_deinit(sdcardio_singleton);
    }
}
