# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test LED blink functionality on native_sim."""

from conftest import InputTrigger, parse_gpio_trace


def test_blank_flash_hello_world(run_circuitpython):
    """Test that an erased flash shows code.py output header."""
    result = run_circuitpython(None, timeout=4, erase_flash=True)

    assert "Board ID:native_native_sim" in result.output
    assert "UID:" in result.output
    assert "code.py output:" in result.output
    assert "Hello World" in result.output
    assert "done" in result.output


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


def test_blink_output(run_circuitpython):
    """Test blink program produces expected output and GPIO traces."""
    result = run_circuitpython(BLINK_CODE, timeout=5)

    # Check serial output
    assert "LED on 0" in result.output
    assert "LED off 0" in result.output
    assert "LED on 2" in result.output
    assert "LED off 2" in result.output
    assert "done" in result.output

    # Check GPIO traces - LED is on gpio_emul.00
    gpio_trace = parse_gpio_trace(result.trace_file, "gpio_emul.00")

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


# --- PTY Input Tests ---


INPUT_CODE = """\
import sys

print("ready")
char = sys.stdin.read(1)
print(f"received: {repr(char)}")
print("done")
"""


def test_basic_serial_input(run_circuitpython):
    """Test reading single character from serial via PTY write."""
    result = run_circuitpython(
        INPUT_CODE,
        timeout=5.0,
        input_sequence=[InputTrigger(trigger="ready", data=b"A")],
    )

    assert "ready" in result.output
    assert "received: 'A'" in result.output
    assert "done" in result.output


INPUT_FUNC_CODE = """\
print("ready")
name = input("Enter name: ")
print(f"hello {name}")
print("done")
"""


def test_input_function(run_circuitpython):
    """Test the built-in input() function with PTY input."""
    result = run_circuitpython(
        INPUT_FUNC_CODE,
        timeout=5.0,
        input_sequence=[InputTrigger(trigger="Enter name:", data=b"World\r")],
    )

    assert "ready" in result.output
    assert "Enter name:" in result.output
    assert "hello World" in result.output
    assert "done" in result.output


INTERRUPT_CODE = """\
import time

print("starting")
for i in range(100):
    print(f"loop {i}")
    time.sleep(0.1)
print("completed")
"""


def test_ctrl_c_interrupt(run_circuitpython):
    """Test sending Ctrl+C (0x03) to interrupt running code."""
    result = run_circuitpython(
        INTERRUPT_CODE,
        timeout=15.0,
        input_sequence=[InputTrigger(trigger="loop 5", data=b"\x03")],
    )

    assert "starting" in result.output
    assert "loop 5" in result.output
    assert "KeyboardInterrupt" in result.output
    assert "completed" not in result.output


RELOAD_CODE = """\
print("first run")
import time
time.sleep(1)
print("done")
"""


def test_ctrl_d_soft_reload(run_circuitpython):
    """Test sending Ctrl+D (0x04) to trigger soft reload."""
    result = run_circuitpython(
        RELOAD_CODE,
        timeout=10.0,
        input_sequence=[InputTrigger(trigger="first run", data=b"\x04")],
    )

    # Should see "first run" appear multiple times due to reload
    # or see a soft reboot message
    assert "first run" in result.output
    # The soft reload should restart the code before "done" is printed
    assert "done" in result.output
    assert result.output.count("first run") > 1
