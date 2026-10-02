# neopixel Zephyr module build.
#
# The core is always compiled so that callers can include
# <neopixel/neopixel.h> and call the module on every SoC. SoC-specific
# transmit implementations are added under src/<vendor>/<soc>/; without one
# the send function reports -ENOSYS.

zephyr_library()
# SoC-agnostic core: -ENOSYS stubs when no SoC implementation is built.
zephyr_library_sources(src/neopixel.c)

# SoC implementations live in src/<vendor>/<soc>/. The nRF implementation
# covers every nRF SoC with a PWM peripheral, so the <soc> level is the nRF
# family. Other vendors add their own directories and a corresponding
# zephyr_library_sources_ifdef() line here.
zephyr_library_sources_ifdef(CONFIG_SOC_FAMILY_NORDIC_NRF
  src/nordic/nrf/neopixel_nrf.c
)

# Public header.
zephyr_include_directories(include)
