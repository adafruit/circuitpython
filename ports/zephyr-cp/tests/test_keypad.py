# SPDX-FileCopyrightText: 2026 Dan Halbert for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test keypad functionality on native_sim."""

import re

import pytest


KEYS_CODE = """\
import time
import keypad
import microcontroller

keys = keypad.Keys(
    (microcontroller.pin.P_01, microcontroller.pin.P_02),
    value_when_pressed={value_when_pressed},
    pull=True,
)
event = keypad.Event()
print(f"t_abs={{time.monotonic():.3f}} start")

while time.monotonic() < 7.0:
    if keys.events.get_into(event):
        print(
            f"t_abs={{time.monotonic():.3f}} key={{event.key_number}} "
            f"pressed={{event.pressed}} ts={{event.timestamp}}"
        )
    time.sleep(0.01)

print(f"t_abs={{time.monotonic():.3f}} done")
"""


def keys_trace(pressed: int) -> dict:
    """Press and release key 0 at 4.0-4.5 s, then key 1 at 5.0-5.5 s."""
    released = 1 - pressed
    return {
        "gpio_emul.01": [
            (4_000_000_000, pressed),
            (4_500_000_000, released),
        ],
        "gpio_emul.02": [
            (5_000_000_000, pressed),
            (5_500_000_000, released),
        ],
    }


# (key_number, pressed, trace time in seconds)
EXPECTED_EVENTS = [
    (0, True, 4.0),
    (0, False, 4.5),
    (1, True, 5.0),
    (1, False, 5.5),
]

EVENT_RE = re.compile(r"t_abs=([0-9.]+) key=(\d+) pressed=(True|False) ts=(\d+)")


def check_keys_events(output: str) -> None:
    start_match = re.search(r"t_abs=([0-9.]+) start", output)
    assert start_match is not None
    # The trace only reaches pins that are already inputs.
    assert float(start_match.group(1)) < 4.0

    events = [
        (float(t_abs), int(key), pressed == "True", int(ts))
        for t_abs, key, pressed, ts in EVENT_RE.findall(output)
    ]
    assert [(key, pressed) for _, key, pressed, _ in events] == [
        (key, pressed) for key, pressed, _ in EXPECTED_EVENTS
    ]

    # Seen within one scan interval plus one poll of the trace edge.
    for (t_abs, _, _, _), (_, _, trace_time) in zip(events, EXPECTED_EVENTS):
        assert trace_time <= t_abs <= trace_time + 0.1

    # Event timestamps are in ms; each key is held for 500 ms.
    for press, release in ((events[0], events[1]), (events[2], events[3])):
        assert abs((release[3] - press[3]) - 500) <= 50

    assert "done" in output


@pytest.mark.circuitpy_drive({"code.py": KEYS_CODE.format(value_when_pressed=False)})
@pytest.mark.input_trace(keys_trace(pressed=0))
def test_keypad_keys_active_low(circuitpython):
    """Keys with pull-ups reports presses when a trace pulls the pins low."""
    circuitpython.wait_until_done()
    check_keys_events(circuitpython.serial.all_output)


@pytest.mark.circuitpy_drive({"code.py": KEYS_CODE.format(value_when_pressed=True)})
@pytest.mark.input_trace(keys_trace(pressed=1))
def test_keypad_keys_active_high(circuitpython):
    """Keys with pull-downs reports presses when a trace drives the pins high."""
    circuitpython.wait_until_done()
    check_keys_events(circuitpython.serial.all_output)


PIN_CONFIGURATION_CODE = """\
import time
import digitalio
import keypad
import microcontroller
from microcontroller import PinConfiguration

keys = keypad.Keys(
    (
        PinConfiguration(microcontroller.pin.P_01, pull=digitalio.Pull.UP),
        PinConfiguration(microcontroller.pin.P_02, pull=digitalio.Pull.DOWN),
        PinConfiguration(microcontroller.pin.P_03),
    ),
    value_when_pressed=False,
)
keys.reset()
time.sleep(0.2)
event = keypad.Event()
while keys.events.get_into(event):
    print(f"key={event.key_number} pressed={event.pressed}")
keys.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": PIN_CONFIGURATION_CODE})
def test_keypad_keys_pin_configuration_pull(circuitpython):
    """A PinConfiguration pull overrides the pull Keys would choose for that pin."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    # Only the pin pulled down reads low, which is pressed. P_03 gets the default pull-up.
    assert re.findall(r"key=(\d+) pressed=(True|False)", output) == [("1", "True")]
    assert "done" in output
