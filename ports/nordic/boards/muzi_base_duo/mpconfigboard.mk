USB_VID = 0x239A
USB_PID = 0x0081
USB_PRODUCT = "Base Duo"
USB_MANUFACTURER = "muzi works"

MCU_CHIP = nrf52840

QSPI_FLASH_FILESYSTEM = 1
# Rev01 ships a W25Q128JV; the Meshtastic BASE variant documents a W25Q32JV.
# List both families and let the JEDEC ID decide at runtime.
EXTERNAL_FLASH_DEVICES = "W25Q128JVxQ, W25Q64JVxQ, W25Q32JVxQ, W25Q32JVxM"
