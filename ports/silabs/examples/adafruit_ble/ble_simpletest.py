# SPDX-FileCopyrightText: 2020 ladyada for Adafruit Industries
# SPDX-License-Identifier: MIT

"""
This example scans for any BLE advertisements and prints one advertisement and one scan response
from every device found.
"""

from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement

ble = BLERadio()
print("scanning")
found = set()
scan_responses = set()
for advertisement in ble.start_scan(ProvideServicesAdvertisement):
    addr = advertisement.address
    if advertisement.scan_response and addr not in scan_responses:
        scan_responses.add(addr)
    elif not advertisement.scan_response and addr not in found:
        found.add(addr)
    else:
        continue
    print(addr, advertisement)
    print("\t" + repr(advertisement))
    print()

print("scan done")
