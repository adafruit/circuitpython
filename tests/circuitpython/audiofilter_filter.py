from audiofilters import Filter
from audiofilterhelper import synth_test, sine8k


@synth_test
def basic_filter():
    effect = Filter(
        bits_per_sample=16,
        samples_signed=True,
    )
    yield effect, []

    effect.play(sine8k, loop=True)
    yield 4

    effect.stop()
    yield 2
