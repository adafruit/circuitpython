import logging
import pathlib
import cpbuild

from devicetree import dtlib
import yaml

from compat2driver import COMPAT_TO_DRIVER

logger = logging.getLogger(__name__)

MANUAL_COMPAT_TO_DRIVER = {
    "renesas_ra_nv_flash": "flash",
}

# These are controllers, not the flash devices themselves.
BLOCKED_FLASH_COMPAT = (
    "renesas,ra-qspi",
    "renesas,ra-ospi-b",
    "nordic,nrf-spim",
)

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
    "renesas,ra-gpio-mipi-header": [
        "IIC_SDA",
        "DISP_BLEN",
        "IIC_SCL",
        "DISP_INT",
        "DISP_RST",
    ],
}


@cpbuild.run_in_thread
def zephyr_dts_to_cp_board(builddir, zephyrbuilddir):  # noqa: C901
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
    board_info["vendor_id"] = board_yaml["board"]["vendor"]
    vendor_index = zephyr_board_dir.parent / "index.rst"
    if vendor_index.exists():
        vendor_index = vendor_index.read_text()
        vendor_index = vendor_index.split("\n")
        vendor_name = vendor_index[2].strip()
    else:
        vendor_name = board_info["vendor_id"]
    board_info["vendor"] = vendor_name
    soc_name = board_yaml["board"]["socs"][0]["name"]
    board_info["soc"] = soc_name
    board_name = board_yaml["board"]["full_name"]
    board_info["name"] = board_name
    # board_id_yaml = zephyr_board_dir / (zephyr_board_dir.name + ".yaml")
    # board_id_yaml = yaml.safe_load(board_id_yaml.read_text())
    # print(board_id_yaml)
    # board_name = board_id_yaml["name"]

    dts = zephyrbuilddir / "zephyr.dts"
    edt_pickle = dtlib.DT(dts)
    node2alias = {}
    for alias in edt_pickle.alias2node:
        node = edt_pickle.alias2node[alias]
        if node not in node2alias:
            node2alias[node] = []
        node2alias[node].append(alias)
    ioports = {}
    all_ioports = []
    board_names = {}
    flashes = []
    rams = []
    status_led = None
    path2chosen = {}
    chosen2path = {}
    usb_num_endpoint_pairs = 0
    for k in edt_pickle.root.nodes["chosen"].props:
        value = edt_pickle.root.nodes["chosen"].props[k]
        path2chosen[value.to_path()] = k
        chosen2path[k] = value.to_path()
    remaining_nodes = set([edt_pickle.root])
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
        logger.debug(node.name, status)
        chosen = None
        if node in path2chosen:
            chosen = path2chosen[node]
            logger.debug(" chosen:", chosen)
        for c in compatible:
            underscored = c.replace(",", "_").replace("-", "_")
            driver = COMPAT_TO_DRIVER.get(underscored, None)
            if "mmio" in c:
                logger.debug(" ", c, node.labels, node.props)
                address, size = node.props["reg"].to_nums()
                end = address + size
                if chosen == "zephyr,sram":
                    start = "z_mapped_end"
                elif "zephyr,memory-region" in node.props:
                    start = "__" + node.props["zephyr,memory-region"].to_string() + "_end"
                else:
                    # Check to see if the chosen sram is a subset of this region. If it is,
                    # then do as above for a smaller region and assume the rest is reserved.
                    chosen_sram = chosen2path["zephyr,sram"]
                    chosen_address, chosen_size = chosen_sram.props["reg"].to_nums()
                    chosen_end = chosen_address + chosen_size
                    if address <= chosen_address <= end and address <= chosen_end <= end:
                        start = "z_mapped_end"
                        address = chosen_address
                        size = chosen_size
                        end = chosen_end
                    else:
                        start = address
                info = (node.labels[0], start, end, size, node.path)
                if chosen == "zephyr,sram":
                    rams.insert(0, info)
                else:
                    rams.append(info)
            if not driver:
                driver = MANUAL_COMPAT_TO_DRIVER.get(underscored, None)
            logger.debug(" ", underscored, driver)
            if not driver:
                continue
            if driver == "flash" and status == "okay":
                if not chosen and compatible[0] not in BLOCKED_FLASH_COMPAT:
                    # Skip chosen nodes because they are used by Zephyr.
                    flashes.append(f"DEVICE_DT_GET(DT_NODELABEL({node.labels[0]}))")
                else:
                    logger.debug("  skipping due to blocked compat")
            if driver == "usb/udc" and status == "okay":
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
            if driver.startswith("wifi") and status == "okay":
                board_info["wifi"] = True

        if gpio:
            if "ngpios" in node.props:
                ngpios = node.props["ngpios"].to_num()
            else:
                ngpios = 32
            all_ioports.append(node.labels[0])
            if status == "okay":
                ioports[node.labels[0]] = set(range(0, ngpios))
        if gpio_map:
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

    a, b = all_ioports[:2]
    i = 0
    while a[i] == b[i]:
        i += 1
    shared_prefix = a[:i]
    for ioport in ioports:
        if not ioport.startswith(shared_prefix):
            shared_prefix = ""
            break

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

    board_dir.mkdir(exist_ok=True, parents=True)
    header = board_dir / "mpconfigboard.h"
    if status_led:
        status_led = f"#define MICROPY_HW_LED_STATUS (&pin_{status_led})\n"
    else:
        status_led = ""
    ram_list = []
    ram_externs = []
    max_size = 0
    for ram in rams:
        device, start, end, size, path = ram
        max_size = max(max_size, size)
        if isinstance(start, str):
            ram_externs.append(f"extern uint32_t {start};")
            start = "&" + start
        else:
            start = f"(uint32_t*) 0x{start:08x}"
        ram_list.append(f"    {start}, (uint32_t*) 0x{end:08x}, // {path}")
    ram_list = "\n".join(ram_list)
    ram_externs = "\n".join(ram_externs)

    new_header_content = f"""#pragma once

#define MICROPY_HW_BOARD_NAME       "{board_name}"
#define MICROPY_HW_MCU_NAME         "{soc_name}"
#define CIRCUITPY_RAM_DEVICE_COUNT  {len(rams)}
{status_led}
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

const struct device* const flashes[] = {{ {", ".join(flashes)} }};
const int circuitpy_flash_device_count = {len(flashes)};

{ram_externs}
const uint32_t* const ram_bounds[] = {{
{ram_list}
}};
const size_t circuitpy_max_ram_size = {max_size};

{pin_defs}

static const mp_rom_map_elem_t mcu_pin_globals_table[] = {{
{mcu_pin_mapping}
}};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_globals_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {{
CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

{board_pin_mapping}

// {{ MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) }},
}};

MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
"""
    if not board_c.exists() or new_board_c_content != board_c.read_text():
        board_c.write_text(new_board_c_content)
    board_info["source_files"] = [board_c]
    board_info["cflags"] = ("-I", board_dir)
    board_info["flash_count"] = len(flashes)
    board_info["usb_num_endpoint_pairs"] = usb_num_endpoint_pairs
    return board_info
