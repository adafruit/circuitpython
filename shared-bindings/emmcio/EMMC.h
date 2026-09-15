// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#define EMMC_BLOCK_SIZE 512u

extern const mp_obj_type_t emmcio_emmc_type;

// Why bring-up failed. The binding turns these into exceptions; the automount
// path just gives up.
typedef enum {
    EMMCIO_OK = 0,
    EMMCIO_ERR_PIN_PORT,      // detail: qstr of the pin the port cannot drive
    EMMCIO_ERR_SPI_IN_USE,
    EMMCIO_ERR_PIN_IN_USE,
    EMMCIO_ERR_USB_OWNED,
    EMMCIO_ERR_IN_USE,
    EMMCIO_ERR_INIT,          // detail: the MMC command that did not answer
    EMMCIO_ERR_HIGH_SPEED,    // detail: the switch step it stopped at
} emmcio_construct_result_t;

// ---- native block-device protocol -----------------------------------------
//
// The same shape sdcardio and sdioio present, so extmod/vfs_blockdev.c can
// bypass the Python method call.

// 0 on success, negative errno on failure. Never raises.
mp_uint_t emmcio_emmc_readblocks_native(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks);

// 0 on success, -MP_EROFS on an object without write_enabled=True, other
// negative errno on failure. Never raises.
mp_uint_t emmcio_emmc_writeblocks_native(mp_obj_t self_in, const uint8_t *buf,
    uint32_t start_block, uint32_t nblocks);

// false = op not implemented, the caller turns that into None.
bool emmcio_emmc_ioctl_native(mp_obj_t self_in, uint32_t cmd, uint32_t arg,
    size_t *out_value);

// Whether this object may write at all
bool emmcio_emmc_is_write_enabled(mp_obj_t self_in);
