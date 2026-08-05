#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Dan Halbert for Adafruit Industries
#
# SPDX-License-Identifier: MIT

"""A command-line BLE terminal for the CircuitPython BLE workflow REPL.

Linux only. It drives BlueZ over D-Bus to register a Just Works pairing agent,
which bleak does not provide and the workflow's encrypted characteristics
require, and it puts the terminal in raw mode with termios.

Peripheral output goes to stdout verbatim; this program's own status messages go
to stderr prefixed with "[ble]", so you can separate the two:

    python3 ble_terminal.py 2>/dev/null      # REPL output only
    python3 ble_terminal.py >/dev/null       # status only

stdin is put in raw mode and every byte is forwarded as typed, so Ctrl-C, Tab and
arrow keys reach the REPL.  Press Ctrl-] to quit.

The board cycles its SoftDevice whenever it switches VMs -- entering the REPL from
the "press any key" prompt, reloading, running code that imports _bleio -- and that
drops the link.  Bonds survive it, so this reconnects rather than exiting, the same
way the web editor's js/workflows/ble.js does.  Keys typed while the link is down
are held and flushed once it is back.
"""

import argparse
import asyncio
import collections
import os
import sys
import termios
import time
import tty

from bleak import BleakClient, BleakScanner
from bleak.exc import BleakDeviceNotFoundError, BleakError
from dbus_fast import BusType, DBusError, Message
from dbus_fast.aio import MessageBus
from dbus_fast.service import ServiceInterface, method

# Advertised by the peripheral (Adafruit's 16-bit 0xFEBB, expanded).
ADAFRUIT_SERVICE_UUID = "0000febb-0000-1000-8000-00805f9b34fb"
# Peripheral's RX: we write here.  Peripheral's TX: we subscribe here.
UART_RX_UUID = "adaf0002-4369-7263-7569-74507974686e"
UART_TX_UUID = "adaf0003-4369-7263-7569-74507974686e"

AGENT_PATH = "/org/adafruit/ble_terminal_agent"
QUIT_KEY = 0x1D  # Ctrl-]
SLOW_RESPONSE = 2.0  # seconds to wait before reporting silence
# The 23 byte default ATT MTU, less the 3 byte write header.  Learning the real
# negotiated MTU means calling bleak's private _acquire_mtu(), which acquires and
# immediately closes a characteristic fd -- if it picks AcquireNotify on the TX
# characteristic that toggles its CCCD off and on again before start_notify, which
# is a candidate for the peripheral dropping the link.  Not worth it for typing.
WRITE_CHUNK = 20
# The web editor's ladder (js/workflows/ble.js RECONNECT_DELAYS_MS).  The first
# delay also gives the peripheral time to start advertising again.
RECONNECT_DELAYS = (1.5, 2.5, 4.0)
# BlueZ's Connect() will chase a device that is not answering for a long time, so
# bound it rather than letting one attempt swallow the whole reconnect ladder.
CONNECT_TIMEOUT = 20.0
DISCONNECT_TIMEOUT = 5.0


# Set from the command line before the event loop starts.
_quiet = False
_quiet_writes = False


def status(message):
    """Report our own state on stderr.  \r for raw mode, which eats plain \n."""
    if _quiet:
        return
    sys.stderr.write(f"[ble] {message}\r\n")
    sys.stderr.flush()


def write_status(message):
    """Per-packet and per-line chatter, suppressible on its own.

    This is the noisiest output -- a line per packet written and a latency report
    per line submitted -- and the least useful once things are working.
    """
    if not _quiet_writes:
        status(message)


class JustWorksAgent(ServiceInterface):
    """NoInputNoOutput pairing agent.

    bleak only calls Device1.Pair(); it provides no org.bluez.Agent1.  With no
    agent on the bus, bluetoothd negative-replies the pairing confirmation and the
    peripheral aborts with SMP "Passkey entry failed".  NoInputNoOutput tells BlueZ
    not to ask, so pairing completes as Just Works -- encrypted, but with no MITM
    protection, since neither end can display or confirm a passkey.
    """

    def __init__(self):
        super().__init__("org.bluez.Agent1")

    @method()
    def Release(self):  # noqa: N802 - D-Bus method names
        pass

    @method()
    def RequestPinCode(self, device: "o") -> "s":  # noqa: N802, F821
        raise DBusError("org.bluez.Error.Rejected", "no input capability")

    @method()
    def RequestPasskey(self, device: "o") -> "u":  # noqa: N802, F821
        raise DBusError("org.bluez.Error.Rejected", "no input capability")

    @method()
    def DisplayPinCode(self, device: "o", pincode: "s"):  # noqa: N802, F821
        pass

    @method()
    def DisplayPasskey(self, device: "o", passkey: "u", entered: "q"):  # noqa: N802, F821
        pass

    @method()
    def RequestConfirmation(self, device: "o", passkey: "u"):  # noqa: N802, F821
        pass  # not reached with NoInputNoOutput; accept if it ever is

    @method()
    def RequestAuthorization(self, device: "o"):  # noqa: N802, F821
        pass

    @method()
    def Cancel(self):  # noqa: N802
        pass


async def register_agent():
    bus = await MessageBus(bus_type=BusType.SYSTEM).connect()
    bus.export(AGENT_PATH, JustWorksAgent())
    for member, body in (
        ("RegisterAgent", [AGENT_PATH, "NoInputNoOutput"]),
        ("RequestDefaultAgent", [AGENT_PATH]),
    ):
        reply = await bus.call(
            Message(
                destination="org.bluez",
                path="/org/bluez",
                interface="org.bluez.AgentManager1",
                member=member,
                signature="os" if body[1:] else "o",
                body=body,
            )
        )
        if reply.message_type.name == "ERROR":
            raise RuntimeError(f"{member} failed: {reply.error_name}: {reply.body}")
    return bus


async def find_peripheral(name_prefix, timeout):
    """Scan for the peripheral.

    Matches on name as well as service UUID: once BlueZ has the device cached it
    reports only RSSI/TxPower changes, with no UUIDs, so a UUID-only filter never
    matches an already-paired device.
    """
    status(f"scanning for {name_prefix}* ({timeout:.0f}s)")

    def match(device, advertisement):
        return ADAFRUIT_SERVICE_UUID in advertisement.service_uuids or (
            device.name or ""
        ).startswith(name_prefix)

    device = await BleakScanner.find_device_by_filter(match, timeout=timeout)
    if device is None:
        status("no peripheral found")
    else:
        status(f"found {device.name} {device.address}")
    return device


async def clear_bond(address):
    """Drop BlueZ's bond for the device.  Returns True if it had a device object.

    Only worth doing after a connect has actually failed.  CircuitPython keeps its
    bonds across a BLE stack reset and erases them only when it boots into
    discovery mode, so a stored bond is usually still good -- and it is what lets
    BlueZ resolve the private address the board advertises under once bonded.
    Dropping it unconditionally makes the board unrecognizable.

    When a bond really has gone stale, though, bleak skips pairing because BlueZ
    reports the device as already paired, the stored key is reused, the link never
    encrypts, and the connect hangs until it times out.  Hence this escape hatch.
    """
    try:
        await BleakClient(address).unpair()
    except BleakDeviceNotFoundError:
        return False
    status(f"cleared stored bond for {address}")
    return True


class KeyReader:
    """Buffers raw stdin so that typing survives a reconnect.

    The reader stays installed across connections, so keys typed while the link is
    down accumulate here and are flushed to the peripheral once it comes back.
    """

    def __init__(self, fd):
        self._fd = fd
        self._chunks = collections.deque()
        self._ready = asyncio.Event()
        self._closed = False

    def on_readable(self):
        data = os.read(self._fd, 256)
        if data:
            self._chunks.append(data)
        else:
            self._closed = True
        self._ready.set()

    async def get(self):
        """Next chunk of typed bytes; b"" once stdin has closed."""
        while not self._chunks:
            if self._closed:
                return b""
            # There is no await between the clear and the wait, and add_reader
            # callbacks only run at await points, so no chunk can slip past here.
            self._ready.clear()
            await self._ready.wait()
        return self._chunks.popleft()

    async def get_until(self, event):
        """Like get(), but returns None if event fires first."""
        getter = asyncio.ensure_future(self.get())
        waiter = asyncio.ensure_future(event.wait())
        try:
            await asyncio.wait({getter, waiter}, return_when=asyncio.FIRST_COMPLETED)
        finally:
            waiter.cancel()
        if getter.done():
            return getter.result()
        # get() pops its chunk synchronously after its only await returns, so
        # cancelling it while it is still waiting cannot drop anything.
        getter.cancel()
        return None

    def buffered(self):
        return sum(len(chunk) for chunk in self._chunks)

    def quit_typed(self):
        """True if Ctrl-] is sitting unread in the buffer."""
        return any(QUIT_KEY in chunk for chunk in self._chunks)


async def wait_before_reconnect(keys, delay):
    """Wait out a reconnect delay.  True if Ctrl-] was typed while waiting."""
    deadline = time.monotonic() + delay
    while time.monotonic() < deadline:
        if keys.quit_typed():
            return True
        await asyncio.sleep(0.1)
    return keys.quit_typed()


async def watch_for_quit(keys):
    """Resolve once Ctrl-] shows up in the buffer.  Only peeks, never consumes."""
    while not keys.quit_typed():
        await asyncio.sleep(0.1)


def describe(error):
    """BlueZ errors routinely stringify to nothing; never report an empty reason."""
    text = str(error).strip()
    return f"{type(error).__name__}: {text}" if text else repr(error)


async def watchdog(pending):
    """Say something when a submitted line has produced no reply."""
    while True:
        await asyncio.sleep(0.5)
        since = pending.get("since")
        if since is not None and not pending["warned"]:
            waited = time.monotonic() - since
            if waited >= SLOW_RESPONSE:
                status(f"no response for {waited:.1f}s ({pending['bytes']} bytes sent)")
                pending["warned"] = True


async def terminal(client, keys, disconnected):
    """Pump keystrokes until the peripheral goes away or Ctrl-] is typed.

    Returns "quit" or "disconnected".
    """
    pending = {"since": None, "warned": False, "bytes": 0}

    def on_rx(_characteristic, data):
        if pending["since"] is not None:
            write_status(f"response after {time.monotonic() - pending['since']:.2f}s")
            pending["since"] = None
        sys.stdout.write(data.decode("utf-8", "replace"))
        sys.stdout.flush()

    await client.start_notify(UART_TX_UUID, on_rx)
    held = keys.buffered()
    if held:
        status(f"flushing {held} bytes typed while disconnected")
    status("ready -- type to send, Ctrl-] to quit")

    guard = asyncio.create_task(watchdog(pending))
    try:
        while True:
            # Waiting on the disconnect event too, so a link that drops while
            # nothing is being typed is noticed immediately rather than at the
            # next keystroke.
            data = await keys.get_until(disconnected)
            if data is None:
                return "disconnected"
            if not data:
                return "quit"  # stdin closed
            quitting = QUIT_KEY in data
            if quitting:
                data = data[: data.index(QUIT_KEY)]
            if data:
                try:
                    await send(client, data, WRITE_CHUNK, pending)
                except BleakError as error:
                    status(f"send failed, link is gone: {error}")
                    return "disconnected"
            if quitting:
                return "quit"
    finally:
        guard.cancel()


async def send(client, data, chunk, pending):
    # Write without response, as the web editor does.  Nothing here needs the ack,
    # and an acknowledged write that lands while the peripheral is tearing down its
    # BLE stack surfaces as an aborted request rather than a clean failure.
    # Announce before writing, not after, so a write that kills the peripheral
    # still leaves a record of what was sent.
    for i in range(0, len(data), chunk):
        packet = data[i : i + chunk]
        write_status(f"writing {len(packet)} bytes to ADAF0002 {packet!r}")
        await client.write_gatt_char(UART_RX_UUID, packet, response=False)
    pending["bytes"] += len(data)
    if b"\r" in data or b"\n" in data:
        write_status(f"line submitted, {pending['bytes']} bytes total, waiting for response")
        pending["since"] = time.monotonic()
        pending["warned"] = False
        pending["bytes"] = 0


async def session(device, keys):
    """Connect once and run the terminal.

    Returns "quit", "disconnected", or "failed" if the link never came up.

    Takes the BLEDevice, never a bare address string.  bleak only skips its own
    BleakScanner.find_device_by_address() when it can read a D-Bus object path out
    of a BLEDevice; given a string it scans instead, and that scan runs with the
    controller's address resolution disabled.  A bonded board advertises under a
    resolvable private address, so such a scan never matches it and the connect
    fails even though BlueZ could have connected to its existing device object.
    """
    disconnected = asyncio.Event()
    # pair=True bonds during connect, before any GATT access.  bleak skips it if
    # BlueZ already holds a bond, which is the normal case after the first run:
    # the stored key is reused and no pairing happens.
    client = BleakClient(device, disconnected_callback=lambda _: disconnected.set(), pair=True)

    # Race the connect against Ctrl-] and a timeout.  A blocked Connect() must not
    # be able to hold the ladder, nor sit through a quit the user has already typed.
    connecting = asyncio.ensure_future(client.connect())
    quitting = asyncio.ensure_future(watch_for_quit(keys))
    try:
        done, _ = await asyncio.wait(
            {connecting, quitting},
            timeout=CONNECT_TIMEOUT,
            return_when=asyncio.FIRST_COMPLETED,
        )
    finally:
        quitting.cancel()

    if connecting not in done:
        connecting.cancel()
        await hang_up(client)
        if quitting in done:
            return "quit"
        status(f"connect timed out after {CONNECT_TIMEOUT:.0f}s")
        return "failed"
    try:
        connecting.result()
    except (BleakError, OSError) as error:
        status(f"connect failed: {describe(error)}")
        await hang_up(client)
        return "failed"

    try:
        status(f"connected to {device.address}")
        return await terminal(client, keys, disconnected)
    finally:
        await hang_up(client)


async def hang_up(client):
    """Best-effort disconnect.  An abandoned connect can leave one pending."""
    try:
        await asyncio.wait_for(client.disconnect(), timeout=DISCONNECT_TIMEOUT)
    except (BleakError, OSError, asyncio.TimeoutError):
        pass


async def main(name_prefix):
    await register_agent()
    status(f"pairing agent registered ({AGENT_PATH}, NoInputNoOutput)")

    device = await find_peripheral(name_prefix, timeout=15.0)
    if device is None:
        return 1

    loop = asyncio.get_running_loop()
    fd = sys.stdin.fileno()
    saved = termios.tcgetattr(fd)
    tty.setraw(fd)
    keys = KeyReader(fd)
    loop.add_reader(fd, keys.on_readable)
    status("connecting and pairing")
    try:
        rebonded = False
        attempt = 0
        while True:
            outcome = await session(device, keys)
            if outcome == "quit":
                status("quit")
                return 0
            if outcome == "disconnected":
                # Expected whenever the board switches VMs.  A session that
                # actually ran earns a fresh ladder.
                status("peripheral disconnected")
                attempt = 0
            elif not rebonded:
                # Only now is a stale host bond worth suspecting.  Dropping it is
                # destructive -- it takes the IRK that lets BlueZ recognize the
                # board, and leaves it findable only in discovery mode -- so do it
                # once, and only after a real failure.
                rebonded = True
                if await clear_bond(device.address):
                    # RemoveDevice discards BlueZ's device object, so it has to be
                    # rediscovered before a connect can use it.  This only works if
                    # the board is advertising its name, i.e. booted into discovery
                    # mode -- a bonded board advertises anonymously and cannot be
                    # found by name at all.
                    device = await find_peripheral(name_prefix, timeout=15.0)
                    if device is None:
                        status("double-tap reset for discovery mode, then run again")
                        return 1
                    continue

            if attempt >= len(RECONNECT_DELAYS):
                status("reconnect attempts exhausted; run again")
                return 1
            delay = RECONNECT_DELAYS[attempt]
            attempt += 1
            held = keys.buffered()
            extra = f", {held} bytes held" if held else ""
            status(f"retrying in {delay:.1f}s ({attempt}/{len(RECONNECT_DELAYS)}{extra})")
            if await wait_before_reconnect(keys, delay):
                status("quit")
                return 0
    finally:
        loop.remove_reader(fd)
        termios.tcsetattr(fd, termios.TCSADRAIN, saved)


def parse_args():
    parser = argparse.ArgumentParser(
        prog="ble_terminal.py",
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "name_prefix",
        nargs="?",
        default="CIRCUITPY",
        help="advertised name prefix to scan for (default: %(default)s)",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        action="store_true",
        help="suppress every [ble] status message, leaving only REPL output",
    )
    parser.add_argument(
        "-w",
        "--quiet-writes",
        action="store_true",
        help="suppress the per-packet write and per-line timing messages, "
        "keeping connection and reconnection status (does not affect writing)",
    )
    return parser.parse_args()


if __name__ == "__main__":
    _args = parse_args()
    _quiet = _args.quiet
    _quiet_writes = _args.quiet_writes
    try:
        sys.exit(asyncio.run(main(_args.name_prefix)))
    except KeyboardInterrupt:
        pass
