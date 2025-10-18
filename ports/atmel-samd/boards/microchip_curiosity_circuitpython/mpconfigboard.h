
#pragma once

#define MICROPY_HW_BOARD_NAME "Microchip Curiosity CircuitPython"
#define MICROPY_HW_MCU_NAME "same51j20"
#define CIRCUITPY_MCU_FAMILY samd51

#define MICROPY_HW_LED_STATUS (&pin_PB23)
#define MICROPY_HW_NEOPIXEL   (&pin_PB22)

#define BOARD_HAS_CRYSTAL 1

// USB is always used internally so skip the pin objects for it.
#define IGNORE_PIN_PA24     1
#define IGNORE_PIN_PA25     1

#define DEFAULT_I2C_BUS_SCL (&pin_PB30)
#define DEFAULT_I2C_BUS_SDA (&pin_PB31)

#define CIRCUITPY_BOARD_SPI         (3)
// These correspond to the CIRCUITPY_BOARD_BUS_SINGLETON definitions in pins.c
#define CIRCUITPY_BOARD_SPI_PIN     { \
        {.clock = &pin_PB03, .mosi = &pin_PB02, .miso = &pin_PB00}, /*board.SPI()*/ \
        {.clock = &pin_PA05, .mosi = &pin_PA04, .miso = NULL},      /*board.LCD_SPI()*/ \
        {.clock = &pin_PA17, .mosi = &pin_PA16, .miso = &pin_PA18}, /*board.SD_SPI()*/ \
}

#define DEFAULT_CAN_BUS_TX (&pin_PB12)
#define DEFAULT_CAN_BUS_RX (&pin_PB13)
#define DEFAULT_CAN_BUS_STDBY (&pin_PB17)

#define DEFAULT_UART_BUS_RX (&pin_PA23)
#define DEFAULT_UART_BUS_TX (&pin_PA22)
