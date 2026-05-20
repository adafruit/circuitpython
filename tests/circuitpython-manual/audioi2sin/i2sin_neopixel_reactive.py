# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Audio-reactive NeoPixel effect driven by an I2S MEMS microphone.
#
# Wiring (defaults assume an INMP441 / SPH0645 style mic):
#   mic BCLK  -> board.D5
#   mic LRCL  -> board.D6
#   mic DOUT  -> board.D9
#   neopixel  -> board.D10
#
# The mic's 24-bit samples ride in 32-bit slots, so we use bit_depth=32 and
# an array.array("I", ...). For SPH0645 set left_justified=True.

import array
import math
import time

import board
import audioi2sin
import neopixel

NUM_PIXELS = 30
PIXEL_PIN = board.D10

SAMPLE_RATE = 16000
SAMPLES_PER_FRAME = 512  # ~32 ms windows

pixels = neopixel.NeoPixel(PIXEL_PIN, NUM_PIXELS, brightness=0.3, auto_write=False)

mic = audioi2sin.I2SIn(
    bit_clock=board.D5,
    word_select=board.D6,
    data=board.D9,
    sample_rate=SAMPLE_RATE,
    bit_depth=32,
    mono=True,
    left_justified=False,  # set True for SPH0645LM4H
)

buf = array.array("i", [0] * SAMPLES_PER_FRAME)


def wheel(pos):
    pos = pos % 256
    if pos < 85:
        return (pos * 3, 255 - pos * 3, 0)
    if pos < 170:
        pos -= 85
        return (255 - pos * 3, 0, pos * 3)
    pos -= 170
    return (0, pos * 3, 255 - pos * 3)


# Smoothed noise floor + peak so the effect adapts to the room.
noise_floor = 2000.0
peak = 20000.0
hue = 0
smoothed_level = 0.0

while True:
    mic.record(buf, len(buf))

    # Compute RMS of the window.
    acc = 0
    for s in buf:
        acc += s * s
    rms = math.sqrt(acc / len(buf))

    # Track a slow noise floor and a decaying peak for auto-gain.
    noise_floor = 0.995 * noise_floor + 0.005 * rms
    if rms > peak:
        peak = rms
    else:
        peak *= 0.995
    if peak < noise_floor + 1000:
        peak = noise_floor + 1000

    level = (rms - noise_floor) / (peak - noise_floor)
    if level < 0:
        level = 0.0
    elif level > 1:
        level = 1.0

    # Smooth the bar so it doesn't jitter on every frame.
    smoothed_level = 0.6 * smoothed_level + 0.4 * level

    lit = int(smoothed_level * NUM_PIXELS)
    hue = (hue + 2) % 256

    for i in range(NUM_PIXELS):
        if i < lit:
            r, g, b = wheel((hue + i * (256 // NUM_PIXELS)) % 256)
            pixels[i] = (r, g, b)
        else:
            pixels[i] = (0, 0, 0)
    pixels.show()

    time.sleep(0.005)
