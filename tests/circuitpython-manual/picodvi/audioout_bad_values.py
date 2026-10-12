# SPDX-FileCopyrightText: 2026 Adafruit Industries
#
# SPDX-License-Identifier: MIT
# Checks that picodvi.AudioOut rejects what it does not support. Every line
# should print an exception. Pin names are for the Fruit Jam and Metro RP2350.
import array

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


def check(name, fun):
    try:
        fun()
        print(name, "-> no error (unexpected)")
    except Exception as e:
        print(name, "->", type(e).__name__, e)


displayio.release_displays()

fb = picodvi.Framebuffer(720, 400, color_depth=8, **PINS)
check("720-wide framebuffer", lambda: picodvi.AudioOut(fb))
fb.deinit()

fb = picodvi.Framebuffer(320, 240, color_depth=8, **PINS)
audio = picodvi.AudioOut(fb)
silence = array.array("h", [0] * 48)
check("44.1 kHz sample", lambda: audio.play(audiocore.RawSample(silence, sample_rate=44100)))
check("32 kHz sample", lambda: audio.play(audiocore.RawSample(silence, sample_rate=32000)))
check("second AudioOut", lambda: picodvi.AudioOut(fb))
check("pause when stopped", audio.pause)

audio.deinit()
check("play after deinit", lambda: audio.play(audiocore.RawSample(silence, sample_rate=48000)))

audio = picodvi.AudioOut(fb)
fb.deinit()
check("play after framebuffer deinit", lambda: audio.play(audiocore.RawSample(silence)))
print("done")
