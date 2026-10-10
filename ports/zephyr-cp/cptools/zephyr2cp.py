import logging
import pathlib
import re
import tomllib

import cpbuild
import yaml
from compat2driver import COMPAT_TO_DRIVER
from devicetree import dtlib

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# GPIO flags defined here: include/zephyr/dt-bindings/gpio/gpio.h
GPIO_ACTIVE_LOW = 1 << 0

# A region has to be big enough to host TLSF's control structure to be usable as
# the first heap pool, and TLSF sizes that structure from the maximum heap size
# rather than from the region: at an 8 MB maximum it is 2412 bytes. Anything
# smaller than this is not worth adding as a later pool either. The previous
# value of 1024 let through two SiWx917 regions that are exactly 0x400 bytes,
# /memory@0 (reserved for the network processor) and /memory-dma@24061c00,
# neither of which should ever be in the Python heap.
MINIMUM_RAM_SIZE = 8192

MANUAL_COMPAT_TO_DRIVER = {
    "renesas_ra_nv_flash": "flash",
    "soc_nv_flash": "flash",
    "nordic_nrf_uarte": "serial",
    "nordic_nrf_uart": "serial",
    "nordic_nrf_twim": "i2c",
    "nordic_nrf_twi": "i2c",
    "nordic_nrf_spim": "spi",
    "nordic_nrf_spi": "spi",
    "nordic_nrf_i2s": "i2s",
}

# These are controllers, not the flash devices themselves.
BLOCKED_FLASH_COMPAT = (
    "renesas,ra-qspi",
    "renesas,ra-ospi-b",
    "nordic,nrf-spim",
)

BUSIO_CLASSES = {"serial": "UART", "i2c": "I2C", "spi": "SPI"}

# Compatibles analogio is implemented against: the SoC ADC (whose pads
# iobroker resolves to their fixed analog input), the SoC DAC (whose output
# pad is fixed per SoC) and the emulated devices.
ANALOG_COMPAT = {
    "nordic_nrf_saadc": "adc",
    "zephyr_adc_emul": "adc",
    "vnd_dac": "dac",
    "renesas_ra_dac": "dac",
    "nxp_lpc_lpadac": "adc",
    "nxp_lpdac": "dac",
}

AUDIOBUSIO_CLASSES = {"i2s": "I2SOut"}

CONNECTORS = {
    "mikro-bus": [
        "AN",
        "RST",
        "CS",
        "SCK",
        "MISO",
        "MOSI",
        "PWM",
        "INT",
        "RX",
        "TX",
        "SCL",
        "SDA",
    ],
    "arduino-header-r3": [
        "A0",
        "A1",
        "A2",
        "A3",
        "A4",
        "A5",
        "D0",
        "D1",
        "D2",
        "D3",
        "D4",
        "D5",
        "D6",
        "D7",
        "D8",
        "D9",
        "D10",
        "D11",
        "D12",
        "D13",
        "D14",
        "D15",
    ],
    "adafruit-feather-header": [
        "A0",
        "A1",
        "A2",
        "A3",
        "A4",
        "A5",
        "SCK",
        "MOSI",
        "MISO",
        "RX",
        "TX",
        "D4",
        "SDA",
        "SCL",
        "D5",
        "D6",
        "D9",
        "D10",
        "D11",
        "D12",
        "D13",
    ],
    "adafruit-clue": [
        ["P0", "D0", "A2", "RX"],
        ["P1", "D1", "A3", "TX"],
        ["P2", "D2", "A4"],
        ["P3", "D3", "A5"],
        ["P4", "D4", "A6"],
        ["P5", "D5", "BUTTON_A"],
        ["P6", "D6"],
        ["P7", "D7"],
        ["P8", "D8"],
        ["P9", "D9"],
        ["P10", "D10", "A7"],
        ["P11", "D11", "BUTTON_B"],
        ["P12", "D12", "A0"],
        ["P13", "D13", "SCK"],
        ["P14", "D14", "MISO"],
        ["P15", "D15", "MOSI"],
        ["P16", "D16", "A1"],
        ["P17", "D17", "L", "LED"],
        ["P18", "D18", "NEOPIXEL"],
        ["P19", "D19", "SCL"],
        ["P20", "D20", "SDA"],
    ],
    "nordic,expansion-board-header": [
        "P1_04",
        "P1_05",
        "P1_06",
        "P1_07",
        "P1_08",
        "P1_09",
        "P1_10",
        "P1_11",
        "P1_12",
        "P1_13",
        "P1_14",
    ],
    "arducam,dvp-20pin-connector": [
        "SCL",
        "SDA",
        "VS",
        "HS",
        "PCLK",
        "XCLK",
        "D7",
        "D6",
        "D5",
        "D4",
        "D3",
        "D2",
        "D1",
        "D0",
        "PEN",
        "PDN",
        "GPIO0",
        "GPIO1",
    ],
    "nxp,cam-44pins-connector": ["CAM_RESETB", "CAM_PWDN"],
    "nxp,lcd-8080": [
        "TOUCH_SCL",
        "TOUCH_SDA",
        "TOUCH_INT",
        "BACKLIGHT",
        "RESET",
        "LCD_DC",
        "LCD_CS",
        "LCD_WR",
        "LCD_RD",
        "LCD_TE",
        "LCD_D0",
        "LCD_D1",
        "LCD_D2",
        "LCD_D3",
        "LCD_D4",
        "LCD_D5",
        "LCD_D6",
        "LCD_D7",
        "LCD_D8",
        "LCD_D9",
        "LCD_D10",
        "LCD_D11",
        "LCD_D12",
        "LCD_D13",
        "LCD_D14",
        "LCD_D15",
    ],
    "nxp,lcd-pmod": [
        "LCD_WR",
        "TOUCH_SCL",
        "LCD_DC",
        "TOUCH_SDA",
        "LCD_MOSI",
        "TOUCH_RESET",
        "LCD_CS",
        "TOUCH_INT",
    ],
    "raspberrypi,csi-connector": [
        "CSI_D0_N",
        "CSI_D0_P",
        "CSI_D1_N",
        "CSI_D1_P",
        "CSI_CK_N",
        "CSI_CK_P",
        "CSI_D2_N",
        "CSI_D2_P",
        "CSI_D3_N",
        "CSI_D3_P",
        "IO0",
        "IO1",
        "I2C_SCL",
        "I2C_SDA",
    ],
    "renesas,ra-gpio-mipi-header": [
        "IIC_SDA",
        "DISP_BLEN",
        "IIC_SCL",
        "DISP_INT",
        "DISP_RST",
    ],
    "renesas,ra-parallel-graphics-header": [
        "DISP_BLEN",
        "IIC_SDA",
        "DISP_INT",
        "IIC_SCL",
        "DISP_RST",
        "LCDC_TCON0",
        "LCDC_CLK",
        "LCDC_TCON2",
        "LCDC_TCON1",
        "LCDC_EXTCLK",
        "LCDC_TCON3",
        "LCDC_DATA01",
        "LCDC_DATA00",
        "LCDC_DATA03",
        "LCDC_DATA02",
        "LCDC_DATA05",
        "LCDC_DATA04",
        "LCDC_DATA07",
        "LCDC_DATA16",
        "LCDC_DATA09",
        "LCDC_DATA08",
        "LCDC_DATA11",
        "LCDC_DATA10",
        "LCDC_DATA13",
        "LCDC_DATA12",
        "LCDC_DATA15",
        "LCDC_DATA14",
        "LCDC_DATA17",
        "LCDC_DATA16",
        "LCDC_DATA19",
        "LCDC_DATA18",
        "LCDC_DATA21",
        "LCDC_DATA20",
        "LCDC_DATA23",
        "LCDC_DATA22",
    ],
    "st,stm32-dcmi-camera-fpu-330zh": [
        "SCL",
        "SDA",
        "RESET",
        "PEN",
        "VS",
        "HS",
        "PCLK",
        "D7",
        "D6",
        "D5",
        "D4",
        "D3",
        "D2",
        "D1",
        "D0",
    ],
    "raspberrypi,pico-header": [
        "GP0",
        "GP1",
        "GP2",
        "GP3",
        "GP4",
        "GP5",
        "GP6",
        "GP7",
        "GP8",
        "GP9",
        "GP10",
        "GP11",
        "GP12",
        "GP13",
        "GP14",
        "GP15",
        "GP16",
        "GP17",
        "GP18",
        "GP19",
        "GP20",
        "GP21",
        "GP22",
        ["GP26_A0", "GP26", "A0"],
        ["GP27_A1", "GP27", "A1"],
        ["GP28_A2", "GP28", "A2"],
    ],
}

EXCEPTIONAL_DRIVERS = ["entropy", "gpio", "led"]


def find_flash_devices(device_tree):
    """
    Find all flash devices from a device tree.

    Args:
        device_tree: Parsed device tree (dtlib.DT object)

    Returns:
        List of device tree flash device reference strings
    """
    # Build path2chosen mapping
    path2chosen = {}
    for k in device_tree.root.nodes["chosen"].props:
        value = device_tree.root.nodes["chosen"].props[k]
        path2chosen[value.to_path()] = k

    # Paths of zephyr,sim-flash controllers (the flash simulator used by
    # native_sim). The simulator driver defines its device on the controller
    # node, not on its soc-nv-flash child, so the controller must be used and
    # the child skipped.
    sim_flash_controller_paths = set()

    flashes = []
    logger.debug("Flash devices:")

    # Traverse all nodes in the device tree
    # A list, not a set, so the nodes are visited in the same order every build.
    remaining_nodes = [device_tree.root]
    while remaining_nodes:
        node = remaining_nodes.pop()
        remaining_nodes.extend(node.nodes.values())

        # Get compatible strings
        compatible = []
        if "compatible" in node.props:
            compatible = node.props["compatible"].to_strings()

        # Get status
        status = node.props.get("status", None)
        if status is None:
            status = "okay"
        else:
            status = status.to_string()

        # Check if this is a flash device
        if not compatible or status != "okay":
            continue

        # Check for flash driver via compat2driver
        drivers = []
        for c in compatible:
            underscored = c.replace(",", "_").replace("-", "_")
            driver = COMPAT_TO_DRIVER.get(underscored, None)
            if not driver:
                driver = MANUAL_COMPAT_TO_DRIVER.get(underscored, None)
            if driver:
                drivers.append(driver)
        logger.debug(f"  {node.labels[0] if node.labels else node.name} drivers: {drivers}")

        if "flash" not in drivers:
            continue

        if compatible[0] == "zephyr,sim-flash":
            # Always use the controller for the flash simulator, even when it
            # is chosen as zephyr,flash-controller.
            sim_flash_controller_paths.add(node.path)
            if node.labels:
                flashes.append(node.labels[0])
            continue

        # The soc-nv-flash child of a flash simulator has no device defined on
        # it; the controller (handled above) is the flash device.
        if node.parent is not None and node.parent.path in sim_flash_controller_paths:
            logger.debug(
                f"  skipping flash {node.labels[0] if node.labels else node.name} (sim-flash child)"
            )
            continue

        # Skip chosen nodes because they are used by Zephyr
        if node in path2chosen:
            logger.debug(
                f"  skipping flash {node.labels[0] if node.labels else node.name} (chosen)"
            )
            continue

        # Skip blocked flash compatibles (controllers, not actual flash devices)
        if compatible[0] in BLOCKED_FLASH_COMPAT:
            logger.debug(
                f"  skipping flash {node.labels[0] if node.labels else node.name} (blocked compat)"
            )
            continue

        if node.labels:
            flashes.append(node.labels[0])

    logger.debug("Flash devices:")
    for flash in flashes:
        logger.debug(f"  {flash}")

    return flashes


def _label_to_end(label):
    return f"(uint32_t*) (DT_REG_ADDR(DT_NODELABEL({label})) + DT_REG_SIZE(DT_NODELABEL({label})))"


def find_ram_regions(device_tree):
    """
    Find all RAM regions from a device tree. Includes the zephyr,sram node and
    any zephyr,memory-region nodes.

    Returns:
        List of RAM region info tuples: (label, start, end, size, path)
    """
    rams = []
    chosen = None
    # Get the chosen SRAM node directly
    if "zephyr,sram" in device_tree.root.nodes["chosen"].props:
        chosen = device_tree.root.nodes["chosen"].props["zephyr,sram"].to_path()
        label = chosen.labels[0]
        size = chosen.props["reg"].to_nums()[1]
        logger.debug(f"Found chosen SRAM node: {label} with size {size}")
        rams.append((label, "z_mapped_end", _label_to_end(label), size, chosen.path))

    # Traverse all nodes in the device tree to find memory-region nodes
    # A list, not a set, so the nodes are visited in the same order every build.
    remaining_nodes = [device_tree.root]
    while remaining_nodes:
        node = remaining_nodes.pop()

        # Check status first so we don't add child nodes that aren't active.
        status = node.props.get("status", None)
        if status is None:
            status = "okay"
        else:
            status = status.to_string()

        if status != "okay":
            continue

        if node == chosen:
            continue

        remaining_nodes.extend(node.nodes.values())

        if "compatible" not in node.props or not node.labels:
            continue

        compatible = node.props["compatible"].to_strings()

        if "zephyr,memory-region" not in compatible or "zephyr,memory-region" not in node.props:
            continue

        is_mmio_sram = "mmio-sram" in compatible
        device_type = node.props.get("device_type")
        has_memory_device_type = device_type and device_type.to_string() == "memory"
        if not (is_mmio_sram or has_memory_device_type):
            continue

        size = node.props["reg"].to_nums()[1]

        start = "__" + node.props["zephyr,memory-region"].to_string() + "_end"
        end = _label_to_end(node.labels[0])

        # Filter by minimum size
        if size >= MINIMUM_RAM_SIZE:
            logger.debug(
                f"Adding extra RAM info: ({node.labels[0]}, {start}, {end}, {size}, {node.path})"
            )
            info = (node.labels[0], start, end, size, node.path)
            rams.append(info)

    return rams


# gpio-keys nodes identify a key with `zephyr,code` rather than the optional and
# deprecated `label`, so the code is the only name a modern board gives.
INPUT_KEY_NAMES = {}


# Mask selecting the nRF pin number field (absolute pin, port*32+pin) of
# a pinctrl psel entry. The pin control entry uses all-ones in this field to
# mark a disconnected signal (NRF_PIN_DISCONNECTED).
NRF_PIN_FIELD_MASK = 0x1FF


def _reg_addr_expr(device_tree, label):
    """C expression for a node's register block address, or 0 when it has none.

    The iobroker tables carry the address for SoC routing code that decides
    reachability from it (nRF54L power domains). Some nodes the tables list
    have no reg property (native_sim's gpio_emul, PIO-based buses), and
    DT_REG_ADDR() does not compile for them.
    """
    node = device_tree.label2node.get(label)
    if node is None or "reg" not in node.props:
        return "0"
    return f"DT_REG_ADDR(DT_NODELABEL({label}))"


def _pinctrl_default_psels(node):
    """Return the raw nRF psel entries of a node's "default" pinctrl state.

    The state node (referenced by pinctrl-0) groups its configuration in
    child nodes (typically named group1, group2, ...) that each carry a
    psels property.

    Returns None when the node does not use pinctrl.
    """
    prop = node.props.get("pinctrl-0")
    if prop is None:
        return None
    psels = []
    try:
        for state in prop.to_nodes():
            for group in state.nodes.values():
                if "psels" not in group.props:
                    continue
                for value in group.props["psels"].to_nums():
                    psels.append(value)
    except (dtlib.DTError, KeyError):
        return None
    return psels


def _populate_input_key_names():
    header = (
        pathlib.Path(__file__).parent.parent
        / "zephyr"
        / "include"
        / "zephyr"
        / "dt-bindings"
        / "input"
        / "input-event-codes.h"
    )
    if not header.exists():
        return
    pattern = re.compile(r"^#define\s+INPUT_(?P<name>KEY_\w+)\s+(?P<code>\d+)")
    for line in header.read_text().splitlines():
        match = pattern.match(line)
        if match:
            INPUT_KEY_NAMES.setdefault(int(match.group("code")), match.group("name"))


_populate_input_key_names()


def add_toml_pin_names(
    board_names,
    mpconfigboard,
    package_pins,
    package_choice,
    port_indexes,
    ioports,
    pad_name_of_pad,
    gpio_pad_of_pad,
):
    """Add board pin names from circuitpython.toml's ``[pins]`` table.

    Each entry maps a board module name to a package pin number or, for ball
    grid array packages, the datasheet's ball id (e.g. ``"A1"``). Entries are
    resolved to SoC pads with the iobroker package pin map selected by the
    ``IOBROKER_PACKAGE`` choice, and appended to ``board_names`` like
    devicetree-derived names are.

    A name that already maps to the same pin (from the devicetree walk or an
    earlier entry) is deduplicated; a name that would map to a different pin
    than an existing entry is a build error.

    Pads whose package map entry carries no GPIO bond (the map's
    analog-only pads) cannot key ``board_names``, whose entries describe
    GPIO controller pins; the names collected for them are keyed by SoC pad
    and returned as a ``pad -> board module names`` dict instead, with the
    pin objects created from the map's datasheet pad names (e.g.
    "ANA_0") by the caller.
    """
    toml_pins = (mpconfigboard or {}).get("pins")
    if not toml_pins:
        return {}
    if package_pins is None:
        reason = "NONE" if package_choice == "none" else "unset"
        raise RuntimeError(
            f"circuitpython.toml [pins] needs an iobroker package pin map but "
            f"CONFIG_IOBROKER_PACKAGE is {reason}"
        )
    port_label_of_index = {index: label for label, index in port_indexes.items()}

    # Board module names for the package map's analog-only pads, keyed by
    # their SoC pad (a pad no enabled GPIO controller covers).
    analog_board_names = {}

    def sanitize(name):
        return name.upper().replace(" ", "_").replace("-", "_").replace("(", "").replace(")", "")

    # Package pin -> pad (the SoC pad number; SoC packages default it to
    # the pin), ball id -> pad, for resolving [pins] values.
    package_pin_to_pad = {}
    balls = {}
    for package_pin_entry in package_pins:
        pad = package_pin_entry.get("pad", None)
        if pad is None and "pad_name" not in package_pin_entry:
            # Informational module pin (e.g. bonded to GND): no pad.
            continue
        pin = package_pin_entry["pin"]
        if pad is None:
            pad = pin
        if pin in package_pin_to_pad and package_pin_to_pad[pin] != pad:
            raise RuntimeError(f"package pin {pin} maps to multiple pads in the package pin map")
        package_pin_to_pad[pin] = pad
        if "ball" in package_pin_entry:
            ball = package_pin_entry["ball"]
            if ball in balls and balls[ball] != pad:
                raise RuntimeError(f"ball {ball} maps to multiple pads in the package pin map")
            balls[ball] = pad

    # Names already in use from the devicetree walk, as sanitized name ->
    # list of pins it is attached to.
    pins_of_name = {}
    for pin_key, names in board_names.items():
        for existing_name in names:
            pins = pins_of_name.setdefault(sanitize(existing_name), [])
            if pin_key not in pins:
                pins.append(pin_key)
    for name, package_pin in toml_pins.items():
        board_name = sanitize(name)
        if not re.match(r"^[A-Z][A-Z0-9_]*$", board_name):
            raise RuntimeError(
                f"circuitpython.toml [pins] name {name!r} is not usable as a board module name"
            )
        if isinstance(package_pin, str):
            ball = package_pin.strip().upper()
            pad = balls.get(ball)
            if pad is None:
                raise RuntimeError(
                    f"circuitpython.toml [pins] name {name}: ball {package_pin} is not in "
                    f"the package pin map"
                )
        elif isinstance(package_pin, int) and not isinstance(package_pin, bool):
            pad = package_pin_to_pad.get(package_pin)
            if pad is None:
                raise RuntimeError(
                    f"circuitpython.toml [pins] name {name}: package pin {package_pin} is "
                    f"not in the package pin map"
                )
        else:
            raise RuntimeError(
                f"circuitpython.toml [pins] name {name}: value must be a package pin number "
                f"or ball id"
            )
        gpio = gpio_pad_of_pad.get(pad)
        if gpio is None:
            # A pad whose package map entry carries no GPIO bond (an
            # analog-only pad): the pin object is created by the caller
            # from the map's datasheet pad name.
            # A pad no enabled GPIO controller covers (an analog-only pad):
            # the pin object is created by the caller from the map's
            # datasheet pad name.
            if pad not in pad_name_of_pad:
                raise RuntimeError(
                    f"circuitpython.toml [pins] name {name}: package pin {package_pin} is "
                    f"on pad {pad}, which has no GPIO bond and no pad name in the "
                    f"package pin map"
                )
            previous = pins_of_name.get(board_name, [])
            if previous and pad not in previous:
                previous = ", ".join(
                    f"{label} pin {num}" if isinstance(label, str) else f"pad {label}"
                    for label, num in previous
                )
                raise RuntimeError(
                    f"circuitpython.toml [pins] name {name}: already maps to {previous}, "
                    f"but [pins] assigns it to analog pad {pad}"
                )
            if previous:
                # Same name already attached to this pad (devicetree or an
                # earlier entry); nothing to add.
                continue
            pins_of_name[board_name] = [pad]
            analog_board_names.setdefault(pad, []).append(board_name)
            continue
        label = port_label_of_index.get(gpio // 32)
        if label is None or gpio % 32 not in ioports.get(label, []):
            raise RuntimeError(
                f"circuitpython.toml [pins] name {name}: pad {pad} bonds gpio pad "
                f"{gpio}, which is not on an enabled GPIO controller"
            )
        pin_key = (label, gpio % 32)
        previous_pins = pins_of_name.get(board_name, [])
        if previous_pins and pin_key not in previous_pins:
            previous = ", ".join(f"{label} pin {num}" for label, num in previous_pins)
            raise RuntimeError(
                f"circuitpython.toml [pins] name {name}: already maps to {previous}, "
                f"but [pins] assigns it to {label} pin {gpio % 32}"
            )
        if previous_pins:
            # Same name already attached to this pin (devicetree or an
            # earlier entry); nothing to add.
            continue
        pins_of_name[board_name] = [pin_key]
        board_names.setdefault(pin_key, []).append(board_name)
    return analog_board_names


def _toml_status_pin(mpconfigboard, board_names, key):
    """Resolve a circuitpython.toml status pin key to its ``(ioport, num)``.

    The key's value is a board pin name (``[pins]`` entries and
    devicetree-derived names alike, matched after sanitizing the same way).
    Returns None when the key is absent; raises when the name is unknown.
    """
    name = (mpconfigboard or {}).get(key)
    if name is None:
        return None
    wanted = str(name).upper().replace(" ", "_").replace("-", "_")
    for pin_key, names in board_names.items():
        if any(existing.upper() == wanted for existing in names):
            return pin_key
    raise RuntimeError(f"circuitpython.toml {key} = {name!r} is not a board pin name")


@cpbuild.run_in_thread
def zephyr_dts_to_cp_board(board_id, portdir, builddir, zephyrbuilddir, mpconfigboard=None):  # noqa: C901
    board_dir = builddir / "board"
    # Auto generate board files from device tree.

    board_info = {
        "wifi": False,
        "usb_device": False,
        "_bleio": False,
        "hostnetwork": board_id in ["native_sim"],
        "audiobusio": False,
        "neopixel_write": False,
    }

    config_bt_enabled = False
    config_bt_found = False
    config_adc_enabled = False
    config_dac_enabled = False
    config_rtc_enabled = False
    config_present = True
    config = zephyrbuilddir / ".config"
    if not config.exists():
        config_present = False
    else:
        for line in config.read_text().splitlines():
            if line.startswith("CONFIG_BT="):
                config_bt_enabled = line.strip().endswith("=y")
                config_bt_found = True
            elif line.startswith("# CONFIG_BT is not set"):
                config_bt_enabled = False
                config_bt_found = True
            elif line.startswith("CONFIG_ADC="):
                config_adc_enabled = line.strip().endswith("=y")
            elif line.startswith("CONFIG_DAC="):
                config_dac_enabled = line.strip().endswith("=y")
            elif line.startswith("CONFIG_RTC="):
                config_rtc_enabled = line.strip().endswith("=y")

    runners = zephyrbuilddir / "runners.yaml"
    runners = yaml.safe_load(runners.read_text())
    zephyr_board_dir = pathlib.Path(runners["config"]["board_dir"])
    board_yaml = zephyr_board_dir / "board.yml"
    board_yaml = yaml.safe_load(board_yaml.read_text())
    if "board" not in board_yaml and "boards" in board_yaml:
        for board in board_yaml["boards"]:
            if board["name"] == board_id:
                board_yaml = board
                break
    else:
        board_yaml = board_yaml["board"]
    board_info["vendor_id"] = board_yaml["vendor"]
    # Most vendors put boards directly in boards/<vendor>/<board>, but some group
    # them further, like boards/silabs/dev_kits/<board>. Walk up to the directory
    # named for the vendor so we read the vendor's index.rst and not a category's.
    vendor_dir = zephyr_board_dir.parent
    for parent in zephyr_board_dir.parents:
        if parent.name == board_info["vendor_id"]:
            vendor_dir = parent
            break
    vendor_index = vendor_dir / "index.rst"
    if vendor_index.exists():
        vendor_index = vendor_index.read_text()
        vendor_index = vendor_index.split("\n")
        vendor_name = vendor_index[2].strip()
    else:
        vendor_name = board_info["vendor_id"]
    board_info["vendor"] = vendor_name
    soc_name = board_yaml["socs"][0]["name"]
    board_info["soc"] = soc_name
    board_name = board_yaml["full_name"]
    if mpconfigboard and "NAME" in mpconfigboard:
        board_name = mpconfigboard["NAME"]
    board_info["name"] = board_name
    # board_id_yaml = zephyr_board_dir / (zephyr_board_dir.name + ".yaml")
    # board_id_yaml = yaml.safe_load(board_id_yaml.read_text())
    # print(board_id_yaml)
    # board_name = board_id_yaml["name"]

    dts = zephyrbuilddir / "zephyr.dts"
    device_tree = dtlib.DT(dts)
    node2alias = {}
    for alias in device_tree.alias2node:
        node = device_tree.alias2node[alias]
        if node not in node2alias:
            node2alias[node] = []
        node2alias[node].append(alias)
    ioports = {}
    all_ioports = []
    board_names = {}
    status_led = None
    status_led_inverted = False
    boot_button = None
    boot_button_active_high = False
    path2chosen = {}
    chosen2path = {}

    # Find flash and RAM regions using extracted functions
    flashes = find_flash_devices(device_tree)
    rams = find_ram_regions(device_tree)  # Returns filtered and sorted list

    # Store active Zephyr device labels per-driver so that we can make them available via board.
    active_zephyr_devices = {}
    usb_num_endpoint_pairs = 0
    ble_hardware_present = False
    adc_present = False
    dac_present = False
    for k in device_tree.root.nodes["chosen"].props:
        value = device_tree.root.nodes["chosen"].props[k]
        path2chosen[value.to_path()] = k
        chosen2path[k] = value.to_path()

    chosen_display = chosen2path.get("zephyr,display")
    if chosen_display is not None:
        status = chosen_display.props.get("status", None)
        if status is None or status.to_string() == "okay":
            board_info["zephyr_display"] = True
            board_info["displayio"] = True

    # A list, not a set, so the nodes are visited in the same order every build.
    remaining_nodes = [device_tree.root]
    while remaining_nodes:
        node = remaining_nodes.pop()
        remaining_nodes.extend(node.nodes.values())
        gpio = node.props.get("gpio-controller", False)
        gpio_map = node.props.get("gpio-map", [])
        status = node.props.get("status", None)
        if status is None:
            status = "okay"
        else:
            status = status.to_string()

        compatible = []
        if "compatible" in node.props:
            compatible = node.props["compatible"].to_strings()
        logger.debug(f"{node.name}: {status}")
        logger.debug(f"compatible: {compatible}")
        chosen = None
        if node in path2chosen:
            chosen = path2chosen[node]
            logger.debug(f" chosen: {chosen}")
        for c in compatible:
            underscored = c.replace(",", "_").replace("-", "_")
            driver = COMPAT_TO_DRIVER.get(underscored, None)
            if not driver:
                driver = MANUAL_COMPAT_TO_DRIVER.get(underscored, None)
            logger.debug(f" {c} -> {underscored} -> {driver}")
            if not driver or status != "okay":
                continue
            if driver == "flash":
                pass  # Handled by find_flash_devices()
            elif driver == "usb/udc" or "zephyr_udc0" in node.labels:
                board_info["usb_device"] = True
                props = node.props
                if "num-bidir-endpoints" not in props:
                    props = node.parent.props
                usb_num_endpoint_pairs = 0
                if "num-bidir-endpoints" in props:
                    usb_num_endpoint_pairs = props["num-bidir-endpoints"].to_num()
                single_direction_endpoints = []
                for d in ("in", "out"):
                    eps = f"num-{d}-endpoints"
                    single_direction_endpoints.append(props[eps].to_num() if eps in props else 0)
                # Count separate in/out pairs as bidirectional.
                usb_num_endpoint_pairs += min(single_direction_endpoints)
            elif driver.startswith("wifi"):
                board_info["wifi"] = True
            elif driver == "bluetooth/hci":
                ble_hardware_present = True
            elif driver in AUDIOBUSIO_CLASSES:
                # audiobusio driver (i2s, audio/dmic)
                board_info["audiobusio"] = True
                logger.info(f"Supported audiobusio driver: {driver}")
                if driver not in active_zephyr_devices:
                    active_zephyr_devices[driver] = []
                active_zephyr_devices[driver].append(node.labels)
            elif driver in EXCEPTIONAL_DRIVERS:
                pass
            elif driver in BUSIO_CLASSES:
                # busio driver (i2c, spi, uart)
                board_info["busio"] = True
                if driver == "spi":
                    board_info["async_spi"] = True
                logger.info(f"Supported busio driver: {driver}")
                if driver not in active_zephyr_devices:
                    active_zephyr_devices[driver] = []
                active_zephyr_devices[driver].append(node.labels)
            elif driver == "pwm":
                # PWM instances are allocated at runtime through iobroker
                # for neopixel_write and pwmio; no board singleton.
                logger.info(f"Supported pwm driver: {driver}")
                if driver not in active_zephyr_devices:
                    active_zephyr_devices[driver] = []
                active_zephyr_devices[driver].append(node.labels)
            elif driver in ("adc", "dac"):
                # Analog peripherals: analogio is only enabled for the
                # compatibles it implements (the SoC ADCs iobroker can resolve
                # to their fixed analog inputs, and the emulated devices);
                # external sensor ADC chips are not pad-addressable.
                if underscored in ANALOG_COMPAT:
                    if driver == "adc":
                        adc_present = True
                    else:
                        dac_present = True
                    logger.info(f"Supported analog driver: {underscored}")
                else:
                    logger.debug(f"Analog driver without analogio support: {underscored}")
            else:
                logger.warning(f"Unsupported driver: {driver}")

        if gpio:
            if "ngpios" in node.props:
                ngpios = node.props["ngpios"].to_num()
            else:
                ngpios = 32
            all_ioports.append(node.labels[0])
            if status == "okay":
                ioports[node.labels[0]] = range(0, ngpios)
        if gpio_map and compatible and compatible[0] != "gpio-nexus":
            # Per-board connector names from circuitpython.toml's
            # ``[connectors.<node label>]`` table take precedence. They key the
            # gpio-map position (the header pin number as a string) to a name
            # or a list of names, so boards whose silkscreen differs from the
            # generic per-compatible list can supply their own.
            connector_override = None
            if node.labels:
                connector_override = (
                    (mpconfigboard or {}).get("connectors", {}).get(node.labels[0])
                )
            connector_pins = (
                connector_override
                if connector_override is not None
                else CONNECTORS.get(compatible[0], None)
            )
            if connector_pins is None:
                logger.warning(f"Unsupported connector mapping compatible: {compatible[0]}")
            else:
                i = 0
                for offset, t, label in gpio_map._markers:
                    if not label:
                        continue
                    num = int.from_bytes(gpio_map.value[offset + 4 : offset + 8], "big")
                    if isinstance(connector_pins, dict):
                        pin_entry = connector_pins.get(str(i))
                        if pin_entry is None:
                            logger.debug(
                                f"Connector {node.labels[0]} position {i} has no name; skipping"
                            )
                            i += 1
                            continue
                    else:
                        if i >= len(connector_pins):
                            logger.warning(
                                f"Connector mapping for {compatible[0]} has more pins than names; "
                                f"stopping at {len(connector_pins)}"
                            )
                            break
                        pin_entry = connector_pins[i]
                    if (label, num) not in board_names:
                        board_names[(label, num)] = []
                    if isinstance(pin_entry, list):
                        board_names[(label, num)].extend(pin_entry)
                    else:
                        board_names[(label, num)].append(pin_entry)
                    i += 1
        if "gpio-leds" in compatible:
            for led in node.nodes:
                led = node.nodes[led]
                props = led.props
                ioport = props["gpios"]._markers[1][2]
                num = int.from_bytes(props["gpios"].value[4:8], "big")
                flags = int.from_bytes(props["gpios"].value[8:12], "big")
                if "label" in props:
                    if (ioport, num) not in board_names:
                        board_names[(ioport, num)] = []
                    board_names[(ioport, num)].append(props["label"].to_string())
                if led in node2alias:
                    if (ioport, num) not in board_names:
                        board_names[(ioport, num)] = []
                    if "led0" in node2alias[led]:
                        board_names[(ioport, num)].append("LED")
                        status_led = (ioport, num)
                        status_led_inverted = flags & GPIO_ACTIVE_LOW
                    board_names[(ioport, num)].extend(node2alias[led])

        if "gpio-keys" in compatible:
            for key in node.nodes:
                key_node = node.nodes[key]
                props = key_node.props
                ioport = props["gpios"]._markers[1][2]
                num = int.from_bytes(props["gpios"].value[4:8], "big")
                flags = int.from_bytes(props["gpios"].value[8:12], "big")
                active_high = not (flags & GPIO_ACTIVE_LOW)

                if (ioport, num) not in board_names:
                    board_names[(ioport, num)] = []
                # `label` is optional and deprecated on gpio-keys. Modern
                # boards identify keys with `zephyr,code`, so fall back to the
                # name of that code.
                if "label" in props:
                    board_names[(ioport, num)].append(props["label"].to_string())
                elif "zephyr,code" in props:
                    key_code = props["zephyr,code"].to_num()
                    if key_code in INPUT_KEY_NAMES:
                        board_names[(ioport, num)].append(INPUT_KEY_NAMES[key_code])
                if key_node in node2alias:
                    aliases = node2alias[key_node]
                    if "sw0" in aliases:
                        board_names[(ioport, num)].append("BUTTON")
                        # The sw0 alias designates the conventional first user
                        # button, so prefer it as the boot button.
                        boot_button = (ioport, num)
                        boot_button_active_high = active_high
                    board_names[(ioport, num)].extend(aliases)
                # Default to the first button in device tree order when no sw0
                # alias has designated one yet.
                if boot_button is None:
                    boot_button = (ioport, num)
                    boot_button_active_high = active_high

    if len(all_ioports) > 1:
        a, b = all_ioports[:2]
        i = 0
        max_i = min(len(a), len(b))
        while i < max_i and a[i] == b[i]:
            i += 1
        shared_prefix = a[:i]
        for ioport in ioports:
            if not ioport.startswith(shared_prefix):
                shared_prefix = ""
                break
    elif all_ioports:
        shared_prefix = all_ioports[0]
    else:
        shared_prefix = ""

    pin_defs = []
    pin_declarations = ["#pragma once"]
    mcu_pin_mapping = []
    board_pin_mapping = []
    # Hardware port index of each GPIO controller; it defines the global pin
    # numbering (index * 32 + pin) that pin objects and the iobroker module
    # both use. On nRF SoCs the index comes from the label digits (gpio0 ->
    # port 0, gpio6 -> port 6). Virtual GPIO controllers, such as the
    # infineon,cyw43-gpio on the Pico 2 W, get the lowest unused index so they
    # never collide with a numbered controller.
    port_indexes = {}
    used_indexes = set()
    for label in sorted(ioports.keys()):
        match = re.match(r"^gpio(\d+)$", label)
        if match:
            port_indexes[label] = int(match.group(1))
            used_indexes.add(port_indexes[label])
    for label in sorted(ioports.keys()):
        if label in port_indexes:
            continue
        index = 0
        while index in used_indexes:
            index += 1
        port_indexes[label] = index
        used_indexes.add(index)
    # Package pin map selected through the IOBROKER_PACKAGE choice: the pin
    # objects, their package pins and the reserved pads all come from the
    # iobroker package data. Pads are numbered by the package's pin ids
    # (row-major ball order, or the datasheet's pin number for a QFN/QFP
    # package); a pad bonded to a GPIO controller carries its global GPIO
    # number (port index * 32 + pin). The 1:1 choice is an identity map (pad
    # == package pin == GPIO number); IOBROKER_PACKAGE_NONE has no map, so
    # every pin object gets IOBROKER_NO_PIN.
    package_pin_of_pad = {}
    gpio_pad_of_pad = {}
    # Datasheet pad name of every mapped pad (e.g. "P0.02", "ANA_0"): the
    # pin objects take their names from these.
    pad_name_of_pad = {}
    package_pins = None
    package_choice = None
    if config_present:
        for line in config.read_text().splitlines():
            stripped = line.strip()
            if not stripped.startswith("CONFIG_IOBROKER_PACKAGE_") or not stripped.endswith("=y"):
                continue
            package_choice = stripped[len("CONFIG_IOBROKER_PACKAGE_") : -len("=y")].lower()
            break
        if package_choice == "one_to_one":
            # Identity map over the enabled GPIO controllers.
            package_pins = []
            for ioport in sorted(ioports.keys()):
                for num in ioports[ioport]:
                    global_number = port_indexes[ioport] * 32 + num
                    pin_object_name = f"P{ioport[len(shared_prefix) :].upper()}_{num:02d}"
                    package_pins.append(
                        {
                            "pin": global_number,
                            "pad": global_number,
                            "pad_name": pin_object_name,
                            "gpio": global_number,
                        }
                    )
                    package_pin_of_pad[global_number] = global_number
                    gpio_pad_of_pad[global_number] = global_number
                    pad_name_of_pad[global_number] = pin_object_name
        elif package_choice not in (None, "none"):
            package_toml = (
                pathlib.Path(__file__).resolve().parent.parent
                / "internal-modules"
                / "iobroker"
                / "packages"
                / f"{package_choice}.toml"
            )
            with package_toml.open("rb") as f:
                package = tomllib.load(f)
            package_pins = package["pins"]
            for package_pin_entry in package_pins:
                if "pad_name" not in package_pin_entry and "pad" not in package_pin_entry:
                    # Informational module pin (e.g. bonded to GND): it
                    # carries no SoC pad, so it maps nothing.
                    continue
                pad = package_pin_entry.get("pad", package_pin_entry["pin"])
                package_pin_of_pad[pad] = package_pin_entry["pin"]
                if "pad_name" in package_pin_entry:
                    pad_name_of_pad[pad] = package_pin_entry["pad_name"]
                if "gpio" in package_pin_entry:
                    gpio_pad_of_pad[pad] = package_pin_entry["gpio"]
    # Pads no package pin names (GPIO pads whose controller is missing from
    # the map or pads the board enables beyond the package): they get pin
    # objects named after the GPIO controller with an IOBROKER_NO_PIN
    # package pin, like boards without an iobroker package always had.
    # Board pin names from circuitpython.toml: ``[pins]`` maps a board module
    # name to a package pin number or ball id, resolved to a SoC pad with
    # the package pin map above. This is independent of Zephyr's devicetree
    # labels and aliases. Pads with no GPIO bond (the map's analog-only
    # pads) get their names back keyed by pad; their pin objects are
    # created below from the map's datasheet pad names.
    analog_board_names = add_toml_pin_names(
        board_names,
        mpconfigboard,
        package_pins,
        package_choice,
        port_indexes,
        ioports,
        pad_name_of_pad,
        gpio_pad_of_pad,
    )
    # Status NeoPixel from circuitpython.toml: ``STATUS_NEOPIXEL`` (and the
    # optional ``STATUS_NEOPIXEL_POWER``) name a board pin, usually one from
    # the [pins] table, that the status LED code drives with neopixel_write.
    status_neopixel = _toml_status_pin(mpconfigboard, board_names, "STATUS_NEOPIXEL")
    status_neopixel_power = _toml_status_pin(mpconfigboard, board_names, "STATUS_NEOPIXEL_POWER")
    # GPIO pad number -> pad: every (label, num) name and alias the
    # devicetree walk produced keys into one pin object per pad.
    pad_of_gpio = {}
    for pad, gpio in gpio_pad_of_pad.items():
        if gpio in pad_of_gpio and pad_of_gpio[gpio] != pad:
            logger.warning(
                f"package pin maps {pad_of_gpio[gpio]} and {pad} both bond gpio pad {gpio}"
            )
            continue
        pad_of_gpio[gpio] = pad
    # Pin object names, deduplicated: two pads with the same datasheet name
    # would produce two MP_QSTR bindings for one name.
    pin_name_of_pad = {}
    seen_pin_names = {}
    for pad, name in pad_name_of_pad.items():
        pin_object_name = (
            name.upper()
            .replace(".", "_")
            .replace(" ", "_")
            .replace("-", "_")
            .replace("(", "")
            .replace(")", "")
        )
        if not re.match(r"^[A-Z][A-Z0-9_]*$", pin_object_name):
            logger.warning(
                f"pad {pad} has unusable pad name {name!r} in the package "
                f"pin map; its pin object is skipped"
            )
            continue
        if pin_object_name in seen_pin_names:
            raise RuntimeError(
                f"package pin map: pads {seen_pin_names[pin_object_name]} and {pad} "
                f"both carry pad name {name!r}"
            )
        seen_pin_names[pin_object_name] = pad
        pin_name_of_pad[pad] = pin_object_name
    for ioport in sorted(ioports.keys()):
        for num in ioports[ioport]:
            gpio = port_indexes[ioport] * 32 + num
            pad = pad_of_gpio.get(gpio)
            if pad is not None:
                pin_object_name = pin_name_of_pad.get(pad)
                if pin_object_name is None:
                    continue
                package_pin = package_pin_of_pad[pad]
                package_pin_init = str(package_pin)
            else:
                # Unbonded GPIO pad: no package pin; keep the object for
                # devicetree names, with a disconnected package pin.
                pin_object_name = f"P{ioport[len(shared_prefix) :].upper()}_{num:02d}"
                package_pin_init = "IOBROKER_NO_PIN"
            if status_led and (ioport, num) == status_led:
                status_led = pin_object_name
            if boot_button and (ioport, num) == boot_button:
                boot_button = pin_object_name
            if status_neopixel and (ioport, num) == status_neopixel:
                status_neopixel = pin_object_name
            if status_neopixel_power and (ioport, num) == status_neopixel_power:
                status_neopixel_power = pin_object_name
            pin_defs.append(
                f"const mcu_pin_obj_t pin_{pin_object_name} = {{ .base.type = &mcu_pin_type, .package_pin = {package_pin_init}}};"
            )
            pin_declarations.append(f"extern const mcu_pin_obj_t pin_{pin_object_name};")
            mcu_pin_mapping.append(
                f"{{ MP_ROM_QSTR(MP_QSTR_{pin_object_name}), MP_ROM_PTR(&pin_{pin_object_name}) }},"
            )
            board_pin_names = board_names.get((ioport, num), [])
            for board_pin_name in board_pin_names:
                board_pin_name = (
                    board_pin_name.upper()
                    .replace(" ", "_")
                    .replace("-", "_")
                    .replace("(", "")
                    .replace(")", "")
                )
                board_pin_mapping.append(
                    f"{{ MP_ROM_QSTR(MP_QSTR_{board_pin_name}), MP_ROM_PTR(&pin_{pin_object_name}) }},"
                )

    # Pin objects for the package map's analog-only pads (pads with no GPIO
    # bond, e.g. MCX N's ANA pads), keyed by pad: they are named by their
    # datasheet pad name and reach the outside world only through the
    # analog APIs, which take package pins.
    for pad in sorted(analog_board_names):
        pin_object_name = pin_name_of_pad.get(pad)
        if pin_object_name is None:
            continue
        pin_defs.append(
            f"const mcu_pin_obj_t pin_{pin_object_name} = {{ .base.type = &mcu_pin_type, .package_pin = {package_pin_of_pad[pad]}}};"
        )
        pin_declarations.append(f"extern const mcu_pin_obj_t pin_{pin_object_name};")
        mcu_pin_mapping.append(
            f"{{ MP_ROM_QSTR(MP_QSTR_{pin_object_name}), MP_ROM_PTR(&pin_{pin_object_name}) }},"
        )
        for board_pin_name in analog_board_names[pad]:
            board_pin_mapping.append(
                f"{{ MP_ROM_QSTR(MP_QSTR_{board_pin_name}), MP_ROM_PTR(&pin_{pin_object_name}) }},"
            )

    pin_defs = "\n".join(pin_defs)
    pin_declarations = "\n".join(pin_declarations)
    board_pin_mapping = "\n    ".join(board_pin_mapping)
    mcu_pin_mapping = "\n    ".join(mcu_pin_mapping)

    # Bus instances the board enabled with all-disconnected pins are routed to
    # arbitrary pins at runtime by busio objects instead of being exposed as
    # fixed board.X() singletons.
    iobroker_labels = set()
    for driver in BUSIO_CLASSES:
        for labels in active_zephyr_devices.get(driver, []):
            node = device_tree.label2node[labels[0]]
            psels = _pinctrl_default_psels(node)
            if psels is not None and all(
                (value & NRF_PIN_FIELD_MASK) == NRF_PIN_FIELD_MASK for value in psels
            ):
                iobroker_labels.add(labels[0])

    zephyr_binding_headers = []
    zephyr_binding_objects = []
    zephyr_binding_labels = []
    i2sout_instance_names = []
    for driver, instances in active_zephyr_devices.items():
        # Determine if this is busio or audiobusio
        if driver in BUSIO_CLASSES:
            module = "busio"
            driverclass = BUSIO_CLASSES[driver]
        elif driver in AUDIOBUSIO_CLASSES:
            module = "audiobusio"
            driverclass = AUDIOBUSIO_CLASSES[driver]
        else:
            continue

        zephyr_binding_headers.append(f'#include "shared-bindings/{module}/{driverclass}.h"')

        # Designate a main device such as board.I2C or board.I2S.
        if len(instances) == 1:
            instances[0].append(driverclass)
        else:
            # Check to see if a main device has already been designated
            found_main = False
            for labels in instances:
                for label in labels:
                    if label == driverclass:
                        found_main = True
            if not found_main:
                for priority_label in (f"zephyr_{driver}", f"arduino_{driver}"):
                    for labels in instances:
                        if priority_label in labels:
                            labels.append(driverclass)
                            found_main = True
                            break
                    if found_main:
                        break
        for labels in instances:
            if labels[0] in iobroker_labels:
                # Dynamically routable instances are not exposed as board
                # singletons; construct a busio object with pins instead.
                continue
            instance_name = f"{driver.replace('/', '_')}_{labels[0]}"
            c_function_name = f"_{instance_name}"
            singleton_ptr = f"{c_function_name}_singleton"
            function_object = f"{c_function_name}_obj"
            obj_type = f"{module}_{driverclass.lower()}"

            # Handle special cases for different drivers
            if driver == "serial":
                # UART needs a receiver buffer
                buffer_decl = f"static byte {instance_name}_buffer[128];"
                construct_call = f"common_hal_busio_uart_construct_from_device(&{instance_name}_obj, DEVICE_DT_GET(DT_NODELABEL({labels[0]})), 128, {instance_name}_buffer)"
            else:
                # Default case (I2C, SPI, I2S)
                buffer_decl = ""
                construct_call = f"common_hal_{module}_{driverclass.lower()}_construct_from_device(&{instance_name}_obj, DEVICE_DT_GET(DT_NODELABEL({labels[0]})))"

            if driver == "i2s":
                i2sout_instance_names.append(instance_name)

            zephyr_binding_objects.append(
                f"""{buffer_decl}
static {obj_type}_obj_t {instance_name}_obj;
static mp_obj_t {singleton_ptr} = mp_const_none;
static mp_obj_t {c_function_name}(void) {{
    if ({singleton_ptr} != mp_const_none) {{
        return {singleton_ptr};
    }}
    {singleton_ptr} = {construct_call};
    return {singleton_ptr};
}}
static MP_DEFINE_CONST_FUN_OBJ_0({function_object}, {c_function_name});""".lstrip()
            )
            for label in labels:
                zephyr_binding_labels.append(
                    f"{{ MP_ROM_QSTR(MP_QSTR_{label.upper()}), MP_ROM_PTR(&{function_object}) }},"
                )
    zephyr_binding_headers = "\n".join(zephyr_binding_headers)
    zephyr_binding_objects = "\n".join(zephyr_binding_objects)
    zephyr_binding_labels = "\n".join(zephyr_binding_labels)

    # Generate tables of allocatable bus instances for the iobroker
    # Zephyr module (dynamic pin routing; nRF SoCs). Instances enabled with
    # all-disconnected pinctrl can be routed to arbitrary pins at runtime;
    # instances with fixed devicetree pins are only usable when the requested
    # pins match their state.
    pinctrl_nrf = False
    pwm_nrfx = False
    pwm_emul = False
    if config_present:
        for line in config.read_text().splitlines():
            if line.startswith("CONFIG_PINCTRL_NRF="):
                pinctrl_nrf = line.strip().endswith("=y")
            elif line.startswith("CONFIG_PWM_NRFX="):
                # Zephyr's nRF PWM driver defines the PWM instance devices.
                # Without it, PWM nodes have no device to reference, and
                # neopixel_write (the neopixel module's nRF implementation,
                # which allocates an instance through iobroker) cannot transmit, so
                # both the pwm pool and the module follow this symbol. pwmio
                # allocates its channels through iobroker's nRF implementation
                # too.
                pwm_nrfx = line.strip().endswith("=y")
            elif line.startswith("CONFIG_PWM_ADAFRUIT_EMUL="):
                # The emulated PWM controllers on native_sim (pwm_emul
                # module), which iobroker allocates pwmio channels on too.
                pwm_emul = line.strip().endswith("=y")
    board_info["neopixel_write"] = pwm_nrfx
    board_info["pwmio"] = pwm_nrfx or pwm_emul

    iobroker_includes = """
#include <zephyr/device.h>
#include <iobroker/iobroker.h>
"""

    iobroker_tables = ""
    table_parts = []

    # Map GPIO controller devices to their hardware port index. The indexes
    # define the global pin numbering shared by the pin objects and the
    # iobroker module, which resolves a global number back to the
    # controller device and pin within it. Boards without GPIO controllers
    # generate an empty table so that gpio_split() returns -EINVAL.
    if ioports:
        devices = ", ".join(
            f"DEVICE_DT_GET(DT_NODELABEL({label}))" for label in sorted(ioports.keys())
        )
        indexes = ", ".join(str(port_indexes[label]) for label in sorted(ioports.keys()))
        addrs = ", ".join(_reg_addr_expr(device_tree, label) for label in sorted(ioports.keys()))
        count = len(port_indexes)
    else:
        devices = "NULL"
        indexes = "0"
        addrs = "0"
        count = 0
    table_parts.append(
        f"""
const struct device * const iobroker_gpio_port_devices[] = {{ {devices} }};
const uint8_t iobroker_gpio_port_indexes[] = {{ {indexes} }};
const uint32_t iobroker_gpio_port_addrs[] = {{ {addrs} }};
const size_t iobroker_gpio_port_count = {count};
"""
    )

    if pinctrl_nrf:
        pool_kinds = (("i2c", "i2c"), ("spi", "spi"), ("serial", "uart"), ("pwm", "pwm"))
        bus_table_parts = []
        for driver, pool in pool_kinds:
            dynamic_entries = []
            fixed_entries = []
            # PWM instances only exist as devices when the driver is built.
            instances = (
                active_zephyr_devices.get(driver, []) if driver != "pwm" or pwm_nrfx else []
            )
            for labels in instances:
                node = device_tree.label2node[labels[0]]
                if node in path2chosen:
                    # Console and other system devices are not allocatable.
                    continue
                psels = _pinctrl_default_psels(node)
                if psels is None or len(psels) > 4:
                    continue
                if all((value & NRF_PIN_FIELD_MASK) == NRF_PIN_FIELD_MASK for value in psels):
                    dynamic_entries.append((labels[0], None, 0))
                else:
                    fixed_entries.append((labels[0], psels, len(psels)))

            entries = dynamic_entries + fixed_entries

            # Always define all four pools, even when empty: iobroker.c and
            # the nRF routing code reference every pool's tables whenever
            # CONFIG_PINCTRL_NRF is on, so an empty pool is still an empty
            # array plus a zero count.
            if not entries:
                bus_table_parts.append(
                    "const iobroker_instance_t"
                    f" iobroker_{pool}_buses[] = {{}};\n"
                    "iobroker_state_t"
                    f" iobroker_{pool}_bus_states[ARRAY_SIZE(iobroker_{pool}_buses)];\n"
                    "const size_t"
                    f" iobroker_{pool}_bus_count = ARRAY_SIZE(iobroker_{pool}_buses);"
                )
                continue

            entry_lines = []
            psel_arrays = []
            declares = []
            for label, psels, count in entries:
                declares.append(f"PINCTRL_DT_DEV_CONFIG_DECLARE(DT_NODELABEL({label}));")
                entry = (
                    f"    {{ .dev = DEVICE_DT_GET(DT_NODELABEL({label})), "
                    f".reg_addr = {_reg_addr_expr(device_tree, label)}, "
                    f".pcfg = PINCTRL_DT_DEV_CONFIG_GET(DT_NODELABEL({label}))"
                )
                if psels is not None:
                    values = ", ".join(hex(value) for value in psels)
                    psel_arrays.append(
                        f"static const pinctrl_soc_pin_t cp_{label}_dt_psels[] = {{ {values} }};"
                    )
                    entry += f", .dt_psels = cp_{label}_dt_psels, .dt_psel_count = {count}"
                entry += " },"
                entry_lines.append(entry)

            bus_table_parts.append(
                "\n".join(declares)
                + "\n\n"
                + "\n".join(psel_arrays)
                + f"\nconst iobroker_instance_t iobroker_{pool}_buses[] = {{\n"
                + "\n".join(entry_lines)
                + "\n};\n"
                + f"iobroker_state_t iobroker_{pool}_bus_states[ARRAY_SIZE(iobroker_{pool}_buses)];\n"
                + f"const size_t iobroker_{pool}_bus_count = ARRAY_SIZE(iobroker_{pool}_buses);"
            )

        iobroker_tables = (
            "#if defined(CONFIG_PINCTRL)\n"
            "#include <zephyr/drivers/pinctrl.h>\n"
            + "\n\n".join(bus_table_parts)
            + "\n#endif // CONFIG_PINCTRL"
        )

    if pinctrl_nrf:
        # Pads claimed at boot by fixed peripherals (console UART, flash
        # instance, I2S, ...): their devicetree pinctrl "default" state points
        # at real pads. iobroker reports these pads as always in use so that
        # allocate() rejects requests for them with -EBUSY instead of
        # re-routing pads that something else is already driving. Dynamically
        # routable instances have all-disconnected default states and
        # contribute nothing here. The pads are iobroker package pads: the
        # devicetree psels carry global GPIO numbers, so translate through
        # the package map; pads without a package pin stay unrestricted
        # (they are unreachable through the package pin APIs).
        reserved_pads = set()
        for instances in active_zephyr_devices.values():
            for labels in instances:
                node = device_tree.label2node[labels[0]]
                psels = _pinctrl_default_psels(node)
                if psels is None:
                    continue
                for value in psels:
                    gpio = value & NRF_PIN_FIELD_MASK
                    if gpio == NRF_PIN_FIELD_MASK:
                        continue
                    pad = pad_of_gpio.get(gpio)
                    if pad is not None:
                        reserved_pads.add(pad)
        # Always emit the table, even when empty: iobroker.c references it
        # whenever CONFIG_PINCTRL_NRF is on, so an empty set is still an
        # empty array plus a zero count.
        pads = ", ".join(str(pad) for pad in sorted(reserved_pads))
        reserved_table = (
            "const uint16_t iobroker_reserved_pads[] = { " + pads + " };\n"
            "const size_t iobroker_reserved_pads_count = "
            f"{len(reserved_pads)};"
        )
        iobroker_tables = (
            "#if defined(CONFIG_PINCTRL)\n"
            "#include <zephyr/drivers/pinctrl.h>\n"
            + "\n\n".join(bus_table_parts + [reserved_table])
            + "\n#endif // CONFIG_PINCTRL"
        )

    if table_parts:
        iobroker_tables = iobroker_tables + "\n\n" + "\n\n".join(table_parts)

    # Generate i2sout_reset() that stops all board I2SOut instances
    if i2sout_instance_names:
        stop_calls = "\n    ".join(
            f"common_hal_audiobusio_i2sout_stop(&{name}_obj);" for name in i2sout_instance_names
        )
        i2sout_reset_func = f"""
void i2sout_reset(void) {{
    {stop_calls}
}}"""
    else:
        i2sout_reset_func = ""

    zephyr_display_header = ""
    zephyr_display_object = ""
    zephyr_display_board_entry = ""
    if board_info.get("zephyr_display", False):
        zephyr_display_header = """
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include "shared-module/displayio/__init__.h"
#include "bindings/zephyr_display/Display.h"
        """.strip()
        zephyr_display_object = """
void board_init(void) {
#if CIRCUITPY_ZEPHYR_DISPLAY && DT_HAS_CHOSEN(zephyr_display)
    // Always allocate a display slot so board.DISPLAY is at least a valid
    // NoneType object even if the underlying Zephyr display is unavailable.
    primary_display_t *display_obj = allocate_display();
    if (display_obj == NULL) {
        return;
    }

    zephyr_display_display_obj_t *display = &display_obj->zephyr_display;
    display->base.type = &mp_type_NoneType;

    const struct device *display_dev = device_get_binding(DEVICE_DT_NAME(DT_CHOSEN(zephyr_display)));
    if (display_dev == NULL || !device_is_ready(display_dev)) {
        return;
    }

    display->base.type = &zephyr_display_display_type;
    common_hal_zephyr_display_display_construct_from_device(display, display_dev, 0, true);
#endif
}
        """.strip()
        zephyr_display_board_entry = (
            "{ MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].zephyr_display) },"
        )

    board_dir.mkdir(exist_ok=True, parents=True)
    header = board_dir / "mpconfigboard.h"
    if status_led:
        status_led = f"#define MICROPY_HW_LED_STATUS (&pin_{status_led})\n"
        status_led_inverted = (
            f"#define MICROPY_HW_LED_STATUS_INVERTED ({'1' if status_led_inverted else '0'})\n"
        )
    else:
        status_led = ""
        status_led_inverted = ""
    if boot_button:
        boot_button = f"#define CIRCUITPY_BOOT_BUTTON (&pin_{boot_button})\n"
        boot_button_active_high = (
            f"#define CIRCUITPY_BOOT_BUTTON_ACTIVE_HIGH "
            f"({'1' if boot_button_active_high else '0'})\n"
        )
    else:
        boot_button = ""
        boot_button_active_high = ""
    if status_neopixel:
        status_neopixel = f"#define MICROPY_HW_NEOPIXEL (&pin_{status_neopixel})\n"
    else:
        status_neopixel = ""
    if status_neopixel_power:
        status_neopixel_power = (
            f"#define CIRCUITPY_STATUS_LED_POWER (&pin_{status_neopixel_power})\n"
        )
    else:
        status_neopixel_power = ""
    ram_list = []
    ram_externs = []
    max_size = 0
    for ram in rams:
        device, start, end, size, path = ram
        max_size = max(max_size, size)
        # We always start at the end of a Zephyr linker section so we need the externs and &.
        # Native/simulated boards don't have real memory-mapped RAM, so we allocate static arrays.
        if board_id in ["native_sim"] or "bsim" in board_id:
            ram_externs.append("// This is a native board so we provide all of RAM for our heaps.")
            ram_externs.append(f"static uint32_t _{device}[{size // 4}]; // {path}")
            start = f"(const uint32_t *) (_{device})"
            end = f"(const uint32_t *)(_{device} + {size // 4})"
        else:
            ram_externs.append(f"extern uint32_t {start};")
            start = "&" + start
        ram_list.append(f"    {start}, {end}, // {path}")
    ram_list = "\n".join(ram_list)
    ram_externs = "\n".join(ram_externs)

    flashes = [f"DEVICE_DT_GET(DT_NODELABEL({flash}))" for flash in flashes]

    new_header_content = f"""#pragma once

#define MICROPY_HW_BOARD_NAME       "{board_name}"
#define MICROPY_HW_MCU_NAME         "{soc_name}"
#define CIRCUITPY_RAM_DEVICE_COUNT  {len(rams)}
{status_led}
{status_led_inverted}
{boot_button}
{boot_button_active_high}
{status_neopixel}
{status_neopixel_power}
        """
    if not header.exists() or header.read_text() != new_header_content:
        header.write_text(new_header_content)

    pins = board_dir / "autogen-pins.h"
    if not pins.exists() or pins.read_text() != pin_declarations:
        pins.write_text(pin_declarations)

    board_c = board_dir / "board.c"
    hostnetwork_include = ""
    hostnetwork_entry = ""
    if board_info.get("hostnetwork", False):
        hostnetwork_include = (
            '#if CIRCUITPY_HOSTNETWORK\n#include "bindings/hostnetwork/__init__.h"\n#endif\n'
        )
        hostnetwork_entry = (
            "#if CIRCUITPY_HOSTNETWORK\n"
            "    { MP_ROM_QSTR(MP_QSTR_NETWORK), MP_ROM_PTR(&common_hal_hostnetwork_obj) },\n"
            "#endif\n"
        )

    new_board_c_content = f"""
    // This file is autogenerated by build_circuitpython.py

#include "shared-bindings/board/__init__.h"

{hostnetwork_include}

#include <stdint.h>

#include "py/obj.h"
#include "py/mphal.h"

{zephyr_binding_headers}
{iobroker_includes}
{zephyr_display_header}

const struct device* const flashes[] = {{ {", ".join(flashes)} }};
const int circuitpy_flash_device_count = {len(flashes)};

{ram_externs}
const uint32_t* const ram_bounds[] = {{
{ram_list}
}};
const size_t circuitpy_max_ram_size = {max_size};

{pin_defs}

{zephyr_binding_objects}
{iobroker_tables}
{zephyr_display_object}
{i2sout_reset_func}

static const mp_rom_map_elem_t mcu_pin_globals_table[] = {{
{mcu_pin_mapping}
}};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {{
CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

{hostnetwork_entry}
{zephyr_display_board_entry}
{board_pin_mapping}

{zephyr_binding_labels}

}};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
"""
    # Only write board.c when it has changed. Rewriting it on every build gives it a new
    # modification time even if the content is the same, and cpbuild then recompiles it,
    # regenerates qstrdefs.generated.h from its qstrs, and recompiles every file that
    # includes that header, which is all of them.
    if not board_c.exists() or board_c.read_text() != new_board_c_content:
        board_c.write_text(new_board_c_content)
    if ble_hardware_present:
        if not config_present:
            raise RuntimeError(
                "Missing Zephyr .config; CONFIG_BT must be set explicitly when BLE hardware is present."
            )
        if not config_bt_found:
            raise RuntimeError(
                "CONFIG_BT is missing from Zephyr .config; set it explicitly when BLE hardware is present."
            )

    board_info["_bleio"] = ble_hardware_present and config_bt_enabled
    board_info["source_files"] = [board_c]
    board_info["cflags"] = ("-I", board_dir)
    board_info["flash_count"] = len(flashes)
    board_info["rotaryio"] = bool(ioports)
    # analogio requires both the devicetree device and the Zephyr driver:
    # the ADC path resolves pads to their analog inputs through iobroker and
    # the DAC path needs the emulated DAC for its counter tracks.
    board_info["analogio"] = (adc_present and config_adc_enabled) or (
        dac_present and config_dac_enabled
    )
    board_info["rtc"] = False
    # RTC needs both a devicetree `rtc` alias pointing at the RTC device
    # (the same selection Zephyr's own RTC sample uses) and the Zephyr RTC
    # driver layer enabled (CONFIG_RTC).
    rtc_node = device_tree.alias2node.get("rtc")
    if rtc_node is not None:
        rtc_status = rtc_node.props.get("status")
        rtc_present = rtc_status is None or rtc_status.to_string() == "okay"
        if rtc_present and config_rtc_enabled:
            board_info["rtc"] = True
        elif rtc_present:
            logger.warning(
                "Devicetree has a status-okay 'rtc' alias but CONFIG_RTC is "
                "not enabled, so the rtc module is disabled."
            )
    board_info["watchdog"] = True
    # Watchdog is always enabled: RAISE mode raises the WatchDogTimeout
    # exception from a software Zephyr k_timer and works on every board.
    # RESET mode additionally uses the hardware watchdog selected by the
    # devicetree `watchdog0` alias (the same selection Zephyr's own watchdog
    # sample uses); boards without the alias support only RAISE mode and
    # raise NotImplementedError when RESET is requested.
    board_info["usb_num_endpoint_pairs"] = usb_num_endpoint_pairs

    # Detect NVM partition size from the device tree.
    nvm_size = 0
    nvm_node = device_tree.label2node.get("nvm_partition")
    if nvm_node and "reg" in nvm_node.props:
        nvm_size = nvm_node.props["reg"].to_nums()[1]
    board_info["nvm_size"] = nvm_size

    # The user filesystem type is a compile-time choice made by the partition
    # layout: a littlefs_partition node (named for littlefs in the Adaboot
    # fork's layout dtsi) mounts littlefs; everything else mounts FAT.
    board_info["littlefs"] = device_tree.label2node.get("littlefs_partition") is not None

    return board_info
