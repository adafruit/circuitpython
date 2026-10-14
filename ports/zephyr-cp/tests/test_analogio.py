# SPDX-FileCopyrightText: 2026 Scott Shawcroft for Adafruit Industries LLC
# SPDX-License-Identifier: MIT

"""Test analogio functionality on native_sim."""

import re

import pytest
from perfetto.trace_processor import TraceProcessor


ANALOGIO_TRACE_CODE = """\
import time
import analogio
import microcontroller

adc0 = analogio.AnalogIn(microcontroller.pin.P_00)
adc1 = analogio.AnalogIn(microcontroller.pin.P_01)
adc2 = analogio.AnalogIn(microcontroller.pin.P_02)

print(f"t0 {adc0.value} {adc1.value} {adc2.value}")

time.sleep(2.2)
print(f"t1 {adc0.value} {adc1.value} {adc2.value}")

time.sleep(0.5)
print(f"t2 {adc0.value} {adc1.value} {adc2.value}")

time.sleep(0.6)
print(f"t3 {adc0.value} {adc1.value} {adc2.value}")

time.sleep(1.0)
print(f"t4 {adc0.value} {adc1.value} {adc2.value}")
print("done")
"""


ANALOGIO_INPUT_TRACE = {
    # Counter tracks carry millivolts, the same unit the Zephyr ADC emul's
    # value functions use. Trace timestamps are absolute simulated time; the
    # simulator boots at ~2s, and code.py reads at ~2.0s (t0), ~4.2s (t1),
    # ~4.7s (t2), ~5.3s (t3) and ~6.3s (t4). The step lands before t3 so all
    # channels read full scale from t3 on. AnalogIn reports 16-bit codes
    # relative to the 3300 mV internal reference.
    "adc.0": [
        (3_000_000_000, 660),  # 1/5 of full scale
        (5_000_000_000, 3300),
    ],
    "adc.1": [
        (3_500_000_000, 1320),  # 2/5
        (5_000_000_000, 3300),
    ],
    "adc.2": [
        (4_000_000_000, 1980),  # 3/5
        (5_000_000_000, 3300),
    ],
}


def _code_of_mv(mv: int) -> int:
    """Code AnalogIn reports for an input in millivolts: the ADC emul converts
    the voltage through the 3300 mV internal reference to the read's bit
    depth, truncating like a real converter."""
    return (mv * 65535) // 3300


def _raw14_of_mv(mv: int) -> int:
    """Raw 14-bit code the ADC emul reports for an input in millivolts when
    --adc-resolution narrows the emulated converter to 14 bits."""
    return (mv * 16383) // 3300


def _mirror_top_bits(raw: int, bits: int) -> int:
    """16-bit code a raw *bits*-bit reading is expanded to: the top bits are
    replicated into the bottom (atmel-samd style), so zero and full scale are
    preserved."""
    shift = 16 - bits
    return (raw << shift) | (raw >> (bits - shift))


def _scale_bits(value: int, from_bits: int, to_bits: int) -> int:
    from_max = (1 << from_bits) - 1
    to_max = (1 << to_bits) - 1
    return (value * to_max + (from_max // 2)) // from_max


def _mv_of_code(code: int, bits: int = 16) -> int:
    """Millivolts the DAC track records for a written code at *bits*
    resolution: the emulated DAC converts the quantized code through the
    3300 mV reference with the same rounding a converter applies."""
    out_max = (1 << bits) - 1
    return (code * 3300 + out_max // 2) // out_max


@pytest.mark.duration(12.0)
@pytest.mark.circuitpy_drive({"code.py": ANALOGIO_TRACE_CODE})
@pytest.mark.input_trace(ANALOGIO_INPUT_TRACE)
def test_analogio_reads_trace_millivolts(circuitpython):
    """ADC values from the trace read as 16-bit codes relative to the reference."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output

    t0 = re.search(r"t0 (\d+) (\d+) (\d+)", output)
    t1 = re.search(r"t1 (\d+) (\d+) (\d+)", output)
    t2 = re.search(r"t2 (\d+) (\d+) (\d+)", output)
    t3 = re.search(r"t3 (\d+) (\d+) (\d+)", output)
    t4 = re.search(r"t4 (\d+) (\d+) (\d+)", output)

    assert t0 is not None
    assert t1 is not None
    assert t2 is not None
    assert t3 is not None
    assert t4 is not None
    assert "done" in output

    t0_values = tuple(int(v) for v in t0.groups())
    t1_values = tuple(int(v) for v in t1.groups())
    t2_values = tuple(int(v) for v in t2.groups())
    t3_values = tuple(int(v) for v in t3.groups())
    t4_values = tuple(int(v) for v in t4.groups())

    expected_mid = (_code_of_mv(660), _code_of_mv(1320), _code_of_mv(1980))
    expected_max = (65535, 65535, 65535)

    assert t0_values == (0, 0, 0)
    assert t1_values == expected_mid
    assert t2_values == expected_mid
    assert t3_values == expected_max
    assert t4_values == expected_max


@pytest.mark.duration(12.0)
@pytest.mark.native_sim_args("--adc-resolution=14")
@pytest.mark.circuitpy_drive({"code.py": ANALOGIO_TRACE_CODE})
@pytest.mark.input_trace(ANALOGIO_INPUT_TRACE)
def test_analogio_14bit_reads_mirror_top_bits(circuitpython):
    """Reads from a narrowed emulated ADC (like CircuitPython's 14-bit SAADC
    hardware reads) stretch to the 16-bit contract by replicating the top
    bits into the bottom (atmel-samd style), not by scaling."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output

    t1 = re.search(r"t1 (\d+) (\d+) (\d+)", output)
    t3 = re.search(r"t3 (\d+) (\d+) (\d+)", output)
    assert t1 is not None and t3 is not None
    assert "done" in output

    t1_values = tuple(int(v) for v in t1.groups())
    t3_values = tuple(int(v) for v in t3.groups())

    expected_mid = tuple(_mirror_top_bits(_raw14_of_mv(mv), 14) for mv in (660, 1320, 1980))
    assert t1_values == expected_mid
    # The mirroring keeps zero and full scale exact.
    assert t3_values == (65535, 65535, 65535)
    # And the mirrored codes differ from plain scaling, so the test can tell
    # the two apart: 1320 mV reads 6553 *at 14 bits* -> 26213 mirrored instead
    # of the scaled 26214.
    assert t1_values[1] == 26213


ANALOGOUT_TRACE_CODE = """\
import time
import analogio
import microcontroller

out = analogio.AnalogOut(microcontroller.pin.P_00)
out.value = 0

time.sleep(0.1)
out.value = 12345

time.sleep(0.1)
out.value = 65535

print("done")
"""


def _parse_counter_track(trace_file, track_name: str) -> list[tuple[int, int]]:
    tp = TraceProcessor(file_path=str(trace_file))
    result = tp.query(
        f'''
        SELECT c.ts, c.value
        FROM counter c
        JOIN track t ON c.track_id = t.id
        WHERE t.name = "{track_name}"
        ORDER BY c.ts
        '''
    )
    return [(int(row.ts), int(row.value)) for row in result]


@pytest.mark.duration(8.0)
@pytest.mark.circuitpy_drive({"code.py": ANALOGOUT_TRACE_CODE})
def test_analogout_writes_counter_trace(circuitpython):
    """Writes appear on the track as millivolts through the 3300 mV reference."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    trace = _parse_counter_track(circuitpython.trace_file, "dac.0")
    values = [value for _ts, value in trace]

    half_scale_mv = _mv_of_code(12345)
    full_scale_mv = _mv_of_code(65535)
    assert full_scale_mv == 3300
    assert int(half_scale_mv) in values
    assert int(full_scale_mv) in values
    assert values.index(half_scale_mv) < values.index(full_scale_mv)


@pytest.mark.duration(8.0)
@pytest.mark.native_sim_args("--dac-resolution=8")
@pytest.mark.circuitpy_drive({"code.py": ANALOGOUT_TRACE_CODE})
def test_analogout_resolution_override(circuitpython):
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "done" in output

    trace = _parse_counter_track(circuitpython.trace_file, "dac.0")
    values = [value for _ts, value in trace]

    # The quantized 8-bit code maps to a different millivolt value than the
    # unquantized 16-bit code (621 vs 622), so the override is observable.
    assert _mv_of_code(_scale_bits(12345, 16, 8), 8) in values
    assert _mv_of_code(_scale_bits(65535, 16, 8), 8) in values
