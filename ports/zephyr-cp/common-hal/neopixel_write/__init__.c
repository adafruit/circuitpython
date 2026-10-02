// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// neopixel_write on top of the neopixel Zephyr module
// (internal-modules/neopixel): this file waits for any previous write to
// finish, provides the pattern buffer the module needs, and maps its errors.
// The waveform itself is produced by the module's per-SoC implementation.

#include <errno.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include <iobroker/iobroker.h>
#include <neopixel/neopixel.h>

#include "py/mpstate.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/neopixel_write/__init__.h"
#include "common-hal/neopixel_write/__init__.h"
#include "supervisor/port.h"

LOG_MODULE_REGISTER(neopixel_write);

// Earliest raw tick at which the next write may start. The strip latches the
// previous data while its input stays low for the reset period (a few tens
// of microseconds); a raw tick is about a millisecond, so 4 ticks is ample.
static uint64_t next_start_raw_ticks = 0;

// Pattern buffer for the module: writes of up to STACK_PIXELS RGB pixels
// (or 3/4 as many RGBW pixels: the buffer is sized in pixel bytes) use a
// stack array (uint32_t for the 4-byte alignment the module requires). As
// in ports/nordic: the status NeoPixel is written between VM instantiations,
// when the heap is not available, so it must never allocate, and 24 pixels
// also covers Circuit Playground's 10 and the common 12- and 16-pixel rings.
// Longer strips use a heap buffer that is kept between writes and grown when
// a longer strip appears. Only user code with such a strip reaches the heap
// path, with the VM running, so the raising allocator is fine there.
#define STACK_PIXELS 24
#define STACK_PATTERN_BUFFER_SIZE NEOPIXEL_PATTERN_BUFFER_SIZE(3 * STACK_PIXELS)

static size_t pattern_buffer_heap_size = 0;

void neopixel_write_reset(void) {
    MP_STATE_VM(neopixel_write_pattern_buffer_heap) = NULL;
    pattern_buffer_heap_size = 0;
}

// Claim the pin as a GPIO again after neopixel_send() and put it back in the
// configuration the DigitalInOut recorded (normally an output driven low).
static void reclaim_pin(const digitalio_digitalinout_obj_t *digitalinout) {
    const struct device *port;
    gpio_pin_t number;
    if (iobroker_gpio_allocate(digitalinout->pin->package_pin, &port, &number) < 0) {
        LOG_WRN("could not reclaim the pin after a write");
        return;
    }
    gpio_flags_t flags;
    if (digitalinout->direction == DIRECTION_OUTPUT) {
        flags = digitalinout->value ? GPIO_OUTPUT_HIGH : GPIO_OUTPUT_LOW;
        if (digitalinout->drive_mode == DRIVE_MODE_OPEN_DRAIN) {
            flags |= GPIO_OPEN_DRAIN;
        }
    } else {
        flags = GPIO_INPUT;
        if (digitalinout->pull == PULL_UP) {
            flags |= GPIO_PULL_UP;
        } else if (digitalinout->pull == PULL_DOWN) {
            flags |= GPIO_PULL_DOWN;
        }
    }
    (void)gpio_pin_configure(port, number, flags);
}

void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *digitalinout,
    uint8_t *pixels, uint32_t num_bytes) {
    size_t pattern_buffer_size = NEOPIXEL_PATTERN_BUFFER_SIZE(num_bytes);
    uint32_t stack_pattern_buffer[(STACK_PATTERN_BUFFER_SIZE + 3) / sizeof(uint32_t)];
    void *pattern_buffer;
    if (pattern_buffer_size <= sizeof(stack_pattern_buffer)) {
        pattern_buffer = stack_pattern_buffer;
        pattern_buffer_size = sizeof(stack_pattern_buffer);
    } else {
        if (pattern_buffer_heap_size < pattern_buffer_size) {
            MP_STATE_VM(neopixel_write_pattern_buffer_heap) =
                m_realloc(MP_STATE_VM(neopixel_write_pattern_buffer_heap), pattern_buffer_size);
            pattern_buffer_heap_size = pattern_buffer_size;
        }
        pattern_buffer = MP_STATE_VM(neopixel_write_pattern_buffer_heap);
        pattern_buffer_size = pattern_buffer_heap_size;
    }

    while (port_get_raw_ticks(NULL) < next_start_raw_ticks) {
    }

    // neopixel_send() allocates the transmit hardware and the pin itself, so
    // the DigitalInOut's GPIO claim is released for the frame.
    (void)iobroker_gpio_release(digitalinout->port, digitalinout->number);
    int ret = neopixel_send(digitalinout->pin->package_pin, pixels, num_bytes,
        pattern_buffer, pattern_buffer_size);
    reclaim_pin(digitalinout);
    next_start_raw_ticks = port_get_raw_ticks(NULL) + 4;

    if (ret == -EINVAL) {
        // No transmit hardware can drive this pad, neither a PWM instance nor
        // bit-bang (such as nRF54L P0): a wrong pin choice, so say so.
        raise_ValueError_invalid_pin();
    }
    if (ret < 0) {
        // A runtime condition (a frame preempted on every resend, a transfer
        // error) the status LED must survive, since it calls this from the
        // supervisor: skip the write.
        LOG_WRN("write skipped (%d)", ret);
    }
}

MP_REGISTER_ROOT_POINTER(uint8_t * neopixel_write_pattern_buffer_heap);
