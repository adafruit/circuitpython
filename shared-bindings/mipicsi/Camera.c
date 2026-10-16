// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <math.h>

#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared-bindings/displayio/Bitmap.h"
#include "shared-bindings/mipicsi/__init__.h"
#include "shared-bindings/mipicsi/Camera.h"
#include "shared-bindings/util.h"
#include "shared/runtime/context_manager_helpers.h"

#if CIRCUITPY_VIDEOCORE
#include "shared-module/videocore/__init__.h"
#endif

//| class Camera:
//|     """Receive frames from a camera sensor on a MIPI CSI-2 interface
//|
//|     The camera sensor is configured separately (typically over I2C) to
//|     transmit frames. The MIPI CSI-2 receiver hardware has dedicated pins,
//|     so no pins are configured here.
//|
//|     Pass the configured sensor to the constructor; the receiver matches
//|     itself to the sensor's current mode and starts it streaming. The
//|     sensor object must provide the properties and methods of
//|     ``circuitpython_typing.CameraSensor``: ``width``, ``height``,
//|     ``pixel_format``, ``lane_bit_rate_mbps``, ``data_lanes`` and
//|     ``bayer_order`` (a `BayerOrder`) properties plus ``_start()`` and
//|     ``_stop()`` methods. Those two are internal: the camera calls them
//|     itself as the stream starts and stops, so user code should not.
//|     All are validated here (and again at play, so the sensor may change
//|     its configuration freely while it isn't being played).
//|
//|     The sensor decides the output image size, and the receiver is built
//|     around it: a sensor cannot change its mode while frames are captured.
//|     Sensors are started (``_start()``) when a `videocore.VideoBuffer`
//|     starts playing the camera; they are never stopped by the camera, so
//|     implementations should refuse to reconfigure (raise) while streaming
//|     rather than change the output size or format mid-stream, stop through
//|     their own finaliser, and have a new Camera made when the mode
//|     changes.
//|
//|     Camera objects implement the internal videosource protocol, so a
//|     `videocore.VideoBuffer` can stream their frames into a framebuffer.
//|     """
//|
//|     def __init__(
//|         self,
//|         sensor: circuitpython_typing.CameraSensor,
//|         *,
//|         output_format: Optional[PixelFormat] = None,
//|     ) -> None:
//|         """
//|         Configure and initialize the MIPI CSI-2 camera receiver
//|
//|         .. code-block:: python
//|
//|             sensor = sc2336.SC2336(i2c)
//|             camera = mipicsi.Camera(sensor)
//|
//|         :param sensor: A camera sensor to receive frames from. The receiver
//|             is configured to match the sensor's current mode. The sensor
//|             starts streaming when a `videocore.VideoBuffer` plays the
//|             camera.
//|         :param output_format: The format of the captured frames. When given and different from ``pixel_format``, the ISP converts each frame as it is captured (e.g. the sensor's RAW10 to `PixelFormat.RGB565`). Only RAW formats convert. Default: no conversion.
//|         """
//|
// Read the sensor's parameters off of the sensor object. A missing or
// invalid property fails here rather than later; mp_load_attr raises
// AttributeError for any property the sensor object doesn't provide. When
// pixel_format_override is not None it replaces the sensor's reported
// transmission format.
static mipicsi_pixel_format_t mipicsi_camera_load_sensor_params(
    mp_obj_t sensor,
    mipicsi_sensor_params_t *params) {
    params->width = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_width)), 1, MP_QSTR_width);
    params->height = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_height)), 1, MP_QSTR_height);
    params->lane_bit_rate_mbps = mp_arg_validate_int_min(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_lane_bit_rate_mbps)), 1, MP_QSTR_lane_bit_rate_mbps);
    params->data_lanes = mp_arg_validate_int_range(mp_obj_get_int(mp_load_attr(sensor, MP_QSTR_data_lanes)), 1, 2, MP_QSTR_data_lanes);
    params->bayer_order = validate_bayer_order(mp_load_attr(sensor, MP_QSTR_bayer_order), MP_QSTR_bayer_order);
    return validate_pixel_format(mp_load_attr(sensor, MP_QSTR_pixel_format), MP_QSTR_pixel_format);
}

static mp_obj_t mipicsi_camera_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, true);
    mp_obj_t sensor = all_args[0];

    enum { ARG_output_format, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_output_format, MP_ARG_OBJ | MP_ARG_KW_ONLY, { .u_obj = MP_ROM_NONE } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);
    mp_arg_parse_all_kw_array(0, n_kw, all_args + 1, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // All of the sensor's parameters (including the transmission format) are
    // read off of the sensor object now, so a missing or invalid property
    // fails here rather than at play time. They are read again at play,
    // since the sensor is free to reconfigure between streams.
    mipicsi_sensor_params_t params;
    mipicsi_pixel_format_t pixel_format = mipicsi_camera_load_sensor_params(sensor, &params);
    mipicsi_pixel_format_t output_format = pixel_format;
    if (args[ARG_output_format].u_obj != mp_const_none) {
        output_format = validate_pixel_format(args[ARG_output_format].u_obj, MP_QSTR_output_format);
    }

    // Validate the sensor contract methods (_start/_stop) exist now too,
    // since the camera drives them from C as the stream starts and stops.
    mp_obj_t validated[2];
    mp_load_method(sensor, MP_QSTR__start, validated);
    mp_load_method(sensor, MP_QSTR__stop, validated);

    mipicsi_camera_obj_t *self = mp_obj_malloc_with_finaliser(mipicsi_camera_obj_t, &mipicsi_camera_type);
    common_hal_mipicsi_camera_construct(self, output_format);
    // Keep the sensor alive while receiving from it. It is only started
    // streaming when a videocore.VideoBuffer starts playing (see the
    // videosource play step): until then nothing captures or transmits.
    common_hal_mipicsi_camera_set_sensor(self, sensor);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the camera receiver and releases all memory resources for reuse."""
//|         ...
//|
static mp_obj_t mipicsi_camera_deinit(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_mipicsi_camera_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mipicsi_camera_deinit_obj, mipicsi_camera_deinit);

//|     def __enter__(self) -> Camera:
//|         """No-op used by Context Managers."""
//|         ...
//|
//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//|

static const mp_rom_map_elem_t mipicsi_camera_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mipicsi_camera_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mipicsi_camera_deinit_obj) },
};

static MP_DEFINE_CONST_DICT(mipicsi_camera_locals_dict, mipicsi_camera_locals_dict_table);

#if CIRCUITPY_VIDEOCORE
// Implementation of the videocore videosource protocol so a VideoBuffer can
// tell the Camera where to put frames.
static void mipicsi_camera_play(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    // The camera always has a sensor from make_new on. deinit() clears it
    // (the sensor stops through its own finaliser), so a missing sensor only
    // means the camera was deinitialised.
    if (self->sensor == mp_const_none) {
        raise_deinited_error();
    }
    // Re-read the sensor's mode now: it is free to reconfigure between
    // streams, and only a mode change while streaming is prevented (the
    // sensor refuses it itself).
    mipicsi_sensor_params_t params;
    self->pixel_format = mipicsi_camera_load_sensor_params(self->sensor, &params);
    common_hal_mipicsi_camera_videosource_play(self, &params);
}

static void mipicsi_camera_stop(mp_obj_t self_in) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    common_hal_mipicsi_camera_videosource_stop(self);
}

static bool mipicsi_camera_set_destination(mp_obj_t self_in, const videocore_destination_t *dest,
    videocore_on_frame_fun on_frame, void *ctx) {
    mipicsi_camera_obj_t *self = MP_OBJ_TO_PTR(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
    // The protocol has no stop value on the destination: stopping is stop()'s
    // job.
    return common_hal_mipicsi_camera_set_destination(self, dest->base, dest->stride, on_frame, ctx);
}

static mp_int_t mipicsi_camera_get_width(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_width(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static mp_int_t mipicsi_camera_get_height(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_height(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static mp_int_t mipicsi_camera_get_bits_per_pixel(mp_obj_t self_in) {
    return common_hal_mipicsi_camera_get_bits_per_pixel(mp_obj_cast_to_native_base(self_in, &mipicsi_camera_type));
}

static const videosource_p_t mipicsi_camera_videosource_protocol = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_videosource)
    .play = mipicsi_camera_play,
    .stop = mipicsi_camera_stop,
    .set_destination = mipicsi_camera_set_destination,
    .get_width = mipicsi_camera_get_width,
    .get_height = mipicsi_camera_get_height,
    .get_bits_per_pixel = mipicsi_camera_get_bits_per_pixel,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mipicsi_camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mipicsi_camera_make_new,
    locals_dict, &mipicsi_camera_locals_dict,
    protocol, &mipicsi_camera_videosource_protocol
    );
#else
MP_DEFINE_CONST_OBJ_TYPE(
    mipicsi_camera_type,
    MP_QSTR_Camera,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, mipicsi_camera_make_new,
    locals_dict, &mipicsi_camera_locals_dict
    );
#endif
