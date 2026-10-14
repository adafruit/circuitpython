# pwm_emul Zephyr module build: the emulated PWM driver, built when the
# devicetree enables an adafruit,pwm-emul node (native_sim).

if(CONFIG_PWM_ADAFRUIT_EMUL)
  zephyr_library()
  zephyr_library_sources(src/pwm_emul.c)
endif()
