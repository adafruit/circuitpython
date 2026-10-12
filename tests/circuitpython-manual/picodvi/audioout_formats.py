# SPDX-FileCopyrightText: 2026 Adafruit Industries
#
# SPDX-License-Identifier: MIT
# Plays a 1 kHz tone over DVI in every supported sample format. Listen on a
# display with speakers. Pin names are for the Fruit Jam and Metro RP2350.
import array
import math
import time

import audiocore
import audiomixer
import board
import displayio
import picodvi
import synthio

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
audio = picodvi.AudioOut(fb)
time.sleep(3)  # Most displays take a few seconds to lock on to a new signal.

length = 48  # One period of 1 kHz at 48 kHz.
for typecode in ("B", "b", "H", "h"):
    for channel_count in (1, 2):
        bits = 8 if typecode in "Bb" else 16
        full = 2 ** (bits - 1) - 1
        offset = full + 1 if typecode in "BH" else 0
        values = []
        for i in range(length):
            value = int(math.sin(math.pi * 2 * i / length) * full / 4) + offset
            values.extend([value] * channel_count)
        sample = audiocore.RawSample(
            array.array(typecode, values), sample_rate=48000, channel_count=channel_count
        )
        print(typecode, bits, "bit", "stereo" if channel_count == 2 else "mono")
        audio.play(sample, loop=True)
        time.sleep(1)
        print("  playing", audio.playing)
        audio.stop()
        time.sleep(0.2)

tone = audiocore.RawSample(
    array.array("h", [int(math.sin(math.pi * 2 * i / length) * 8000) for i in range(length)]),
    sample_rate=48000,
)

print("mixer")
mixer = audiomixer.Mixer(voice_count=1, sample_rate=48000, channel_count=1, buffer_size=2048)
audio.play(mixer)
mixer.voice[0].play(tone, loop=True)
time.sleep(1)
audio.stop()

print("synthio")
synth = synthio.Synthesizer(sample_rate=48000)
audio.play(synth)
synth.press(69)
time.sleep(1)
audio.stop()

print("pause and resume")
audio.play(tone, loop=True)
time.sleep(1)
audio.pause()
print("  paused", audio.paused)
time.sleep(1)
audio.resume()
print("  paused", audio.paused)
time.sleep(1)
audio.stop()

audio.deinit()
print("done")
