from adafruit_ble import *
from adafruit_ble.services import *
from adafruit_ble.uuid import *
from adafruit_ble.characteristics.float import FloatCharacteristic
from adafruit_ble.characteristics.string import *
from time import sleep


class TemperatureService(Service):
    uuid = VendorUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9F")
    temperature = FloatCharacteristic(
        uuid=VendorUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9F"),
        properties=(Characteristic.READ | Characteristic.NOTIFY | Characteristic.WRITE),
        write_perm=Attribute.NO_ACCESS,
    )


temp_svc = TemperatureService()
ble = BLERadio()
adv = Advertisement()
adv.short_name = "Silabs"
adv.connectable = True
while True:
    ble.start_advertising(adv)
    print("start_advertising")
    while not ble.connected:
        pass
    ble.stop_advertising()
    print("stop_advertising")
    while ble.connected:
        print(temp_svc.temperature)
        sleep(1)
    print("disconnected")
