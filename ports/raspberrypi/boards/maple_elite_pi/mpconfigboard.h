#define MICROPY_HW_BOARD_NAME "Maple Computing Elite-Pi"
#define MICROPY_HW_MCU_NAME "rp2040"

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO5, .sda = &pin_GPIO4}}
