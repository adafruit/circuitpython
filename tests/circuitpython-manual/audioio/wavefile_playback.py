import audiocore
import audioio
import board
import digitalio
import time
import os

# Optional trigger pin for oscilloscope synchronisation.
try:
    trigger = digitalio.DigitalInOut(board.D4)
    trigger.switch_to_output(True)
except AttributeError:
    trigger = None

# List WAV files from the audiocore test samples directory on the device.
# Copy tests/circuitpython-manual/audiocore/*.wav to the board's filesystem.
sample_prefix = "jeplayer-splash"

samples = []
for fn in os.listdir("/"):
    if fn.startswith(sample_prefix):
        samples.append(fn)

if not samples:
    print("No sample files found. Copy *.wav files from tests/circuitpython-manual/audiocore/ to the board.")

dac = audioio.AudioOut(board.A0)
for filename in sorted(samples):
    print("playing", filename)
    with open(filename, "rb") as sample_file:
        try:
            sample = audiocore.WaveFile(sample_file)
        except OSError as e:
            print(e)
            continue
        if trigger: trigger.value = False
        dac.play(sample)
        while dac.playing:
            time.sleep(0.1)
        if trigger: trigger.value = True
    time.sleep(0.1)
    print()

dac.deinit()
print("done")
