// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mike Mabey
//
// SPDX-License-Identifier: MIT

#pragma once

// board.EFUSE_KEY0 .. board.EFUSE_KEY5: one fixed entry per eFuse key block,
// injected into every board's globals table through CIRCUITPY_BOARD_HARDWARE_KEYS
// (see shared-bindings/board/__init__.h). Each points at a static HardwareKey the
// startup probe (espressif_hardwarekey_init) fills in -- so, like board pins, the
// names exist at compile time and the objects are ready before user code runs.

#include "shared-module/hardwarekey/HardwareKey.h"

// Every HMAC-capable Espressif chip (S2/S3/C3/C5/C6/H2/P4) has 6 eFuse key blocks.
// HardwareKey.c static-asserts this against EFUSE_BLK_KEY_MAX - EFUSE_BLK_KEY0;
// if a future chip differs, update this list (and the assert) to match.
#define HARDWAREKEY_EFUSE_SLOT_COUNT 6

extern hardwarekey_hardwarekey_obj_t hardwarekey_efuse_keys[HARDWAREKEY_EFUSE_SLOT_COUNT];

#define CIRCUITPY_BOARD_HARDWARE_KEYS \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY0), MP_ROM_PTR(&hardwarekey_efuse_keys[0]) }, \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY1), MP_ROM_PTR(&hardwarekey_efuse_keys[1]) }, \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY2), MP_ROM_PTR(&hardwarekey_efuse_keys[2]) }, \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY3), MP_ROM_PTR(&hardwarekey_efuse_keys[3]) }, \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY4), MP_ROM_PTR(&hardwarekey_efuse_keys[4]) }, \
    { MP_ROM_QSTR(MP_QSTR_EFUSE_KEY5), MP_ROM_PTR(&hardwarekey_efuse_keys[5]) },
