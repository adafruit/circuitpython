// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/runtime.h"

#include "common-hal/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/Camera.h"
#include "shared-bindings/mipicsi/__init__.h"
#include "bindings/espidf/__init__.h"

#include "common-hal/mipidsi/Bus.h"

#include <string.h>
#include "esp_attr.h"
#include "esp_cache.h"
#include "esp_cam_ctlr.h"
#include "esp_private/esp_cache_private.h"
#include "esp_cam_ctlr_csi.h"
#include "hal/color_hal.h"

// MIPI CSI receives data from a fixed, dedicated set of pins, so no pin
// configuration or claiming is done here.

#define ISP_CLK_HZ (80 * 1000 * 1000)

static cam_ctlr_color_t mipicsi_color_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW8:
            return CAM_CTLR_COLOR_RAW8;
        case MIPICSI_FORMAT_RAW10:
            return CAM_CTLR_COLOR_RAW10;
        case MIPICSI_FORMAT_RAW12:
            return CAM_CTLR_COLOR_RAW12;
        case MIPICSI_FORMAT_RGB888:
            return CAM_CTLR_COLOR_RGB888;
        case MIPICSI_FORMAT_YUV422:
            return CAM_CTLR_COLOR_YUV422_YUYV;
        case MIPICSI_FORMAT_GRAYSCALE:
            return CAM_CTLR_COLOR_GRAY8;
        case MIPICSI_FORMAT_RGB565:
        default:
            return CAM_CTLR_COLOR_RGB565;
    }
}

static mp_int_t mipicsi_bits_per_pixel_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW10:
            return 10;
        case MIPICSI_FORMAT_RAW12:
            return 12;
        case MIPICSI_FORMAT_RGB888:
            return 24;
        case MIPICSI_FORMAT_RGB565:
        case MIPICSI_FORMAT_YUV422:
            return 16;
        case MIPICSI_FORMAT_RAW8:
        case MIPICSI_FORMAT_GRAYSCALE:
        default:
            return 8;
    }
}

#if SOC_ISP_SUPPORTED
// The ISP color types share their FOURCC values with the CAM_CTLR color
// types, except for GRAYSCALE (no ISP grey mode) and YUV422 (the ISP input is
// UYVY, the CAM_CTLR YUYV). Only sensor RAW formats convert here.
static isp_color_t mipicsi_isp_color_for(mipicsi_pixel_format_t format) {
    switch (format) {
        case MIPICSI_FORMAT_RAW8:
            return ISP_COLOR_RAW8;
        case MIPICSI_FORMAT_RAW10:
            return ISP_COLOR_RAW10;
        case MIPICSI_FORMAT_RAW12:
            return ISP_COLOR_RAW12;
        case MIPICSI_FORMAT_RGB565:
            return ISP_COLOR_RGB565;
        case MIPICSI_FORMAT_RGB888:
            return ISP_COLOR_RGB888;
        case MIPICSI_FORMAT_YUV422:
        case MIPICSI_FORMAT_GRAYSCALE:
        default:
            return 0;
    }
}
#endif

// Called by the driver (in ISR context) when the CSI host detects an error
// (PHY, packet, frame or CRC problems). Only installed for diagnostics: the
// counts are printed to the ROM console so they show up on the serial console.
static IRAM_ATTR bool mipicsi_on_error(
    esp_cam_ctlr_handle_t handle, const esp_cam_ctlr_error_event_data_t *edata, void *user_data) {
    mipicsi_camera_obj_t *self = user_data;
    self->error_count++;
    self->error_events |= edata->csi_host_err_evts;
    if (self->error_count <= 3) {
        esp_rom_printf("mipicsi: csi error #%lu events 0x%lx\n",
            (unsigned long)self->error_count, (unsigned long)edata->csi_host_err_evts);
    }
    return false;
}

// Called by the driver (in ISR context) to get the next buffer to DMA into.
static IRAM_ATTR bool mipicsi_on_get_new_trans(
    esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data) {
    mipicsi_camera_obj_t *self = user_data;

    // Direct streaming: DMA the whole frame into the current destination.
    // The videobuffer can change the destination between frames; the change
    // is picked up here on the next frame.
    //
    // The controller only runs while a destination is set, so there is
    // always somewhere to DMA into here; set_destination(NULL) stops the
    // controller before clearing the destination. With the driver's backup
    // buffer disabled, no buffer to hand over is unrecoverable (the driver
    // asserts, or DMAs to address 0 in release builds), so there is no
    // "give NULL" path.
    trans->buffer = (void *)self->dest_base;
    trans->buflen = self->buffer_len;
    return false;
}

// Called by the driver (in ISR context) when a frame has been written.
static IRAM_ATTR bool mipicsi_on_trans_finished(
    esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data) {
    mipicsi_camera_obj_t *self = user_data;

    // Report where the frame actually landed: after a destination change
    // (or a pause) the frame that was in flight lands at the previous one.
    if (self->dest_base != NULL && self->dest_on_frame != NULL) {
        self->dest_on_frame(self->dest_ctx, trans->buffer);
    }
    return false;
}

void common_hal_mipicsi_camera_construct(
    mipicsi_camera_obj_t *self,
    mipicsi_pixel_format_t output_format) {

    self->width = 1;
    self->height = 1;
    self->pixel_format = output_format;
    self->output_format = output_format;
    self->dest_base = NULL;
    self->dest_on_frame = NULL;
    self->dest_ctx = NULL;
    self->ctlr_started = false;
    self->paused = false;
    self->error_count = 0;
    self->error_events = 0;
    self->sensor = mp_const_none;
    self->phy_ldo = NULL;
    self->isp_proc = NULL;
    self->spinlock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    // No receiver hardware yet: it is created in the videosource play step,
    // where the sensor's current mode is known.
}

// Videosource protocol play step: create the receiver hardware for the
// given (freshly read) sensor parameters and start the sensor streaming.
// When paused is true the CSI controller is left stopped, so the camera is
// active but no frames are DMAd until resume(). Raises when the camera is
// already streaming for someone else. Not a user facing method: it is driven
// by the videosource protocol (from VideoBuffer.play), so it may raise.
void common_hal_mipicsi_camera_videosource_play(
    mipicsi_camera_obj_t *self,
    const mipicsi_sensor_params_t *params,
    bool paused) {

    if (self->handle != NULL) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }
    self->paused = paused;

    // Adopt what we need from the sensor's current mode; it may have
    // changed since the Camera was made (sensors are free to reconfigure
    // while they aren't being played).
    self->width = params->width;
    self->height = params->height;
    self->bayer_order = params->bayer_order;
    self->data_lanes = params->data_lanes;
    self->lane_bit_rate_mbps = params->lane_bit_rate_mbps;

    const cam_ctlr_color_t color = mipicsi_color_for(self->pixel_format);

    #if SOC_ISP_SUPPORTED
    if (self->output_format != self->pixel_format) {
        // The sensor transmits in a different format than the one requested.
        // On gen-1 P4 silicon the CSI receiver cannot convert, so route it
        // through the ISP processor instead, mirroring the IDF
        // esp_driver_cam csi test.
        const isp_color_t isp_in = mipicsi_isp_color_for(self->pixel_format);
        const isp_color_t isp_out = mipicsi_isp_color_for(self->output_format);
        if (isp_in == 0 || (isp_in != ISP_COLOR_RAW8 && isp_in != ISP_COLOR_RAW10 && isp_in != ISP_COLOR_RAW12)) {
            mp_raise_ValueError(MP_ERROR_TEXT("pixel_format must be RAW8, RAW10 or RAW12 when output_format differs"));
        }
        if (isp_out != ISP_COLOR_RGB565 && isp_out != ISP_COLOR_RGB888) {
            mp_raise_ValueError(MP_ERROR_TEXT("output_format must be RGB565 or RGB888"));
        }
        esp_isp_processor_cfg_t isp_config = {
            .clk_hz = ISP_CLK_HZ,
            .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
            .input_data_color_type = isp_in,
            .output_data_color_type = isp_out,
            .has_line_start_packet = false,
            .has_line_end_packet = false,
            .h_res = self->width,
            .v_res = self->height,
            .bayer_order = (color_raw_element_order_t)self->bayer_order,
        };
        CHECK_ESP_RESULT(esp_isp_new_processor(&isp_config, &self->isp_proc));
    }
    #else
    if (self->output_format != self->pixel_format) {
        mp_raise_NotImplementedError(MP_ERROR_TEXT("ISP conversion not supported on this chip"));
    }
    #endif

    // Power the MIPI D-PHY. The CSI and DSI PHYs share LDO channel 3; the
    // shared reference count (see mipidsi_phy_ldo_acquire) keeps the rail up
    // while either the camera or a mipidsi.Bus is alive. The sensor's I2C
    // control interface is on a different rail and works without this.
    self->phy_ldo = mipidsi_phy_ldo_acquire();

    // The received images are not converted by the CSI bridge (gen-1 P4
    // silicon can't convert RAW to RGB565), so the input and output color
    // types are the same and bridge conversion is bypassed. The ISP converts
    // to the output format instead, so tell the CSI driver the memory-side
    // frame is the ISP output depth. Without this it sizes the transfer and
    // buffer from the 10 bpp RAW input (384,000 bytes for 1024x300) instead of
    // the 16 bpp RGB565 output (614,400 bytes), which lets the DMA run past
    // the destination. bits_per_pixel also makes the driver bypass bridge
    // conversion; keep its default CSI packet data-type range.
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = self->width,
        .v_res = self->height,
        .data_lane_num = self->data_lanes,
        .lane_bit_rate_mbps = self->lane_bit_rate_mbps,
        .input_data_color_type = color,
        .output_data_color_type = color,
        .data_type = {
            .bits_per_pixel = mipicsi_bits_per_pixel_for(self->output_format),
            .data_type_min = 0x12,
            .data_type_max = 0x2f,
        },
        // One transaction queue item; frames go straight to the current
        // destination. The controller is only started while a destination
        // is set, so the driver's backup buffer is never needed and its
        // frame-sized allocation is skipped.
        .queue_items = 1,
        .bk_buffer_dis = true,
    };

    esp_err_t result = esp_cam_new_csi_ctlr(&csi_config, &self->handle);
    if (result != ESP_OK) {
        mipidsi_phy_ldo_release();
        self->phy_ldo = NULL;
        raise_esp_error(result);
    }

    // Exact frame size. Destinations are written directly (no cache-line
    // padding is ever DMA'd past the last row), so they only need to be wide
    // enough to hold the whole frame; the DSI framebuffer always is. When
    // the ISP is in the pipeline, the CSI receiver hands its converted
    // output to us.
    self->buffer_len = ((size_t)self->width * self->height * mipicsi_bits_per_pixel_for(self->output_format) + 7) / 8;

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = mipicsi_on_get_new_trans,
        .on_trans_finished = mipicsi_on_trans_finished,
        .on_error = mipicsi_on_error,
    };
    result = esp_cam_ctlr_register_event_callbacks(self->handle, &cbs, self);
    if (result != ESP_OK) {
        common_hal_mipicsi_camera_deinit(self);
        self->sensor = mp_const_none;
        raise_esp_error(result);
    }

    result = esp_cam_ctlr_enable(self->handle);
    if (result != ESP_OK) {
        common_hal_mipicsi_camera_deinit(self);
        self->sensor = mp_const_none;
        raise_esp_error(result);
    }
    #if SOC_ISP_SUPPORTED
    if (self->isp_proc) {
        result = esp_isp_enable(self->isp_proc);
        if (result != ESP_OK) {
            common_hal_mipicsi_camera_deinit(self);
            self->sensor = mp_const_none;
            raise_esp_error(result);
        }
    }
    #endif

    // Start the sensor streaming so frames exist to capture once a
    // destination is set. Sensors' _start() is expected to be idempotent.
    // The camera never calls the sensor's stop(): when a stream ends (or on
    // reload) only the CSI receiver and ISP are torn down; the sensor stops
    // through its own finaliser (deinit/__del__).
    if (self->sensor != mp_const_none) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            mp_obj_t start_method[2];
            mp_load_method(self->sensor, MP_QSTR__start, start_method);
            mp_call_method_n_kw(0, 0, start_method);
            nlr_pop();
        } else {
            common_hal_mipicsi_camera_deinit(self);
            self->sensor = mp_const_none;
            nlr_raise(nlr.ret_val);
        }
    }

    // Don't start the controller yet: it only runs while a destination is
    // set (see common_hal_mipicsi_camera_set_destination), so no frame is
    // ever captured without a place to put it.
}

// Tear down the receiver hardware quietly: this runs from Python (a
// VideoBuffer stopped) and from background contexts (destination dropped on
// reload), so it must not raise. The sensor is left alone: its own finaliser
// stops it. Only the CSI receiver and ISP turn off.
static void mipicsi_camera_teardown(mipicsi_camera_obj_t *self) {
    // Always stop, even when paused: a pause's stop finishes in the
    // background, and it must be complete before the controller is disabled.
    // esp_cam_ctlr_stop waits for it (and is a no-op once stopped).
    if (self->handle != NULL) {
        esp_cam_ctlr_stop(self->handle);
    }
    self->ctlr_started = false;
    portENTER_CRITICAL(&self->spinlock);
    self->dest_base = NULL;
    self->dest_on_frame = NULL;
    self->dest_ctx = NULL;
    self->paused = false;
    portEXIT_CRITICAL(&self->spinlock);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (self->handle != NULL) {
            esp_cam_ctlr_disable(self->handle);
            esp_cam_ctlr_del(self->handle);
            self->handle = NULL;
        }
        #if SOC_ISP_SUPPORTED
        if (self->isp_proc) {
            esp_isp_disable(self->isp_proc);
            esp_isp_del_processor(self->isp_proc);
            self->isp_proc = NULL;
        }
        #endif
        mipidsi_phy_ldo_release();
        self->phy_ldo = NULL;
        nlr_pop();
    } else {
        // Nothing to do about a failed teardown, not even a partially
        // released one; swallow the exception and continue.
    }
}

void common_hal_mipicsi_camera_set_sensor(mipicsi_camera_obj_t *self, mp_obj_t sensor) {
    self->sensor = sensor;
}

void common_hal_mipicsi_camera_deinit(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        // Not currently playing: nothing to tear down.
        return;
    }

    // Only turn off the CSI receiver and ISP here (including from background
    // contexts such as reload): the sensor stops itself through its own
    // finaliser (deinit/__del__).
    mipicsi_camera_teardown(self);
    self->sensor = mp_const_none;
}
// Videosource protocol stop step: tear down the receiver hardware and stop
// the sensor so its settings can be changed. Not a user facing method: it is
// driven by the videosource protocol (from VideoBuffer.stop), so it may
// raise; exceptions from the sensor's stop() propagate.
void common_hal_mipicsi_camera_videosource_stop(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    // The receiver first: after the CSI controller stops, no more frames are
    // captured, whatever the sensor does.
    mipicsi_camera_teardown(self);

    // Stop the sensor (e.g. the sc2336 stops streaming and unlocks its
    // mode). Sensors' _stop() is expected to be idempotent.
    if (self->sensor != mp_const_none) {
        mp_obj_t stop_method[2];
        mp_load_method(self->sensor, MP_QSTR__stop, stop_method);
        mp_call_method_n_kw(0, 0, stop_method);
    }
}

// Videosource protocol pause step: stop the CSI controller's DMA without
// tearing down the receiver, ISP or sensor. The sensor keeps streaming, so the
// camera stays active and resume is quick; no frames are written to memory
// while stopped. Best effort: this runs from displayio group operations, so it
// must not raise. Called when the target is hidden. The destination is kept so
// resume() can use the latest one.
void common_hal_mipicsi_camera_videosource_pause(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    // Keep the ISP enabled too: disabling it would also stop its AE/AWB/etc.
    // processing and require a pipeline restart. Stopping the CSI controller
    // disables the CSI bridge and its GDMA channel, which is enough to keep
    // frames out of memory.
    //
    // Don't wait for it: the controller stops at the end of the frame being
    // captured (stopping mid-frame would leave a partial frame behind), up to
    // a frame time from now. That frame still lands at the current
    // destination and is reported through on_frame, so the target can
    // repaint over it.
    if (self->ctlr_started) {
        // Only fails when not started, which ctlr_started rules out.
        (void)esp_cam_ctlr_csi_stop_at_frame_end(self->handle);
        self->ctlr_started = false;
    }
    self->paused = true;
}

// Videosource protocol resume step: start the CSI controller again at the most
// recent destination. The receiver hardware, ISP and sensor were never torn
// down. Called from Python contexts, so it may raise; paused stays set if the
// controller can't start.
void common_hal_mipicsi_camera_videosource_resume(mipicsi_camera_obj_t *self) {
    if (self->handle == NULL) {
        return; // Not playing.
    }

    if (!self->ctlr_started && self->dest_base != NULL) {
        // Starting while the pause's stop is still pending cancels it; the
        // frame in flight still lands at the destination from pause time and
        // on_frame reports it there.
        CHECK_ESP_RESULT(esp_cam_ctlr_start(self->handle));
        self->ctlr_started = true;
    }
    self->paused = false;
}

bool common_hal_mipicsi_camera_deinited(mipicsi_camera_obj_t *self) {
    return self->handle == NULL;
}

bool common_hal_mipicsi_camera_set_destination(
    mipicsi_camera_obj_t *self,
    uint8_t *base,
    size_t stride,
    void (*on_frame)(void *ctx, const void *frame),
    void *ctx) {
    const size_t row_bytes = ((size_t)self->width * mipicsi_bits_per_pixel_for(self->output_format) + 7) / 8;
    const size_t frame_bytes = (size_t)self->height * row_bytes;

    // There is no stop value on the destination: stopping is the videosource
    // stop step's job. The protocol never passes NULL,
    // so this guard is only about internal misuse.
    assert(base != NULL);

    // DMA destinations must be cache-line aligned: the driver syncs the
    // buffer on every frame with esp_cache_msync, which rejects anything
    // else. PSRAM heap areas (and with 64-byte GC blocks, every GC
    // allocation in them, e.g. bytearrays) are allocated 64-byte aligned on
    // this chip, so this is a safety net.
    size_t cache_line = 0;
    esp_cache_get_alignment(MALLOC_CAP_SPIRAM, &cache_line);
    if (cache_line == 0) {
        cache_line = 64;
    }
    if ((uintptr_t)base % cache_line != 0) {
        return false;
    }

    // The destination is assumed to hold the frame rows with no stride
    // padding: the CSI link-list DMA only does contiguous transfers in this
    // IDF. The videobuffer validates the fit before getting here; the whole
    // (padded) frame must lie within the destination: rows can't wrap past
    // its end, so there is no cropping or overhang.
    if (stride != row_bytes) {
        return false;
    }

    // Size handed to the driver: the frame, cache-line padded at the end so
    // the driver's per-frame sync is happy.
    const size_t buffer_len = (frame_bytes + cache_line - 1) / cache_line * cache_line;

    portENTER_CRITICAL(&self->spinlock);
    self->dest_base = base;
    self->dest_on_frame = on_frame;
    self->dest_ctx = ctx;
    self->buffer_len = buffer_len;
    portEXIT_CRITICAL(&self->spinlock);

    // The sensor streaming and the receiver hardware exist from play();
    // here only the controller runs or doesn't. While paused, just remember
    // the new destination: resume() starts the controller there.
    if (!self->paused && !self->ctlr_started) {
        esp_err_t result = esp_cam_ctlr_start(self->handle);
        if (result != ESP_OK) {
            return false;
        }
        self->ctlr_started = true;
    }
    return true;
}

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_pixel_format(mipicsi_camera_obj_t *self) {
    return self->pixel_format;
}

mipicsi_pixel_format_t common_hal_mipicsi_camera_get_output_format(mipicsi_camera_obj_t *self) {
    return self->output_format;
}

mp_int_t common_hal_mipicsi_camera_get_width(mipicsi_camera_obj_t *self) {
    return self->width;
}

mp_int_t common_hal_mipicsi_camera_get_height(mipicsi_camera_obj_t *self) {
    return self->height;
}

mp_int_t common_hal_mipicsi_camera_get_bits_per_pixel(mipicsi_camera_obj_t *self) {
    return mipicsi_bits_per_pixel_for(self->output_format);
}
