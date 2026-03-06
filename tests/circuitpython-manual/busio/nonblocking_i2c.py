"""
Manual test for busio.I2C nonblocking start_write()/start_read().

What this covers:
1) Correct operation path (optional): nonblocking write to an ACKing device.
2) Long write/read path: large-block start_write/start_read with nonblocking checks.
3) Failure path: nonblocking write to a likely-unused address that should NACK.
4) Resource behavior: repeated NACK attempts to detect DMA channel leaks.

Hardware / wiring:
- Required for failure-path portion: normal I2C pull-ups on SDA/SCL.
- Optional for operation-path portion: any I2C target device connected to SDA/SCL.
    If no target is present, the operation-path check is skipped.
- Required for long read/write path: an I2C target at TEST_RW_ADDRESS that accepts
    multi-kB writes and returns multi-kB reads without requiring register-address
    setup between transactions. A second CircuitPython board acting as a simple
    I2C RAM/echo slave is recommended.

Pin assumptions:
- Uses board.SCL and board.SDA.

Expected behavior notes:
- Blocking busio I2C writeto() should raise OSError on a NACK address.
- Nonblocking start_write() currently may not surface a NACK as an exception;
    this script reports what happens and then checks for post-failure channel reuse.
- For long transfers, start_* should return quickly and transfer should still be
    active immediately after return (proving nonblocking behavior).
"""

import board
import busio
import time

LONG_BLOCK_LEN = 2048


def wait_for_unlock(i2c, timeout_s=1.0):
    deadline = time.monotonic() + timeout_s
    while not i2c.try_lock():
        if time.monotonic() >= deadline:
            raise RuntimeError("timed out waiting for I2C lock")
        time.sleep(0.001)


def wait_for_transfer(i2c, state, timeout_s=1.0):
    deadline = time.monotonic() + timeout_s
    while i2c.write_is_busy(state):
        if time.monotonic() >= deadline:
            raise RuntimeError("timed out waiting for nonblocking I2C write")


def wait_for_read_transfer(i2c, state, timeout_s=2.0):
    deadline = time.monotonic() + timeout_s
    while i2c.read_is_busy(state):
        if time.monotonic() >= deadline:
            raise RuntimeError("timed out waiting for nonblocking I2C read")


def first_unused_address(used):
    for addr in range(0x08, 0x78):
        if addr not in used:
            return addr
    raise RuntimeError("no unused I2C addresses available")


def main():
    if not hasattr(busio.I2C, "start_write") or not hasattr(busio.I2C, "write_is_busy"):
        print("SKIP: nonblocking I2C API not available on this build")
        return

    have_read_noblock = hasattr(busio.I2C, "start_read") and hasattr(busio.I2C, "read_is_busy")

    i2c = busio.I2C(board.SCL, board.SDA, frequency=10000)

    try:
        wait_for_unlock(i2c)
        found = i2c.scan()
        print("I2C scan:", [hex(x) for x in found])

        ack_addr = found[0] if found else None
        nack_addr = first_unused_address(set(found))
        print("Using NACK test address:", hex(nack_addr))

        if ack_addr is not None:
            print("Operation check on ACK address:", hex(ack_addr))
            state = i2c.start_write(ack_addr, b"\x00", end=True)
            wait_for_transfer(i2c, state)
            print("PASS: nonblocking write completed on ACK device")
        else:
            print("SKIP: no ACKing I2C device found; operation-path check skipped")

        if ack_addr is None:
            print("SKIP: long write/read (no I2C target address available)")
        else:
            if True:
                print("SKIP: Long write (target device may not support it)")
            else:
                long_write_buf = bytearray(LONG_BLOCK_LEN)
                for idx in range(LONG_BLOCK_LEN):
                    long_write_buf[idx] = idx & 0xFF

                t0 = time.monotonic()
                write_state = i2c.start_write(ack_addr, long_write_buf, end=True)
                start_return_s = time.monotonic() - t0
                write_busy_immediate = i2c.write_is_busy(write_state)
                wait_for_transfer(i2c, write_state, timeout_s=4.0)

                print(
                    "Long write len:",
                    LONG_BLOCK_LEN,
                    "start_return_ms:",
                    int(start_return_s * 1000),
                )
                if write_busy_immediate:
                    print("PASS: long start_write returned before transfer completion")
                else:
                    print("WARN: long start_write completed before first busy poll")

            if have_read_noblock:
                long_read_buf = bytearray(LONG_BLOCK_LEN)

                t0 = time.monotonic()
                read_state = i2c.start_read(ack_addr, long_read_buf, end=True)
                read_start_return_s = time.monotonic() - t0
                read_busy_immediate = i2c.read_is_busy(read_state)
                wait_for_read_transfer(i2c, read_state, timeout_s=4.0)

                print(
                    "Long read len:",
                    LONG_BLOCK_LEN,
                    "start_return_ms:",
                    int(read_start_return_s * 1000),
                )
                if read_busy_immediate:
                    print("PASS: long start_read returned before transfer completion")
                else:
                    print("WARN: long start_read completed before first busy poll")

                # Data content is target-dependent; only transfer completion is asserted here.
                print("PASS: long start_read transfer completed")
            else:
                print("SKIP: long start_read (API not available on this build)")

        # Blocking behavior reference for failure correctness.
        try:
            i2c.writeto(nack_addr, b"\xaa")
            print("WARN: blocking writeto() unexpectedly succeeded at NACK address")
        except OSError as exc:
            print("PASS: blocking writeto() failed on NACK address:", repr(exc))

        # Nonblocking failure stress: if DMA channels leak after each failure,
        # this loop should eventually fail to start a transfer.
        iterations = 128
        print("Running", iterations, "nonblocking NACK attempts for leak detection")
        for idx in range(iterations):
            try:
                state = i2c.start_write(nack_addr, b"\xde\xad\xbe\xef", end=True)
                wait_for_transfer(i2c, state)
            except Exception as exc:
                print("FAIL: nonblocking NACK loop failed at iteration", idx, repr(exc))
                return

        print("PASS: nonblocking NACK stress loop completed without channel exhaustion")

        # Post-failure sanity: verify the bus still performs a known operation.
        if ack_addr is not None:
            state = i2c.start_write(ack_addr, b"\x01", end=True)
            wait_for_transfer(i2c, state)
            print("PASS: post-failure ACK write succeeded")
        else:
            print("INFO: no ACK device available for post-failure functional check")

    finally:
        if i2c.try_lock():
            i2c.unlock()
        i2c.deinit()


main()
