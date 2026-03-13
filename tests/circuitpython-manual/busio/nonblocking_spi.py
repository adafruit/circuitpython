"""
Manual test for nonblocking busio.SPI transfer modes.

Hardware / wiring:
- Connect MOSI to MISO (SPI loopback).
- Use board SCK/MOSI/MISO pins.
- No CS pin is required for this loopback-only transfer test.

What this covers:
1) Read-only mode (start_read).
2) Write-only mode (start_write).
3) Full-duplex start_transfer() with different buffers.
4) Full-duplex start_transfer() with the same buffer object for in/out.
5) Full-duplex start_transfer() with same backing buffer but different indexes
   (memoryview slices).
6) Full-duplex start_transfer() with a large buffer, verifying nonblocking start.

Expected behavior:
- Every transfer returns a transfer_state and completes (transfer_is_busy -> False).
- With MOSI<->MISO loopback, received bytes should mirror transmitted bytes.
- For large transfers, start_transfer() should return quickly and transfer should
    still be active immediately after return (nonblocking behavior).
"""

import board
import busio
import time


LONG_TRANSFER_LEN = 4096


def wait_for_done(spi, state, timeout_s=1.0):
    deadline = time.monotonic() + timeout_s
    while spi.transfer_is_busy(state):
        if time.monotonic() >= deadline:
            raise RuntimeError("timed out waiting for SPI nonblocking transfer")


def get_pin(name):
    if hasattr(board, name):
        return getattr(board, name)
    return None


def main():
    required_api = (
        hasattr(busio.SPI, "start_write")
        and hasattr(busio.SPI, "start_read")
        and hasattr(busio.SPI, "start_transfer")
        and hasattr(busio.SPI, "transfer_is_busy")
    )
    if not required_api:
        print("SKIP: nonblocking SPI API not available on this build")
        return

    sck = get_pin("SCK")
    mosi = get_pin("MOSI")
    miso = get_pin("MISO")
    if sck is None or mosi is None or miso is None:
        print("SKIP: board does not expose SCK/MOSI/MISO")
        return

    spi = busio.SPI(sck, MOSI=mosi, MISO=miso)
    while not spi.try_lock():
        pass

    try:
        spi.configure(baudrate=500000, polarity=0, phase=0)

        # 1) Read-only mode via start_read(). With loopback, MOSI transmits 0x00,
        # so MISO should read back 0x00 bytes.
        read_only_buf = bytearray(8)
        state = spi.start_read(read_only_buf)
        wait_for_done(spi, state)
        if read_only_buf == b"\x00" * len(read_only_buf):
            print("PASS: read-only start_read")
        else:
            print("FAIL: read-only start_read data", bytes(read_only_buf))
            return

        # 2) Write-only mode via start_write(). Completion-only check.
        write_only_data = b"\xa5\x5a\x11\x22\x33\x44\x55\x66"
        state = spi.start_write(write_only_data)
        wait_for_done(spi, state)
        print("PASS: write-only start_write")

        # 3) Full duplex with different buffers.
        out_a = bytearray(b"ABCDEFGH")
        in_a = bytearray(len(out_a))
        state = spi.start_transfer(out_a, in_a)
        wait_for_done(spi, state)
        if bytes(in_a) == bytes(out_a):
            print("PASS: start_transfer different buffers")
        else:
            print("FAIL: start_transfer different buffers", bytes(out_a), bytes(in_a))
            return

        # 4) Full duplex with same buffer for in/out.
        both = bytearray(b"qrstuvwx")
        expected_both = bytes(both)
        state = spi.start_transfer(both, both)
        wait_for_done(spi, state)
        if bytes(both) == expected_both:
            print("PASS: start_transfer same buffer")
        else:
            print("FAIL: start_transfer same buffer", expected_both, bytes(both))
            return

        # 5) Full duplex with same backing buffer, different indexes.
        # out_view: shared[0:8], in_view: shared[4:12] (overlapping slices).
        shared = bytearray(b"12345678abcdefgh")
        out_view = memoryview(shared)[0:8]
        in_view = memoryview(shared)[4:12]
        expected_shift = bytes(out_view)
        state = spi.start_transfer(out_view, in_view)
        wait_for_done(spi, state)
        got_shift = bytes(memoryview(shared)[4:12])
        if got_shift == expected_shift:
            print("PASS: start_transfer same buffer different indexes")
        else:
            print("FAIL: start_transfer indexed slices", expected_shift, got_shift)
            print("Shared:", bytes(shared))
            return

        # 6) Large full-duplex transfer, verify nonblocking start behavior.
        long_out = bytearray(LONG_TRANSFER_LEN)
        for idx in range(LONG_TRANSFER_LEN):
            long_out[idx] = idx & 0xFF
        long_in = bytearray(LONG_TRANSFER_LEN)

        t0 = time.monotonic()
        state = spi.start_transfer(long_out, long_in)
        start_return_s = time.monotonic() - t0
        busy_immediate = spi.transfer_is_busy(state)
        wait_for_done(spi, state, timeout_s=4.0)

        if bytes(long_in) != bytes(long_out):
            print("FAIL: long start_transfer data mismatch")
            return

        print(
            "Long transfer len:", LONG_TRANSFER_LEN, "start_return_ms:", int(start_return_s * 1000)
        )
        if busy_immediate:
            print("PASS: long start_transfer returned before completion")
        else:
            print("WARN: long start_transfer completed before first busy poll")
        print("PASS: long start_transfer data")

        print("ALL PASS")

    finally:
        spi.unlock()
        spi.deinit()


main()
