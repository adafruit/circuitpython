// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/rm690b0/RM690B0.h"
#include "image_converter.h"

//| """RM690B0 AMOLED display driver with image conversion support
//|
//| This module provides direct control of the RM690B0 AMOLED display controller
//| along with hardware-accelerated image format conversion (RAW, BMP, JPEG).
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

//| def jpg_to_rgb565(jpg_data: bytes) -> tuple[bytearray, dict]:
//|     """Convert JPEG image data to RGB565 format
//|
//|     :param bytes jpg_data: JPEG file data including headers
//|     :return: Tuple of (rgb565_buffer, info_dict)
//|     :rtype: tuple[bytearray, dict]
//|
//|     The info_dict contains: width, height, data_size, bit_depth, channels, has_alpha
//|
//|     Example::
//|
//|         import rm690b0
//|         with open("/image.jpg", "rb") as f:
//|             jpg_data = f.read()
//|         rgb565_buffer, info = rm690b0.jpg_to_rgb565(jpg_data)
//|         print(f"Image: {info['width']}x{info['height']}")
//|     """
//|     ...
//|
//| def bmp_to_rgb565(bmp_data: bytes) -> tuple[bytearray, dict]:
//|     """Convert BMP image data to RGB565 format
//|
//|     :param bytes bmp_data: BMP file data including headers
//|     :return: Tuple of (rgb565_buffer, info_dict)
//|     :rtype: tuple[bytearray, dict]
//|
//|     The info_dict contains: width, height, data_size, bit_depth, channels, has_alpha
//|
//|     Example::
//|
//|         import rm690b0
//|         with open("/image.bmp", "rb") as f:
//|             bmp_data = f.read()
//|         rgb565_buffer, info = rm690b0.bmp_to_rgb565(bmp_data)
//|         print(f"Image: {info['width']}x{info['height']}")
//|     """
//|     ...
//|
static mp_obj_t rm690b0_bmp_to_rgb565(mp_obj_t bmp_data_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(bmp_data_obj, &bufinfo, MP_BUFFER_READ);

    // Parse header to get dimensions
    img_info_t info;
    img_error_t err = img_bmp_parse_header(bufinfo.buf, bufinfo.len, &info);
    if (err != IMG_OK) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Invalid BMP format (%s)"), img_error_string(err));
    }

    // Allocate output buffer
    size_t buffer_size = info.width * info.height * 2;
    vstr_t vstr;
    vstr_init_len(&vstr, buffer_size);

    // Convert - store error string before cleanup to preserve error context
    err = img_bmp_to_rgb565(bufinfo.buf, bufinfo.len,
        (uint8_t *)vstr.buf, buffer_size, &info);
    if (err != IMG_OK) {
        const char *error_text = img_error_string(err);
        vstr_clear(&vstr);
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("BMP conversion failed (%s)"), error_text);
    }

    // Create info dict
    mp_obj_t info_dict = mp_obj_new_dict(6);
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_width), MP_OBJ_NEW_SMALL_INT(info.width));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_height), MP_OBJ_NEW_SMALL_INT(info.height));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_data_size), MP_OBJ_NEW_SMALL_INT(info.data_size));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_bit_depth), MP_OBJ_NEW_SMALL_INT(info.bit_depth));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_channels), MP_OBJ_NEW_SMALL_INT(info.channels));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_has_alpha), mp_obj_new_bool(info.has_alpha));

    // Return tuple of (buffer, info)
    mp_obj_t buffer_obj = mp_obj_new_bytes_from_vstr(&vstr);
    mp_obj_t tuple[2] = {buffer_obj, info_dict};
    return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_bmp_to_rgb565_obj, rm690b0_bmp_to_rgb565);

static mp_obj_t rm690b0_jpg_to_rgb565(mp_obj_t jpg_data_obj) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(jpg_data_obj, &bufinfo, MP_BUFFER_READ);

    // Parse header to get dimensions
    img_info_t info;
    img_error_t err = img_jpg_parse_header(bufinfo.buf, bufinfo.len, &info);
    if (err != IMG_OK) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("Invalid JPEG format (%s)"), img_error_string(err));
    }

    // Allocate output buffer
    size_t buffer_size = info.width * info.height * 2;
    vstr_t vstr;
    vstr_init_len(&vstr, buffer_size);

    // Convert
    err = img_jpg_to_rgb565(bufinfo.buf, bufinfo.len,
        (uint8_t *)vstr.buf, buffer_size, &info);

    // Handle errors with centralized cleanup
    if (err != IMG_OK) {
        // Capture error info before cleanup
        const char *error_text = img_error_string(err);
        bool is_unsupported = (err == IMG_ERR_UNSUPPORTED);

        // Cleanup
        vstr_clear(&vstr);

        // Raise appropriate exception
        if (is_unsupported) {
            mp_raise_NotImplementedError(MP_ERROR_TEXT("JPEG support not compiled in"));
        } else {
            mp_raise_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("JPEG conversion failed (%s)"), error_text);
        }
    }

    // Create info dict
    mp_obj_t info_dict = mp_obj_new_dict(6);
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_width), MP_OBJ_NEW_SMALL_INT(info.width));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_height), MP_OBJ_NEW_SMALL_INT(info.height));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_data_size), MP_OBJ_NEW_SMALL_INT(info.data_size));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_bit_depth), MP_OBJ_NEW_SMALL_INT(info.bit_depth));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_channels), MP_OBJ_NEW_SMALL_INT(info.channels));
    mp_obj_dict_store(info_dict, MP_ROM_QSTR(MP_QSTR_has_alpha), mp_obj_new_bool(info.has_alpha));

    // Return tuple of (buffer, info)
    mp_obj_t buffer_obj = mp_obj_new_bytes_from_vstr(&vstr);
    mp_obj_t tuple[2] = {buffer_obj, info_dict};
    return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_1(rm690b0_jpg_to_rgb565_obj, rm690b0_jpg_to_rgb565);



static const mp_rom_map_elem_t rm690b0_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rm690b0) },
    { MP_ROM_QSTR(MP_QSTR_RM690B0),   MP_ROM_PTR(&rm690b0_rm690b0_type) },

    // Image conversion functions
    { MP_ROM_QSTR(MP_QSTR_bmp_to_rgb565), MP_ROM_PTR(&rm690b0_bmp_to_rgb565_obj) },
    { MP_ROM_QSTR(MP_QSTR_jpg_to_rgb565), MP_ROM_PTR(&rm690b0_jpg_to_rgb565_obj) },

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
