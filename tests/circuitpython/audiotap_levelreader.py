import array
from audiocore import get_buffer, RawSample
from audiotap import LevelReader, Tap

ramp = array.array("h", range(-300, 300, 3))  # 200 samples
source = RawSample(ramp, sample_rate=8000)


def output(sample, times=1):
    # Stands in for an audio output, which pulls the audio through the tap
    # and plays it. Returns what it played.
    out = []
    for _ in range(times):
        result, buffer = get_buffer(sample)
        out.extend(buffer)
    return out


# Before any audio passes, the levels are 0.
tap = Tap(capacity=0.125)
levels = LevelReader(tap)
print("idle", levels.peak, levels.rms)

# Peak and RMS are measured separately, each since it was last read.
tap.play(source, loop=True)
output(tap, 4)
print("peak", round(levels.peak, 4), levels.peak)
output(tap)
print("rms", round(levels.rms, 4), levels.rms)
print("peak again", round(levels.peak, 4))

# Measuring does not take audio from readinto.
copy = array.array("h", [0] * 1000)
print("read", tap.readinto(copy), list(copy[:200]) == list(ramp))

# A tap has one LevelReader at a time.
try:
    LevelReader(tap)
except ValueError as error:
    print("second", error)

# deinit detaches it, and audio that passes after that is not measured.
levels.deinit()
levels.deinit()
try:
    levels.peak
except ValueError as error:
    print("deinit", error)
output(tap)
with LevelReader(tap) as again:
    print("not measured", again.peak, again.rms)
    output(tap)
    print("measured", round(again.peak, 4))
print("detached by exit", LevelReader(tap).peak)

# Levels only: a tap that keeps no audio.
meter = Tap(capacity=0)
meter_levels = LevelReader(meter)
meter.play(RawSample(array.array("h", [16384, -32768] * 16), sample_rate=8000), loop=True)
output(meter)
print("meter", meter.available, meter_levels.peak, meter_levels.rms)

# 8 bit unsigned levels: 0x80 is silence.
eight = Tap(capacity=0.002)
eight_levels = LevelReader(eight)
eight.play(RawSample(array.array("B", [0x80, 0xC0] * 8), sample_rate=8000), loop=True)
output(eight)
print("8 bit", eight.bits_per_sample, eight_levels.peak, eight_levels.rms)

# Only a Tap can be measured.
try:
    LevelReader(source)
except TypeError as error:
    print("not a tap", error)

# A deinitialized tap can't be measured.
meter.deinit()
try:
    meter_levels.peak
except ValueError as error:
    print("tap deinit", error)
try:
    LevelReader(meter)
except ValueError as error:
    print("new on deinit tap", error)
