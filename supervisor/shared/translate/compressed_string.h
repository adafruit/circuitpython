// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// The format of the compressed data is:
// - the size of the uncompressed string in UTF-8 bytes, encoded as a
//   (compress_max_length_bits)-bit number.  compress_max_length_bits is
//   computed during dictionary generation time, and is 8 for all
//   current translations except ru, which needs 9.  This length excludes
//   the trailing NUL, though notably decompress_length includes it.
//
// - followed by the huffman encoding of the symbols that make up the
//   string.  The trailing "\0" is not represented by a huffman code, but
//   is implied by the length.
//
// - Each symbol is coded with one of TRANSLATION_CLASSES canonical Huffman
//   tables, chosen by the class of the previously decoded byte: start of
//   string or space, lowercase, uppercase, digit, '%', other, or >= 0x80.
//   base_class() computes that seven-way class and class_map[] collapses it
//   to a table index; the generator picks how many tables pay for themselves.
//   The per-table code-length counts are rows of LENGTHS_ROW bytes in
//   lengths[], and the symbols of table n are values[values_offset[n]..].
//
// - Symbol values 0..0x7F are ASCII (with 1, 2 and 3 reserved for the
//   escapes below).  Values from 0x80 up to 0x80 + alphabet_size - 1 are the
//   most frequent non-ASCII characters used by the translation, renumbered
//   by decreasing frequency; alphabet[] maps them back to Unicode code
//   points.  Most translations fit their whole non-ASCII repertoire in the
//   alphabet.  Those that don't (ja, ko) keep the 32 to 80 most frequent
//   characters there and escape the rest: value 2 is followed by a
//   rare_index_bits-bit index into rare_chars[] (characters used more than
//   once), value 3 by a raw 16-bit code point (characters used once, which
//   are cheaper without a table entry).  All symbols are therefore 8 bits.
//   At present, no translation requires code points outside the BMP, so
//   this is adequate.
//
// - Symbol values from word_start to word_end stand for dictionary entries
//   in a dictionary of up to 256 - word_start entries.  The dictionary
//   entries are computed with a heuristic based on frequent substrings of 2
//   to 11 symbols.  These are called "words" but are not, grammatically
//   speaking, words.  They're just spans of symbols that frequently occur
//   together.  They are ordered shortest to longest.
//
// - dictionary entries are non-overlapping, and the _ending_ index of each
//   entry is stored in an array.  A count of words of each length, from
//   minlen to maxlen, is given in the array called wlencount.  From
//   this small array, the start and end of the N'th word can be
//   calculated by an efficient, small loop.  (A bit of time is traded
//   to reduce the size of this table indicating lengths)
//
// - Value 1 ('\1') is used to indicate that a QSTR number follows. the
//   QSTR is encoded as a fixed number of bits (translation_qstr_bits), e.g.,
//   10 bits if the highest core qstr is from 512 to 1023 inclusive.
//   (maketranslationdata uses a simple heuristic where any qstr >= 4
//   characters long may be encoded in this way; whether a given occurrence
//   is coded as a qstr or as characters is decided per occurrence by the
//   parser, which picks the tokenization of each string that takes the
//   fewest bits.)
//
// The "data" / "tail" construct is so that the struct's last member is a
// "flexible array".  However, the _only_ member is not permitted to be
// a flexible member, so we have to declare the first byte as a separate
// member of the structure.
//
// For translations where length needs 8 bits, this saves about 1.5
// bytes per string on average compared to a structure of {uint16_t,
// flexible array}, but is also future-proofed against strings with
// UTF-8 length above 256, with a savings of about 1.375 bytes per
// string.
typedef struct compressed_string {
    uint8_t data;
    const uint8_t tail[];
} const *mp_rom_error_text_t;

// Class of the previously decoded byte, which selects the Huffman table for
// the next symbol. Must match base_class() in py/maketranslationdata.py.
typedef enum {
    CLASS_START_OR_SPACE = 0,
    CLASS_LOWER = 1,
    CLASS_UPPER = 2,
    CLASS_DIGIT = 3,
    CLASS_OTHER = 4,
    CLASS_PERCENT = 5,
    CLASS_NON_ASCII = 6,
} translation_class_t;

// Symbol values with a special meaning; every other value is a character or a
// dictionary word. Must match py/maketranslationdata.py.
typedef enum {
    SYMBOL_QSTR = 1,        // followed by translation_qstr_bits bits of qstr index
    SYMBOL_RARE_INDEX = 2,  // followed by rare_index_bits bits of rare_chars[] index
    SYMBOL_RARE_RAW = 3,    // followed by a 16-bit code point
} translation_symbol_t;

// Return the compressed, translated version of a source string
// Usually, due to LTO, this is optimized into a load of a constant
// pointer.
// mp_rom_error_text_t MP_ERROR_TEXT(const char *c);
void serial_write_compressed(mp_rom_error_text_t compressed);
char *decompress(mp_rom_error_text_t compressed, char *decompressed);
uint16_t decompress_length(mp_rom_error_text_t compressed);
