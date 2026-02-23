// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/mperrno.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/sdioio/SDCard.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t sdioio_SDCard_type;

// Construct an underlying SDIO object.
extern void common_hal_sdioio_sdcard_construct(sdioio_sdcard_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *command,
    uint8_t num_data, const mcu_pin_obj_t **data, uint32_t frequency);

extern void common_hal_sdioio_sdcard_deinit(sdioio_sdcard_obj_t *self);
extern bool common_hal_sdioio_sdcard_deinited(sdioio_sdcard_obj_t *self);

extern bool common_hal_sdioio_sdcard_configure(sdioio_sdcard_obj_t *self, uint32_t baudrate, uint8_t width);

extern void common_hal_sdioio_sdcard_unlock(sdioio_sdcard_obj_t *self);

// Return actual SDIO bus frequency.
uint32_t common_hal_sdioio_sdcard_get_frequency(sdioio_sdcard_obj_t *self);

// Return SDIO bus width.
uint8_t common_hal_sdioio_sdcard_get_width(sdioio_sdcard_obj_t *self);

// Return number of device blocks
uint32_t common_hal_sdioio_sdcard_get_count(sdioio_sdcard_obj_t *self);

// Read or write blocks - returns 0 on success or negative error code from mperrno.h
mp_negative_errno_t common_hal_sdioio_sdcard_readblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo);
mp_negative_errno_t common_hal_sdioio_sdcard_writeblocks(sdioio_sdcard_obj_t *self, uint32_t start_block, mp_buffer_info_t *bufinfo);

// Used by native vfs blockdev.
mp_negative_errno_t sdioio_sdcard_readblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
mp_negative_errno_t sdioio_sdcard_writeblocks(mp_obj_t self_in, uint8_t *buf, uint32_t start_block, uint32_t buflen);
bool sdioio_sdcard_ioctl(mp_obj_t self_in, size_t cmd, size_t arg, mp_int_t *out_value);

// This is used by the supervisor to claim SDIO devices indefinitely.
extern void common_hal_sdioio_sdcard_never_reset(sdioio_sdcard_obj_t *self);
