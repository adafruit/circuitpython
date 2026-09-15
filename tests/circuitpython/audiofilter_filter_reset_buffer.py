# audiofilters.Filter.reset_buffer() should leave no audio behind: a biquad
# reset should clear both its input and output history, not just the input.
import array
import math
from audiocore import get_buffer, reset_buffer, RawSample
from audiofilters import Filter
from synthio import Biquad, FilterMode

RATE = 48000
FRAMES = 1024


def s16(data):
    out = []
    for index in range(0, len(data) - 1, 2):
        value = data[index] | (data[index + 1] << 8)
        out.append(value - 65536 if value >= 32768 else value)
    return out


loud = array.array(
    "h", [int(30000 * math.sin(2 * math.pi * 200 * frame / RATE)) for frame in range(FRAMES)]
)
silence = array.array("h", [0] * FRAMES)

biquad = Biquad(FilterMode.LOW_PASS, 800.0)
node = Filter(
    filter=biquad,
    sample_rate=RATE,
    channel_count=1,
    bits_per_sample=16,
    samples_signed=True,
    buffer_size=FRAMES * 2,
)

node.play(RawSample(loud, sample_rate=RATE, channel_count=1), loop=True)
for _ in range(4):
    get_buffer(node)  # fill the filter's memory

node.play(RawSample(silence, sample_rate=RATE, channel_count=1), loop=True)
reset_buffer(node)  # what an AudioOut does on play()
first = s16(bytes(get_buffer(node)[1]))
print("peak after reset_buffer:", max(abs(value) for value in first))
