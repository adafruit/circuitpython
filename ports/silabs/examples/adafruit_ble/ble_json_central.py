# SPDX-FileCopyrightText: 2020 Mark Raleson
#
# SPDX-License-Identifier: MIT

# Read sensor readings from peripheral BLE device using a JSON characteristic.

from ble_json_service import SensorService
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement


ble = BLERadio()
connection = None

while True:

    if not connection:
        print("Scanning for BLE device advertising our sensor service...")
        for adv in ble.start_scan(ProvideServicesAdvertisement):
            if SensorService in adv.services:
                connection = ble.connect(adv)
                print("Connected")
                break
        ble.stop_scan()

    if connection and connection.connected:
        service = connection[SensorService]
        service.settings = {"unit": "celsius"}  #  'fahrenheit'
        while connection.connected:
            print("Sensors: ", service.sensors)
