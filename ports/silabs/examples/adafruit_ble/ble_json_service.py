# SPDX-FileCopyrightText: 2020 Mark Raleson
#
# SPDX-License-Identifier: MIT

# Read sensor readings from peripheral BLE device using a JSON characteristic.

from adafruit_ble.uuid import VendorUUID
from adafruit_ble.services import Service
from adafruit_ble.characteristics import Characteristic
from adafruit_ble.characteristics.json import JSONCharacteristic


# A custom service with two JSON characteristics for this device.  The "sensors" characteristic
# provides updated sensor values for any connected device to read.  The "settings" characteristic
# can be changed by any connected device to update the peripheral's settings.  The UUID of your
# service can be any valid random uuid (some BLE UUID's are reserved).
# NOTE: JSON data is limited by characteristic max_length of 512 byes.
class SensorService(Service):
    # pylint: disable=too-few-public-methods

    uuid = VendorUUID("51ad213f-e568-4e35-84e4-67af89c79ef0")

    settings = JSONCharacteristic(
        uuid=VendorUUID("e077bdec-f18b-4944-9e9e-8b3a815162b4"),
        properties=Characteristic.READ | Characteristic.WRITE,
        initial_value={"unit": "celsius"},
    )

    sensors = JSONCharacteristic(
        uuid=VendorUUID("528ff74b-fdb8-444c-9c64-3dd5da4135ae"),
        properties=Characteristic.READ,
    )

    def __init__(self, service=None):
        super().__init__(service=service)
        self.connectable = True
