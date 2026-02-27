"""
Manual test for busio.UART nonblocking start_write()/write_is_busy(), and
partial read timeout behavior.

Hardware / wiring:
- Connect TX to RX on the same board (loopback wire).
- Optional but recommended: common GND reference if using external transceiver.

Pin assumptions:
- Uses board.TX and board.RX.

What this covers:
1) Correct operation path: nonblocking write and full readback on loopback.
2) Failure/timeout path: request more bytes than sent and verify readinto()
   returns a partial count after timeout.
3) Large-transfer path: verify start_write() is nonblocking for a large buffer.

Expected behavior:
- start_write() returns a transfer_state token.
- write_is_busy(token) is True until the transfer completes.
- readinto(buf) returns the number of bytes read; with timeout and partial data,
  that number should be >0 and <len(buf).
- For large writes, start_write() should return quickly and transfer should
    still be busy immediately after return.
"""

import board
import busio
import time


LONG_WRITE_LEN = 4096


def wait_for_write_done(uart, state, timeout_s=1.0):
    deadline = time.monotonic() + timeout_s
    while uart.write_is_busy(state):
        if time.monotonic() >= deadline:
            raise RuntimeError("timed out waiting for nonblocking UART write")


def clear_rx(uart):
    while uart.in_waiting:
        uart.read(uart.in_waiting)


def read_exact(uart, nbytes, timeout_s=3.0):
    out = bytearray()
    deadline = time.monotonic() + timeout_s
    while len(out) < nbytes:
        chunk = uart.read(nbytes - len(out))
        if chunk:
            out.extend(chunk)
            deadline = time.monotonic() + timeout_s
        if time.monotonic() >= deadline:
            break
    return bytes(out)


def main():
    if not hasattr(busio.UART, "start_write") or not hasattr(busio.UART, "write_is_busy"):
        print("SKIP: nonblocking UART API not available on this build")
        return

    uart = busio.UART(tx=board.TX, rx=board.RX, baudrate=115200, timeout=0.05)
    time.sleep(0.05)

    try:
        clear_rx(uart)

        payload = b"CP-NONBLOCK-OK"
        state = uart.start_write(payload)
        wait_for_write_done(uart, state)

        rx = uart.read(len(payload))
        if rx == payload:
            print("PASS: nonblocking UART write/read loopback operation")
        else:
            print("FAIL: loopback mismatch", rx, payload)
            return

        clear_rx(uart)

        partial_payload = b"XY"
        state = uart.start_write(partial_payload)
        wait_for_write_done(uart, state)

        target_len = 6
        read_buf = bytearray(target_len)
        count = uart.readinto(read_buf)

        if count is None:
            print("FAIL: readinto() returned None, expected partial count")
            return

        if 0 < count < target_len and bytes(read_buf[:count]) == partial_payload:
            print("PASS: partial-timeout read returned", count, "bytes:", bytes(read_buf[:count]))
        else:
            print("FAIL: partial-timeout behavior unexpected")
            print("  count:", count)
            print("  data:", bytes(read_buf[: count or 0]))
            return

        clear_rx(uart)

        long_payload = bytearray(LONG_WRITE_LEN)
        for idx in range(LONG_WRITE_LEN):
            long_payload[idx] = idx & 0xFF

        t0 = time.monotonic()
        state = uart.start_write(long_payload)
        start_return_s = time.monotonic() - t0
        busy_immediate = uart.write_is_busy(state)

        long_rx = read_exact(uart, LONG_WRITE_LEN, timeout_s=4.0)
        if long_rx != bytes(long_payload):
            print("FAIL: long nonblocking UART write/read mismatch")
            print("  got:", len(long_rx), "expected:", LONG_WRITE_LEN)
            return

        if uart.write_is_busy(state):
            print("WARN: UART is still busy after write completion")
            return

        print("Long write len:", LONG_WRITE_LEN, "start_return_ms:", int(start_return_s * 1000))
        if busy_immediate:
            print("PASS: long start_write returned before completion")
        else:
            print("WARN: long start_write completed before first busy poll")
        print("PASS: long start_write data")

    finally:
        uart.deinit()


main()
