// SPDX-FileCopyrightText: Copyright (c) 2026 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/qspibus/__init__.h"
#include "shared-bindings/qspibus/QSPIBus.h"

//| """QSPI bus protocol for quad-SPI displays
//|
//| The `qspibus` module provides a low-level QSPI bus interface for displays
//| that use four data lines. It is analogous to `fourwire` for standard SPI.
//|
//| Use :class:`qspibus.QSPIBus` to create a bus instance.
//|
//| Example usage::
//|
//|   import board
//|   import qspibus
//|   import displayio
//|
//|   displayio.release_displays()
//|
//|   bus = qspibus.QSPIBus(
//|       clock=board.LCD_CLK,
//|       data0=board.LCD_D0,
//|       data1=board.LCD_D1,
//|       data2=board.LCD_D2,
//|       data3=board.LCD_D3,
//|       cs=board.LCD_CS,
//|       reset=board.LCD_RESET,
//|       frequency=80_000_000,
//|   )
//| """

static const mp_rom_map_elem_t qspibus_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_qspibus) },
    { MP_ROM_QSTR(MP_QSTR_QSPIBus), MP_ROM_PTR(&qspibus_qspibus_type) },
};

static MP_DEFINE_CONST_DICT(qspibus_module_globals, qspibus_module_globals_table);

const mp_obj_module_t qspibus_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&qspibus_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_qspibus, qspibus_module);
