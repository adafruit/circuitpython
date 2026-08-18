// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// The native block-device face of sp1emmc.EMMC, split out of
// bindings.c so extmod/vfs_blockdev.c can reach it without dragging the rest of
// the module's headers. The same shape sdcardio and sdioio present.

#pragma once

#include "py/obj.h"

extern const mp_obj_type_t sp1emmc_emmc_type;

// 0 on success, negative errno on failure. Never raises.
mp_uint_t sp1emmc_emmc_readblocks_native(mp_obj_t self_in, uint8_t *buf,
    uint32_t start_block, uint32_t nblocks);

// 0 on success, -MP_EROFS on an object without write_enabled=True, other
// negative errno on failure. Never raises.
mp_uint_t sp1emmc_emmc_writeblocks_native(mp_obj_t self_in, const uint8_t *buf,
    uint32_t start_block, uint32_t nblocks);

// false = op not implemented, the caller turns that into None.
bool sp1emmc_emmc_ioctl_native(mp_obj_t self_in, uint32_t cmd, uint32_t arg,
    size_t *out_value);

// Whether this object may write at all
bool sp1emmc_emmc_is_write_enabled(mp_obj_t self_in);
