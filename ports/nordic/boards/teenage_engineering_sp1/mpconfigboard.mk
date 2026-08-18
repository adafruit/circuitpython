# Placeholder USB IDs: pid.codes 0x1209/0x0001 is the "test" PID, valid for
# development only. A real allocation is needed before any release. Do NOT
# ship Teenage Engineering's 0x2367 -- this is not their firmware.
USB_VID = 0x1209
USB_PID = 0x0001
USB_PRODUCT = "SP-1 (CircuitPython)"
USB_MANUFACTURER = "Teenage Engineering"

MCU_CHIP = nrf52840

# No UICR write may survive into this board's image (plan §4.5.a). UICR is
# undoable only by ERASEALL, which needs SWD; the SP-1's SWD pads are not
# reachable, and an ERASEALL would take TE's bootloader with it — the only copy
# in existence, since the USB protocol has no read command. So each of the
# three conditional UICR writes on the boot path is turned off here, and the
# acceptance test is the disassembly: every reference to 0x10001000 in
# firmware.elf must be a read.
#
# 1. PSELRESET. The SP-1 has no reset pin and P0.18's wiring is unknown. With
#    CONFIG_GPIO_AS_PINRESET on, SystemInit would burn PSELRESET[0..1] = 18
#    on the first boot (PSELRESET is almost certainly erased on this device),
#    turning P0.18 into nRESET forever — if anything ever pulls that net low,
#    the chip is held in reset and unreachable even over SWD.
NRF_GPIO_AS_PINRESET = 0

# 2. REGOUT0. Fires only in high-voltage mode (VDDH supplied) with REGOUT0
#    unprogrammed. The proving ground never reached it — a USB-powered board
#    reads MAINREGSTATUS = 0 — but the SP-1 runs from a LiPo and may well be
#    VDDH-supplied, which is exactly the case that writes UICR. Safe to remove
#    either way: the stock firmware drives 3.3 V logic (eMMC, codecs, LEDs) on
#    this hardware today, so REGOUT0 is already whatever it needs to be.
NRF_REGOUT0_3V3 = 0

# 3. NFCPINS. P0.09/P0.10 are the NFC pins and are TAS_RESET and BT_RESET here.
#    Stock firmware drives both as GPIO, so NFCPINS.PROTECT must already be
#    cleared on this device and the write cannot fire — but "must already be"
#    is not the bar for a one-way change, and if the assumption is wrong the
#    right outcome is two dead reset lines to diagnose, not a UICR burn. The
#    actual value is read from the REPL at first light (plan §5.2).
NRF_NFCT_PINS_AS_GPIOS = 0

# No SoftDevice. It would have to live at 0x1000, which is inside the TE
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

# The watchdog is started by the TE bootloader before our first instruction and
# its config registers are locked.
CIRCUITPY_WATCHDOG = 0

# alarm's idle paths need a WDT-feed audit before they are safe here.
CIRCUITPY_ALARM = 0

# Audio
CIRCUITPY_AUDIOPWMIO = 0
CIRCUITPY_SYNTHIO = 1
CIRCUITPY_AUDIOEFFECTS = 1
# audiomp3 would otherwise come along for free with audiocore on a full build.
# It is 7.1's §6 A8 item -- decode CPU headroom on this chip is unassessed and
# it costs ~26 KB of flash -- so it stays off until it is deliberately assessed.
CIRCUITPY_AUDIOMP3 = 0

# eMMC
CIRCUITPY_SP1_EMMC = 1

# HS_TIMING for EMMC
CIRCUITPY_SP1_EMMC_HS = 1

# EMMC writing
CIRCUITPY_SP1_EMMC_WRITE = 1

# auto mount EMMC as /sd
CIRCUITPY_SP1_EMMC_USB = 1
