# SPDX-FileCopyrightText: 2026 Dan Halbert for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test pwmio on native_sim.

The emulated PWM controller (adafruit,pwm-emul) has one channel per gpio_emul
pin and two period groups that any channel can join, clocked at 16 MHz. It
records what pwm_set_cycles() receives on the counter tracks "pwm.NN.period"
and "pwm.NN.pulse". The tests check pwmio's own behavior and the grouping
rules every port shares, not any SoC's layout.
"""

import pytest
from perfetto.trace_processor import TraceProcessor

CLOCK = 16_000_000


def _track_values(trace_file, track_name: str) -> list[int]:
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        f'''
        SELECT c.value
        FROM counter c
        JOIN track t ON c.track_id = t.id
        WHERE t.name = "{track_name}"
        ORDER BY c.ts
        '''
    )
    return [int(row.value) for row in result]


def _pulse(period: int, duty: int) -> int:
    return period * duty // 0xFFFF


PWMIO_SETTINGS_CODE = """\
import microcontroller
import pwmio

pwm = pwmio.PWMOut(microcontroller.pin.P_05, frequency=1000, duty_cycle=0x4000)
print(f"frequency {pwm.frequency}")
pwm.duty_cycle = 0xC000
pwm.duty_cycle = 0xFFFF
pwm.deinit()
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_SETTINGS_CODE})
def test_pwmio_settings(circuitpython):
    """Frequency becomes the period, duty the pulse, and deinit stops the output."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "frequency 1000" in output
    assert "done" in output

    period = CLOCK // 1000
    trace = circuitpython.trace_file
    assert _track_values(trace, "pwm.05.period") == [period] * 4
    assert _track_values(trace, "pwm.05.pulse") == [
        _pulse(period, 0x4000),
        _pulse(period, 0xC000),
        period,
        0,
    ]


PWMIO_FREQUENCY_CODE = """\
import microcontroller
import pwmio

pwm = pwmio.PWMOut(microcontroller.pin.P_05, frequency=12345)
print(f"frequency {pwm.frequency}")
pwm.deinit()
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_FREQUENCY_CODE})
def test_pwmio_frequency_is_actual(circuitpython):
    """frequency reports what the period produces, not what was requested."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "done" in output

    period = _track_values(circuitpython.trace_file, "pwm.05.period")[0]
    assert period == CLOCK // 12345
    # 16 MHz / 1296 = 12345.7 Hz, reported to the nearest Hz.
    assert f"frequency {round(CLOCK / period)}" in output


PWMIO_VARIABLE_FREQUENCY_CODE = """\
import microcontroller
import pwmio

pwm = pwmio.PWMOut(
    microcontroller.pin.P_05, frequency=440, duty_cycle=0x8000, variable_frequency=True
)
pwm.frequency = 880
print(f"frequency {pwm.frequency}")
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_VARIABLE_FREQUENCY_CODE})
def test_pwmio_variable_frequency_change(circuitpython):
    """Changing the frequency writes a new period and keeps the duty cycle."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "frequency 880" in output
    assert "done" in output

    trace = circuitpython.trace_file
    assert _track_values(trace, "pwm.05.period") == [CLOCK // 440, CLOCK // 880]
    assert _track_values(trace, "pwm.05.pulse") == [
        _pulse(CLOCK // 440, 0x8000),
        _pulse(CLOCK // 880, 0x8000),
    ]


PWMIO_INVALID_FREQUENCY_CODE = """\
import microcontroller
import pwmio

for frequency in (0, 20_000_000):
    try:
        pwmio.PWMOut(microcontroller.pin.P_05, frequency=frequency)
        print(f"{frequency} ok")
    except ValueError:
        print(f"{frequency} ValueError")
fastest = pwmio.PWMOut(microcontroller.pin.P_05, frequency=8_000_000)
print(f"fastest {fastest.frequency}")
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_INVALID_FREQUENCY_CODE})
def test_pwmio_invalid_frequency(circuitpython):
    """Frequencies the clock can't produce raise ValueError; the fastest one works."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "0 ValueError" in output
    assert "20000000 ValueError" in output
    # 16 MHz / 8 MHz = a 2-cycle period, the shortest allowed.
    assert "fastest 8000000" in output
    assert "done" in output


PWMIO_PIN_CLAIM_CODE = """\
import digitalio
import microcontroller
import pwmio

p = microcontroller.pin
dio = digitalio.DigitalInOut(p.P_05)
try:
    pwmio.PWMOut(p.P_05)
    print("over DigitalInOut ok")
except ValueError:
    print("over DigitalInOut ValueError")
dio.deinit()

pwm = pwmio.PWMOut(p.P_05, frequency=1000)
try:
    pwmio.PWMOut(p.P_05, frequency=1000)
    print("second PWMOut ok")
except ValueError:
    print("second PWMOut ValueError")
pwm.deinit()

dio = digitalio.DigitalInOut(p.P_05)
print("DigitalInOut after deinit ok")
dio.deinit()
again = pwmio.PWMOut(p.P_05, frequency=2000)
print(f"PWMOut after deinit {again.frequency}")
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_PIN_CLAIM_CODE})
def test_pwmio_pin_claims(circuitpython):
    """A PWMOut claims its pin while it lives and releases it on deinit."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "over DigitalInOut ValueError" in output
    assert "second PWMOut ValueError" in output
    assert "DigitalInOut after deinit ok" in output
    assert "PWMOut after deinit 2000" in output
    assert "done" in output


PWMIO_GROUPS_CODE = """\
import microcontroller
import pwmio

p = microcontroller.pin
shared = [pwmio.PWMOut(pin, frequency=1000) for pin in (p.P_05, p.P_06, p.P_07)]
other = pwmio.PWMOut(p.P_08, frequency=2000)
print("two groups ok")
try:
    pwmio.PWMOut(p.P_09, frequency=3000)
    print("third frequency ok")
except RuntimeError:
    print("third frequency RuntimeError")
for pwm in shared:
    pwm.deinit()
third = pwmio.PWMOut(p.P_09, frequency=3000)
print("third frequency after deinit ok")
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_GROUPS_CODE})
def test_pwmio_same_frequency_shares_a_group(circuitpython):
    """Equal frequencies share a group; with every group taken a new frequency fails
    until a group is freed."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "two groups ok" in output
    assert "third frequency RuntimeError" in output
    assert "third frequency after deinit ok" in output
    assert "done" in output

    trace = circuitpython.trace_file
    for pin in (5, 6, 7):
        assert _track_values(trace, f"pwm.{pin:02}.period")[0] == CLOCK // 1000
    assert _track_values(trace, "pwm.08.period")[0] == CLOCK // 2000
    assert _track_values(trace, "pwm.09.period") == [CLOCK // 3000]


PWMIO_VARIABLE_EXCLUSIVE_CODE = """\
import microcontroller
import pwmio

p = microcontroller.pin
var = pwmio.PWMOut(p.P_05, frequency=440, variable_frequency=True)
other = pwmio.PWMOut(p.P_06, frequency=440)
print("other ok")
try:
    pwmio.PWMOut(p.P_07, frequency=300)
    print("300 ok")
except RuntimeError:
    print("300 RuntimeError")
joins = pwmio.PWMOut(p.P_08, frequency=440)
print("joins ok")
var.deinit()
after = pwmio.PWMOut(p.P_09, frequency=300)
print("300 after deinit ok")
print("done")
"""


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": PWMIO_VARIABLE_EXCLUSIVE_CODE})
def test_pwmio_variable_frequency_has_own_group(circuitpython):
    """A variable-frequency PWMOut shares its group with nobody."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    # The 440 Hz fixed-frequency PWMOut can't join the variable one's group.
    assert "other ok" in output
    # Both groups are taken.
    assert "300 RuntimeError" in output
    # Another fixed 440 Hz PWMOut joins the fixed 440 Hz group.
    assert "joins ok" in output
    assert "300 after deinit ok" in output
    assert "done" in output
