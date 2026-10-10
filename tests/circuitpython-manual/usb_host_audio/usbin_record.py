# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Record from a USB Audio Class 1.0 microphone with usb_host_audio.USBIn and
# print the level of each tenth of a second.
#
# usb_host_audio only moves the audio, so this sets the microphone up by hand
# with the values of a Logitech C270 webcam: streaming interface 3, alternate
# setting 1, isochronous IN endpoint 0x86, 16 bit mono at 16 kHz. The
# adafruit_usb_host_microphone library finds these in the descriptors.

import array
import math

import usb.core
import usb_host_audio

INTERFACE = 3
ALTERNATE = 1
ENDPOINT = 0x86
SAMPLE_RATE = 16000

device = usb.core.find(idVendor=0x046D, idProduct=0x0825)
device.set_configuration()
# SET_INTERFACE starts the stream, then SET_CUR of the sampling frequency.
device.ctrl_transfer(0x01, 11, ALTERNATE, INTERFACE, None)
device.ctrl_transfer(0x22, 0x01, 0x0100, ENDPOINT, SAMPLE_RATE.to_bytes(3, "little"))

samples = array.array("h", bytes(2 * SAMPLE_RATE // 10))
with usb_host_audio.USBIn(device, ENDPOINT, sample_rate=SAMPLE_RATE, max_packet_size=68) as mic:
    for _ in range(50):
        count = mic.record(samples, len(samples))
        if count == 0:
            break
        mean = sum(samples) / count
        rms = math.sqrt(sum((s - mean) ** 2 for s in samples) / count)
        print("#" * int(rms / 100), "overflow:", mic.overflow)
    # Stop the stream before the object lets go of it.
    device.ctrl_transfer(0x01, 11, 0, INTERFACE, None)
