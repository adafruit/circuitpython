# MAX32650FTHR

[TODO]

## Onboard connectors & peripherals

[TODO]

## Product Resources

[TODO]

### Building for this board

To build for this board, ensure you are in the `ports/analog` directory and run the following command. Note that passing in the `-jN` flag, where N is the # of cores on your machine, can speed up compile times.

```
make BOARD=max32650fthr
```

### Flashing this board

To flash the board, run the following command if using the MAX32625PICO:

```
make BOARD=max32655fthr flash-msdk
```

If using Segger JLink, please run the following command instead:

```
make BOARD=max32655fthr flash-jlink
```
