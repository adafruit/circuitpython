// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/__init__.h"
#include "shared-bindings/mii/MDIO.h"
#include "shared-bindings/mii/Ethernet.h"
#include "shared-bindings/mii/RMII.h"

//| """
//| Media-independent interface (MII) is a family of standards to connect an Ethernet medium access control (MAC) peripheral to an Ethernet physical transceiver (PHY). Different versions have different pinouts depending on the pin count and Ethernet speed. Reduced pin count variants like reduced media-independent interface (RMII) and reduced gigabit media-independent interface (RGMII) are often used in microcontrollers to save pin count.
//|
//| A portion of the pins in MII are for the Management Data Input/Output (MDIO) which is used to configure the PHY. Like I2C, multiple PHYs can share an MDIO because the commands include an address. This module splits the Management Data Input/Output (MDIO) definition from the MII portion because the MDIO bus can be shared between PHYs.
//|
//| See: https://en.wikipedia.org/wiki/Media-independent_interface
//| """
//|

static const mp_rom_map_elem_t mii_module_globals_table[] = {
    // Name
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_mii) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_MDIO),        MP_ROM_PTR(&mii_mdio_type) },
    { MP_ROM_QSTR(MP_QSTR_Ethernet),    MP_ROM_PTR(&mii_ethernet_type) },
    { MP_ROM_QSTR(MP_QSTR_RMII),        MP_ROM_PTR(&mii_rmii_type) },
};
static MP_DEFINE_CONST_DICT(mii_module_globals, mii_module_globals_table);

const mp_obj_module_t mii_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mii_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mii, mii_module);
