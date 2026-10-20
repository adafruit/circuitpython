// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "py/obj.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#include "esp_cam_ctlr.h"
#include "esp_ldo_regulator.h"

#if SOC_ISP_SUPPORTED
#include "driver/isp.h"
#endif

#include "shared-bindings/mipicsi/__init__.h"

typedef struct mipicsi_camera_obj mipicsi_camera_obj_t;

// Sensor parameters used to build the receiver. Read off the sensor object
// when a stream starts (the sensor is free to reconfigure between streams).
typedef struct {
    mp_int_t width;
    mp_int_t height;
    mp_int_t bayer_order;
    mp_int_t data_lanes;
    mp_int_t lane_bit_rate_mbps;
} mipicsi_sensor_params_t;

struct mipicsi_camera_obj {
    mp_obj_base_t base;
    esp_cam_ctlr_handle_t handle;

    // Power for the MIPI D-PHY, shared with the DSI transmitter. Acquired
    // when a stream starts and released when it (or the camera) ends.
    esp_ldo_channel_handle_t phy_ldo;

    // Camera sensor configured to transmit to this receiver, or None.
    mp_obj_t sensor;

    // ISP processor used to convert sensor frames to output_format, only
    // present when the CSI receiver itself cannot convert (e.g. gen-1 P4).
    mipicsi_pixel_format_t output_format;
    #if SOC_ISP_SUPPORTED
    isp_proc_handle_t isp_proc;
    #endif

    // The receiver hardware (LDO, ISP, CSI controller) is created when a
    // stream starts (see play) and torn down when it ends. The sensor's
    // parameters (width, height, data_lanes, lane_bit_rate_mbps, bayer_order)
    // come from the sensor at play time as well; pixel_format only when it
    // wasn't overridden at construction.
    // Streaming destination, set by videocore.VideoBuffer. While base is
    // NULL the CSI controller is stopped: no capture buffers are used at
    // all. The destination is assumed to be a whole frame: DMA'd straight
    // into base, stride equal to the row size. on_frame(ctx, frame) is called
    // from ISR context each time a frame lands, with the base it landed at
    // (a frame in flight when base changes lands at the previous base).
    // The DMA destination: set_destination requires it to be cache-line
    // aligned (the driver's per-frame cache sync needs that) and reports a
    // rejected destination with a false return, rather than shifting frames
    // elsewhere. The frame must be contiguous.
    volatile void *dest_base;
    void (*dest_on_frame)(void *ctx, const void *frame);
    void *dest_ctx;
    bool ctlr_started;
    // True between pause and resume: the receiver hardware, ISP and sensor stay
    // alive, but the CSI controller is stopped so no frame is DMAd. A
    // destination can still be changed while paused; resume starts the
    // controller again at the latest one.
    bool paused;

    // Diagnostics from the CSI host error interrupt.
    volatile uint32_t error_count;
    volatile uint32_t error_events;

    portMUX_TYPE spinlock;

    mp_int_t width;
    mp_int_t height;
    mipicsi_pixel_format_t pixel_format;
    mp_int_t data_lanes;
    mp_int_t lane_bit_rate_mbps;
    // Bayer element order of the sensor's RAW output, only used when the ISP
    // converts from a RAW format.
    mp_int_t bayer_order;
    size_t buffer_len; // Length handed to the driver, includes cache-alignment padding.
};
