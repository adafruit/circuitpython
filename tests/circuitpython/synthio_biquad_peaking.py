# A peaking EQ bell should be unity at DC and near Nyquist, and hit its own
# gain at the centre frequency.
import array
import math
from audiocore import get_buffer, RawSample
from audiofilters import Filter
from synthio import Biquad, FilterMode

RATE = 48000
FRAMES = 4096
LEVEL = 2000
GAIN_DB = 6.0
CENTRE = 1000.0


def through(values):
    source = RawSample(values, sample_rate=RATE, channel_count=1)
    bell = Biquad(FilterMode.PEAKING_EQ, CENTRE, Q=1.0, A=10 ** (GAIN_DB / 40))
    effect = Filter(
        filter=bell,
        sample_rate=RATE,
        channel_count=1,
        bits_per_sample=16,
        samples_signed=True,
        buffer_size=FRAMES * 2,
    )
    effect.play(source, loop=True)
    out = []
    for block in range(3):
        data = bytes(get_buffer(effect)[1])
        if block < 2:  # let the filter settle
            continue
        for index in range(0, len(data) - 1, 2):
            sample = data[index] | (data[index + 1] << 8)
            out.append(sample - 65536 if sample >= 32768 else sample)
    return out


def tone(hz):
    return array.array(
        "h",
        [int(LEVEL * math.sin(2 * math.pi * hz * frame / RATE)) for frame in range(FRAMES)],
    )


def rms_db(values):
    total = sum(value * value for value in values)
    return 20 * math.log10(math.sqrt(total / len(values)) / (LEVEL / math.sqrt(2)))


dc = through(array.array("h", [LEVEL] * FRAMES))
print("DC      %+.1f" % (20 * math.log10(abs(dc[-1]) / LEVEL)))
print("centre  %+.1f" % rms_db(through(tone(CENTRE))))
print("12 kHz  %+.1f" % rms_db(through(tone(12000.0))))
