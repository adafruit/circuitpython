# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Record a longer I2S audio capture to a WAV file on an SD card.
#
# Produces /sd/recording.wav. Set left_justified=True for SPH0645LM4H mics.

import array
import struct
import time

import ulab.numpy as np
import board
import busio
import sdcardio
import storage
import audioi2sin

# ---- Recording config ------------------------------------------------------
SAMPLE_RATE = 16000
RECORD_SECONDS = 10
CHUNK_SAMPLES = 1024  # samples captured per record() call
OUTPUT_PATH = "/sd/talk.wav"

# ---- Mount SD --------------------------------------------------------------
spi = board.SPI()
sdcard = sdcardio.SDCard(spi, cs=board.D10, baudrate=24_000_000)
vfs = storage.VfsFat(sdcard)
storage.mount(vfs, "/sd")

# ---- Mic -------------------------------------------------------------------
# 24-bit MEMS mics ride in 32-bit slots. Downconvert each slot to a
# signed 16-bit PCM sample before writing.
mic = audioi2sin.I2SIn(
    bit_clock=board.D5,
    word_select=board.D6,
    data=board.D9,
    sample_rate=SAMPLE_RATE,
    bit_depth=32,
    mono=True,
    left_justified=False,  # True for SPH0645LM4H
)

actual_rate = mic.sample_rate
print("Recording at", actual_rate, "Hz for", RECORD_SECONDS, "s ->", OUTPUT_PATH)

raw = array.array("i", [0] * CHUNK_SAMPLES)
pcm16 = array.array("h", [0] * CHUNK_SAMPLES)


def write_wav_header(f, sample_rate, num_samples, bits_per_sample=16, channels=1):
    byte_rate = sample_rate * channels * bits_per_sample // 8
    block_align = channels * bits_per_sample // 8
    data_size = num_samples * block_align
    f.write(b"RIFF")
    f.write(struct.pack("<I", 36 + data_size))
    f.write(b"WAVE")
    f.write(b"fmt ")
    f.write(
        struct.pack(
            "<IHHIIHH",
            16,  # fmt chunk size
            1,  # PCM
            channels,
            sample_rate,
            byte_rate,
            block_align,
            bits_per_sample,
        )
    )
    f.write(b"data")
    f.write(struct.pack("<I", data_size))


total_samples = actual_rate * RECORD_SECONDS
written = 0

# Open with a write+read header placeholder; we'll seek back at the end to
# patch in the real sample count.
with open(OUTPUT_PATH, "wb") as f:
    write_wav_header(f, actual_rate, 0)  # placeholder; rewritten below
    start = time.monotonic()

    while written < total_samples:
        n = mic.record(raw, CHUNK_SAMPLES)
        # Convert 24-bit-in-32-bit slots to signed int16. Top 16 bits of
        # each slot are the high bits of the signed 24-bit sample, which
        # is already a serviceable 16-bit PCM representation.
        for i in range(n):
            v = raw[i]
            s = v >> 16  # take top 16 bits
            pcm16[i] = s
        # Write only the valid portion.
        f.write(memoryview(pcm16)[:n])
        written += n

    elapsed = time.monotonic() - start
    # Rewrite header now that we know the true sample count.
    f.seek(0)
    write_wav_header(f, actual_rate, written)

storage.umount("/sd")

print("Done. Wrote", written, "samples in", round(elapsed, 2), "s")
