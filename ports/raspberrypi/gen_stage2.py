import math
import sys
import cascadetoml
import pathlib
import typer
from jinja2 import Template


def main(input_template: pathlib.Path, output_path: pathlib.Path, skus: str = typer.Argument("")):
    skus = [sku.strip() for sku in skus.split(",") if sku.strip()]
    if not skus:
        print("Set EXTERNAL_FLASH_DEVICES in mpconfigboard.mk with all possible flash skus")
        raise typer.Exit(code=1)

    flashes = cascadetoml.filter_toml(
        pathlib.Path("../../data/nvm.toml"), ['sku="{}"'.format(sku) for sku in skus]
    )

    # Stop if a listed device is not in data/nvm.toml. Otherwise it would be
    # left out silently, and its settings and read timing would not be used.
    found = {nvm["sku"] for nvm in flashes["nvm"]}
    missing = [sku for sku in skus if sku not in found]
    if missing:
        print("EXTERNAL_FLASH_DEVICES lists devices not in data/nvm.toml:", ", ".join(missing))
        raise typer.Exit(code=1)

    def all_have(nvms, key):
        for nvm in nvms:
            if not nvm.get(key, False):
                return False
        return True

    def all_match(nvms, key, default=None):
        shared_value = nvms[0].get(key, default)
        for nvm in nvms:
            this_value = nvm.get(key, default)
            if this_value != shared_value:
                print(
                    "{}.{} = {} does not match {}".format(
                        nvm["sku"], key, this_value, shared_value
                    )
                )
                return None
        return shared_value

    quad_enable_status_byte = all_match(flashes["nvm"], "quad_enable_status_byte", None)
    quad_enable_bit_mask = all_match(flashes["nvm"], "quad_enable_bit_mask")
    continuous_status_write = all_have(flashes["nvm"], "01_continuous_status_write")
    split_status_write = all_have(flashes["nvm"], "write_status_register_split")

    quad_ok = quad_enable_status_byte is not None and quad_enable_bit_mask is not None

    default_power_of_two = None
    for nvm in flashes["nvm"]:
        capacity = nvm.get("capacity", 0)
        if not 21 <= capacity < 30:
            power_of_two = int(math.log2(nvm["total_size"]))
            if not default_power_of_two:
                default_power_of_two = power_of_two
            else:
                default_power_of_two = min(power_of_two, default_power_of_two)
    if not default_power_of_two:
        default_power_of_two = 21

    # Check that we have a consistent way to set quad enable.
    if continuous_status_write is None and split_status_write is None:
        print("quad not ok", continuous_status_write, split_status_write)
        quad_ok = False

    clock_divider = 4

    # RP2350 stage 2 runs after every flash erase or program, at whatever the
    # system clock is then. In quad mode it makes one read to enter
    # continuous-read mode. Its divider and sample delay must be safe at any
    # system clock. Divider 8 with a sample delay of 6 (set in the template)
    # meets the setup and hold times of flash with an output valid time of 8 ns
    # or less, up to 532 MHz. CircuitPython sets the real timing for the system
    # clock afterward.
    rp2350_clock_divider = 8

    # Stage 2 always uses 0xeb for quad reads, because the read timing below is
    # rated for 0xeb. Datasheets often give no separate rating for 0xe7.
    read_command = 0xEB if quad_ok else 0x03
    wait_cycles = 4 if quad_ok else 0

    # Read timing for choosing the flash clock divider, and on RP2350 the sample delay
    # (mcu_processor_update_flash_timing() in common-hal/microcontroller/Processor.c).
    # Use the worst case across all the listed devices, because any of them may
    # be fitted. A device without a value uses the defaults passed to worst()
    # below: 75 MHz, an output valid time of 8 ns and an output hold time of 0.
    #
    # A device with eb_quad_read_max_clock_speed_mhz = 0 has no 0xeb read, so
    # stage 2 uses the plain 0x03 read. All the devices in data/nvm.toml
    # support 0x03 at 33 MHz or more.
    def worst(key, default, pick):
        values = []
        for nvm in flashes["nvm"]:
            value = nvm.get(key, default)
            if key == "eb_quad_read_max_clock_speed_mhz" and value == 0:
                value = 33
            values.append(value)
        return pick(values)

    eb_max_khz = int(worst("eb_quad_read_max_clock_speed_mhz", 75, min) * 1000)
    max_output_valid_ps = int(worst("max_output_valid_ns", 8, max) * 1000)
    min_output_hold_ps = int(worst("min_output_hold_ns", 0, min) * 1000)

    flash_settings = {
        "quad_ok": quad_ok,
        "quad_enable_status_byte": quad_enable_status_byte,
        "quad_enable_bit_mask": quad_enable_bit_mask,
        "split_status_write": split_status_write,
        "clock_divider": clock_divider,
        "rp2350_clock_divider": rp2350_clock_divider,
        "read_command": read_command,
        "wait_cycles": wait_cycles,
        "default_power_of_two": default_power_of_two,
        "eb_max_khz": eb_max_khz,
        "max_output_valid_ps": max_output_valid_ps,
        "min_output_hold_ps": min_output_hold_ps,
    }

    template = Template(input_template.read_text())

    output_path.write_text(template.render(flash_settings))


if __name__ == "__main__":
    typer.run(main)
