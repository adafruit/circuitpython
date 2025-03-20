# SPDX-FileCopyrightText: 2025 Tim Cocks for Adafruit Industries
# SPDX-License-Identifier: MIT
import terminalio
import array
import usb.core
import supervisor
from displayio import Group, OnDiskBitmap, TileGrid
from adafruit_display_text.bitmap_label import Label

display = supervisor.runtime.display

main_group = Group()
display.root_group = main_group

mouse_bmp = OnDiskBitmap("mouse_cursor.bmp")
mouse_bmp.pixel_shader.make_transparent(0)
mouse_tg = TileGrid(mouse_bmp, pixel_shader=mouse_bmp.pixel_shader)
mouse_tg.x = display.width // 2
mouse_tg.y = display.height // 2

output_lbl = Label(terminalio.FONT, text=f"{mouse_tg.x},{mouse_tg.y}", color=0xFFFFFF, scale=1)
output_lbl.anchor_point = (0, 0)
output_lbl.anchored_position = (1, 1)
main_group.append(output_lbl)

main_group.append(mouse_tg)

# This is a basic Microsoft optical mouse with two buttons and a wheel that can
# also be pressed.
USB_VID = 0x046D
USB_PID = 0xC52F
# This is ordered by bit position.
BUTTONS = ["left", "right", "middle"]

for device in usb.core.find(find_all=True):
    print(f"{device.idVendor:04x}:{device.idProduct:04x}")
    print(device.manufacturer, device.product)
    print(device.serial_number)
    if device.idVendor == USB_VID and device.idProduct == USB_PID:
        mouse = device
#
print(mouse.manufacturer, mouse.product)

if mouse.is_kernel_driver_active(0):
    mouse.detach_kernel_driver(0)

mouse.set_configuration()

# Boot mice have 4 byte reports
buf = array.array("b", [0] * 4)
report_count = 0

# try:
while True:
    try:
        count = mouse.read(0x81, buf, timeout=10)
    except usb.core.USBTimeoutError:
        continue

    mouse_tg.x = max(0, min(display.width - 1, mouse_tg.x + buf[1]))
    mouse_tg.y = max(0, min(display.height - 1, mouse_tg.y + buf[2]))
    out_str = f"{mouse_tg.x},{mouse_tg.y}"
    for i, button in enumerate(BUTTONS):
        if buf[0] & (1 << i) != 0:
            out_str += f" {button}"

    output_lbl.text = out_str
