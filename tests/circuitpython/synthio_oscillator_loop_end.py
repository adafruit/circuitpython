# A note whose loop end is reached exactly (accum == lim) should wrap back
# to the loop start on that same sample, not read one sample past the loop.
import array
from audiocore import get_buffer
from synthio import Note, Synthesizer

RATE = 8000
LOOP = 256

# 512 samples: the loop is [0, 256) and is silent; sample 256 is a marker
# the oscillator should never reach.
table = array.array("h", [0] * 512)
table[LOOP] = 30000

synth = Synthesizer(sample_rate=RATE, channel_count=1)
note = Note(
    frequency=RATE / LOOP,  # exactly one table step per output sample
    waveform=table,
    waveform_loop_start=0,
    waveform_loop_end=LOOP,
    envelope=None,
)
synth.press(note)

out = []
for _ in range(4):
    data = bytes(get_buffer(synth)[1])
    for index in range(0, len(data) - 1, 2):
        value = data[index] | (data[index + 1] << 8)
        out.append(value - 65536 if value >= 32768 else value)

hits = [i for i, v in enumerate(out) if abs(v) > 1000]
print("marker hits:", hits)
