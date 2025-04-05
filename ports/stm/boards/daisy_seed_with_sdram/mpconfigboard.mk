USB_VID = 0x0483
USB_PID = 0x5740
USB_PRODUCT = "Daisy Seed"
USB_MANUFACTURER = "STMicroelectronics"

# Small FS created on half of the internal flash -- other half is reserved for the H750 bootloader
INTERNAL_FLASH_FILESYSTEM = 1

MCU_SERIES = H7
MCU_VARIANT = STM32H750xx
MCU_PACKAGE = UFBGA176

LD_COMMON = boards/common_tcm.ld
LD_FILE = boards/STM32H750.ld

CIRCUITPY_SDIOIO = 1
CIRCUITPY_PWMIO = 1
CIRCUITPY_AUDIOPWMIO = 1
