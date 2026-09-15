Environment Variables
=====================

CircuitPython provides support for environment variables. These values
can be examined by user code, and are also used as settings by CircuitPython during startup.

CircuitPython looks for a  file called ``settings.toml`` at the **CIRCUITPY** drive root
to find the values of environment variables,
The file format is a subset of the `TOML config file language <https://toml.io>`__.

User code can access the values from the file using either `os.getenv()` or `supervisor.get_setting()`
The value returned by `os.getenv()` is always a string, but `supervisor.get_setting()`
will parse a value into a Python object: a string, an integer, a float, or a boolean.

Both `os.getenv()` and `supervisor.get_setting()`
read and parse the ``settings.toml`` file on every access.
It will save time to copy any values you use repeatedly into variables.

Environment variables are sometimes used to store "secrets" such as Wi-Fi passwords and API
keys. The ``settings.toml`` file *does not* make the secrets secure. It only separates them from the
code.

CircuitPython supports only a subset of the full TOML specification; see below for more details.
The subset is very "Python-like", which is a key reason the format was selected.
To make the code simpler, the implementation accepts some files that are
not valid TOML, but do not depend on this.

The full TOML specification provides for tables labeled with table names in brackets, like
``[table_name]``.
CircuitPython does not support this and ignores any explicit inline TOML tables.

Here is an example ``settings.toml`` file.
Entries consist of a key and value, separated by an ``=`` sign.
Upper and lower case may both be used in the key name.

.. code-block::

   # Comment.
   CIRCUITPY_WIFI_PASSWORD = "mypassword"
   GREETING="Hello world"       # trailing comments are ok
   REPEAT_COUNT = 7             # an integer
   CIRCUITPY_SDCARD_USB = false # a boolean
   delay = 0.75                 # a float
   FRENCH="œuvre"               # unicode can be used
   FRENCH2="\u0153uvre"        # same unicode string, using a 16-bit escape code
   FRENCH3="\U00000153uvre"    # same unicode string, using a 32-bit escape code
   STRING_WITH_ESCAPE_CODES="supported, including \r \n \" \\"

Details of the TOML language subset
-----------------------------------

* The content must be in UTF-8 encoding
* The supported data types are strings, integers, floats, and booleans.
* Whitespace is allowed.
* Only basic strings are supported, not triple-quoted strings.
* Only integers supported by ``strtol()`` can be parsed:
  no ``0o``, no ``0b``, no underscores ``1_000``, ``011`` is 9, not 11.
* Only bare keys are supported.
* Duplicate keys are not diagnosed.
* Comments are allowed.
* Only values from the "root table" can be retrieved.


CircuitPython behavior
----------------------

On startup, CircuitPython looks for for certain key/value pairs to use as configuration values.
Some values are read only once, after a hard reset, and others are read on each reload (ctrl-D in the REPL).
If you edit ``settings.toml`` and a reload doesn't read your changes,
then try a hard reset (a power cycle or pressing the reset button).
You can also include any other key/value pairs in the file for use with your own code.

Keys that affect CircuitPython behavior
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CIRCUITPY_BLE_NAME (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~
If supplied, sets the BLE name the board advertises as, including for the BLE workflow.
Otherwise, defaults to ``CIRCUITPYxxxx``, where ``xxxx`` varies per board.

CIRCUITPY_BLE_WORKFLOW (boolean)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If ``false``, disable the BLE workflow. Defaults to ``true``. If ``false``,
changing ``supervisor.runtime.ble_workflow`` has no effect.


CIRCUITPY_HEAP_START_SIZE (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sets the initial size of the python heap, allocated from the outer heap. Must be a multiple of 4.
The default is currently 8192.
The python heap will grow by doubling and redoubling this initial size until it cannot fit in the outer heap.
Larger values will reserve more RAM for python use and prevent the supervisor and SDK
from large allocations of their own.
Smaller values will likely grow sooner than large start sizes.

CIRCUITPY_PYSTACK_SIZE (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Sets the size of the python stack. Must be a multiple of 4. The default value is currently 1536.
Increasing the stack reduces the size of the heap available to python code.
Used to avoid "Pystack exhausted" errors when the code can't be reworked to avoid it.

CIRCUITPY_WEB_API_PASSWORD (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Password required to make modifications to the board from the Web Workflow.
If the password is not specified, the Web Workflow is not enabled.

CIRCUITPY_WEB_API_PORT (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
TCP port number used for the Web Workflow HTTP API. Defaults to 80 when omitted.

CIRCUITPY_WEB_INSTANCE_NAME (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Human-friendly name the board advertises over mDNS for the Web Workflow.
Defaults to the human-readable board name if omitted.
This is not the hostname.

CIRCUITPY_WIFI_SSID (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CIRCUITPY_WIFI_PASSWORD (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If these values are supplied, connects automatically to a local WiFi network
with the specified SSID and password before ``boot.py`` and/or ``code.py`` are run.

CIRCUITPY_WIFI_HOSTNAME (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
If supplied, sets the initial ``wifi.radio.hostname`` to the given value.
Otherwise, the default value is ``cpy-<board_name>-<mac_address>``,
with some shortening for length if necessary.
If the supplied value is an invalid hostname or is too long, it is ignored.

CIRCUITPY_SDCARD_USB (boolean)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Present a mounted SD card as a USB MSC device. If the board has default pins for an SD card socket,
the card is mounted automatically on startup.
Only one card can be presented.
Defaults to ``true``, except on the Adafruit Memento board, where it defaults to ``false``
for now to avoid a bug.
SD card presentation can slow down board startup,
so set this to ``false`` if you don't need this feature.


Additional board-specific keys
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CIRCUITPY_DISPLAY_WIDTH (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(Sunton, MaTouch boards)

Selects the correct screen resolution (1024x600 or 800x640) for the particular board variant.
If the CIRCUITPY_DISPLAY_WIDTH parameter is set to a value of 1024 the display is initialized
during power up at 1024x600 otherwise the display will be initialized at a resolution
of 800x480.

`MaTouch ESP32-S3 Parallel TFT with Touch 7“ <https://circuitpython.org/board/makerfabs_tft7/>`_
`Sunton ESP32-2432S028 <https://circuitpython.org/board/sunton_esp32_2432S028/>`_
`Sunton ESP32-2432S024C <https://circuitpython.org/board/sunton_esp32_2432S024C/>`_

CIRCUITPY_DISPLAY_ROTATION (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Selects the correct screen rotation (0, 90, 180 or 270) for the particular board variant.
If the CIRCUITPY_DISPLAY_ROTATION parameter is set the display will be initialized
during power up with the selected rotation, otherwise the display will be initialized with
a rotation of 0. Attempting to initialize the screen with a rotation other than 0,
90, 180 or 270 is not supported and will result in an unexpected screen rotation.

`Sunton ESP32-8048S050 <https://circuitpython.org/board/sunton_esp32_8048S050/>`_
`Adafruit Feather RP2350 <https://circuitpython.org/board/adafruit_feather_rp2350/>`_
`Adafruit Metro RP2350 <https://circuitpython.org/board/adafruit_metro_rp2350/>`_

CIRCUITPY_DISPLAY_FREQUENCY (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Allows the entry of a display frequency used during the "dotclock" framebuffer construction.
If a valid frequency is not defined the board will initialize the framebuffer with a
frequency of 12500000hz (12.5Mhz). The value should be entered as an integer in hertz
i.e. CIRCUITPY_DISPLAY_FREQUENCY=16000000 will override the default value with a 16Mhz
display frequency.

`Sunton ESP32-8048S050 <https://circuitpython.org/board/sunton_esp32_8048S050/>`_


CIRCUITPY_PICODVI_ENABLE (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

CIRCUITPY_DISPLAY_WIDTH (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CIRCUITPY_DISPLAY_HEIGHT (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CIRCUITPY_DISPLAY_COLOR_DEPTH (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
(RP2350 boards with DVI or HSTX connector)

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

CIRCUITPY_SAFEMODE_DELAY (float)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Wait for the specified amount of time, in seconds, for the user to press the reset button
to initiate safe mode after a hard reset.
The status LED blinks during this time.
If not specified, use the default delay, which is one second.

CIRCUITPY_TERMINAL_SCALE (integer)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Allows the entry of a display scaling factor used during the terminalio console construction.
The entered scaling factor only affects the terminalio console and has no impact on
the UART, Web Workflow, BLE Workflow, etc consoles.

This feature is not enabled on boards that the CIRCUITPY_SETTINGS_TOML (or CIRCUITPY_FULL_BUILD)
flag has been set to 0. Currently this is primarily boards with limited flash including some
of the Atmel_samd boards based on the SAMD21/M0 microprocessor.

CIRCUITPY_TERMINAL_FONT (string)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Specifies a custom font file path to use for the terminalio console instead of the default
``/fonts/terminal.lvfontbin``. This allows users to create and use custom fonts for the
CircuitPython console.

This feature requires both CIRCUITPY_SETTINGS_TOML and CIRCUITPY_LVFONTIO to be enabled.

Example:

.. code-block::

    CIRCUITPY_TERMINAL_FONT="/fonts/myfont.lvfontbin"

`boards that the terminalio core module is available on <https://docs.circuitpython.org/en/latest/shared-bindings/terminalio/>`_
