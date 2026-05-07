import audiocore
import audioio
import board
import digitalio
import array
import time
import math

# Optional trigger pin for oscilloscope synchronisation.
try:
    trigger = digitalio.DigitalInOut(board.D4)
    trigger.switch_to_output(True)
except AttributeError:
    trigger = None

# Generate one period of a 440 Hz sine wave. All four samples use the same
# sample rate so each plays the same audible pitch — the test is comparing
# the four format-conversion paths, not playback rates.
sample_rate = 8000
length = sample_rate // 440  # samples per cycle

sample_names = ["unsigned 8 bit", "signed 8 bit", "unsigned 16 bit", "signed 16 bit"]

# unsigned 8 bit
u8 = array.array("B", [0] * length)
for i in range(length):
    u8[i] = int(math.sin(math.pi * 2 * i / length) * 127 + 128)
samples = [audiocore.RawSample(u8, sample_rate=sample_rate)]

# signed 8 bit
s8 = array.array("b", [0] * length)
for i in range(length):
    s8[i] = int(math.sin(math.pi * 2 * i / length) * 127)
samples.append(audiocore.RawSample(s8, sample_rate=sample_rate))

# unsigned 16 bit
u16 = array.array("H", [0] * length)
for i in range(length):
    u16[i] = int(math.sin(math.pi * 2 * i / length) * 32767 + 32768)
samples.append(audiocore.RawSample(u16, sample_rate=sample_rate))

# signed 16 bit
s16 = array.array("h", [0] * length)
for i in range(length):
    s16[i] = int(math.sin(math.pi * 2 * i / length) * 32767)
samples.append(audiocore.RawSample(s16, sample_rate=sample_rate))

dac = audioio.AudioOut(board.A0)
for sample, name in zip(samples, sample_names):
    print(name)
    if trigger:
        trigger.value = False
    dac.play(sample, loop=True)
    time.sleep(1)
    dac.stop()
    time.sleep(0.1)
    if trigger:
        trigger.value = True
    print()

dac.deinit()
print("done")
