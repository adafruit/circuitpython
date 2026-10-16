# SPDX-FileCopyrightText: 2026 Adafruit Industries
#
# SPDX-License-Identifier: MIT
# Plays a 1 kHz tone over DVI at each supported sample rate, twice round.
# Listen on a display with speakers: the pitch should not change. Pin names
# are for the Fruit Jam, Metro RP2350 and Feather RP2350.
import array
import math
import time

import audiocore
import board
import displayio
import picodvi

PINS = dict(
    clk_dp=board.CKP,
    clk_dn=board.CKN,
    red_dp=board.D0P,
    red_dn=board.D0N,
    green_dp=board.D1P,
    green_dn=board.D1N,
    blue_dp=board.D2P,
    blue_dn=board.D2N,
)

displayio.release_displays()
fb = picodvi.Framebuffer(320, 240, color_depth=8, **PINS)
audio = picodvi.AudioOut(fb)
time.sleep(3)  # Most displays take a few seconds to lock on to a new signal.

for rate in (32000, 44100, 48000) * 2:
    length = rate // 100  # Ten periods of 1 kHz.
    tone = audiocore.RawSample(
        array.array("h", [int(math.sin(math.pi * 20 * i / length) * 8000) for i in range(length)]),
        sample_rate=rate,
    )
    audio.play(tone, loop=True)
    time.sleep(2)
    print(rate, "playing", audio.playing)
    audio.stop()
    time.sleep(0.5)
print("done")
