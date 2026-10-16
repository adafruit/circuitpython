// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/videocore/VideoBuffer.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"

//| class VideoBuffer:
//|     """Stream a video source into a framebuffer or buffer.
//|
//|     A VideoBuffer can be added to displayio.Groups to show on a display
//|     but may have size and position limitations.
//|
//|     A VideoBuffer connects a video source (an object implementing the
//|     videosource protocol, such as `mipicsi.Camera`) to a target: either a
//|     framebuffer (an object implementing the framebuffer protocol, such as
//|     `mipidsi.Display`) or a writable buffer such as a ``bytearray`` or
//|     ``memoryview``. It is the video counterpart of `audioio.AudioOut`.
//|
//|     The VideoBuffer tells the source where to put frames and moves the
//|     destination as needed; the source streams continuously into it. While
//|     playing, the target always holds the newest frame.
//|
//|     When the target is a framebuffer, position the video with ``x`` and
//|     ``y``: they may be changed at any time, including while playing, which
//|     moves where the source puts its next frame. Some implementations may
//|     restrict size and position. Setting position directly will raise an
//|     error but indirectly through Group may result in weird output. For
//|     example, on ESP32-P4 a VideoBuffer written into the framebuffer
//|     directly must be full width because the ESP can't handle a stride
//|     without an intermediate buffer.
//|
//|     When the target is a plain buffer, frames are written compactly (row
//|     stride equals ``width * bits_per_pixel / 8``) and can be read from
//|     Python at any time — for example to process or save camera pixels.
//|
//|     Two complete examples are in the `videocore` module overview: one
//|     streaming a camera directly into a display, and one capturing a
//|     320x240 window into a ``bytearray`` that displayio places in a Group.
//|     """
//|
//|     def __init__(
//|         self,
//|         framebuffer: Union[
//|             circuitpython_typing.FrameBuffer, circuitpython_typing.WriteableBuffer
//|         ],
//|     ) -> None:
//|         """Create a VideoBuffer that plays video into the given framebuffer
//|
//|         :param framebuffer: The framebuffer to copy video frames into, or a
//|             `framebufferio.FramebufferDisplay` wrapping one, or a writable
//|             buffer such as a ``bytearray`` or ``memoryview`` to read the raw
//|             pixels from
//|         """
//|
static mp_obj_t videocore_videobuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_framebuffer, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_framebuffer, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    videocore_videobuffer_obj_t *self = mp_obj_malloc_with_finaliser(videocore_videobuffer_obj_t, &videocore_videobuffer_type);
    common_hal_videocore_videobuffer_construct(self, args[ARG_framebuffer].u_obj);
    return MP_OBJ_FROM_PTR(self);
}

//|     def play(self, source: circuitpython_typing.VideoSource) -> None:
//|         """Begin streaming the given video source into the target.
//|
//|         The source streams continuously, writing each new frame where the
//|         VideoBuffer tells it to, replacing the previous one. The source's
//|         frame format must be byte aligned. When the target is a
//|         framebuffer, its bits per pixel must match the framebuffer color
//|         depth and the whole frame must fit within the framebuffer at the
//|         current ``x``/``y`` position (rows can't wrap past its end). When
//|         the target is a plain buffer, it must be large enough for a whole
//|         frame. Buffer size is validated here; the
//|         source's size and format are then fixed for the duration of the
//|         play. Sources that change them require ``stop`` and a new
//|         ``play``.
//|
//|         :param ~circuitpython_typing.VideoSource source: The video source to
//|             play, such as `mipicsi.Camera`
//|         """
//|
static mp_obj_t videocore_videobuffer_play(mp_obj_t self_in, mp_obj_t source) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_videocore_videobuffer_play(self, source);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(videocore_videobuffer_play_obj, videocore_videobuffer_play);

//|     def stop(self) -> None:
//|         """Stop playing the video source."""
//|         ...
//|
static mp_obj_t videocore_videobuffer_stop(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_videocore_videobuffer_stop(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_stop_obj, videocore_videobuffer_stop);

//|     playing: bool
//|     """True when a video source is being played, False otherwise. (read-only)"""
//|
static mp_obj_t videocore_videobuffer_get_playing(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_videocore_videobuffer_get_playing(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_get_playing_obj, videocore_videobuffer_get_playing);
MP_PROPERTY_GETTER(videocore_videobuffer_playing_obj,
    (mp_obj_t)&videocore_videobuffer_get_playing_obj);

//|     source: Optional[circuitpython_typing.VideoSource]
//|     """The video source being played, or None when not playing. (read-only)"""
//|
static mp_obj_t videocore_videobuffer_get_source(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_videocore_videobuffer_get_source(self);
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_get_source_obj, videocore_videobuffer_get_source);
MP_PROPERTY_GETTER(videocore_videobuffer_source_obj,
    (mp_obj_t)&videocore_videobuffer_get_source_obj);

//|     x: int
//|     """The x position of the video within the display. Framebuffer targets
//|     must be at x == 0 (video sources DMA whole rows): ``ValueError``
//|     otherwise. Plain buffer targets only use it to place the video in a
//|     Group: it doesn't change where frames land within the buffer."""
//|
//|     y: int
//|     """The y position of the video within the display."""
//|
//|
static void videocore_videobuffer_get_position(mp_obj_t self_in, mp_int_t *x, mp_int_t *y) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    *x = self->x;
    *y = self->y;
}

static mp_obj_t videocore_videobuffer_get_x(mp_obj_t self_in) {
    mp_int_t x, y;
    videocore_videobuffer_get_position(self_in, &x, &y);
    return MP_OBJ_NEW_SMALL_INT(x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_get_x_obj, videocore_videobuffer_get_x);

static mp_obj_t videocore_videobuffer_set_x(mp_obj_t self_in, mp_obj_t value) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t x = mp_obj_get_int(value);
    common_hal_videocore_videobuffer_set_position(self, x, self->y);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(videocore_videobuffer_set_x_obj, videocore_videobuffer_set_x);
MP_PROPERTY_GETSET(videocore_videobuffer_x_obj,
    (mp_obj_t)&videocore_videobuffer_get_x_obj,
    (mp_obj_t)&videocore_videobuffer_set_x_obj);

static mp_obj_t videocore_videobuffer_get_y(mp_obj_t self_in) {
    mp_int_t x, y;
    videocore_videobuffer_get_position(self_in, &x, &y);
    return MP_OBJ_NEW_SMALL_INT(y);
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_get_y_obj, videocore_videobuffer_get_y);

static mp_obj_t videocore_videobuffer_set_y(mp_obj_t self_in, mp_obj_t value) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t y = mp_obj_get_int(value);
    common_hal_videocore_videobuffer_set_position(self, self->x, y);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(videocore_videobuffer_set_y_obj, videocore_videobuffer_set_y);
MP_PROPERTY_GETSET(videocore_videobuffer_y_obj,
    (mp_obj_t)&videocore_videobuffer_get_y_obj,
    (mp_obj_t)&videocore_videobuffer_set_y_obj);

//|     def deinit(self) -> None:
//|         """Stop playing. The video source stops itself."""
//|         ...
//|
//|     def __enter__(self) -> VideoBuffer:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
static mp_obj_t videocore_videobuffer_deinit(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_videocore_videobuffer_stop(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer_deinit_obj, videocore_videobuffer_deinit);

// The finaliser must not raise (it runs during garbage collection), so stop
// without one instead of deinit(): a failed stop stays best-effort, matching
// the source's own finaliser.
static mp_obj_t videocore_videobuffer___del__(mp_obj_t self_in) {
    videocore_videobuffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
    videocore_videobuffer_stop_quiet(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(videocore_videobuffer___del___obj, videocore_videobuffer___del__);

static const mp_rom_map_elem_t videocore_videobuffer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&videocore_videobuffer___del___obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&videocore_videobuffer_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&videocore_videobuffer_play_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&videocore_videobuffer_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_playing), MP_ROM_PTR(&videocore_videobuffer_playing_obj) },
    { MP_ROM_QSTR(MP_QSTR_source), MP_ROM_PTR(&videocore_videobuffer_source_obj) },
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&videocore_videobuffer_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&videocore_videobuffer_y_obj) },
};

static MP_DEFINE_CONST_DICT(videocore_videobuffer_locals_dict, videocore_videobuffer_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    videocore_videobuffer_type,
    MP_QSTR_VideoBuffer,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, videocore_videobuffer_make_new,
    locals_dict, &videocore_videobuffer_locals_dict,
    protocol, &videocore_videobuffer_draw_protocol
    );
