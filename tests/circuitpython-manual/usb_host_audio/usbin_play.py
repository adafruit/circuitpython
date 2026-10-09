# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Play a USB microphone live on the Fruit Jam's headphone jack: USBIn is the
# root of the audio chain, resampled from its 16 kHz to the DAC's 48 kHz.
# Reports once a second whether audio was dropped. Needs the adafruit_tlv320 library.
#
# See usbin_record.py for the microphone set up.

import time

import adafruit_tlv320
import audiobusio
import audiomixer
import audiospeed
import board
import digitalio
import usb.core
import usb_host_audio

INTERFACE = 3
ALTERNATE = 1
ENDPOINT = 0x86
SAMPLE_RATE = 16000

reset_pin = digitalio.DigitalInOut(board.PERIPH_RESET)
reset_pin.switch_to_output(False)
time.sleep(0.1)
reset_pin.value = True
dac = adafruit_tlv320.TLV320DAC3100(board.I2C())
dac.configure_clocks(sample_rate=48000, bit_depth=16)
dac.headphone_output = True
dac.dac_volume = -10
audio = audiobusio.I2SOut(board.I2S_BCLK, board.I2S_WS, board.I2S_DIN)
mixer = audiomixer.Mixer(
    voice_count=1, sample_rate=48000, channel_count=1, bits_per_sample=16, buffer_size=2048
)
audio.play(mixer)

device = usb.core.find(idVendor=0x046D, idProduct=0x0825)
device.set_configuration()
device.ctrl_transfer(0x01, 11, ALTERNATE, INTERFACE, None)
device.ctrl_transfer(0x22, 0x01, 0x0100, ENDPOINT, SAMPLE_RATE.to_bytes(3, "little"))

mic = usb_host_audio.USBIn(device, ENDPOINT, sample_rate=SAMPLE_RATE, max_packet_size=68)
mixer.voice[0].play(audiospeed.Resampler(mic))
try:
    while mic.streaming:
        print("overflow", mic.overflow)
        time.sleep(1)
finally:
    mixer.voice[0].stop()
    device.ctrl_transfer(0x01, 11, 0, INTERFACE, None)
    mic.deinit()
