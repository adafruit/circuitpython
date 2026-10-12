# neopixel

A Zephyr module that transmits NeoPixel (WS2812-style) LED data on a pin
chosen at runtime. Callers hand it pixel bytes in the strip's byte order and
a package pin, which they must not hold claimed during the call; the SoC
implementation picks the transmit hardware per call and allocates it, with
the pin, from the [iobroker](../iobroker/README.md) module for the duration
of the transfer. On nRF that is a PWM instance's DMA sequence playback, with
a bit-bang fallback when every instance is busy or none can
reach the pin (nRF54L PWMs drive only their own power domain's pads),
except where the CPU's GPIO access is too slow to bit-bang (nRF54L P0, whose
pins raise `ValueError`).

## Status

Implemented for nRF SoCs. On other SoCs the module compiles but
`neopixel_send()` returns `-ENOSYS`.

## Source layout

```
include/neopixel/neopixel.h    # public API
src/
  neopixel.c                   # -ENOSYS stubs when no SoC implementation
  nordic/
    nrf/
      neopixel_nrf.c           # PWM sequence playback + bit-bang fallback
```

Each vendor adds a `src/<vendor>/<soc>/` directory with an implementation of
`neopixel_send()`, its `NEOPIXEL_PATTERN_BUFFER_SIZE()` formula in the public
header, a corresponding `zephyr_library_sources_ifdef()` line in
`CMakeLists.txt`, and its SoC condition in the `#if` that guards the stub in
`src/neopixel.c`.

## Enabling

The module is registered by the CircuitPython Zephyr application
(`ports/zephyr-cp/CMakeLists.txt`) via `ZEPHYR_EXTRA_MODULES`, so it works
without west manifest changes. It depends on the iobroker module for
peripheral allocation and package pin resolution.

## Public API

See `include/neopixel/neopixel.h`. `NEOPIXEL_PATTERN_BUFFER_SIZE()` tells the
caller how large a pattern buffer (the waveform built from the pixel bytes) a
transfer of a given length needs, as a constant expression, so the caller
decides where that memory lives; `neopixel_send()` transmits. The caller
configures the pin as an output driving low before the call and paces
successive calls so that the strip's reset (latch) period elapses between
them.
