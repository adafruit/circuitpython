# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test digitalio functionality on native_sim."""

import re
from pathlib import Path

import pytest
from perfetto.trace_processor import TraceProcessor


DIGITALIO_INPUT_TRACE_READ_CODE = """\
import time
import digitalio
import microcontroller

pin = digitalio.DigitalInOut(microcontroller.pin.P_01)
pin.direction = digitalio.Direction.INPUT

start = time.monotonic()
last = pin.value
print(f"t_abs={time.monotonic():.3f} initial={last}")

# Poll long enough to observe a high pulse injected through input trace.
while time.monotonic() - start < 8.0:
    value = pin.value
    if value != last:
        print(f"t_abs={time.monotonic():.3f} edge={value}")
        last = value
    time.sleep(0.05)

print(f"t_abs={time.monotonic():.3f} done")
"""


DIGITALIO_INPUT_TRACE = {
    "gpio_emul.01": [
        (8_000_000_000, 0),
        (9_000_000_000, 1),
        (10_000_000_000, 0),
    ],
}


@pytest.mark.duration(14.0)
@pytest.mark.circuitpy_drive({"code.py": DIGITALIO_INPUT_TRACE_READ_CODE})
@pytest.mark.input_trace(DIGITALIO_INPUT_TRACE)
def test_digitalio_reads_input_trace(circuitpython):
    """Test DigitalInOut input reads values injected via input trace."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output

    initial_match = re.search(r"t_abs=([0-9]+\.[0-9]+) initial=False", output)
    edge_match = re.search(r"t_abs=([0-9]+\.[0-9]+) edge=True", output)
    done_match = re.search(r"t_abs=([0-9]+\.[0-9]+) done", output)

    assert initial_match is not None
    assert edge_match is not None
    assert done_match is not None

    initial_abs = float(initial_match.group(1))
    edge_abs = float(edge_match.group(1))
    done_abs = float(done_match.group(1))

    # Input trace edge is at 9.0s for gpio_emul.01.
    assert 8.5 <= edge_abs <= 9.5
    assert initial_abs <= edge_abs <= done_abs


BLINK_CODE = """\
import time
import board
import digitalio

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

for i in range(3):
    print(f"LED on {i}")
    led.value = True
    time.sleep(0.1)
    print(f"LED off {i}")
    led.value = False
    time.sleep(0.1)

print("done")
"""


def parse_gpio_trace(trace_file: Path, pin_name: str = "gpio_emul.00") -> list[tuple[int, int]]:
    """Parse GPIO trace from Perfetto trace file."""
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        f'''
        SELECT c.ts, c.value
        FROM counter c
        JOIN track t ON c.track_id = t.id
        WHERE t.name = "{pin_name}"
        ORDER BY c.ts
        '''
    )
    return [(row.ts, int(row.value)) for row in result]


@pytest.mark.circuitpy_drive({"code.py": BLINK_CODE})
def test_digitalio_blink_output(circuitpython):
    """Test blink program produces expected output and GPIO traces."""
    circuitpython.wait_until_done()

    # Check serial output
    output = circuitpython.serial.all_output
    assert "LED on 0" in output
    assert "LED off 0" in output
    assert "LED on 2" in output
    assert "LED off 2" in output
    assert "done" in output

    # Check GPIO traces - LED is on gpio_emul.00
    gpio_trace = parse_gpio_trace(circuitpython.trace_file, "gpio_emul.00")

    # Deduplicate by timestamp (keep last value at each timestamp)
    by_timestamp = {}
    for ts, val in gpio_trace:
        by_timestamp[ts] = val
    sorted_trace = sorted(by_timestamp.items())

    # Find transition points (where value changes), skipping initialization at ts=0
    transitions = []
    for i in range(1, len(sorted_trace)):
        prev_ts, prev_val = sorted_trace[i - 1]
        curr_ts, curr_val = sorted_trace[i]
        if prev_val != curr_val and curr_ts > 0:
            transitions.append((curr_ts, curr_val))

    # We expect at least 6 transitions (3 on + 3 off) from the blink loop
    assert len(transitions) >= 6, f"Expected at least 6 transitions, got {len(transitions)}"

    # Verify timing between consecutive transitions
    # Each sleep is 0.1s = 100ms = 100,000,000 ns
    expected_interval_ns = 100_000_000
    tolerance_ns = 20_000_000  # 20ms tolerance

    # Find a sequence of 6 consecutive transitions with ~100ms intervals (the blink loop)
    # This filters out initialization and cleanup noise
    blink_transitions = []
    for i in range(len(transitions) - 1):
        interval = transitions[i + 1][0] - transitions[i][0]
        if abs(interval - expected_interval_ns) < tolerance_ns:
            if not blink_transitions:
                blink_transitions.append(transitions[i])
            blink_transitions.append(transitions[i + 1])
        elif blink_transitions:
            # Found end of blink sequence
            break

    assert len(blink_transitions) >= 6, (
        f"Expected at least 6 blink transitions with ~100ms intervals, got {len(blink_transitions)}"
    )

    # Verify timing between blink transitions
    for i in range(1, min(6, len(blink_transitions))):
        prev_ts = blink_transitions[i - 1][0]
        curr_ts = blink_transitions[i][0]
        interval = curr_ts - prev_ts
        assert abs(interval - expected_interval_ns) < tolerance_ns, (
            f"Transition interval {interval / 1_000_000:.1f}ms deviates from "
            f"expected {expected_interval_ns / 1_000_000:.1f}ms by more than "
            f"{tolerance_ns / 1_000_000:.1f}ms tolerance"
        )


PIN_CONFIGURATION_CODE = """\
import digitalio
import microcontroller
from microcontroller import PinConfiguration

for pull in (digitalio.Pull.UP, digitalio.Pull.DOWN, digitalio.Pull.UP):
    pin = digitalio.DigitalInOut(PinConfiguration(microcontroller.pin.P_01, pull=pull))
    print(f"pull={pin.pull} value={pin.value}")
    pin.deinit()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": PIN_CONFIGURATION_CODE})
def test_digitalio_pin_configuration_pull(circuitpython):
    """DigitalInOut starts with the pull given by a PinConfiguration."""
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert re.findall(r"pull=(\S+) value=(True|False)", output) == [
        ("digitalio.Pull.UP", "True"),
        ("digitalio.Pull.DOWN", "False"),
        ("digitalio.Pull.UP", "True"),
    ]
    assert "done" in output
