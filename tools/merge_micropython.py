"""
This is a helper script for merging in new versions of MicroPython. You *must*
evaluate its correctness and adapt it for each MP version. This is committed
in the repo more for reference than "fire and forget" use.

I have found I have to run each piece separately, because there are some errors.
For instance, there are file renames in the porcelain output that are not handled.
I add a sys.exit(0) after a section, and once a section runs, I delete it temporarily
and move on to the next section. -- dhalbert

Updated for v1.25.0 merge - dhalbert

"""

from io import StringIO

import sh
from sh import git
import sys


def rm_paths(base, paths):
    for path in paths:
        try:
            git.rm("-rf", base + "/" + path)
        except sh.ErrorReturnCode_128:
            pass


def checkout_ours(always_ours):
    for ours in always_ours:
        out_buf = StringIO()
        git.status("--porcelain=1", ours, _out=out_buf)
        out_buf.seek(0)
        line = out_buf.readline()
        while line:
            state, path = line.split()
            if state == "UU":
                print("ours", path)
                git.checkout("--ours", path)
                git.add(path)
            else:
                print(state, path)
            line = out_buf.readline()


rm_paths(
    "ports",
    [
        "alif",
        "bare-arm",
        "cc3200",
        "embed",
        "esp32",
        "esp8266",
        "mimxrt",
        "minimal",
        "nrf",
        "pic16bit",
        "powerpc",
        "qemu",
        "renesas-ra",
        "rp2",
        "samd",
        "stm32",
        "webassembly",
        "windows",
        "zephyr",
    ],
)

# Delete MicroPython-specific docs.
rm_paths(
    "docs",
    [
        "conf.py",
        "develop",
        "differences",
        "esp32",
        "esp8266",
        "library/bluetooth.rst",
        "library/btree.rst",
        "library/cryptolib.rst",
        "library/esp*.rst",
        "library/framebuf.rst",
        "library/hashlib.rst",
        "library/lcd160cr.rst",
        "library/machine*.rst",
        "library/math.rst",
        "library/network*.rst",
        "library/os.rst",
        "library/pyb*.rst",
        "library/random.rst",
        "library/rp2*.rst",
        "library/uos.rst",
        "library/socket.rst",
        "library/ssl.rst",
        "library/stm.rst",
        "library/struct.rst",
        "library/_thread.rst",
        "library/time.rst",
        "library/uasyncio.rst",
        "library/uctypes.rst",
        "library/vfs.rst",
        "library/wipy.rst",
        "library/wm8960.rst",
        "library/zephyr*.rst",
        "library/zlib.rst",
        "make.bat",
        "mimxrt",
        "pyboard",
        "renesas-ra",
        "rp2",
        "samd",
        "templates/topindex.html",
        "wipy",
        "zephyr",
    ],
)

# Delete MicroPython-specific tests.
rm_paths(
    "tests",
    [
        "esp32",
        "multi_bluetooth",
        "multi_espnow",
        "multi_net",
        "multi_pyb_can",
        "multi_wlan",
        "net_hosted",
        "net_inet",
        "ports",
        "pyb",
        "wipy",
    ],
)

# libs we don't use
rm_paths(
    "lib",
    [
        "alif*",
        "asf4",
        "btstack",
        "libhydrogen",
        "lwip",
        "mynewt-nimble",
        "nrfx",
        "nxp_driver",
        "pico-sdk",
        "protobuf-c",
        "stm32lib",
        "wiznet5k",
    ],
)

# extmod modules we don't use
rm_paths(
    "extmod",
    [
        "btstack",
        "extmod.cmake",
        "machine_*",
        "mbedtls",
        "modbluetooth.*",
        "modbtree.*",
        "modframebuf.*",
        "modlwip.*",
        "modmachine.*",
        "modnetwork.*",
        "modonewire.*",
        "moducryptolib.*",
        "modsocket.*",
        "modssl_*",
        "modtls_*",
        "modtimeq.*",
        "modwebsocket.*",
        "modwebrepl.*",
        "mpbthci.*",
        "network_*",
        "nimble",
        "virtpin.*",
    ],
)

# shared things we don't use
rm_paths(
    "shared",
    [
        "netutils",
        "tinyusb",
        "runtime/softtimer.*",
    ],
)

# top-level files and dirs we don't use
rm_paths(
    "",
    [
        "drivers",
        "LICENSE_MicroPython",
        "README.md",
        "CODEOFCONDUCT.md",
        "CODECONVENTIONS.md",
    ],
)

# .github and CI we don't use
rm_paths(
    ".github",
    [
        "dependabot.yml",
        "FUNDING.yml",
        "ISSUE_TEMPLATE/feature_request.yml",
        "ISSUE_TEMPLATE/documentation.yml",
        "ISSUE_TEMPLATE/security.yml",
        "workflows/biome.yml",
        "workflows/code_formatting.yml",
        "workflows/code_size_comment.yml",
        "workflows/code_size.yml",
        "workflows/codespell.yml",
        "workflows/commit_formatting.yml",
        "workflows/docs.yml",
        "workflows/examples.ymlworkflows/mpremote.yml",
        "workflows/mpy_format.yml",
        "workflows/ports_*.yml",
        "workflows/ruff.yml",
    ],
)

# Always ours:
checkout_ours(
    [
        ".github",
        "devices",
        "lib/mbedtls_config",
        "supervisor",
        "shared-bindings",
        "shared-module",
        "ports/atmel-samd",
        "ports/cxd56",
        "ports/espressif",
        "ports/mimxrt10xx",
        "ports/raspberrypi",
        "ports/silabs",
        # We inherit stm32 changes into stm because we did a git rename.
        "ports/stm",
    ]
)
