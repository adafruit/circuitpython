# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test audiobusio I2SOut functionality on native_sim."""

from pathlib import Path

import pytest
from perfetto.trace_processor import TraceProcessor


I2S_PLAY_CODE = """\
import array
import math
import audiocore
import board
import time

# Generate a 440 Hz sine wave, 16-bit signed stereo at 16000 Hz
sample_rate = 16000
length = sample_rate // 440  # ~36 samples per period
values = []
for i in range(length):
    v = int(math.sin(math.pi * 2 * i / length) * 30000)
    values.append(v)  # left
    values.append(v)  # right

sample = audiocore.RawSample(
    array.array("h", values),
    sample_rate=sample_rate,
    channel_count=2,
)

dac = board.I2S0()
print("playing")
dac.play(sample, loop=True)
time.sleep(0.5)
dac.stop()
print("stopped")
print("done")
"""


def parse_i2s_trace(trace_file: Path, track_name: str) -> list[tuple[int, int]]:
    """Parse I2S counter trace from Perfetto trace file."""
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        f"""
        SELECT c.ts, c.value
        FROM counter c
        JOIN track t ON c.track_id = t.id
        WHERE t.name LIKE "%{track_name}"
        ORDER BY c.ts
        """
    )
    return [(int(row.ts), int(row.value)) for row in result]


@pytest.mark.duration(10)
@pytest.mark.circuitpy_drive({"code.py": I2S_PLAY_CODE})
def test_i2s_play_and_stop(circuitpython):
    """Test I2SOut play and stop produce expected output and correct waveform traces."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "playing" in output
    assert "stopped" in output
    assert "done" in output

    # Check that Perfetto trace has I2S counter tracks with data
    left_trace = parse_i2s_trace(circuitpython.trace_file, "Left")
    right_trace = parse_i2s_trace(circuitpython.trace_file, "Right")

    # Should have counter events (initial zero + audio data)
    assert len(left_trace) > 10, f"Expected many Left channel events, got {len(left_trace)}"
    assert len(right_trace) > 10, f"Expected many Right channel events, got {len(right_trace)}"

    # Verify timestamps are spread out (not all the same)
    left_timestamps = [ts for ts, _ in left_trace]
    assert left_timestamps[-1] > left_timestamps[1], "Timestamps should increase over time"
    time_span_ns = left_timestamps[-1] - left_timestamps[1]
    # We play for 0.5s, so span should be at least 100ms
    assert time_span_ns > 100_000_000, f"Expected >100ms time span, got {time_span_ns / 1e6:.1f}ms"

    # Audio data should contain non-zero values (sine wave)
    # Skip the initial zero value
    left_values = [v for _, v in left_trace if v != 0]
    right_values = [v for _, v in right_trace if v != 0]
    assert len(left_values) > 5, "Left channel has too few non-zero values"
    assert len(right_values) > 5, "Right channel has too few non-zero values"

    # Sine wave should have both positive and negative values
    assert any(v > 0 for v in left_values), "Left channel has no positive values"
    assert any(v < 0 for v in left_values), "Left channel has no negative values"

    # Verify amplitude is in the expected range (we generate with amplitude 30000)
    max_left = max(left_values)
    min_left = min(left_values)
    assert max_left > 20000, f"Left max {max_left} too low, expected >20000"
    assert min_left < -20000, f"Left min {min_left} too high, expected <-20000"

    # Left and right should match (we write the same value to both channels)
    # Compare a subset of matching timestamps
    left_by_ts = dict(left_trace)
    right_by_ts = dict(right_trace)
    common_ts = sorted(set(left_by_ts.keys()) & set(right_by_ts.keys()))
    mismatches = 0
    for ts in common_ts[:100]:
        if left_by_ts[ts] != right_by_ts[ts]:
            mismatches += 1
    assert mismatches == 0, (
        f"{mismatches} L/R mismatches in first {min(100, len(common_ts))} common timestamps"
    )


I2S_PLAY_NO_STOP_CODE = """\
import array
import math
import audiocore
import board
import time

sample_rate = 16000
length = sample_rate // 440
values = []
for i in range(length):
    v = int(math.sin(math.pi * 2 * i / length) * 30000)
    values.append(v)
    values.append(v)

sample = audiocore.RawSample(
    array.array("h", values),
    sample_rate=sample_rate,
    channel_count=2,
)

dac = board.I2S0()
dac.play(sample, loop=True)
print("playing")
time.sleep(0.2)
# Exit without calling dac.stop() — reset_port should clean up
print("exiting")
"""


@pytest.mark.duration(15)
@pytest.mark.code_py_runs(2)
@pytest.mark.circuitpy_drive({"code.py": I2S_PLAY_NO_STOP_CODE})
def test_i2s_stops_on_code_exit(circuitpython):
    """Test I2S is stopped by reset_port when code.py exits without explicit stop."""
    # First run: plays audio then exits without stopping
    circuitpython.serial.wait_for("exiting")
    circuitpython.serial.wait_for("Press any key to enter the REPL")
    # Trigger soft reload
    circuitpython.serial.write("\x04")

    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    # Should see "playing" and "exiting" at least twice (once per run)
    assert output.count("playing") >= 2
    assert output.count("exiting") >= 2


I2S_PAUSE_RESUME_CODE = """\
import array
import math
import audiocore
import board
import time

sample_rate = 16000
length = sample_rate // 440
values = []
for i in range(length):
    v = int(math.sin(math.pi * 2 * i / length) * 30000)
    values.append(v)
    values.append(v)

sample = audiocore.RawSample(
    array.array("h", values),
    sample_rate=sample_rate,
    channel_count=2,
)

dac = board.I2S0()
dac.play(sample, loop=True)
print("playing")
time.sleep(0.2)

dac.pause()
print("paused")
assert dac.paused
time.sleep(0.1)

dac.resume()
print("resumed")
assert not dac.paused
time.sleep(0.2)

dac.stop()
print("done")
"""


@pytest.mark.duration(10)
@pytest.mark.circuitpy_drive({"code.py": I2S_PAUSE_RESUME_CODE})
def test_i2s_pause_resume(circuitpython):
    """Test I2SOut pause and resume work correctly."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "playing" in output
    assert "paused" in output
    assert "resumed" in output
    assert "done" in output
