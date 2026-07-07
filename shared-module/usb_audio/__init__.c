// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb_audio/__init__.h"
#include "shared-bindings/usb_audio/USBMicrophone.h"
#include "shared-bindings/usb_audio/USBSpeaker.h"
#include "shared-module/usb_audio/__init__.h"
#include "shared-module/usb_audio/USBMicrophone.h"
#include "shared-module/usb_audio/USBSpeaker.h"
#include "shared-module/usb_audio/usb_audio_descriptors.h"

#include <string.h>

#include "py/misc.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "supervisor/shared/tick.h"
#include "tusb.h"

int16_t *usb_audio_mic_samples;
size_t usb_audio_mic_samples_len;

static bool usb_audio_is_enabled = false;

// The host opens each AudioStreaming interface independently (alt 0 = idle, alt 1
// = streaming), so the two directions of a headset are tracked separately. For
// the single-direction microphone/speaker only the matching flag is ever set.
static bool usb_audio_mic_streaming = false;
static bool usb_audio_spk_streaming = false;

// AudioStreaming interface numbers assigned when the descriptor is built, used to
// route the host's set-interface requests to the right direction. 0xff until a
// descriptor that includes that direction has been emitted.
static uint8_t usb_audio_mic_as_itf = 0xff;
static uint8_t usb_audio_spk_as_itf = 0xff;

uint32_t usb_audio_sample_rate;
uint8_t usb_audio_channel_count;
uint8_t usb_audio_bits_per_sample;
bool usb_audio_microphone_enabled;
bool usb_audio_speaker_enabled;

// Audio control state surfaced to the host. One extra entry for the master channel 0.
static int8_t usb_audio_mute[USB_AUDIO_MAX_CHANNELS + 1];
static int16_t usb_audio_volume[USB_AUDIO_MAX_CHANNELS + 1];

bool shared_module_usb_audio_enable(mp_int_t sample_rate, mp_int_t channel_count, mp_int_t bits_per_sample, bool microphone, bool speaker) {
    if (tud_connected()) {
        return false;
    }

    // One scratch chunk (1 ms at the sample rate); we loop until the FIFO reaches
    // the setpoint.
    usb_audio_mic_samples_len = sample_rate / 1000 * channel_count * USB_AUDIO_N_BYTES_PER_SAMPLE;
    usb_audio_mic_samples = (int16_t *)m_malloc_without_collect(usb_audio_mic_samples_len);
    if (usb_audio_mic_samples == NULL) {
        m_malloc_fail(usb_audio_mic_samples_len);
        return false;
    }
    memset(usb_audio_mic_samples, 0, usb_audio_mic_samples_len);

    usb_audio_sample_rate = sample_rate;
    usb_audio_channel_count = channel_count;
    usb_audio_bits_per_sample = bits_per_sample;
    usb_audio_microphone_enabled = microphone;
    usb_audio_speaker_enabled = speaker;
    usb_audio_is_enabled = true;

    return true;
}

bool shared_module_usb_audio_disable(void) {
    if (tud_connected()) {
        return false;
    }
    usb_audio_is_enabled = false;
    return true;
}

bool usb_audio_enabled(void) {
    return usb_audio_is_enabled;
}

bool usb_audio_streaming(void) {
    return usb_audio_mic_streaming || usb_audio_spk_streaming;
}

// True while the host has the speaker (OUT) stream open, i.e. it is actively
// sending audio. Used by USBSpeaker.connected so it reflects the speaker
// direction specifically even when a mic shares the same headset function.
bool usb_audio_speaker_streaming(void) {
    return usb_audio_spk_streaming;
}

void usb_audio_setup_singletons(void) {
    // USBMicrophone and USBSpeaker are singletons rather than constructible
    // classes (like usb_midi.ports). The host-facing format and direction are
    // fixed by usb_audio.enable() in boot.py and persist in C globals, but the
    // instances themselves live on the GC heap, which is reset between boot.py
    // and code.py, so they are rebuilt here once per VM. Each is created only
    // when its direction was enabled; otherwise it is left as None.
    //
    // The objects are held in MP_STATE_VM root pointers so the GC keeps them
    // alive for the whole VM (the module globals table is static data and is not
    // a GC root, so a reference from there alone would be swept). Rooting the
    // microphone also traces its bound audiosample (self->sample). They are then
    // installed in the module globals so they are reachable as the
    // usb_audio.usb_microphone / usb_audio.usb_speaker attributes.
    mp_obj_t microphone = mp_const_none;
    mp_obj_t speaker = mp_const_none;

    if (usb_audio_is_enabled) {
        const bool has_input = usb_audio_microphone_enabled;
        const bool has_output = usb_audio_speaker_enabled;

        if (has_input) {
            usb_audio_usbmicrophone_obj_t *self =
                mp_obj_malloc_with_finaliser(usb_audio_usbmicrophone_obj_t, &usb_audio_USBMicrophone_type);
            common_hal_usb_audio_usbmicrophone_construct(self);
            microphone = MP_OBJ_FROM_PTR(self);
        }
        if (has_output) {
            usb_audio_usbspeaker_obj_t *self =
                mp_obj_malloc_with_finaliser(usb_audio_usbspeaker_obj_t, &usb_audio_USBSpeaker_type);
            common_hal_usb_audio_usbspeaker_construct(self);
            speaker = MP_OBJ_FROM_PTR(self);
        }
    }

    MP_STATE_VM(usb_audio_microphone_singleton) = microphone;
    MP_STATE_VM(usb_audio_speaker_singleton) = speaker;

    mp_map_lookup(&usb_audio_module_globals.map, MP_ROM_QSTR(MP_QSTR_usb_microphone), MP_MAP_LOOKUP)->value =
        microphone;
    mp_map_lookup(&usb_audio_module_globals.map, MP_ROM_QSTR(MP_QSTR_usb_speaker), MP_MAP_LOOKUP)->value =
        speaker;
}

// Hand-rolled UAC2 speaker (host -> board) descriptor WITHOUT an async
// feedback endpoint. This mirrors TinyUSB's TUD_AUDIO_SPEAKER_STEREO_FB_DESCRIPTOR
// (lib/tinyusb/src/device/usbd.h) but drops the trailing feedback endpoint, so
// the streaming alt-setting declares a single OUT endpoint (_nEPs = 0x01). The
// entity IDs match the mic descriptor (see usb_audio_descriptors.h); only the
// terminal roles reverse: the input terminal is the USB-streaming side and the
// output terminal is the desktop speaker, and the AS interface links the input
// terminal (0x01). Async feedback for true clock matching is a later step.
#define USB_AUDIO_SPEAKER_ONE_CH_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epout, _epsize) \
    /* Standard Interface Association Descriptor (IAD) */ \
    TUD_AUDIO_DESC_IAD(/*_firstitf*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00), \
    /* Standard AC Interface Descriptor(4.7.1) */ \
    TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx), \
    /* Class-Specific AC Interface Header Descriptor(4.7.2) */ \
    TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_DESKTOP_SPEAKER, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS), \
    /* Clock Source Descriptor(4.7.2.1) */ \
    TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_stridx*/ 0x00), \
    /* Input Terminal Descriptor(4.7.2.4) -- USB streaming in from the host */ \
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00), \
    /* Output Terminal Descriptor(4.7.2.5) -- desktop speaker */ \
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_OUT_DESKTOP_SPEAKER, /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_srcid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
    /* Feature Unit Descriptor(4.7.2.8) */ \
    TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL(/*_unitid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 0, zero bandwidth */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 1, one OUT endpoint */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00), \
    /* Class-Specific AS Interface Descriptor(4.9.2) -- linked to the input terminal */ \
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00), \
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
    TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (uint8_t)((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

#define USB_AUDIO_SPEAKER_TWO_CH_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epout, _epsize) \
    /* Standard Interface Association Descriptor (IAD) */ \
    TUD_AUDIO_DESC_IAD(/*_firstitf*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00), \
    /* Standard AC Interface Descriptor(4.7.1) */ \
    TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx), \
    /* Class-Specific AC Interface Header Descriptor(4.7.2) */ \
    TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_DESKTOP_SPEAKER, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS), \
    /* Clock Source Descriptor(4.7.2.1) */ \
    TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_stridx*/ 0x00), \
    /* Input Terminal Descriptor(4.7.2.4) -- USB streaming in from the host */ \
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00), \
    /* Output Terminal Descriptor(4.7.2.5) -- desktop speaker */ \
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_OUT_DESKTOP_SPEAKER, /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_srcid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
    /* Feature Unit Descriptor(4.7.2.8) */ \
    TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(/*_unitid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch2*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 0, zero bandwidth */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 1, one OUT endpoint */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00), \
    /* Class-Specific AS Interface Descriptor(4.9.2) -- linked to the input terminal */ \
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00), \
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
    TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (uint8_t)((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

// Hand-rolled UAC2 microphone (board -> host) descriptor WITHOUT an async
// feedback endpoint. This mirrors TinyUSB's TUD_AUDIO_MIC_ONE_CH_DESCRIPTOR
// (lib/tinyusb/src/device/usbd.h) but drops the trailing feedback endpoint, so
// the streaming alt-setting declares a single IN endpoint (_nEPs = 0x00). Async
// feedback for true clock matching is a later step.
#define USB_AUDIO_MIC_ONE_CH_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epin, _epsize) \
  /* Standard Interface Association Descriptor (IAD) */ \
  TUD_AUDIO_DESC_IAD(/*_firstitf*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00), \
  /* Standard AC Interface Descriptor(4.7.1) */ \
  TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx), \
  /* Class-Specific AC Interface Header Descriptor(4.7.2) */ \
  TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_MICROPHONE, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS), \
  /* Clock Source Descriptor(4.7.2.1) */ \
  TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL,  /*_stridx*/ 0x00), \
  /* Input Terminal Descriptor(4.7.2.4) -- microphone */ \
  TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_IN_GENERIC_MIC, /*_assocTerm*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS, /*_stridx*/ 0x00), \
  /* Output Terminal Descriptor(4.7.2.5) -- USB streaming */ \
  TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_srcid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
  /* Feature Unit Descriptor(4.7.2.8) */ \
  TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL(/*_unitid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
  /* Standard AS Interface Descriptor(4.9.1) */ \
  /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */ \
  TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00),\
  /* Standard AS Interface Descriptor(4.9.1) */ \
  /* Interface 1, Alternate 1 - alternate interface for data streaming */ \
  TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00),\
  /* Class-Specific AS Interface Descriptor(4.9.2) */ \
  TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
  /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
  TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
  /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
  TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
  /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
  TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

#define USB_AUDIO_MIC_TWO_CH_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epin, _epsize) \
  /* Standard Interface Association Descriptor (IAD) */ \
  TUD_AUDIO_DESC_IAD(/*_firstitf*/ _itfnum, /*_nitfs*/ 0x02, /*_stridx*/ 0x00), \
  /* Standard AC Interface Descriptor(4.7.1) */ \
  TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx), \
  /* Class-Specific AC Interface Header Descriptor(4.7.2) */ \
  TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_MICROPHONE, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL_LEN, /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS), \
  /* Clock Source Descriptor(4.7.2.1) */ \
  TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL,  /*_stridx*/ 0x00), \
  /* Input Terminal Descriptor(4.7.2.4) -- microphone */ \
  TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_IN_GENERIC_MIC, /*_assocTerm*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS, /*_stridx*/ 0x00), \
  /* Output Terminal Descriptor(4.7.2.5) -- USB streaming */ \
  TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_srcid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
  /* Feature Unit Descriptor(4.7.2.8) */ \
  TUD_AUDIO_DESC_FEATURE_UNIT_TWO_CHANNEL(/*_unitid*/ USB_AUDIO_ENTITY_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_ENTITY_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch2*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
  /* Standard AS Interface Descriptor(4.9.1) */ \
  /* Interface 1, Alternate 0 - default alternate setting with 0 bandwidth */ \
  TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00),\
  /* Standard AS Interface Descriptor(4.9.1) */ \
  /* Interface 1, Alternate 1 - alternate interface for data streaming */ \
  TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00),\
  /* Class-Specific AS Interface Descriptor(4.9.2) */ \
  TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_ENTITY_OUTPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x02, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00),\
  /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
  TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
  /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
  TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (uint8_t) ((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
  /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
  TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

// Hand-rolled UAC2 mono headset (microphone + speaker both enabled): one audio function
// presenting both a speaker (host -> board OUT) and a microphone (board -> host
// IN) at once. This combines USB_AUDIO_SPEAKER_DESCRIPTOR's speaker chain with
// USB_AUDIO_MIC_DESCRIPTOR's mic chain under a single IAD. The two chains
// must use distinct entity IDs (USB_AUDIO_HS_ENTITY_*; see usb_audio_descriptors.h)
// because they live in the same AudioControl interface, and they share one clock
// source. The function spans three interfaces: AudioControl (_itfnum), the
// speaker AudioStreaming interface (_itfnum + 1, OUT endpoint), and the mic
// AudioStreaming interface (_itfnum + 2, IN endpoint). Neither stream has an
// async feedback endpoint, matching the single-direction descriptors.
#define USB_AUDIO_HEADSET_DESCRIPTOR(_itfnum, _stridx, _nBytesPerSample, _nBitsUsedPerSample, _epout, _epin, _epsize) \
    /* Standard Interface Association Descriptor (IAD) -- 3 interfaces */ \
    TUD_AUDIO_DESC_IAD(/*_firstitf*/ _itfnum, /*_nitfs*/ 0x03, /*_stridx*/ 0x00), \
    /* Standard AC Interface Descriptor(4.7.1) */ \
    TUD_AUDIO_DESC_STD_AC(/*_itfnum*/ _itfnum, /*_nEPs*/ 0x00, /*_stridx*/ _stridx), \
    /* Class-Specific AC Interface Header Descriptor(4.7.2) -- clock + both chains */ \
    TUD_AUDIO_DESC_CS_AC(/*_bcdADC*/ 0x0200, /*_category*/ AUDIO_FUNC_HEADSET, /*_totallen*/ TUD_AUDIO_DESC_CLK_SRC_LEN + 2 * (TUD_AUDIO_DESC_INPUT_TERM_LEN + TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL_LEN + TUD_AUDIO_DESC_OUTPUT_TERM_LEN), /*_ctrl*/ AUDIO_CS_AS_INTERFACE_CTRL_LATENCY_POS), \
    /* Clock Source Descriptor(4.7.2.1) -- shared by both chains */ \
    TUD_AUDIO_DESC_CLK_SRC(/*_clkid*/ USB_AUDIO_HS_ENTITY_CLOCK_SOURCE, /*_attr*/ AUDIO_CLOCK_SOURCE_ATT_INT_FIX_CLK, /*_ctrl*/ (AUDIO_CTRL_R << AUDIO_CLOCK_SOURCE_CTRL_CLK_FRQ_POS), /*_assocTerm*/ 0x00, /*_stridx*/ 0x00), \
    /* --- Speaker chain (host -> board) --- */ \
    /* Input Terminal Descriptor(4.7.2.4) -- USB streaming in from the host */ \
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_HS_ENTITY_SPK_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_clkid*/ USB_AUDIO_HS_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ 0 * (AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS), /*_stridx*/ 0x00), \
    /* Feature Unit Descriptor(4.7.2.8) */ \
    TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL(/*_unitid*/ USB_AUDIO_HS_ENTITY_SPK_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_HS_ENTITY_SPK_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
    /* Output Terminal Descriptor(4.7.2.5) -- desktop speaker */ \
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_HS_ENTITY_SPK_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_OUT_DESKTOP_SPEAKER, /*_assocTerm*/ 0x00, /*_srcid*/ USB_AUDIO_HS_ENTITY_SPK_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_HS_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
    /* --- Mic chain (board -> host) --- */ \
    /* Input Terminal Descriptor(4.7.2.4) -- generic microphone */ \
    TUD_AUDIO_DESC_INPUT_TERM(/*_termid*/ USB_AUDIO_HS_ENTITY_MIC_INPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_IN_GENERIC_MIC, /*_assocTerm*/ 0x00, /*_clkid*/ USB_AUDIO_HS_ENTITY_CLOCK_SOURCE, /*_nchannelslogical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_idxchannelnames*/ 0x00, /*_ctrl*/ AUDIO_CTRL_R << AUDIO_IN_TERM_CTRL_CONNECTOR_POS, /*_stridx*/ 0x00), \
    /* Feature Unit Descriptor(4.7.2.8) */ \
    TUD_AUDIO_DESC_FEATURE_UNIT_ONE_CHANNEL(/*_unitid*/ USB_AUDIO_HS_ENTITY_MIC_FEATURE_UNIT, /*_srcid*/ USB_AUDIO_HS_ENTITY_MIC_INPUT_TERMINAL, /*_ctrlch0master*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_ctrlch1*/ AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_MUTE_POS | AUDIO_CTRL_RW << AUDIO_FEATURE_UNIT_CTRL_VOLUME_POS, /*_stridx*/ 0x00), \
    /* Output Terminal Descriptor(4.7.2.5) -- USB streaming out to the host */ \
    TUD_AUDIO_DESC_OUTPUT_TERM(/*_termid*/ USB_AUDIO_HS_ENTITY_MIC_OUTPUT_TERMINAL, /*_termtype*/ AUDIO_TERM_TYPE_USB_STREAMING, /*_assocTerm*/ 0x00, /*_srcid*/ USB_AUDIO_HS_ENTITY_MIC_FEATURE_UNIT, /*_clkid*/ USB_AUDIO_HS_ENTITY_CLOCK_SOURCE, /*_ctrl*/ 0x0000, /*_stridx*/ 0x00), \
    /* --- Speaker AudioStreaming interface (_itfnum + 1) --- */ \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 0, zero bandwidth */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 1, one OUT endpoint */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 1), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00), \
    /* Class-Specific AS Interface Descriptor(4.9.2) -- linked to the speaker input terminal */ \
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_HS_ENTITY_SPK_INPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00), \
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
    TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epout, /*_attr*/ (uint8_t)((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000), \
    /* --- Mic AudioStreaming interface (_itfnum + 2) --- */ \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 0, zero bandwidth */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 2), /*_altset*/ 0x00, /*_nEPs*/ 0x00, /*_stridx*/ 0x00), \
    /* Standard AS Interface Descriptor(4.9.1) -- alt 1, one IN endpoint */ \
    TUD_AUDIO_DESC_STD_AS_INT(/*_itfnum*/ (uint8_t)((_itfnum) + 2), /*_altset*/ 0x01, /*_nEPs*/ 0x01, /*_stridx*/ 0x00), \
    /* Class-Specific AS Interface Descriptor(4.9.2) -- linked to the mic output terminal */ \
    TUD_AUDIO_DESC_CS_AS_INT(/*_termid*/ USB_AUDIO_HS_ENTITY_MIC_OUTPUT_TERMINAL, /*_ctrl*/ AUDIO_CTRL_NONE, /*_formattype*/ AUDIO_FORMAT_TYPE_I, /*_formats*/ AUDIO_DATA_FORMAT_TYPE_I_PCM, /*_nchannelsphysical*/ 0x01, /*_channelcfg*/ AUDIO_CHANNEL_CONFIG_NON_PREDEFINED, /*_stridx*/ 0x00), \
    /* Type I Format Type Descriptor(2.3.1.6 - Audio Formats) */ \
    TUD_AUDIO_DESC_TYPE_I_FORMAT(_nBytesPerSample, _nBitsUsedPerSample), \
    /* Standard AS Isochronous Audio Data Endpoint Descriptor(4.10.1.1) */ \
    TUD_AUDIO_DESC_STD_AS_ISO_EP(/*_ep*/ _epin, /*_attr*/ (uint8_t)((uint8_t)TUSB_XFER_ISOCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_ASYNCHRONOUS | (uint8_t)TUSB_ISO_EP_ATT_DATA), /*_maxEPsize*/ _epsize, /*_interval*/ 0x01), \
    /* Class-Specific AS Isochronous Audio Data Endpoint Descriptor(4.10.1.2) */ \
    TUD_AUDIO_DESC_CS_AS_ISO_EP(/*_attr*/ AUDIO_CS_AS_ISO_DATA_EP_ATT_NON_MAX_PACKETS_OK, /*_ctrl*/ AUDIO_CTRL_NONE, /*_lockdelayunit*/ AUDIO_CS_AS_ISO_DATA_EP_LOCK_DELAY_UNIT_UNDEFINED, /*_lockdelay*/ 0x0000)

// Combined headset: both a microphone (board -> host IN) and a speaker
// (host -> board OUT) under one audio function.
static bool usb_audio_direction_is_input_output(void) {
    return usb_audio_microphone_enabled && usb_audio_speaker_enabled;
}

// Speaker only (host -> board OUT), no microphone.
static bool usb_audio_direction_is_output(void) {
    return usb_audio_speaker_enabled && !usb_audio_microphone_enabled;
}

size_t usb_audio_descriptor_length(void) {
    if (usb_audio_direction_is_input_output()) {
        return USB_AUDIO_HEADSET_DESC_LEN;
    }
    if (usb_audio_direction_is_output()) {
        if (usb_audio_channel_count == 1) {
            return USB_AUDIO_SPEAKER_ONE_CH_DESC_LEN;
        } else {
            return USB_AUDIO_SPEAKER_TWO_CH_DESC_LEN;
        }
    }
    if (usb_audio_channel_count == 1) {
        return USB_AUDIO_MIC_ONE_CH_DESC_LEN;
    } else {
        return USB_AUDIO_MIC_TWO_CH_DESC_LEN;
    }
}

size_t usb_audio_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    // Pick the isochronous endpoint number. By default it follows the same
    // sequential allocation as every other interface. On ports that pin ISO to a
    // fixed, dedicated endpoint (USB_AUDIO_ISO_EP_NUM != 0; see the header for the
    // nRF52 case), we use that number instead and leave the sequential counters
    // untouched: the dedicated ISO endpoint is a separate hardware resource, so
    // it must not consume one of the regular endpoint numbers the other
    // interfaces draw from.
    const bool forced_iso_ep = (USB_AUDIO_ISO_EP_NUM != 0);
    const uint8_t iso_ep_num = forced_iso_ep ? USB_AUDIO_ISO_EP_NUM : descriptor_counts->current_endpoint;

    if (usb_audio_direction_is_input_output()) {
        // Combined headset: a speaker AudioStreaming interface (OUT) and a mic
        // AudioStreaming interface (IN) under one AudioControl interface. This
        // needs two isochronous endpoints, so it cannot be served by ports that
        // pin ISO to a single dedicated endpoint number (forced_iso_ep, e.g.
        // nRF52, which has only one ISO-capable endpoint). On those ports the
        // sequential numbers below will not match the hardware's required ISO
        // endpoint and the stream will not open; INPUT_OUTPUT is effectively
        // unsupported there. The sequential-allocation ports (e.g. RP2) take a
        // distinct number for each direction.
        const uint8_t ep_out = descriptor_counts->current_endpoint;
        const uint8_t ep_in = descriptor_counts->current_endpoint + 1;

        usb_add_interface_string(*current_interface_string, "CircuitPython Headset");
        (*current_interface_string)++;
        
        const uint8_t usb_audio_descriptor[] = {
            USB_AUDIO_HEADSET_DESCRIPTOR(
                /*_itfnum*/ descriptor_counts->current_interface,
                /*_stridx*/ *current_interface_string,
                /*_nBytesPerSample*/ USB_AUDIO_N_BYTES_PER_SAMPLE,
                /*_nBitsUsedPerSample*/ USB_AUDIO_BITS_PER_SAMPLE,
                /*_epout*/ ep_out,
                /*_epin*/ ep_in | 0x80,
                /*_epsize*/ CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX)
        };

        // Speaker AS is the first interface after AudioControl, mic AS the second.
        usb_audio_spk_as_itf = descriptor_counts->current_interface + 1;
        usb_audio_mic_as_itf = descriptor_counts->current_interface + 2;

        // One IAD wrapping an AudioControl + two AudioStreaming interfaces, plus
        // one OUT and one IN endpoint.
        descriptor_counts->current_interface += 3;
        descriptor_counts->num_out_endpoints++;
        descriptor_counts->num_in_endpoints++;
        descriptor_counts->current_endpoint += 2;

        memcpy(descriptor_buf, usb_audio_descriptor, sizeof(usb_audio_descriptor));

        return sizeof(usb_audio_descriptor);
    }

    if (usb_audio_direction_is_output()) {
        usb_add_interface_string(*current_interface_string, "CircuitPython Speaker");
        (*current_interface_string)++;

        // The AudioStreaming interface follows the AudioControl interface.
        usb_audio_spk_as_itf = descriptor_counts->current_interface + 1;

        if (usb_audio_channel_count == 1) {
            const uint8_t usb_audio_descriptor[] = {
                USB_AUDIO_SPEAKER_ONE_CH_DESCRIPTOR(
                    /*_itfnum*/ descriptor_counts->current_interface,
                    /*_stridx*/ *current_interface_string,
                    /*_nBytesPerSample*/ USB_AUDIO_N_BYTES_PER_SAMPLE,
                    /*_nBitsUsedPerSample*/ USB_AUDIO_BITS_PER_SAMPLE,
                    /*_epout*/ iso_ep_num,
                    /*_epsize*/ CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX)
            };

            // One IAD wrapping an AudioControl + an AudioStreaming interface, plus one OUT endpoint.
            descriptor_counts->current_interface += 2;
            if (!forced_iso_ep) {
                descriptor_counts->num_out_endpoints++;
                descriptor_counts->current_endpoint++;
            }

            memcpy(descriptor_buf, usb_audio_descriptor, sizeof(usb_audio_descriptor));

            return sizeof(usb_audio_descriptor);
        } else {
            const uint8_t usb_audio_descriptor[] = {
                USB_AUDIO_SPEAKER_TWO_CH_DESCRIPTOR(
                    /*_itfnum*/ descriptor_counts->current_interface,
                    /*_stridx*/ *current_interface_string,
                    /*_nBytesPerSample*/ USB_AUDIO_N_BYTES_PER_SAMPLE,
                    /*_nBitsUsedPerSample*/ USB_AUDIO_BITS_PER_SAMPLE,
                    /*_epout*/ iso_ep_num,
                    /*_epsize*/ CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX)
            };

            // One IAD wrapping an AudioControl + an AudioStreaming interface, plus one OUT endpoint.
            descriptor_counts->current_interface += 3;
            if (!forced_iso_ep) {
                descriptor_counts->num_out_endpoints += 2;
                descriptor_counts->current_endpoint += 2;
            }

            memcpy(descriptor_buf, usb_audio_descriptor, sizeof(usb_audio_descriptor));

            return sizeof(usb_audio_descriptor);
        }
    }

    usb_add_interface_string(*current_interface_string, "CircuitPython Microphone");
    (*current_interface_string)++;

    // The AudioStreaming interface follows the AudioControl interface.
    usb_audio_mic_as_itf = descriptor_counts->current_interface + 1;

    if (usb_audio_channel_count == 2) {
        const uint8_t usb_audio_descriptor[] = {
            USB_AUDIO_MIC_ONE_CH_DESCRIPTOR(
                /*_itfnum*/ descriptor_counts->current_interface,
                /*_stridx*/ *current_interface_string,
                /*_nBytesPerSample*/ USB_AUDIO_N_BYTES_PER_SAMPLE,
                /*_nBitsUsedPerSample*/ USB_AUDIO_BITS_PER_SAMPLE,
                /*_epin*/ iso_ep_num | 0x80,
                /*_epsize*/ CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX)
        };

        // One IAD wrapping an AudioControl + an AudioStreaming interface, plus one IN endpoint.
        descriptor_counts->current_interface += 2;
        if (!forced_iso_ep) {
            descriptor_counts->num_in_endpoints++;
            descriptor_counts->current_endpoint++;
        }

        memcpy(descriptor_buf, usb_audio_descriptor, sizeof(usb_audio_descriptor));

        return sizeof(usb_audio_descriptor);
    } else {
        const uint8_t usb_audio_descriptor[] = {
            USB_AUDIO_MIC_TWO_CH_DESCRIPTOR(
                /*_itfnum*/ descriptor_counts->current_interface,
                /*_stridx*/ *current_interface_string,
                /*_nBytesPerSample*/ USB_AUDIO_N_BYTES_PER_SAMPLE,
                /*_nBitsUsedPerSample*/ USB_AUDIO_BITS_PER_SAMPLE,
                /*_epin*/ iso_ep_num | 0x80,
                /*_epsize*/ CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX)
        };

        // One IAD wrapping an AudioControl + an AudioStreaming interface, plus one IN endpoint.
        descriptor_counts->current_interface += 3;
        if (!forced_iso_ep) {
            descriptor_counts->num_in_endpoints += 2;
            descriptor_counts->current_endpoint += 2;
        }

        memcpy(descriptor_buf, usb_audio_descriptor, sizeof(usb_audio_descriptor));

        return sizeof(usb_audio_descriptor);
    }
}

// --------------------------------------------------------------------+
// Speaker (host -> board) receive task
// --------------------------------------------------------------------+

// Drain everything the host has delivered to the OUT endpoint since the last
// pass into the active USBSpeaker's ring. TinyUSB's weak rx-done handler has
// already moved the isochronous data into ep_out_ff; we copy it out here, in
// task (non-ISR) context, matching the project's "defer ISR work" rule. The ring
// itself, and the overrun/underrun handling, live in USBSpeaker.c so the data
// sits in the audiosample source the output backend pulls from.
static void usb_audio_speaker_task(void) {
    // One USB packet of scratch; we loop until ep_out_ff is empty.
    static uint8_t chunk[CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX];

    while (tud_audio_available() > 0) {
        uint16_t got = tud_audio_read(chunk, sizeof(chunk));
        if (got == 0) {
            break;
        }
        // tud_audio_read() never returns more than the buffer it was given, but
        // clamp so the compiler can prove the drain copies stay within chunk[]
        // once this loop is inlined into it.
        if (got > sizeof(chunk)) {
            got = sizeof(chunk);
        }
        usb_audio_usbspeaker_background_drain(chunk, got);
    }
}

// --------------------------------------------------------------------+
// Microphone (board -> host) transmit task
// --------------------------------------------------------------------+

static void usb_audio_microphone_task(void) {
    // Pace production by the IN FIFO level. Each pass we top the software FIFO
    // back up to its half-full setpoint, generating only the samples the host has
    // actually drained since the last pass. This limits our production rate to the
    // host's true consumption rate (its USB SOF / audio clock), keeps the FIFO
    // around the level TinyUSB's flow control targets so it can send steady
    // nominal-size packets, and automatically catches up after any scheduling gap
    // (GCpause, other background work) in a single pass instead of underrunning.
    // Underruns here showed up to the host as discrete sample-drop/insert splices,
    // i.e. the erratic ticking on a held tone.
    tu_fifo_t *ep_in_ff = tud_audio_get_ep_in_ff();
    uint16_t const target = tu_fifo_depth(ep_in_ff) / 2;

    uint16_t count;
    while ((count = tu_fifo_count(ep_in_ff)) < target) {
        size_t want = target - count;
        if (want > usb_audio_mic_samples_len) {
            want = usb_audio_mic_samples_len;
        }

        // Pull the next chunk from whichever USBMicrophone is playing.
        bool underran = false;
        size_t filled = usb_audio_usbmicrophone_background_fill((uint8_t *)usb_audio_mic_samples, want);
        if (filled == 0) {
            // No source attached, paused, or fully drained: keep the endpoint
            // alive with silence so the host never sees a starved stream.
            memset((uint8_t *)usb_audio_mic_samples, 0, want);
        } else if (filled < want) {
            // The source momentarily underran. Send just what it produced and
            // let the FIFO cushion ride until it catches up next pass, rather
            // than flooding the stream with a burst of silence.
            want = filled;
            underran = true;
        }

        if (tud_audio_write((uint8_t *)usb_audio_mic_samples, (uint16_t)want) == 0) {
            break;  // FIFO unexpectedly full / host not ready
        }
        if (underran) {
            break;
        }
    }
}

void usb_audio_task(void) {
    // Each direction is gated on the host having opened its AudioStreaming
    // interface. For a headset both run in the same pass: drain the host's
    // speaker audio and refill the mic stream independently. The single-direction
    // modes simply never have the other flag set.
    if (usb_audio_spk_streaming) {
        usb_audio_speaker_task();
    }
    if (usb_audio_mic_streaming) {
        usb_audio_microphone_task();
    }
}

// --------------------------------------------------------------------+
// TinyUSB audio class callbacks (weak symbols overridden here)
// --------------------------------------------------------------------+

// Host opened/closed the AudioStreaming alternate setting. The host selects each
// direction's interface independently (a headset has two), so route by interface
// number to the matching streaming flag rather than assuming a single stream.
bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
    (void)rhport;
    uint8_t const itf = (uint8_t)tu_u16_low(p_request->wIndex);
    bool const streaming = (tu_u16_low(p_request->wValue) != 0);

    if (itf == usb_audio_spk_as_itf) {
        usb_audio_spk_streaming = streaming;
        // Start each speaker streaming session from live audio: drop anything
        // left in the OUT FIFO/ring from a previous session.
        tud_audio_clear_ep_out_ff();
        usb_audio_usbspeaker_streaming_reset();
    } else if (itf == usb_audio_mic_as_itf) {
        usb_audio_mic_streaming = streaming;
    }
    return true;
}

bool tud_audio_set_itf_close_ep_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
    (void)rhport;
    uint8_t const itf = (uint8_t)tu_u16_low(p_request->wIndex);

    if (itf == usb_audio_spk_as_itf) {
        usb_audio_spk_streaming = false;
        tud_audio_clear_ep_out_ff();
        usb_audio_usbspeaker_streaming_reset();
    } else if (itf == usb_audio_mic_as_itf) {
        usb_audio_mic_streaming = false;
    }
    return true;
}

// Class-specific SET requests for an entity (we accept mute/volume on the feature unit).
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *pBuff) {
    (void)rhport;

    uint8_t const channelNum = (uint8_t)tu_u16_low(p_request->wValue);
    uint8_t const ctrlSel = (uint8_t)tu_u16_high(p_request->wValue);
    uint8_t const entityID = (uint8_t)tu_u16_high(p_request->wIndex);

    // Only current-value requests are supported.
    TU_VERIFY(p_request->bRequest == AUDIO_CS_REQ_CUR);

    // A headset exposes a feature unit per direction; the speaker's id matches the
    // single-direction USB_AUDIO_ENTITY_FEATURE_UNIT, the mic adds a second one.
    // Mute/volume state is shared across them.
    if (entityID == USB_AUDIO_ENTITY_FEATURE_UNIT ||
        entityID == USB_AUDIO_HS_ENTITY_MIC_FEATURE_UNIT) {
        if (channelNum > USB_AUDIO_MAX_CHANNELS) {
            return false;
        }
        switch (ctrlSel) {
            case AUDIO_FU_CTRL_MUTE:
                TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_1_t));
                usb_audio_mute[channelNum] = ((audio_control_cur_1_t *)pBuff)->bCur;
                return true;

            case AUDIO_FU_CTRL_VOLUME:
                TU_VERIFY(p_request->wLength == sizeof(audio_control_cur_2_t));
                usb_audio_volume[channelNum] = ((audio_control_cur_2_t *)pBuff)->bCur;
                return true;

            default:
                return false;
        }
    }
    return false;
}

// Class-specific GET requests for an entity (clock source, feature unit, input terminal).
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request) {
    (void)rhport;

    uint8_t const channelNum = (uint8_t)tu_u16_low(p_request->wValue);
    uint8_t const ctrlSel = (uint8_t)tu_u16_high(p_request->wValue);
    uint8_t const entityID = (uint8_t)tu_u16_high(p_request->wIndex);

    // Input terminal connector control. The single-mic descriptor uses
    // USB_AUDIO_ENTITY_INPUT_TERMINAL; the headset's microphone input terminal is
    // a distinct id. (The USB-streaming input terminals don't advertise a readable
    // connector control, so the host won't query them here.)
    if (entityID == USB_AUDIO_ENTITY_INPUT_TERMINAL ||
        entityID == USB_AUDIO_HS_ENTITY_MIC_INPUT_TERMINAL) {
        switch (ctrlSel) {
            case AUDIO_TE_CTRL_CONNECTOR: {
                audio_desc_channel_cluster_t ret;
                ret.bNrChannels = usb_audio_channel_count;
                ret.bmChannelConfig = (audio_channel_config_t)0;
                ret.iChannelNames = 0;
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &ret, sizeof(ret));
            }
            default:
                return false;
        }
    }

    // Feature unit (mute/volume) for either the speaker or mic chain.
    if (entityID == USB_AUDIO_ENTITY_FEATURE_UNIT ||
        entityID == USB_AUDIO_HS_ENTITY_MIC_FEATURE_UNIT) {
        if (channelNum > USB_AUDIO_MAX_CHANNELS) {
            return false;
        }
        switch (ctrlSel) {
            case AUDIO_FU_CTRL_MUTE:
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &usb_audio_mute[channelNum], sizeof(usb_audio_mute[channelNum]));

            case AUDIO_FU_CTRL_VOLUME:
                switch (p_request->bRequest) {
                    case AUDIO_CS_REQ_CUR:
                        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &usb_audio_volume[channelNum], sizeof(usb_audio_volume[channelNum]));

                    case AUDIO_CS_REQ_RANGE: {
                        audio_control_range_2_n_t(1) ret;
                        ret.wNumSubRanges = 1;
                        ret.subrange[0].bMin = -90 * 256;  // -90 dB
                        ret.subrange[0].bMax = 90 * 256;   // +90 dB
                        ret.subrange[0].bRes = 256;        // 1 dB steps
                        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &ret, sizeof(ret));
                    }

                    default:
                        return false;
                }

            default:
                return false;
        }
    }

    // Clock source (sample rate set in usb_audio.enable()).
    if (entityID == USB_AUDIO_ENTITY_CLOCK_SOURCE) {
        switch (ctrlSel) {
            case AUDIO_CS_CTRL_SAM_FREQ:
                switch (p_request->bRequest) {
                    case AUDIO_CS_REQ_CUR: {
                        audio_control_cur_4_t cur = { .bCur = (int32_t)usb_audio_sample_rate };
                        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &cur, sizeof(cur));
                    }

                    case AUDIO_CS_REQ_RANGE: {
                        audio_control_range_4_n_t(1) ret;
                        ret.wNumSubRanges = 1;
                        ret.subrange[0].bMin = (int32_t)usb_audio_sample_rate;
                        ret.subrange[0].bMax = (int32_t)usb_audio_sample_rate;
                        ret.subrange[0].bRes = 0;
                        return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &ret, sizeof(ret));
                    }

                    default:
                        return false;
                }

            case AUDIO_CS_CTRL_CLK_VALID: {
                audio_control_cur_1_t cur = { .bCur = 1 };
                return tud_audio_buffer_and_schedule_control_xfer(rhport, p_request, &cur, sizeof(cur));
            }

            default:
                return false;
        }
    }

    return false;
}

// Keep the per-VM singleton instances (and, for the microphone, its bound
// audiosample) alive for the GC. Installed in the module globals by
// usb_audio_setup_singletons() and reset to NULL at each VM start.
MP_REGISTER_ROOT_POINTER(mp_obj_t usb_audio_microphone_singleton);
MP_REGISTER_ROOT_POINTER(mp_obj_t usb_audio_speaker_singleton);
