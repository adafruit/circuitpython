USB_VID = 0x239A
USB_PID = 0x817A
USB_PRODUCT = "SP-1 (CircuitPython)"
USB_MANUFACTURER = "Teenage Engineering"

MCU_CHIP = nrf52840

# SystemInit() burns UICR PSELRESET[0..1] = 18 and resets whenever it finds
# them unprogrammed, which is the case on this board. Opt out of it
$(BUILD)/nrfx/mdk/system_nrf52840.o: CFLAGS += -UCONFIG_GPIO_AS_PINRESET

# No BLE. A SoftDevice would have to live at 0x1000, which is inside this
# board's bootloader, and the radio has no antenna.
CIRCUITPY_BLEIO_NATIVE = 0
CIRCUITPY_BLE_FILE_SERVICE = 0
CIRCUITPY_BLE_SERIAL_SERVICE = 0

# CIRCUITPY is in internal flash; there is no external flash chip.
INTERNAL_FLASH_FILESYSTEM = 1

# No UF2 bootloader on this device.
CIRCUITPY_BUILD_EXTENSIONS = bin,hex

# displayio off no display.
CIRCUITPY_DISPLAYIO = 0
CIRCUITPY_FRAMEBUFFERIO = 0
CIRCUITPY_RGBMATRIX = 0
CIRCUITPY_SHARPDISPLAY = 0
CIRCUITPY_IS31FL3741 = 0
CIRCUITPY_VECTORIO = 0

# The watchdog is started by the bootloader before our first instruction and
# its config registers are locked.
CIRCUITPY_WATCHDOG = 0

# alarm's idle paths need a WDT-feed audit before they are safe here.
CIRCUITPY_ALARM = 0

# Audio
CIRCUITPY_AUDIOPWMIO = 0
CIRCUITPY_SYNTHIO = 1
CIRCUITPY_AUDIOEFFECTS = 1
CIRCUITPY_AUDIOMP3 = 1

# Hold-to-power-off, opted into by BOARD_POWER_OFF_BUTTON_PIN in
# mpconfigboard.h.
SRC_C += boards/$(BOARD)/power_off.c

# ACL write-protection for the MBR and the bootloader, which this board's
# bootloader does not set up for itself.
SRC_C += boards/$(BOARD)/flash_protect.c

# The soldered-down eMMC, and the supervisor mount that puts it on /sd and on
# USB as a second drive.
CIRCUITPY_EMMCIO = 1
CIRCUITPY_EMMC_USB = 1

FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_Register
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_CS42L42
FROZEN_MPY_DIRS += $(TOP)/frozen/Adafruit_CircuitPython_TAS2505

CIRCUITPY_REQUIRE_I2C_PULLUPS = 0
