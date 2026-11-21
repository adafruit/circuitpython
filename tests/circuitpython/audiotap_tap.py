import array
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from audiotap import Tap

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


# Before a sample is played, the tap has no format and keeps nothing.
tap = Tap(capacity=0.125)
print("idle", tap.playing, tap.available)

# The tap takes the format of the first sample it plays and keeps 125 ms of it.
tap.play(source, loop=True)
print("playing", tap.playing, tap.sample_rate, tap.channel_count, tap.bits_per_sample)

# Audio passes through to the output unchanged, and the tap keeps a copy.
played = output(tap, 4)
print("played", len(played), played[:3], played == list(ramp) * 4)
print("available", tap.available)
copy = array.array("h", [0] * 1200)
print("read", tap.readinto(copy), list(copy[:800]) == played, tap.available)
print("read empty", tap.readinto(copy))

# Levels are measured by a LevelReader (audiotap_levelreader.py), not the tap.
print("no levels", hasattr(tap, "peak"), hasattr(tap, "rms"))

# Later samples must match the first one's format.
try:
    tap.play(RawSample(ramp, sample_rate=16000))
except ValueError as error:
    print("mismatch", error)

# A later sample's longer buffers are passed on in pieces no longer than the first's.
short = Tap()
short.play(RawSample(array.array("h", range(100)), sample_rate=8000))
short.play(RawSample(array.array("h", range(250)), sample_rate=8000))
print("pieces", [len(output(short)) for _ in range(3)], short.playing)

# Overflow keeps the oldest audio and drops the newer.
small = Tap(capacity=0.01)  # 80 samples
small.play(source, loop=True)
played = output(small)
print("full", small.available, small.overflow, small.overflow)
copy = array.array("h", [0] * 100)
print("kept oldest", small.readinto(copy), list(copy[:80]) == played[:80])
output(small)
small.clear()
print("cleared", small.available)

# A bytearray holds half as many 16 bit samples, and memoryview slices work.
output(small)
raw = bytearray(10)
print("bytearray", small.readinto(raw), small.available)
part = array.array("h", [7] * 8)
print("slice", small.readinto(memoryview(part)[2:5]), list(part))

# A tap that keeps nothing still passes audio through.
nothing = Tap(capacity=0)
nothing.play(source, loop=True)
print("keeps nothing", output(nothing) == list(ramp), nothing.available, nothing.readinto(copy))

# Stereo reads whole frames.
stereo = Tap(capacity=0.004)  # 32 frames
stereo.play(RawSample(array.array("h", range(16)), sample_rate=8000, channel_count=2), loop=True)
output(stereo)
odd = array.array("h", [0] * 5)
print("stereo", stereo.channel_count, stereo.readinto(odd), list(odd))

# In a chain, and the end of a sample that does not loop: then the tap plays silence.
once = Tap()
chain = Filter(buffer_size=128, sample_rate=8000)
chain.play(once.play(source))
output(chain, 6)
print("ended", once.playing, once.available)
print("silence", set(output(once)), once.available)

once.deinit()
try:
    once.readinto(copy)
except ValueError as error:
    print("deinit", error)
