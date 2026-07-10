// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/memorymap/__init__.h"

//| """Raw memory map access
//|
//| The `memorymap` module allows you to read and write memory addresses in the
//| address space seen from the processor running CircuitPython. It is usually
//| the physical address space.
//| """
//|
//| def AddressRange(start_address: int, length: int) -> memoryview:
//|     r"""Presents a range of addresses as a memoryview.
//|
//|
//|     The addresses may access memory or memory mapped peripherals.
//|
//|     Some address ranges may be protected by CircuitPython to prevent errors.
//|     An exception will be raised when constructing an AddressRange for an
//|     invalid or protected address.
//|
//|     Multiple AddressRanges may overlap. There is no "claiming" of addresses.
//|
//|     *New in CircuitPython 11.0:* This returns a `memoryview` object instead of an ``AddressRange`` object.
//|     A `memoryview` object can be cast, used as an argument to `struct.pack` or `struct.unpack`, etc.
//|
//|     *New in CircuitPython 11.0:* On RP2 family microcontrollers, I/O ranges can be accessed as 8- or 16-
//|     bit values. When writing, the smaller value is repeated 2 or 4 times to form the value actually
//|     transferred, as documented in the MCU datasheet.
//|
//|     A `memoryview` object can be cast, used as an argument to `struct.pack` or `struct.unpack`, etc.
//|     Example usage on ESP32-S2::
//|
//|        import memorymap
//|        rtc_slow_mem = memorymap.AddressRange(start=0x50000000, length=0x2000)
//|        rtc_slow_mem[0:3] = b"\xcc\x10\x00"
//|
//|     Example I/O register usage on RP2040::
//|
//|        import binascii
//|        import board
//|        import digitalio
//|        import memorymap
//|
//|        pads_bank0 = memorymap.AddressRange(start=0x4001C000, length=0x4000).cast('L')
//|        pads_bank0_xor = memorymap.AddressRange(start=0x4001C000+0x1000, length=0x4000).cast('L')
//|
//|        def rp2040_set_pad_drive(p, d):
//|            pad_ctrl = pads_bank0[p+1]
//|            # Pad control register is updated using an MP-safe atomic XOR
//|            pad_ctrl ^= (d << 4)
//|            pad_ctrl &= 0x00000030
//|            pads_bank0_xor[p+1] = pad_ctrl
//|
//|        def rp2040_get_pad_drive(p):
//|            pad_ctrl = pads_bank0[p+1]
//|            return (pad_ctrl >> 4) & 0x3
//|
//|        # set GPIO16 pad drive strength to 12 mA
//|        rp2040_set_pad_drive(16, 3)
//|
//|        # print GPIO16 pad drive strength
//|        print(rp2040_get_pad_drive(16))
//|
//|     Note that the above example does **not** work on RP2350 because base
//|     address and  organization of the "pads0" registers changed compared
//|     to the RP2040.
//|     """
//|

static mp_obj_t memorymap_addressrange(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_start, ARG_length };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_length, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_INT },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Argument start is a pointer into the address map, so we validate it here because a
    // signed int argument will overflow if it is in the upper half of the map.
    size_t start = mp_obj_get_uint(args[ARG_start].u_obj);
    size_t length =
        mp_arg_validate_int_min(args[ARG_length].u_int, 1, MP_QSTR_length);

    // Check for address range wrap here as this can break port-specific code due to size_t overflow.
    if (start + length - 1 < start) {
        mp_raise_ValueError(MP_ERROR_TEXT("Address range wraps around"));
    }

    return common_hal_memorymap_addressrange_make_new((uint8_t *)start, length);
}
MP_DEFINE_CONST_FUN_OBJ_KW(memorymap_addressrange_obj, 0, memorymap_addressrange);

static const mp_rom_map_elem_t memorymap_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_memorymap) },
    { MP_ROM_QSTR(MP_QSTR_AddressRange),   MP_ROM_PTR(&memorymap_addressrange_obj) },
};

static MP_DEFINE_CONST_DICT(memorymap_module_globals, memorymap_module_globals_table);

const mp_obj_module_t memorymap_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&memorymap_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_memorymap, memorymap_module);
