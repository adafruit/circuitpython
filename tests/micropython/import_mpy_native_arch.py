# test that a build which can load native code imports foo.<arch>.mpy
# after foo.py and before foo.mpy

try:
    import sys, os

    sys.implementation._mpy
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

mpy_arch = sys.implementation._mpy >> 10
if mpy_arch == 0:
    # This system does not support .mpy files containing native code
    print("SKIP")
    raise SystemExit

# mpy-cross -march names, indexed by the arch field of the .mpy header
arch = [
    None,
    "x86",
    "x64",
    "armv6",
    "armv6m",
    "armv7m",
    "armv7em",
    "armv7emsp",
    "armv7emdp",
    "xtensa",
    "xtensawin",
    "rv32imc",
    "rv64imc",
][mpy_arch & 0x0F]

# a bytecode-only .mpy of an empty module, as written by mpy-cross; it loads
# on any architecture, so only the file name decides which one is imported
empty_mpy = b"\x43\x06\x00\x1f\x02\x00\x10empty.py\x00\x0f\x28\x00\x02\x01\x51\x63"

files = {
    "foo." + arch + ".mpy": empty_mpy,
    "foo.mpy": empty_mpy,
    "bar.mpy": empty_mpy,
    "baz." + arch + ".mpy": empty_mpy,
    "baz.py": b"",
}

try:
    for name, data in files.items():
        with open(name, "wb") as f:
            f.write(data)

    sys.path.insert(0, ".")
    for name in ("foo", "bar", "baz"):
        mod = __import__(name)
        print(name, mod.__file__.replace(arch, "<arch>"))
finally:
    for name in files:
        os.remove(name)
