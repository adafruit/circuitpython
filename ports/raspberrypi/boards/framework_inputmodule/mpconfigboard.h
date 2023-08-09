#define MICROPY_HW_BOARD_NAME "Framework Laptop 16 Keyboard (CircuitPython)"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO27, .sda = &pin_GPIO26}}
#define DEFAULT_I2C_BUS_SCL         (&pin_GPIO27)
#define DEFAULT_I2C_BUS_SDA         (&pin_GPIO26)
// pin_GPIO0 sleep
// pin_GPIO28 INTB
// pin_GPIO29 SDB
