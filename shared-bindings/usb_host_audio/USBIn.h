// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_host_audio/USBIn.h"

extern const mp_obj_type_t usb_host_audio_usbin_type;

void common_hal_usb_host_audio_usbin_construct(usb_host_audio_usbin_obj_t *self,
    usb_core_device_obj_t *device, uint8_t endpoint, uint32_t sample_rate,
    uint8_t channel_count, uint8_t bit_depth, bool samples_signed,
    uint16_t max_packet_size, uint32_t buffer_frames);
void common_hal_usb_host_audio_usbin_deinit(usb_host_audio_usbin_obj_t *self);
bool common_hal_usb_host_audio_usbin_deinited(usb_host_audio_usbin_obj_t *self);
// length and the return value count samples; a stereo frame is two samples.
uint32_t common_hal_usb_host_audio_usbin_record_to_buffer(usb_host_audio_usbin_obj_t *self,
    uint8_t *buffer, uint32_t length);
uint8_t common_hal_usb_host_audio_usbin_get_bit_depth(usb_host_audio_usbin_obj_t *self);
bool common_hal_usb_host_audio_usbin_get_samples_signed(usb_host_audio_usbin_obj_t *self);
bool common_hal_usb_host_audio_usbin_get_streaming(usb_host_audio_usbin_obj_t *self);
bool common_hal_usb_host_audio_usbin_get_overflow(usb_host_audio_usbin_obj_t *self);

void common_hal_usb_host_audio_usbin_reset_buffer(usb_host_audio_usbin_obj_t *self,
    bool single_channel_output, uint8_t channel);
audioio_get_buffer_result_t common_hal_usb_host_audio_usbin_get_buffer(
    usb_host_audio_usbin_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length);
