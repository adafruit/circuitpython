// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_audio/__init__.h"
#include "shared-bindings/usb_audio/USBMicrophone.h"
#include "shared-bindings/usb_audio/USBSpeaker.h"
#include "shared-module/usb_audio/__init__.h"
#include "shared-module/usb_audio/usb_audio_descriptors.h"

//| """Stream audio to a host computer via USB
//|
//| This makes your CircuitPython device identify to the host computer as a USB
//| Audio Class (UAC2) microphone: the board is the audio *source* and streams
//| samples to the host over a USB isochronous IN endpoint.
//|
//| This mode requires 1 (mono) or 2 (stereo / bidirectional) IN endpoint and 2 interfaces.
//| Generally, microcontrollers have a limit on the number of endpoints. If you exceed the number
//| of endpoints, CircuitPython will automatically enter Safe Mode. Even in this case, you may be
//| able to enable USB audio by also disabling other USB functions, such as
//| `usb_hid` or `usb_midi`.
//|
//| To enable this mode, you must configure the audio format in ``boot.py`` and then
//| use the `usb_microphone` singleton instance in ``code.py``.
//|
//| .. code-block:: py
//|
//|     # boot.py
//|     import usb_audio
//|     usb_audio.enable(sample_rate=16000, channel_count=1, bits_per_sample=16)
//|
//| .. code-block:: py
//|
//|     # code.py
//|     import time
//|     import usb_audio
//|     import synthio
//|
//|     # usb_audio.usb_microphone is a singleton instance (created by enable() above),
//|     # not a class you construct. It is a consumer of an audio sample, just like
//|     # audioio.AudioOut: the samples it pulls are streamed to the host PC instead
//|     # of to a pin.
//|     mic = usb_audio.usb_microphone
//|     synth = synthio.Synthesizer(sample_rate=16000, channel_count=1)
//|     mic.play(synth, loop=True)
//|
//|     c_major_scale = [60, 62, 64, 65, 67, 69, 71, 72]
//|     try:
//|         while True:
//|             for note in c_major_scale:
//|                 synth.press(note)
//|                 time.sleep(0.1)
//|                 synth.release(note)
//|                 time.sleep(0.05)
//|     except KeyboardInterrupt:
//|         pass
//|     mic.stop()
//|
//| The ``sample_rate`` and ``channel_count`` of the sample played must match the values passed to `enable`,
//| and the sample must be 16-bit signed; otherwise ``play`` raises a ``ValueError``.
//|
//| If both speaker and headphone features are enabled (bidirectional), only mono operation is
//| permitted.
//|
//| This interface is experimental and may change without notice even in stable
//| versions of CircuitPython."""
//|
//|

//| usb_microphone: Optional[USBMicrophone]
//| """The shared `USBMicrophone` singleton instance, or ``None`` until
//| ``usb_audio.enable()`` has configured an input (microphone) stream in ``boot.py``.
//| `USBMicrophone` is exposed only as a type; you do not construct it."""
//|
//| usb_speaker: Optional[USBSpeaker]
//| """The shared `USBSpeaker` singleton instance, or ``None`` until
//| ``usb_audio.enable()`` has configured an output (speaker) stream in ``boot.py``.
//| `USBSpeaker` is exposed only as a type; you do not construct it."""
//|

//| def enable(
//|     sample_rate: int = 16000,
//|     channel_count: int = 1,
//|     bits_per_sample: int = 16,
//|     microphone: bool = True,
//|     speaker: bool = False,
//| ) -> None:
//|     """Enable the USB audio interface with the given PCM format.
//|
//|     This function may only be used from ``boot.py``.
//|
//|     :param int sample_rate: Samples per second of the streamed audio.
//|     :param int channel_count: Number of channels. Either mono (1) or stereo (2) is supported.
//|     :param int bits_per_sample: Bits per signed PCM sample. Only 16 is supported initially.
//|     :param bool microphone: Present a microphone (audio flows board -> host). Enabled by default.
//|     :param bool speaker: Present a speaker (audio flows host -> board).
//|
//|     Enabling both ``microphone`` and ``speaker`` presents a combined headset. At
//|     least one of the two must be enabled."""
//|
//|
static mp_obj_t usb_audio_enable(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_sample_rate, ARG_channel_count, ARG_bits_per_sample, ARG_microphone, ARG_speaker };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sample_rate, MP_ARG_INT, { .u_int = 16000 } },
        { MP_QSTR_channel_count, MP_ARG_INT, { .u_int = 1 } },
        { MP_QSTR_bits_per_sample, MP_ARG_INT, { .u_int = 16 } },
        { MP_QSTR_microphone, MP_ARG_BOOL, { .u_bool = true } },
        { MP_QSTR_speaker, MP_ARG_BOOL, { .u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t sample_rate = mp_arg_validate_int_range(args[ARG_sample_rate].u_int, 1, USB_AUDIO_MAX_SAMPLE_RATE, MP_QSTR_sample_rate);
    mp_int_t channel_count = mp_arg_validate_int_range(args[ARG_channel_count].u_int, 1, USB_AUDIO_MAX_CHANNELS, MP_QSTR_channel_count);
    mp_int_t bits_per_sample = mp_arg_validate_int(args[ARG_bits_per_sample].u_int, USB_AUDIO_BITS_PER_SAMPLE, MP_QSTR_bits_per_sample);
    bool microphone = args[ARG_microphone].u_bool;
    bool speaker = args[ARG_speaker].u_bool;

    if (!microphone && !speaker) {
        mp_raise_ValueError(MP_ERROR_TEXT("At least one of microphone and speaker must be enabled"));
    }

    if (microphone && speaker && channel_count == 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only mono operation is supported when microphone and speaker are both enabled"));
    }

    if (!shared_module_usb_audio_enable(sample_rate, channel_count, bits_per_sample, microphone, speaker)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Cannot change USB devices now"));
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(usb_audio_enable_obj, 0, usb_audio_enable);

mp_map_elem_t usb_audio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_audio) },
    // USBMicrophone and USBSpeaker are the classes, exposed only for typing and
    // isinstance() checks; you do not construct them. The usable objects are the
    // pre-made singleton instances installed at the usb_microphone / usb_speaker
    // attributes below.
    { MP_ROM_QSTR(MP_QSTR_USBMicrophone), MP_OBJ_FROM_PTR(&usb_audio_USBMicrophone_type) },
    { MP_ROM_QSTR(MP_QSTR_USBSpeaker), MP_OBJ_FROM_PTR(&usb_audio_USBSpeaker_type) },
    // usb_microphone and usb_speaker are the singleton instances, like
    // usb_midi.ports. usb_audio_setup_singletons() replaces these None
    // placeholders with the real instances at the start of each VM when the
    // matching direction was enabled in boot.py.
    { MP_ROM_QSTR(MP_QSTR_usb_microphone), MP_ROM_NONE },
    { MP_ROM_QSTR(MP_QSTR_usb_speaker), MP_ROM_NONE },
    { MP_ROM_QSTR(MP_QSTR_enable), MP_OBJ_FROM_PTR(&usb_audio_enable_obj) },
};

// This isn't const so the singleton instances can be installed dynamically.
MP_DEFINE_MUTABLE_DICT(usb_audio_module_globals, usb_audio_module_globals_table);

const mp_obj_module_t usb_audio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_audio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb_audio, usb_audio_module);
