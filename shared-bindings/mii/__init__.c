// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/__init__.h"
#include "shared-bindings/mii/Ethernet.h"
#include "shared-bindings/mii/RMII.h"

//| """
//| The `mii` module provides necessary low-level functionality for managing
//| wired ethernet connections through a Media-indepentent interface (MII)
//| bus. Use `socketpool` for communicating over the network.
//|
//| The `mii.Ethernet` object(s) are available as ``board.ETHERNET`` on boards
//| with onboard ethernet. The interface is enabled and DHCP is started
//| automatically.
//| """
//|

static const mp_rom_map_elem_t mii_module_globals_table[] = {
    // Name
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_mii) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_Ethernet),    MP_ROM_PTR(&mii_ethernet_type) },
    { MP_ROM_QSTR(MP_QSTR_RMII),        MP_ROM_PTR(&mii_rmii_type) },
};
static MP_DEFINE_CONST_DICT(mii_module_globals, mii_module_globals_table);

const mp_obj_module_t mii_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mii_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mii, mii_module);
