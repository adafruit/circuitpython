import asyncio
import colorlog
import sys
import logging
import os
import pathlib
import tomllib
import tomlkit
import yaml
import pickle

import cpbuild
import board_tools

logger = logging.getLogger(__name__)

# print("hello zephyr", sys.argv)

# print(os.environ)
cmake_args = {}
for var in sys.argv[1:]:
    key, value = var.split("=", 1)
    cmake_args[key] = value

# Path to ports/zephyr-cp
portdir = pathlib.Path(cmake_args["PORT_SRC_DIR"])

# Path to CP root
srcdir = portdir.parent.parent

# Path to where CMake wants to put our build output.
builddir = pathlib.Path.cwd()

zephyrdir = portdir / "zephyr"

# Path to where CMake puts Zephyr's build output.
zephyrbuilddir = builddir / ".." / ".." / ".." / "zephyr"

sys.path.append(str(portdir / "zephyr/scripts/dts/python-devicetree/src/"))
from zephyr2cp import zephyr_dts_to_cp_board

compiler = cpbuild.Compiler(srcdir, builddir, cmake_args)

ALWAYS_ON_MODULES = ["sys", "collections"]
DEFAULT_MODULES = [
    "time",
    "os",
    "microcontroller",
    "struct",
    "array",
    "json",
    "random",
    "digitalio",
    "zephyr_serial",
]
MPCONFIG_FLAGS = ["ulab", "nvm", "displayio", "warnings", "alarm", "array", "json"]


async def preprocess_and_split_defs(compiler, source_file, build_path, flags):
    build_file = source_file.with_suffix(".pp")
    build_file = build_path / (build_file.relative_to(srcdir))
    await compiler.preprocess(source_file, build_file, flags=flags)
    async with asyncio.TaskGroup() as tg:
        for mode in ("qstr", "module", "root_pointer"):
            split_file = build_file.relative_to(build_path).with_suffix(f".{mode}")
            split_file = build_path / "genhdr" / mode / split_file
            split_file.parent.mkdir(exist_ok=True, parents=True)
            tg.create_task(
                cpbuild.run_command(
                    [
                        "python",
                        srcdir / "py/makeqstrdefs.py",
                        "split",
                        mode,
                        build_file,
                        build_path / "genhdr" / mode,
                        split_file,
                    ],
                    srcdir,
                )
            )


async def collect_defs(mode, build_path):
    output_file = build_path / f"{mode}defs.collected"
    splitdir = build_path / "genhdr" / mode
    await cpbuild.run_command(
        ["cat", "-s", *splitdir.glob(f"**/*.{mode}"), ">", output_file],
        splitdir,
    )
    return output_file


async def generate_qstr_headers(build_path, compiler, flags, translation):
    collected = await collect_defs("qstr", build_path)
    generated = build_path / "genhdr" / "qstrdefs.generated.h"

    await cpbuild.run_command(
        ["python", srcdir / "py" / "makeqstrdata.py", collected, ">", generated],
        srcdir,
    )

    compression_level = 9

    # TODO: Do this alongside qstr stuff above.
    await cpbuild.run_command(
        [
            "python",
            srcdir / "tools" / "msgfmt.py",
            "-o",
            build_path / f"{translation}.mo",
            srcdir / "locale" / f"{translation}.po",
        ],
        srcdir,
    )

    await cpbuild.run_command(
        [
            "python",
            srcdir / "py" / "maketranslationdata.py",
            "--compression_filename",
            build_path / "genhdr" / "compressed_translations.generated.h",
            "--translation",
            build_path / f"{translation}.mo",
            "--translation_filename",
            build_path / f"translations-{translation}.c",
            "--qstrdefs_filename",
            generated,
            "--compression_level",
            compression_level,
            generated,
        ],
        srcdir,
    )


async def generate_module_header(build_path):
    collected = await collect_defs("module", build_path)
    await cpbuild.run_command(
        [
            "python",
            srcdir / "py" / "makemoduledefs.py",
            collected,
            ">",
            build_path / "genhdr" / "moduledefs.h",
        ],
        srcdir,
    )


async def generate_root_pointer_header(build_path):
    collected = await collect_defs("root_pointer", build_path)
    await cpbuild.run_command(
        [
            "python",
            srcdir / "py" / "make_root_pointers.py",
            collected,
            ">",
            build_path / "genhdr" / "root_pointers.h",
        ],
        srcdir,
    )


TINYUSB_SETTINGS = {
    "": {
        "CFG_TUSB_MCU": "OPT_MCU_MIMXRT10XX",
        "CFG_TUD_CDC_RX_BUFSIZE": 640,
        "CFG_TUD_CDC_TX_BUFSIZE": 512,
    },
    "stm32u575xx": {"CFG_TUSB_MCU": "OPT_MCU_STM32U5"},
    "nrf52840": {"CFG_TUSB_MCU": "OPT_MCU_NRF5X"},
    "nrf5340": {"CFG_TUSB_MCU": "OPT_MCU_NRF5X"},
    # "r7fa8d1bhecbd": {"CFG_TUSB_MCU": "OPT_MCU_RAXXX", "USB_HIGHSPEED": "1", "USBHS_USB_INT_RESUME_IRQn": "54", "USBFS_INT_IRQn": "54", "CIRCUITPY_USB_DEVICE_INSTANCE": "1"},
    # ifeq ($(CHIP_FAMILY),$(filter $(CHIP_FAMILY),MIMXRT1011 MIMXRT1015))
    # CFLAGS += -DCFG_TUD_MIDI_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=64 -DCFG_TUD_MSC_BUFSIZE=512
    # else
    # CFLAGS += -DCFG_TUD_MIDI_RX_BUFSIZE=512 -DCFG_TUD_MIDI_TX_BUFSIZE=512 -DCFG_TUD_MSC_BUFSIZE=1024
    # endif
}

TINYUSB_SOURCE = {
    "stm32u575xx": [
        "src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c",
        "src/portable/synopsys/dwc2/dcd_dwc2.c",
        "src/portable/synopsys/dwc2/hcd_dwc2.c",
        "src/portable/synopsys/dwc2/dwc2_common.c",
    ],
    "nrf52840": [
        "src/portable/nordic/nrf5x/dcd_nrf5x.c",
    ],
    "nrf5340": [
        "src/portable/nordic/nrf5x/dcd_nrf5x.c",
    ],
    # "r7fa8d1bhecbd": [
    #     "src/portable/renesas/rusb2/dcd_rusb2.c",
    #     "src/portable/renesas/rusb2/hcd_rusb2.c",
    #     "src/portable/renesas/rusb2/rusb2_common.c",
    # ],
}


async def build_circuitpython():
    circuitpython_flags = ["-DCIRCUITPY"]
    port_flags = []
    enable_mpy_native = False
    full_build = False
    usb_host = False
    tusb_mem_align = 4
    board = cmake_args["BOARD_ALIAS"]
    if not board:
        board = cmake_args["BOARD"]
    translation = cmake_args["TRANSLATION"]
    if not translation:
        translation = "en_US"
    for module in ALWAYS_ON_MODULES:
        circuitpython_flags.append(f"-DCIRCUITPY_{module.upper()}=1")
    lto = cmake_args.get("LTO", "n") == "y"
    circuitpython_flags.append(f"-DCIRCUITPY_ENABLE_MPY_NATIVE={1 if enable_mpy_native else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_FULL_BUILD={1 if full_build else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_USB_HOST={1 if usb_host else 0}")
    circuitpython_flags.append(f'-DCIRCUITPY_BOARD_ID=\\"{board}\\"')
    circuitpython_flags.append(f"-DCIRCUITPY_TUSB_MEM_ALIGN={tusb_mem_align}")
    circuitpython_flags.append(f"-DCIRCUITPY_TRANSLATE_OBJECT={1 if lto else 0}")
    circuitpython_flags.append("-DINTERNAL_FLASH_FILESYSTEM")
    circuitpython_flags.append("-DLONGINT_IMPL_MPZ")
    circuitpython_flags.append("-DCIRCUITPY_SSL_MBEDTLS")
    circuitpython_flags.append('-DFFCONF_H=\\"lib/oofatfs/ffconf.h\\"')
    circuitpython_flags.extend(("-I", srcdir))
    circuitpython_flags.extend(("-I", srcdir / "lib/tinyusb/src"))
    circuitpython_flags.extend(("-I", srcdir / "supervisor/shared/usb"))
    circuitpython_flags.extend(("-I", builddir))
    circuitpython_flags.extend(("-I", portdir))
    # circuitpython_flags.extend(("-I", srcdir / "ports" / port / "peripherals"))

    # circuitpython_flags.extend(("-I", build_path / board_id))

    genhdr = builddir / "genhdr"
    genhdr.mkdir(exist_ok=True, parents=True)
    version_header = genhdr / "mpversion.h"
    async with asyncio.TaskGroup() as tg:
        tg.create_task(
            cpbuild.run_command(
                [
                    "python",
                    srcdir / "py" / "makeversionhdr.py",
                    version_header,
                    "&&",
                    "touch",
                    version_header,
                ],
                srcdir,
                check_hash=[version_header],
            )
        )

        board_autogen_task = tg.create_task(zephyr_dts_to_cp_board(builddir, zephyrbuilddir))
    board_info = board_autogen_task.result()
    mpconfigboard_fn = board_tools.find_mpconfigboard(portdir, board)
    mpconfigboard = {
        "USB_VID": 0x1209,
        "USB_PID": 0x000C,
    }
    if mpconfigboard_fn is None:
        mpconfigboard_fn = (
            portdir / "boards" / board_info["vendor_id"] / board / "circuitpython.toml"
        )
        logging.warning(
            f"Could not find board config at: boards/{board_info['vendor_id']}/{board}"
        )
    elif mpconfigboard_fn.exists():
        with mpconfigboard_fn.open("rb") as f:
            mpconfigboard = tomllib.load(f)

    autogen_board_info_fn = mpconfigboard_fn.parent / "autogen_board_info.toml"

    enabled_modules = set(DEFAULT_MODULES)
    module_reasons = {}
    if board_info["wifi"]:
        enabled_modules.add("wifi")
        module_reasons["wifi"] = "Zephyr board has wifi"

    if board_info["flash_count"] > 0:
        enabled_modules.add("storage")
        module_reasons["storage"] = "Zephyr board has flash"

    if "wifi" in enabled_modules:
        enabled_modules.add("socketpool")
        enabled_modules.add("ssl")
        module_reasons["socketpool"] = "Zephyr networking enabled"
        module_reasons["ssl"] = "Zephyr networking enabled"

    circuitpython_flags.extend(board_info["cflags"])
    supervisor_source = [
        "main.c",
        "extmod/vfs_fat.c",
        "lib/tlsf/tlsf.c",
        portdir / "background.c",
        portdir / "common-hal/microcontroller/__init__.c",
        portdir / "common-hal/microcontroller/Pin.c",
        portdir / "common-hal/microcontroller/Processor.c",
        portdir / "common-hal/os/__init__.c",
        "shared/readline/readline.c",
        "shared/runtime/context_manager_helpers.c",
        "shared/runtime/pyexec.c",
        "shared/runtime/interrupt_char.c",
        "shared/runtime/stdout_helpers.c",
        "shared/runtime/sys_stdio_mphal.c",
        "shared-bindings/board/__init__.c",
        "shared-bindings/supervisor/Runtime.c",
        "shared-bindings/microcontroller/Pin.c",
        "shared-bindings/util.c",
        "shared-module/board/__init__.c",
        "extmod/vfs_reader.c",
        "extmod/vfs_blockdev.c",
        "extmod/vfs_fat_file.c",
    ]
    top = srcdir
    supervisor_source = [pathlib.Path(p) for p in supervisor_source]
    supervisor_source.extend(board_info["source_files"])
    supervisor_source.extend(top.glob("supervisor/shared/*.c"))
    supervisor_source.append(top / "supervisor/shared/translate/translate.c")
    # if web_workflow:
    #     supervisor_source.extend(top.glob("supervisor/shared/web_workflow/*.c"))

    usb_num_endpoint_pairs = board_info.get("usb_num_endpoint_pairs", 0)
    soc = board_info["soc"]
    usb_ok = usb_num_endpoint_pairs > 0 and soc in TINYUSB_SETTINGS
    circuitpython_flags.append(f"-DCIRCUITPY_TINYUSB={1 if usb_ok else 0}")
    circuitpython_flags.append(f"-DCIRCUITPY_USB_DEVICE={1 if usb_ok else 0}")

    tinyusb_files = []
    if usb_ok:
        enabled_modules.add("usb_cdc")
        for setting in TINYUSB_SETTINGS[soc]:
            circuitpython_flags.append(f"-D{setting}={TINYUSB_SETTINGS[soc][setting]}")
        tinyusb_files.extend((top / "lib" / "tinyusb" / path for path in TINYUSB_SOURCE[soc]))
        for macro in ("USB_PID", "USB_VID"):
            circuitpython_flags.append(f"-D{macro}=0x{mpconfigboard.get(macro):04x}")
        for macro, limit, value in (
            ("USB_PRODUCT", 16, board_info["name"]),
            ("USB_MANUFACTURER", 8, board_info["vendor"]),
        ):
            circuitpython_flags.append(f"-D{macro}='\"{value}\"'")
            circuitpython_flags.append(f"-D{macro}_{limit}='\"{value[:limit]}\"'")

        usb_interface_name = "CircuitPython"

        circuitpython_flags.append("-DCFG_TUSB_OS=OPT_OS_ZEPHYR")
        circuitpython_flags.append(f"-DUSB_INTERFACE_NAME='\"{usb_interface_name}\"'")
        circuitpython_flags.append(f"-DUSB_NUM_ENDPOINT_PAIRS={usb_num_endpoint_pairs}")
        for direction in ("IN", "OUT"):
            circuitpython_flags.append(f"-DUSB_NUM_{direction}_ENDPOINTS={usb_num_endpoint_pairs}")
        # USB is special because it doesn't have a matching module.
        msc_enabled = board_info["flash_count"] > 0
        if msc_enabled:
            circuitpython_flags.append("-DCFG_TUD_MSC_BUFSIZE=1024")
            circuitpython_flags.append("-DCIRCUITPY_USB_MSC_ENABLED_DEFAULT=1")
            tinyusb_files.append(top / "lib/tinyusb/src/class/msc/msc_device.c")
            supervisor_source.append(top / "supervisor/shared/usb/usb_msc_flash.c")
        circuitpython_flags.append(f"-DCIRCUITPY_USB_MSC={1 if msc_enabled else 0}")
        if "usb_cdc" in enabled_modules:
            tinyusb_files.extend(top.glob("lib/tinyusb/*.c"))
            tinyusb_files.append(top / "lib/tinyusb/src/class/cdc/cdc_device.c")
            circuitpython_flags.append("-DCFG_TUD_CDC_RX_BUFSIZE=640")
            circuitpython_flags.append("-DCFG_TUD_CDC_TX_BUFSIZE=512")
            circuitpython_flags.append("-DCFG_TUD_CDC=2")
            circuitpython_flags.append("-DCIRCUITPY_USB_CDC_CONSOLE_ENABLED_DEFAULT=1")
            circuitpython_flags.append("-DCIRCUITPY_USB_CDC_DATA_ENABLED_DEFAULT=0")

        if "usb_hid_enabled_default" not in mpconfigboard:
            mpconfigboard["usb_hid_enabled_default"] = usb_num_endpoint_pairs >= 5
        if "usb_midi_enabled_default" not in mpconfigboard:
            mpconfigboard["usb_midi_enabled_default"] = usb_num_endpoint_pairs >= 8

        tinyusb_files.extend(
            (top / "lib/tinyusb/src/common/tusb_fifo.c", top / "lib/tinyusb/src/tusb.c")
        )
        supervisor_source.extend(
            (portdir / "supervisor/usb.c", top / "supervisor/shared/usb/usb.c")
        )

        tinyusb_files.extend(
            (
                top / "lib/tinyusb/src/device/usbd.c",
                top / "lib/tinyusb/src/device/usbd_control.c",
            )
        )
        supervisor_source.extend(
            (top / "supervisor/shared/usb/usb_desc.c", top / "supervisor/shared/usb/usb_device.c")
        )
    elif usb_num_endpoint_pairs > 0:
        module_reasons["usb_cdc"] = f"No TinyUSB settings for {soc}"

    circuitpython_flags.append(f"-DCIRCUITPY_PORT_SERIAL={0 if usb_ok else 1}")
    # ifeq ($(CIRCUITPY_USB_HID), 1)
    #   SRC_SUPERVISOR += \
    #     lib/tinyusb/src/class/hid/hid_device.c \
    #     shared-bindings/usb_hid/__init__.c \
    #     shared-bindings/usb_hid/Device.c \
    #     shared-module/usb_hid/__init__.c \
    #     shared-module/usb_hid/Device.c \

    # endif

    # ifeq ($(CIRCUITPY_USB_MIDI), 1)
    #   SRC_SUPERVISOR += \
    #     lib/tinyusb/src/class/midi/midi_device.c \
    #     shared-bindings/usb_midi/__init__.c \
    #     shared-bindings/usb_midi/PortIn.c \
    #     shared-bindings/usb_midi/PortOut.c \
    #     shared-module/usb_midi/__init__.c \
    #     shared-module/usb_midi/PortIn.c \
    #     shared-module/usb_midi/PortOut.c \

    # endif

    # ifeq ($(CIRCUITPY_USB_VIDEO), 1)
    #   SRC_SUPERVISOR += \
    #     shared-bindings/usb_video/__init__.c \
    #     shared-module/usb_video/__init__.c \
    #     shared-bindings/usb_video/USBFramebuffer.c \
    #     shared-module/usb_video/USBFramebuffer.c \
    #     lib/tinyusb/src/class/video/video_device.c \

    #   CFLAGS += -DCFG_TUD_VIDEO=1 -DCFG_TUD_VIDEO_STREAMING=1 -DCFG_TUD_VIDEO_STREAMING_EP_BUFSIZE=256 -DCFG_TUD_VIDEO_STREAMING_BULK=1
    # endif

    # ifeq ($(CIRCUITPY_USB_VENDOR), 1)
    #   SRC_SUPERVISOR += \
    #     lib/tinyusb/src/class/vendor/vendor_device.c \

    # endif

    # ifeq ($(CIRCUITPY_TINYUSB_HOST), 1)
    #   SRC_SUPERVISOR += \
    #     lib/tinyusb/src/host/hub.c \
    #     lib/tinyusb/src/host/usbh.c \

    # endif

    # ifeq ($(CIRCUITPY_USB_KEYBOARD_WORKFLOW), 1)
    #   SRC_SUPERVISOR += \
    #     lib/tinyusb/src/class/hid/hid_host.c \
    #     supervisor/shared/usb/host_keyboard.c \

    # endif

    if "ssl" in enabled_modules:
        # TODO: Figure out how to get these paths from zephyr
        circuitpython_flags.append('-DMBEDTLS_CONFIG_FILE=\\"config-tls-generic.h\\"')
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "tinycrypt" / "lib" / "include")
        )
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "mbedtls" / "include")
        )
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "mbedtls" / "configs")
        )
        circuitpython_flags.extend(
            ("-isystem", portdir / "modules" / "crypto" / "mbedtls" / "include")
        )
        circuitpython_flags.extend(("-isystem", zephyrdir / "modules" / "mbedtls" / "configs"))
        supervisor_source.append(top / "lib" / "mbedtls_config" / "crt_bundle.c")

    # Make sure all modules have a setting by filling in defaults.
    hal_source = []
    autogen_board_info = tomlkit.document()
    autogen_board_info.add(
        tomlkit.comment(
            "This file is autogenerated when a board is built. Do not edit. Do commit it to git. Other scripts use its info."
        )
    )
    autogen_board_info.add("name", board_info["vendor"] + " " + board_info["name"])
    autogen_modules = tomlkit.table()
    autogen_board_info.add("modules", autogen_modules)
    for module in sorted(
        list(top.glob("shared-bindings/*")) + list(portdir.glob("bindings/*")),
        key=lambda x: x.name,
    ):
        if not module.is_dir():
            continue
        enabled = module.name in enabled_modules
        # print(f"Module {module.name} enabled: {enabled}")
        v = tomlkit.item(enabled)
        if module.name in module_reasons:
            v.comment(module_reasons[module.name])
        autogen_modules.add(module.name, v)
        circuitpython_flags.append(f"-DCIRCUITPY_{module.name.upper()}={1 if enabled else 0}")

        if enabled:
            hal_source.extend(portdir.glob(f"bindings/{module.name}/*.c"))
            hal_source.extend(top.glob(f"ports/zephyr-cp/common-hal/{module.name}/*.c"))
            hal_source.extend(top.glob(f"shared-bindings/{module.name}/*.c"))
            hal_source.extend(top.glob(f"shared-module/{module.name}/*.c"))

    if os.environ.get("CI", "false") == "true":
        # Fail the build if it isn't up to date.
        if (
            not autogen_board_info_fn.exists()
            or autogen_board_info_fn.read_text() != tomlkit.dumps(autogen_board_info)
        ):
            logger.error("autogen_board_info.toml is out of date.")
            raise RuntimeError(
                f"autogen_board_info.toml is missing or out of date. Please run `make BOARD={board}` locally and commit {autogen_board_info_fn}."
            )
    elif autogen_board_info_fn.parent.exists():
        autogen_board_info_fn.write_text(tomlkit.dumps(autogen_board_info))

    for mpflag in MPCONFIG_FLAGS:
        enabled = mpflag in DEFAULT_MODULES
        circuitpython_flags.append(f"-DCIRCUITPY_{mpflag.upper()}={1 if enabled else 0}")

    source_files = supervisor_source + hal_source + ["extmod/vfs.c"]
    assembly_files = []
    for file in top.glob("py/*.c"):
        source_files.append(file)
    qstr_flags = "-DNO_QSTR"
    async with asyncio.TaskGroup() as tg:
        for source_file in source_files:
            tg.create_task(
                preprocess_and_split_defs(
                    compiler,
                    top / source_file,
                    builddir,
                    [qstr_flags, *circuitpython_flags, *port_flags],
                )
            )

        if "ssl" in enabled_modules:
            crt_bundle = builddir / "x509_crt_bundle.S"
            roots_pem = srcdir / "lib/certificates/data/roots-full.pem"
            generator = srcdir / "tools/gen_crt_bundle.py"
            tg.create_task(
                cpbuild.run_command(
                    [
                        "python",
                        generator,
                        "-i",
                        roots_pem,
                        "-o",
                        crt_bundle,
                        "--asm",
                    ],
                    srcdir,
                )
            )
            assembly_files.append(crt_bundle)

    async with asyncio.TaskGroup() as tg:
        board_build = builddir
        tg.create_task(
            generate_qstr_headers(
                board_build, compiler, [qstr_flags, *circuitpython_flags, *port_flags], translation
            )
        )
        tg.create_task(generate_module_header(board_build))
        tg.create_task(generate_root_pointer_header(board_build))

    # This file is generated by the QSTR/translation process.
    source_files.append(builddir / f"translations-{translation}.c")
    # These files don't include unique QSTRs. They just need to be compiled.
    source_files.append(portdir / "supervisor" / "flash.c")
    source_files.append(portdir / "supervisor" / "port.c")
    source_files.append(portdir / "supervisor" / "serial.c")
    source_files.append(srcdir / "lib" / "oofatfs" / "ff.c")
    source_files.append(srcdir / "lib" / "oofatfs" / "ffunicode.c")
    source_files.append(srcdir / "extmod" / "vfs_fat_diskio.c")
    source_files.append(srcdir / "shared/timeutils/timeutils.c")
    source_files.append(srcdir / "shared-module/time/__init__.c")
    source_files.append(srcdir / "shared-module/os/__init__.c")
    source_files.append(srcdir / "shared-module/supervisor/__init__.c")
    source_files.append(portdir / "bindings/zephyr_kernel/__init__.c")
    source_files.append(portdir / "common-hal/zephyr_kernel/__init__.c")
    # source_files.append(srcdir / "ports" / port / "peripherals" / "nrf" / "nrf52840" / "pins.c")

    assembly_files.append(srcdir / "supervisor/shared/cpu_regs.S")

    source_files.extend(assembly_files)

    source_files.extend(tinyusb_files)

    objects = []
    async with asyncio.TaskGroup() as tg:
        for source_file in source_files:
            source_file = top / source_file
            build_file = source_file.with_suffix(".o")
            object_file = builddir / (build_file.relative_to(top))
            objects.append(object_file)
            tg.create_task(
                compiler.compile(source_file, object_file, [*circuitpython_flags, *port_flags])
            )

    await compiler.archive(objects, pathlib.Path(cmake_args["OUTPUT_FILE"]))


async def main():
    try:
        await build_circuitpython()
    except* RuntimeError as e:
        logger.error(e)
        sys.exit(len(e.exceptions))


handler = colorlog.StreamHandler()
handler.setFormatter(colorlog.ColoredFormatter("%(log_color)s%(levelname)s:%(name)s:%(message)s"))

logging.basicConfig(level=logging.INFO, handlers=[handler])

asyncio.run(main())
