# SPDX-FileCopyrightText: 2020 Mark Raleson
#
# SPDX-License-Identifier: MIT

# Provide readable sensor values and writable settings to connected devices via JSON characteristic.

import time
import random
from ble_json_service import SensorService
from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement


# Create BLE radio, custom service, and advertisement.
ble = BLERadio()
service = SensorService()
advertisement = ProvideServicesAdvertisement(service)

# Function to get some fake weather sensor readings for this example in the desired unit.
def measure(unit):
    temperature = random.uniform(0.0, 10.0)
    humidity = random.uniform(0.0, 100.0)
    if unit == "fahrenheit":
        temperature = (temperature * 9.0 / 5.0) + 32.0
    return {"temperature": temperature, "humidity": humidity}


# Advertise until another device connects, when a device connects, provide sensor data.
while True:
    print("Advertise services")
    ble.stop_advertising()  # you need to do this to stop any persistent old advertisement
    ble.start_advertising(advertisement)

    print("Waiting for connection...")
    while not ble.connected:
        pass

    print("Connected")
    while ble.connected:
        settings = service.settings
        measurement = measure(settings.get("unit", "celsius"))
        service.sensors = measurement
        print("Settings: ", settings)
        print("Sensors: ", measurement)
        time.sleep(0.25)

    print("Disconnected")
