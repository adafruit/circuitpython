# Manual regression test for closing an open SD-card file on soft reload.
# Run on a board with board.SD_CS and an SD card. Delete
# /sdcard-reload-test-state.txt from CIRCUITPY and
# /sdcard-reload-test.txt from the SD card before starting.

import board
import sdcardio
import storage
import supervisor

STATE_FILE = "/sdcard-reload-test-state.txt"
SD_FILE = "/sd/sdcard-reload-test.txt"
EXPECTED = "unflushed write survives soft reload"

sd = sdcardio.SDCard(board.SPI(), board.SD_CS)
storage.mount(storage.VfsFat(sd), "/sd")

try:
    with open(STATE_FILE, "r") as state_file:
        state = state_file.read()
except OSError:
    state = ""

if state != "pending":
    with open(STATE_FILE, "w") as state_file:
        state_file.write("pending")

    # Deliberately keep this file open and unflushed as the VM is stopped.
    test_file = open(SD_FILE, "w")
    test_file.write(EXPECTED)
    print("Wrote test data; reloading without closing the SD file.")
    supervisor.reload()
else:
    with open(SD_FILE, "r") as test_file:
        actual = test_file.read()
    if actual == EXPECTED:
        print("PASS: open SD-card file was closed before its SPI bus reset.")
    else:
        print("FAIL: expected %r, got %r" % (EXPECTED, actual))
