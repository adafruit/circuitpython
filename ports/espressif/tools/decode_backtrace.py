"""Simple script that translates "Backtrace:" lines from the ESP output to files
and line numbers.

Run with: python3 tools/decode_backtrace.py <board>

Enter the backtrace line at the "? " prompt. CTRL-C to exit the script.
"""

import subprocess
import sys

board = sys.argv[1]
print(board)

elfs = [
    f"build-{board}/firmware.elf",
    # Add additional ELF files here such as the ROM ELF files from:
    # https://github.com/espressif/esp-rom-elfs/releases
    # "/home/tannewt/Downloads/esp-rom-elfs-20241011/esp32c6_rev0_rom.elf",
]

while True:
    print('"Backtrace:" or "Stack memory:". CTRL-D to finish multiline paste')
    addresses = input("? ")
    if addresses.startswith("Backtrace:"):
        addresses = addresses[len("Backtrace:") :]
        addresses = addresses.strip().split()
        addresses = [address.split(":")[0] for address in addresses]
    elif addresses.startswith("Stack memory:"):
        addresses = []
        extra_lines = sys.stdin.readlines()
        for line in extra_lines:
            if not line.strip():
                continue
            addresses.extend(line.split(":")[1].strip().split())
    for address in addresses:
        if address == "0xa5a5a5a5":
            # Skip stack fill value.
            continue
        for elf in elfs:
            result = subprocess.run(
                ["riscv32-esp-elf-addr2line", "-aipfe", elf, address],
                capture_output=True,
            )
            stdout = result.stdout.decode("utf-8")
            if not stdout:
                continue
            if "?? ??" not in stdout:
                print(stdout.strip())
                break

    print("loop")
