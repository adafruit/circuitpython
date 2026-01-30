// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "sdmmc_cmd.h"
#include "driver/gpio.h"
#include "common-hal/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    sdmmc_card_t *card;
    int spi_host;
    int slot;
    gpio_num_t cs_pin;
    gpio_num_t miso_pin;
    gpio_num_t mosi_pin;
    gpio_num_t clk_pin;
    bool mounted;
    bool spi_bus_initialized;
    const char *mount_point;
} espsdcard_sdcard_obj_t;

extern const mp_obj_type_t espsdcard_sdcard_type;

// Function declarations
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
    );

void common_hal_espsdcard_sdcard_deinit(espsdcard_sdcard_obj_t *self);
bool common_hal_espsdcard_sdcard_deinited(espsdcard_sdcard_obj_t *self);
bool common_hal_espsdcard_sdcard_is_present(espsdcard_sdcard_obj_t *self);
int common_hal_espsdcard_sdcard_get_status(espsdcard_sdcard_obj_t *self);
uint32_t common_hal_espsdcard_sdcard_get_count(espsdcard_sdcard_obj_t *self);
void common_hal_espsdcard_sdcard_readblocks(espsdcard_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);
void common_hal_espsdcard_sdcard_writeblocks(espsdcard_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *buf);
int common_hal_espsdcard_sdcard_writeblocks_sync(espsdcard_sdcard_obj_t *self);
float common_hal_espsdcard_sdcard_get_capacity_mb(espsdcard_sdcard_obj_t *self);
mp_obj_t common_hal_espsdcard_sdcard_get_card_type(espsdcard_sdcard_obj_t *self);

// Reset function
void espsdcard_sdcard_deinit_all(void);
