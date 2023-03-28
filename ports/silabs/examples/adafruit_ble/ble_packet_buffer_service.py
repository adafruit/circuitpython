# SPDX-FileCopyrightText: 2022 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""
Used with ble_packet_buffer_*.py. Establishes a simple server to test
PacketBuffer with.
"""

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
        properties=Characteristic.WRITE_NO_RESPONSE | Characteristic.NOTIFY | Characteristic.READ,
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
