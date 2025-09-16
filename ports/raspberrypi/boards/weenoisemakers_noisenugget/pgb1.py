from board import *
import busio
import displayio
import adafruit_displayio_ssd1306
from fourwire import FourWire
from neopixel import NeoPixel
import pwmio
from digitalio import DigitalInOut, Direction, Pull

_DISPLAY = None
_LEDS = None
_KEYBOARD_STATE = 0
_KEYBOARD_PREV_STATE = 0

VOLTAGE_MONITOR = A2
BATTERY = A2

K_TRACK = 0x10000000
K_STEP = 0x08000000
K_PLAY = 0x02000000
K_REC = 0x04000000
K_ALT = 0x00040000
K_PATT = 0x00001000
K_SONG = 0x00000040
K_MENU = 0x00000020

K_UP = 0x00020000
K_DOWN = 0x00800000
K_RIGHT = 0x00000800
K_LEFT = 0x20000000
K_A = 0x01000000
K_B = 0x00000001

K_1 = 0x00400000
K_2 = 0x00010000
K_3 = 0x00000400
K_4 = 0x00000010
K_5 = 0x00000002
K_6 = 0x00000080
K_7 = 0x00002000
K_8 = 0x00080000
K_9 = 0x00200000
K_10 = 0x00008000
K_11 = 0x00000200
K_12 = 0x00000008
K_13 = 0x00000004
K_14 = 0x00000100
K_15 = 0x00004000
K_16 = 0x00100000

KEY_LIST = [
    K_TRACK,
    K_STEP,
    K_PLAY,
    K_REC,
    K_ALT,
    K_PATT,
    K_SONG,
    K_MENU,
    K_UP,
    K_DOWN,
    K_RIGHT,
    K_LEFT,
    K_A,
    K_B,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_10,
    K_11,
    K_12,
    K_13,
    K_14,
    K_15,
    K_16,
]

KEY_NAME = {
    K_TRACK: "Track",
    K_STEP: "Step",
    K_PLAY: "Play",
    K_REC: "Rec",
    K_ALT: "Alt",
    K_PATT: "Pattern",
    K_SONG: "Song",
    K_MENU: "Menu",
    K_UP: "Up",
    K_DOWN: "Down",
    K_RIGHT: "Right",
    K_LEFT: "Left",
    K_A: "A",
    K_B: "B",
    K_1: "1",
    K_2: "2",
    K_3: "3",
    K_4: "4",
    K_5: "5",
    K_6: "6",
    K_7: "7",
    K_8: "8",
    K_9: "9",
    K_10: "10",
    K_11: "11",
    K_12: "12",
    K_13: "13",
    K_14: "14",
    K_15: "15",
    K_16: "16",
}

LED_MENU = 0
LED_SONG = 1
LED_PATT = 2
LED_ALT = 3
LED_TRACK = 4
LED_K_1 = 5
LED_K_2 = 6
LED_K_3 = 7
LED_K_4 = 8
LED_K_5 = 9
LED_K_6 = 10
LED_K_7 = 11
LED_K_8 = 12
LED_PLAY = 13
LED_STEP = 14
LED_K_9 = 15
LED_K_10 = 16
LED_K_11 = 17
LED_K_12 = 18
LED_K_13 = 19
LED_K_14 = 20
LED_K_15 = 21
LED_K_16 = 22
LED_REC = 23

_KEY_COL = [
    DigitalInOut(GP21),
    DigitalInOut(GP22),
    DigitalInOut(GP26),
    DigitalInOut(GP23),
    DigitalInOut(GP29),
]
_KEY_ROW = [
    DigitalInOut(GP20),
    DigitalInOut(GP18),
    DigitalInOut(GP19),
    DigitalInOut(GP24),
    DigitalInOut(GP25),
    DigitalInOut(GP27),
]

for pin in _KEY_COL:
    pin.direction = Direction.OUTPUT
    pin.value = False

for pin in _KEY_ROW:
    pin.direction = Direction.INPUT
    pin.pull = Pull.DOWN


def scan_keyboard():
    global _KEYBOARD_STATE
    global _KEYBOARD_PREV_STATE

    val = 0
    for col in _KEY_COL:
        col.value = True
        for row in _KEY_ROW:
            val = val << 1
            if row.value:
                val = val | 1
        col.value = False
    _KEYBOARD_PREV_STATE = _KEYBOARD_STATE
    _KEYBOARD_STATE = val


def pressed(keys):
    global _KEYBOARD_STATE

    return (_KEYBOARD_STATE & keys) != 0


def falling(keys):
    global _KEYBOARD_STATE
    global _KEYBOARD_PREV_STATE

    all_falling_keys = _KEYBOARD_STATE & ~_KEYBOARD_PREV_STATE
    return (all_falling_keys & keys) != 0


def raising(keys):
    global _KEYBOARD_STATE
    global _KEYBOARD_PREV_STATE

    all_raising_keys = ~_KEYBOARD_STATE & _KEYBOARD_PREV_STATE
    return (all_raising_keys & keys) != 0


def display(spi_frequency=1000000):
    global _DISPLAY

    if not _DISPLAY:
        displayio.release_displays()

        spi = busio.SPI(GP10, GP11)
        cs = None
        dc = GP12
        reset = GP13

        display_bus = FourWire(
            spi, command=dc, chip_select=cs, reset=reset, baudrate=spi_frequency
        )
        _DISPLAY = adafruit_displayio_ssd1306.SSD1306(display_bus, width=128, height=64)

    return _DISPLAY


def neopixel(brightness=0.1):
    global _LEDS

    if not _LEDS:
        _LEDS = NeoPixel(GP14, 24, brightness=brightness)

    return _LEDS


def key_to_led(key):
    map = {
        K_TRACK: LED_TRACK,
        K_STEP: LED_STEP,
        K_PLAY: LED_PLAY,
        K_REC: LED_REC,
        K_ALT: LED_ALT,
        K_PATT: LED_PATT,
        K_SONG: LED_SONG,
        K_MENU: LED_MENU,
        K_1: LED_K_1,
        K_2: LED_K_2,
        K_3: LED_K_3,
        K_4: LED_K_4,
        K_5: LED_K_5,
        K_6: LED_K_6,
        K_7: LED_K_7,
        K_8: LED_K_8,
        K_9: LED_K_9,
        K_10: LED_K_10,
        K_11: LED_K_11,
        K_12: LED_K_12,
        K_13: LED_K_13,
        K_14: LED_K_14,
        K_15: LED_K_15,
        K_16: LED_K_16,
    }
    return map[key]


def simple_synth(synth, verbose=False):
    base_note = 60

    leds = neopixel()

    keys = [
        (K_9, 0),
        (K_2, 1),
        (K_10, 2),
        (K_3, 3),
        (K_11, 4),
        (K_12, 5),
        (K_5, 6),
        (K_13, 7),
        (K_6, 8),
        (K_14, 9),
        (K_7, 10),
        (K_15, 11),
        (K_16, 12),
    ]

    for key, _ in keys:
        leds[key_to_led(key)] = (0, 0, 255)
    leds[key_to_led(K_1)] = (255, 255, 0)
    leds[key_to_led(K_8)] = (255, 255, 0)
    leds.show()

    while True:
        scan_keyboard()

        # Lower base note 1 octave down
        if falling(K_1) and base_note > 12:
            # Turn off any potentially on notes
            for _, offset in keys:
                synth.release(base_note + offset)
            base_note -= 12
            if verbose:
                print("Octave down")

        # Raise base note 1 octave up
        if falling(K_8) and base_note <= 96:
            # Turn off any potentially on notes
            for _, offset in keys:
                synth.release(base_note + offset)
            base_note += 12
            if verbose:
                print("Octave up")

        for key, offset in keys:
            note = base_note + offset
            if falling(key):
                if verbose:
                    print("On " + str(note))
                synth.press(note)
            if raising(key):
                if verbose:
                    print("Off " + str(note))
                synth.release(note)
