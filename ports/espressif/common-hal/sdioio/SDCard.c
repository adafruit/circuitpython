// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Jacob Rigby
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include "esp_err.h"

#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/sdioio/SDCard.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "esp_log.h"
static const char* TAG = "SDCard.c";

static bool sdio_host_init = false;

STATIC int check_pins(const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command, const uint8_t num_data, const mcu_pin_obj_t **data) {
    if (CONFIG_SOC_SDMMC_USE_GPIO_MATRIX) {
        // ESP32-S3 and P4 can use any pin for any SDMMC func in either slot
        // Default to SLOT_1 for SD cards
        ESP_LOGI(TAG, "Using chip with CONFIG_SOC_SDMMC_USE_GPIO_MATRIX");
        return SDMMC_HOST_SLOT_1;
    }
    if (command->number == GPIO_NUM_11 && clock->number == GPIO_NUM_6 && data[0]->number == GPIO_NUM_7) {
        // Might be slot 0
        if (num_data == 1 || (num_data == 4 && data[1]->number == GPIO_NUM_8 && data[2]->number == GPIO_NUM_9 && data[3]->number == GPIO_NUM_10)) {
            return SDMMC_HOST_SLOT_0;
        }
    }
    else if (command->number == GPIO_NUM_15 && clock->number == GPIO_NUM_14 && data[0]->number ==2) {
        // Might be slot 1
        if (num_data == 1 || (num_data == 4 && data[1]->number == GPIO_NUM_4 && data[2]->number == GPIO_NUM_12 && data[3]->number == GPIO_NUM_13)) {
            return SDMMC_HOST_SLOT_1;
        }
    }
    return -1;
}

void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency) {
    int sd_slot = check_pins(clock, command, num_data, data);
    if (sd_slot != SDMMC_HOST_SLOT_0 && sd_slot != SDMMC_HOST_SLOT_1) {
        // Bad pin combo
        raise_ValueError_invalid_pins();
    }

    ESP_LOGI(TAG, "Using slot %d", sd_slot);
    esp_err_t err = ESP_OK;

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    slot_config.width = 1;
    slot_config.clk = clock->number;
    slot_config.cmd = command->number;
    slot_config.d0 = data[0]->number;
    if (num_data == 4) {
        slot_config.width = 4;
        slot_config.d1 = data[1]->number;
        slot_config.d2 = data[2]->number;
        slot_config.d3 = data[3]->number;
    }

    ESP_LOGI(TAG, "slot_config:\nwidth: %d, clk: %d, cmd: %d\nd0: %d, d1: %d, d2: %d, d3: %d",
        slot_config.width, slot_config.clk, slot_config.cmd,
        slot_config.d0, slot_config.d1, slot_config.d2, slot_config.d3);

    if (!sdio_host_init) {
        err = sdmmc_host_init();
        if (err != ESP_OK) {
            mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Failed to initialize SDMMC host: %x"), err);
        }
        // sdio_host_init = true;
    }
    err = sdmmc_host_init_slot(sd_slot, &slot_config);
    if (err != ESP_OK) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Failed to initialize SDMMC slot: %x"), err);
    }
    err = sdmmc_card_init(&host, self->card);
    if (err != ESP_OK) {
        mp_raise_OSError_msg_varg(MP_ERROR_TEXT("Failed to initialize SDMMC card: %x"), err);
    }

    return;
}

uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self) {
    return self->capacity;
}

uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self) {
    return self->frequency;
}

uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self) {
    return self->num_data;
}

int common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    esp_err_t err;
    err = sdmmc_io_write_blocks(self->card, 1, start_block, bufinfo->buf, bufinfo->len);
    if (err != ESP_OK) {
        // Bad!
    }
    return 0;
}

int common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo) {
    esp_err_t err;
    err = sdmmc_io_read_blocks(self->card, 1, start_block, bufinfo->buf, bufinfo->len);
    if (err != ESP_OK) {
        // Bad!
    }
    return 0;
}

bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t frequency, uint8_t bits) {
    return true;
}

bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self) {
    return self->command == COMMON_HAL_MCU_NO_PIN;
}

void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }
    return;
}

void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self) {
    if (common_hal_sdioio_sdcard_deinited(self)) {
        return;
    }
}

void sdioio_reset() {
    return;
}
