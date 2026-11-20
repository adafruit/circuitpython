# SPDX-FileCopyrightText: Copyright (c) 2026
# SPDX-License-Identifier: MIT

"""Native display/owner lifetime tests for port-heap allocated objects."""

import pytest


SHARED_BUS_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import gc
import supervisor
import time
from microcontroller import pin

displayio.release_displays()
assert supervisor.runtime.display is None
try:
    board.DISPLAY.width
except (AttributeError, ValueError):
    pass
else:
    raise AssertionError('board display still live')

bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11, reset=pin.P_12)
displays = [busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    for _ in range(4)]
assert all(display.bus is bus for display in displays)
assert supervisor.runtime.display is displays[0]
assert all(type(display) is busdisplay.BusDisplay for display in displays)

# Repeatedly drop secondary references. LVGL draw buffers make a finalization
# failure exhaust the native simulator's port heap quickly.
for i in range(200):
    temporary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    if i % 4 == 0:
        gc.collect()
del temporary
gc.collect()

class CustomDisplay(busdisplay.BusDisplay):
    pass

custom = CustomDisplay(bus, b'', width=64, height=64, auto_refresh=False)
assert isinstance(custom, busdisplay.BusDisplay)
assert custom.bus is bus
supervisor.runtime.display = custom
assert supervisor.runtime.display is custom

# Proxy fields keep the current root group alive even without another reference.
group = displayio.Group()
custom.root_group = group
del group
gc.collect()
assert custom.root_group is not None
custom.root_group = None

# Removing one display does not release a shared owner.
displays[0].deinit()
displays[0].deinit()
bus.reset()
assert displays[1].width == 64
try:
    displays[0].refresh()
    raise AssertionError('released display used')
except ValueError:
    pass

# Owner-first release invalidates every attached display and is idempotent.
bus.deinit()
bus.deinit()
for display in displays[1:] + [custom]:
    display.deinit()
    try:
        _ = display.width
        raise AssertionError('released display attribute used')
    except ValueError:
        pass

print('shared bus lifetime ok')
while True:
    time.sleep(1)
"""


RELEASE_PRIMARY_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import supervisor
import time
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
primary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
secondary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
assert supervisor.runtime.display is primary

displayio.release_displays()
displayio.release_displays()
assert supervisor.runtime.display is None
for display in (primary, secondary):
    display.deinit()
    try:
        display.refresh()
        raise AssertionError('released shared display used')
    except ValueError:
        pass
bus.deinit()
print('primary release ok')
while True:
    time.sleep(1)
"""


UNRELATED_OWNER_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import time
from microcontroller import pin

displayio.release_displays()
primary_bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
other_bus = fourwire.FourWire(board.SPI(), command=pin.P_20, chip_select=pin.P_21)
unattached = fourwire.FourWire(board.SPI(), command=None, chip_select=None)
primary = busdisplay.BusDisplay(primary_bus, b'', width=64, height=64, auto_refresh=False)
other = busdisplay.BusDisplay(other_bus, b'', width=64, height=64, auto_refresh=False)
displayio.release_displays()
assert other.width == 64
assert other.bus is other_bus
unattached.send(0x00, b'')
primary.deinit()
other.deinit()
other_bus.deinit()
unattached.deinit()
print('unrelated owner ok')
while True:
    time.sleep(1)
"""


SOFT_RELOAD_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import supervisor
import time
from microcontroller import pin

try:
    with open('display-run.txt', 'r') as f:
        run = int(f.read()) + 1
except OSError:
    run = 0
with open('display-run.txt', 'w') as f:
    f.write(str(run))

if run == 0:
    displayio.release_displays()
    bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
    primary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    secondary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    assert supervisor.runtime.display is primary
    assert secondary.bus is bus
    print('reload first')
else:
    # The native primary survives, but its old VM proxy does not.
    primary = supervisor.runtime.display
    assert primary is not None
    assert primary.width == 64
    bus = primary.bus
    secondary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    assert secondary.bus is bus
    primary.deinit()
    assert secondary.width == 64
    secondary.deinit()
    bus.deinit()
    print('soft reload lifetime ok')
time.sleep(1)
print('done')
"""


BOARD_AND_PRIMARY_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import supervisor
import time
from microcontroller import pin

try:
    with open('display-run.txt', 'r') as f:
        run = int(f.read()) + 1
except OSError:
    run = 0
with open('display-run.txt', 'w') as f:
    f.write(str(run))

builtin = board.DISPLAY
assert builtin is not None
if run == 0:
    runtime = supervisor.runtime.display
    assert runtime is not None
    assert runtime.width == builtin.width
    builtin.root_group = displayio.Group()
    bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
    primary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    supervisor.runtime.display = primary
    print('two displays before reload')
elif run == 1:
    primary = supervisor.runtime.display
    assert primary is not builtin
    assert primary.width == 64
    assert builtin.width > 0
    builtin.root_group = displayio.Group()
    supervisor.runtime.display = None
    print('primary cleared before reload')
else:
    assert supervisor.runtime.display is None
    assert builtin.width > 0
    builtin.auto_refresh = False
    builtin.root_group = displayio.Group()
    builtin.refresh()
    displayio.release_displays()
    assert supervisor.runtime.display is None
    try:
        board.DISPLAY.width
    except (AttributeError, ValueError):
        pass
    else:
        raise AssertionError('board display still live')
    print('built-in survived both reloads')
time.sleep(1)
"""


EPAPER_SHARED_BUS_CODE = """\
import board
import busdisplay
import displayio
import epaperdisplay
import fourwire
import time
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
primary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
secondary = epaperdisplay.EPaperDisplay(bus, b'', b'', width=16, height=16,
    ram_width=16, ram_height=16, write_black_ram_command=0x24,
    refresh_display_command=0x20, refresh_time=0.01, seconds_per_frame=0)
assert secondary.bus is bus
secondary.root_group = displayio.Group()
secondary.refresh()
displayio.release_displays()
for display in (primary, secondary):
    try:
        _ = display.width
        raise AssertionError('display still live')
    except ValueError:
        pass
print('shared e-paper display released')
while True:
    time.sleep(1)
"""


FAILED_CONSTRUCTION_CODE = """\
import board
import busdisplay
import displayio
import fourwire
import gc
import supervisor
import time
from microcontroller import pin

displayio.release_displays()
bus = fourwire.FourWire(board.SPI(), command=pin.P_10, chip_select=pin.P_11)
for _ in range(16):
    try:
        busdisplay.BusDisplay(bus, b'', width=64, height=64, color_depth=1)
        raise AssertionError('LVGL accepted an unsupported depth')
    except NotImplementedError:
        pass
    assert supervisor.runtime.display is None
    gc.collect()
display = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
assert supervisor.runtime.display is display
assert display.bus is bus
print('failed construction left bus usable')
while True:
    time.sleep(1)
"""


PRIMARY_MOVEABLE_CODE = """\
import busdisplay
import busio
import displayio
import fourwire
import gc
import supervisor
import time
from microcontroller import pin

try:
    with open('moveable-run.txt', 'r') as f:
        run = int(f.read()) + 1
except OSError:
    run = 0
with open('moveable-run.txt', 'w') as f:
    f.write(str(run))

if run == 0:
    displayio.release_displays()
    spi = busio.SPI(clock=pin.P_00, MOSI=pin.P_01, MISO=pin.P_02)
    bus = fourwire.FourWire(spi, command=pin.P_10, chip_select=pin.P_11)
    primary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    secondary = busdisplay.BusDisplay(bus, b'', width=64, height=64, auto_refresh=False)
    assert supervisor.runtime.display is primary

    assert spi.try_lock()
    try:
        supervisor.runtime.display = secondary
        raise AssertionError('locked bus accepted as primary')
    except ValueError:
        pass
    assert supervisor.runtime.display is primary
    spi.unlock()
    supervisor.runtime.display = secondary
    assert supervisor.runtime.display is secondary
    print('primary moveability checked')
else:
    primary = supervisor.runtime.display
    assert primary is not None
    assert primary.width == 64
    primary.bus.send(0x00, b'')
    primary.deinit()
    gc.collect()
    # Reclaiming the restored heap bus releases the routed peripheral and pins.
    spi = busio.SPI(clock=pin.P_00, MOSI=pin.P_01, MISO=pin.P_02)
    spi.deinit()
    print('dynamic bus reload ok')
time.sleep(1)
print('done')
"""


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": SHARED_BUS_CODE})
@pytest.mark.duration(15)
def test_shared_bus_proxy_lifetime(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "shared bus lifetime ok" in output
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": PRIMARY_MOVEABLE_CODE})
@pytest.mark.port_resets(3)
@pytest.mark.duration(20)
def test_primary_rejects_unmoveable_resources(circuitpython):
    circuitpython.serial.wait_for("primary moveability checked")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("dynamic bus reload ok")
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert output.count("primary moveability checked") == 1
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": RELEASE_PRIMARY_CODE})
@pytest.mark.duration(15)
def test_primary_release_includes_shared_bus_displays(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "primary release ok" in output
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": SOFT_RELOAD_CODE})
@pytest.mark.port_resets(3)
@pytest.mark.duration(15)
def test_primary_proxy_and_owner_survive_soft_reload(circuitpython):
    circuitpython.serial.wait_for("reload first")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("soft reload lifetime ok")
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert output.count("reload first") == 1
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": UNRELATED_OWNER_CODE})
@pytest.mark.duration(15)
def test_primary_release_leaves_unrelated_owner(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "unrelated owner ok" in output
    assert "Traceback" not in output


@pytest.mark.parametrize(
    "board",
    ["native_native_sim", "native_native_sim_asan", "native_native_sim_lvgl_asan"],
    indirect=True,
)
@pytest.mark.circuitpy_drive({"code.py": BOARD_AND_PRIMARY_CODE})
@pytest.mark.port_resets(4)
@pytest.mark.duration(20)
def test_builtin_survives_with_different_or_no_primary(circuitpython):
    circuitpython.serial.wait_for("two displays before reload")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("primary cleared before reload")
    circuitpython.serial.write("\x04")
    circuitpython.serial.wait_for("built-in survived both reloads")
    circuitpython.wait_until_done()
    assert "Traceback" not in circuitpython.serial.all_output


@pytest.mark.parametrize("board", ["native_native_sim_lvgl_asan"], indirect=True)
@pytest.mark.circuitpy_drive({"code.py": FAILED_CONSTRUCTION_CODE})
@pytest.mark.duration(15)
def test_failed_constructor_leaves_no_display_and_keeps_bus(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "failed construction left bus usable" in output
    assert "Traceback" not in output


@pytest.mark.parametrize("board", ["native_native_sim_asan"], indirect=True)
@pytest.mark.circuitpy_drive({"code.py": EPAPER_SHARED_BUS_CODE})
@pytest.mark.duration(15)
def test_release_includes_epaper_on_primary_bus(circuitpython):
    circuitpython.wait_until_done()
    output = circuitpython.serial.all_output
    assert "shared e-paper display released" in output
    assert "Traceback" not in output
