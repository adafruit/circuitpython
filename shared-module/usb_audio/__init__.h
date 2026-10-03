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

// The two directions a feature unit can control. A headset has one unit per
// direction; a single-direction function has one, and both names resolve to it.
typedef enum {
    USB_AUDIO_DIR_SPEAKER,
    USB_AUDIO_DIR_MICROPHONE,
} usb_audio_direction_t;

// What the host has asked for on a direction's feature unit. These are what the
// host set, not what the board did with it: by default nothing is applied to the
// samples and the application decides (see usb_audio_set_apply_host_volume).
//
// All three report the effective setting for the first channel, with the master
// and per-channel controls in series: Windows drives the per-channel volume and
// leaves the master at 0 dB, so reporting the master alone would always read
// unchanged. usb_audio_host_gain() is that as a linear factor with mute folded in
// as zero, so it can be assigned straight to an audiomixer.MixerVoice level.
// Before the host sets anything they read False, 0.0 dB and 1.0.
bool usb_audio_host_mute(usb_audio_direction_t dir);
mp_float_t usb_audio_host_volume(usb_audio_direction_t dir);
mp_float_t usb_audio_host_gain(usb_audio_direction_t dir);

// Whether the board scales the samples of this direction by the host's gain
// itself. False by default: a UAC2 device that declares the control is expected
// to honour it, but doing it silently in the background surprises applications
// that manage their own levels, so the default leaves it to the application and
// this turns on the classic sound-card behaviour.
bool usb_audio_apply_host_volume(usb_audio_direction_t dir);
void usb_audio_set_apply_host_volume(usb_audio_direction_t dir, bool apply);

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
