// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "py/obj.h"
#include "supervisor/usb.h"

// Enable/disable the USB Audio Class (UAC2) interface. These may only be
// called before USB is connected (i.e. from boot.py); they return false
// otherwise. At least one of microphone/speaker must be true; enabling both
// presents a combined headset.
bool shared_module_usb_audio_enable(mp_int_t sample_rate, mp_int_t channel_count, bool microphone, bool speaker);
bool shared_module_usb_audio_disable(void);

// True once enable() has been called successfully.
bool usb_audio_enabled(void);

// True while the host has opened either AudioStreaming alternate setting, i.e. it
// is actively listening or sending. This is the real "stream the audio now"
// signal.
bool usb_audio_streaming(void);

// True while the host has the speaker (host -> board OUT) stream open. Distinct
// from usb_audio_streaming() so USBSpeaker can report its own direction even when
// it shares a headset function with a microphone.
bool usb_audio_speaker_streaming(void);

// Negotiated audio format, valid when usb_audio_enabled() is true.
extern uint32_t usb_audio_sample_rate;
extern uint8_t usb_audio_channel_count;

// Which streams were requested in enable(), valid when usb_audio_enabled() is
// true. Both true presents a combined headset.
extern bool usb_audio_microphone_enabled;
extern bool usb_audio_speaker_enabled;

// Descriptor injection hooks, called from supervisor/shared/usb/usb_desc.c.
size_t usb_audio_descriptor_length(void);
size_t usb_audio_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string);

// Background task that streams samples to the host, called from
// supervisor/shared/usb/usb.c.
void usb_audio_task(void);

// (Re)create the USBMicrophone and USBSpeaker singleton instances for the
// current VM and install them as the usb_audio.usb_microphone and
// usb_audio.usb_speaker module attributes (or None when that direction was not
// enabled in boot.py). Called once per VM from usb_setup_with_vm(), mirroring
// usb_midi_setup_ports(): the instances live on the GC heap, which is reset
// between boot.py and code.py, so they must be rebuilt each time.
void usb_audio_setup_singletons(void);
