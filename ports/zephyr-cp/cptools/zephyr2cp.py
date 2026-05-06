import logging
import pathlib

import cpbuild
import yaml
from compat2driver import COMPAT_TO_DRIVER
from devicetree import dtlib

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# GPIO flags defined here: include/zephyr/dt-bindings/gpio/gpio.h
GPIO_ACTIVE_LOW = 1 << 0

MINIMUM_RAM_SIZE = 1024

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

    flashes = []
    logger.debug("Flash devices:")

    # Traverse all nodes in the device tree
    remaining_nodes = set([device_tree.root])
    while remaining_nodes:
        node = remaining_nodes.pop()
        remaining_nodes.update(node.nodes.values())

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
    remaining_nodes = set([device_tree.root])
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

        remaining_nodes.update(node.nodes.values())

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
    }

    config_bt_enabled = False
    config_bt_found = False
    config_present = True
    config = zephyrbuilddir / ".config"
    if not config.exists():
        config_present = False
    else:
        for line in config.read_text().splitlines():
            if line.startswith("CONFIG_BT="):
                config_bt_enabled = line.strip().endswith("=y")
                config_bt_found = True
                break
            if line.startswith("# CONFIG_BT is not set"):
                config_bt_enabled = False
                config_bt_found = True
                break

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
    vendor_index = zephyr_board_dir.parent / "index.rst"
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
    path2chosen = {}
    chosen2path = {}

    # Find flash and RAM regions using extracted functions
    flashes = find_flash_devices(device_tree)
    rams = find_ram_regions(device_tree)  # Returns filtered and sorted list

    # Store active Zephyr device labels per-driver so that we can make them available via board.
    active_zephyr_devices = {}
    usb_num_endpoint_pairs = 0
    ble_hardware_present = False
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

    remaining_nodes = set([device_tree.root])
    while remaining_nodes:
        node = remaining_nodes.pop()
        remaining_nodes.update(node.nodes.values())
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
                logger.info(f"Supported busio driver: {driver}")
                if driver not in active_zephyr_devices:
                    active_zephyr_devices[driver] = []
                active_zephyr_devices[driver].append(node.labels)
            else:
                logger.warning(f"Unsupported driver: {driver}")

        if gpio:
            if "ngpios" in node.props:
                ngpios = node.props["ngpios"].to_num()
            else:
                ngpios = 32
            all_ioports.append(node.labels[0])
            if status == "okay":
                ioports[node.labels[0]] = set(range(0, ngpios))
        if gpio_map and compatible and compatible[0] != "gpio-nexus":
            connector_pins = CONNECTORS.get(compatible[0], None)
            if connector_pins is None:
                logger.warning(f"Unsupported connector mapping compatible: {compatible[0]}")
            else:
                i = 0
                for offset, t, label in gpio_map._markers:
                    if not label:
                        continue
                    if i >= len(connector_pins):
                        logger.warning(
                            f"Connector mapping for {compatible[0]} has more pins than names; "
                            f"stopping at {len(connector_pins)}"
                        )
                        break
                    num = int.from_bytes(gpio_map.value[offset + 4 : offset + 8], "big")
                    if (label, num) not in board_names:
                        board_names[(label, num)] = []
                    pin_entry = connector_pins[i]
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
                props = node.nodes[key].props
                ioport = props["gpios"]._markers[1][2]
                num = int.from_bytes(props["gpios"].value[4:8], "big")

                if (ioport, num) not in board_names:
                    board_names[(ioport, num)] = []
                board_names[(ioport, num)].append(props["label"].to_string())
                if key in node2alias:
                    if "sw0" in node2alias[key]:
                        board_names[(ioport, num)].append("BUTTON")
                    board_names[(ioport, num)].extend(node2alias[key])

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
    for ioport in sorted(ioports.keys()):
        for num in ioports[ioport]:
            pin_object_name = f"P{ioport[len(shared_prefix) :].upper()}_{num:02d}"
            if status_led and (ioport, num) == status_led:
                status_led = pin_object_name
            pin_defs.append(
                f"const mcu_pin_obj_t pin_{pin_object_name} = {{ .base.type = &mcu_pin_type, .port = DEVICE_DT_GET(DT_NODELABEL({ioport})), .number = {num}}};"
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

    pin_defs = "\n".join(pin_defs)
    pin_declarations = "\n".join(pin_declarations)
    board_pin_mapping = "\n    ".join(board_pin_mapping)
    mcu_pin_mapping = "\n    ".join(mcu_pin_mapping)

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
    board_info["usb_num_endpoint_pairs"] = usb_num_endpoint_pairs

    # Detect NVM partition from the device tree.
    nvm_node = device_tree.label2node.get("nvm_partition")
    board_info["nvm"] = nvm_node is not None

    return board_info
