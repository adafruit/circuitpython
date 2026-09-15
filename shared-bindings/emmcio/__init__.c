// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/emmcio/EMMC.h"

//| """Block device access to the on-board eMMC
//|
//| The `emmcio` module exposes an eMMC chip as a block device.
//| It provides no filesystem of its own: to read files, hand an `EMMC`
//| object to `storage.VfsFat` and mount it.
//|
//| """
//|

static const mp_rom_map_elem_t emmcio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_emmcio) },
    { MP_ROM_QSTR(MP_QSTR_EMMC), MP_ROM_PTR(&emmcio_emmc_type) },
};
static MP_DEFINE_CONST_DICT(emmcio_module_globals, emmcio_module_globals_table);

const mp_obj_module_t emmcio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&emmcio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_emmcio, emmcio_module);
