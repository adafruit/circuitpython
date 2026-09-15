// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/shared/translate/translate.h"
#include "py/qstr.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef NO_QSTR
#include "genhdr/compressed_translations.generated.h"
#endif

#include "py/misc.h"
#include "py/mpprint.h"
#include "supervisor/shared/serial.h"

void serial_write_compressed(mp_rom_error_text_t compressed) {
    mp_printf(MP_PYTHON_PRINTER, "%S", compressed);
}

static void get_word(int n, const uint8_t **pos, const uint8_t **end) {
    int len = minlen;
    int i = 0;
    *pos = words;
    while (wlencount[i] <= n) {
        n -= wlencount[i];
        *pos += len * wlencount[i];
        i++;
        len++;
    }
    *pos += len * n;
    *end = *pos + len;
}

static void put_utf8(vstr_t *vstr, unsigned u) {
    if (word_start <= u && u <= word_end) {
        const uint8_t *pos, *end;
        get_word(u - word_start, &pos, &end);
        // note that at present, entries in the words table are
        // guaranteed not to represent words themselves, so this adds
        // at most 1 level of recursive call
        for (; pos < end; pos++) {
            put_utf8(vstr, *pos);
        }
        return;
    }
    // alphabet_size is a compile-time constant; the test folds away when it is 0.
    if (alphabet_size > 0 && u >= 0x80 && u < 0x80 + alphabet_size) {
        u = alphabet[u - 0x80];
    }
    vstr_add_char(vstr, u);
}

// The Huffman table used for the next symbol depends on the class of the last
// byte written. Classifying the last byte is the same as classifying the last
// code point: every byte of a multi-byte UTF-8 sequence is >= 0x80.
// This must match base_class() in py/maketranslationdata.py.
static translation_class_t base_class(uint8_t last) {
    if (last == ' ') {
        return CLASS_START_OR_SPACE;
    }
    if (last >= 'a' && last <= 'z') {
        return CLASS_LOWER;
    }
    if (last >= 'A' && last <= 'Z') {
        return CLASS_UPPER;
    }
    if (last >= '0' && last <= '9') {
        return CLASS_DIGIT;
    }
    if (last == '%') {
        return CLASS_PERCENT;
    }
    if (last >= 0x80) {
        return CLASS_NON_ASCII;
    }
    return CLASS_OTHER;
}

uint16_t decompress_length(mp_rom_error_text_t compressed) {
    #ifndef NO_QSTR
    #if (compress_max_length_bits <= 8)
    return 1 + (compressed->data >> (8 - compress_max_length_bits));
    #else
    return 1 + ((compressed->data * 256 + compressed->tail[0]) >> (16 - compress_max_length_bits));
    #endif
    #endif
}

typedef struct {
    const uint8_t *ptr;
    uint8_t bit;
} bitstream_state_t;

static bool next_bit(bitstream_state_t *st) {
    bool r = *st->ptr & st->bit;
    st->bit >>= 1;
    if (!st->bit) {
        st->bit = 0x80;
        st->ptr++;
    }
    return r;
}

static int get_nbits(bitstream_state_t *st, int n) {
    int r = 0;
    while (n--) {
        r = (r << 1) | next_bit(st);
    }
    return r;
}

// note: the vstr must be a fixed-buffer vstr that matches the decompressed length of the string
static void decompress_vstr(mp_rom_error_text_t compressed, vstr_t *decompressed) {
    bitstream_state_t b = {
        .ptr = &(compressed->data) + (compress_max_length_bits >> 3),
        .bit = 1 << (7 - ((compress_max_length_bits) & 0x7)),
    };

    size_t alloc = decompressed->alloc - 1;
    // Stop one early because the last byte is always NULL.
    for (; decompressed->len < alloc;) {
        // The start of a string is classed like a space.
        uint8_t last = decompressed->len ? (uint8_t)decompressed->buf[decompressed->len - 1] : ' ';
        uint8_t cls = class_map[base_class(last)];
        const uint8_t *len_row = lengths + cls * LENGTHS_ROW;

        uint32_t bits = 0;
        uint8_t bit_length = 0;
        uint32_t max_code = len_row[0];
        uint32_t searched_length = len_row[0];
        while (true) {
            bits = (bits << 1) | next_bit(&b);
            bit_length += 1;
            if (max_code > 0 && bits < max_code) {
                break;
            }
            max_code = (max_code << 1) + len_row[bit_length];
            searched_length += len_row[bit_length];
        }
        unsigned v = values[values_offset[cls] + searched_length + bits - max_code];
        if (v == SYMBOL_QSTR) {
            qstr q = get_nbits(&b, translation_qstr_bits);
            vstr_add_str(decompressed, qstr_str(q));
        } else if (rare_index_count > 0 && v == SYMBOL_RARE_INDEX) {
            // A non-ASCII character outside the dense alphabet, by table index.
            vstr_add_char(decompressed, rare_chars[get_nbits(&b, rare_index_bits)]);
        } else if (rare_raw_count > 0 && v == SYMBOL_RARE_RAW) {
            // A non-ASCII character used only once, as a raw code point.
            vstr_add_char(decompressed, get_nbits(&b, 16));
        } else {
            put_utf8(decompressed, v);
        }
    }
}


char *decompress(mp_rom_error_text_t compressed, char *decompressed) {
    vstr_t vstr;
    vstr_init_fixed_buf(&vstr, decompress_length(compressed), decompressed);
    decompress_vstr(compressed, &vstr);
    return vstr_null_terminated_str(&vstr);
}

#if CIRCUITPY_TRANSLATE_OBJECT == 1
#include "supervisor/shared/translate/translate_impl.h"
#endif
