#define MICROPY_HW_BOARD_NAME "Adafruit SproutSense M0"
#define MICROPY_HW_MCU_NAME "samd21e18"

#define MICROPY_HW_NEOPIXEL (&pin_PA01)
#define MICROPY_HW_NEOPIXEL_COUNT (1)

#define IGNORE_PIN_PA00     1

#define IGNORE_PIN_PA05     1
#define IGNORE_PIN_PA06     1
#define IGNORE_PIN_PA07     1
#define IGNORE_PIN_PA08     1
#define IGNORE_PIN_PA09     1
#define IGNORE_PIN_PA10     1
#define IGNORE_PIN_PA11     1

#define IGNORE_PIN_PA14     1
#define IGNORE_PIN_PA15     1

#define IGNORE_PIN_PA18     1
#define IGNORE_PIN_PA19     1

#define IGNORE_PIN_PA22     1
#define IGNORE_PIN_PA23     1
// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define IGNORE_PIN_PA27     1
#define IGNORE_PIN_PA28     1
#define IGNORE_PIN_PA30     1
#define IGNORE_PIN_PA31     1

#define DEFAULT_I2C_BUS_SCL (&pin_PA17)
#define DEFAULT_I2C_BUS_SDA (&pin_PA16)
