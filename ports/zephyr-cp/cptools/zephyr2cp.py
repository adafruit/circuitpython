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
    "nordic_nrf_spim": "spi",
}

# These are controllers, not the flash devices themselves.
BLOCKED_FLASH_COMPAT = (
    "renesas,ra-qspi",
    "renesas,ra-ospi-b",
    "nordic,nrf-spim",
)

BUSIO_CLASSES = {"serial": "UART", "i2c": "I2C", "spi": "SPI"}

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
def zephyr_dts_to_cp_board(board_id, portdir, builddir, zephyrbuilddir):  # noqa: C901
    board_dir = builddir / "board"
    # Auto generate board files from device tree.

    board_info = {
        "wifi": False,
        "usb_device": False,
    }

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
    for k in device_tree.root.nodes["chosen"].props:
        value = device_tree.root.nodes["chosen"].props[k]
        path2chosen[value.to_path()] = k
        chosen2path[k] = value.to_path()
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
        if gpio_map and compatible[0] != "gpio-nexus":
            i = 0
            for offset, t, label in gpio_map._markers:
                if not label:
                    continue
                num = int.from_bytes(gpio_map.value[offset + 4 : offset + 8], "big")
                if (label, num) not in board_names:
                    board_names[(label, num)] = []
                board_names[(label, num)].append(CONNECTORS[compatible[0]][i])
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
        while a[i] == b[i]:
            i += 1
        shared_prefix = a[:i]
        for ioport in ioports:
            if not ioport.startswith(shared_prefix):
                shared_prefix = ""
                break
    else:
        shared_prefix = all_ioports[0]

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
                board_pin_name = board_pin_name.upper().replace(" ", "_").replace("-", "_")
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
    for driver, instances in active_zephyr_devices.items():
        driverclass = BUSIO_CLASSES[driver]
        zephyr_binding_headers.append(f'#include "shared-bindings/busio/{driverclass}.h"')

        # Designate a main bus such as board.I2C.
        if len(instances) == 1:
            instances[0].append(driverclass)
        else:
            # Check to see if a main bus has already been designated
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
            instance_name = f"{driver}_{labels[0]}"
            c_function_name = f"_{instance_name}"
            singleton_ptr = f"{c_function_name}_singleton"
            function_object = f"{c_function_name}_obj"
            busio_type = f"busio_{driverclass.lower()}"

            # UART needs a receiver buffer
            if driver == "serial":
                buffer_decl = f"static byte {instance_name}_buffer[128];"
                construct_call = f"common_hal_busio_uart_construct_from_device(&{instance_name}_obj, DEVICE_DT_GET(DT_NODELABEL({labels[0]})), 128, {instance_name}_buffer)"
            else:
                buffer_decl = ""
                construct_call = f"common_hal_busio_{driverclass.lower()}_construct_from_device(&{instance_name}_obj, DEVICE_DT_GET(DT_NODELABEL({labels[0]})))"

            zephyr_binding_objects.append(
                f"""{buffer_decl}
static {busio_type}_obj_t {instance_name}_obj;
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
        if board_id in ["native_sim"]:
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
    new_board_c_content = f"""
    // This file is autogenerated by build_circuitpython.py

#include "shared-bindings/board/__init__.h"

#include <stdint.h>

#include "py/obj.h"
#include "py/mphal.h"

{zephyr_binding_headers}

const struct device* const flashes[] = {{ {", ".join(flashes)} }};
const int circuitpy_flash_device_count = {len(flashes)};

{ram_externs}
const uint32_t* const ram_bounds[] = {{
{ram_list}
}};
const size_t circuitpy_max_ram_size = {max_size};

{pin_defs}

{zephyr_binding_objects}

static const mp_rom_map_elem_t mcu_pin_globals_table[] = {{
{mcu_pin_mapping}
}};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {{
CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

{board_pin_mapping}

{zephyr_binding_labels}

}};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
"""
    board_c.write_text(new_board_c_content)
    board_info["source_files"] = [board_c]
    board_info["cflags"] = ("-I", board_dir)
    board_info["flash_count"] = len(flashes)
    board_info["usb_num_endpoint_pairs"] = usb_num_endpoint_pairs
    return board_info
