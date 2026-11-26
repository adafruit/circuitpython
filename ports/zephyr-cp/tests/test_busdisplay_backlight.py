# SPDX-FileCopyrightText: Copyright (c) 2026
# SPDX-License-Identifier: MIT

"""Tests for externally supplied BusDisplay backlight objects."""

import pytest


EXTERNAL_BACKLIGHT_CODE = """\
import board
import busdisplay
import digitalio
import displayio
import fourwire
import pwmio
import time
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
digital = digitalio.DigitalInOut(pin.P_12)
digital.switch_to_output(False)
pwm = pwmio.PWMOut(pin.P_05, frequency=1000, duty_cycle=0)

digital_display = busdisplay.BusDisplay(bus, b'', width=64, height=64,
    backlight=digital, auto_refresh=False)
pwm_display = busdisplay.BusDisplay(bus, b'', width=64, height=64,
    backlight=pwm, auto_refresh=False)

digital_display.brightness = 0.5
assert not digital.value
digital_display.brightness = 1.0
assert digital.value
pwm_display.brightness = 0.25
assert abs(pwm.duty_cycle - 0x3fff) <= 1

try:
    busdisplay.BusDisplay(bus, b'', width=64, height=64, backlight=object())
    raise AssertionError('invalid backlight accepted')
except TypeError:
    pass

# Supplied objects are borrowed, not released with either display.
digital_display.deinit()
digital.value = False
assert pwm_display.width == 64
pwm_display.deinit()
pwm.duty_cycle = 0
bus.deinit()
digital.deinit()
pwm.deinit()
print('external backlights ok')
while True:
    time.sleep(1)
"""


SOFT_RELOAD_BACKLIGHT_CODE = """\
import board
import busdisplay
import digitalio
import displayio
import fourwire
import supervisor
import time
from microcontroller import pin

try:
    with open('backlight-run.txt', 'r') as f:
        run = int(f.read()) + 1
except OSError:
    run = 0
with open('backlight-run.txt', 'w') as f:
    f.write(str(run))

if run == 0:
    displayio.release_displays()
    bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
    backlight = digitalio.DigitalInOut(pin.P_12)
    backlight.switch_to_output(False)
    primary = busdisplay.BusDisplay(bus, b'', width=64, height=64,
        backlight=backlight, auto_refresh=False)
    supervisor.runtime.display = primary
    print('backlight reload first')
else:
    primary = supervisor.runtime.display
    assert primary is not None
    backlight = primary.backlight
    primary.brightness = 1.0
    assert backlight.value
    primary.deinit()
    # The adopted native copy moved back to this heap object on VM start.
    backlight.deinit()
    backlight = digitalio.DigitalInOut(pin.P_12)
    backlight.deinit()
    print('backlight soft reload ok')
time.sleep(1)
print('done')
"""


PRIMARY_PWM_RELOAD_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import gc
import pwmio
import supervisor
import time
from microcontroller import pin

try:
    with open('backlight-run.txt', 'r') as f:
        run = int(f.read()) + 1
except OSError:
    run = 0
with open('backlight-run.txt', 'w') as f:
    f.write(str(run))

if run == 0:
    displayio.release_displays()
    bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
    pwm = pwmio.PWMOut(pin.P_05, frequency=1000, duty_cycle=0)
    primary = busdisplay.BusDisplay(bus, b'', width=64, height=64,
        backlight=pwm, brightness=0.5, auto_refresh=False)
    supervisor.runtime.display = primary
    assert abs(pwm.duty_cycle - 0x7fff) <= 1
    assert primary.backlight is pwm
    # The display proxy roots the borrowed PWM until the primary adopts it.
    del pwm
    gc.collect()
    print('primary PWM before reload')
elif run == 1:
    primary = supervisor.runtime.display
    backlight = primary.backlight
    assert isinstance(backlight, pwmio.PWMOut)
    assert abs(backlight.duty_cycle - 0x7fff) <= 1
    next_primary = busdisplay.BusDisplay(primary.bus, b'', width=64, height=64,
        backlight=backlight, brightness=0.5, auto_refresh=False)
    supervisor.runtime.display = next_primary
    print('primary PWM handed off')
else:
    primary = supervisor.runtime.display
    backlight = primary.backlight
    assert isinstance(backlight, pwmio.PWMOut)
    assert primary.brightness == 0.5
    primary.brightness = 0.25
    assert abs(backlight.duty_cycle - 0x3fff) <= 1
    primary.deinit()
    # The display borrowed the heap object for this VM, so release it directly.
    backlight.deinit()
    pwm = pwmio.PWMOut(pin.P_05, frequency=1000, duty_cycle=0)
    pwm.deinit()
    print('primary PWM survived reload')
time.sleep(1)
"""


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": EXTERNAL_BACKLIGHT_CODE})
@pytest.mark.duration(15)
def test_external_digitalinout_and_pwmout_backlights(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "external backlights ok" in output
    assert "Traceback" not in output


@pytest.mark.parametrize("board", ["native_native_sim"], indirect=True)
@pytest.mark.circuitpy_drive({"code.py": SOFT_RELOAD_BACKLIGHT_CODE})
@pytest.mark.port_resets(3)
@pytest.mark.duration(15)
def test_external_backlight_survives_soft_reload(circuitpython):
    circuitpython.serial.wait_for("backlight reload first")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("backlight soft reload ok")
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert output.count("backlight reload first") == 1
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board", ["native_native_sim_asan", "native_native_sim_lvgl_asan"], indirect=True
)
@pytest.mark.circuitpy_drive({"code.py": PRIMARY_PWM_RELOAD_CODE})
@pytest.mark.port_resets(4)
@pytest.mark.duration(15)
def test_primary_pwm_survives_soft_reload(circuitpython):
    circuitpython.serial.wait_for("primary PWM before reload")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("primary PWM handed off")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("primary PWM survived reload")
    circuitpython.wait_until_done()
    assert "Traceback" not in circuitpython.serial.all_output
