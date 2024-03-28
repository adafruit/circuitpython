from board import *
import busio
import audiobusio
import displayio
import adafruit_displayio_ssd1306
import wm8960
from fourwire import FourWire
from neopixel import NeoPixel
import pwmio
from digitalio import DigitalInOut, Direction, Pull

_AUDIO = None
_I2C = None
_DISPLAY= None
_LEDS = None
_SAMPLE_RATE = 22050
_KEYBOARD_STATE = 0
_KEYBOARD_PREV_STATE = 0

VOLTAGE_MONITOR = A2
BATTERY = A2

K_TRACK = 0x10000000
K_STEP  = 0x08000000
K_PLAY  = 0x02000000
K_REC   = 0x04000000
K_ALT   = 0x00040000
K_PATT  = 0x00001000
K_SONG  = 0x00000040
K_MENU  = 0x00000020

K_UP    = 0x00020000
K_DOWN  = 0x00800000
K_RIGHT = 0x00000800
K_LEFT  = 0x20000000
K_A     = 0x01000000
K_B     = 0x00000001

K_1  = 0x00400000
K_2  = 0x00010000
K_3  = 0x00000400
K_4  = 0x00000010
K_5  = 0x00000002
K_6  = 0x00000080
K_7  = 0x00002000
K_8  = 0x00080000
K_9  = 0x00200000
K_10 = 0x00008000
K_11 = 0x00000200
K_12 = 0x00000008
K_13 = 0x00000004
K_14 = 0x00000100
K_15 = 0x00004000
K_16 = 0x00100000

KEY_LIST = [K_TRACK, K_STEP, K_PLAY, K_REC, K_ALT,
             K_PATT, K_SONG, K_MENU, 

             K_UP, K_DOWN, K_RIGHT, K_LEFT, K_A, K_B, 

             K_1, K_2, K_3, K_4, K_5, K_6, K_7, K_8, 
             K_9, K_10, K_11, K_12, K_13, K_14, K_15, K_16]

KEY_NAME = {K_TRACK : "Track",
            K_STEP  : "Step",
            K_PLAY  : "Play",
            K_REC   : "Rec",
            K_ALT   : "Alt",
            K_PATT  : "Pattern",
            K_SONG  : "Song",
            K_MENU  : "Menu", 

            K_UP    : "Up",
            K_DOWN  : "Down",
            K_RIGHT : "Right",
            K_LEFT  : "Left",
            K_A     : "A",
            K_B     : "B", 

            K_1 : "1", 
            K_2 : "2",
            K_3 : "3", 
            K_4 : "4", 
            K_5 : "5", 
            K_6 : "6", 
            K_7 : "7", 
            K_8 : "8", 
            K_9 : "9", 
            K_10 : "10", 
            K_11 : "11", 
            K_12 : "12", 
            K_13 : "13", 
            K_14 : "14", 
            K_15 : "15", 
            K_16 : "16"}

_KEY_COL = [DigitalInOut(GP18),
            DigitalInOut(GP19),
            DigitalInOut(GP26),
            DigitalInOut(GP23),
            DigitalInOut(GP29)]
_KEY_ROW = [DigitalInOut(GP20),
            DigitalInOut(GP21),
            DigitalInOut(GP22),
            DigitalInOut(GP24),
            DigitalInOut(GP25),
            DigitalInOut(GP27)]

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

def I2C():
    global _I2C

    if not _I2C:
        SCL = GP7
        SDA = GP6
        _I2C = busio.I2C(SCL, SDA)

    return _I2C

def audio(sample_rate=22050):
    global _AUDIO
    global _SAMPLE_RATE

    if not _AUDIO:
        i2s_out   = GP1
        i2s_lrclk = GP2
        i2s_bclk  = GP3
        mclk_pin  = GP4

	_SAMPLE_RATE = sample_rate
	# Target MCLK frequency
        MCLK_FREQUENCY = 256 * sample_rate
     
        # Start DAC MCLK using a PWM channel
        mclk = pwmio.PWMOut(mclk_pin, frequency=MCLK_FREQUENCY, duty_cycle=0x7fff)
         
        dac = wm8960.WM8960(I2C())
        dac.start_i2s_out_org()
         
	_AUDIO = audiobusio.I2SOut(i2s_bclk, i2s_lrclk, i2s_out, left_justified=False)

    return _AUDIO

def sample_rate():
    return _SAMPLE_RATE

def display(spi_frequency=1000000):
    global _DISPLAY

    if not _DISPLAY:
        displayio.release_displays()
    
        spi = busio.SPI(GP10, GP11)
        cs = None
        dc = GP12
        reset = GP13
         
        display_bus = FourWire(spi, command=dc, chip_select=cs, reset=reset, baudrate=spi_frequency)
        _DISPLAY = adafruit_displayio_ssd1306.SSD1306(display_bus, width=128, height=64)

    return _DISPLAY

def neopixel(brightness=0.1):
    global _LEDS

    if not _LEDS:
        _LEDS = NeoPixel(GP5, 24, brightness=brightness)

    return _LEDS
