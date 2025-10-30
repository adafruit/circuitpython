import sys

sys.path.insert(
    0, f"{__file__.rpartition('/')[0] or '.'}/../../../../frozen/Adafruit_CircuitPython_Wave"
)

import random
import audiocore
import synthio
from ulab import numpy as np
import adafruit_wave as wave

random.seed(9)

envelope = synthio.Envelope(
    attack_time=0.15, decay_time=0, release_time=0.08, attack_level=1.0, sustain_level=1.0
)

SAMPLE_SIZE = 1024
VOLUME = 14700
sine = np.array(
    np.sin(np.linspace(0, 2 * np.pi, SAMPLE_SIZE, endpoint=False)) * VOLUME,
    dtype=np.int16,
)
noise = np.array([random.randint(-VOLUME, VOLUME) for i in range(SAMPLE_SIZE)], dtype=np.int16)
bend_out = np.linspace(0, 32767, num=SAMPLE_SIZE, endpoint=True, dtype=np.int16)
sweep = np.linspace(-32767, 32767, num=SAMPLE_SIZE, endpoint=True, dtype=np.int16)

lfos_of_interest = []


def synthesize(synth):
    freq_sweep = synthio.LFO(
        sweep, offset=synthio.midi_to_hz(72), scale=synthio.midi_to_hz(72), rate=1, once=True
    )

    for biquad in (
        None,
        synthio.BlockBiquad(synthio.FilterMode.LOW_PASS, freq_sweep),
        synthio.BlockBiquad(synthio.FilterMode.HIGH_PASS, freq_sweep),
        synthio.BlockBiquad(synthio.FilterMode.BAND_PASS, freq_sweep, Q=8),
        synthio.BlockBiquad(synthio.FilterMode.NOTCH, freq_sweep, Q=8),
    ):
        n = synthio.Note(
            frequency=synthio.midi_to_hz(72),
            envelope=envelope,
            filter=biquad,
            waveform=sine,
        )

        freq_sweep.retrigger()
        synth.press(n)
        print("n", n.frequency)
        yield 24 * 6
        synth.release_all()
        yield 24


with wave.open("blockfilter.wav", "w") as f:
    f.setnchannels(1)
    f.setsampwidth(2)
    f.setframerate(48000)
    synth = synthio.Synthesizer(sample_rate=48000)
    for n in synthesize(synth):
        for i in range(n):
            result, data = audiocore.get_buffer(synth)
            f.writeframes(data)
