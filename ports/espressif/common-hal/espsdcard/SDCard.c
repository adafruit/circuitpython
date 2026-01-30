// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "common-hal/espsdcard/SDCard.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "sdmmc_cmd.h"
#include "sd_protocol_defs.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_log.h"
#include <string.h>
#include <sys/stat.h>

static const char *TAG = "espsdcard";

// Singleton tracker for cleanup
static espsdcard_sdcard_obj_t *espsdcard_singleton = NULL;

void common_hal_espsdcard_sdcard_construct(
    espsdcard_sdcard_obj_t *self,
    const mcu_pin_obj_t *cs,
    const mcu_pin_obj_t *miso,
    const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *clk,
    int spi_host,
    int max_transfer_size,
    int allocation_unit_size,
    int max_files,
    bool format_if_mount_failed,
    int dma_channel
    ) {
    // Store pin assignments
    self->cs_pin = cs->number;
    self->miso_pin = miso->number;
    self->mosi_pin = mosi->number;
    self->clk_pin = clk->number;
    self->spi_host = spi_host;
    self->slot = -1;
    self->mounted = false;
    self->spi_bus_initialized = false;
    self->card = NULL;

    // Configure SPI bus
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = mosi->number,
        .miso_io_num = miso->number,
        .sclk_io_num = clk->number,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = max_transfer_size,
    };

    // Initialize SPI bus with DMA
    spi_dma_chan_t dma = (dma_channel < 0) ? SPI_DMA_CH_AUTO : (spi_dma_chan_t)dma_channel;
    esp_err_t ret = spi_bus_initialize((spi_host_device_t)spi_host, &bus_cfg, dma);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("SPI bus init failed: %d"), ret);
        return;
    }
    // Track if we initialized the bus (not if it was already initialized)
    self->spi_bus_initialized = (ret == ESP_OK);

    // Configure SD card slot
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t)cs->number;
    slot_config.host_id = (spi_host_device_t)spi_host;

    // Configure host
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = spi_host;

    // Allocate card structure
    self->card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (self->card == NULL) {
        if (self->spi_bus_initialized) {
            spi_bus_free((spi_host_device_t)spi_host);
            self->spi_bus_initialized = false;
        }
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("No memory for SD card structure"));
        return;
    }

    // Initialize the card
    ret = sdspi_host_init_device(&slot_config, &self->slot);
    if (ret != ESP_OK) {
        free(self->card);
        self->card = NULL;
        if (self->spi_bus_initialized) {
            spi_bus_free((spi_host_device_t)spi_host);
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
            spi_bus_free((spi_host_device_t)spi_host);
            self->spi_bus_initialized = false;
        }
        if (ret == ESP_ERR_TIMEOUT) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SD card not responding"));
        } else if (ret == ESP_ERR_NOT_FOUND) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SD card not found"));
        } else {
            mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("SD card init failed: %d"), ret);
        }
        return;
    }

    // Mark as mounted
    self->mounted = true;

    // Claim pins
    claim_pin(cs);
    claim_pin(miso);
    claim_pin(mosi);
    claim_pin(clk);

    // Set singleton for cleanup
    espsdcard_singleton = self;

    ESP_LOGI(TAG, "SD card mounted successfully");
    if (self->card != NULL) {
        sdmmc_card_print_info(stdout, self->card);
    }
}

void common_hal_espsdcard_sdcard_deinit(espsdcard_sdcard_obj_t *self) {
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
        // If spi_bus_free fails, it might be in use by another component.
        // Mark as not initialized regardless to prevent double-free attempts.
        (void)spi_bus_free((spi_host_device_t)self->spi_host);
        self->spi_bus_initialized = false;
    }

    // Release pins
    reset_pin_number(self->cs_pin);
    reset_pin_number(self->miso_pin);
    reset_pin_number(self->mosi_pin);
    reset_pin_number(self->clk_pin);

    self->mounted = false;

    // Clear singleton if this was it
    if (espsdcard_singleton == self) {
        espsdcard_singleton = NULL;
    }

    ESP_LOGI(TAG, "SD card deinitialized");
}

bool common_hal_espsdcard_sdcard_deinited(espsdcard_sdcard_obj_t *self) {
    return !self->mounted;
}

bool common_hal_espsdcard_sdcard_is_present(espsdcard_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        return false;
    }

    esp_err_t err = sdmmc_get_status(self->card);
    return err == ESP_OK;
}

int common_hal_espsdcard_sdcard_get_status(espsdcard_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        return 2;  // STATUS_NOT_PRESENT
    }

    esp_err_t err = sdmmc_get_status(self->card);
    return (err == ESP_OK) ? 0 : 1;  // STATUS_OK or STATUS_ERROR
}

uint32_t common_hal_espsdcard_sdcard_get_count(espsdcard_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        return 0;
    }

    // Return number of 512-byte blocks
    return (uint32_t)self->card->csd.capacity;
}

void common_hal_espsdcard_sdcard_readblocks(espsdcard_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf) {
    if (!self->mounted || self->card == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SD card not mounted"));
        return;
    }

    if (buf->len % 512 != 0) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Buffer must be multiple of 512 bytes"));
        return;
    }

    uint32_t block_count = buf->len / 512;

    esp_err_t err = sdmmc_read_sectors(self->card, buf->buf, start_block, block_count);
    if (err != ESP_OK) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Read failed: %d"), err);
    }
}

void common_hal_espsdcard_sdcard_writeblocks(espsdcard_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf) {
    if (!self->mounted || self->card == NULL) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SD card not mounted"));
        return;
    }

    if (buf->len % 512 != 0) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("Buffer must be multiple of 512 bytes"));
        return;
    }

    uint32_t block_count = buf->len / 512;

    esp_err_t err = sdmmc_write_sectors(self->card, buf->buf, start_block, block_count);
    if (err != ESP_OK) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Write failed: %d"), err);
    }
}

int common_hal_espsdcard_sdcard_writeblocks_sync(espsdcard_sdcard_obj_t *self) {
    // ESP-IDF handles synchronization internally
    // Return 0 for success
    return 0;
}

float common_hal_espsdcard_sdcard_get_capacity_mb(espsdcard_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        return 0.0f;
    }

    // Capacity is in 512-byte sectors
    uint64_t capacity_bytes = (uint64_t)self->card->csd.capacity * 512;
    return (float)capacity_bytes / (1024.0f * 1024.0f);
}

mp_obj_t common_hal_espsdcard_sdcard_get_card_type(espsdcard_sdcard_obj_t *self) {
    if (!self->mounted || self->card == NULL) {
        return mp_obj_new_str("Unknown", 7);
    }

    if (self->card->ocr & SD_OCR_SDHC_CAP) {
        return mp_obj_new_str("SDHC/SDXC", 9);
    } else {
        return mp_obj_new_str("SD", 2);
    }
}

// Module-level deinit for convenience
void espsdcard_sdcard_deinit_all(void) {
    if (espsdcard_singleton != NULL) {
        common_hal_espsdcard_sdcard_deinit(espsdcard_singleton);
    }
}
