# Testing: STM32F405 / STM32F407 DAC AudioOut

These tests exercise the DAC-based `audioio.AudioOut` implementation added for STM32F405xx and STM32F407xx.

## Automated vs Manual Tests

| Test | Automated | Requires audio/scope |
|------|-----------|----------------------|
| 1 — WAV File Playback | Yes | Yes (audio check) |
| 2 — Pause / Resume | Yes | Yes (audio check) |
| 3 — Looping Sine Wave | Yes | Yes (audio check) |
| 4 — deinit and Re-init | Yes | No |
| 5 — Soft Reset Cleanup | No (manual Ctrl-C/D) | No |

`run_serial_tests.py` automates Tests 1, 2, 3, and 4: it copies the necessary
files to the board and runs each script over the serial REPL, comparing the
printed output to the expected patterns.  You still need to listen to the audio
(and optionally use an oscilloscope) for the audio-quality checks.

### Quick start

```bash
# Install the one dependency (if not already present)
pip install mpremote

# Run all automated tests (board must be connected and CIRCUITPY mounted)
python3 tests/circuitpython-manual/audioio/run_serial_tests.py

# Skip file copy if files are already on the board
python3 tests/circuitpython-manual/audioio/run_serial_tests.py --no-copy

# Override the serial port or CIRCUITPY path
python3 tests/circuitpython-manual/audioio/run_serial_tests.py \
    --port /dev/cu.usbmodem1234 \
    --circuitpy /Volumes/CIRCUITPY

# Run only specific tests
python3 tests/circuitpython-manual/audioio/run_serial_tests.py --tests 3,4
```

The script exits 0 if all selected tests pass, 1 otherwise — suitable for CI.

---

## Hardware Required

- An STM32F405 or STM32F407 board running the freshly-built firmware (e.g. Feather STM32F405 Express).
- A passive speaker or audio amplifier connected to **pin A0 (PA04, DAC channel 1)** and GND.
  - A simple test: 100 Ω resistor in series with a small speaker between A0 and GND.
  - For better audio: connect A0 to a small amp module (e.g. PAM8403) then to a speaker.
- Optional: an oscilloscope or logic analyser probe on **D4** (used as a trigger output by the test scripts).
- A USB cable for REPL/filesystem access (CircuitPython storage must not be read-only).

## Build

Enable the feature by building for an F405 or F407 target:

```
make -C ports/stm BOARD=feather_stm32f405_express -j CROSS_COMPILE=~/arm-toolchain/arm-gnu-toolchain-14.3.rel1-darwin-arm64-arm-none-eabi/bin/arm-none-eabi- PYTHON=/opt/homebrew/bin/python3
```

`CIRCUITPY_AUDIOIO` is now set to `1` automatically for those variants. Verify it is present in the build:

```
grep CIRCUITPY_AUDIOIO ports/stm/build-feather_stm32f405_express/mpconfigport.mk
# Expected output: CIRCUITPY_AUDIOIO = 1
```

Flash the resulting `.bin` to the board using your preferred method (e.g. `dfu-util`).

## File Setup

> **If using `run_serial_tests.py`** this step is done automatically — skip ahead.

Copy the WAV test samples onto the board's `CIRCUITPY` drive root:

```
cp \
  tests/circuitpython-manual/audiocore/jeplayer-splash-8000-8bit-mono-unsigned.wav \
  tests/circuitpython-manual/audiocore/jeplayer-splash-8000-16bit-mono-signed.wav \
  tests/circuitpython-manual/audiocore/jeplayer-splash-44100-16bit-mono-signed.wav \
  /Volumes/CIRCUITPY/
```

These three files (~447 KB total) cover every exercised code path:
- `8000-8bit-mono-unsigned` — 8-bit unsigned decode path, audibly lo-fi
- `8000-16bit-mono-signed` — 16-bit signed decode path at lowest sample rate
- `44100-16bit-mono-signed` — 16-bit at highest sample rate (DMA reconfiguration, audible quality difference)

Stereo, 24-bit, and the 16 kHz files are omitted: stereo and 24-bit will `OSError`; 16 kHz adds no new code paths over 8 kHz and 44.1 kHz.

Copy the three test scripts to the board as well (or paste them into the REPL):

```
cp tests/circuitpython-manual/audioio/wavefile_playback.py      /Volumes/CIRCUITPY/
cp tests/circuitpython-manual/audioio/wavefile_pause_resume.py  /Volumes/CIRCUITPY/
cp tests/circuitpython-manual/audioio/single_buffer_loop.py     /Volumes/CIRCUITPY/
```

## Test 1 — WAV File Playback (`wavefile_playback.py`) *(automated)*

Verifies that `AudioOut` can play WAV files at 8 kHz, 16 kHz, and 44.1 kHz in mono and stereo (only left channel output), with 8-bit unsigned and 16-bit signed encodings.

**Note:** 24-bit WAV files and the stereo MP3 are intentionally excluded — `audiocore.WaveFile` does not support 24-bit, and those files will print an `OSError`. That is expected.

**Run from the REPL:**

```python
import os
os.chdir("/")
exec(open("wavefile_playback.py").read())
```

**Expected output (order may vary by filename sort):**

```
playing jeplayer-splash-44100-16bit-mono-signed.wav
playing jeplayer-splash-8000-16bit-mono-signed.wav
playing jeplayer-splash-8000-8bit-mono-unsigned.wav
done
```

**What to listen for:**

- Each supported WAV plays the "jeplayer splash" jingle to completion before the next starts.
- No loud pops at the start or end of each file (the DAC ramp-in / ramp-out should suppress them).
- Audio pitch should match the sample rate: the 44100 Hz file sounds the most natural; the 8000 Hz file sounds lower fidelity.

## Test 2 — Pause and Resume (`wavefile_pause_resume.py`) *(automated)*

Verifies `AudioOut.pause()` / `AudioOut.resume()` by toggling every 100 ms during playback. The audio will sound choppy — that is intentional.

**Run from the REPL:**

```python
exec(open("wavefile_pause_resume.py").read())
```

**Expected output (repeating for each WAV):**

```
playing with pause/resume: jeplayer-splash-44100-16bit-mono-signed.wav
  paused
  resumed
  ...
playing with pause/resume: jeplayer-splash-8000-16bit-mono-signed.wav
  paused
  resumed
  ...
playing with pause/resume: jeplayer-splash-8000-8bit-mono-unsigned.wav
  paused
  resumed
  ...
done
```

**What to verify:**

- The REPL prints alternating "paused" / "resumed" lines.
- Audio cuts in and out in sync with the prints.
- Playback eventually completes (the `while dac.playing` loop exits normally).
- No hard fault or hang.

## Test 3 — Looping Sine Wave (`single_buffer_loop.py`) *(automated)*

Verifies `RawSample` with `loop=True` and tests all four sample formats:
`unsigned 8-bit`, `signed 8-bit`, `unsigned 16-bit`, `signed 16-bit`.

Each sample generates one cycle of a 440 Hz sine wave and loops for 1 second.

**Run from the REPL:**

```python
exec(open("single_buffer_loop.py").read())
```

**Expected output:**

```
unsigned 8 bit

signed 8 bit

unsigned 16 bit

signed 16 bit

done
```

**What to listen for:**

- A 440 Hz tone (concert A) for approximately 1 second for each format.
- All four formats should sound essentially identical in pitch and volume.
- No pops or glitches during the loop.
- Clean silence between tones (quiescent DAC value holds between `stop()` calls).

## Test 4 — `deinit` and Re-init *(automated)*

Verifies that `AudioOut` can be deconstructed and reconstructed without rebooting, and that pin A0 is properly released.

```python
import audioio, analogio, board

# Construct and immediately deinit AudioOut
dac = audioio.AudioOut(board.A0)
dac.deinit()

# PA04 should now be free for AnalogOut
aout = analogio.AnalogOut(board.A0)
aout.value = 32768  # mid-scale
aout.deinit()

# Re-create AudioOut on the same pin
dac2 = audioio.AudioOut(board.A0)
dac2.deinit()
print("pass")
```

**Expected output:** `pass` with no exceptions.

## Test 5 — Soft Reset Cleanup *(manual)*

Verifies that `audioout_reset()` properly cleans up when the REPL soft-resets during active playback.

1. Start a looping tone in the REPL:

```python
import audiocore, audioio, board, array, math, time
length = 8000 // 440
s16 = array.array("h", [int(math.sin(math.pi * 2 * i / length) * 32767) for i in range(length)])
dac = audioio.AudioOut(board.A0)
dac.play(audiocore.RawSample(s16, sample_rate=8000), loop=True)
```

2. While the tone is playing, press **Ctrl-C** then **Ctrl-D** (soft reset).

**Expected:**
- **Ctrl-C** interrupts the Python code but the tone *keeps playing* — this is normal. The DMA and TIM6 run in hardware independently of Python; a `KeyboardInterrupt` does not stop them.
- **Ctrl-D** triggers a soft reset which calls `audioout_reset()`. The tone stops immediately and the board returns to the `>>>` prompt with no crash or fault.
- Running any of the above tests again afterwards should work normally.

## Oscilloscope Checks (Optional)

Each test script drives `board.D4` (pin D4) low at the start of each playback and high when it ends. This provides a clean trigger edge for a scope.

- **Test 1:** Probe A0 — should show a sampled waveform at the correct sample rate. Probe D4 for a gate signal that spans the file duration.
- **Test 3:** Probe A0 — should show a 440 Hz staircase-sine at the DAC output (12-bit steps visible at 44.1 kHz; fewer at 8 kHz). A simple RC low-pass filter (1 kΩ + 100 nF) on the A0 output will smooth the staircase significantly.

## Known Limitations

- **Right channel / stereo output** is not implemented. Passing `right_channel` to `AudioOut()` raises `ValueError: Stereo not supported on this board`.
- **Only pin A0 (PA04)** is supported as the left channel. Any other pin raises `ValueError: AudioOut requires pin A0 (PA04)`.
- **24-bit WAV files** are not supported by `audiocore.WaveFile` and will raise `OSError` when opened.
- Only one `AudioOut` instance can be active at a time (single DAC channel).
