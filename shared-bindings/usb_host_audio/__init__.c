// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_host_audio/__init__.h"
#include "shared-bindings/usb_host_audio/USBIn.h"

//| """Audio from USB devices connected to a USB host port.
//|
//| The `usb_host_audio` module contains the `USBIn` class, which streams the
//| audio of a USB microphone into the audio pipeline or records it.
//|
//| This module only moves the audio. Finding the microphone, picking one of its
//| formats and starting its stream is done with `usb.core`, for example by the
//| ``adafruit_usb_host_microphone`` library.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed. To do so, either call :py:meth:`!deinit` or use a
//| context manager."""

static const mp_rom_map_elem_t usb_host_audio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_host_audio) },
    { MP_ROM_QSTR(MP_QSTR_USBIn), MP_ROM_PTR(&usb_host_audio_usbin_type) },
};

static MP_DEFINE_CONST_DICT(usb_host_audio_module_globals, usb_host_audio_module_globals_table);

const mp_obj_module_t usb_host_audio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_host_audio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_host_audio, usb_host_audio_module);
