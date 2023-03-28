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
a.short_name = "Silabs-BLE"
a.connectable = True
from adafruit_ble import _bleio

my_info = DeviceInfoService(manufacturer="CircuitPython.org", model_number="1234")
print("start_advertising")

radio.start_advertising(a, scan_response=b"")
my_info.model_number = bytearray("1234")
my_info.serial_number = bytearray("A5A5A5A500C4040200000000")
my_info.software_revision = bytearray("Beta0.0")
my_info.hardware_revision = bytearray("EFR32MG24-Silabs")
my_info.firmware_revision = bytearray("v1.0.1")
my_info.manufacturer = bytearray("CircuitPython.org")
time.sleep(1)
