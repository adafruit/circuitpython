Environment Variables
=====================

CircuitPython 8.0.0 introduces support for environment variables. Environment
variables are commonly used to store "secrets" such as Wi-Fi passwords and API
keys. This method *does not* make them secure. It only separates them from the
code.

CircuitPython uses a file called ``settings.toml`` at the drive root (no
folder) as the environment.  User code can access the values from the file
using `os.getenv()`. It is recommended to save any values used repeatedly in a
variable because `os.getenv()` will parse the ``settings.toml`` file contents
on every access.

CircuitPython only supports a subset of the full toml specification, see below
for more details. The subset is very "Python-like", which is a key reason we
selected the format.

Due to technical limitations it probably also accepts some files that are
not valid TOML files; bugs of this nature are subject to change (i.e., be
fixed) without the usual deprecation period for incompatible changes.

File format example:

.. code-block::

   str_key="Hello world" # with trailing comment
   int_key = 7
   unicode_key="œuvre"
   unicode_key2="\\u0153uvre" # same as above
   unicode_key3="\\U00000153uvre" # same as above
   escape_codes="supported, including \\r\\n\\"\\\\"
   # comment
   [subtable]
   subvalue="cannot retrieve this using getenv"


Details of the toml language subset
-----------------------------------

* The content is required to be in UTF-8 encoding
* The supported data types are string and integer
* Only basic strings are supported, not triple-quoted strings
* Only integers supported by strtol. (no 0o, no 0b, no underscores 1_000, 011
  is 9, not 11)
* Only bare keys are supported
* Duplicate keys are not diagnosed.
* Comments are supported
* Only values from the "root table" can be retrieved
* due to technical limitations, the content of multi-line
  strings can erroneously be parsed as a value.

CircuitPython behavior
----------------------

CircuitPython will also read the environment to configure its behavior. Some keys are read at
startup once and others are read on reload (ctrl-D in the REPL). If a reload doesn't change things,
then try a reset (a power cycle or pressing the reset button). Other keys are ignored by CircuitPython.
Here are the keys it uses:

Core CircuitPython keys
^^^^^^^^^^^^^^^^^^^^^^^

CIRCUITPY_BLE_NAME
~~~~~~~~~~~~~~~~~~
Default BLE name the board advertises as, including for the BLE workflow.

CIRCUITPY_HEAP_START_SIZE
~~~~~~~~~~~~~~~~~~~~~~~~~
Sets the initial size of the python heap, allocated from the outer heap. Must be a multiple of 4.
The default is currently 8192.
The python heap will grow by doubling and redoubling this initial size until it cannot fit in the outer heap.
Larger values will reserve more RAM for python use and prevent the supervisor and SDK
from large allocations of their own.
Smaller values will likely grow sooner than large start sizes.

CIRCUITPY_PYSTACK_SIZE
~~~~~~~~~~~~~~~~~~~~~~
Sets the size of the python stack. Must be a multiple of 4. The default value is currently 1536.
Increasing the stack reduces the size of the heap available to python code.
Used to avoid "Pystack exhausted" errors when the code can't be reworked to avoid it.

CIRCUITPY_WEB_API_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~~~~
Password required to make modifications to the board from the Web Workflow.

CIRCUITPY_WEB_API_PORT
~~~~~~~~~~~~~~~~~~~~~~
TCP port number used for the web HTTP API. Defaults to 80 when omitted.

CIRCUITPY_WEB_INSTANCE_NAME
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Name the board advertises as for the WEB workflow. Defaults to human readable board name if omitted.

CIRCUITPY_WIFI_PASSWORD
~~~~~~~~~~~~~~~~~~~~~~~
Wi-Fi password used to auto connect to CIRCUITPY_WIFI_SSID.

CIRCUITPY_WIFI_SSID
~~~~~~~~~~~~~~~~~~~
Wi-Fi SSID to auto-connect to even if user code is not running.

Additional board specific keys
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CIRCUITPY_DISPLAY_WIDTH (Sunton, MaTouch)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Selects the correct screen resolution (1024x600 or 800x640) for the particular board variant.
If the CIRCUITPY_DISPLAY_WIDTH parameter is set to a value of 1024 the display is initialized
during power up at 1024x600 otherwise the display will be initialized at a resolution
of 800x480.

`MaTouch ESP32-S3 Parallel TFT with Touch 7“ <https://circuitpython.org/board/makerfabs_tft7/>`_
`Sunton ESP32-2432S028 <https://circuitpython.org/board/sunton_esp32_2432S028/>`_
`Sunton ESP32-2432S024C <https://circuitpython.org/board/sunton_esp32_2432S024C/>`_

CIRCUITPY_DISPLAY_ROTATION
~~~~~~~~~~~~~~~~~~~~~~~~~~
Selects the correct screen rotation (0, 90, 180 or 270) for the particular board variant.
If the CIRCUITPY_DISPLAY_ROTATION parameter is set the display will be initialized
during power up with the selected rotation, otherwise the display will be initialized with
a rotation of 0. Attempting to initialize the screen with a rotation other than 0,
90, 180 or 270 is not supported and will result in an unexpected screen rotation.

`Sunton ESP32-8048S050 <https://circuitpython.org/board/sunton_esp32_8048S050/>`_
`Adafruit Feather RP2350 <https://circuitpython.org/board/adafruit_feather_rp2350/>`_
`Adafruit Metro RP2350 <https://circuitpython.org/board/adafruit_metro_rp2350/>`_

CIRCUITPY_DISPLAY_FREQUENCY
~~~~~~~~~~~~~~~~~~~~~~~~~~~
Allows the entry of a display frequency used during the "dotclock" framebuffer construction.
If a valid frequency is not defined the board will initialize the framebuffer with a
frequency of 12500000hz (12.5Mhz). The value should be entered as an integer in hertz
i.e. CIRCUITPY_DISPLAY_FREQUENCY=16000000 will override the default value with a 16Mhz
display frequency.

`Sunton ESP32-8048S050 <https://circuitpython.org/board/sunton_esp32_8048S050/>`_


CIRCUITPY_PICODVI_ENABLE
~~~~~~~~~~~~~~~~~~~~~~~~
Whether to configure the display at board initialization time, one of the following:

.. code-block::

    CIRCUITPY_PICODVI_ENABLE="detect" # when EDID EEPROM is detected (default)
    CIRCUITPY_PICODVI_ENABLE="always"
    CIRCUITPY_PICODVI_ENABLE="never"

A display configured in this manner is available at ``supervisor.runtime.display``
until it is released by ``displayio.release_displays()``. It does not appear at
``board.DISPLAY``.

`Adafruit Feather RP2350 <https://circuitpython.org/board/adafruit_feather_rp2350/>`_
`Adafruit Metro RP2350 <https://circuitpython.org/board/adafruit_metro_rp2350/>`_

CIRCUITPY_DISPLAY_WIDTH, CIRCUITPY_DISPLAY_HEIGHT, and CIRCUITPY_DISPLAY_COLOR_DEPTH (RP2350 boards with DVI or HSTX connector)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Selects the desired resolution and color depth.

Supported resolutions are:
 * 640x480 with color depth 1, 2, 4 or 8 bits per pixel
 * 320x240 with pixel doubling and color depth 8, 16, or 32 bits per pixel
 * 360x200 with pixel doubling and color depth 8, 16, or 32 bits per pixel

See :py:class:`picodvi.Framebuffer` for more details.

The default value, if unspecified, is 360x200 16 bits per pixel if the connected
display is 1920x1080 or a multiple of it, otherwise 320x240 with 16 bits per pixel.

If height is unspecified, it is set from the width. For example, a width of 640
implies a height of 480.

Example: Configure the display to 640x480 black and white (1 bit per pixel):

.. code-block::

    CIRCUITPY_DISPLAY_WIDTH=640
    CIRCUITPY_DISPLAY_COLOR_DEPTH=1

`Adafruit Feather RP2350 <https://circuitpython.org/board/adafruit_feather_rp2350/>`_
`Adafruit Metro RP2350 <https://circuitpython.org/board/adafruit_metro_rp2350/>`_

CIRCUITPY_TERMINAL_SCALE
~~~~~~~~~~~~~~~~~~~~~~~~
Allows the entry of a display scaling factor used during the terminalio console construction.
The entered scaling factor only affects the terminalio console and has no impact on
the UART, Web Workflow, BLE Workflow, etc consoles.

This feature is not enabled on boards that the CIRCUITPY_SETTINGS_TOML (or CIRCUITPY_FULL_BUILD)
flag has been set to 0. Currently this is primarily boards with limited flash including some
of the Atmel_samd boards based on the SAMD21/M0 microprocessor.

CIRCUITPY_TERMINAL_FONT
~~~~~~~~~~~~~~~~~~~~~~~
Specifies a custom font file path to use for the terminalio console instead of the default
``/fonts/terminal.lvfontbin``. This allows users to create and use custom fonts for the
CircuitPython console.

This feature requires both CIRCUITPY_SETTINGS_TOML and CIRCUITPY_LVFONTIO to be enabled.

Example:

.. code-block::

    CIRCUITPY_TERMINAL_FONT="/fonts/myfont.lvfontbin"

`boards that the terminalio core module is available on <https://docs.circuitpython.org/en/latest/shared-bindings/terminalio/>`_
