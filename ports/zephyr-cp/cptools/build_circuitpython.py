import asyncio
import logging
import os
import pathlib
import pickle
import sys

import board_tools
import colorlog
import cpbuild
import tomlkit
import tomllib
import yaml

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
    "rainbowio",
    "traceback",
    "warnings",
    "supervisor",
    "errno",
    "io",
]
# Flags that don't match with with a *bindings module. Some used by adafruit_requests
MPCONFIG_FLAGS = ["array", "errno", "io", "json"]

# List of other modules (the value) that can be enabled when another one (the key) is.
REVERSE_DEPENDENCIES = {
    "busio": ["fourwire", "i2cdisplaybus", "sdcardio", "sharpdisplay"],
    "fourwire": ["displayio", "busdisplay", "epaperdisplay"],
    "i2cdisplaybus": ["displayio", "busdisplay", "epaperdisplay"],
    "displayio": [
        "vectorio",
        "bitmapfilter",
        "bitmaptools",
        "terminalio",
        "lvfontio",
        "tilepalettemapper",
        "fontio",
    ],
    "sharpdisplay": ["framebufferio"],
    "framebufferio": ["displayio"],
}

# Other flags to set when a module is enabled
EXTRA_FLAGS = {"busio": ["BUSIO_SPI", "BUSIO_I2C"]}

SHARED_MODULE_AND_COMMON_HAL = ["os"]


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
    to_collect = list(splitdir.glob(f"**/*.{mode}"))
    batch_size = 50
    await cpbuild.run_command(
        ["cat", "-s", *to_collect[:batch_size], ">", output_file],
        splitdir,
    )
    for i in range(0, len(to_collect), batch_size):
        await cpbuild.run_command(
            ["cat", "-s", *to_collect[i : i + batch_size], ">>", output_file],
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


async def generate_display_resources(output_path, translation, font, extra_characters):
    await cpbuild.run_command(
        [
            "python",
            srcdir / "tools" / "gen_display_resources.py",
            "--font",
            srcdir / font,
            "--sample_file",
            srcdir / "locale" / f"{translation}.po",
            "--extra_characters",
            repr(extra_characters),
            "--output_c_file",
            output_path,
        ],
        srcdir,
        check_hash=[output_path],
    )


def determine_enabled_modules(board_info, portdir, srcdir):
    """Determine which CircuitPython modules should be enabled based on board capabilities.

    Args:
        board_info: Dictionary containing board hardware capabilities
        portdir: Path to the port directory (ports/zephyr-cp)
        srcdir: Path to the CircuitPython source root

    Returns:
        tuple: (enabled_modules set, module_reasons dict)
    """
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

    for port_module in (portdir / "bindings").iterdir():
        if not board_info.get(port_module.name, False):
            continue
        enabled_modules.add(port_module.name)
        module_reasons[port_module.name] = f"Zephyr board has {port_module.name}"

    for shared_module in (srcdir / "shared-bindings").iterdir():
        if not board_info.get(shared_module.name, False) or not shared_module.glob("*.c"):
            continue
        enabled_modules.add(shared_module.name)
        module_reasons[shared_module.name] = f"Zephyr board has {shared_module.name}"

        more_modules = []
        more_modules.extend(REVERSE_DEPENDENCIES.get(shared_module.name, []))
        while more_modules:
            reverse_dependency = more_modules.pop(0)
            if reverse_dependency in enabled_modules:
                continue
            logger.debug(f"Enabling {reverse_dependency} because {shared_module.name} is enabled")
            enabled_modules.add(reverse_dependency)
            more_modules.extend(REVERSE_DEPENDENCIES.get(reverse_dependency, []))
            module_reasons[reverse_dependency] = f"Zephyr board has {shared_module.name}"

    return enabled_modules, module_reasons


async def build_circuitpython():
    circuitpython_flags = ["-DCIRCUITPY"]
    port_flags = []
    enable_mpy_native = False
    full_build = True
    usb_host = False
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
    circuitpython_flags.append(f"-DCIRCUITPY_BOARD_ID='\"{board}\"'")
    circuitpython_flags.append(f"-DCIRCUITPY_TRANSLATE_OBJECT={1 if lto else 0}")
    circuitpython_flags.append("-DINTERNAL_FLASH_FILESYSTEM")
    circuitpython_flags.append("-DLONGINT_IMPL_MPZ")
    circuitpython_flags.append("-DCIRCUITPY_SSL_MBEDTLS")
    circuitpython_flags.append("-DFFCONF_H='\"lib/oofatfs/ffconf.h\"'")
    circuitpython_flags.extend(("-I", srcdir))
    circuitpython_flags.extend(("-I", builddir))
    circuitpython_flags.extend(("-I", portdir))

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

        board_autogen_task = tg.create_task(
            zephyr_dts_to_cp_board(portdir, builddir, zephyrbuilddir)
        )
    board_info = board_autogen_task.result()
    mpconfigboard_fn = board_tools.find_mpconfigboard(portdir, board)
    mpconfigboard = {"USB_VID": 0x1209, "USB_PID": 0x000C, "USB_INTERFACE_NAME": "CircuitPython"}
    if mpconfigboard_fn is None:
        mpconfigboard_fn = (
            portdir / "boards" / board_info["vendor_id"] / board / "circuitpython.toml"
        )
        logging.warning(
            f"Could not find board config at: boards/{board_info['vendor_id']}/{board}"
        )
    elif mpconfigboard_fn.exists():
        with mpconfigboard_fn.open("rb") as f:
            mpconfigboard.update(tomllib.load(f))

    autogen_board_info_fn = mpconfigboard_fn.parent / "autogen_board_info.toml"

    enabled_modules, module_reasons = determine_enabled_modules(board_info, portdir, srcdir)

    circuitpython_flags.extend(board_info["cflags"])
    supervisor_source = [
        "main.c",
        "extmod/modjson.c",
        "extmod/vfs_fat.c",
        "lib/tlsf/tlsf.c",
        portdir / "background.c",
        portdir / "common-hal/microcontroller/__init__.c",
        portdir / "common-hal/microcontroller/Pin.c",
        portdir / "common-hal/microcontroller/Processor.c",
        portdir / "common-hal/os/__init__.c",
        "shared/readline/readline.c",
        "shared/runtime/buffer_helper.c",
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

    usb_ok = board_info.get("usb_device", False)
    circuitpython_flags.append(f"-DCIRCUITPY_USB_DEVICE={1 if usb_ok else 0}")

    if usb_ok:
        enabled_modules.add("usb_cdc")

        for macro in ("USB_PID", "USB_VID"):
            print(f"Setting {macro} to {mpconfigboard.get(macro)}")
            circuitpython_flags.append(f"-D{macro}=0x{mpconfigboard.get(macro):04x}")
        circuitpython_flags.append(
            f"-DUSB_INTERFACE_NAME='\"{mpconfigboard['USB_INTERFACE_NAME']}\"'"
        )
        for macro, limit, value in (
            ("USB_PRODUCT", 16, board_info["name"]),
            ("USB_MANUFACTURER", 8, board_info["vendor"]),
        ):
            circuitpython_flags.append(f"-D{macro}='\"{value}\"'")
            circuitpython_flags.append(f"-D{macro}_{limit}='\"{value[:limit]}\"'")

        circuitpython_flags.append("-DCIRCUITPY_USB_CDC_CONSOLE_ENABLED_DEFAULT=1")
        circuitpython_flags.append("-DCIRCUITPY_USB_CDC_DATA_ENABLED_DEFAULT=0")

        supervisor_source.extend(
            (portdir / "supervisor/usb.c", srcdir / "supervisor/shared/usb.c")
        )

    # Always use port serial. It'll switch between USB and UART automatically.
    circuitpython_flags.append("-DCIRCUITPY_PORT_SERIAL=1")

    if "ssl" in enabled_modules:
        # TODO: Figure out how to get these paths from zephyr
        circuitpython_flags.append("-DMBEDTLS_CONFIG_FILE='\"config-mbedtls.h\"'")
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
        # Skip files and directories without C source files (like artifacts from a docs build)
        if not module.is_dir() or len(list(module.glob("*.c"))) == 0:
            continue
        enabled = module.name in enabled_modules
        # print(f"Module {module.name} enabled: {enabled}")
        v = tomlkit.item(enabled)
        if module.name in module_reasons:
            v.comment(module_reasons[module.name])
        autogen_modules.add(module.name, v)
        circuitpython_flags.append(f"-DCIRCUITPY_{module.name.upper()}={1 if enabled else 0}")

        if enabled:
            if module.name in EXTRA_FLAGS:
                for flag in EXTRA_FLAGS[module.name]:
                    circuitpython_flags.append(f"-DCIRCUITPY_{flag}=1")

        if enabled:
            hal_source.extend(portdir.glob(f"bindings/{module.name}/*.c"))
            len_before = len(hal_source)
            hal_source.extend(top.glob(f"ports/zephyr-cp/common-hal/{module.name}/*.c"))
            # Only include shared-module/*.c if no common-hal/*.c files were found
            if len(hal_source) == len_before or module.name in SHARED_MODULE_AND_COMMON_HAL:
                hal_source.extend(top.glob(f"shared-module/{module.name}/*.c"))
            hal_source.extend(top.glob(f"shared-bindings/{module.name}/*.c"))

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

        if "terminalio" in enabled_modules:
            output_path = board_build / f"autogen_display_resources-{translation}.c"
            font_path = srcdir / mpconfigboard.get(
                "CIRCUITPY_DISPLAY_FONT", "tools/fonts/ter-u12n.bdf"
            )
            extra_characters = mpconfigboard.get("CIRCUITPY_FONT_EXTRA_CHARACTERS", "")
            tg.create_task(
                generate_display_resources(output_path, translation, font_path, extra_characters)
            )
            source_files.append(output_path)

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
