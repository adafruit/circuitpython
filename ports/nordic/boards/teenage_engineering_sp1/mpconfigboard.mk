USB_VID = 0x239A
USB_PID = 0x817A
USB_PRODUCT = "SP-1 (CircuitPython)"
USB_MANUFACTURER = "Teenage Engineering"

MCU_CHIP = nrf52840

# No UICR write may survive into this board's image. UICR is undoable only by
# ERASEALL, which needs SWD; the SP-1's SWD pads are not easily reachable, and an
# ERASEALL would take the bootloader with it. So each of the three conditional
# UICR writes on the boot path is turned off here.
#
# 1. PSELRESET reads erased here, so CONFIG_GPIO_AS_PINRESET would burn
#    PSELRESET[0..1] = 18 on the first boot, making P0.18 nRESET forever. This
#    board has no reset pin and P0.18's wiring is unknown; if anything pulls
#    that net low the chip is held in reset and unreachable even over SWD.
NRF_GPIO_AS_PINRESET = 0

# 2. REGOUT0 is unprogrammed but MAINREGSTATUS reads 0 (normal-voltage mode),
#    so the write cannot fire. Off anyway: VDD is externally regulated to
#    3.3 V, which is what the eMMC, codecs and LEDs already run at.
NRF_REGOUT0_3V3 = 0

# 3. NFCPINS reads 0xFFFFFFFE, i.e. PROTECT already cleared, so the write
#    cannot fire and P0.09/P0.10 work as TAS_RESET and BT_RESET. Off anyway:
#    if that ever fails to hold, two dead reset lines are diagnosable and a
#    UICR burn is not.
NRF_NFCT_PINS_AS_GPIOS = 0

# No SoftDevice. It would have to live at 0x1000, which is inside the
# bootloader, and the radio has no antenna. Empty SD also forces
# CIRCUITPY_BLEIO_NATIVE and the BLE workflow services to 0 (mpconfigport.mk).
SD =

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
# audiomp3 would otherwise come along for free with audiocore on a full build.
# but doesn't reliably decode mp3s off of emmc
CIRCUITPY_AUDIOMP3 = 0

# eMMC
CIRCUITPY_SP1EMMC = 1

# auto mount EMMC as /sd
CIRCUITPY_SP1EMMC_USB = 1
