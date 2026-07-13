// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

// A streaming WAV *sink*: it consumes an audiosample source (a mic, synthio,
// or an effect chain) and writes the resulting PCM to a file. Unlike WaveFile
// it is NOT an audiosample, it is the thing that drives a source, playing the
// role an AudioOut would.
typedef struct _audiofilewriter_audiofilewriter_obj_t {
    mp_obj_base_t base;

    // Output stream (anything with write + MP_STREAM_SEEK ioctl: a file or a
    // BytesIO). Held so it stays referenced while recording.
    mp_obj_t file;
    // The source being recorded. Only valid (and referenced) while playing.
    mp_obj_t sample;

    // Format, captured from the source at play() time. AudioFileWriter is the
    // format authority for the WAV header.
    uint32_t sample_rate;
    uint8_t channel_count;
    uint8_t bits_per_sample;
    bool samples_signed;
    uint8_t bytes_per_frame;      // channel_count * bits_per_sample / 8
    uint32_t source_max_buffer;   // largest buffer the source can hand back, bytes

    // RAM ring that decouples SD-write latency from the source. Written by the
    // pump, drained to the file by the pump. Only touched from background-task
    // context (never an ISR), so no locking is needed.
    uint8_t *ring;
    uint32_t ring_size;           // capacity in bytes
    uint32_t ring_head;           // write cursor
    uint32_t ring_tail;           // read cursor
    uint32_t ring_count;          // bytes currently buffered

    // Real-time pacing: budget accrues sample_rate frames per second of elapsed
    // supervisor ticks; a buffer is pulled only when budget is positive.
    int64_t budget_frames;
    uint64_t last_tick_ms;

    // Absolute file offset of the RIFF header start, so stop() can seek back and
    // patch the two size fields once the final length is known.
    uint32_t header_offset;
    uint32_t data_bytes;          // total PCM bytes handed to the file

    bool playing;
    bool source_done;             // source returned DONE/ERROR; drain then finalize

    // Intrusive linked list of active writers, walked once per supervisor tick.
    struct _audiofilewriter_audiofilewriter_obj_t *reg_next;
} audiofilewriter_audiofilewriter_obj_t;

// Called once per supervisor tick (from supervisor_background_tick), in
// background-task context. Pumps every active writer.
void audiofilewriter_background(void);

// Called during soft reset to abandon any writer left recording.
void audiofilewriter_reset(void);
