# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test the rtc module against native_sim's emulated RTC.

On a fresh boot the RTC has never been set (the emulated driver has no
battery backup), so reading it must give the start of the CircuitPython
epoch, 2000-01-01, rather than an error. Setting the datetime must update
what time.localtime() reports, and the RTC must advance on its own.
"""

import pytest


RTC_SET_CODE = """\
import rtc
import time

r = rtc.RTC()

t = time.localtime()
print("UNSET:", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec)

r.datetime = time.struct_time((2025, 7, 4, 12, 34, 56, 4, -1, -1))

t = time.localtime()
print("SET:", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec)

time.sleep(2)
t = time.localtime()
print("ADVANCED:", t.tm_year, (t.tm_hour, t.tm_min, t.tm_sec) != (12, 34, 56))

try:
    r.calibration = 1
except NotImplementedError:
    print("CALIBRATION NOT SUPPORTED")
"""


@pytest.mark.circuitpy_drive({"code.py": RTC_SET_CODE})
@pytest.mark.duration(30)
def test_rtc_set_and_get(circuitpython):
    """Setting rtc.RTC().datetime updates time.localtime() and advances."""
    circuitpython.serial.wait_for("UNSET: 2000 1 1", timeout=30)
    circuitpython.serial.wait_for("SET: 2025 7 4 12 34 56", timeout=30)
    circuitpython.serial.wait_for("ADVANCED: 2025 True", timeout=30)
    circuitpython.serial.wait_for("CALIBRATION NOT SUPPORTED", timeout=30)
