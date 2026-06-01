// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/abusio/__init__.h"
#include "shared-bindings/abusio/SPI.h"

//| """Asynchronous bus I/O using native awaitables.
//|
//| Provides ``abusio.SPI``, an async-native SPI bus using DMA-IRQ completion
//| and ``circuitpy_awaitable_obj_t``.
//|
//| Example::
//|
//|     import asyncio
//|     import abusio
//|     import board
//|
//|     async def main():
//|         spi = abusio.SPI(board.SCK, board.MOSI, board.MISO)
//|         spi.configure(baudrate=1_000_000)
//|         spi.try_lock()
//|         await spi.write(b'\\x9f\\x00\\x00')
//|         spi.unlock()
//|
//|     asyncio.run(main())
//| """

static const mp_rom_map_elem_t abusio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_abusio) },
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&abusio_spi_type) },
};

static MP_DEFINE_CONST_DICT(abusio_module_globals, abusio_module_globals_table);

const mp_obj_module_t abusio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&abusio_module_globals,
};

#if CIRCUITPY_ABUSIO
MP_REGISTER_MODULE(MP_QSTR_abusio, abusio_module);
#endif
