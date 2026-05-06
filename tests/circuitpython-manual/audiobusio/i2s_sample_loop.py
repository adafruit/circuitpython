# SPDX-FileCopyrightText: 2018 Kattni Rembor for Adafruit Industries
#
# SPDX-License-Identifier: MIT
import audiocore
import board
import digitalio
import array
import struct
import time
import math

trigger = digitalio.DigitalInOut(board.D4)
trigger.switch_to_output(True)

# Generate one period of sine wav.
sample_names = [
    "mono unsigned 8 bit",
    "stereo unsigned 8 bit",
    "mono signed 8 bit",
    "stereo signed 8 bit",
    "mono unsigned 16 bit",
    "stereo unsigned 16 bit",
    "mono signed 16 bit",
    "stereo signed 16 bit",
]
sample_config = {
    "mono unsigned 8 bit": {"format": "B", "channel_count": 1},
    "stereo unsigned 8 bit": {"format": "B", "channel_count": 2},
    "mono signed 8 bit": {"format": "b", "channel_count": 1},
    "stereo signed 8 bit": {"format": "b", "channel_count": 2},
    "mono unsigned 16 bit": {"format": "H", "channel_count": 1},
    "stereo unsigned 16 bit": {"format": "H", "channel_count": 2},
    "mono signed 16 bit": {"format": "h", "channel_count": 1},
    "stereo signed 16 bit": {"format": "h", "channel_count": 2},
}

for sample_rate in [8000, 16000, 32000, 44100]:
    print(f"{sample_rate / 1000} kHz")
    length = sample_rate // 440

    samples = []

    for name in sample_names:
        config = sample_config[name]
        format = config["format"]
        channel_count = config["channel_count"]
        length = sample_rate // 440
        values = []
        for i in range(length):
            range = 2 ** (struct.calcsize(format) * 8 - 1) - 1
            value = int(math.sin(math.pi * 2 * i / length) * range)
            if "unsigned" in name:
                value += range
            values.append(value)
            if channel_count == 2:
                values.append(value)
        sample = audiocore.RawSample(
            array.array(format, values), sample_rate=sample_rate, channel_count=channel_count
        )
        samples.append(sample)

    dac = board.I2S0()
    for sample, name in zip(samples, sample_names):
        print(" ", name)
        trigger.value = False
        dac.play(sample, loop=True)
        time.sleep(1)
        dac.stop()
        time.sleep(0.1)
        trigger.value = True
    print()

print("done")
