"""Reproducer for https://github.com/adafruit/circuitpython/issues/10896

Copy to code.py. Sequences through test steps using sleep_memory to
track state.

The test scoreboard is printed to the magtag e-ink display after every
run so the final display shows cumulative results.
"""

import alarm
import binascii
import microcontroller
import struct
import supervisor
import time

# Safe mode avoidance
# On boot CircuitPython writes a SAFE_MODE_USER guard to an RTC register,
# If a second boot occurs before 1000ms elapses then we enter safe mode.
# So, wait for 1000+1ms before calling microcontroller.reset().
_SAFE_MODE_WINDOW_MS = 1000


def _wait_for_safe_mode_window():
    elapsed_ms = time.monotonic() * 1000
    remaining = _SAFE_MODE_WINDOW_MS + 1 - elapsed_ms
    if remaining > 0:
        time.sleep(remaining / 1000)


# Test result enum
_UNTESTED = 0
_PASS = 1
_FAIL = 2
_LABEL = {_UNTESTED: "-", _PASS: "PASS", _FAIL: "FAIL"}

# CRC32-protected state in sleep_memory
# [magic:2][step:1][r_reset:1][r_reload:1][pad:1][crc32:4]
_MAGIC = 0xBE01
_FMT = "<HBBBx"
_DATA_SZ = struct.calcsize(_FMT)  # 6
_TOTAL = _DATA_SZ + 4  # 10


def _write(step, r_reset=_UNTESTED, r_reload=_UNTESTED):
    data = struct.pack(_FMT, _MAGIC, step, r_reset, r_reload)
    crc = struct.pack("<I", binascii.crc32(data) & 0xFFFFFFFF)
    alarm.sleep_memory[0:_TOTAL] = data + crc


def _read():
    raw = bytes(alarm.sleep_memory[0:_TOTAL])
    data = raw[:_DATA_SZ]
    stored_crc = struct.unpack_from("<I", raw, _DATA_SZ)[0]
    if (binascii.crc32(data) & 0xFFFFFFFF) != stored_crc:
        return (0, _UNTESTED, _UNTESTED, False)
    magic, step, r_reset, r_reload = struct.unpack(_FMT, data)
    if magic != _MAGIC:
        return (0, _UNTESTED, _UNTESTED, False)
    return (step, r_reset, r_reload, True)


def _scoreboard(r_reset, r_reload):
    print(f"  reset:  {_LABEL[r_reset]}")
    print(f"  reload: {_LABEL[r_reload]}")


# Main
reason = microcontroller.cpu.reset_reason
step, r_reset, r_reload, valid = _read()

if reason == microcontroller.ResetReason.POWER_ON:
    _scoreboard(_UNTESTED, _UNTESTED)
    print("writing marker, resetting...")
    _write(0)
    _wait_for_safe_mode_window()
    microcontroller.reset()

elif valid and step == 0:
    r_reset = _PASS
    _scoreboard(r_reset, _UNTESTED)
    print("testing supervisor.reload()...")
    _write(1, r_reset)
    _wait_for_safe_mode_window()
    supervisor.reload()

elif not valid and step == 0:
    r_reset = _FAIL
    _scoreboard(r_reset, _UNTESTED)
    print("Install patched FW, power cycle")

elif valid and step == 1:
    r_reload = _PASS
    _scoreboard(r_reset, r_reload)
    _write(2, r_reset, r_reload)

else:
    _scoreboard(r_reset, r_reload)
    print(f"unexpected: step={step} valid={valid}")
    print("Power cycle to restart tests")
