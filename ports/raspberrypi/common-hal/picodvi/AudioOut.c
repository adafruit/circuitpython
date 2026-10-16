// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Adafruit Industries
//
// SPDX-License-Identifier: MIT

// 32, 44.1 or 48 kHz stereo DVI audio for the RP2350 picodvi Framebuffer.
// No interrupt-time allocation, sample conversion, packet encoding, or Python callbacks.

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"

#include "bindings/picodvi/AudioOut.h"
#include "common-hal/picodvi/dvi_audio_packet.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "shared-module/audiocore/__init__.h"
#include "shared-bindings/supervisor/Runtime.h"
#include "supervisor/background_callback.h"
#include "supervisor/port.h"

typedef struct {
    uint32_t sample_rate;
    // The usual ACR N, for when no N gives an exact CTS.
    uint32_t usual_n;
} dvi_audio_rate_t;

// The basic audio rates every DVI sink takes.
static const dvi_audio_rate_t dvi_audio_rates[] = {
    { 32000, 4096 },
    { 44100, 6272 },
    { 48000, 6144 },
};
#define DVI_AUDIO_RATE_COUNT MP_ARRAY_SIZE(dvi_audio_rates)
#define DVI_AUDIO_H_TOTAL (MODE_640_H_FRONT_PORCH + MODE_640_H_SYNC_WIDTH + MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS)
// Lines per frame that carry a data island. At the usual 25 MHz pixel clock
// and 48 kHz a 625 line frame needs 240 sample packets, 20 ACR packets and 2
// InfoFrames.
#define DVI_AUDIO_SLOTS 280
// Buffers for encoded sample packets. Other packets never change, so their
// islands point at fixed copies. 160 hold about 13 ms of 48 kHz audio.
#define DVI_AUDIO_POOL 160
// Packet words of an island, without its two guard band words at each end.
#define DVI_AUDIO_SLOT_WORDS (HSTX_DATA_ISLAND_WORDS - 4)
// The refill never writes a slot this close ahead of the one DMA reads.
#define DVI_AUDIO_MARGIN 4
// Nor more than this far ahead, so play() starts within about 15 ms.
#define DVI_AUDIO_AHEAD (DVI_AUDIO_SLOTS * 3 / 4)
// While a sample is waiting for data, silence is only written this close
// ahead of DMA.
#define DVI_AUDIO_LOW_WATER (DVI_AUDIO_SLOTS / 4)
// The sample pool holds about 13 ms, less than one 20 ms frame, so a timer
// queues the refill this often as well as once per frame.
#define DVI_AUDIO_REFILL_US 2500
#define DVI_AUDIO_PREFIX_WORDS 9
#define DVI_AUDIO_SUFFIX_BLANK_WORDS 9
#define DVI_AUDIO_SUFFIX_ACTIVE_WORDS 15

typedef uint32_t dvi_audio_island_t[DVI_AUDIO_SLOT_WORDS];

typedef struct dvi_audio_state {
    // One fixed command list. Each island line is three transfers: a prefix
    // up to and including the leading guard band, the island, and a suffix
    // from the trailing guard band on. Once DMA has read a slot, the refill
    // points its island at the packet for the next frame.
    uint32_t *commands;
    dvi_audio_island_t *pool;
    size_t command_size;
    repeating_timer_t timer;
    bool timer_running;
    // Frames started on the audio command list, counted by the frame
    // interrupt.
    volatile uint32_t frames;
    // Set to go back to video only at the next frame, then acknowledged by
    // the frame interrupt once nothing reads this state any more.
    volatile bool detach;
    volatile bool detached;
    // The next slot to write, counted from the first audio frame, and its
    // index in the frame.
    uint64_t write_position;
    size_t write_slot;
    // The last read position found, to search on from.
    uint32_t read_frames;
    size_t read_slot;
    // Pool buffers are used in order. Each remembers the slot it was
    // written for, and is free again once DMA has read that slot.
    uint64_t pool_position[DVI_AUDIO_POOL];
    size_t pool_head;
    size_t pool_tail;
    size_t pool_count;
    uint32_t pixel_clock;
    // The rate being sent, and the rate play() asked for, which takes over
    // from the next slot written. Indexes into dvi_audio_rates.
    size_t rate_index;
    size_t next_rate_index;
    // ACR packets per second at each rate, 128 * fs / N.
    uint32_t acr_rate[DVI_AUDIO_RATE_COUNT];
    // Sample and ACR timing, carried from slot to slot.
    uint64_t sample_phase;
    uint64_t acr_phase;
    int channel_frame;
    // Idle slots written in a row, the timing at the last idle slot 0, and
    // whether the slots hold a repeating idle frame that needs no writes.
    size_t idle_run;
    uint64_t idle_sample_phase;
    uint64_t idle_acr_phase;
    bool idle_steady;
    // Command list entry of each slot's island, its line, and the sample
    // frames of the playing sample it holds.
    uint16_t slot_entry[DVI_AUDIO_SLOTS];
    uint16_t slot_line[DVI_AUDIO_SLOTS];
    uint8_t slot_samples[DVI_AUDIO_SLOTS];
    // Packets that never change, for blanking [0] and vsync [1] lines.
    dvi_audio_island_t acr[DVI_AUDIO_RATE_COUNT][2];
    dvi_audio_island_t null_packet[2];
    dvi_audio_island_t silence[2];
    dvi_audio_island_t audio_infoframe[DVI_AUDIO_RATE_COUNT];
    dvi_audio_island_t avi_infoframe;
    uint32_t prefix[2][DVI_AUDIO_PREFIX_WORDS];
    uint32_t suffix_blank[2][DVI_AUDIO_SUFFIX_BLANK_WORDS];
    uint32_t suffix_active[DVI_AUDIO_SUFFIX_ACTIVE_WORDS];
} dvi_audio_state_t;

// Static so it can never be freed while it is queued.
static background_callback_t dvi_audio_refill_callback;

#if defined(DEFAULT_DVI_BUS_CLK_DP)
// supervisor.runtime.audio. Static so it outlives the VM, like the boot display.
picodvi_audioout_obj_t picodvi_board_audioout = {
    .base = { .type = &mp_type_NoneType },
};

static bool is_board_audioout(const picodvi_audioout_obj_t *self) {
    return self == &picodvi_board_audioout;
}
#else
static bool is_board_audioout(const picodvi_audioout_obj_t *self) {
    (void)self;
    return false;
}
#endif

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

// Fills the line pieces around a slot. The island sits in the horizontal
// sync pulse, after an 8 character data island preamble.
static void dvi_audio_init_line_pieces(dvi_audio_state_t *audio) {
    for (int vsync = 0; vsync < 2; vsync++) {
        uint32_t h0 = vsync ? SYNC_V0_H0 : SYNC_V1_H0;
        uint32_t h1 = vsync ? SYNC_V0_H1 : SYNC_V1_H1;
        // The guard band only depends on the sync levels.
        hstx_packet_t packet;
        hstx_data_island_t island;
        hstx_packet_init(&packet);
        hstx_encode_data_island(&island, &packet, vsync, true);
        uint32_t guard = island.words[0];

        uint32_t *p = audio->prefix[vsync];
        *p++ = HSTX_CMD_RAW_REPEAT | MODE_640_H_FRONT_PORCH;
        *p++ = h1;
        *p++ = HSTX_CMD_NOP;
        *p++ = HSTX_CMD_RAW_REPEAT | 8;
        *p++ = (h0 & 0x3ff) | (TMDS_CTRL_01 << 10) | (TMDS_CTRL_01 << 20);
        *p++ = HSTX_CMD_NOP;
        *p++ = HSTX_CMD_RAW | HSTX_DATA_ISLAND_WORDS;
        *p++ = guard;
        *p++ = guard;

        uint32_t *s = audio->suffix_blank[vsync];
        *s++ = guard;
        *s++ = guard;
        *s++ = HSTX_CMD_NOP;
        *s++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_SYNC_WIDTH - 8 - HSTX_DATA_ISLAND_WORDS);
        *s++ = h0;
        *s++ = HSTX_CMD_NOP;
        *s++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH + MODE_640_H_ACTIVE_PIXELS);
        *s++ = h1;
        *s++ = HSTX_CMD_NOP;

        if (!vsync) {
            // Active lines are never in vsync.
            memcpy(audio->suffix_active, audio->suffix_blank[0], 6 * sizeof(uint32_t));
            s = &audio->suffix_active[6];
            *s++ = HSTX_CMD_RAW_REPEAT | (MODE_640_H_BACK_PORCH - 10);
            *s++ = h1;
            *s++ = HSTX_CMD_NOP;
            *s++ = HSTX_CMD_RAW_REPEAT | 8;
            *s++ = (h1 & 0x3ff) | (TMDS_CTRL_01 << 10) | (TMDS_CTRL_00 << 20);
            *s++ = HSTX_CMD_NOP;
            *s++ = HSTX_CMD_RAW_REPEAT | 2;
            *s++ = 0x2ccu | (0x133u << 10) | (0x2ccu << 20);
            *s++ = HSTX_CMD_TMDS | MODE_640_H_ACTIVE_PIXELS;
        }
    }
}

static void dvi_audio_encode(uint32_t *out, const hstx_packet_t *packet, bool vsync) {
    hstx_data_island_t island;
    hstx_encode_data_island(&island, packet, vsync, true);
    memcpy(out, &island.words[2], sizeof(dvi_audio_island_t));
}

static bool dvi_audio_slot_vsync(const dvi_audio_state_t *audio, size_t slot) {
    return audio->slot_line[slot] < MODE_640_V_SYNC_WIDTH;
}

// The command list word holding the address of a slot's island.
static uint32_t *dvi_audio_slot_island(dvi_audio_state_t *audio, size_t slot) {
    return &audio->commands[(audio->slot_entry[slot] + 1) * audio->command_size - 1];
}

// Pick N so that every frame has the same whole number of ACR packets, and
// CTS is exact. Then a frame of silence repeats exactly and needs no
// rewriting. N may be anything from 128 * fs / 1500 to 128 * fs / 300.
// If no N fits, use the usual N for the rate and round CTS.
static uint32_t dvi_audio_find_n(uint32_t pixel_clock, const dvi_audio_rate_t *rate_info) {
    const uint64_t frame_pixels = (uint64_t)MODE_640_V_TOTAL_LINES * DVI_AUDIO_H_TOTAL;
    const uint32_t n_times_rate = 128 * rate_info->sample_rate;
    uint32_t per_frame = (1000 * frame_pixels + pixel_clock / 2) / pixel_clock;
    for (uint32_t k = per_frame - 1; k <= per_frame + 1; k++) {
        // ACR packets per second when each frame has k.
        uint64_t rate = (uint64_t)k * pixel_clock;
        if (k == 0 || rate % frame_pixels != 0) {
            continue;
        }
        rate /= frame_pixels;
        if (n_times_rate % rate != 0) {
            continue;
        }
        uint32_t candidate = n_times_rate / rate;
        if (candidate >= n_times_rate / 1500 && candidate <= n_times_rate / 300 &&
            ((uint64_t)pixel_clock * candidate) % n_times_rate == 0) {
            return candidate;
        }
    }
    return rate_info->usual_n;
}

static void dvi_audio_init_packets(dvi_audio_state_t *audio) {
    hstx_packet_t packet;
    for (size_t rate_index = 0; rate_index < DVI_AUDIO_RATE_COUNT; rate_index++) {
        const dvi_audio_rate_t *rate_info = &dvi_audio_rates[rate_index];
        const uint32_t n_times_rate = 128 * rate_info->sample_rate;
        uint32_t n = dvi_audio_find_n(audio->pixel_clock, rate_info);
        audio->acr_rate[rate_index] = n_times_rate / n;
        uint32_t cts = ((uint64_t)audio->pixel_clock * n + n_times_rate / 2) / n_times_rate;
        hstx_packet_set_acr(&packet, n, cts);
        for (int vsync = 0; vsync < 2; vsync++) {
            dvi_audio_encode(audio->acr[rate_index][vsync], &packet, vsync);
        }
        // The first two slots of every frame carry the InfoFrames.
        hstx_packet_set_audio_infoframe(&packet, rate_info->sample_rate, 2, 16);
        dvi_audio_encode(audio->audio_infoframe[rate_index], &packet, dvi_audio_slot_vsync(audio, 0));
    }
    // 48 kHz until play() asks for another rate.
    audio->rate_index = DVI_AUDIO_RATE_COUNT - 1;
    audio->next_rate_index = audio->rate_index;
    hstx_packet_init(&packet);
    for (int vsync = 0; vsync < 2; vsync++) {
        dvi_audio_encode(audio->null_packet[vsync], &packet, vsync);
    }
    // Frame position 4 never sets the block start flag, and frames 4 to 7
    // carry no channel status bits, so silence is the same at every rate.
    audio_sample_t silence[4] = {{0}};
    hstx_packet_set_audio_samples(&packet, silence, 4, 4, dvi_audio_rates[DVI_AUDIO_RATE_COUNT - 1].sample_rate);
    for (int vsync = 0; vsync < 2; vsync++) {
        dvi_audio_encode(audio->silence[vsync], &packet, vsync);
    }
    // Unspecified VIC: preserve CircuitPython's existing 625-line timing.
    hstx_packet_set_avi_infoframe(&packet, 0, 0);
    dvi_audio_encode(audio->avi_infoframe, &packet, dvi_audio_slot_vsync(audio, 1));
}

// Builds the audio command list from the video one. Lines without a slot
// keep their video transfers, except that active lines get the video
// preamble and guard band that audio mode needs. Every island starts as a
// null packet.
static void dvi_audio_build_commands(dvi_audio_state_t *audio, const picodvi_framebuffer_obj_t *self) {
    const size_t size = audio->command_size;
    // Word offset of the transfer count and read address within an entry.
    const size_t data = size - 2;
    const size_t active_start = MODE_640_V_SYNC_WIDTH + MODE_640_V_BACK_PORCH;
    const size_t active_end = active_start + MODE_640_V_ACTIVE_LINES;
    const uint32_t *video = self->dma_commands;
    uint32_t *out = audio->commands;
    size_t slot = 0;
    for (size_t line = 0; line < MODE_640_V_TOTAL_LINES; line++) {
        bool active = line >= active_start && line < active_end;
        bool vsync = line < MODE_640_V_SYNC_WIDTH;
        // video points at this line's first entry, the whole line when
        // blanking or the part before the pixels when active.
        if (slot < DVI_AUDIO_SLOTS && audio->slot_line[slot] == line) {
            const uint32_t *pieces[3] = {
                audio->prefix[vsync], audio->null_packet[vsync],
                active ? audio->suffix_active : audio->suffix_blank[vsync],
            };
            const uint32_t lengths[3] = {
                DVI_AUDIO_PREFIX_WORDS, DVI_AUDIO_SLOT_WORDS,
                active ? DVI_AUDIO_SUFFIX_ACTIVE_WORDS : DVI_AUDIO_SUFFIX_BLANK_WORDS,
            };
            for (int i = 0; i < 3; i++) {
                if (i == 1) {
                    audio->slot_entry[slot] = (out - audio->commands) / size;
                }
                memcpy(out, video, data * sizeof(uint32_t));
                out[data] = lengths[i];
                out[data + 1] = (uintptr_t)pieces[i];
                out += size;
            }
            slot++;
        } else {
            memcpy(out, video, size * sizeof(uint32_t));
            if (active) {
                out[data] = MP_ARRAY_SIZE(dvi_audio_vactive_line640);
                out[data + 1] = (uintptr_t)dvi_audio_vactive_line640;
            }
            out += size;
        }
        video += size;
        if (active) {
            // The pixel transfer.
            memcpy(out, video, size * sizeof(uint32_t));
            out += size;
            video += size;
        }
    }
    // The empty last entry that raises the frame interrupt.
    memcpy(out, video, size * sizeof(uint32_t));
}

// Command list words for the framebuffer's video list plus two more
// transfers on each slot line.
static size_t dvi_audio_commands_len(const picodvi_framebuffer_obj_t *self, size_t command_size) {
    return self->dma_commands_len + DVI_AUDIO_SLOTS * 2 * command_size;
}

static void dvi_audio_free_state(dvi_audio_state_t *audio) {
    if (audio->timer_running) {
        cancel_repeating_timer(&audio->timer);
    }
    if (audio->commands) {
        port_free(audio->commands);
    }
    if (audio->pool) {
        port_free(audio->pool);
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
        if (is_board_audioout(audioout)) {
            // supervisor.runtime.audio goes back to None, as the display does.
            audioout->base.type = &mp_type_NoneType;
        }
    }
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return;
    }
    self->dvi_audio = NULL;
    dvi_audio_free_state(audio);
}

// The first slot DMA has not finished reading, counted from the first audio
// frame. Before the first audio frame starts, nothing has been read.
static uint64_t dvi_audio_read_position(dvi_audio_state_t *audio, const picodvi_framebuffer_obj_t *self) {
    uint32_t frames;
    uintptr_t read_addr;
    do {
        frames = audio->frames;
        read_addr = dma_hw->ch[self->dma_command_channel].read_addr;
    } while (frames != audio->frames);
    if (frames == 0) {
        return 0;
    }
    // Entries the command channel has loaded. The last one may still be
    // sending.
    size_t loaded = (read_addr - (uintptr_t)audio->commands) / (audio->command_size * sizeof(uint32_t));
    // Count the slots whose transfer is done, slot_entry[i] + 1 < loaded,
    // going on from the last answer in the same frame.
    size_t done = frames == audio->read_frames ? audio->read_slot : 0;
    while (done < DVI_AUDIO_SLOTS && audio->slot_entry[done] + 1u < loaded) {
        done++;
    }
    audio->read_frames = frames;
    audio->read_slot = done;
    return (uint64_t)(frames - 1) * DVI_AUDIO_SLOTS + done;
}

static void dvi_audio_advance(dvi_audio_state_t *audio) {
    audio->write_position++;
    audio->write_slot = audio->write_slot + 1 == DVI_AUDIO_SLOTS ? 0 : audio->write_slot + 1;
}

// Works out the packet for the next slot and advances the timing past it:
// an InfoFrame, an ACR packet, up to four sample frames from samples into a
// pool buffer, silence or a null packet. Takes real samples only while
// available is not zero, and sets *count to how many it took. Returns NULL,
// changing nothing, if the samples need a pool buffer and none is free.
static const uint32_t *dvi_audio_next_packet(dvi_audio_state_t *audio, const int16_t *samples, size_t available, size_t *count) {
    size_t slot = audio->write_slot;
    size_t previous = slot == 0 ? DVI_AUDIO_SLOTS - 1 : slot - 1;
    size_t lines = (audio->slot_line[slot] + MODE_640_V_TOTAL_LINES - audio->slot_line[previous]) % MODE_640_V_TOTAL_LINES;
    const uint64_t pixel_clock = audio->pixel_clock;
    uint64_t sample_phase = audio->sample_phase + (uint64_t)lines * dvi_audio_rates[audio->rate_index].sample_rate * DVI_AUDIO_H_TOTAL;
    uint64_t acr_phase = audio->acr_phase + (uint64_t)lines * audio->acr_rate[audio->rate_index] * DVI_AUDIO_H_TOTAL;
    // Never let sample timing build up a long burst.
    if (sample_phase > pixel_clock * 12) {
        sample_phase = pixel_clock * 12;
    }
    bool vsync = dvi_audio_slot_vsync(audio, slot);
    const uint32_t *island;
    *count = 0;
    if (slot == 0) {
        island = audio->audio_infoframe[audio->rate_index];
    } else if (slot == 1) {
        island = audio->avi_infoframe;
    } else if (acr_phase >= pixel_clock) {
        acr_phase -= pixel_clock;
        island = audio->acr[audio->rate_index][vsync];
    } else {
        // Sample packets carry four frames, fewer only at the end of a
        // sample.
        size_t due = 0;
        while (due < 4 && sample_phase >= (due + 1) * pixel_clock) {
            due++;
        }
        if (available > 0 && (due == 4 || (due > 0 && due >= available))) {
            if (audio->pool_count == DVI_AUDIO_POOL) {
                return NULL;
            }
            size_t n = MIN(due, available);
            audio_sample_t packet_samples[4];
            for (size_t i = 0; i < n; i++) {
                packet_samples[i].left = samples[2 * i];
                packet_samples[i].right = samples[2 * i + 1];
            }
            hstx_packet_t packet;
            audio->channel_frame = hstx_packet_set_audio_samples(&packet, packet_samples, n, audio->channel_frame,
                dvi_audio_rates[audio->rate_index].sample_rate);
            uint32_t *buffer = audio->pool[audio->pool_head];
            dvi_audio_encode(buffer, &packet, vsync);
            audio->pool_position[audio->pool_head] = audio->write_position;
            audio->pool_head = (audio->pool_head + 1) % DVI_AUDIO_POOL;
            audio->pool_count++;
            sample_phase -= n * pixel_clock;
            island = buffer;
            *count = n;
        } else if (available == 0 && due == 4) {
            sample_phase -= 4 * pixel_clock;
            island = audio->silence[vsync];
        } else {
            island = audio->null_packet[vsync];
        }
    }
    audio->sample_phase = sample_phase;
    audio->acr_phase = acr_phase;
    dvi_audio_advance(audio);
    return island;
}

// Points the next slot at its packet. Sets *used to the real sample frames
// it took. With wait set, writes nothing unless DMA is close behind. Returns
// false when the slot cannot be written yet. Never allocates or raises.
//
// With nothing to play, every frame comes out the same once the timing at
// slot 0 repeats. Then the slots already hold the right packets, and this
// stops writing until there is something to play. At 44.1 kHz a frame holds
// 882 samples, not a whole number of packets, so frames never repeat and
// silence keeps being written.
static bool dvi_audio_fill_slot(picodvi_framebuffer_obj_t *self, dvi_audio_state_t *audio, const int16_t *samples, size_t available, bool wait, size_t *used) {
    *used = 0;
    bool idle = available == 0 && !wait;
    if (audio->detach || (audio->idle_steady && idle)) {
        return false;
    }
    uint64_t read = dvi_audio_read_position(audio, self);
    if (audio->idle_steady) {
        // Catch the timing up to just ahead of DMA. The slots passed over
        // already hold the packets it works out. Whole frames repeat, so
        // skip them, then work through the rest of one.
        uint64_t target = read + DVI_AUDIO_MARGIN;
        if (audio->write_position < target) {
            audio->write_position += (target - audio->write_position) / DVI_AUDIO_SLOTS * DVI_AUDIO_SLOTS;
        }
        while (audio->write_position < target) {
            size_t count;
            dvi_audio_next_packet(audio, NULL, 0, &count);
        }
        audio->idle_steady = false;
        audio->idle_run = 0;
    }
    audio->rate_index = audio->next_rate_index;
    // Free the pool buffers DMA has finished with.
    while (audio->pool_count > 0 && audio->pool_position[audio->pool_tail] < read) {
        audio->pool_tail = (audio->pool_tail + 1) % DVI_AUDIO_POOL;
        audio->pool_count--;
    }
    if (audio->write_position >= read + DVI_AUDIO_AHEAD) {
        return false;
    }
    if (audio->frames > 0 && audio->write_position < read + DVI_AUDIO_MARGIN) {
        // Fell behind. The slots skipped send null packets rather than
        // whatever they last pointed at. A frame or more behind, null every
        // slot once and skip the whole frames.
        uint64_t target = read + DVI_AUDIO_MARGIN;
        if (target - audio->write_position >= DVI_AUDIO_SLOTS) {
            for (size_t slot = 0; slot < DVI_AUDIO_SLOTS; slot++) {
                *dvi_audio_slot_island(audio, slot) = (uintptr_t)audio->null_packet[dvi_audio_slot_vsync(audio, slot)];
                audio->slot_samples[slot] = 0;
            }
            audio->write_position += (target - audio->write_position) / DVI_AUDIO_SLOTS * DVI_AUDIO_SLOTS;
        }
        while (audio->write_position < target) {
            size_t slot = audio->write_slot;
            *dvi_audio_slot_island(audio, slot) = (uintptr_t)audio->null_packet[dvi_audio_slot_vsync(audio, slot)];
            audio->slot_samples[slot] = 0;
            dvi_audio_advance(audio);
        }
        audio->idle_run = 0;
    }
    if (wait && audio->write_position >= read + DVI_AUDIO_LOW_WATER) {
        return false;
    }
    size_t slot = audio->write_slot;
    if (slot == 0 && idle) {
        if (audio->idle_run >= DVI_AUDIO_SLOTS &&
            audio->sample_phase == audio->idle_sample_phase &&
            audio->acr_phase == audio->idle_acr_phase) {
            audio->idle_steady = true;
            return false;
        }
        audio->idle_sample_phase = audio->sample_phase;
        audio->idle_acr_phase = audio->acr_phase;
    }
    size_t count;
    const uint32_t *island = dvi_audio_next_packet(audio, samples, available, &count);
    if (island == NULL) {
        // Wait for DMA to free a pool buffer.
        return false;
    }
    *dvi_audio_slot_island(audio, slot) = (uintptr_t)island;
    audio->slot_samples[slot] = count;
    audio->idle_run = idle ? audio->idle_run + 1 : 0;
    *used = count;
    return true;
}

static void dvi_audio_queue_refill(void);

static bool __not_in_flash_func(dvi_audio_timer_callback)(repeating_timer_t * timer) {
    (void)timer;
    dvi_audio_queue_refill();
    return true;
}

// Allocates the audio state and starts sending audio signaling and silence
// from the next frame. Returns false, leaving nothing allocated, if internal
// RAM is short. Never raises, so it can run before the VM starts.
static bool dvi_audio_try_allocate(picodvi_framebuffer_obj_t *self) {
    if (self->dvi_audio) {
        return true;
    }
    dvi_audio_state_t *audio = port_malloc(sizeof(*audio), true);
    if (!audio) {
        return false;
    }
    memset(audio, 0, sizeof(*audio));
    audio->command_size = self->output_width != self->width ? 4 : 2;
    // DMA reads all of these, so they must be in internal RAM.
    audio->commands = port_malloc(dvi_audio_commands_len(self, audio->command_size) * sizeof(uint32_t), true);
    audio->pool = port_malloc(DVI_AUDIO_POOL * sizeof(dvi_audio_island_t), true);
    if (!audio->commands || !audio->pool) {
        dvi_audio_free_state(audio);
        return false;
    }
    audio->pixel_clock = clock_get_hz(clk_hstx) / 5;
    for (size_t i = 0; i < DVI_AUDIO_SLOTS; i++) {
        audio->slot_line[i] = i * MODE_640_V_TOTAL_LINES / DVI_AUDIO_SLOTS;
    }
    dvi_audio_init_line_pieces(audio);
    dvi_audio_init_packets(audio);
    dvi_audio_build_commands(audio, self);
    // Start with silence, before the frame interrupt can switch to the list.
    size_t used;
    while (dvi_audio_fill_slot(self, audio, NULL, 0, false, &used)) {
    }
    audio->timer_running = add_repeating_timer_us(-DVI_AUDIO_REFILL_US, dvi_audio_timer_callback, NULL, &audio->timer);
    if (!audio->timer_running) {
        // Refilling once a frame can't keep ahead of DMA.
        dvi_audio_free_state(audio);
        return false;
    }
    self->dvi_audio = audio;
    return true;
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

// Called from interrupts: the frame interrupt and the refill timer.
static void __not_in_flash_func(dvi_audio_queue_refill)(void) {
    if (active_picodvi != NULL && active_picodvi->dvi_audio && active_picodvi->audioout != MP_OBJ_NULL) {
        background_callback_add(&dvi_audio_refill_callback, dvi_audio_run_refill, active_picodvi);
    }
}

// Called from the frame interrupt after the next frame has started.
void __not_in_flash_func(picodvi_audioout_frame_done)(void) {
    dvi_audio_queue_refill();
}

// Called at frame boundary only; old DMA frame has completed.
uint32_t *__not_in_flash_func(picodvi_audioout_next_frame)(void) {
    dvi_audio_state_t *audio = active_picodvi->dvi_audio;
    if (!audio || audio->detached) {
        return active_picodvi->dma_commands;
    }
    if (audio->detach) {
        // From this frame on, DMA reads only the plain video commands.
        audio->detached = true;
        return active_picodvi->dma_commands;
    }
    audio->frames++;
    return audio->commands;
}

// Allocates the audio state and starts sending audio signaling and silence.
// Does nothing if already reserved. Raises ValueError if the output is not
// 640 pixels wide, or MemoryError, leaving nothing allocated.
static void picodvi_framebuffer_audio_reserve(picodvi_framebuffer_obj_t *self) {
    if (self->output_width != 640) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_framebuffer);
    }
    if (!dvi_audio_try_allocate(self)) {
        m_malloc_fail(sizeof(dvi_audio_state_t));
    }
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

// Returns the sample frames written ahead of DMA, 0 if not reserved.
static size_t picodvi_framebuffer_audio_pending(picodvi_framebuffer_obj_t *self) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return 0;
    }
    uint64_t read = dvi_audio_read_position(audio, self);
    size_t count = 0;
    if (audio->write_position > read) {
        size_t slot = read % DVI_AUDIO_SLOTS;
        for (uint64_t ahead = audio->write_position - read; ahead > 0; ahead--) {
            count += audio->slot_samples[slot];
            slot = slot + 1 == DVI_AUDIO_SLOTS ? 0 : slot + 1;
        }
    }
    return count;
}

// Forgets the sample frames written ahead of DMA. They still play, for at
// most about 15 ms.
static void picodvi_framebuffer_audio_stop(picodvi_framebuffer_obj_t *self) {
    dvi_audio_state_t *audio = self->dvi_audio;
    if (!audio) {
        return;
    }
    memset(audio->slot_samples, 0, sizeof(audio->slot_samples));
}

// Sends the rate at rate_index in dvi_audio_rates from the next slot written.
// The packets already written ahead of DMA keep the old rate.
static void picodvi_framebuffer_audio_set_rate(picodvi_framebuffer_obj_t *self, size_t rate_index) {
    self->dvi_audio->next_rate_index = rate_index;
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

// Runs in the background after every frame and every few milliseconds, and
// once from play() and resume(). Points every slot DMA has finished with at
// the next packet: from the stage, or silence when nothing is playing, so
// the timing carries on.
static void audioout_refill(picodvi_audioout_obj_t *self) {
    dvi_audio_state_t *audio = self->framebuffer->dvi_audio;
    if (self->refilling || !audio) {
        return;
    }
    self->refilling = true;
    // Stage frames already sent to slots.
    size_t offset = 0;
    while (true) {
        // stop() may empty the stage while a sample buffer is fetched.
        offset = MIN(offset, self->stage_frames);
        bool playing = self->sample != MP_OBJ_NULL && !self->paused;
        if (playing && self->stage_frames - offset < 4) {
            // Move what is left to the front, then top the stage up.
            self->stage_frames -= offset;
            memmove(self->stage, &self->stage[offset * 2], self->stage_frames * 4);
            offset = 0;
            audioout_stage(self);
        }
        size_t available = playing ? self->stage_frames - offset : 0;
        // A sample still playing may have more frames later.
        bool wait = playing && available == 0 && !self->source_done;
        size_t used;
        if (!dvi_audio_fill_slot(self->framebuffer, audio, &self->stage[offset * 2], available, wait, &used)) {
            break;
        }
        offset += used;
    }
    offset = MIN(offset, self->stage_frames);
    self->stage_frames -= offset;
    memmove(self->stage, &self->stage[offset * 2], self->stage_frames * 4);
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
    if (active_picodvi == NULL || active_picodvi->audioout == MP_OBJ_NULL) {
        return;
    }
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(active_picodvi->audioout);
    if (is_board_audioout(self)) {
        // Keep supervisor.runtime.audio for the next VM. Drop the sample, which is on
        // the VM heap.
        common_hal_picodvi_audioout_stop(self);
        return;
    }
    common_hal_picodvi_audioout_deinit(self);
}

void picodvi_audioout_collect_ptrs(picodvi_framebuffer_obj_t *framebuffer) {
    if (framebuffer->audioout == MP_OBJ_NULL) {
        return;
    }
    gc_collect_ptr(framebuffer->audioout);
    // supervisor.runtime.audio is not on the heap, so the GC does not trace into it.
    picodvi_audioout_obj_t *self = MP_OBJ_TO_PTR(framebuffer->audioout);
    gc_collect_ptr(self->sample);
}

#if defined(DEFAULT_DVI_BUS_CLK_DP)
bool picodvi_audioout_construct_board(picodvi_framebuffer_obj_t *framebuffer) {
    picodvi_audioout_obj_t *self = &picodvi_board_audioout;
    if (framebuffer->output_width != 640 || framebuffer->audioout != MP_OBJ_NULL) {
        return false;
    }
    if (!dvi_audio_try_allocate(framebuffer)) {
        return false;
    }
    memset(self, 0, sizeof(*self));
    self->base.type = &picodvi_audioout_type;
    self->framebuffer = framebuffer;
    self->sample = MP_OBJ_NULL;
    // The framebuffer schedules its refills, which send silence until a
    // sample plays.
    framebuffer->audioout = MP_OBJ_FROM_PTR(self);
    return true;
}

mp_obj_t common_hal_supervisor_runtime_get_audio(void) {
    if (picodvi_board_audioout.base.type != &picodvi_audioout_type) {
        return mp_const_none;
    }
    return MP_OBJ_FROM_PTR(&picodvi_board_audioout);
}
#endif

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
    if (is_board_audioout(self)) {
        // Nothing can construct it again until the next reset.
        self->base.type = &mp_type_NoneType;
    }
}

void common_hal_picodvi_audioout_play(picodvi_audioout_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_base_t *base = audiosample_check(sample);
    size_t rate_index = 0;
    while (rate_index < DVI_AUDIO_RATE_COUNT && dvi_audio_rates[rate_index].sample_rate != base->sample_rate) {
        rate_index++;
    }
    if (rate_index == DVI_AUDIO_RATE_COUNT) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_sample_rate);
    }
    mp_arg_validate_int_range(base->channel_count, 1, 2, MP_QSTR_channel_count);
    if (base->bits_per_sample != 8 && base->bits_per_sample != 16) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), MP_QSTR_bits_per_sample);
    }
    common_hal_picodvi_audioout_stop(self);
    picodvi_framebuffer_audio_set_rate(self->framebuffer, rate_index);
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
    // Drops what was staged.
    self->stage_frames = 0;
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
