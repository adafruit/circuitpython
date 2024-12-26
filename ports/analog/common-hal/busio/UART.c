/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"

#include "uart.h"
#include "UART.h"

typedef enum {
    BUSIO_UART_PARITY_NONE,
    BUSIO_UART_PARITY_EVEN,
    BUSIO_UART_PARITY_ODD
} busio_uart_parity_t;

// Construct an underlying UART object.
extern void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled);

extern void common_hal_busio_uart_deinit(busio_uart_obj_t *self);
extern bool common_hal_busio_uart_deinited(busio_uart_obj_t *self);

// Read characters. len is in characters NOT bytes!
extern size_t common_hal_busio_uart_read(busio_uart_obj_t *self,
    uint8_t *data, size_t len, int *errcode);

// Write characters. len is in characters NOT bytes!
extern size_t common_hal_busio_uart_write(busio_uart_obj_t *self,
    const uint8_t *data, size_t len, int *errcode);

extern uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self);
extern void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate);
extern mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self);
extern void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout);

extern uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self);
extern void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self);
extern bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self);

extern void common_hal_busio_uart_never_reset(busio_uart_obj_t *self);
