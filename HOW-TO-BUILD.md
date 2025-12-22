# Build CircuitPython for Eliobot

Eliobot need 2 different builds of CircuitPython, one for that flash with repl ON and one for that flash with repl OFF.

Repl on for flashing from the web interface and repl off for flashing from the mass storage with Eliobot-Programmer.


1. Read the [CircuitPython BUILDING.md](BUILDING.md) guide and follow the instructions to build CircuitPython.

2. before the "make BOARD" step of the build process, go to "ports/espressif/" and run :

```bash
cd ports/espressif
```

```bash
./esp-idf/install.sh
```

after the installation, run :

```bash
. ./esp-idf/export.sh
```

(this step is needed every time you open a new terminal session)

3. go back to "ports/espressif/" and run :

```bash
make BOARD=elio_eliobot_s3
```

if the build fails, you may need to run :

```bash
pip3 install minify_html jsmin
```

if the build fails again, it might be because of the PATH variable of the esp-idf tools,
you can try to find the path that is missing and add it to the PATH variable of your system.

To make a clean build, you need to create a git tag and push it to the repository :

```bash
git tag -a <tag_name>
git push origin <tag_name>
```

example :

```bash
git tag -a 9.1.2
git push origin 9.1.2
```

If you want to keep using the same tag name, you need to delete the tag before creating a new one :

```bash
git tag -d <tag_name>
git push origin :refs/tags/<tag_name>

git tag -a <tag_name>
git push origin <tag_name>
```

example :

```bash
git tag -d 9.1.2
git push origin :refs/tags/9.1.2

git tag -a 9.1.2 -m "Circuitpython 9.1.2"
git push origin 9.1.2 
```

It will push the last commit you made to the repository with the tag name you specified.


# How to flash with repl ON 

go to supervisor/filesystem.c and change the line 137 that create the code.py file to :

```c
        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&vfs_fat->fatfs, "/main.py", "print(\"Hello World!\")\n");

        MAKE_FILE_WITH_OPTIONAL_CONTENTS(&vfs_fat->fatfs, "/boot.py", "import board\nimport storage\n\n# Attribution de l'ecriture : True = Mass Storage, False = REPL\nstorage.remount(\"/\", False)\n");
```

It will create a boot.py file that will remount the filesystem in REPL mode when the board is powered on.
Don't forget to commitand tag those changes before building the firmware to make a clean build.

# elio_eliobot specific files

## board.c

- Purpose: 
This file typically contains board-specific initialization code. It includes definitions for hardware setup, initialization routines for various peripherals (like GPIOs, UARTs, etc.), and board-specific configurations. This is where you'd set up things that are unique to the specific board your firmware will run on.

No modifications have been made to this file.

## mpconfigboard.h

Purpose: This header file contains macro definitions for microcontroller configurations specific to the board.
These include settings like the CPU frequency, available memory, and specific hardware feature enablement.

In this file, the following changes have been made:

We defined the following macros:

- MICROPY_HW_BOARD_NAME: This macro defines the name of the board.
- MICROPY_HW_MCU_NAME: This macro defines the name of the microcontroller.
- CIRCUITPY_DRIVE_LABEL: This macro defines the name of the drive that will be used to store files on the board.
- MICROPY_HW_NEOPIXEL: This macro defines the pin number of the neopixel LED on the board.
- DEFAULT_I2C_BUS_SCL/SDA: This macro defines the pin number of the I2C bus on the board.
- DEFAULT_SPI_BUS_SCK/MISO/MOSI: This macro defines the pin numbers of the SPI bus on the board.
- DEFAULT_UART_BUS_RX/TX: This macro defines the pin numbers of the UART bus on the board.

## mpconfigboard.mk

Purpose: This makefile is typically used for setting up board-specific flags and variables for the build process. It might set options that affect how the firmware is compiled and linked, such as compiler flags, source file locations, and definitions specific to the board. This helps in creating a firmware build that is optimized for your hardware.

In this file, the following changes have been made:

- USB_VID: This variable defines the USB vendor ID for the board.
- USB_PID: This variable defines the USB product ID for the board.
- USB_PRODUCT: This variable defines the USB product name for
the board.
- USB_MANUFACTURER: This variable defines the USB manufacturer name for the board.

- IDF_TARGET: This variable defines the target chip for the ESP-IDF build system.

- CIRCUITPY_ESP_FLASH_SIZE/MODE/FREQ: These variables define the flash size, mode, and frequency for the ESP32 chip.

- CIRCUITPY_ESP_PSRAM_SIZE/MODE/FREQ: These variables define the PSRAM size, mode, and frequency for the ESP32 chip.

- CIRCUITPY_BITBANG_NEOPIXEL: This variable defines whether the neopixel LED is controlled using bit-banging.

- CIRCUITPY_STAGE: This variable defines the stage of the build process.

- FROZEN_MPY_DIRS: These variables define the directories containing the frozen libraries to be included in the firmware.

## pins.c

Purpose: This file defines the pin layout for the microcontroller, mapping pin names to physical hardware pins.

## sdkconfig

Purpose: This file contains the configuration settings for the ESP-IDF build system. It defines various options related to the build process, such as compiler flags, feature enablement, and other build configurations.

In this file, the following changes have been made:

- CONFIG_ESPTOOLPY_FLASHSIZE_4MB: This option says that the flash size is 4MB.
- CONFIG_ESPTOOLPY_FLASHSIZE: This option sets the flashsize to 4MB.

- CONFIG_PARTITION_TABLE_CUSTOM_FILENAME: This option specifies the custom partition table filename.
- CONFIG_PARTITION_TABLE_FILENAME: This option specifies the partition table filename.

- CONFIG_LWIP_LOCAL_HOSTNAME: This option sets the local hostname for the device.

In this file we use a custom partition table because it left us some space for more frozen modules.
