# The MIT License (MIT)
#
# SPDX-FileCopyrightText: Copyright (c) 2019 Michael Schroeder
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import json
import os
import pathlib
import re
import subprocess
import tomllib

from concurrent.futures import ThreadPoolExecutor

SUPPORTED_PORTS = [
    "analog",
    "atmel-samd",
    "cxd56",
    "espressif",
    "mimxrt10xx",
    "nordic",
    "raspberrypi",
    "silabs",
    "stm",
    "zephyr-cp",
]

ALIASES_BY_BOARD = {
    "circuitplayground_express": [
        "circuitplayground_express_4h",
        "circuitplayground_express_digikey_pycon2019",
    ],
    "pybadge": ["edgebadge"],
    "pyportal": ["pyportal_pynt"],
    "gemma_m0": ["gemma_m0_pycon2018"],
}

ALIASES_BRAND_NAMES = {
    "circuitplayground_express_4h": "Adafruit Circuit Playground Express 4-H",
    "circuitplayground_express_digikey_pycon2019": "Circuit Playground Express Digi-Key PyCon 2019",
    "edgebadge": "Adafruit EdgeBadge",
    "pyportal_pynt": "Adafruit PyPortal Pynt",
    "gemma_m0_pycon2018": "Adafruit Gemma M0 PyCon 2018",
}

ADDITIONAL_MODULES = {
    "_asyncio": "MICROPY_PY_ASYNCIO",
    "_bleio (native)": "CIRCUITPY_BLEIO_NATIVE",
    "_bleio (HCI co-processor)": "CIRCUITPY_BLEIO_HCI",
    "_eve": "CIRCUITPY__EVE",
    "adafruit_bus_device": "CIRCUITPY_BUSDEVICE",
    "adafruit_pixelbuf": "CIRCUITPY_PIXELBUF",
    "array": "CIRCUITPY_ARRAY",
    # always available, so depend on something that's always 1.
    "builtins": "CIRCUITPY",
    "builtins.pow3": "CIRCUITPY_BUILTINS_POW3",
    "busio.SPI": "CIRCUITPY_BUSIO_SPI",
    "busio.UART": "CIRCUITPY_BUSIO_UART",
    "collections": "CIRCUITPY_COLLECTIONS",
    "fontio": "CIRCUITPY_DISPLAYIO",
    "io": "CIRCUITPY_IO",
    "keypad.KeyMatrix": "CIRCUITPY_KEYPAD_KEYMATRIX",
    "keypad.Keys": "CIRCUITPY_KEYPAD_KEYS",
    "keypad.ShiftRegisterKeys": "CIRCUITPY_KEYPAD_SHIFTREGISTERKEYS",
    "keypad_demux.DemuxKeyMatrix": "CIRCUITPY_KEYPAD_DEMUX",
    "os.getenv": "CIRCUITPY_SETTINGS_TOML",
    "select": "MICROPY_PY_SELECT_SELECT",
    "supervisor.get_setting": "CIRCUITPY_SETTINGS_TOML",
    "sys": "CIRCUITPY_SYS",
    "terminalio": "CIRCUITPY_DISPLAYIO",
    "usb": "CIRCUITPY_PYUSB",
    "socketpool.socketpool.AF_INET6": "CIRCUITPY_SOCKETPOOL_IPV6",
}

MODULES_NOT_IN_BINDINGS = ["binascii", "errno", "json", "re", "ulab"]

FROZEN_EXCLUDES = ["examples", "docs", "tests", "utils", "conf.py", "setup.py"]
"""Files and dirs at the root of a frozen directory that should be ignored.
This is the same list as in the preprocess_frozen_modules script."""

repository_urls = {}
"""Cache of repository URLs for frozen modules."""

root_dir = pathlib.Path(__file__).resolve().parent.parent


def get_circuitpython_root_dir():
    """The path to the root './circuitpython' directory."""
    return root_dir


def get_bindings():
    """Get a list of modules in shared-bindings and ports/*/bindings
    based on folder names."""
    shared_bindings_modules = [
        module.name
        for module in (get_circuitpython_root_dir() / "shared-bindings").iterdir()
        if module.is_dir()
    ]
    bindings_modules = []
    for d in get_circuitpython_root_dir().glob("ports/*/bindings"):
        bindings_modules.extend(module.name for module in d.iterdir() if d.is_dir())
    return (
        shared_bindings_modules
        + bindings_modules
        + MODULES_NOT_IN_BINDINGS
        + list(ADDITIONAL_MODULES.keys())
    )


def get_board_mapping():
    """
    Compiles the list of boards from the directories, with aliases and mapping
    to the port.
    """
    boards = {}
    for port in SUPPORTED_PORTS:
        board_path = root_dir / "ports" / port / "boards"
        # Zephyr port has vendor specific subdirectories to match zephyr (and
        # clean up the boards folder.)
        g = "*/*" if port == "zephyr-cp" else "*"
        for board_path in board_path.glob(g):
            if board_path.is_dir():
                board_id = board_path.name
                if port == "zephyr-cp":
                    vendor = board_path.parent.name
                    board_id = f"{vendor}_{board_id}"
                aliases = ALIASES_BY_BOARD.get(board_path.name, [])
                boards[board_id] = {
                    "port": port,
                    "download_count": 0,
                    "aliases": aliases,
                    "directory": board_path,
                }
                for alias in aliases:
                    boards[alias] = {
                        "port": port,
                        "download_count": 0,
                        "alias": True,
                        "aliases": [],
                        "directory": board_path,
                    }
    return boards


def build_module_map():
    """Establish the base of the JSON file, based on the contents from
    `configs`. Base contains the module name and the controlling C macro name.
    """
    base = dict()
    modules = get_bindings()
    for module in modules:
        full_name = module
        if module in ADDITIONAL_MODULES:
            search_identifier = ADDITIONAL_MODULES[module]
        else:
            search_identifier = "CIRCUITPY_" + module.lstrip("_").upper()

        base[module] = {
            "name": full_name,
            "key": search_identifier,
        }

    return base


def get_settings_from_makefile(port_dir, board_name):
    """Invoke make to print the value of critical build settings

    This means that the effect of all Makefile directives is taken
    into account, without having to re-encode the logic that sets them
    in this script, something that has proved error-prone

    This list must explicitly include any setting queried by tools/ci_set_matrix.py.
    """
    if os.getenv("NO_BINDINGS_MATRIX"):
        return {"CIRCUITPY_BUILD_EXTENSIONS": ".bin"}

    contents = subprocess.run(
        [
            "make",
            # Don't let make run in parallel; it can mix up the output from the various "print-" targets.
            "-j1",
            "-C",
            port_dir,
            "-f",
            "Makefile",
            f"BOARD={board_name}",
            "print-CFLAGS",
            "print-CIRCUITPY_BUILD_EXTENSIONS",
            "print-FROZEN_MPY_DIRS",
            "print-SRC_PATTERNS",
            "print-SRC_SUPERVISOR",
        ],
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    # Make signals errors with exit status 2; 0 and 1 are "non-error" statuses
    if contents.returncode not in (0, 1):
        error_msg = (
            f"Invoking '{' '.join(contents.args)}' exited with "
            f"{contents.returncode}: {contents.stderr}"
        )
        raise RuntimeError(error_msg)

    settings = {}
    for line in contents.stdout.split("\n"):
        if line.startswith("CFLAGS ="):
            for m in re.findall(r"-D([A-Z][A-Z0-9_]*)=(\d+)", line):
                settings[m[0]] = m[1]
        elif m := re.match(r"^([A-Z][A-Z0-9_]*) = (.*)$", line):
            settings[m.group(1)] = m.group(2)

    return settings


def get_repository_url(directory):
    if directory in repository_urls:
        return repository_urls[directory]
    readme = None
    for readme_path in (
        os.path.join(directory, "README.rst"),
        os.path.join(os.path.dirname(directory), "README.rst"),
    ):
        if os.path.exists(readme_path):
            readme = readme_path
            break
    path = None
    if readme:
        with open(readme, "r") as fp:
            for line in fp.readlines():
                if m := re.match(
                    r"\s+:target:\s+(http\S+(docs.circuitpython|readthedocs)\S+)\s*",
                    line,
                ):
                    path = m.group(1)
                    break
                if m := re.search(r"<(http[^>]+)>", line):
                    path = m.group(1)
                    break
    if path is None:
        contents = subprocess.run(
            ["git", "remote", "get-url", "origin"],
            encoding="utf-8",
            errors="replace",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            cwd=directory,
        )
        path = contents.stdout.strip()
    repository_urls[directory] = path
    return path


def remove_prefix(s, prefix):
    if not s.startswith(prefix):
        raise ValueError(f"{s=} does not start with {prefix=}")
    return s.removeprefix(prefix)


def frozen_modules_from_dirs(frozen_mpy_dirs, withurl):
    """
    Go through the list of frozen directories and extract the python modules.
    Paths are of the type:
        $(TOP)/frozen/Adafruit_CircuitPython_CircuitPlayground
        $(TOP)/frozen/circuitpython-stage/meowbit
    Python modules are at the root of the path, and are python files or directories
    containing python files. Except the ones in the FROZEN_EXCLUDES list.
    """
    frozen_modules = []
    for frozen_path in filter(lambda x: x, frozen_mpy_dirs.split(" ")):
        frozen_path = remove_prefix(frozen_path, "../../")
        source_dir = get_circuitpython_root_dir() / frozen_path
        url_repository = get_repository_url(source_dir)
        for sub in source_dir.glob("*"):
            if sub.name in FROZEN_EXCLUDES:
                continue
            if sub.name.endswith(".py"):
                if withurl:
                    frozen_modules.append((sub.name[:-3], url_repository))
                else:
                    frozen_modules.append(sub.name[:-3])
                continue
            if next(sub.glob("**/*.py"), None):  # tests if not empty
                if withurl:
                    frozen_modules.append((sub.name, url_repository))
                else:
                    frozen_modules.append(sub.name)
    return frozen_modules


def lookup_setting(settings, key, default=""):
    while True:
        value = settings.get(key, default)
        if not value.startswith("$"):
            break
        key = value[2:-1]
    return value


def get_branded_name(board_id, board_directory, autogen_board_info):
    """Return the board's product name, falling back to its board id."""
    if autogen_board_info:
        # Many zephyr-cp boards share a name with a native board, so tell them apart.
        return autogen_board_info["name"] + " (Zephyr)"
    with open(board_directory / "mpconfigboard.h") as f:
        board_name_re = re.search(r"(?<=MICROPY_HW_BOARD_NAME)\s+(.+)", f.read())
    if not board_name_re:
        return board_id
    branded_name = board_name_re.group(1).strip('"')
    if '"' in branded_name:  # sometimes the closing " is not at line end
        branded_name = branded_name[: branded_name.index('"')]
    return branded_name


def get_mcu_and_flash(board_directory, autogen_board_info):
    """Return the board's MCU name and its external flash chips as a quoted, comma-separated string.

    zephyr-cp boards don't record either yet, so they get empty strings.
    """
    if autogen_board_info:
        return "", ""
    with open(board_directory / "mpconfigboard.h") as f:
        mcu_re = re.search(r"(?<=MICROPY_HW_MCU_NAME)\s+(.+)", f.read())
    mcu = ""
    if mcu_re:
        mcu = mcu_re.group(1).strip('"')
        if '"' in mcu:  # in case the closing " is not at line end
            mcu = mcu[: mcu.index('"')]
    with open(board_directory / "mpconfigboard.mk") as f:
        flash_re = re.search(r"(?<=EXTERNAL_FLASH_DEVICES)\s+=\s+(.+)", f.read())
    flash = ""
    if flash_re:
        # Multiple flash chips are written in several ways. Normalize them to
        # a quoted, comma-separated string.
        flash = '"' + flash_re.group(1).replace('"', "") + '"'
    return mcu, flash


def get_pins(board_directory):
    """Return the board's [board_pin, chip_pin] pairs from pins.c."""
    try:
        with open(board_directory / "pins.c") as f:
            pin_lines = f.readlines()
    except FileNotFoundError:  # silabs and zephyr-cp boards have no pins.c
        return []
    pins = []
    for line in pin_lines:
        if pin_re := re.search(r"QSTR_([^\)]+).+pin_([^\)]+)", line):
            pins.append([pin_re.group(1), pin_re.group(2)])
    return pins


def get_board_modules(settings, autogen_board_info, base):
    """Return the sorted names of the modules enabled on the board."""
    if autogen_board_info:
        modules = [k for k, enabled in autogen_board_info["modules"].items() if enabled]
    else:
        modules = [
            info["name"]
            for info in base.values()
            if int(lookup_setting(settings, info["key"], "0"))
        ]
    return sorted(modules)


def get_board_extensions(board_id, settings):
    """Return the file extensions the board's build produces, such as ["uf2"]."""
    if "CIRCUITPY_BUILD_EXTENSIONS" not in settings:
        raise OSError(f"Board extensions undefined: {board_id}.")
    extensions = settings["CIRCUITPY_BUILD_EXTENSIONS"]
    if isinstance(extensions, str):
        extensions = [extension.strip() for extension in extensions.split(",")]
    return extensions


def collect_board_data():
    """Gather what support_matrix_by_board() needs about every board, keyed by board id.

    Aliases are left out; support_matrix_by_board() adds them. The result holds
    only JSON types, so it can be saved and reused.

    This runs make once for each board outside zephyr-cp, which takes a while.
    """
    base = build_module_map()

    def collect(arg):
        board_id, board_info = arg
        port = board_info["port"]
        board_directory = board_info["directory"]
        if port == "zephyr-cp":
            with (board_directory / "circuitpython.toml").open("rb") as f:
                settings = tomllib.load(f)
            with (board_directory / "autogen_board_info.toml").open("rb") as f:
                autogen_board_info = tomllib.load(f)
        else:
            port_dir = board_directory.parent.parent
            settings = get_settings_from_makefile(str(port_dir), board_directory.name)
            autogen_board_info = None

        frozen_libraries = []
        if "FROZEN_MPY_DIRS" in settings:
            frozen_libraries = [
                list(library)
                for library in sorted(
                    frozen_modules_from_dirs(settings["FROZEN_MPY_DIRS"], withurl=True)
                )
            ]

        mcu, flash = get_mcu_and_flash(board_directory, autogen_board_info)
        return board_id, {
            "port": port,
            "branded_name": get_branded_name(board_id, board_directory, autogen_board_info),
            "mcu": mcu,
            "flash": flash,
            "pins": get_pins(board_directory),
            "modules": get_board_modules(settings, autogen_board_info, base),
            "extensions": get_board_extensions(board_id, settings),
            # [name, repository url] pairs
            "frozen_libraries": frozen_libraries,
        }

    real_boards = [
        (board_id, board_info)
        for board_id, board_info in get_board_mapping().items()
        if not board_info.get("alias", False)
    ]
    with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
        return dict(executor.map(collect, real_boards))


def write_board_data(path, board_data):
    """Save the output of collect_board_data() as JSON."""
    # Keep the board order: when boards share a product name, the last one wins.
    with open(path, "w") as f:
        json.dump(board_data, f, indent=1)


def read_board_data(path):
    """Return board data saved by write_board_data(), or None if path doesn't exist."""
    try:
        with open(path) as f:
            return json.load(f)
    except FileNotFoundError:
        return None


def support_matrix_by_board(
    use_branded_name=True,
    withurl=True,
    add_port=False,
    add_chips=False,
    add_pins=False,
    add_branded_name=False,
    board_data=None,
):
    """Return each board's modules, frozen libraries and build extensions.

    Aliases get their own entries. Keys are board ids, or product names if
    use_branded_name is set. The add_* flags add more fields.

    board_data is the output of collect_board_data(). If it is None, it is collected here.
    """
    if board_data is None:
        board_data = collect_board_data()

    def board_entry(data):
        entry = {
            "modules": data["modules"],
            "frozen_libraries": [
                tuple(library) if withurl else library[0] for library in data["frozen_libraries"]
            ],
            "extensions": data["extensions"],
        }
        if add_branded_name:
            entry["branded_name"] = data["branded_name"]
        if add_port:
            entry["port"] = data["port"]
        if add_chips:
            entry["mcu"] = data["mcu"]
            entry["flash"] = data["flash"]
        if add_pins:
            entry["pins"] = [tuple(pin) for pin in data["pins"]]
        return entry

    boards = []
    for board_id, data in board_data.items():
        boards.append((data["branded_name"] if use_branded_name else board_id, board_entry(data)))
        for alias in ALIASES_BY_BOARD.get(board_id, []):
            if use_branded_name:
                alias = ALIASES_BRAND_NAMES.get(alias, alias.replace("_", " ").title())
            boards.append((alias, board_entry(data)))

    # Several boards can share a product name; the last one listed wins.
    return dict(sorted(boards, key=lambda board: board[0]))


if __name__ == "__main__":
    print(json.dumps(support_matrix_by_board(), indent=2))
