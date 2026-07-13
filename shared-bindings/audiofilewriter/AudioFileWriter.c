// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/audiofilewriter/AudioFileWriter.h"
#include "shared-bindings/util.h"

// ~1 s of 16 kHz mono 16-bit PCM. Sized to absorb a worst-case SD-write stall.
#define AUDIOFILEWRITER_DEFAULT_BUFFER_SIZE (32 * 1024)

//| class AudioFileWriter:
//|     """Streams an audio source to a ``.wav`` file in the background.
//|
//|     ``AudioFileWriter`` is the inverse of `audiocore.WaveFile`: rather than being
//|     an audio *source* played by an `audioio.AudioOut`, it is a *sink* that
//|     drives an audio source (a microphone, ``synthio``, or an ``audiofilters``/
//|     ``audiodelays``/``audiofreeverb``/``audiospeed`` effect chain) and writes
//|     the resulting PCM to a file as a WAV.
//|
//|     Recording runs on a background pump paced to the source's real-time rate,
//|     so it does not block and does not require a Python read loop."""
//|
//|     def __init__(self, file: typing.BinaryIO, *, buffer_size: int = 32768) -> None:
//|         """Create an ``AudioFileWriter`` that writes to ``file``.
//|
//|         :param typing.BinaryIO file: An already-open writable binary stream
//|           (a file opened in ``"wb"`` mode, or an `io.BytesIO`). The stream must
//|           support seeking so the WAV header sizes can be patched when recording
//|           stops. ``AudioFileWriter`` does not close it; the caller owns it.
//|         :param int buffer_size: Size in bytes of the internal RAM ring that
//|           decouples file-write latency from the source. Larger values tolerate
//|           longer write stalls (e.g. a slow SD card) at the cost of RAM.
//|           Minimum valid value, and default, is ``512``. Must be at least the
//|           size of the source buffer.
//|
//|         The audio format (sample rate, channel count, bit depth) is taken from
//|         the source at `play()` time, so there are no format arguments here.
//|
//|         Recording synthio to SD::
//|
//|           import time
//|           import synthio
//|           from audiofilewriter import AudioFileWriter
//|           import storage
//|
//|           SAMPLE_RATE = 16000
//|           OUTPUT_PATH = "/sd/demo_file.wav"
//|
//|           C_major_scale = [60, 62, 64, 65, 67, 69, 71, 72, 71, 69, 67, 65, 64, 62, 60]
//|           synth = synthio.Synthesizer(sample_rate=SAMPLE_RATE)
//|
//|           with open(OUTPUT_PATH, "wb") as f:
//|               writer = AudioFileWriter(f)
//|               writer.play(synth)
//|               for note in C_major_scale:
//|                   synth.press(note)
//|                   time.sleep(0.1)
//|                   synth.release(note)
//|                   time.sleep(0.10)
//|               writer.stop()
//|           print("Done ->", OUTPUT_PATH)
//|         """
//|         ...
//|
static mp_obj_t audiofilewriter_audiofilewriter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_file, ARG_buffer_size };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_size, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = AUDIOFILEWRITER_DEFAULT_BUFFER_SIZE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // A buffer smaller than one source buffer is useless; require a sane floor.
    mp_int_t buffer_size = mp_arg_validate_int_min(args[ARG_buffer_size].u_int, 512, MP_QSTR_buffer_size);

    audiofilewriter_audiofilewriter_obj_t *self = mp_obj_malloc(audiofilewriter_audiofilewriter_obj_t, &audiofilewriter_audiofilewriter_type);
    common_hal_audiofilewriter_audiofilewriter_construct(self, args[ARG_file].u_obj, (uint32_t)buffer_size);

    return MP_OBJ_FROM_PTR(self);
}

static void check_for_deinit(audiofilewriter_audiofilewriter_obj_t *self) {
    if (common_hal_audiofilewriter_audiofilewriter_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def deinit(self) -> None:
//|         """Stops recording (patching the WAV header) and releases resources."""
//|         ...
//|
static mp_obj_t audiofilewriter_audiofilewriter_deinit(mp_obj_t self_in) {
    audiofilewriter_audiofilewriter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_audiofilewriter_audiofilewriter_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiofilewriter_audiofilewriter_deinit_obj, audiofilewriter_audiofilewriter_deinit);

//|     def __enter__(self) -> AudioFileWriter:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     def play(self, sample: circuitpython_typing.AudioSample) -> None:
//|         """Begin recording ``sample`` to the file. Does not block.
//|
//|         Writes a WAV header (in the format of ``sample``) and starts the
//|         background pump. ``sample`` must be an 8-bit or 16-bit mono or stereo
//|         audio source. Use `playing` to tell when a finite source has finished,
//|         or call `stop()` to end recording of a continuous source (e.g. a mic)."""
//|         ...
//|
static mp_obj_t audiofilewriter_audiofilewriter_obj_play(mp_obj_t self_in, mp_obj_t sample_in) {
    audiofilewriter_audiofilewriter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiofilewriter_audiofilewriter_play(self, sample_in);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(audiofilewriter_audiofilewriter_play_obj, audiofilewriter_audiofilewriter_obj_play);

//|     def stop(self) -> None:
//|         """Stop recording, flush the RAM ring to the file, and patch the WAV
//|         header sizes. The file is left open for the caller to close."""
//|         ...
//|
static mp_obj_t audiofilewriter_audiofilewriter_obj_stop(mp_obj_t self_in) {
    audiofilewriter_audiofilewriter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_audiofilewriter_audiofilewriter_stop(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiofilewriter_audiofilewriter_stop_obj, audiofilewriter_audiofilewriter_obj_stop);

//|     playing: bool
//|     """True while recording is in progress. Becomes False on its own when a
//|     finite source finishes, or after `stop()`. (read-only)"""
//|
static mp_obj_t audiofilewriter_audiofilewriter_obj_get_playing(mp_obj_t self_in) {
    audiofilewriter_audiofilewriter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(common_hal_audiofilewriter_audiofilewriter_get_playing(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(audiofilewriter_audiofilewriter_get_playing_obj, audiofilewriter_audiofilewriter_obj_get_playing);

MP_PROPERTY_GETTER(audiofilewriter_audiofilewriter_playing_obj,
    (mp_obj_t)&audiofilewriter_audiofilewriter_get_playing_obj);

static const mp_rom_map_elem_t audiofilewriter_audiofilewriter_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audiofilewriter_audiofilewriter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&audiofilewriter_audiofilewriter_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&audiofilewriter_audiofilewriter_stop_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&audiofilewriter_audiofilewriter_playing_obj) },
};
static MP_DEFINE_CONST_DICT(audiofilewriter_audiofilewriter_locals_dict, audiofilewriter_audiofilewriter_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    audiofilewriter_audiofilewriter_type,
    MP_QSTR_AudioFileWriter,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, audiofilewriter_audiofilewriter_make_new,
    locals_dict, &audiofilewriter_audiofilewriter_locals_dict
    );
