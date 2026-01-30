// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"
#include "bindings/espsdcard/SDCard.h"

//| """ESP32-native SD card interface using ESP-IDF drivers
//|
//| This module provides reliable SD card access on ESP32 boards using
//| native ESP-IDF drivers. It is a drop-in replacement for the generic
//| sdcardio module with better reliability and performance.
//|
//| API Compatibility:
//|     This module is fully compatible with sdcardio. Existing code using
//|     sdcardio can simply import espsdcard instead:
//|
//|         import espsdcard as sdcardio
//|
//|     All sdcardio code will work without modifications.
//|
//| Native API:
//|     For new code, the native API is cleaner and exposes ESP-IDF features:
//|
//|         sd = espsdcard.SDCard(
//|             cs=board.SD_CS,
//|             miso=board.SD_MISO,
//|             mosi=board.SD_MOSI,
//|             clk=board.SD_CLK
//|         )
//|
//| Example (sdcardio-compatible)::
//|
//|     import board
//|     import busio
//|     import espsdcard as sdcardio
//|     import storage
//|
//|     spi = busio.SPI(board.SD_CLK, MOSI=board.SD_MOSI, MISO=board.SD_MISO)
//|     sd = sdcardio.SDCard(spi, board.SD_CS)
//|     vfs = storage.VfsFat(sd)
//|     storage.mount(vfs, "/sd")
//|
//| Example (espsdcard native)::
//|
//|     import board
//|     import espsdcard
//|     import storage
//|
//|     sd = espsdcard.SDCard(
//|         cs=board.SD_CS,
//|         miso=board.SD_MISO,
//|         mosi=board.SD_MOSI,
//|         clk=board.SD_CLK
//|     )
//|     vfs = storage.VfsFat(sd)
//|     storage.mount(vfs, "/sd")
//| """

//| SPI2_HOST: int
//| """SPI2 host constant"""
//|
//| SPI3_HOST: int
//| """SPI3 host constant"""
//|
//| DMA_AUTO: int
//| """Automatic DMA channel selection"""
//|
//| STATUS_OK: int
//| """Card status: OK"""
//|
//| STATUS_ERROR: int
//| """Card status: Error"""
//|
//| STATUS_NOT_PRESENT: int
//| """Card status: Not present"""
//|

static const mp_rom_map_elem_t espsdcard_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espsdcard) },
    { MP_ROM_QSTR(MP_QSTR_SDCard), MP_ROM_PTR(&espsdcard_sdcard_type) },

    // SPI host constants
    { MP_ROM_QSTR(MP_QSTR_SPI2_HOST), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SPI3_HOST), MP_ROM_INT(2) },

    // DMA constants
    { MP_ROM_QSTR(MP_QSTR_DMA_AUTO), MP_ROM_INT(-1) },

    // Status constants
    { MP_ROM_QSTR(MP_QSTR_STATUS_OK), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_ERROR), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_STATUS_NOT_PRESENT), MP_ROM_INT(2) },
};
static MP_DEFINE_CONST_DICT(espsdcard_module_globals, espsdcard_module_globals_table);

const mp_obj_module_t espsdcard_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espsdcard_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_espsdcard, espsdcard_module);
