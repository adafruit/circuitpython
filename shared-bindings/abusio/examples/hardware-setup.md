# Hardware Debug Setup — Raspberry Pi Pico2 W

Reference for the physical test bench wiring and tool configurations used for
SPI phase/polarity testing and SWD debugging.

---

## Logic Analyzer — fx2lafw

**Device:** fx2lafw-compatible USB logic analyzer (e.g. generic 8-channel clone)

### Wiring (Pico2W → fx2lafw)

| Pico2W Pin | SPI Role | fx2lafw Channel |
|------------|----------|-----------------|
| GP1        | CS       | D6              |
| GP2        | SCK      | D5              |
| GP3        | MOSI     | D3              |
| GP4        | MISO     | D1              |

> Don't forget a shared GND between the Pico and the logic analyzer.

### Capture

Use the helper script for sigrok-cli SPI captures:

```sh
.vscode/sigrok-spi.sh --help
.vscode/sigrok-spi.sh                        # SPI mode 0 (CPOL=0, CPHA=0)
.vscode/sigrok-spi.sh --cpol 1 --cpha 1     # SPI mode 3
```

See `.vscode/sigrok-spi.sh` for full usage and all mode options.

### Install Notes

```sh
# Arch Linux
sudo pacman -S sigrok-cli

# Debian/Ubuntu
sudo apt install sigrok-cli

# Allow non-root access to fx2lafw
sudo cp .vscode/99-fx2lafw.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
# Then add yourself to the plugdev group if not already a member:
sudo usermod -aG plugdev "$USER"
# Log out and back in for the group change to take effect.
```

---

## SWD Debugger — Particle Debugger (CMSIS-DAP)

**Adapter:** Particle Debugger (CMSIS-DAP, USB VID `2b04` PID `c00e`)
**Target:** Raspberry Pi Pico2 W (RP2350)
**Protocol:** SWD via OpenOCD

### VS Code Launch

Open the **Run & Debug** panel and select **"Pico2W Debug (Cortex-Debug)"**.
This runs the `Build Pico2W Debug` task first, then launches OpenOCD and attaches GDB.

Config files:
- `.vscode/launch.json` — Cortex-Debug launch configuration
- `.vscode/openocd-pico2w.cfg` — OpenOCD interface + target config

### Wiring (Pico2W SWD header → Particle Debugger)

| Pico2W SWD  | Particle Debugger |
|-------------|-------------------|
| SWDIO       | D0 / SWD IO       |
| SWDCLK      | D1 / SWD CLK      |
| GND         | GND               |
| 3V3 (power) | 3V3 (optional)    |

### Install Notes

```sh
# openocd-raspberrypi-git is required for RP2350 support.
# Standard openocd 0.12.0 on Arch does NOT include rp2350.cfg.
yay -S openocd-raspberrypi-git arm-none-eabi-gdb
# or:
paru -S openocd-raspberrypi-git arm-none-eabi-gdb

# VS Code extension
code --install-extension marus25.cortex-debug

# Allow non-root access to the Particle Debugger
sudo cp .vscode/99-particle-debugger.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
```
