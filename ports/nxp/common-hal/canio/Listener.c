/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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


#include <math.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared/runtime/interrupt_char.h"

#include "common-hal/canio/__init__.h"
#include "common-hal/canio/Listener.h"
#include "shared-bindings/canio/Listener.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/safe_mode.h"

void set_filters(canio_listener_obj_t *self, size_t nmatch, canio_match_obj_t **matches) {
    return;
}

void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout) {
    return;
}

void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout) {
    return;
}


float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self) {
    return 0.0f;
}

void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self) {
    return;
}

int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self) {
    return 0;
}

mp_obj_t common_hal_canio_listener_receive(canio_listener_obj_t *self) {
    canio_message_obj_t *message = m_new_obj(canio_message_obj_t);

    return message;
}

void common_hal_canio_listener_deinit(canio_listener_obj_t *self) {
    return;
}
