// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/rm690b0/RM690B0.h"

//| """RM690B0 AMOLED display driver
//|
//| This module provides direct control of the RM690B0 AMOLED display controller.
//|
//| **Font Constants**
//|
//| Built-in font size constants for use with ``set_font()``::
//|
//|     rm690b0.FONT_8x8     # 8×8 monospace (smallest, debug/logs)
//|     rm690b0.FONT_16x16   # 16×16 monospace (standard UI)
//|     rm690b0.FONT_16x24   # 16×24 monospace (readable UI)
//|     rm690b0.FONT_24x24   # 24×24 monospace (headers)
//|     rm690b0.FONT_24x32   # 24×32 monospace (large headers)
//|     rm690b0.FONT_32x32   # 32×32 monospace (big displays)
//|     rm690b0.FONT_32x48   # 32×48 monospace (huge displays)
//|
//| Example::
//|
//|     import rm690b0
//|     display = rm690b0.RM690B0()
//|     display.init_display()
//|
//|     # Use font constants instead of magic numbers
//|     display.set_font(rm690b0.FONT_8x8)
//|     display.text(10, 10, "Debug info", rm690b0.WHITE)
//|
//|     display.set_font(rm690b0.FONT_24x24)
//|     display.text(10, 50, "Header", rm690b0.CYAN)
//|
//| **Color Constants**
//|
//| RGB565 color constants are also provided (WHITE, BLACK, RED, GREEN, BLUE, etc.)
//| """
//|

static const mp_rom_map_elem_t rm690b0_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rm690b0) },
    { MP_ROM_QSTR(MP_QSTR_RM690B0),   MP_ROM_PTR(&rm690b0_rm690b0_type) },

    // RGB565 convenience constants
    { MP_ROM_QSTR(MP_QSTR_WHITE),        MP_ROM_INT(0xFFFF) },
    { MP_ROM_QSTR(MP_QSTR_BLACK),        MP_ROM_INT(0x0000) },
    { MP_ROM_QSTR(MP_QSTR_DARK_GRAY),    MP_ROM_INT(0x2104) },
    { MP_ROM_QSTR(MP_QSTR_GRAY),         MP_ROM_INT(0x8410) },
    { MP_ROM_QSTR(MP_QSTR_LIGHT_GRAY),   MP_ROM_INT(0xC618) },
    { MP_ROM_QSTR(MP_QSTR_BROWN),        MP_ROM_INT(0x59E4) },
    { MP_ROM_QSTR(MP_QSTR_DARK_BROWN),   MP_ROM_INT(0x30A0) },
    { MP_ROM_QSTR(MP_QSTR_YELLOW),       MP_ROM_INT(0xFFE0) },
    { MP_ROM_QSTR(MP_QSTR_BLUE),         MP_ROM_INT(0x001F) },
    { MP_ROM_QSTR(MP_QSTR_ROYAL_BLUE),   MP_ROM_INT(0x435C) },
    { MP_ROM_QSTR(MP_QSTR_SKY_BLUE),     MP_ROM_INT(0x867D) },
    { MP_ROM_QSTR(MP_QSTR_DARK_BLUE),    MP_ROM_INT(0x0010) },
    { MP_ROM_QSTR(MP_QSTR_LIGHT_BLUE),   MP_ROM_INT(0x261F) },
    { MP_ROM_QSTR(MP_QSTR_LIGHT_VIOLET), MP_ROM_INT(0x8BFD) },
    { MP_ROM_QSTR(MP_QSTR_VIOLET),       MP_ROM_INT(0x49F1) },
    { MP_ROM_QSTR(MP_QSTR_PURPLE),       MP_ROM_INT(0x8010) },
    { MP_ROM_QSTR(MP_QSTR_PINK),         MP_ROM_INT(0xF81F) },
    { MP_ROM_QSTR(MP_QSTR_MAGENTA),      MP_ROM_INT(0xBABA) },
    { MP_ROM_QSTR(MP_QSTR_OLIVE),        MP_ROM_INT(0x8400) },
    { MP_ROM_QSTR(MP_QSTR_GREEN),        MP_ROM_INT(0x0400) },
    { MP_ROM_QSTR(MP_QSTR_DARK_GREEN),   MP_ROM_INT(0x0200) },
    { MP_ROM_QSTR(MP_QSTR_LIME),         MP_ROM_INT(0xAFE5) },
    { MP_ROM_QSTR(MP_QSTR_CYAN),         MP_ROM_INT(0x07FF) },
    { MP_ROM_QSTR(MP_QSTR_RED),          MP_ROM_INT(0xF800) },
    { MP_ROM_QSTR(MP_QSTR_ORANGE),       MP_ROM_INT(0xFC60) },

    // Font size constants
    { MP_ROM_QSTR(MP_QSTR_FONT_8x8),     MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_FONT_16x16),   MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_FONT_16x24),   MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_FONT_24x24),   MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_FONT_24x32),   MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_FONT_32x32),   MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_FONT_32x48),   MP_ROM_INT(6) },
};

static MP_DEFINE_CONST_DICT(rm690b0_module_globals, rm690b0_module_globals_table);

const mp_obj_module_t rm690b0_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rm690b0_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rm690b0, rm690b0_module);
