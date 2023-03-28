# SPDX-FileCopyrightText: 2022 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""
This example does a generic connectable advertisement and prints out the
manufacturer and model number of the device(s) that connect to it.
"""

import time
import adafruit_ble
from adafruit_ble.advertising.standard import Advertisement
from adafruit_ble.services.standard.device_info import DeviceInfoService

radio = adafruit_ble.BLERadio()
a = Advertisement()
a.connectable = True
radio.start_advertising(a)

# Info that the other device can read about us.
my_info = DeviceInfoService(manufacturer="CircuitPython.org", model_number="1234")

print("advertising")

while not radio.connected:
    pass

print("connected")

while radio.connected:
    for connection in radio.connections:
        if not connection.paired:
            connection.pair()
            print("paired")
        # dis = connection[DeviceInfoService]
        # print(dis.manufacturer)
        # print(dis.model_number)
        print(my_info.manufacturer)
        print(my_info.model_number)
    time.sleep(3)

print("disconnected")
