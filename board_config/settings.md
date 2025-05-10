# CircuitPython Board Configuration Settings Reference

This document lists all available configuration options for `board_setting.toml` files with detailed annotations.

## How to Use This Guide
1. **For New Boards**: Start with the [metadata] section and work downward
2. **For Existing Boards**: Check each section against your hardware specs
3. **Validation**: Run `python tools/validate_board.py` to check your config

❗ **Important Notes**:
- All paths are relative to the board directory
- Changes require rebuilding firmware to take effect
- Use the template (`board_template.toml`) as a starting point

## Table of Contents
- [Metadata](#metadata)
- [USB Configuration](#usb-configuration)
- [Build Settings](#build-settings)
- [Feature Flags](#feature-flags)
- [Frozen Modules](#frozen-modules)
- [Pin Definitions](#pin-definitions)
- [Default Interfaces](#default-interfaces)
- [Wireless Settings](#wireless-settings)

## Metadata
```toml
[metadata]
# REQUIRED: Board display name (shown in CircuitPython REPL)
# Format: Short but descriptive (e.g., "Adafruit ItsyBitsy ESP32")
name = "Board Name"

# REQUIRED: Manufacturer name
# Format: Full company name (e.g., "Adafruit Industries")
vendor = "Vendor Name"

# RECOMMENDED: Product webpage or documentation URL
# Format: Valid HTTPS URL
url = "https://example.com"

# REQUIRED: Microcontroller family
# Valid values depend on port:
# - Espressif: ESP32, ESP32-S2, ESP32-S3, ESP8266
# - Nordic: nRF52840, nRF52833
# - Raspberry Pi: RP2040
# - SAMD: SAMD21, SAMD51
mcu = "MCU_TYPE"

# OPTIONAL: Board revision (for hardware variants)
# Format: String or number (e.g., "1.0" or "Rev B")
revision = ""

# OPTIONAL: List of board designers/maintainers
# Format: Array of strings
contributors = ["Developer Name <email@example.com>"]
```

## USB Configuration
```toml
[usb]
# REQUIRED: Vendor ID (VID) in hexadecimal format
# Format: 0x followed by 4 hex digits (e.g., 0x303A for Espressif)
# Note: Must be officially assigned by USB-IF for commercial products
vid = "0x303a"

# REQUIRED: Product ID (PID) in hexadecimal format
# Format: 0x followed by 4 hex digits (e.g., 0x80AB)
# Note: Must be unique under your VID
pid = "0x80AB"

# REQUIRED: Product name string (shown in USB descriptors)
# Format: Max 32 characters, no special symbols
# Example: "Feather ESP32-S2"
product = "Product Name"

# REQUIRED: Manufacturer string (shown in USB descriptors)
# Format: Max 32 characters, company name
# Example: "Adafruit Industries LLC"
manufacturer = "Company"

# OPTIONAL: Serial number generation method
# Values: "none", "chip", or custom string template
# Default: "chip" (uses MCU's unique ID)
serial = "chip"

# OPTIONAL: USB power settings (in milliamps)
# Typical values: 100 (low power) to 500 (high power)
max_power = "500"

# OPTIONAL: USB version (defaults to 2.0)
# Values: "1.1", "2.0", or "2.1"
usb_version = "2.0"

# OPTIONAL: USB device class/subclass/protocol
# Defaults to CDC/MSC/HID composite device
device_class = "0xEF"
device_subclass = "0x02"
device_protocol = "0x01"
```

## Build Settings

### ESP Family (ESP32, ESP32-S2/S3, ESP8266)
```toml
[build]
# REQUIRED: ESP-IDF target chip type
# Values: "esp32", "esp32s2", "esp32s3", "esp32c3", "esp8266"
idf_target = "esp32"

# OPTIONAL: Flash interface mode (affects performance)
# Values: "qio" (Quad I/O, fastest), "qout" (Quad Output),
#         "dio" (Dual I/O), "dout" (Dual Output)
# Default: "qio" for most boards
flash_mode = "qio"

# OPTIONAL: Flash frequency
# Values: "80m" (MHz), "40m", "20m" (ESP8266 only)
# Note: Higher frequencies may require voltage >3.3V
flash_freq = "80m"

# OPTIONAL: Flash size (must match hardware)
# Values: "1MB", "2MB", "4MB", "8MB", "16MB"
# Warning: Overestimating may cause runtime failures
flash_size = "4MB"

# OPTIONAL: PSRAM configuration (if board has PSRAM)
psram_size = "2MB"          # Size: "2MB", "4MB", "8MB", "16MB"
psram_mode = "qio"          # Mode: Same as flash_mode options
psram_freq = "80m"          # Frequency: Same as flash_freq options

# OPTIONAL: Partition table configuration
# Default: "default" (single factory partition)
# Alternatives: "huge_app" (3MB app space), "custom" (requires CSV file)
partition_table = "default"

# OPTIONAL: Board-specific SDK configuration
# Format: key-value pairs matching ESP-IDF sdkconfig defaults
sdkconfig = {
    "CONFIG_FREERTOS_HZ" = "1000",
    "CONFIG_ESPTOOLPY_FLASHSIZE" = "4MB"
}
```

### ARM Family (Cortex-M0/M0+/M4/M7)
```toml
[build]
# OPTIONAL: CPU core type (auto-detected if not specified)
# Values: "cortex-m0", "cortex-m0plus", "cortex-m4", "cortex-m7"
# Note: Must match the actual microcontroller hardware
cpu = "cortex-m4"

# OPTIONAL: Floating Point Unit configuration
# For M4 chips: "fpv4-sp-d16" (single precision)
# For M7 chips: "fpv5-d16" (double precision)
# Leave empty for M0/M0+ (no FPU)
fpu = "fpv4-sp-d16"

# OPTIONAL: Floating point ABI
# Values: "soft" (software FP), "softfp" (hardware with soft ABI),
#         "hard" (hardware FP with hard ABI)
# Recommendation: "hard" for M4/M7 with FPU
float_abi = "hard"

# OPTIONAL: Optimization flags
# Default: "-Os" (optimize for size)
# Alternatives: "-O2", "-O3", "-Og" (debug)
optimize = "-Os"

# OPTIONAL: Custom compiler flags
# Format: Array of strings
cflags = [
    "-DDEBUG_LEVEL=1",
    "-fstack-usage"
]

# OPTIONAL: Linker script override
# Default: Uses port-specific linker script
# Format: Relative path from board directory
linker_script = "ld/custom.ld"

# OPTIONAL: Startup file override
# Default: Uses port-specific startup file
# Format: Relative path from board directory
startup_file = "startup_custom.c"

# OPTIONAL: Board-specific memory layout
# Units: kilobytes (kB)
flash_size = "512"      # Total flash capacity
ram_size = "128"        # Total RAM capacity
```

## Feature Flags
```toml
[features]
# Enable/disable major CircuitPython features
# Note: Disabling unused features reduces firmware size

### Wireless Features
circuitpy_ble = false       # Bluetooth Low Energy support
                            # Required for: BLE libraries, _bleio
                            # Hardware requirement: BLE-capable chip

circuitpy_wifi = false      # WiFi network support
                            # Required for: wifi, socketpool
                            # Hardware requirement: WiFi-capable chip

circuitpy_ssl = false       # SSL/TLS encryption support
                            # Requires: circuitpy_wifi or ethernet
                            # Adds ~50kB to firmware size

### Hardware Peripherals
circuitpy_esp_camera = false # ESP32-specific camera support
                            # Requires: ESP32 with camera module

circuitpy_nfc = false       # NFC/RFID support
                            # Hardware requirement: NFC controller

circuitpy_displayio = true  # Display output subsystem
                            # Required for: display drivers, framebuf
                            # Disable if no display connected

### Core Features
circuitpy_audioio = true    # Audio input/output support
circuitpy_touchio = true    # Capacitive touch support
circuitpy_countio = true    # Pulse counting support
circuitpy_framebuffer = true # Frame buffer support

### Debug/Development
circuitpy_debug = false     # Enable debug features
                            # Adds debug output, reduces performance

### Memory Configuration
circuitpy_heap_size = "32K" # Custom heap size allocation
                            # Format: Number with K/M suffix
                            # Default: Port-specific optimal size

### Special Modes
safe_mode = false           # Enable safe mode on boot
                            # Disables most hardware interfaces
```

## Frozen Modules & Libraries
```toml
[frozen]
### Common Peripheral Libraries
libraries = [
    # USB HID Devices
    "adafruit_hid",
    "usb_hid/__init__.py",

    # Display Output
    "adafruit_display_text",
    "adafruit_displayio_ssd1306",

    # Audio Devices
    "adafruit_audiobus",
    "adafruit_audioio",

    # Camera
    "adafruit_esp32spi/adafruit_esp32cam.py",

    # Touchscreen
    "adafruit_focaltouch",

    # Utility Libraries
    "adafruit_bus_device",
    "neopixel"
]

### Testing Frozen Modules
# Add these to your test_board_config.py:
"""
import usb_hid  # Test USB HID
import adafruit_display_text  # Test display
import audiobusio  # Test audio
import adafruit_esp32cam  # Test camera
import adafruit_focaltouch  # Test touch
"""

### Pin Configuration Examples
[pins]
# I2C Example (multiple buses)
i2c0 = {
    scl = 5,
    sda = 4,
    frequency = 400000
}
i2c1 = {
    scl = 27,
    sda = 26,
    frequency = 100000
}

# SPI Example
spi0 = {
    sck = 18,
    mosi = 19,
    miso = 16,
    baudrate = 24000000
}

# UART Example
uart0 = {
    tx = 0,
    rx = 1,
    baudrate = 115200
}

# LED Example (with PWM)
status_led = {
    number = 13,
    capabilities = ["digital", "pwm"],
    inverted = true,
    pwm_frequency = 5000
}

# Button Example
boot_button = {
    number = 0,
    capabilities = ["digital"],
    pull = "up",
    debounce = 50
}

# USB Pins (if configurable)
usb_dp = {
    number = "DP",
    capabilities = ["usb"]
}
usb_dm = {
    number = "DM",
    capabilities = ["usb"]
}

### Peripheral Validation Tests
# Add to test_board_config.py:
"""
import board
import busio

# Test I2C
i2c = busio.I2C(board.SCL, board.SDA)
assert i2c.scan(), "I2C devices not found"

# Test SPI
spi = busio.SPI(board.SCK, MOSI=board.MOSI, MISO=board.MISO)
assert spi.try_lock(), "SPI init failed"

# Test UART
uart = busio.UART(board.TX, board.RX, baudrate=115200)
assert uart.baudrate == 115200, "UART config error"

# Test LED
import digitalio
led = digitalio.DigitalInOut(board.LED)
led.switch_to_output()
led.value = True
"""
```

## Pin Definitions
```toml
[pins]
# Define all available pins and their capabilities
# Format: pin_name = {number = X, capabilities = [...]}

### Basic Digital/Analog Pins
led = {
    number = 25,            # REQUIRED: MCU pin number or special name (e.g. "LED")
    capabilities = ["digital"],  # REQUIRED: Array of supported functions
    inverted = true,        # OPTIONAL: True if active-low (default: false)
    description = "User LED" # OPTIONAL: Human-readable description
}

button = {
    number = 0,
    capabilities = ["digital", "analog"],
    pull = "up",            # OPTIONAL: Internal pull resistor ("up"/"down"/null)
    debounce = 10           # OPTIONAL: Debounce time in milliseconds
}

### Special Function Pins
neopixel = {
    number = 21,
    capabilities = ["digital", "neopixel"],
    frequency = 800000      # OPTIONAL: NeoPixel data rate in Hz
}

### Peripheral Pins
i2c_sda = {
    number = 4,
    capabilities = ["digital", "i2c"],
    i2c_function = "SDA",   # OPTIONAL: Explicit function assignment
    i2c_bus = 0             # OPTIONAL: Bus number for multi-bus MCUs
}

### Pin Capabilities Reference
# Available capabilities (choose all that apply):
# Basic:
# - "digital"   : Basic GPIO input/output
# - "analog"    : Analog input (ADC)
# - "pwm"       : PWM output

# Communication:
# - "i2c"       : I2C bus
# - "spi"       : SPI bus
# - "uart"      : UART serial
# - "can"       : CAN bus

# Special:
# - "adc"       : Analog-to-digital
# - "dac"       : Digital-to-analog
# - "touch"     : Capacitive touch
# - "neopixel"  : NeoPixel/WS2812 control
# - "display"   : Display interface
# - "usb"       : USB pins

### Advanced Options
# OPTIONAL: Alternate functions (for STM32, etc.)
alternate_functions = [
    {pin = "PA8", af = "TIM1_CH1", mode = "AF_PP"},
    {pin = "PB6", af = "I2C1_SCL", mode = "AF_OD"}
]

# OPTIONAL: Electrical characteristics
pin_rules = {
    max_voltage = "3.3",    # Maximum input voltage (V)
    max_current = "20",     # Maximum current (mA)
    drive_strength = "high" # Drive strength (low/medium/high)
}

# OPTIONAL: Board-specific pin labels
labels = {
    "D0" = 0,               # Map schematic names to MCU pins
    "A0" = "PA2",
    "SCL" = {number = 5, bus = 0}  # Detailed mapping
}

# OPTIONAL: Pin groups for common functions
groups = {
    "i2c0" = ["i2c_sda", "i2c_scl"],
    "spi0" = ["spi_mosi", "spi_miso", "spi_sck"]
}
```

## Default Interfaces
```toml
[default_interfaces]
# Define default communication interfaces when not specified
# These pins will be used when code calls:
# busio.I2C() / busio.SPI() / busio.UART() without pin parameters

### UART Configuration
uart_rx = 1        # Default UART receive pin
uart_tx = 0        # Default UART transmit pin
uart_baudrate = 115200  # Optional: Default baud rate
uart_timeout = 1.0 # Optional: Default timeout (seconds)

### I2C Configuration
i2c_scl = 5        # Default I2C clock pin
i2c_sda = 4        # Default I2C data pin
i2c_frequency = 400000  # Optional: Default clock frequency (Hz)
i2c_timeout = 255  # Optional: Default timeout (ms)

### SPI Configuration
spi_sck = 18       # Default SPI clock pin
spi_mosi = 19      # Default SPI MOSI pin
spi_miso = 16      # Default SPI MISO pin
spi_baudrate = 1000000  # Optional: Default clock rate
spi_polarity = 0   # Optional: Clock polarity (0/1)
spi_phase = 0      # Optional: Clock phase (0/1)

### CAN Configuration (if supported)
can_tx = 22        # Default CAN transmit pin
can_rx = 23        # Default CAN receive pin
can_baudrate = 250000  # Optional: Default bitrate

### Special Cases
# OPTIONAL: Multiple interface instances
i2c1 = {
    scl = 27,
    sda = 26,
    frequency = 100000
}

# OPTIONAL: Interface-specific settings
uart_console = {
    tx = 0,        # Console output pin
    rx = 1,        # Console input pin
    baudrate = 115200,
    timeout = 0.1
}

# OPTIONAL: Peripheral pin validation
validate_pins = true  # Check pin capabilities match interface requirements
```

## Wireless Settings
```toml
[wireless]
# Configuration for wireless-capable boards
# Note: Requires corresponding feature flags to be enabled

### WiFi Configuration
wifi_chip = "CYW43439"     # REQUIRED: WiFi chip model
                           # Examples: "CYW43439", "ESP32", "ESP8266"

wifi_antenna = "chip"      # REQUIRED: Antenna selection
                           # Values: "chip", "external", "both"

wifi_country = "US"        # OPTIONAL: Regulatory domain
                           # Default: "US" (FCC)

wifi_max_power = 19.5      # OPTIONAL: Max TX power (dBm)
                           # Default: Chip-specific maximum

wifi_region = 1            # OPTIONAL: WiFi region code
                           # 1: Worldwide, 2: Japan, etc.

### Bluetooth/BLE Configuration
ble_chip = "nRF52840"      # REQUIRED: BLE chip model
                           # Examples: "nRF52840", "ESP32", "CYW43439"

ble_antenna = "chip"       # REQUIRED: Antenna selection
                           # Values: same as wifi_antenna

ble_name = "CIRCUITPY"     # OPTIONAL: Default BLE device name
                           # Default: "CIRCUITPY"

ble_tx_power = 4           # OPTIONAL: TX power level (dBm)
                           # Range: -20 to +20 (chip-dependent)

### Advanced Wireless Settings
# OPTIONAL: WiFi channel configuration
wifi_channels = {
    allowed = "1-11",      # Allowed channels (varies by country)
    preferred = 6          # Preferred channel
}

# OPTIONAL: BLE service UUIDs
ble_services = [
    "0000180a-0000-1000-8000-00805f9b34fb",  # Device Information
    "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"   # UART
]

# OPTIONAL: Wireless coexistence configuration
coexistence = {
    wifi_ble = true,       # Allow simultaneous WiFi/BLE
    priority = "wifi"      # Default: "wifi" or "ble"
}

# OPTIONAL: Wireless debugging
debug = {
    wifi = false,          # Enable WiFi debug output
    ble = false            # Enable BLE debug output
}
```

## Example Configurations
See the [examples directory](./examples) for complete configuration examples for various boards.

## Validation
All configurations are validated against this schema during build.
