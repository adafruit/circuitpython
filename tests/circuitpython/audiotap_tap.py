import array
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from audiotap import Tap

ramp = array.array("h", range(-300, 300, 3))  # 200 samples
source = RawSample(ramp, sample_rate=8000)


def pull(sample, times=1):
    out = []
    for _ in range(times):
        result, buffer = get_buffer(sample)
        out.extend(buffer)
    return out


# Audio passes through unchanged and a copy can be read.
tap = Tap(capacity=1000, buffer_size=128, sample_rate=8000)
print("idle", tap.playing, tap.available, tap.levels())
print("idle silence", set(pull(tap)), tap.available)
tap.play(source, loop=True)
print("playing", tap.playing)
played = pull(tap, 4)
print("played", len(played), played[:3], played == list(ramp) + list(ramp)[:56])
print("available", tap.available)
copy = array.array("h", [0] * 300)
print("read", tap.readinto(copy), list(copy[:256]) == played, tap.available)
print("read empty", tap.readinto(copy))

# Levels of the audio since the last call.
peak, rms = tap.levels()
print("levels", round(peak, 4), round(rms, 4))
print("levels again", tap.levels())

# Overflow keeps the oldest audio and drops the newer.
small = Tap(capacity=100, buffer_size=128, sample_rate=8000)
small.play(source, loop=True)
played = pull(small, 2)
print("full", small.available, small.overflow, small.overflow)
copy = array.array("h", [0] * 100)
print("kept oldest", small.readinto(copy), list(copy) == played[:100])
pull(small)
small.clear()
print("cleared", small.available)

# A bytearray holds half as many 16 bit samples, and memoryview slices work.
pull(small)
raw = bytearray(10)
print("bytearray", small.readinto(raw), small.available)
part = array.array("h", [7] * 8)
print("slice", small.readinto(memoryview(part)[2:5]), list(part))

# Levels only.
meter = Tap(capacity=0, buffer_size=64, sample_rate=8000)
meter.play(RawSample(array.array("h", [16384, -32768] * 16), sample_rate=8000), loop=True)
pull(meter)
print("meter", meter.capacity, meter.readinto(copy), meter.levels())

# Stereo reads whole frames.
stereo = Tap(capacity=64, buffer_size=32, sample_rate=8000, channel_count=2)
stereo.play(RawSample(array.array("h", range(16)), sample_rate=8000, channel_count=2), loop=True)
pull(stereo)
odd = array.array("h", [0] * 5)
print("stereo", stereo.readinto(odd), list(odd))

# 8 bit unsigned levels: 0x80 is silence.
eight = Tap(capacity=16, buffer_size=16, sample_rate=8000, bits_per_sample=8, samples_signed=False)
eight.play(RawSample(array.array("B", [0x80, 0xC0] * 8), sample_rate=8000), loop=True)
pull(eight)
print("8 bit", eight.levels())

# In a chain, and the end of a sample that does not loop.
once = Tap(capacity=1000, buffer_size=128, sample_rate=8000)
chain = Filter(buffer_size=128, sample_rate=8000)
chain.play(once)
once.play(source)
pull(chain, 6)
print("ended", once.playing, once.available)

once.deinit()
try:
    once.readinto(copy)
except ValueError as error:
    print("deinit", error)
