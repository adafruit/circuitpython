// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2019 Jim Mussared
// SPDX-FileCopyrightText: Copyright (c) 2020 Dan Halbert for Adafruit Industries LLC
//
// SPDX-License-Identifier: MIT

// CIRCUITPY-CHANGE: API and implementation thoroughly reworked
// No attempt to have atomic operations. Add guards if atomicity required.

#include <string.h>

#include "py/misc.h"
#include "ringbuf.h"

bool ringbuf_init(ringbuf_t *r, uint8_t *buf, size_t size) {
    r->buf = buf;
    r->size = size;
    r->used = 0;
    r->next_read = 0;
    r->next_write = 0;
    return r->buf != NULL;
}

// Dynamic initialization. This should be accessible from a root pointer..
bool ringbuf_alloc(ringbuf_t *r, size_t size) {
    bool result = ringbuf_init(r, m_malloc(size), size);
    return result;
}

void ringbuf_deinit(ringbuf_t *r) {
    // Free buf by doing nothing and letting gc take care of it. If the VM has finished already,
    // this will be safe.
    r->buf = (uint8_t *)NULL;
    r->size = 0;
    ringbuf_clear(r);
}

size_t ringbuf_size(ringbuf_t *r) {
    return r->size;
}

// Return -1 if buffer is empty, else return byte fetched.
int ringbuf_get(ringbuf_t *r) {
    if (r->used < 1) {
        return -1;
    }
    uint8_t v = r->buf[r->next_read];
    r->next_read++;
    if (r->next_read >= r->size) {
        r->next_read = 0;
    }
    r->used--;
    return v;
}

int ringbuf_get16(ringbuf_t *r) {
    if (r->used < 2) {
        return -1;
    }
    int high_byte = ringbuf_get(r);
    int low_byte = ringbuf_get(r);
    return (high_byte << 8) | low_byte;
}

// Return -1 if no room in buffer, else return 0.
int ringbuf_put(ringbuf_t *r, uint8_t v) {
    if (r->used >= r->size) {
        return -1;
    }
    r->buf[r->next_write] = v;
    r->next_write++;
    if (r->next_write >= r->size) {
        r->next_write = 0;
    }
    r->used++;
    return 0;
}

int ringbuf_put16(ringbuf_t *r, uint16_t v) {
    if (r->size - r->used < 2) {
        return -1;
    }
    ringbuf_put(r, (v >> 8) & 0xff);
    ringbuf_put(r, v & 0xff);
    return 0;
}

void ringbuf_clear(ringbuf_t *r) {
    r->next_write = 0;
    r->next_read = 0;
    r->used = 0;
}

// Number of free slots that can be written.
size_t ringbuf_num_empty(ringbuf_t *r) {
    return r->size - r->used;
}

// Number of bytes available to read.
size_t ringbuf_num_filled(ringbuf_t *r) {
    return r->used;
}

// If the ring buffer fills up, not all bytes will be written.
// Returns how many bytes were successfully written.
size_t ringbuf_put_n(ringbuf_t *r, const uint8_t *buf, size_t bufsize) {
    size_t n = MIN(bufsize, r->size - r->used);
    if (n == 0) {
        return 0;
    }
    // At most two copies: up to the end of buf, then from its start.
    size_t first = MIN(n, r->size - r->next_write);
    memcpy(r->buf + r->next_write, buf, first);
    memcpy(r->buf, buf + first, n - first);
    r->next_write += n;
    if (r->next_write >= r->size) {
        r->next_write -= r->size;
    }
    r->used += n;
    return n;
}

// Returns how many bytes were fetched.
size_t ringbuf_get_n(ringbuf_t *r, uint8_t *buf, size_t bufsize) {
    size_t n = MIN(bufsize, r->used);
    if (n == 0) {
        return 0;
    }
    size_t first = MIN(n, r->size - r->next_read);
    memcpy(buf, r->buf + r->next_read, first);
    memcpy(buf + first, r->buf, n - first);
    return ringbuf_discard_n(r, n);
}

// Drops up to n of the oldest bytes. Returns how many were dropped.
size_t ringbuf_discard_n(ringbuf_t *r, size_t n) {
    n = MIN(n, r->used);
    r->next_read += n;
    if (r->next_read >= r->size) {
        r->next_read -= r->size;
    }
    r->used -= n;
    return n;
}
