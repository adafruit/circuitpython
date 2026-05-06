from pathlib import Path

import serial
import subprocess
import threading
import time


class StdSerial:
    def __init__(self, stdin, stdout):
        self.stdin = stdin
        self.stdout = stdout

    def read(self, amount=None):
        data = self.stdout.read(amount)
        if data == b"":
            raise EOFError("stdout closed")
        return data

    def write(self, buf):
        if self.stdin is None:
            return
        self.stdin.write(buf)
        self.stdin.flush()

    def close(self):
        if self.stdin is not None:
            self.stdin.close()
        self.stdout.close()

    @property
    def in_waiting(self):
        if self.stdout is None:
            return 0
        return len(self.stdout.peek())


class SerialSaver:
    """Capture serial output in a background thread so output isn't missed."""

    def __init__(self, serial_obj, name="serial"):
        self.all_output = ""
        self.all_input = ""
        self.serial = serial_obj
        self.name = name

        self._stop = threading.Event()
        self._lock = threading.Lock()
        self._cv = threading.Condition(self._lock)
        self._reader = threading.Thread(target=self._reader_loop, daemon=True)
        self._reader.start()

    def _reader_loop(self):
        while not self._stop.is_set():
            try:
                read = self.serial.read(1)
            except Exception:
                # Serial port closed or device disconnected.
                break

            if read == b"":
                # Timeout with no data — keep waiting.  Only a real
                # exception or an explicit stop should end the loop.
                continue

            text = read.decode("utf-8", errors="replace")
            with self._cv:
                self.all_output += text
                self._cv.notify_all()
        in_waiting = 0
        try:
            in_waiting = self.serial.in_waiting
        except OSError:
            pass
        if in_waiting > 0:
            self.all_output += self.serial.read().decode("utf-8", errors="replace")

    def wait_for(self, text, timeout=10):
        with self._cv:
            while text not in self.all_output and self._reader.is_alive():
                if not self._cv.wait(timeout=timeout):
                    break
            if text not in self.all_output:
                tail = self.all_output[-400:]
                raise TimeoutError(
                    f"Timed out waiting for {text!r} on {self.name}. Output tail:\n{tail}"
                )

    def read(self, amount=None):
        # Kept for compatibility with existing callers.
        return

    def close(self):
        if not self.serial:
            return

        self._stop.set()
        self._reader.join(timeout=1.0)
        try:
            self.serial.close()
        except Exception:
            pass
        self.serial = None

    def write(self, text):
        self.all_input += text
        self.serial.write(text.encode("utf-8"))


class NativeSimProcess:
    def __init__(self, cmd, timeout=5, trace_file=None, env=None, flash_file=None):
        if trace_file:
            cmd.append(f"--trace-file={trace_file}")

        self._timeout = timeout
        self.trace_file = trace_file
        self.flash_file = flash_file
        print("Running", " ".join(cmd))
        self._proc = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=None,
            env=env,
        )
        if self._proc.stdout is None:
            raise RuntimeError("Failed to capture simulator stdout")

        # Discard the test warning
        uart_pty_line = self._proc.stdout.readline().decode("utf-8")
        if "connected to pseudotty:" not in uart_pty_line:
            raise RuntimeError("Failed to connect to UART")
        pty_path = uart_pty_line.strip().rsplit(":", maxsplit=1)[1].strip()
        self.serial = SerialSaver(
            serial.Serial(pty_path, baudrate=115200, timeout=0.05, write_timeout=0),
            name="uart0",
        )
        self.debug_serial = SerialSaver(
            StdSerial(self._proc.stdin, self._proc.stdout), name="debug"
        )

    def shutdown(self):
        if self._proc.poll() is None:
            self._proc.terminate()
            self._proc.wait(timeout=self._timeout)

        self.serial.close()
        self.debug_serial.close()

    def display_capture_paths(self) -> list[Path]:
        """Return paths to numbered PNG capture files produced by trace-driven capture."""
        pattern = getattr(self, "_capture_png_pattern", None)
        count = getattr(self, "_capture_count", 0)
        if not pattern or count == 0:
            return []
        return [Path(pattern % i) for i in range(count)]

    def wait_until_done(self):
        start_time = time.monotonic()
        while self._proc.poll() is None and time.monotonic() - start_time < self._timeout:
            time.sleep(0.01)
        self.shutdown()
