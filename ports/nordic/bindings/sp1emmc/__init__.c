// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "bindings/sp1emmc/EMMC.h"
#include "sp1_emmc/automount.h"

//| """Block device access to the on-board eMMC
//|
//| The `sp1emmc` module exposes the board's soldered-down eMMC chip as a block
//| device. It provides no filesystem of its own: to read files, hand an `EMMC`
//| object to `storage.VfsFat` and mount it.
//|
//| .. note:: This module is only available on boards with an eMMC wired to the
//|     dedicated SPI peripheral, and only one `EMMC` object may exist at a time.
//| """
//|

//| def automounted() -> bool:
//|     """`True` when the eMMC has been mounted as a filesystem for you at
//|     startup, `False` when it is free for Python to open.
//|
//|     While this is `True`, constructing `EMMC` raises a `ValueError`."""
//|     ...
//|
static mp_obj_t sp1emmc_automounted(void) {
    return mp_obj_new_bool(sp1emmc_is_automounted());
}
static MP_DEFINE_CONST_FUN_OBJ_0(sp1emmc_automounted_obj, sp1emmc_automounted);

static const mp_rom_map_elem_t sp1emmc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sp1emmc) },
    { MP_ROM_QSTR(MP_QSTR_EMMC), MP_ROM_PTR(&sp1emmc_emmc_type) },
    { MP_ROM_QSTR(MP_QSTR_automounted), MP_ROM_PTR(&sp1emmc_automounted_obj) },
};
static MP_DEFINE_CONST_DICT(sp1emmc_module_globals, sp1emmc_module_globals_table);

const mp_obj_module_t sp1emmc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sp1emmc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sp1emmc, sp1emmc_module);
