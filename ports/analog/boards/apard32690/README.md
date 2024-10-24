# AD-APARD32690-SL

This board features the MAX32690, a dual-core ARM Cortex-M4/RISC-V MCU with 3MiB Flash, 1MiB SRAM. The board also has support for 10BASE-T1L Ethernet, Wifi, Bluetooth, USB, and Security via MAXQ1065. However, most of these features are not yet available for this CircuitPython port (USB will be added soon; others are currently unplanned).

## Onboard connectors & peripherals

This board comes in a form-factor similar to an Arduino Mega, enabling Arduino-style shield boards to be plugged in and evaluated with the MAX32690. This vastly opens up the options for easily plugging peripherals into the board, especially when combined with the two Pmod:tm: connectors, P8 (SPI) and P13 (I2C).

## Product Resources

For more info about AD-APARD32690-SL, visit our product webpages for datasheets, User Guides, Software, and Design Documents:

[AD-APARD32690-SL Product Webpage](https://www.analog.com/en/resources/evaluation-hardware-and-software/evaluation-boards-kits/ad-apard32690-sl.html)
[AD-APARD32690-SL User Guide](https://wiki.analog.com/resources/eval/user-guides/ad-apard32690-sl)

### Building for this board

To build for this board, ensure you are in the `ports/analog` directory and run the following command. Note that passing in the `-jN` flag, where N is the # of cores on your machine, can speed up compile times.

```
make BOARD=apard32690
```

### Flashing this board

To flash the board, run the following command if using the MAX32625PICO:

```
make BOARD=APARD flash-msdk
```

If using Segger JLink, please run the following command instead:

```
make BOARD=APARD flash-jlink
```
