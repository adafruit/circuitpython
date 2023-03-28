from adafruit_ble import BLERadio
from adafruit_ble.advertising.standard import ProvideServicesAdvertisement
import time
import _bleio
from adafruit_ble.services import Service
from adafruit_ble.characteristics import (
    Attribute,
    Characteristic,
    ComplexCharacteristic,
)
from adafruit_ble.uuid import VendorUUID


class PacketBufferUUID(VendorUUID):
    """UUIDs with the PacketBuffer base UUID."""

    # pylint: disable=too-few-public-methods
    def __init__(self, uuid16):
        uuid128 = bytearray("reffuBtekcaP".encode("utf-8") + b"\x00\x00\xaf\xad")
        uuid128[-3] = uuid16 >> 8
        uuid128[-4] = uuid16 & 0xFF
        super().__init__(uuid128)


class PacketBufferCharacteristic(ComplexCharacteristic):
    def __init__(
        self,
        *,
        uuid=None,
        buffer_size=4,
        properties=Characteristic.WRITE
        | Characteristic.WRITE_NO_RESPONSE
        | Characteristic.NOTIFY
        | Characteristic.READ,
        read_perm=Attribute.OPEN,
        write_perm=Attribute.OPEN
    ):
        self.buffer_size = buffer_size
        super().__init__(
            uuid=uuid,
            properties=properties,
            read_perm=read_perm,
            write_perm=write_perm,
            max_length=512,
            fixed_length=False,
        )

    def bind(self, service):
        """Binds the characteristic to the given Service."""
        bound_characteristic = super().bind(service)
        return _bleio.PacketBuffer(
            bound_characteristic, buffer_size=self.buffer_size, max_packet_size=512
        )


class PacketBufferService(Service):
    """Test service that has one packet buffer"""

    uuid = PacketBufferUUID(0x0001)
    packets = PacketBufferCharacteristic(uuid=PacketBufferUUID(0x0101))

    def readinto(self, buf):
        return self.packets.readinto(buf)

    def write(self, buf, *, header=None):
        return self.packets.write(buf, header=header)


ble = BLERadio()
pbs = PacketBufferService()
advertisement = ProvideServicesAdvertisement(pbs)
advertisement.short_name = "Silabs"

buf = bytearray(512)

count = 0
while True:
    ble.start_advertising(advertisement)
    while not ble.connected:
        pass
    ble.stop_advertising()
    while ble.connected:
        # Returns b'' if nothing was read.
        packet_len = pbs.readinto(buf)
        if packet_len > 0:
            packet = buf[:packet_len]
            print(packet)
        count += 1
        a = pbs.write("Hello" + str(count))
        time.sleep(2)
