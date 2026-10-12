# SPDX-FileCopyrightText: 2026 Adafruit Industries
#
# SPDX-License-Identifier: MIT
# Creates, plays and deinitializes an AudioOut many times. Each one reserves
# over 100 KB of internal RAM, so a leak runs out of memory within a few
# rounds. Pin names are for the Fruit Jam and Metro RP2350.
import array
import math
import time

import audiocore
import board
import displayio
import picodvi

displayio.release_displays()
fb = picodvi.Framebuffer(
    320,
    240,
    clk_dp=board.CKP,
    clk_dn=board.CKN,
    red_dp=board.D0P,
    red_dn=board.D0N,
    green_dp=board.D1P,
    green_dn=board.D1N,
    blue_dp=board.D2P,
    blue_dn=board.D2N,
    color_depth=8,
)
tone = audiocore.RawSample(
    array.array("h", [int(math.sin(math.pi * 2 * i / 48) * 8000) for i in range(48)]),
    sample_rate=48000,
)

for i in range(25):
    audio = picodvi.AudioOut(fb)
    audio.play(tone, loop=True)
    time.sleep(0.1)
    audio.deinit()
print("25 rounds ok")

with picodvi.AudioOut(fb) as audio:
    audio.play(tone, loop=True)
    time.sleep(0.5)
    print("context manager playing", audio.playing)
print("done")
