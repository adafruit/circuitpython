"""
    This example is to read integer type value.
"""
from adafruit_ble import *
from adafruit_ble.services import *
from adafruit_ble.uuid import *
from adafruit_ble.characteristics.int import *
from adafruit_ble.characteristics.string import *
from time import sleep


class TemperatureService(Service):  # pylint: disable=too-few-public-methods
    """Temperature sensor."""

    uuid = StandardUUID(0x181A)
    temperature = Int16Characteristic(
        uuid=StandardUUID(0x2A6E),
        properties=(Characteristic.READ | Characteristic.NOTIFY),
        write_perm=Attribute.NO_ACCESS,
    )


temp_svc = TemperatureService()
ble = BLERadio()
adv = Advertisement()
adv.short_name = "Silabs-BLE"
adv.connectable = True

while True:
    ble.start_advertising(adv)
    print("start_advertising")
    while not ble.connected:
        pass
    ble.stop_advertising()
    print("stop_advertising")
    while ble.connected:
        temp_svc.temperature = 27 * 100
        sleep(1)

    print("disconnected")
