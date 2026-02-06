# CircuitPython RM690B0 Driver - Changes for v2.0

## Version

- Release: `2.0.0`
- Release date: `2026-02-06`
- Type: `BREAKING CHANGES`
- Board scope: `waveshare_esp32_s3_amoled_241`
- Last updated: `2026-02-06`

## 1. Executive Summary

Version `2.0.0` replaces the standalone `rm690b0` module with the standard CircuitPython display stack:

- `qspibus` for QSPI panel transport
- `displayio` and `busdisplay` for scene and refresh lifecycle
- RM690B0 panel init sequence integrated with `BusDisplay`
- `sdioio` instead of custom `sdcardio` implementation

This change aligns the board with CircuitPython upstream architecture expectations and makes display code compatible with standard ecosystem libraries.

Important:
- Existing applications that use `import rm690b0` must migrate.
- Existing applications that use legacy `sdcardio` APIs on this board must migrate to `sdioio`.

Quick migration example:

```python
# OLD (v1.x)
import rm690b0
rm690b0.init_display()
rm690b0.fill_color(rm690b0.RED)
rm690b0.swap_buffers()

# NEW (v2.0)
import board
import displayio
import qspibus
from adafruit_rm690b0 import RM690B0

displayio.release_displays()
bus = qspibus.QSPIBus(
    clock=board.LCD_CLK,
    data0=board.LCD_D0,
    data1=board.LCD_D1,
    data2=board.LCD_D2,
    data3=board.LCD_D3,
    cs=board.LCD_CS,
    reset=board.LCD_RESET,
    frequency=40_000_000,
)
panel = RM690B0(bus, width=600, height=450)
display = displayio.Display(panel, width=600, height=450)
```

## 2. Breaking Changes

### 2.1 Standalone `rm690b0` module removed

The old standalone API was removed from firmware build and bindings.

Removed behavior:
- `import rm690b0`
- imperative drawing calls from `rm690b0.*`
- standalone framebuffer and swap model

Required replacement:
- `displayio` scene graph (`Group`, `TileGrid`, `Bitmap`, `Palette`)
- `qspibus.QSPIBus` transport
- RM690B0 panel class using `BusDisplay`

### 2.2 Custom `sdcardio` path removed

Legacy board-specific SD path was removed.

Required replacement:
- `sdioio.SDCard(...)`
- `storage.VfsFat` + `storage.mount`

Example:

```python
import board
import sdioio

sd = sdioio.SDCard(
    clock=board.SD_CLK,
    command=board.SD_MOSI,
    data=[board.SD_MISO],
    frequency=20_000_000,
)
```

### 2.3 API comparison

| Legacy v1.x API | v2.0 replacement |
|---|---|
| `rm690b0.init_display()` | `qspibus.QSPIBus(...)` + panel init + `displayio.Display(...)` |
| `rm690b0.fill_color(color)` | Full-screen `Bitmap` + single-color `Palette` |
| `rm690b0.fill_rect(...)` | Rect bitmap/tile in `displayio.Group` |
| `rm690b0.line(...)` | `vectorio` or bitmap draw helper |
| `rm690b0.circle(...)` | `vectorio.Circle` |
| `rm690b0.text(...)` | `adafruit_display_text.label` or `terminalio` |
| `rm690b0.blit_bmp(...)` | `adafruit_imageload` + displayio |
| `rm690b0.blit_jpeg(...)` | `jpegio` path (future integration for accelerated flow) |
| `rm690b0.swap_buffers()` | `display.refresh()` or auto-refresh |
| custom `sdcardio` usage | `sdioio.SDCard(...)` |

## 3. New Features

### 3.1 `qspibus` module

New module added as QSPI display transport analog to `fourwire`.

Main points:
- explicit command/data path used by `busdisplay`
- stable pin ownership lifecycle for display stack
- context manager support
- deinit support with panel sleep handling for safer reruns

Location:
- `shared-bindings/qspibus/`
- `shared-module/qspibus/`
- `ports/espressif/common-hal/qspibus/`

### 3.2 RM690B0 displayio panel integration

RM690B0 panel is now initialized and refreshed through `BusDisplay`.

Main points:
- panel init sequence migrated to displayio flow
- geometry offsets supported (`colstart`, `rowstart`)
- tested color rendering path on real hardware
- integrated cleanup sequence for reruns

### 3.3 Standard displayio compatibility

Applications can now use regular displayio ecosystem primitives and libraries.

Examples:
- `displayio.Bitmap`, `displayio.Palette`, `displayio.Group`
- `adafruit_display_text`
- `vectorio`
- `adafruit_imageload`

### 3.4 Backward-compatible board aliases

Canonical aliases are now generic `LCD_*` names. Compatibility aliases are still exposed for existing scripts.

Examples:
- canonical: `LCD_CLK`, `LCD_D0`, `LCD_D1`, `LCD_D2`, `LCD_D3`, `LCD_CS`, `LCD_RESET`
- compatibility: `QSPI_CLK`, `QSPI_D0`, `QSPI_D1`, `QSPI_D2`, `QSPI_D3`, `QSPI_CS`, `AMOLED_RESET`

## 4. Removed Code

### 4.1 Core components removed

| Component | Approx lines removed | Reason |
|---|---:|---|
| `shared-bindings/rm690b0/*` | 656 | Standalone module removed |
| `ports/espressif/common-hal/rm690b0/RM690B0.c` and `.h` | 3788 | Replaced by displayio panel path |
| `ports/espressif/common-hal/rm690b0/fonts/*.h` (7 files) | 5449 | Built-in standalone font pack removed |
| custom board `sdcardio` path | ~501 | Replaced by standard `sdioio` |

Totals:
- conservative template baseline previously tracked: `~4963` lines
- full audit (including font headers): `9893` lines removed

Audit reference:
- `../ws-esp32-s3-amoled-241/docs/CODE_REMOVAL_AUDIT.md`

## 5. Retained Code

The following components were intentionally kept:

| Component | Approx lines | Location | Purpose |
|---|---:|---|---|
| `esp_lcd_rm690b0.c` | 342 | `ports/espressif/common-hal/rm690b0/` | Vendor RM690B0 init commands |
| `esp_lcd_rm690b0.h` | 109 | `ports/espressif/common-hal/rm690b0/` | Panel definitions |
| `esp_jpeg.c` | 295 | `ports/espressif/common-hal/rm690b0/esp_jpeg/` | JPEG decode path for future integration |
| `esp_jpeg.h` | 72 | `ports/espressif/common-hal/rm690b0/esp_jpeg/` | JPEG API definitions |

Rationale:
- Vendor init command source remains required for panel bring-up.
- JPEG path is retained for planned `jpegio` integration work.

## 6. Configuration Changes

### 6.1 Board config (`mpconfigboard.mk`)

Active settings in `ports/espressif/boards/waveshare_esp32_s3_amoled_241/mpconfigboard.mk`:

```makefile
CIRCUITPY_QSPIBUS = 1
CIRCUITPY_SDIOIO = 1

# Disabled legacy path
CIRCUITPY_SDCARDIO = 0
CIRCUITPY_SDCARDIO_ESPRESSIF = 0
```

### 6.2 Build graph cleanup

Build graph references for standalone `rm690b0` module were removed from make config where applicable.

Impact:
- standalone module symbols are no longer present in the built firmware
- display pipeline now resolves through `qspibus` + `displayio`

### 6.3 Pin naming updates

Board pin API normalized to generic LCD naming while preserving compatibility aliases.

Examples:
- old naming style: `RM690B0_QSPI_*`
- current naming style: `LCD_*`

## 7. Architecture Changes

### 7.1 Old architecture

```text
Application
  -> rm690b0 standalone module
    -> board-specific draw and swap path
      -> RM690B0 hardware
```

### 7.2 New architecture

```text
Application
  -> displayio scene graph
    -> BusDisplay RM690B0 panel driver
      -> qspibus transport
        -> RM690B0 hardware
```

### 7.3 Benefits

- standardized API model for CircuitPython users
- better upstream maintainability
- easier reuse of ecosystem display libraries
- clear separation of concerns between scene, panel, and transport
- future optimization path without changing user-level API

## 8. Performance Impact

### 8.1 Summary

`v2.0` prioritizes correctness and upstream-compatible architecture. Full-screen refresh is slower than historical standalone path, but integration stability and API compatibility are achieved.

### 8.2 Measured metrics

Measured on Waveshare ESP32-S3 AMOLED 2.41 with current test scripts:

| Operation | Legacy v1.x (historical) | v2.0 baseline | v2.0 optimized in Phase 5 | Notes |
|---|---:|---:|---:|---|
| Full screen fill (600x450) | ~25 ms (historical standalone estimate) | 360.499 ms | 341.250 ms | still above `<100 ms` target |
| Partial update (100x100) | n/a | 62.584 ms | 52.916 ms | improved after qspibus transfer updates |
| Multi-element scene | n/a | 59.700 ms | 51.500 ms | improved after batching/tuning |

Additional Phase 1 SD metrics:
- write throughput: around `180 KB/s`
- read throughput (`readinto`): up to `~1.13 MB/s` at 40 MHz

### 8.3 Optimization plan for v2.1

Planned focus areas:
- deeper refresh batching and dirty-region behavior tuning
- panel/bus refresh path profiling under scene-heavy workloads
- optional pipeline improvements for high-frequency full-screen refresh use cases
- incremental validation against stability constraints (no reboot/deinit regressions)

## 9. Migration Guide

Migration details are documented in:
- `../ws-esp32-s3-amoled-241/docs/MIGRATION_GUIDE.md`

What it includes:
- old-to-new API mapping
- init sequence migration examples
- common pitfalls and compatibility notes
- cleanup best practices for reliable rerun behavior

## 10. Testing

Hardware validation used the phase test suite in:
- `../ws-esp32-s3-amoled-241/examples/tests/`

Core tests:
- `test_phase1_sdioio.py`
- `test_phase2_qspibus.py`
- `test_phase3_displayio.py`
- `test_phase4_integration.py`

Optional benchmark:
- `benchmark_phase5_displayio.py`

Observed status in this cycle:
- Phase 1: pass
- Phase 2: pass
- Phase 3: pass (display colors and rectangle visible)
- Phase 4: pass (full stack integration)

Testing guide:
- `../ws-esp32-s3-amoled-241/docs/TESTING_GUIDE.md`

## 11. Known Issues

1. Full-screen displayio refresh is slower than standalone historical path.
2. JPEG acceleration path is retained but not fully integrated with a finalized `jpegio` workflow.
3. Legacy built-in font bundle from standalone module is removed; applications should use `terminalio` or external font assets.
4. For robust reruns, cleanup order matters (`screen black` -> `displayio.release_displays()` -> `qspibus.deinit()`).

## 12. Credits

- Original board and RM690B0 integration work: project author and contributors
- Refactor toward upstream displayio architecture: project maintainer workstream
- CircuitPython architecture feedback and direction: `@tannewt`
- Hardware validation and regression checks: Waveshare ESP32-S3 AMOLED 2.41 test runs

## References

- `../ws-esp32-s3-amoled-241/docs/IMPLEMENTATION_PLAN.md`
- `../ws-esp32-s3-amoled-241/docs/IMPLEMENTATION_PLAN_SUMMARY.md`
- `../ws-esp32-s3-amoled-241/docs/TECHNICAL_NOTES.md`
- `../ws-esp32-s3-amoled-241/docs/CODE_REMOVAL_AUDIT.md`
- `../ws-esp32-s3-amoled-241/docs/MIGRATION_GUIDE.md`
- `../ws-esp32-s3-amoled-241/docs/TESTING_GUIDE.md`

## Support

For issue reporting and debugging context, include:
- board model and firmware build date
- full serial log from test run
- phase script name and whether display output matched expected behavior
