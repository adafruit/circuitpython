# SPDX-FileCopyrightText: 2020 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

"""
Can be used with ble_packet_buffer_client.py. Receives packets from the
PacketBufferService and transmits them back.
"""

from ble_packet_buffer_service import PacketBufferService

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement


ble = BLERadio()
pbs = PacketBufferService()
advertisement = ProvideServicesAdvertisement(pbs)

buf = bytearray(512)

while True:
    ble.start_advertising(advertisement)
    while not ble.connected:
        pass
    while ble.connected:
        # Returns b'' if nothing was read.
        packet_len = pbs.readinto(buf)
        if packet_len > 0:
            packet = buf[:packet_len]
            print(packet)
            pbs.write(packet)
