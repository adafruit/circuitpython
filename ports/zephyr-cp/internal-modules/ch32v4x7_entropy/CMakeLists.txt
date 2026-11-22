# ch32v4x7_entropy Zephyr module build: the entropy driver, built when the
# devicetree enables an adafruit,ch32v4x7-adc-entropy node.

if(CONFIG_ENTROPY_CH32V4X7_ADC)
  set(circuitpython_root ${ZEPHYR_CURRENT_MODULE_DIR}/../../../..)
  zephyr_library()
  zephyr_library_sources(
    src/entropy_ch32v4x7_adc.c
    ${circuitpython_root}/lib/crypto-algorithms/sha256.c
  )
  zephyr_library_include_directories(${circuitpython_root})
endif()
