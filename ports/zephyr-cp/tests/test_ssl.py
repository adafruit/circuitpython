# SPDX-FileCopyrightText: 2025 Scott Shawcroft for Adafruit Industries
# SPDX-License-Identifier: MIT

"""Test ssl against a real server."""

import pytest

CODE = """\
import hostnetwork, socketpool, ssl

pool = socketpool.SocketPool(hostnetwork.HostNetwork())
print("start")
context = ssl.create_default_context()
sock = context.wrap_socket(
    pool.socket(pool.AF_INET, pool.SOCK_STREAM),
    server_hostname="www.adafruit.com",
)
sock.settimeout(30)
sock.connect(("www.adafruit.com", 443))
print("connected")
sock.send(b"GET /api/quotes.php HTTP/1.0\\r\\nHost: www.adafruit.com\\r\\n\\r\\n")
buf = bytearray(1024)
n = sock.recv_into(buf)
print(bytes(buf[:n]))
sock.close()
print("done")
"""


@pytest.mark.circuitpy_drive({"code.py": CODE})
@pytest.mark.duration(90)
def test_https_fetch(board, circuitpython):
    """Fetch a page over TLS from the device."""
    circuitpython.serial.wait_for("start")
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "Traceback" not in output
    assert "connected" in output
    assert "HTTP/1.1 200 OK" in output
    assert '"text"' in output  # a quote from /api/quotes.php
    assert "done" in output
