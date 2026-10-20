// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/__init__.h"

extern const mp_obj_type_t mipicsi_camera_type;

// Record the output format (possibly the sensor's). No hardware is created
// here: that happens in play().
void common_hal_mipicsi_camera_construct(
    mipicsi_camera_obj_t *self,
    mipicsi_pixel_format_t output_format);

// Remember the sensor configured to transmit to this receiver, if any. The
// receiver reads the sensor's mode from it at play time; the sensor stops
// itself (its finaliser does), never the receiver.
void common_hal_mipicsi_camera_set_sensor(mipicsi_camera_obj_t *self, mp_obj_t sensor);

// Videosource protocol play step: create the receiver hardware for the
// sensor's current mode and start the sensor streaming. When paused is true
// the CSI controller is left stopped, so the camera is active but no frames
// are captured until resume(). Not a user facing method: it is driven by the
// videosource protocol (from VideoBuffer.play), so it may raise.
void common_hal_mipicsi_camera_videosource_play(
    mipicsi_camera_obj_t *self,
    const mipicsi_sensor_params_t *params,
    bool paused);

// Videosource protocol stop step: stop the stream and the sensor. Not a user
// facing method: it is driven by the videosource protocol (from
// VideoBuffer.stop), so it may raise.
void common_hal_mipicsi_camera_videosource_stop(mipicsi_camera_obj_t *self);

// Videosource protocol pause step: stop the CSI controller's DMA without
// tearing down the receiver, ISP or sensor, so the camera stays active but no
// frames are captured. Not a user facing method: it is driven by the
// videosource protocol (when a framebuffer target is hidden).
void common_hal_mipicsi_camera_videosource_pause(mipicsi_camera_obj_t *self);

// Videosource protocol resume step: start the CSI controller again at the most
// recent destination after a pause. Not a user facing method: it is driven by
// the videosource protocol (when a framebuffer target is shown again).
void common_hal_mipicsi_camera_videosource_resume(mipicsi_camera_obj_t *self);

void common_hal_mipicsi_camera_deinit(mipicsi_camera_obj_t *self);
bool common_hal_mipicsi_camera_deinited(mipicsi_camera_obj_t *self);

// Stream frames directly into base row after row. base must lead to a
// cache-line-aligned, contiguous DMA region: when base itself isn't aligned,
// the destination is not accepted. The destination must hold the whole
// (padded) frame back to back: the caller validates the fit, since the DMA
// has no stride. Returns whether those conditions hold (false instead of
// raising; the caller may ignore the result).
bool common_hal_mipicsi_camera_set_destination(
    mipicsi_camera_obj_t *self,
    uint8_t *base,
    size_t stride,
    void (*on_frame)(void *ctx, const void *frame),
    void *ctx);

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_pixel_format(mipicsi_camera_obj_t *self);
mipicsi_pixel_format_t common_hal_mipicsi_camera_get_output_format(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_width(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_height(mipicsi_camera_obj_t *self);
mp_int_t common_hal_mipicsi_camera_get_bits_per_pixel(mipicsi_camera_obj_t *self);
