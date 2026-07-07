// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>

// Fixed audio format for the UAC2 microphone profile. These must have no other
// dependencies because this header is included from the TinyUSB tusb_config.h
// (to size the IN endpoint) as well as from the descriptor/binding code.

// Actual length, in bytes, of the audio function descriptor emitted for the
// current direction (mic, speaker, or headset). Declared here -- in the
// dependency-free header tusb_config.h already includes -- because TinyUSB's
// audio class driver reads CFG_TUD_AUDIO_FUNC_1_DESC_LEN at enumeration time and
// returns it to the device core as the number of configuration-descriptor bytes
// the function owns. That value MUST equal the descriptor we actually emitted:
// the three directions differ in length, so a compile-time maximum would over-
// report for the shorter ones and make the core swallow the interfaces that
// follow audio (CDC/MSC), breaking their enumeration. The full definition lives
// in __init__.c (also declared in __init__.h for the descriptor builder).
size_t usb_audio_descriptor_length(void);

// The isochronous IN endpoint's wMaxPacketSize in the USB descriptor is computed
// for this rate, so it is the highest rate usb_audio.enable() will accept.
#define USB_AUDIO_MAX_SAMPLE_RATE (48000)
#define USB_AUDIO_MAX_CHANNELS (2)

// 16-bit signed LE PCM.
#define USB_AUDIO_N_BYTES_PER_SAMPLE (2)
#define USB_AUDIO_BITS_PER_SAMPLE (USB_AUDIO_N_BYTES_PER_SAMPLE * 8)

// Endpoint number for the single isochronous audio data endpoint. Most device
// controllers accept an ISO endpoint on any number, so the descriptor builder
// just takes the next sequential one (signalled by 0 here). The nRF52 USBD,
// however, implements isochronous transfers only on a fixed, dedicated endpoint
// number (8): its dcd_edpt_open() rejects any other number for an ISO endpoint,
// so the stream silently never opens and the host sees a mic/speaker that
// transfers no data. Such ports override this (e.g. -DUSB_AUDIO_ISO_EP_NUM=8 in
// the port's mpconfigport.mk) to force the audio endpoint onto that number.
#ifndef USB_AUDIO_ISO_EP_NUM
#define USB_AUDIO_ISO_EP_NUM (0)
#endif

// Fixed UAC2 entity IDs baked into USB_AUDIO_MIC_DESCRIPTOR and the
// hand-rolled speaker descriptor (USB_AUDIO_SPEAKER_DESCRIPTOR in __init__.c).
// The speaker reuses the same IDs as the mic; only the terminal roles reverse
// (input terminal = USB streaming, output terminal = desktop speaker).
#define USB_AUDIO_ENTITY_INPUT_TERMINAL (0x01)
#define USB_AUDIO_ENTITY_FEATURE_UNIT (0x02)
#define USB_AUDIO_ENTITY_OUTPUT_TERMINAL (0x03)
#define USB_AUDIO_ENTITY_CLOCK_SOURCE (0x04)

// Combined headset (microphone + speaker both enabled) topology. A single audio function
// carries both a speaker chain (host -> board) and a mic chain (board -> host),
// so every unit/terminal needs an ID unique across the whole function -- unlike
// the single-direction descriptors above, which can reuse the same small set.
// The clock source is shared by both chains. (USB_AUDIO_HEADSET_DESCRIPTOR in
// __init__.c bakes these in.)
#define USB_AUDIO_HS_ENTITY_CLOCK_SOURCE (0x04)
#define USB_AUDIO_HS_ENTITY_SPK_INPUT_TERMINAL (0x01)   // USB streaming in from host
#define USB_AUDIO_HS_ENTITY_SPK_FEATURE_UNIT (0x02)
#define USB_AUDIO_HS_ENTITY_SPK_OUTPUT_TERMINAL (0x03)  // desktop speaker
#define USB_AUDIO_HS_ENTITY_MIC_INPUT_TERMINAL (0x05)   // generic microphone
#define USB_AUDIO_HS_ENTITY_MIC_FEATURE_UNIT (0x06)
#define USB_AUDIO_HS_ENTITY_MIC_OUTPUT_TERMINAL (0x07)  // USB streaming out to host

// Length of the no-feedback speaker descriptor. It uses the same set of
// sub-descriptors as USB_AUDIO_MIC_DESCRIPTOR (except one isochronous data
// endpoint, no feedback endpoint), so this is identical to
// USB_AUDIO_MIC_DESC_LEN -- but spell it out independently so the two
// can diverge later (e.g. stereo) without silently mis-sizing the descriptor.
// These TUD_AUDIO_DESC_*_LEN macros come from TinyUSB's usbd.h; this expression
// is only expanded where that header is already included (never at the point
// tusb_config.h includes us), so the header stays dependency-free.
#define USB_AUDIO_SPEAKER_ONE_CH_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN \
    + TUD_AUDIO_DESC_STD_AC_LEN \
    + TUD_AUDIO_DESC_CS_AC_LEN \
    + TUD_AUDIO_DESC_CLK_SRC_LEN \
    /* speaker chain: USB-streaming input terminal -> feature unit -> speaker */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN \
    /* speaker AudioStreaming interface (alt 0 + alt 1 with OUT endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

#define USB_AUDIO_SPEAKER_TWO_CH_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN \
    + TUD_AUDIO_DESC_STD_AC_LEN \
    + TUD_AUDIO_DESC_CS_AC_LEN \
    + TUD_AUDIO_DESC_CLK_SRC_LEN \
    /* speaker chain: USB-streaming input terminal -> feature unit -> speaker */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN \
    /* speaker AudioStreaming interface (alt 0 + alt 1 with OUT endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

#define USB_AUDIO_MIC_ONE_CH_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN \
    + TUD_AUDIO_DESC_STD_AC_LEN \
    + TUD_AUDIO_DESC_CS_AC_LEN \
    + TUD_AUDIO_DESC_CLK_SRC_LEN \
    /* mic chain: microphone input terminal -> feature unit -> USB-streaming out */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN \
    /* mic AudioStreaming interface (alt 0 + alt 1 with IN endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

#define USB_AUDIO_MIC_TWO_CH_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN \
    + TUD_AUDIO_DESC_STD_AC_LEN \
    + TUD_AUDIO_DESC_CS_AC_LEN \
    + TUD_AUDIO_DESC_CLK_SRC_LEN \
    /* mic chain: microphone input terminal -> feature unit -> USB-streaming out */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN \
    /* mic AudioStreaming interface (alt 0 + alt 1 with IN endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)

// Length of the combined headset descriptor (microphone + speaker both enabled): one IAD
// wrapping a single AudioControl interface plus two AudioStreaming interfaces
// (speaker OUT + mic IN). The AC interface declares one shared clock, plus an
// input terminal / feature unit / output terminal for each of the two chains;
// each AS interface contributes a zero-bandwidth alt 0 and a streaming alt 1
// with one isochronous data endpoint (no feedback endpoint, matching the
// single-direction descriptors). See USB_AUDIO_HEADSET_DESCRIPTOR in __init__.c.
// Expanded only where TinyUSB's usbd.h is already included (never at the point
// tusb_config.h includes us), so this header stays dependency-free.
#define USB_AUDIO_HEADSET_DESC_LEN (TUD_AUDIO_DESC_IAD_LEN \
    + TUD_AUDIO_DESC_STD_AC_LEN \
    + TUD_AUDIO_DESC_CS_AC_LEN \
    + TUD_AUDIO_DESC_CLK_SRC_LEN \
    /* speaker chain: USB-streaming input terminal -> feature unit -> speaker */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    /* mic chain: microphone input terminal -> feature unit -> USB-streaming out */ \
    + TUD_AUDIO_DESC_INPUT_TERM_LEN \
    + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN \
    + TUD_AUDIO_DESC_OUTPUT_TERM_LEN \
    /* speaker AudioStreaming interface (alt 0 + alt 1 with OUT endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN \
    /* mic AudioStreaming interface (alt 0 + alt 1 with IN endpoint) */ \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_STD_AS_INT_LEN \
    + TUD_AUDIO_DESC_CS_AS_INT_LEN \
    + TUD_AUDIO_DESC_TYPE_I_FORMAT_LEN \
    + TUD_AUDIO_DESC_STD_AS_ISO_EP_LEN \
    + TUD_AUDIO_DESC_CS_AS_ISO_EP_LEN)
