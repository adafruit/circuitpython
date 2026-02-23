# CircuitPython Simulator Test Ideas

Test ideas for the native_sim simulator, organized by module/category.

## Core Python / Interpreter

### 1. Multiple file priority
Test that boot.py runs before code.py, and main.py is used as fallback when code.py doesn't exist.

```python
# boot.py
print("boot.py ran")

# code.py
print("code.py ran")
```

Expected output order: "boot.py ran" then "code.py ran"

### 2. Exception handling
Verify tracebacks print correctly with file/line info, and exception types propagate properly.

```python
def inner():
    raise ValueError("test error")

def outer():
    inner()

outer()
```

### 3. Memory / gc module
Test `gc.collect()`, `gc.mem_free()`, `gc.mem_alloc()`, and behavior under memory pressure.

```python
import gc
gc.collect()
free_before = gc.mem_free()
data = [0] * 1000
free_after = gc.mem_free()
assert free_after < free_before
del data
gc.collect()
free_final = gc.mem_free()
print("done")
```

### 4. Import system
Test importing frozen modules vs filesystem modules, verify import errors are clear.

```python
import sys
import board
import digitalio
print(f"modules loaded: {len(sys.modules)}")
print("done")
```

---

## digitalio

### 5. Input mode
Test reading GPIO input state. May require trace file injection or loopback configuration.

```python
import board
import digitalio

pin = digitalio.DigitalInOut(board.D0)
pin.direction = digitalio.Direction.INPUT
value = pin.value
print(f"input value: {value}")
print("done")
```

### 6. Pull resistors
Verify pull-up/pull-down configuration affects input readings.

```python
import board
import digitalio

pin = digitalio.DigitalInOut(board.D0)
pin.direction = digitalio.Direction.INPUT
pin.pull = digitalio.Pull.UP
value_up = pin.value
pin.pull = digitalio.Pull.DOWN
value_down = pin.value
print(f"pull-up: {value_up}, pull-down: {value_down}")
print("done")
```

### 7. Direction switching
Switch same pin between input/output modes multiple times.

```python
import board
import digitalio

pin = digitalio.DigitalInOut(board.D0)
pin.direction = digitalio.Direction.OUTPUT
pin.value = True
pin.direction = digitalio.Direction.INPUT
_ = pin.value
pin.direction = digitalio.Direction.OUTPUT
pin.value = False
print("done")
```

---

## time module

### 8. time.sleep() precision
Verify sleep timing via Perfetto trace timestamps. Use GPIO transitions as timing markers.

```python
import time
import board
import digitalio

led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT

led.value = True
time.sleep(0.05)  # 50ms
led.value = False
time.sleep(0.1)   # 100ms
led.value = True
time.sleep(0.05)  # 50ms
led.value = False
print("done")
```

Verify trace shows: 50ms high, 100ms low, 50ms high pattern.

### 9. time.monotonic()
Test monotonic clock increments correctly and never goes backward.

```python
import time

samples = []
for _ in range(10):
    samples.append(time.monotonic())
    time.sleep(0.01)

# Verify monotonic increase
for i in range(1, len(samples)):
    assert samples[i] > samples[i-1], "monotonic went backward!"

elapsed = samples[-1] - samples[0]
assert 0.08 < elapsed < 0.15, f"unexpected elapsed: {elapsed}"
print("done")
```

### 10. time.localtime / struct_time
Test time structure operations if available.

```python
import time

t = time.localtime()
print(f"year: {t.tm_year}")
print(f"month: {t.tm_mon}")
print(f"day: {t.tm_mday}")
print("done")
```

---

## microcontroller

### 11. microcontroller.cpu properties
Test frequency, temperature, UID properties.

```python
import microcontroller

print(f"frequency: {microcontroller.cpu.frequency}")
print(f"uid: {microcontroller.cpu.uid.hex()}")
print("done")
```

### 12. Safe mode
Trigger and verify safe mode entry (requires CONFIG_NATIVE_SIM_REBOOT=y).

```python
import microcontroller
# This would trigger safe mode - test carefully
# microcontroller.on_next_reset(microcontroller.RunMode.SAFE_MODE)
# microcontroller.reset()
print("done")
```

### 13. Reset reason
Test microcontroller.reset_reason after various reset types.

```python
import microcontroller

reason = microcontroller.reset_reason
print(f"reset reason: {reason}")
print("done")
```

---

## os module

### 14. os.uname()
Verify returns correct system info for native_sim.

```python
import os

info = os.uname()
print(f"sysname: {info.sysname}")
print(f"machine: {info.machine}")
print(f"release: {info.release}")
print("done")
```

### 15. Filesystem operations
Test os.listdir(), os.stat(), os.remove().

```python
import os

# List root
files = os.listdir("/")
print(f"root files: {files}")

# Stat a file
if "code.py" in files:
    stat = os.stat("/code.py")
    print(f"code.py size: {stat[6]}")

print("done")
```

### 16. os.getenv()
Test environment variable reading.

```python
import os

# May return None if not set
path = os.getenv("PATH")
print(f"PATH exists: {path is not None}")
print("done")
```

---

## board module

### 17. board.board_id
Verify board ID matches expected value.

```python
import board

print(f"board_id: {board.board_id}")
assert board.board_id == "native_native_sim"
print("done")
```

### 18. Pin availability
Test that board.LED and other defined pins exist.

```python
import board

assert hasattr(board, "LED"), "board.LED missing"
print(f"LED pin: {board.LED}")
print("done")
```

---

## Filesystem

### 19. File read/write
Create, read, and modify files on CIRCUITPY.

```python
# Write a test file
with open("/test.txt", "w") as f:
    f.write("hello world")

# Read it back
with open("/test.txt", "r") as f:
    content = f.read()

assert content == "hello world"
print("done")
```

### 20. Directory operations
Test mkdir, rmdir, nested paths.

```python
import os

# Create directory
os.mkdir("/testdir")
assert "testdir" in os.listdir("/")

# Create file in directory
with open("/testdir/file.txt", "w") as f:
    f.write("nested")

# Clean up
os.remove("/testdir/file.txt")
os.rmdir("/testdir")
assert "testdir" not in os.listdir("/")
print("done")
```

### 21. Large file handling
Test with larger files approaching flash limits.

```python
import gc

# Write 10KB file
data = "x" * 10240
with open("/large.txt", "w") as f:
    f.write(data)

# Verify size
import os
stat = os.stat("/large.txt")
assert stat[6] == 10240

# Clean up
os.remove("/large.txt")
gc.collect()
print("done")
```

---

## Error Conditions

### 22. Syntax error in code.py
Verify graceful error message when code.py has syntax error.

```python
# code.py with intentional syntax error:
def broken(
    print("missing close paren"
```

Expected: Clear syntax error message with line number.

### 23. Runtime error
Verify traceback format shows file, line, and function.

```python
def cause_error():
    x = 1 / 0

cause_error()
```

Expected: Traceback showing ZeroDivisionError with line info.

### 24. Keyboard interrupt
Test Ctrl+C handling via PTY if applicable.

```python
import time

print("starting long loop")
for i in range(100):
    print(f"iteration {i}")
    time.sleep(0.1)
print("done")
```

Send Ctrl+C during execution, verify clean KeyboardInterrupt.

---

## busio (if emulation available)

### 25. busio.UART
Basic UART operations if configured.

```python
import board
import busio

# Check if UART pins exist
if hasattr(board, "TX") and hasattr(board, "RX"):
    uart = busio.UART(board.TX, board.RX, baudrate=115200)
    uart.write(b"test")
    uart.deinit()
print("done")
```

### 26. busio.I2C scan
Scan for emulated I2C devices.

```python
import board
import busio

if hasattr(board, "SCL") and hasattr(board, "SDA"):
    i2c = busio.I2C(board.SCL, board.SDA)
    while not i2c.try_lock():
        pass
    devices = i2c.scan()
    print(f"I2C devices: {[hex(d) for d in devices]}")
    i2c.unlock()
    i2c.deinit()
print("done")
```

### 27. busio.SPI
Basic SPI transfer to emulated device.

```python
import board
import busio

if hasattr(board, "SCK") and hasattr(board, "MOSI"):
    spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=getattr(board, "MISO", None))
    while not spi.try_lock():
        pass
    spi.configure(baudrate=1000000)
    result = bytearray(4)
    spi.write_readinto(b"\x00\x00\x00\x00", result)
    spi.unlock()
    spi.deinit()
print("done")
```

---

## Serial/PTY Input

These tests require extending conftest.py to support writing to the PTY, not just reading.

### 28. Basic serial input
Test reading single characters from serial via PTY write.

```python
import sys

print("ready")
char = sys.stdin.read(1)
print(f"received: {repr(char)}")
print("done")
```

**Test harness**: After seeing "ready", write "A" to PTY, verify output shows "received: 'A'".

### 29. input() function
Test the built-in input() function with PTY input.

```python
print("ready")
name = input("Enter name: ")
print(f"hello {name}")
print("done")
```

**Test harness**: After seeing "Enter name:", write "World\n" to PTY, verify "hello World".

### 30. Serial line buffering
Test reading a complete line with newline termination.

```python
import sys

print("ready")
line = sys.stdin.readline()
print(f"got: {repr(line)}")
print("done")
```

**Test harness**: Write "test line\n" to PTY, verify complete line received.

### 31. usb_cdc.console data read
Test reading from usb_cdc.console.data if available.

```python
import usb_cdc

print("ready")
if usb_cdc.console:
    while not usb_cdc.console.in_waiting:
        pass
    data = usb_cdc.console.read(usb_cdc.console.in_waiting)
    print(f"console got: {data}")
print("done")
```

**Test harness**: Write bytes to PTY, verify they're received via usb_cdc.console.

### 32. REPL interaction
Test entering REPL mode and executing commands interactively.

```python
# No code.py - boots to REPL
```

**Test harness**:
1. Boot with empty/no code.py to get REPL prompt
2. Write "1 + 1\r\n" to PTY
3. Verify output contains "2"
4. Write "print('hello')\r\n"
5. Verify output contains "hello"

### 33. Ctrl+C interrupt via PTY
Test sending Ctrl+C (0x03) to interrupt running code.

```python
import time

print("starting")
for i in range(100):
    print(f"loop {i}")
    time.sleep(0.1)
print("completed")  # Should not reach this
```

**Test harness**:
1. Wait for "loop 5" in output
2. Write b"\x03" (Ctrl+C) to PTY
3. Verify KeyboardInterrupt raised
4. Verify "completed" NOT in output

### 34. Ctrl+D soft reload via PTY
Test sending Ctrl+D (0x04) to trigger soft reload.

```python
print("first run")
import time
time.sleep(10)  # Long sleep to allow interrupt
print("done")
```

**Test harness**:
1. Wait for "first run"
2. Write b"\x04" (Ctrl+D) to PTY
3. Verify code restarts (see "first run" again or reload message)

### 35. Serial input timeout
Test behavior when waiting for input with timeout.

```python
import sys
import select

print("ready")
# Poll for input with timeout
readable, _, _ = select.select([sys.stdin], [], [], 0.5)
if readable:
    data = sys.stdin.read(1)
    print(f"got: {repr(data)}")
else:
    print("timeout")
print("done")
```

**Test harness**: Don't send anything, verify "timeout" appears.

---

## Fixture Changes for PTY Input

The `run_circuitpython` fixture in conftest.py needs to be extended:

```python
@dataclass
class SimulatorResult:
    output: str
    trace_file: Path
    pty_write_fd: int  # New: file descriptor for writing to PTY

def _run(code: str | None, timeout: float = 5.0, ...) -> SimulatorResult:
    # Open PTY for both read AND write
    pty_fd = os.open(pty_path, os.O_RDWR | os.O_NONBLOCK)
    # ...
```

Or provide a callback/queue mechanism:

```python
def _run(code, timeout=5.0, input_sequence=None):
    """
    input_sequence: list of (trigger_text, bytes_to_send) tuples
    When trigger_text is seen in output, send bytes_to_send to PTY
    """
```

---

## Implementation Priority

Suggested order for implementation:

### Phase 1: Basic module tests (no fixture changes)
1. **#17 board.board_id** - Quick sanity check
2. **#14 os.uname()** - Tests another module, simple
3. **#9 time.monotonic()** - Core timing functionality
4. **#8 time.sleep() precision** - Builds on GPIO trace infrastructure
5. **#1 Multiple file priority** - Tests interpreter boot sequence
6. **#19 File read/write** - Tests filesystem layer
7. **#3 Memory / gc** - Tests memory management
8. **#2 Exception handling** - Tests error reporting

### Phase 2: PTY input tests (requires fixture extension)
9. **#28 Basic serial input** - Foundation for all input tests
10. **#33 Ctrl+C interrupt** - Important for interactive use
11. **#29 input() function** - Common user pattern
12. **#32 REPL interaction** - Tests interactive mode
