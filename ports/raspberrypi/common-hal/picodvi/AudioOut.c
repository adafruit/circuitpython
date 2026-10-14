// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

// 48 kHz stereo DVI audio for the RP2350 picodvi Framebuffer.
// No interrupt-time allocation, sample conversion, packet encoding, or Python callbacks.

#include <string.h>

#include "py/runtime.h"

#include "bindings/picodvi/AudioOut.h"
#include "common-hal/picodvi/dvi_audio_packet.h"
#include "hardware/clocks.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "shared-module/audiocore/__init__.h"
#include "supervisor/background_callback.h"
#include "supervisor/port.h"

#define DVI_AUDIO_RATE 48000u
#define DVI_AUDIO_BANKS 2
#define DVI_AUDIO_H_TOTAL (MODE_640_H_FRONT_PORCH + MODE_640_H_SYNC_WIDTH + MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS)
// The longest line dvi_audio_build_line() writes: an active line.
#define DVI_AUDIO_LINE_WORDS (7 + HSTX_DATA_ISLAND_WORDS + 4 + 9)

typedef uint32_t dvi_audio_line_t[DVI_AUDIO_LINE_WORDS];

typedef struct {
    uint32_t *commands;
    dvi_audio_line_t *lines;
    volatile uint32_t samples;
    volatile uint32_t order;
    volatile bool ready;
} dvi_audio_bank_t;

typedef struct dvi_audio_state {
    // Plain video command list plus audio headers, silence and active-line
    // preambles. Banks start as copies of it, and it is shown when no bank is
    // ready.
    uint32_t *base_commands;
    dvi_audio_bank_t bank[DVI_AUDIO_BANKS];
    volatile int active;
    // Set to go back to video only at the next frame, then acknowledged by
    // the frame interrupt once nothing reads this state any more.
    volatile bool detach;
    volatile bool detached;
    uint32_t next_order;
    uint32_t pixel_clock;
    uint32_t max_packets;
    uint64_t phase;
    // ACR timing, carried from frame to frame so the average is exact.
    uint64_t acr_phase;
    int channel_frame;
    // Fixed lines the command lists point at: ACR, audio InfoFrame and AVI
    // InfoFrame on lines 0-2, then ACR and silence on a blanking line [0] and
    // an active line [1].
    dvi_audio_line_t header_line[3];
    uint32_t header_length[3];
    dvi_audio_line_t acr_line[2];
    uint32_t acr_length[2];
    // One bit per line that carries an ACR packet in the base list.
    uint32_t acr_lines[(MODE_640_V_TOTAL_LINES + 31) / 32];
    dvi_audio_line_t silence_line[2];
    uint32_t silence_length[2];
} dvi_audio_state_t;

// Static so it can never be freed while it is queued.
static background_callback_t dvi_audio_refill_callback;

// Active line with the video preamble and guard band that audio mode needs.
static uint32_t dvi_audio_vactive_line640[] = {
    HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_640_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH - 10),
    SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | 8,
    TMDS_CTRL_11 | (TMDS_CTRL_01 << 10) | (TMDS_CTRL_00 << 20),
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | 2,
    0x2ccu | (0x133u << 10) | (0x2ccu << 20),
    HSTX_CMD_TMDS | MODE_640_H_ACTIVE_PIXELS
};

static uint32_t dvi_audio_build_line(uint32_t *p, const hstx_data_island_t *island, bool vsync, bool active) {
    uint32_t *start = p;
    uint32_t h0 = vsync ? SYNC_V0_H0 : SYNC_V1_H0;
    uint32_t h1 = vsync ? SYNC_V0_H1 : SYNC_V1_H1;
    uint32_t preamble = (h0 & 0x3ff) | (TMDS_CTRL_01 << 10) | (TMDS_CTRL_01 << 20);
    *p++ = HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH;
    *p++ = h1;
    *p++ = HSTX_CMD_NOP;
    *p++ = HSTX_CMD_RAW_REPEAT | 8;
    *p++ = preamble;
    *p++ = HSTX_CMD_NOP;
    *p++ = HSTX_CMD_RAW | HSTX_DATA_ISLAND_WORDS;
    memcpy(p, island->words, sizeof(island->words));
    p += HSTX_DATA_ISLAND_WORDS;
    *p++ = HSTX_CMD_NOP;
    *p++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_SYNC_WIDTH - 8 - HSTX_DATA_ISLAND_WORDS);
    *p++ = h0;
    *p++ = HSTX_CMD_NOP;
    if (active) {
        *p++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH - 10);
        *p++ = h1;
        *p++ = HSTX_CMD_NOP;
        *p++ = HSTX_CMD_RAW_REPEAT | 8;
        *p++ = (h1 & 0x3ff) | (TMDS_CTRL_01 << 10) | (TMDS_CTRL_00 << 20);
        *p++ = HSTX_CMD_NOP;
        *p++ = HSTX_CMD_RAW_REPEAT | 2;
        *p++ = 0x2ccu | (0x133u << 10) | (0x2ccu << 20);
        *p++ = HSTX_CMD_TMDS | MODE_640_H_ACTIVE_PIXELS;
    } else {
        *p++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS);
        *p++ = h1;
        *p++ = HSTX_CMD_NOP;
    }
    return p - start;
}

// Returns true when an ACR packet is due on this line. ACR packets go out at
// 128 * 48000 / 6144 = 1000 per second, spread over every line.
static bool dvi_audio_acr_due(uint64_t *phase, uint32_t pixel_clock) {
    bool due = *phase >= pixel_clock;
    if (due) {
        *phase -= pixel_clock;
    }
    *phase += (uint64_t)(128 * DVI_AUDIO_RATE / 6144) * DVI_AUDIO_H_TOTAL;
    return due;
}

static void dvi_audio_init_lines(dvi_audio_state_t *audio) {
    hstx_packet_t packet;
    hstx_data_island_t island;
    uint32_t cts = ((uint64_t)audio->pixel_clock * 6144 + (128 * DVI_AUDIO_RATE / 2)) / (128 * DVI_AUDIO_RATE);
    hstx_packet_set_acr(&packet, 6144, cts);
    hstx_encode_data_island(&island, &packet, true, true);
    audio->header_length[0] = dvi_audio_build_line(audio->header_line[0], &island, true, false);
    hstx_encode_data_island(&island, &packet, false, true);
    for (int active = 0; active < 2; active++) {
        audio->acr_length[active] = dvi_audio_build_line(audio->acr_line[active], &island, false, active);
    }
    // The base list, shown only when no bank is ready, has a fixed ACR
    // pattern that starts on line 0.
    uint64_t acr_phase = audio->pixel_clock;
    for (size_t line = 0; line < MODE_640_V_TOTAL_LINES; line++) {
        if (dvi_audio_acr_due(&acr_phase, audio->pixel_clock)) {
            audio->acr_lines[line / 32] |= 1u << (line % 32);
        }
    }
    audio->acr_phase = audio->pixel_clock;
    hstx_packet_set_audio_infoframe(&packet, DVI_AUDIO_RATE, 2, 16);
    hstx_encode_data_island(&island, &packet, true, true);
    audio->header_length[1] = dvi_audio_build_line(audio->header_line[1], &island, true, false);
    // Unspecified VIC: preserve CircuitPython's existing 625-line timing.
    hstx_packet_set_avi_infoframe(&packet, 0, 0);
    hstx_encode_data_island(&island, &packet, false, true);
    audio->header_length[2] = dvi_audio_build_line(audio->header_line[2], &island, false, false);
    // Frame position 4 never sets the block start flag, as in pico_hdmi.
    audio_sample_t silence[4] = {{0}};
    hstx_packet_set_audio_samples(&packet, silence, 4, 4);
    hstx_encode_data_island(&island, &packet, false, true);
    for (int active = 0; active < 2; active++) {
        audio->silence_length[active] = dvi_audio_build_line(audio->silence_line[active], &island, false, active);
    }
}

static bool dvi_audio_acr_line(const dvi_audio_state_t *audio, size_t line) {
    return (audio->acr_lines[line / 32] >> (line % 32)) & 1;
}

// Returns true when a sample packet is due on this line. Reserved lines,
// headers and ACR, never carry sample packets.
static bool dvi_audio_packet_due(uint64_t *phase, uint32_t pixel_clock, bool reserved) {
    *phase += (uint64_t)DVI_AUDIO_RATE * DVI_AUDIO_H_TOTAL;
    if (reserved || *phase < (uint64_t)pixel_clock * 4) {
        return false;
    }
    *phase -= (uint64_t)pixel_clock * 4;
    return true;
}

static void dvi_audio_free_state(dvi_audio_state_t *audio) {
    if (audio->base_commands) {
        port_free(audio->base_commands);
    }
    for (int i = 0; i < DVI_AUDIO_BANKS; i++) {
        if (audio->bank[i].commands) {
            port_free(audio->bank[i].commands);
        }
        if (audio->bank[i].lines) {
            port_free(audio->bank[i].lines);
        }
    }
    port_free(audio);
}

// Frees the audio state. The caller makes sure DMA no longer reads it.
void picodvi_audioout_framebuffer_deinit(picodvi_framebuffer_obj_t *self) {
    // An AudioOut lives as long as its framebuffer.
    if (self->audioout != MP_OBJ_NULL) {
        picodvi_audioout_obj_t *audioout = MP_OBJ_TO_PTR(self->audioout);
        audioout->sample = MP_OBJ_NULL;
        audioout->framebuffer = NULL;
        self->audioout = MP_OBJ_NULL;
    }
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return;
    }
    self->dvi_audio = NULL;
    dvi_audio_free_state(audio);
}

static void dvi_audio_allocate(picodvi_framebuffer_obj_t *self) {
    if (self->dvi_audio) {
        return;
    }
    dvi_audio_state_t *audio = port_malloc(sizeof(*audio), true);
    if (!audio) {
        m_malloc_fail(sizeof(*audio));
    }
    memset(audio, 0, sizeof(*audio));
    audio->active = -1;
    audio->pixel_clock = clock_get_hz(clk_hstx) / 5;
    // Room for the most packets one frame can need at this pixel clock.
    audio->max_packets = (uint64_t)MODE_640_V_TOTAL_LINES * DVI_AUDIO_RATE * DVI_AUDIO_H_TOTAL /
        ((uint64_t)audio->pixel_clock * 4) + 2;
    size_t commands_size = self->dma_commands_len * sizeof(uint32_t);
    size_t lines_size = audio->max_packets * sizeof(dvi_audio_line_t);
    // DMA reads all of these, so they must be in internal RAM.
    audio->base_commands = port_malloc(commands_size, true);
    bool ok = audio->base_commands != NULL;
    for (int i = 0; i < DVI_AUDIO_BANKS && ok; i++) {
        audio->bank[i].commands = port_malloc(commands_size, true);
        audio->bank[i].lines = port_malloc(lines_size, true);
        ok = audio->bank[i].commands != NULL && audio->bank[i].lines != NULL;
    }
    if (!ok) {
        dvi_audio_free_state(audio);
        m_malloc_fail(commands_size + lines_size);
    }
    dvi_audio_init_lines(audio);
    memcpy(audio->base_commands, self->dma_commands, commands_size);
    bool doubled = self->output_width != self->width;
    size_t command_size = doubled ? 4 : 2;
    size_t entry_offset = doubled ? 2 : 0;
    const size_t active_start = MODE_640_V_SYNC_WIDTH + MODE_640_V_BACK_PORCH;
    const size_t active_end = active_start + MODE_640_V_ACTIVE_LINES;
    size_t command_word = 0;
    uint64_t phase = 0;
    for (size_t line = 0; line < MODE_640_V_TOTAL_LINES; line++) {
        bool active = line >= active_start && line < active_end;
        uint32_t *entry = &audio->base_commands[command_word + entry_offset];
        if (dvi_audio_packet_due(&phase, audio->pixel_clock, line < 3 || dvi_audio_acr_line(audio, line))) {
            // Keep the audio link running with silence when no bank is ready.
            entry[0] = audio->silence_length[active];
            entry[1] = (uintptr_t)audio->silence_line[active];
        } else if (line < 3) {
            entry[0] = audio->header_length[line];
            entry[1] = (uintptr_t)audio->header_line[line];
        } else if (dvi_audio_acr_line(audio, line)) {
            entry[0] = audio->acr_length[active];
            entry[1] = (uintptr_t)audio->acr_line[active];
        } else if (active) {
            entry[0] = MP_ARRAY_SIZE(dvi_audio_vactive_line640);
            entry[1] = (uintptr_t)dvi_audio_vactive_line640;
        }
        command_word += command_size * (active ? 2 : 1);
    }
    __dmb();
    self->dvi_audio = audio;
}

static void audioout_refill(picodvi_audioout_obj_t *self);

// Looks the AudioOut up again when it runs, so one deinitialized after the
// callback was queued is skipped. A deinitialized framebuffer's storage may
// already hold another display, so only the active framebuffer is touched.
static void dvi_audio_run_refill(void *data) {
    picodvi_framebuffer_obj_t *self = data;
    if (self != active_picodvi || self->audioout == MP_OBJ_NULL) {
        return;
    }
    audioout_refill(MP_OBJ_TO_PTR(self->audioout));
}

// Called from the frame interrupt after the next frame has started.
void __not_in_flash_func(picodvi_audioout_frame_done)(void) {
    if (active_picodvi->dvi_audio && active_picodvi->audioout != MP_OBJ_NULL) {
        background_callback_add(&dvi_audio_refill_callback, dvi_audio_run_refill, active_picodvi);
    }
}

// Called at frame boundary only; old DMA frame has completed.
uint32_t *__not_in_flash_func(picodvi_audioout_next_frame)(void) {
    dvi_audio_state_t *audio = active_picodvi->dvi_audio;
    if (!audio || audio->detached) {
        return active_picodvi->dma_commands;
    }
    if (audio->detach) {
        // From this frame on, DMA reads only the plain video commands.
        audio->active = -1;
        audio->detached = true;
        return active_picodvi->dma_commands;
    }
    if (audio->active >= 0) {
        audio->bank[audio->active].samples = 0;
        audio->active = -1;
    }
    int next = -1;
    for (int i = 0; i < DVI_AUDIO_BANKS; i++) {
        if (!audio->bank[i].ready) {
            continue;
        }
        if (next < 0 || (int32_t)(audio->bank[i].order - audio->bank[next].order) < 0) {
            next = i;
        }
    }
    if (next >= 0) {
        audio->active = next;
        audio->bank[next].ready = false;
        return audio->bank[next].commands;
    }
    // No bank ready: send a frame of silence rather than repeat the last one.
    return audio->base_commands;
}

// Encodes up to one frame of 16-bit stereo samples, interleaved left, right,
// into a free bank, with silence after them. Sets *used to the sample frames
// taken. Returns false, changing nothing, if audio is not reserved or no bank
// is free. Never allocates or raises.
static bool picodvi_framebuffer_audio_fill(picodvi_framebuffer_obj_t *self, const int16_t *samples, size_t available, size_t *used) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return false;
    }
    int bank_index = -1;
    // Read active + ready atomically: an IRQ can promote a ready bank to active
    // between these two observations. Once selected, an unready bank cannot be
    // taken by the ISR until this writer publishes it below.
    uint32_t irq_state = save_and_disable_interrupts();
    for (int i = 0; i < DVI_AUDIO_BANKS; i++) {
        if (audio->active != i && !audio->bank[i].ready) {
            bank_index = i;
            break;
        }
    }
    restore_interrupts(irq_state);
    if (bank_index < 0) {
        return false;
    }
    dvi_audio_bank_t *bank = &audio->bank[bank_index];
    memcpy(bank->commands, audio->base_commands, self->dma_commands_len * sizeof(uint32_t));
    size_t command_word = 0;
    size_t samples_read = 0;
    unsigned packets = 0;
    bool doubled = self->output_width != self->width;
    size_t command_size = doubled ? 4 : 2;
    size_t entry_offset = doubled ? 2 : 0;
    const size_t active_start = MODE_640_V_SYNC_WIDTH + MODE_640_V_BACK_PORCH;
    const size_t active_end = active_start + MODE_640_V_ACTIVE_LINES;
    for (size_t line = 0; line < MODE_640_V_TOTAL_LINES; line++) {
        bool active = line >= active_start && line < active_end;
        uint32_t *entry = &bank->commands[command_word + entry_offset];
        bool acr = dvi_audio_acr_due(&audio->acr_phase, audio->pixel_clock);
        bool due = dvi_audio_packet_due(&audio->phase, audio->pixel_clock, line < 3 || acr);
        if (line == 0 && acr) {
            entry[0] = audio->header_length[0];
            entry[1] = (uintptr_t)audio->header_line[0];
        } else if (line == 1 || line == 2) {
            // InfoFrames, as in the base list.
        } else if (acr) {
            entry[0] = audio->acr_length[active];
            entry[1] = (uintptr_t)audio->acr_line[active];
        } else if (!due) {
            if (active) {
                entry[0] = MP_ARRAY_SIZE(dvi_audio_vactive_line640);
                entry[1] = (uintptr_t)dvi_audio_vactive_line640;
            } else {
                const uint32_t *plain = &self->dma_commands[command_word + entry_offset];
                entry[0] = plain[0];
                entry[1] = plain[1];
            }
        } else if (samples_read >= available || packets >= audio->max_packets) {
            entry[0] = audio->silence_length[active];
            entry[1] = (uintptr_t)audio->silence_line[active];
        } else {
            audio_sample_t frames[4] = {{0}};
            size_t count = MIN(available - samples_read, 4);
            for (size_t j = 0; j < count; j++) {
                size_t k = samples_read + j;
                frames[j].left = samples[2 * k];
                frames[j].right = samples[2 * k + 1];
            }
            hstx_packet_t packet;
            hstx_data_island_t island;
            audio->channel_frame = hstx_packet_set_audio_samples(&packet, frames, count, audio->channel_frame);
            hstx_encode_data_island(&island, &packet, false, true);
            entry[0] = dvi_audio_build_line(bank->lines[packets], &island, false, active);
            entry[1] = (uintptr_t)bank->lines[packets];
            packets++;
            samples_read += count;
        }
        command_word += command_size * (active ? 2 : 1);
    }
    bank->samples = samples_read;
    bank->order = audio->next_order++;
    __dmb();
    bank->ready = true;
    *used = samples_read;
    return true;
}

// Allocates the audio state and starts sending audio signaling and silence.
// Does nothing if already reserved. Raises ValueError if the output is not
// 640 pixels wide, or MemoryError, leaving nothing allocated.
static void picodvi_framebuffer_audio_reserve(picodvi_framebuffer_obj_t *self) {
    if (self->output_width != 640) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_framebuffer);
    }
    dvi_audio_allocate(self);
}

// Stops audio and frees the audio state, waiting up to two frames for DMA to
// stop reading it.
static void picodvi_framebuffer_audio_release(picodvi_framebuffer_obj_t *self) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return;
    }
    audio->detach = true;
    // Wait for the frame interrupt to move DMA back to the video commands.
    // Without frame interrupts, DMA stops at the end of the current frame.
    for (int i = 0; i < 1000 && !audio->detached && active_picodvi == self; i++) {
        busy_wait_us(100);
    }
    uint32_t irq_state = save_and_disable_interrupts();
    self->dvi_audio = NULL;
    restore_interrupts(irq_state);
    dvi_audio_free_state(audio);
}

// Returns the number of sample frames queued or playing, 0 if not reserved.
static size_t picodvi_framebuffer_audio_pending(picodvi_framebuffer_obj_t *self) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return 0;
    }
    uint32_t irq_state = save_and_disable_interrupts();
    size_t count = 0;
    for (int i = 0; i < DVI_AUDIO_BANKS; i++) {
        if (audio->bank[i].ready || audio->active == i) {
            count += audio->bank[i].samples;
        }
    }
    restore_interrupts(irq_state);
    return count;
}

// Drops queued banks, so silence follows the frame now playing.
static void picodvi_framebuffer_audio_stop(picodvi_framebuffer_obj_t *self) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return;
    }
    uint32_t irq_state = save_and_disable_interrupts();
    for (int i = 0; i < DVI_AUDIO_BANKS; i++) {
        audio->bank[i].ready = false;
        if (audio->active != i) {
            audio->bank[i].samples = 0;
        }
    }
    restore_interrupts(irq_state);
}


// Convert up to space frames of the current sample buffer into the stage.
static void audioout_convert(picodvi_audioout_obj_t *self, size_t space) {
    audiosample_base_t *base = audiosample_cast_obj(self->sample);
    size_t n = MIN(self->source_frames, space);
    int16_t *out = &self->stage[self->stage_frames * 2];
    const uint8_t *in = self->source;
    bool stereo = base->channel_count == 2;
    if (base->bits_per_sample == 8) {
        if (base->samples_signed) {
            if (stereo) {
                audiosample_convert_s8s_s16s(out, (const int8_t *)in, n);
            } else {
                audiosample_convert_s8m_s16s(out, (const int8_t *)in, n);
            }
        } else {
            if (stereo) {
                audiosample_convert_u8s_s16s(out, in, n);
            } else {
                audiosample_convert_u8m_s16s(out, in, n);
            }
        }
    } else {
        if (base->samples_signed) {
            if (stereo) {
                memcpy(out, in, n * 4);
            } else {
                audiosample_convert_s16m_s16s(out, (const int16_t *)in, n);
            }
        } else {
            if (stereo) {
                audiosample_convert_u16s_s16s(out, (const uint16_t *)in, n);
            } else {
                audiosample_convert_u16m_s16s(out, (const uint16_t *)in, n);
            }
        }
    }
    self->source += n * self->bytes_per_frame;
    self->source_frames -= n;
    self->stage_frames += n;
}

// Top up the stage from the sample, getting new buffers and looping as needed.
static void audioout_stage(picodvi_audioout_obj_t *self) {
    while (!self->source_done && self->stage_frames < PICODVI_AUDIOOUT_STAGE_FRAMES) {
        if (self->source_frames == 0) {
            if (self->source_last) {
                // Also stop a looping sample with no frames at all.
                if (!self->loop || self->pass_empty) {
                    self->source_done = true;
                    return;
                }
                audiosample_reset_buffer(self->sample, false, 0);
                self->pass_empty = true;
            }
            uint8_t *buffer;
            uint32_t length;
            audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, &buffer, &length);
            if (result == GET_BUFFER_ERROR) {
                self->source_done = true;
                return;
            }
            self->source = buffer;
            self->source_frames = length / self->bytes_per_frame;
            if (self->source_frames > 0) {
                self->pass_empty = false;
            }
            self->source_last = result == GET_BUFFER_DONE;
            continue;
        }
        audioout_convert(self, PICODVI_AUDIOOUT_STAGE_FRAMES - self->stage_frames);
    }
}

// Runs in the background after every frame, and once from play() and
// resume(). Fills every free bank from the stage, or with silence when
// nothing is playing, so the ACR timing carries on between frames.
static void audioout_refill(picodvi_audioout_obj_t *self) {
    if (self->refilling) {
        return;
    }
    self->refilling = true;
    while (true) {
        size_t available = 0;
        if (self->sample != MP_OBJ_NULL && !self->paused) {
            audioout_stage(self);
            available = self->stage_frames;
            // Wait for more of a sample that is still playing.
            if (available == 0 && !self->source_done) {
                break;
            }
        }
        size_t used;
        if (!picodvi_framebuffer_audio_fill(self->framebuffer, self->stage, available, &used)) {
            break;
        }
        self->stage_frames -= used;
        memmove(self->stage, &self->stage[used * 2], self->stage_frames * 4);
    }
    self->refilling = false;
}

void common_hal_picodvi_audioout_construct(picodvi_audioout_obj_t *self, picodvi_framebuffer_obj_t *framebuffer) {
    if (framebuffer->audioout != MP_OBJ_NULL) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q in use"), MP_QSTR_framebuffer);
    }
    picodvi_framebuffer_audio_reserve(framebuffer);
    self->framebuffer = framebuffer;
    self->sample = MP_OBJ_NULL;
    // The framebuffer keeps this object alive and schedules its refills.
    framebuffer->audioout = MP_OBJ_FROM_PTR(self);
}

void picodvi_audioout_reset(void) {
    if (active_picodvi != NULL && active_picodvi->audioout != MP_OBJ_NULL) {
        common_hal_picodvi_audioout_deinit(MP_OBJ_TO_PTR(active_picodvi->audioout));
    }
}

bool common_hal_picodvi_audioout_deinited(picodvi_audioout_obj_t *self) {
    return self->framebuffer == NULL;
}

void common_hal_picodvi_audioout_deinit(picodvi_audioout_obj_t *self) {
    if (common_hal_picodvi_audioout_deinited(self)) {
        return;
    }
    common_hal_picodvi_audioout_stop(self);
    self->framebuffer->audioout = MP_OBJ_NULL;
    picodvi_framebuffer_audio_release(self->framebuffer);
    self->framebuffer = NULL;
}

void common_hal_picodvi_audioout_play(picodvi_audioout_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_base_t *base = audiosample_check(sample);
    mp_arg_validate_int(base->sample_rate, 48000, MP_QSTR_sample_rate);
    mp_arg_validate_int_range(base->channel_count, 1, 2, MP_QSTR_channel_count);
    if (base->bits_per_sample != 8 && base->bits_per_sample != 16) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_bits_per_sample);
    }
    common_hal_picodvi_audioout_stop(self);
    audiosample_reset_buffer(sample, false, 0);
    self->bytes_per_frame = base->channel_count * base->bits_per_sample / 8;
    self->source_frames = 0;
    self->source_last = false;
    self->source_done = false;
    self->pass_empty = true;
    self->stage_frames = 0;
    self->loop = loop;
    self->paused = false;
    self->refilling = false;
    self->sample = sample;
    audioout_refill(self);
}

void common_hal_picodvi_audioout_stop(picodvi_audioout_obj_t *self) {
    self->sample = MP_OBJ_NULL;
    self->paused = false;
    picodvi_framebuffer_audio_stop(self->framebuffer);
}

bool common_hal_picodvi_audioout_get_playing(picodvi_audioout_obj_t *self) {
    if (self->sample == MP_OBJ_NULL) {
        return false;
    }
    return !self->source_done || self->stage_frames > 0 ||
           picodvi_framebuffer_audio_pending(self->framebuffer) > 0;
}

void common_hal_picodvi_audioout_pause(picodvi_audioout_obj_t *self) {
    self->paused = true;
}

void common_hal_picodvi_audioout_resume(picodvi_audioout_obj_t *self) {
    self->paused = false;
    audioout_refill(self);
}

bool common_hal_picodvi_audioout_get_paused(picodvi_audioout_obj_t *self) {
    return self->paused;
}
