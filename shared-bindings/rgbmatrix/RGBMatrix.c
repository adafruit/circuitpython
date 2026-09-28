// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "shared-module/framebufferio/__init__.h"
#include "shared-module/framebufferio/FramebufferDisplay.h"

//| class RGBMatrix:
//|     """Displays an in-memory framebuffer to a HUB75-style RGB LED matrix."""
//|
//|     def __init__(
//|         self,
//|         *,
//|         width: int,
//|         bit_depth: int,
//|         rgb_pins: Sequence[digitalio.DigitalInOut],
//|         addr_pins: Sequence[digitalio.DigitalInOut],
//|         clock_pin: digitalio.DigitalInOut,
//|         latch_pin: digitalio.DigitalInOut,
//|         output_enable_pin: digitalio.DigitalInOut,
//|         doublebuffer: bool = True,
//|         framebuffer: Optional[WriteableBuffer] = None,
//|         height: int = 0,
//|         tile: int = 1,
//|         serpentine: bool = True,
//|     ) -> None:
//|         """Create a RGBMatrix object with the given attributes.  The height of
//|         the display is determined by the number of rgb and address pins and the number of tiles:
//|         ``len(rgb_pins) // 3 * 2 ** len(address_pins) * abs(tile)``.  With 6 RGB pins, 4
//|         address lines, and a single matrix, the display will be 32 pixels tall.  If the optional height
//|         parameter is specified and is not 0, it is checked against the calculated
//|         height.
//|
//|         Tiled matrices, those with more than one panel, must be laid out `in a specific order, as detailed in the guide
//|         <https://learn.adafruit.com/rgb-led-matrices-matrix-panels-with-circuitpython/advanced-multiple-panels>`_.
//|
//|         At least 6 RGB pins and 5 address pins are supported, for common panels with up to 64 rows of pixels.
//|         Some microcontrollers may support more, up to a soft limit of 30 RGB pins and 8 address pins.
//|
//|         The RGB pins must be within a single "port" and performance and memory
//|         usage are best when they are all within "close by" bits of the port.
//|         The clock pin must also be on the same port as the RGB pins.  See the
//|         documentation of the underlying protomatter C library for more
//|         information.  Generally, Adafruit's interface boards are designed so
//|         that these requirements are met when matched with the intended
//|         microcontroller board.  For instance, the Feather M4 Express works
//|         together with the RGB Matrix Feather.
//|
//|         The framebuffer is in "RGB565" format.
//|
//|         "RGB565" means that it is organized as a series of 16-bit numbers
//|         where the highest 5 bits are interpreted as red, the next 6 as
//|         green, and the final 5 as blue.  The object can be any buffer, but
//|         `array.array` and ``ulab.ndarray`` objects are most often useful.
//|         To update the content, modify the framebuffer and call refresh.
//|
//|         If a framebuffer is not passed in, one is allocated and initialized
//|         to all black.  In any case, the framebuffer can be retrieved
//|         by passing the RGBMatrix object to memoryview().
//|
//|         If doublebuffer is False, some memory is saved, but the display may
//|         flicker during updates.
//|
//|         A RGBMatrix is often used in conjunction with a
//|         `framebufferio.FramebufferDisplay`.
//|
//|         On boards designed for use with RGBMatrix panels, ``board.MTX_ADDRESS`` is a tuple of all the address pins, and ``board.MTX_COMMON`` is a dictionary with ``rgb_pins``, ``clock_pin``, ``latch_pin``, and ``output_enable_pin``.
//|         For panels that use fewer than the maximum number of address pins, "slice" ``MTX_ADDRESS`` to get the correct number of address pins.
//|         Using these board properties makes calling the constructor simpler and more portable:
//|
//|         .. code-block:: python
//|
//|             matrix = rgbmatrix.RGBMatrix(..., addr_pins=board.MTX_ADDRESS[:4], **board.MTX_COMMON)
//|
//|         :param int width: The overall width of the whole matrix in pixels. For a matrix with multiple panels in row, this is the width of a single panel times the number of panels across.
//|         :param int tile: In a multi-row matrix, the number of rows of panels
//|         :param int bit_depth: The color depth of the matrix. A value of 1 gives 8 colors, a value of 2 gives 64 colors, and so on. Increasing bit depth increases the CPU and RAM usage of the RGBMatrix, and may lower the panel refresh rate. The framebuffer is always in RGB565 format regardless of the bit depth setting
//|         :param bool serpentine: In a multi-row matrix, True when alternate rows of panels are rotated 180°, which can reduce wiring length
//|         :param Sequence[digitalio.DigitalInOut] rgb_pins: The matrix's RGB pins in the order ``(R1,G1,B1,R2,G2,B2...)``
//|         :param Sequence[digitalio.DigitalInOut] addr_pins: The matrix's address pins in the order ``(A,B,C,D...)``
//|         :param digitalio.DigitalInOut clock_pin: The matrix's clock pin
//|         :param digitalio.DigitalInOut latch_pin: The matrix's latch pin
//|         :param digitalio.DigitalInOut output_enable_pin: The matrix's output enable pin
//|         :param bool doublebuffer: True if the output is double-buffered
//|         :param Optional[WriteableBuffer] framebuffer: A pre-allocated framebuffer to use. If unspecified, a framebuffer is allocated
//|         :param int height: The optional overall height of the whole matrix in pixels. This value is not required because it can be calculated as described above.
//|         """
//|

static mp_obj_t rgbmatrix_rgbmatrix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_width, ARG_bit_depth, ARG_rgb_list, ARG_addr_list,
           ARG_clock_pin, ARG_latch_pin, ARG_output_enable_pin, ARG_doublebuffer, ARG_framebuffer, ARG_height, ARG_tile, ARG_serpentine };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_width, MP_ARG_INT | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_bit_depth, MP_ARG_INT | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_rgb_pins, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_addr_pins, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_clock_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_latch_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_output_enable_pin, MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY },
        { MP_QSTR_doublebuffer, MP_ARG_BOOL | MP_ARG_KW_ONLY, { .u_bool = true } },
        { MP_QSTR_framebuffer, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = mp_const_none } },
        { MP_QSTR_height, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 0 } },
        { MP_QSTR_tile, MP_ARG_INT | MP_ARG_KW_ONLY, { .u_int = 1 } },
        { MP_QSTR_serpentine, MP_ARG_BOOL | MP_ARG_KW_ONLY, { .u_bool = true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    rgbmatrix_rgbmatrix_obj_t *self = &allocate_display_bus_or_raise()->rgbmatrix;
    self->base.type = &rgbmatrix_RGBMatrix_type;

    uint8_t rgb_count, addr_count;
    const mcu_pin_obj_t *rgb_pins[MP_ARRAY_SIZE(self->rgb_pins)];
    const mcu_pin_obj_t *addr_pins[MP_ARRAY_SIZE(self->addr_pins)];
    const mcu_pin_obj_t *clock_pin = validate_obj_is_free_pin(args[ARG_clock_pin].u_obj, MP_QSTR_clock_pin);
    const mcu_pin_obj_t *latch_pin = validate_obj_is_free_pin(args[ARG_latch_pin].u_obj, MP_QSTR_latch_pin);
    const mcu_pin_obj_t *output_enable_pin = validate_obj_is_free_pin(args[ARG_output_enable_pin].u_obj, MP_QSTR_output_enable_pin);
    mp_int_t bit_depth = mp_arg_validate_int_range(args[ARG_bit_depth].u_int, 1, 6, MP_QSTR_bit_depth);

    validate_list_is_free_pins(MP_QSTR_rgb_pins, rgb_pins, MP_ARRAY_SIZE(rgb_pins), args[ARG_rgb_list].u_obj, &rgb_count);
    validate_list_is_free_pins(MP_QSTR_addr_pins, addr_pins, MP_ARRAY_SIZE(addr_pins), args[ARG_addr_list].u_obj, &addr_count);

    if (rgb_count % 6) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Must use a multiple of 6 rgb pins, not %d"), rgb_count);
    }

    int tile = mp_arg_validate_int_min(args[ARG_tile].u_int, 1, MP_QSTR_tile);

    int computed_height = (rgb_count / 3) * (1 << (addr_count)) * tile;
    if (args[ARG_height].u_int != 0) {
        if (computed_height != args[ARG_height].u_int) {
            mp_raise_ValueError_varg(
                MP_ERROR_TEXT("%d address pins, %d rgb pins and %d tiles indicate a height of %d, not %d"), addr_count, rgb_count, tile, computed_height, args[ARG_height].u_int);
        }
    }

    mp_int_t width = mp_arg_validate_int_min(args[ARG_width].u_int, 1, MP_QSTR_width);

    common_hal_rgbmatrix_rgbmatrix_construct(self,
        width,
        bit_depth,
        rgb_count, rgb_pins,
        addr_count, addr_pins,
        clock_pin, latch_pin, output_enable_pin,
        args[ARG_doublebuffer].u_bool,
        args[ARG_framebuffer].u_obj, tile, args[ARG_serpentine].u_bool, NULL);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Free the resources (pins, timers, etc.) associated with this
//|         rgbmatrix instance.  After deinitialization, no further operations
//|         may be performed."""
//|         ...
//|
static mp_obj_t rgbmatrix_rgbmatrix_deinit(mp_obj_t self_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    common_hal_rgbmatrix_rgbmatrix_deinit(self);
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(rgbmatrix_rgbmatrix_deinit_obj, rgbmatrix_rgbmatrix_deinit);

static void check_for_deinit(rgbmatrix_rgbmatrix_obj_t *self) {
    if (!self->protomatter.rgbPins) {
        raise_deinited_error();
    }
}

//|     brightness: float
//|     """In the current implementation, 0.0 turns the display off entirely
//|     and any other value up to 1.0 turns the display on fully."""
//|
static mp_obj_t rgbmatrix_rgbmatrix_get_brightness(mp_obj_t self_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_rgbmatrix_rgbmatrix_get_paused(self)? 0.0f : 1.0f);
}
MP_DEFINE_CONST_FUN_OBJ_1(rgbmatrix_rgbmatrix_get_brightness_obj, rgbmatrix_rgbmatrix_get_brightness);

static mp_obj_t rgbmatrix_rgbmatrix_set_brightness(mp_obj_t self_in, mp_obj_t value_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    check_for_deinit(self);
    mp_float_t brightness = mp_obj_get_float(value_in);
    if (brightness < 0.0f || brightness > 1.0f) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be %d-%d"), MP_QSTR_brightness, 0, 1);
    }
    common_hal_rgbmatrix_rgbmatrix_set_paused(self, brightness <= 0);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rgbmatrix_rgbmatrix_set_brightness_obj, rgbmatrix_rgbmatrix_set_brightness);

MP_PROPERTY_GETSET(rgbmatrix_rgbmatrix_brightness_obj,
    (mp_obj_t)&rgbmatrix_rgbmatrix_get_brightness_obj,
    (mp_obj_t)&rgbmatrix_rgbmatrix_set_brightness_obj);

//|     def refresh(self) -> None:
//|         """Transmits the color data in the buffer to the pixels so that
//|         they are shown."""
//|         ...
//|
static mp_obj_t rgbmatrix_rgbmatrix_refresh(mp_obj_t self_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    check_for_deinit(self);
    common_hal_rgbmatrix_rgbmatrix_refresh(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rgbmatrix_rgbmatrix_refresh_obj, rgbmatrix_rgbmatrix_refresh);

//|     width: int
//|     """The width of the display, in pixels"""
static mp_obj_t rgbmatrix_rgbmatrix_get_width(mp_obj_t self_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rgbmatrix_rgbmatrix_get_width(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rgbmatrix_rgbmatrix_get_width_obj, rgbmatrix_rgbmatrix_get_width);
MP_PROPERTY_GETTER(rgbmatrix_rgbmatrix_width_obj,
    (mp_obj_t)&rgbmatrix_rgbmatrix_get_width_obj);

//|     height: int
//|     """The height of the display, in pixels"""
//|
//|
static mp_obj_t rgbmatrix_rgbmatrix_get_height(mp_obj_t self_in) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rgbmatrix_rgbmatrix_get_height(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rgbmatrix_rgbmatrix_get_height_obj, rgbmatrix_rgbmatrix_get_height);

MP_PROPERTY_GETTER(rgbmatrix_rgbmatrix_height_obj,
    (mp_obj_t)&rgbmatrix_rgbmatrix_get_height_obj);

static const mp_rom_map_elem_t rgbmatrix_rgbmatrix_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rgbmatrix_rgbmatrix_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&rgbmatrix_rgbmatrix_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&rgbmatrix_rgbmatrix_brightness_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh), MP_ROM_PTR(&rgbmatrix_rgbmatrix_refresh_obj) },
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&rgbmatrix_rgbmatrix_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&rgbmatrix_rgbmatrix_height_obj) },
};
static MP_DEFINE_CONST_DICT(rgbmatrix_rgbmatrix_locals_dict, rgbmatrix_rgbmatrix_locals_dict_table);

static void rgbmatrix_rgbmatrix_get_bufinfo(mp_obj_t self_in, mp_buffer_info_t *bufinfo) {
    common_hal_rgbmatrix_rgbmatrix_get_bufinfo(self_in, bufinfo);
}

// These version exists so that the prototype matches the protocol,
// avoiding a type cast that can hide errors
static void rgbmatrix_rgbmatrix_swapbuffers(mp_obj_t self_in, uint8_t *dirty_row_bitmap) {
    (void)dirty_row_bitmap;
    common_hal_rgbmatrix_rgbmatrix_refresh(self_in);
}

static void rgbmatrix_rgbmatrix_deinit_proto(mp_obj_t self_in) {
    common_hal_rgbmatrix_rgbmatrix_deinit(self_in);
}

static float rgbmatrix_rgbmatrix_get_brightness_proto(mp_obj_t self_in) {
    return common_hal_rgbmatrix_rgbmatrix_get_paused(self_in) ? 0.0f : 1.0f;
}

static bool rgbmatrix_rgbmatrix_set_brightness_proto(mp_obj_t self_in, mp_float_t value) {
    common_hal_rgbmatrix_rgbmatrix_set_paused(self_in, value <= 0);
    return true;
}

static int rgbmatrix_rgbmatrix_get_width_proto(mp_obj_t self_in) {
    return common_hal_rgbmatrix_rgbmatrix_get_width(self_in);
}

static int rgbmatrix_rgbmatrix_get_height_proto(mp_obj_t self_in) {
    return common_hal_rgbmatrix_rgbmatrix_get_height(self_in);
}

static int rgbmatrix_rgbmatrix_get_color_depth_proto(mp_obj_t self_in) {
    return 16;
}

static int rgbmatrix_rgbmatrix_get_bytes_per_cell_proto(mp_obj_t self_in) {
    return 1;
}

static int rgbmatrix_rgbmatrix_get_native_frames_per_second_proto(mp_obj_t self_in) {
    return 250;
}


static const framebuffer_p_t rgbmatrix_rgbmatrix_proto = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_framebuffer)
    .get_bufinfo = rgbmatrix_rgbmatrix_get_bufinfo,
    .set_brightness = rgbmatrix_rgbmatrix_set_brightness_proto,
    .get_brightness = rgbmatrix_rgbmatrix_get_brightness_proto,
    .get_width = rgbmatrix_rgbmatrix_get_width_proto,
    .get_height = rgbmatrix_rgbmatrix_get_height_proto,
    .get_color_depth = rgbmatrix_rgbmatrix_get_color_depth_proto,
    .get_bytes_per_cell = rgbmatrix_rgbmatrix_get_bytes_per_cell_proto,
    .get_native_frames_per_second = rgbmatrix_rgbmatrix_get_native_frames_per_second_proto,
    .swapbuffers = rgbmatrix_rgbmatrix_swapbuffers,
    .deinit = rgbmatrix_rgbmatrix_deinit_proto,
};

static mp_int_t rgbmatrix_rgbmatrix_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    rgbmatrix_rgbmatrix_obj_t *self = (rgbmatrix_rgbmatrix_obj_t *)self_in;
    // a readonly framebuffer would be unusual but not impossible
    if ((flags & MP_BUFFER_WRITE) && !(self->bufinfo.typecode & MP_OBJ_ARRAY_TYPECODE_FLAG_RW)) {
        return 1;
    }
    common_hal_rgbmatrix_rgbmatrix_get_bufinfo(self_in, bufinfo);
    bufinfo->typecode = 'H';
    return 0;
}

MP_DEFINE_CONST_OBJ_TYPE(
    rgbmatrix_RGBMatrix_type,
    MP_QSTR_RGBMatrix,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &rgbmatrix_rgbmatrix_locals_dict,
    make_new, rgbmatrix_rgbmatrix_make_new,
    buffer, rgbmatrix_rgbmatrix_get_buffer,
    protocol, &rgbmatrix_rgbmatrix_proto
    );
