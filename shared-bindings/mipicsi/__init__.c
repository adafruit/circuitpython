// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/mipicsi/__init__.h"
#include "shared-bindings/mipicsi/Camera.h"

//| """Support for MIPI CSI-2 camera interfaces
//|
//| Captures image frames from camera sensors attached to a MIPI CSI-2
//| receiver, such as the one in the ESP32-P4. The camera sensor itself is
//| configured separately (typically over I2C); this module only receives
//| the image data the sensor transmits.
//|
//| The image data arrives in the format the sensor transmits, described by
//| `PixelFormat`. RAW formats carry Bayer data, whose element order is
//| described by `BayerOrder`. For instance, a sensor transmitting RGB565
//| data yields frames of RGB565 pixels. Optionally, `mipicsi.Camera` can
//| convert frames with the ISP processor (set ``output_format`` and
//| ``pixel_format``), for example from the sensor's RAW output to RGB565 for
//| display.
//| """

static const mp_rom_map_elem_t mipicsi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mipicsi) },
    { MP_ROM_QSTR(MP_QSTR_PixelFormat), MP_ROM_PTR(&mipicsi_pixel_format_type) },
    { MP_ROM_QSTR(MP_QSTR_BayerOrder), MP_ROM_PTR(&mipicsi_bayer_order_type) },
    { MP_ROM_QSTR(MP_QSTR_Camera), MP_ROM_PTR(&mipicsi_camera_type) },
};

static MP_DEFINE_CONST_DICT(mipicsi_module_globals, mipicsi_module_globals_table);

const mp_obj_module_t mipicsi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mipicsi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mipicsi, mipicsi_module);
