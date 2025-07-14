// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/wiznet/PIO_SPI.h"

#include "py/runtime.h"

static const mp_rom_map_elem_t wiznet_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_wiznet) },
    { MP_ROM_QSTR(MP_QSTR_PIO_SPI),   MP_ROM_PTR(&wiznet_pio_spi_type) },
};

static MP_DEFINE_CONST_DICT(wiznet_module_globals, wiznet_module_globals_table);

const mp_obj_module_t wiznet_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&wiznet_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_wiznet, wiznet_module);
