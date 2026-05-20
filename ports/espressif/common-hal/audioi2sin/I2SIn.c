// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "bindings/espidf/__init__.h"

#include "common-hal/audioi2sin/I2SIn.h"
#include "py/runtime.h"
#include "shared-bindings/audioi2sin/I2SIn.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "driver/i2s_std.h"

#if CIRCUITPY_AUDIOI2SIN

void common_hal_audioi2sin_i2sin_construct(audioi2sin_i2sin_obj_t *self,
    const mcu_pin_obj_t *bit_clock, const mcu_pin_obj_t *word_select,
    const mcu_pin_obj_t *data, const mcu_pin_obj_t *main_clock,
    uint32_t sample_rate, uint8_t bit_depth, uint8_t output_bit_depth,
    bool mono, bool left_justified, bool samples_signed) {

    i2s_data_bit_width_t bit_width = (i2s_data_bit_width_t)bit_depth;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    esp_err_t err = i2s_new_channel(&chan_cfg, NULL, &self->rx_chan);
    if (err == ESP_ERR_NOT_FOUND) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Peripheral in use"));
    }
    CHECK_ESP_RESULT(err);

    // Always configure the bus as stereo. The newer-family I2S peripherals
    // (S2/S3/C-series) ignore I2S_SLOT_MODE_MONO on RX and write both slots
    // into the DMA buffer regardless, which yields buffers that fill at 2x
    // the WS rate and produces half-speed audio. By configuring stereo and
    // dropping one slot ourselves in record_to_buffer, behavior is uniform
    // across chips.
    i2s_std_slot_config_t slot_cfg = left_justified
        ? (i2s_std_slot_config_t)I2S_STD_MSB_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO)
        : (i2s_std_slot_config_t)I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bit_width, I2S_SLOT_MODE_STEREO);

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate),
        .slot_cfg = slot_cfg,
        .gpio_cfg = {
            .mclk = main_clock != NULL ? main_clock->number : I2S_GPIO_UNUSED,
            .bclk = bit_clock->number,
            .ws = word_select->number,
            .dout = I2S_GPIO_UNUSED,
            .din = data->number,
        },
    };
    CHECK_ESP_RESULT(i2s_channel_init_std_mode(self->rx_chan, &std_cfg));
    CHECK_ESP_RESULT(i2s_channel_enable(self->rx_chan));

    self->bit_clock = bit_clock;
    self->word_select = word_select;
    self->data = data;
    self->mclk = main_clock;
    self->sample_rate = sample_rate;
    self->bit_depth = bit_depth;
    self->output_bit_depth = output_bit_depth;
    self->mono = mono;
    self->samples_signed = samples_signed;

    claim_pin(bit_clock);
    claim_pin(word_select);
    claim_pin(data);
    if (main_clock) {
        claim_pin(main_clock);
    }
}

bool common_hal_audioi2sin_i2sin_deinited(audioi2sin_i2sin_obj_t *self) {
    return self->data == NULL;
}

void common_hal_audioi2sin_i2sin_deinit(audioi2sin_i2sin_obj_t *self) {
    if (common_hal_audioi2sin_i2sin_deinited(self)) {
        return;
    }

    if (self->rx_chan) {
        i2s_channel_disable(self->rx_chan);
        i2s_del_channel(self->rx_chan);
        self->rx_chan = NULL;
    }

    if (self->bit_clock) {
        reset_pin_number(self->bit_clock->number);
    }
    self->bit_clock = NULL;

    if (self->word_select) {
        reset_pin_number(self->word_select->number);
    }
    self->word_select = NULL;

    if (self->data) {
        reset_pin_number(self->data->number);
    }
    self->data = NULL;

    if (self->mclk) {
        reset_pin_number(self->mclk->number);
    }
    self->mclk = NULL;
}

// Sign-extend a raw I2S sample (in the low `in_depth` bits of `raw`) to a
// canonical int32 value.
static inline int32_t i2sin_normalize_signed(uint32_t raw, uint8_t in_depth) {
    if (in_depth == 32) {
        return (int32_t)raw;
    }
    if (in_depth == 24) {
        uint32_t sign_bit = 0x800000u;
        return (int32_t)((raw ^ sign_bit) - sign_bit);
    }
    if (in_depth == 16) {
        return (int16_t)(raw & 0xffffu);
    }
    return (int8_t)(raw & 0xffu);
}

// Read a single sample from the DMA scratch at the given byte offset for the
// configured input bit depth.
static inline uint32_t i2sin_read_raw(const uint8_t *src, uint8_t in_depth) {
    if (in_depth == 8) {
        return (uint32_t)(*src);
    }
    if (in_depth == 16) {
        uint16_t v;
        memcpy(&v, src, sizeof(v));
        return v;
    }
    uint32_t v;
    memcpy(&v, src, sizeof(v));
    return v;
}

// Convert `raw` from `in_depth` to `out_depth` (shift-only semantics, sign-
// preserving for signed) and write it to `buffer` at sample index `idx`.
// Output element size: 1 byte at 8, 2 bytes at 16, 4 bytes at 24 or 32.
static inline void i2sin_write_converted(void *buffer, uint32_t idx,
    uint32_t raw, uint8_t in_depth, uint8_t out_depth, bool samples_signed) {
    int32_t s = i2sin_normalize_signed(raw, in_depth);
    int32_t shifted;
    if (out_depth >= in_depth) {
        // Bit-replicate the input across the wider output so that full-scale
        // input maps to full-scale output (e.g. 8-bit 0xFF -> 16-bit 0xFFFF),
        // rather than leaving the new low bits as zero.
        uint32_t in_mask = (in_depth >= 32) ? 0xffffffffu : ((1u << in_depth) - 1u);
        uint32_t in_bits = (uint32_t)s & in_mask;
        uint32_t result = 0;
        int remaining = out_depth;
        while (remaining > 0) {
            int take = (remaining >= (int)in_depth) ? (int)in_depth : remaining;
            result = (result << take) | (in_bits >> (in_depth - take));
            remaining -= take;
        }
        shifted = (int32_t)result;
    } else {
        shifted = s >> (in_depth - out_depth);
    }
    uint32_t u = (uint32_t)shifted;
    if (!samples_signed) {
        if (out_depth >= 32) {
            u ^= 0x80000000u;
        } else {
            uint32_t mask = (1u << out_depth) - 1u;
            u = (u & mask) ^ (1u << (out_depth - 1));
        }
    }
    switch (out_depth) {
        case 8:
            ((uint8_t *)buffer)[idx] = (uint8_t)(u & 0xffu);
            break;
        case 16:
            ((uint16_t *)buffer)[idx] = (uint16_t)(u & 0xffffu);
            break;
        default: // 24 or 32
            ((uint32_t *)buffer)[idx] = u;
            break;
    }
}

// I2S delivers signed PCM. When samples_signed is false, XOR each sample with
// the sign bit for its width to convert to unsigned PCM (WAV convention).
static void i2sin_convert_to_unsigned(void *buffer, uint32_t samples,
    uint8_t bit_depth, size_t element_size) {
    (void)element_size;
    uint32_t *p = (uint32_t *)buffer;

    if (bit_depth == 8) {
        // 4 samples per word
        uint32_t words = samples / 4;
        for (uint32_t i = 0; i < words; i++) {
            p[i] ^= 0x80808080u;
        }
        // tail: 0–3 leftover bytes
        uint8_t *tail = (uint8_t *)(p + words);
        for (uint32_t i = 0; i < (samples & 3u); i++) {
            tail[i] ^= 0x80u;
        }
    } else if (bit_depth == 16) {
        // 2 samples per word
        uint32_t words = samples / 2;
        for (uint32_t i = 0; i < words; i++) {
            p[i] ^= 0x80008000u;
        }
        if (samples & 1u) {
            ((uint16_t *)(p + words))[0] ^= 0x8000u;
        }
    } else {
        // 24- or 32-bit: one sample per 32-bit slot
        uint32_t mask = (bit_depth == 24) ? 0x00800000u : 0x80000000u;
        for (uint32_t i = 0; i < samples; i++) {
            p[i] ^= mask;
        }
    }
}


uint32_t common_hal_audioi2sin_i2sin_record_to_buffer(audioi2sin_i2sin_obj_t *self,
    void *buffer, uint32_t length) {
    size_t element_size = self->bit_depth / 8;
    // 24-bit samples occupy a 32-bit slot on the I2S bus.
    if (self->bit_depth == 24) {
        element_size = 4;
    }

    if (self->output_bit_depth != self->bit_depth) {
        // Bit-depth conversion path: always read at input width into scratch,
        // convert each sample into the user's buffer at output width.
        const uint8_t in_depth = self->bit_depth;
        const uint8_t out_depth = self->output_bit_depth;
        const bool samples_signed = self->samples_signed;
        uint8_t scratch[256];
        const size_t in_frame_bytes = 2 * element_size;
        const size_t scratch_frames = sizeof(scratch) / in_frame_bytes;
        uint32_t produced = 0;
        while (produced < length) {
            size_t want_frames;
            if (self->mono) {
                want_frames = length - produced;
            } else {
                want_frames = (length - produced + 1) / 2;
            }
            if (want_frames > scratch_frames) {
                want_frames = scratch_frames;
            }
            size_t got_bytes = 0;
            esp_err_t err = i2s_channel_read(self->rx_chan, scratch,
                want_frames * in_frame_bytes, &got_bytes, portMAX_DELAY);
            CHECK_ESP_RESULT(err);
            size_t got_frames = got_bytes / in_frame_bytes;
            for (size_t i = 0; i < got_frames && produced < length; i++) {
                const uint8_t *frame = scratch + i * in_frame_bytes;
                uint32_t left_raw = i2sin_read_raw(frame, in_depth);
                i2sin_write_converted(buffer, produced++, left_raw,
                    in_depth, out_depth, samples_signed);
                if (!self->mono && produced < length) {
                    uint32_t right_raw = i2sin_read_raw(frame + element_size, in_depth);
                    i2sin_write_converted(buffer, produced++, right_raw,
                        in_depth, out_depth, samples_signed);
                }
            }
            if (got_frames < want_frames) {
                break;
            }
        }
        return produced;
    }

    uint32_t produced;
    if (!self->mono) {
        size_t result = 0;
        esp_err_t err = i2s_channel_read(self->rx_chan, buffer, length * element_size,
            &result, portMAX_DELAY);
        CHECK_ESP_RESULT(err);
        produced = result / element_size;
    } else {
        // Mono: bus is configured stereo, so each WS frame yields two slots in
        // the DMA buffer. Read in chunks into a scratch and keep only the left
        // slot of each frame.
        uint8_t scratch[256];
        const size_t frame_bytes = 2 * element_size;
        const size_t scratch_frames = sizeof(scratch) / frame_bytes;
        uint8_t *out = (uint8_t *)buffer;
        produced = 0;
        while (produced < length) {
            size_t want_frames = length - produced;
            if (want_frames > scratch_frames) {
                want_frames = scratch_frames;
            }
            size_t got_bytes = 0;
            esp_err_t err = i2s_channel_read(self->rx_chan, scratch,
                want_frames * frame_bytes, &got_bytes, portMAX_DELAY);
            CHECK_ESP_RESULT(err);
            size_t got_frames = got_bytes / frame_bytes;
            for (size_t i = 0; i < got_frames; i++) {
                memcpy(out + produced * element_size,
                    scratch + i * frame_bytes,
                    element_size);
                produced++;
            }
            if (got_frames < want_frames) {
                break;
            }
        }
    }

    if (!self->samples_signed && produced > 0) {
        i2sin_convert_to_unsigned(buffer, produced, self->bit_depth, element_size);
    }
    return produced;
}

uint8_t common_hal_audioi2sin_i2sin_get_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->bit_depth;
}

uint8_t common_hal_audioi2sin_i2sin_get_output_bit_depth(audioi2sin_i2sin_obj_t *self) {
    return self->output_bit_depth;
}

uint32_t common_hal_audioi2sin_i2sin_get_sample_rate(audioi2sin_i2sin_obj_t *self) {
    return self->sample_rate;
}

bool common_hal_audioi2sin_i2sin_get_samples_signed(audioi2sin_i2sin_obj_t *self) {
    return self->samples_signed;
}

#endif // CIRCUITPY_AUDIOI2SIN
