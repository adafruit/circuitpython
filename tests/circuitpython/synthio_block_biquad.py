from synthnotehelper import *
from synthio import BlockBiquad, FilterMode
import random

random.seed(41)

white_noise = array.array("h", [random.randint(-32000, 32000) for i in range(600)])


@synth_test_rms
def gen(synth):
    l = LFO(sweep, offset=1440, scale=2880, rate=0.025, once=True)
    yield [l]
    b = BlockBiquad(FilterMode.LOW_PASS, l, Q=0.5**0.5)
    n = Note(100, filter=b, waveform=white_noise)
    synth.press(n)
    yield 20
