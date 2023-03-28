from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
from adafruit_ble.services.nordic import UARTService
import time

ble = BLERadio()
uart = UARTService()

advertisement = ProvideServicesAdvertisement(uart)
advertisement.short_name = "Silab"


while True:
    ble.start_advertising(advertisement)
    print("advertising")
    while not ble.connected:
        pass
    ble.stop_advertising()
    print("connected")
    while ble.connected:
        print(uart.read(3))
        time.sleep(1)
        uart.write(b"echo")
        time.sleep(1)
        uart.write(b"echo2")
        time.sleep(1)
