"""
    This example is to write string type value.
"""
from adafruit_ble import *
from adafruit_ble.services import *
from adafruit_ble.uuid import *
from adafruit_ble.characteristics.string import *
from time import sleep


class TestService(Service):
    uuid = VendorUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9F")
    TestString = StringCharacteristic(
        uuid=VendorUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9F"),
        properties=(Characteristic.WRITE | Characteristic.NOTIFY),
        write_perm=Attribute.OPEN,
    )


test_svc = TestService()

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
        print(test_svc.TestString)
        sleep(1)
    print("disconnected")
