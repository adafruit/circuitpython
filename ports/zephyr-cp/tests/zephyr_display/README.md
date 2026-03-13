# Zephyr Display Golden Tests

This directory contains native_sim golden-image tests for the Zephyr-specific `zephyr_display` path.

## What is tested

- `board.DISPLAY` is present and usable.
- CircuitPython terminal/console tilegrids are attached to the default display root group.
- Deterministic console terminal output matches a checked-in golden image.
- `zephyr_display` pixel format constants are exposed.
- `displayio` rendering produces expected stripe colors at sampled pixel locations.

## Files

- `test_zephyr_display.py` – pytest tests.
- `golden/terminal_console_output_320x240.png` – console terminal output golden reference image.

## How capture works

These tests use trace-driven SDL display capture triggered by Perfetto instant events:

- `--input-trace=<path>` provides a Perfetto trace containing a `"display_capture"` track
  with instant events at the desired capture timestamps.
- `--display_capture_png=<path>` specifies the output PNG pattern (may contain `%d` for
  a sequence number).
- `--display_headless` runs SDL in headless/hidden-window mode (always enabled for native_sim tests).

The test harness sets these flags automatically when tests use
`@pytest.mark.display(capture_times_ns=[...])`.

## Regenerating the console golden image

```bash
rm -rf /tmp/zephyr-display-golden
pytest -q ports/zephyr-cp/tests/zephyr_display/test_zephyr_display.py::test_console_output_golden \
  --basetemp=/tmp/zephyr-display-golden
cp /tmp/zephyr-display-golden/test_console_output_golden0/frame_0.png \
  ports/zephyr-cp/tests/zephyr_display/golden/terminal_console_output_320x240.png
```

## Running the tests

```bash
pytest -q ports/zephyr-cp/tests/zephyr_display/test_zephyr_display.py
```
