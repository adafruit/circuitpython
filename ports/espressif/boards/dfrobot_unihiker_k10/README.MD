# CircuitPython Port: DFRobot UNIHIKER K10 (ESP32-S3)

**Status**: ✅ Functional — All major hardware features working:
- PMIC (TCA9555 I2C expander: backlight, camera reset, buttons, audio amp)
- Display ILI9341 (240×320, oriented correctly via MADCTL=0x88)
- NeoPixel (single RGB LED)
- Camera GC2145 (2MP, QVGA via espcamera)
- Audio I2S (MAX98357 speaker amp, 440Hz tone)
- Buttons A/B (PMIC-controlled, NOT GPIOs)
- Sensors: SC7A20H (accelerometer), LTR303ALS (light), AHT20 (temp/humidity)
- PSRAM 8MB OPI @ 80MHz ✅
- Flash 16MB DIO @ 80MHz ✅
- USB Serial/JTAG console ✅
- Frozen Python libraries (27 modules pre-loaded)
