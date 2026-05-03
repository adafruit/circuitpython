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

sample_prefix = "jeplayer-splash"

samples = []
for fn in os.listdir("/"):
    if fn.startswith(sample_prefix):
        samples.append(fn)

if not samples:
    print("No sample files found. Copy *.wav files from tests/circuitpython-manual/audiocore/ to the board.")

dac = audioio.AudioOut(board.A0)
for filename in sorted(samples):
    print("playing with pause/resume:", filename)
    with open(filename, "rb") as sample_file:
        try:
            sample = audiocore.WaveFile(sample_file)
        except OSError as e:
            print(e)
            continue
        if trigger: trigger.value = False
        dac.play(sample)
        # Deliberately toggle pause/resume every 100 ms to stress-test the
        # pause/resume cycle. Audio will sound choppy — that is expected.
        # The wall-clock guard makes the test fail loudly instead of hanging
        # if pause() ever leaves the driver in a state where playing never
        # clears. 30 s is enough for the longest 44.1 kHz mono WAV in the
        # set even with pause-doubling.
        deadline = time.monotonic() + 30.0
        while dac.playing:
            if time.monotonic() > deadline:
                print("  TIMEOUT waiting for playback to finish")
                dac.stop()
                break
            time.sleep(0.1)
            if not dac.playing:  # may have finished during sleep
                break
            if not dac.paused:
                dac.pause()
                print("  paused")
            else:
                dac.resume()
                print("  resumed")
        if trigger: trigger.value = True
    time.sleep(0.1)
    print()

dac.deinit()
print("done")
