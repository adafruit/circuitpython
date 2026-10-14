// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/MDIO.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

//| class MDIO:
//|     """Management data input/output (MDIO) bus wiring.
//|
//|     MDIO is the two wire management bus used to configure Ethernet PHYs
//|     over an `mii` variant. ``mdc`` is the management data clock and
//|     ``mdio`` is the bidirectional management data line. Configuration
//|     commands on the bus address a PHY by a number between 0 and 31, so,
//|     like I2C, multiple PHYs can share one bus.
//|     """
//|
//|     def __init__(
//|         self,
//|         mdc: microcontroller.Pin,
//|         mdio: microcontroller.Pin,
//|     ) -> None:
//|         """Create the MDIO bus on the given clock and data pins."""
//|         ...
//|
//|
static mp_obj_t mii_mdio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_mdc, ARG_mdio };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mdc, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mdio, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *mdc = validate_obj_is_free_pin(args[ARG_mdc].u_obj, MP_QSTR_mdc);
    const mcu_pin_obj_t *mdio = validate_obj_is_free_pin(args[ARG_mdio].u_obj, MP_QSTR_mdio);

    mii_mdio_obj_t *self = m_new_obj(mii_mdio_obj_t);
    self->base.type = &mii_mdio_type;
    common_hal_mii_mdio_construct(self, mdc, mdio);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_rom_map_elem_t mii_mdio_locals_dict_table[] = {
    // No public members; the bus is handed to the mii variants.
};
static MP_DEFINE_CONST_DICT(mii_mdio_locals_dict, mii_mdio_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mii_mdio_type,
    MP_QSTR_MDIO,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, &mii_mdio_make_new,
    locals_dict, &mii_mdio_locals_dict
    );
