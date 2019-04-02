/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/busio/UART.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate.h"
#include "mpconfigboard.h"
#include "py/runtime.h"

#ifdef CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#define BOARD_I2C (defined(DEFAULT_I2C_BUS_SDA) && defined(DEFAULT_I2C_BUS_SCL))
#define BOARD_SPI (defined(DEFAULT_SPI_BUS_SCK) && defined(DEFAULT_SPI_BUS_MISO) && defined(DEFAULT_SPI_BUS_MOSI))
#define BOARD_UART (defined(DEFAULT_UART_BUS_RX) && defined(DEFAULT_UART_BUS_TX))

#if BOARD_I2C
STATIC mp_obj_t i2c_singleton = NULL;

mp_obj_t board_i2c(void) {

    if (i2c_singleton == NULL) {
        busio_i2c_obj_t *self = m_new_ll_obj(busio_i2c_obj_t);
        self->base.type = &busio_i2c_type;

        assert_pin_free(DEFAULT_I2C_BUS_SDA);
        assert_pin_free(DEFAULT_I2C_BUS_SCL);
        common_hal_busio_i2c_construct(self, DEFAULT_I2C_BUS_SCL, DEFAULT_I2C_BUS_SDA, 400000, 0);
        i2c_singleton = (mp_obj_t)self;
    }
    return i2c_singleton;
}
#else
mp_obj_t board_i2c(void) {
    mp_raise_NotImplementedError(translate("No default I2C bus"));
    return NULL;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_i2c_obj, board_i2c);

#if BOARD_SPI
// Statically allocate the SPI object so it can live past the end of the heap and into the next VM.
// That way it can be used by built-in FourWire displays and be accessible through board.SPI().
STATIC busio_spi_obj_t spi_obj;
STATIC mp_obj_t spi_singleton = NULL;

mp_obj_t board_spi(void) {
    if (spi_singleton == NULL) {
        busio_spi_obj_t *self = &spi_obj;
        self->base.type = &busio_spi_type;
        assert_pin_free(DEFAULT_SPI_BUS_SCK);
        assert_pin_free(DEFAULT_SPI_BUS_MOSI);
        assert_pin_free(DEFAULT_SPI_BUS_MISO);
        const mcu_pin_obj_t* clock = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_SCK);
        const mcu_pin_obj_t* mosi = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MOSI);
        const mcu_pin_obj_t* miso = MP_OBJ_TO_PTR(DEFAULT_SPI_BUS_MISO);
        common_hal_busio_spi_construct(self, clock, mosi, miso);
        spi_singleton = (mp_obj_t)self;
    }
    return spi_singleton;
}
#else
mp_obj_t board_spi(void) {
    mp_raise_NotImplementedError(translate("No default SPI bus"));
    return NULL;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_spi_obj, board_spi);

#if BOARD_UART
STATIC mp_obj_t uart_singleton = NULL;

mp_obj_t board_uart(void) {
    if (uart_singleton == NULL) {
        busio_uart_obj_t *self = m_new_ll_obj(busio_uart_obj_t);
        self->base.type = &busio_uart_type;

        assert_pin_free(DEFAULT_UART_BUS_RX);
        assert_pin_free(DEFAULT_UART_BUS_TX);

        const mcu_pin_obj_t* rx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_RX);
        const mcu_pin_obj_t* tx = MP_OBJ_TO_PTR(DEFAULT_UART_BUS_TX);

        common_hal_busio_uart_construct(self, tx, rx, 9600, 8, PARITY_NONE, 1, 1000, 64);
        uart_singleton = (mp_obj_t)self;
    }
    return uart_singleton;
}
#else
mp_obj_t board_uart(void) {
    mp_raise_NotImplementedError(translate("No default UART bus"));
    return NULL;
}
#endif
MP_DEFINE_CONST_FUN_OBJ_0(board_uart_obj, board_uart);


void reset_board_busses(void) {
#if BOARD_I2C
    i2c_singleton = NULL;
#endif
#if BOARD_SPI
    bool display_using_spi = false;
    #ifdef CIRCUITPY_DISPLAYIO
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].fourwire_bus.bus == spi_singleton) {
            display_using_spi = true;
            break;
        }
    }
    #endif
    if (!display_using_spi) {
        spi_singleton = NULL;
    }
#endif
#if BOARD_UART
    uart_singleton = NULL;
#endif
}
