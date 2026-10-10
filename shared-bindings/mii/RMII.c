// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/RMII.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

//| class RMII:
//|     """External RMII bus wiring for the built-in ethernet MAC.
//|
//|     Pass the pins the PHY is wired to: ``mdc`` and ``mdio`` for its SMI,
//|     ``rxd0``, ``rxd1`` and ``rx_dv`` into the MAC, ``txd0``, ``txd1`` and
//|     ``tx_en`` out of it, ``refclk`` for the 50 MHz reference clock and,
//|     optionally, ``reset`` for its active-low reset. The bus can be shared
//|     by two `mii.Ethernet` interfaces on chips with more than one MAC;
//|     the PHYs are distinguished by their MDIO ``device`` number.
//|
//|     Not all chips can route RMII signals freely: ports that can't (or
//|     Zephyr ports, which take their wiring from the devicetree) raise
//|     NotImplementedError.
//|     """
//|

//|     def __init__(
//|         self,
//|         mdc: microcontroller.Pin,
//|         mdio: microcontroller.Pin,
//|         rxd0: microcontroller.Pin,
//|         rxd1: microcontroller.Pin,
//|         rx_dv: microcontroller.Pin,
//|         txd0: microcontroller.Pin,
//|         txd1: microcontroller.Pin,
//|         tx_en: microcontroller.Pin,
//|         refclk: microcontroller.Pin,
//|         reset: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Create the RMII bus on the given signals."""
//|         ...
//|
static mp_obj_t mii_rmii_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_mdc, ARG_mdio, ARG_rxd0, ARG_rxd1, ARG_rx_dv, ARG_txd0, ARG_txd1, ARG_tx_en, ARG_refclk, ARG_reset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mdc, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_mdio, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rxd0, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rxd1, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rx_dv, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_txd0, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_txd1, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_tx_en, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_refclk, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_reset, MP_ARG_KW_ONLY | MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *mdc = validate_obj_is_free_pin(args[ARG_mdc].u_obj, MP_QSTR_mdc);
    const mcu_pin_obj_t *mdio = validate_obj_is_free_pin(args[ARG_mdio].u_obj, MP_QSTR_mdio);
    const mcu_pin_obj_t *rxd0 = validate_obj_is_free_pin(args[ARG_rxd0].u_obj, MP_QSTR_rxd0);
    const mcu_pin_obj_t *rxd1 = validate_obj_is_free_pin(args[ARG_rxd1].u_obj, MP_QSTR_rxd1);
    const mcu_pin_obj_t *rx_dv = validate_obj_is_free_pin(args[ARG_rx_dv].u_obj, MP_QSTR_rx_dv);
    const mcu_pin_obj_t *txd0 = validate_obj_is_free_pin(args[ARG_txd0].u_obj, MP_QSTR_txd0);
    const mcu_pin_obj_t *txd1 = validate_obj_is_free_pin(args[ARG_txd1].u_obj, MP_QSTR_txd1);
    const mcu_pin_obj_t *tx_en = validate_obj_is_free_pin(args[ARG_tx_en].u_obj, MP_QSTR_tx_en);
    const mcu_pin_obj_t *refclk = validate_obj_is_free_pin(args[ARG_refclk].u_obj, MP_QSTR_refclk);
    const mcu_pin_obj_t *reset = NULL;
    if (args[ARG_reset].u_obj != mp_const_none) {
        reset = validate_obj_is_free_pin(args[ARG_reset].u_obj, MP_QSTR_reset);
    }

    mii_rmii_obj_t *self = m_new_obj(mii_rmii_obj_t);
    self->base.type = &mii_rmii_type;
    common_hal_mii_rmii_construct(self, mdc, mdio, rxd0, rxd1, rx_dv,
        txd0, txd1, tx_en, refclk, reset);
    return MP_OBJ_FROM_PTR(self);
}

static const mp_rom_map_elem_t mii_rmii_locals_dict_table[] = {
    // No public members; the bus is handed to mii.Ethernet.
};
static MP_DEFINE_CONST_DICT(mii_rmii_locals_dict, mii_rmii_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mii_rmii_type,
    MP_QSTR_RMII,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, &mii_rmii_make_new,
    locals_dict, &mii_rmii_locals_dict
    );
