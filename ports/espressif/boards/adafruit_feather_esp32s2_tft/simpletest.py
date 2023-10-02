# SPDX-FileCopyrightText: 2023 Tim C
#
# SPDX-License-Identifier: MIT
"""
Simpletest for the Feather ESP32-S2 TFT.

Send an HTTP GET request to Adafruit WIFI test file and show
the response (or an error) on the display. Blink the on-board
standard LED while the on-board button is not pressed, turn on
the LED while the on-board button is pressed. Cycle the on-board
neopixel through a rainbow of colors.

You must set CIRCUITPY_WIFI_SSID and CIRCUITPY_WIFI_PASSWORD
inside your settings.toml file for the request to be able to
be successful, but the error message will display even if
you have not.
"""
import ssl
import time
import board
import terminalio
import wifi
import socketpool
from digitalio import DigitalInOut, Direction, Pull
import neopixel
import rainbowio
import adafruit_requests as requests
from displayio import Group
from adafruit_display_text.bitmap_label import Label
from adafruit_display_text import wrap_text_to_pixels

# Variables
TEXT_URL = "http://wifitest.adafruit.com/testwifi/index.html"

PLAIN_LED_BLINK_DELAY = 0.3
NEOPIXEL_CHANGE_DELAY = 0.05

last_led_blink_time = 0
last_neopixel_change_time = 0
cur_color_number = 0

# requests setup
socket = socketpool.SocketPool(wifi.radio)
https = requests.Session(socket, ssl.create_default_context())

# Plain LED setup
led = DigitalInOut(board.LED)
led.direction = Direction.OUTPUT

# Button setup
button = DigitalInOut(board.BUTTON)
button.direction = Direction.INPUT
button.pull = Pull.UP

# Neopixel setup
rgb_pixel = neopixel.NeoPixel(board.NEOPIXEL, 1, brightness=0.05)

# Displayio objects setup
main_group = Group()
output_label = Label(terminalio.FONT, text="", color=0xFFFFFF)
output_label.anchor_point = (0.5, 0.0)
output_label.anchored_position = (board.DISPLAY.width // 2, 6)
main_group.append(output_label)

# set the root_group to show our objects on the display
board.DISPLAY.root_group = main_group

try:
    print(f"Fetching text from {TEXT_URL}")
    response = https.get(TEXT_URL)
    print("Text Response: ", response.text)
    output_label.text = "\n".join(
        wrap_text_to_pixels(response.text, board.DISPLAY.width - 10, terminalio.FONT)
    )
except OSError as e:
    ERROR_MSG = (
        "No WIFI connection. Please add "
        "CIRCUITPY_WIFI_SSID and CIRCUITPY_WIFI_PASSWORD "
        f"for a valid network into settings.toml\n\nError: {e.args}"
    )
    output_label.text = "\n".join(
        wrap_text_to_pixels(ERROR_MSG, board.DISPLAY.width - 10, terminalio.FONT)
    )

# Wait forever so you can see our label on the display
while True:
    now = time.monotonic()

    if button.value:
        if last_led_blink_time + PLAIN_LED_BLINK_DELAY <= now:
            led.value = not led.value
            last_led_blink_time = now
    else:
        led.value = True
        last_led_blink_time = now

    if last_neopixel_change_time + NEOPIXEL_CHANGE_DELAY <= now:
        rgb_pixel.fill(rainbowio.colorwheel(cur_color_number))
        cur_color_number += 1
        if cur_color_number > 255:
            cur_color_number = 0
        last_neopixel_change_time = now
