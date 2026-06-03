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
//| Provides ``abusio.SPI``, an async-native SPI bus
//|
//|     Example::
//|
//|         import array
//|         import asyncio
//|         import time
//|
//|         import board
//|         import digitalio
//|         import abusio
//|
//|         spi = abusio.SPI(clock=board.GP2, MOSI=board.GP3, MISO=board.GP4)
//|         spi.configure(baudrate=100_000, polarity=0, phase=0)
//|         cs = digitalio.DigitalInOut(board.GP1)
//|         cs.direction = digitalio.Direction.OUTPUT
//|         cs.value = True
//|
//|
//|         async def spi_task():
//|             while True:
//|                 await spi.lock()
//|                 # RX and TX must have separate buffers to avoid DMA race condition
//|                 # Array.array supports several data widths - see :py:array: for details.
//|                 tx_buf = array.array("h", list(range(1000)))
//|                 rx_buf = array.array("h", [0] * 1000)
//|                 tick = time.monotonic()
//|                 for i in range(10):
//|                     cs.value = False
//|                     await spi.write_readinto(tx_buf, rx_buf)
//|                     cs.value = True
//|                 tock = time.monotonic()
//|                 spi.unlock()
//|                 print(f"SPI transfer completed in {tock - tick} seconds")
//|
//|
//|         async def another_task():
//|             msg = "Hi there SPI!"
//|             while True:
//|                 for s in msg.split():
//|                     print(s, end=" ")
//|                     await asyncio.sleep(0)
//|                 print("")
//|                 await asyncio.sleep(1)
//|
//|
//|         async def main():
//|             await asyncio.gather(spi_task(), another_task())
//|
//|
//|         asyncio.run(main())
//|     """

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
