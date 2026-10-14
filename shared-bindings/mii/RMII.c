// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/mii/RMII.h"

#include "py/runtime.h"
#include "shared-bindings/mii/MDIO.h"
#include "shared-bindings/microcontroller/Pin.h"

//| class RMII:
//|     """Reduced media independent interface (RMII) bus wiring for the built-in ethernet MAC.
//|
//|     RMII is a pin-reduced variant of the media independent interface (MII) family: it
//|     shifts two data bits on a pair of pins at a 50 MHz reference clock instead of
//|     running a wider bus more slowly, saving pins on microcontrollers. The PHY sends
//|     data to the MAC over ``rxd0`` and ``rxd1`` and marks received bytes with
//|     ``rx_dv`` (receive data valid). The MAC sends data back over ``txd0`` and ``txd1``
//|     and frames it with ``tx_en`` (transmit enable). ``refclk`` is the 50 MHz reference
//|     clock, usually driven by an oscillator on the PHY.
//|
//|     The PHY is configured over an `mii.MDIO` management bus, which may be shared
//|     between interfaces; pass it as ``mdio``. ``device`` selects the PHY's MDIO
//|     address (0-31) on that bus, so that two `mii.Ethernet` interfaces on chips
//|     with more than one MAC can share it. Optionally, pass ``reset`` wired to
//|     the PHY's active-low reset, if it has one.
//|
//|     Not all chips can route RMII signals freely: ports that can't (or
//|     Zephyr ports, which take their wiring from the devicetree) raise
//|     NotImplementedError.
//|     """
//|

//|     def __init__(
//|         self,
//|         mdio: mii.MDIO,
//|         device: int,
//|         rxd0: microcontroller.Pin,
//|         rxd1: microcontroller.Pin,
//|         rx_dv: microcontroller.Pin,
//|         txd0: microcontroller.Pin,
//|         txd1: microcontroller.Pin,
//|         tx_en: microcontroller.Pin,
//|         refclk: microcontroller.Pin,
//|         reset: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Create the RMII bus on the given `mii.MDIO` management bus, with the PHY
//|         at its MDIO ``device`` address, and the given data signals."""
//|         ...
//|
//|
static mp_obj_t mii_rmii_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_mdio, ARG_device, ARG_rxd0, ARG_rxd1, ARG_rx_dv, ARG_txd0, ARG_txd1, ARG_tx_en, ARG_refclk, ARG_reset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mdio, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_INT },
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

    mp_arg_validate_type(args[ARG_mdio].u_obj, &mii_mdio_type, MP_QSTR_mdio);
    mii_mdio_obj_t *mdio = MP_OBJ_TO_PTR(args[ARG_mdio].u_obj);
    (void)mp_arg_validate_int_range(args[ARG_device].u_int, 0, 31, MP_QSTR_device);
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
    common_hal_mii_rmii_construct(self, mdio, (uint32_t)args[ARG_device].u_int,
        rxd0, rxd1, rx_dv, txd0, txd1, tx_en, refclk, reset);
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
