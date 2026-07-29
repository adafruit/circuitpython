// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/GranularPitchShift.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>
#include "py/runtime.h"
#include <math.h>

void common_hal_audiodelays_granular_pitch_shift_construct(audiodelays_granular_pitch_shift_obj_t *self,
    mp_obj_t semitones, mp_obj_t mix, uint32_t grain_size, uint32_t density,
    mp_float_t spread, uint32_t buffer_size, uint8_t bits_per_sample, bool samples_signed,
    uint8_t channel_count, uint32_t sample_rate) {

    // Basic settings every effect and audio sample has
    // These are the effect's values, not the source sample(s)
    self->base.bits_per_sample = bits_per_sample; // Most common is 16, but 8 is also supported in many places
    self->base.samples_signed = samples_signed; // Are the samples we provide signed
    self->base.channel_count = channel_count; // Channels can be 1 for mono or 2 for stereo
    self->base.sample_rate = sample_rate; // Sample rate for the effect, this generally needs to match all audio objects
    self->base.single_buffer = false;
    self->base.max_buffer_length = buffer_size;

    // To smooth things out as CircuitPython is doing other tasks most audio objects have a buffer
    // A double buffer is set up here so the audio output can use DMA on buffer 1 while we
    // write to and create buffer 2.
    // This buffer is what is passed to the audio component that plays the effect.
    // Samples are set sequentially. For stereo audio they are passed L/R/L/R/...
    self->buffer_len = buffer_size; // in bytes

    self->buffer[0] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[0] == NULL) {
        common_hal_audiodelays_granular_pitch_shift_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_granular_pitch_shift_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[1], 0, self->buffer_len);

    self->last_buf_idx = 1; // Which buffer to use first, toggle between 0 and 1

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the effect's starting values.

    synthio_block_assign_slot(semitones, &self->semitones, MP_QSTR_semitones);
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    // Grain scheduling parameters. `density` (number of overlapping grains) is
    // clamped to the fixed grain pool size so allocation stays deterministic.
    self->grain_size = grain_size;
    if (density < 1) {
        density = 1;
    }
    if (density > GRANULAR_MAX_GRAINS) {
        density = GRANULAR_MAX_GRAINS;
    }
    self->density = density;

    // Grain-start jitter amount and the PRNG that drives it. Seeded with a fixed
    // nonzero constant so xorshift32 never degenerates to the all-zero state and
    // the jitter sequence is reproducible from run to run.
    common_hal_audiodelays_granular_pitch_shift_set_spread(self, spread);
    self->rng_state = 0x1234abcdu;

    // Normalization for the overlap-add gain of the grain envelopes. A Hann
    // window overlapped at hop = grain_size / density sums to a constant gain of
    // density/2, so we scale the summed grains by 2/density in Q15 to keep the
    // output at ~unity. Cap at 1.0 so the degenerate density==1 case (a single
    // windowed grain, no overlap partner) is never amplified.
    self->grain_gain = (1 << 15) * 2 / density; // 2/density in Q15
    if (self->grain_gain > (1 << 15)) {
        self->grain_gain = (1 << 15);
    }

    // Capture buffer (the delay line grains read from), stored as 16-bit,
    // planar per channel. Length is grain_size * 2 words per channel so a grain
    // starting grain_size words behind the write pointer never overruns the live
    // write cursor even when reading ahead at a raised pitch
    self->capture_len = self->grain_size * 2; // words per channel
    uint32_t capture_bytes = self->capture_len * self->base.channel_count * sizeof(int16_t);
    self->capture_buffer = m_malloc_without_collect(capture_bytes);
    if (self->capture_buffer == NULL) {
        common_hal_audiodelays_granular_pitch_shift_deinit(self);
        m_malloc_fail(capture_bytes);
    }
    memset(self->capture_buffer, 0, capture_bytes);
    self->write_index = 0;

    // Precompute the grain amplitude envelope: a raised-cosine (Hann) window in
    // Q15 (0..32767), indexed directly by a grain's phase.
    self->envelope_len = self->grain_size;
    uint32_t envelope_bytes = self->envelope_len * sizeof(int16_t);
    self->envelope_table = m_malloc_without_collect(envelope_bytes);
    if (self->envelope_table == NULL) {
        common_hal_audiodelays_granular_pitch_shift_deinit(self);
        m_malloc_fail(envelope_bytes);
    }
    mp_float_t denom = (self->envelope_len > 1) ? (mp_float_t)(self->envelope_len - 1) : MICROPY_FLOAT_CONST(1.0);
    for (uint32_t n = 0; n < self->envelope_len; n++) {
        mp_float_t w = MICROPY_FLOAT_CONST(0.5) *
            (MICROPY_FLOAT_CONST(1.0) - MICROPY_FLOAT_C_FUN(cos)(
            MICROPY_FLOAT_CONST(2.0) * MICROPY_FLOAT_CONST(3.14159265358979323846) * (mp_float_t)n / denom));
        self->envelope_table[n] = (int16_t)(w * MICROPY_FLOAT_CONST(32767.0));
    }

    // Deactivate all grains and prime the scheduler so the first grain launches
    // immediately on the first output sample.
    for (uint32_t i = 0; i < GRANULAR_MAX_GRAINS; i++) {
        self->grains[i].active = false;
    }
    self->samples_until_next_grain = 0;

    // Calculate the fixed-point read-rate increment applied to launched grains.
    mp_float_t f_semitones = synthio_block_slot_get(&self->semitones);
    granular_pitch_shift_recalculate_rate(self, f_semitones);
}

void common_hal_audiodelays_granular_pitch_shift_deinit(audiodelays_granular_pitch_shift_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->envelope_table = NULL;
    self->capture_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiodelays_granular_pitch_shift_get_semitones(audiodelays_granular_pitch_shift_obj_t *self) {
    return self->semitones.obj;
}

void common_hal_audiodelays_granular_pitch_shift_set_semitones(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t semitones_in) {
    synthio_block_assign_slot(semitones_in, &self->semitones, MP_QSTR_semitones);
    mp_float_t semitones = synthio_block_slot_get(&self->semitones);
    granular_pitch_shift_recalculate_rate(self, semitones);
}

void granular_pitch_shift_recalculate_rate(audiodelays_granular_pitch_shift_obj_t *self, mp_float_t semitones) {
    self->read_rate = (uint32_t)(MICROPY_FLOAT_C_FUN(pow)(2.0, semitones / MICROPY_FLOAT_CONST(12.0)) * (1 << GRANULAR_PITCH_READ_SHIFT));
    self->current_semitones = semitones;
}

mp_obj_t common_hal_audiodelays_granular_pitch_shift_get_mix(audiodelays_granular_pitch_shift_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_granular_pitch_shift_set_mix(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

mp_float_t common_hal_audiodelays_granular_pitch_shift_get_spread(audiodelays_granular_pitch_shift_obj_t *self) {
    return self->spread;
}

void common_hal_audiodelays_granular_pitch_shift_set_spread(audiodelays_granular_pitch_shift_obj_t *self, mp_float_t spread) {
    if (spread < MICROPY_FLOAT_CONST(0.0)) {
        spread = MICROPY_FLOAT_CONST(0.0);
    } else if (spread > MICROPY_FLOAT_CONST(1.0)) {
        spread = MICROPY_FLOAT_CONST(1.0);
    }
    self->spread = spread;
}

void audiodelays_granular_pitch_shift_reset_buffer(audiodelays_granular_pitch_shift_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->capture_buffer, 0, self->capture_len * self->base.channel_count * sizeof(int16_t));

    // Deactivate all grains and reset the scheduler/write cursor.
    for (uint32_t i = 0; i < GRANULAR_MAX_GRAINS; i++) {
        self->grains[i].active = false;
    }
    self->samples_until_next_grain = 0;
    self->write_index = 0;
}

bool common_hal_audiodelays_granular_pitch_shift_get_playing(audiodelays_granular_pitch_shift_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_granular_pitch_shift_play(audiodelays_granular_pitch_shift_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_must_match(&self->base, sample, false);

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;

    return;
}

void common_hal_audiodelays_granular_pitch_shift_stop(audiodelays_granular_pitch_shift_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    self->sample = NULL;
    return;
}

// xorshift32 PRNG for grain-start jitter. Kept local to this module so the
// effect doesn't depend on the `random` module being enabled in a build.
static uint32_t granular_pitch_shift_rand(audiodelays_granular_pitch_shift_obj_t *self) {
    uint32_t x = self->rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    self->rng_state = x;
    return x;
}

// Launch a grain into the first free pool slot, seeded to read from the capture
// buffer starting grain_size words behind the current write cursor (so it reads
// already-captured audio) at the current pitch-shift read rate. When `spread` is
// nonzero the start is jittered up to a further grain_size words backward, which
// is what gives granular synthesis its characteristic "cloud" texture. Jitter is
// always backward (further behind the write cursor), so a grain never reads ahead
// of captured audio and the capture buffer (capture_len == grain_size * 2) is
// never overrun. Grain read state (read_index/phase) is per-frame /
// channel-independent; the per-channel plane offset is applied at read time.
static void granular_pitch_shift_launch_grain(audiodelays_granular_pitch_shift_obj_t *self) {
    for (uint32_t g = 0; g < GRANULAR_MAX_GRAINS; g++) {
        if (!self->grains[g].active) {
            // Backward jitter in [0, spread * grain_size]. Capped at grain_size
            // so grain_size + jitter <= capture_len (== grain_size * 2).
            uint32_t jitter = 0;
            if (self->spread > MICROPY_FLOAT_CONST(0.0)) {
                uint32_t max_jitter = (uint32_t)(self->spread * (mp_float_t)self->grain_size);
                if (max_jitter > 0) {
                    jitter = granular_pitch_shift_rand(self) % (max_jitter + 1);
                }
            }
            uint32_t start = (self->write_index + self->capture_len - self->grain_size - jitter) % self->capture_len;
            self->grains[g].active = true;
            self->grains[g].read_index = start << GRANULAR_PITCH_READ_SHIFT;
            self->grains[g].read_rate = self->read_rate;
            self->grains[g].phase = 0;
            self->grains[g].length = self->grain_size;
            return;
        }
    }
}

audioio_get_buffer_result_t audiodelays_granular_pitch_shift_get_buffer(audiodelays_granular_pitch_shift_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    // Grain scheduler + enveloped grain playback with dry/wet `mix` blending.
    // Grains are launched at a fixed spacing (grain_size / density) and each
    // active grain reads the capture buffer at its own pitch-shifted (fractional,
    // linearly interpolated) read rate, scaled by a Hann amplitude envelope so
    // overlapping grains fade in/out and add without seam clicks. The summed
    // grains are normalized by grain_gain for ~unity overlap-add gain, then
    // crossfaded against the dry input via the `mix` slot. The 8/16-bit,
    // signed/unsigned, and mono/stereo (planar capture, buf_offset) paths are all
    // handled below, mirroring PitchShift.

    if (!single_channel_output) {
        channel = 0;
    }

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    // The capture buffer (delay line) is always stored as 16-bit, planar per
    // channel: channel c occupies capture_buffer[c * capture_len .. ).
    int16_t *capture_buffer = self->capture_buffer;

    // Grains are launched this many frames apart (overlap factor `density`).
    uint32_t grain_spacing = self->grain_size / self->density;
    if (grain_spacing == 0) {
        grain_spacing = 1;
    }

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                if (result == GET_BUFFER_ERROR) {
                    // The sample cannot be read from any more, it was
                    // deinited while we were playing it.
                    self->sample = NULL;
                    self->sample_buffer_length = 0;
                    self->more_data = false;
                } else {
                    // Track length in terms of words.
                    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
                    self->more_data = result == GET_BUFFER_MORE_DATA;
                }
            }
        }

        if (self->sample == NULL) {
            if (self->base.samples_signed) {
                memset(word_buffer, 0, length * (self->base.bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    memset(word_buffer, 32768, length * (self->base.bits_per_sample / 8));
                } else {
                    memset(hword_buffer, 128, length * (self->base.bits_per_sample / 8));
                }
            }

            // tick all block inputs
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, length / self->base.channel_count);
            (void)synthio_block_slot_get(&self->semitones);
            (void)synthio_block_slot_get(&self->mix);

            length = 0;
        } else {
            // we have a sample to play and apply effect
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t n = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * self->base.channel_count);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            // get the effect values we need from the BlockInput.
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, n / self->base.channel_count);
            mp_float_t semitones = synthio_block_slot_get(&self->semitones);
            // Doubled (0.0..2.0) so the crossfade below can hold both dry and wet
            // at full gain around the midpoint, matching PitchShift's mix curve.
            mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0)) * MICROPY_FLOAT_CONST(2.0);

            // Only recalculate rate if semitones has changed
            if (memcmp(&semitones, &self->current_semitones, sizeof(mp_float_t))) {
                granular_pitch_shift_recalculate_rate(self, semitones);
            }

            for (uint32_t i = 0; i < n; i++) {
                bool buf_offset = (channel == 1 || i % self->base.channel_count == 1);

                int32_t sample_word = 0;
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    sample_word = sample_src[i];
                } else {
                    if (self->base.samples_signed) {
                        sample_word = sample_hsrc[i];
                    } else {
                        // Changing from an 8 bit unsigned to signed into a 32-bit signed
                        sample_word = (int8_t)(((uint8_t)sample_hsrc[i]) ^ 0x80);
                    }
                }

                // Write the incoming sample into the capture buffer (the delay
                // line grains read from) at the per-channel write cursor.
                capture_buffer[self->write_index + self->capture_len * buf_offset] = (int16_t)sample_word;

                // Sum all active grains. Each grain reads the capture buffer at
                // its own fractional read cursor (linearly interpolated between
                // adjacent words), which is what produces the pitch shift, then
                // is scaled by its amplitude envelope (Hann, indexed by the
                // grain's phase) so grains fade in/out and overlap-add without
                // seam clicks.
                int32_t word = 0;
                for (uint32_t g = 0; g < GRANULAR_MAX_GRAINS; g++) {
                    if (!self->grains[g].active) {
                        continue;
                    }
                    uint32_t read_index_fp = self->grains[g].read_index;
                    uint32_t ipart = read_index_fp >> GRANULAR_PITCH_READ_SHIFT;
                    uint32_t frac = read_index_fp & ((1 << GRANULAR_PITCH_READ_SHIFT) - 1);
                    uint32_t index_lo = ipart % self->capture_len;
                    uint32_t index_hi = (ipart + 1) % self->capture_len;
                    int32_t sample_lo = capture_buffer[index_lo + self->capture_len * buf_offset];
                    int32_t sample_hi = capture_buffer[index_hi + self->capture_len * buf_offset];
                    int32_t grain_out = sample_lo + (((sample_hi - sample_lo) * (int32_t)frac) >> GRANULAR_PITCH_READ_SHIFT);
                    // Apply the grain envelope (Q15). phase < length == envelope_len.
                    int32_t env = self->envelope_table[self->grains[g].phase];
                    word += (grain_out * env) >> 15;
                }

                // Normalize the overlap-add gain of the enveloped grains back to
                // ~unity (int64 guards the Q15 multiply against overflow). This is
                // the fully-wet (pitch-shifted) sample.
                word = (int32_t)(((int64_t)word * (int64_t)self->grain_gain) >> 15);

                // Dry/wet crossfade: `mix` is doubled to 0.0..2.0 so both terms
                // sit at full gain around the midpoint, then synthio_mix_down_sample
                // scales/soft-clips the summed pair back into range (same curve as
                // PitchShift). mix=0.0 -> dry input, mix=1.0 -> fully wet.
                word = (int32_t)((sample_word * MIN(MICROPY_FLOAT_CONST(2.0) - mix, MICROPY_FLOAT_CONST(1.0))) + (word * MIN(mix, MICROPY_FLOAT_CONST(1.0))));
                word = synthio_mix_down_sample(word, SYNTHIO_MIX_DOWN_SCALE(2));

                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    word_buffer[i] = (int16_t)word;
                    if (!self->base.samples_signed) {
                        word_buffer[i] ^= 0x8000;
                    }
                } else {
                    int8_t mixed = (int8_t)word;
                    if (self->base.samples_signed) {
                        hword_buffer[i] = mixed;
                    } else {
                        hword_buffer[i] = (uint8_t)mixed ^ 0x80;
                    }
                }

                // Per-frame shared work (once per frame: mono every sample,
                // interleaved stereo on the right-channel sample) so both
                // channels read consistent grain state before it advances.
                if (self->base.channel_count == 1 || buf_offset) {
                    // Advance and retire active grains (they were read above).
                    for (uint32_t g = 0; g < GRANULAR_MAX_GRAINS; g++) {
                        if (!self->grains[g].active) {
                            continue;
                        }
                        self->grains[g].read_index += self->grains[g].read_rate;
                        self->grains[g].phase++;
                        if (self->grains[g].phase >= self->grains[g].length) {
                            self->grains[g].active = false;
                        }
                    }

                    // Launch a new grain when the scheduler counter elapses.
                    if (self->samples_until_next_grain == 0) {
                        granular_pitch_shift_launch_grain(self);
                        self->samples_until_next_grain = grain_spacing;
                    } else {
                        self->samples_until_next_grain--;
                    }

                    // Advance the per-channel write cursor.
                    self->write_index++;
                    if (self->write_index >= self->capture_len) {
                        self->write_index = 0;
                    }
                }
            }

            // Update the remaining length and the buffer positions based on how much we wrote into our buffer
            length -= n;
            word_buffer += n;
            hword_buffer += n;
            self->sample_remaining_buffer += (n * (self->base.bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    return GET_BUFFER_MORE_DATA;
}
