import audiocore
import audioio
import board
import digitalio
import array
import gc
import math
import time
import os

# Optional trigger pin for oscilloscope synchronisation.
try:
    trigger = digitalio.DigitalInOut(board.D4)
    trigger.switch_to_output(True)
except AttributeError:
    trigger = None

dac = audioio.AudioOut(board.A0, right_channel=board.A1)

# ---------------------------------------------------------------------------
# Channel-isolation tones: prove each DAC channel can be driven independently.
# Listener should hear the 440 Hz tone shift between ears.
# ---------------------------------------------------------------------------
sample_rate = 8000
freq = 440
length = sample_rate // freq
sine = [int(math.sin(2 * math.pi * i / length) * 16000) for i in range(length)]
silence = [0] * length


def stereo_buffer(left, right):
    buf = array.array("h", [0] * (len(left) * 2))
    for i in range(len(left)):
        buf[2 * i] = left[i]
        buf[2 * i + 1] = right[i]
    return buf


channel_tests = (
    ("left only", sine, silence),
    ("right only", silence, sine),
    ("both channels", sine, sine),
)

for label, left, right in channel_tests:
    print("channel test:", label)
    sample = audiocore.RawSample(
        stereo_buffer(left, right), channel_count=2, sample_rate=sample_rate
    )
    if trigger:
        trigger.value = False
    dac.play(sample, loop=True)
    time.sleep(1.0)
    dac.stop()
    if trigger:
        trigger.value = True
    time.sleep(0.2)
    del sample
    print()

del channel_tests, sine, silence

# ---------------------------------------------------------------------------
# Pan sweep: continuous equal-power L→R over 2 s. Single non-looped buffer,
# played once for a smooth crossfade with no DMA restart clicks.
#
# Linear amplitude pan sounds like centred stereo at the midpoint because both
# channels are equally loud. Equal-power (cos/sin) pan keeps total energy
# constant so the source is perceived as moving rather than collapsing inwards.
#
# 2 s @ 4 kHz stereo 8-bit signed = 16000 bytes. Halving the sample rate keeps
# the buffer small while doubling perceived motion duration; 220 Hz at 4 kHz
# has the same samples-per-cycle as the earlier 440 Hz @ 8 kHz tones.
# ---------------------------------------------------------------------------
gc.collect()
pan_sr = sample_rate // 2
pan_freq = freq // 2
pan_seconds = 3
pan_frames = pan_sr * pan_seconds
pan_buf = array.array("b", bytes(pan_frames * 2))
two_pi_freq_over_sr = 2 * math.pi * pan_freq / pan_sr
half_pi = math.pi / 2
for i in range(pan_frames):
    s = math.sin(two_pi_freq_over_sr * i)
    t = i / pan_frames  # 0 → 1
    l_gain = math.cos(t * half_pi)
    r_gain = math.sin(t * half_pi)
    pan_buf[2 * i] = int(s * l_gain * 120)
    pan_buf[2 * i + 1] = int(s * r_gain * 120)

print("pan sweep: left to right")
pan_sample = audiocore.RawSample(pan_buf, channel_count=2, sample_rate=pan_sr)
if trigger:
    trigger.value = False
dac.play(pan_sample)
while dac.playing:
    time.sleep(0.05)
if trigger:
    trigger.value = True
time.sleep(0.2)
print()

# ---------------------------------------------------------------------------
# Stereo WAV files: full-content check.
# ---------------------------------------------------------------------------
sample_prefix = "jeplayer-splash"
wavs = sorted(fn for fn in os.listdir("/") if fn.startswith(sample_prefix) and "stereo" in fn)

for filename in wavs:
    print("playing stereo:", filename)
    with open(filename, "rb") as sample_file:
        try:
            sample = audiocore.WaveFile(sample_file)
        except OSError as e:
            print(e)
            continue
        if trigger:
            trigger.value = False
        dac.play(sample)
        while dac.playing:
            time.sleep(0.1)
        if trigger:
            trigger.value = True
    time.sleep(0.1)
    print()

dac.deinit()
print("done")
