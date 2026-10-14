// SPDX-FileCopyrightText: 2026 fliperama86
//
// SPDX-License-Identifier: Unlicense
// From https://github.com/fliperama86/pico_hdmi v0.0.6, with the AVI
// InfoFrame and channel status from v0.0.19.
// CircuitPython uses negative H/V sync at the 640-wide timings.

#include "common-hal/picodvi/dvi_audio_packet.h"

#include <string.h>

// ============================================================================
// TERC4 Symbol Table (4-bit to 10-bit encoding)
// ============================================================================

static const uint16_t ter_c4[16] = {
    0b1010011100, // 0
    0b1001100011, // 1
    0b1011100100, // 2
    0b1011100010, // 3
    0b0101110001, // 4
    0b0100011110, // 5
    0b0110001110, // 6
    0b0100111100, // 7
    0b1011001100, // 8
    0b0100111001, // 9
    0b0110011100, // 10
    0b1011000110, // 11
    0b1010001110, // 12
    0b1001110001, // 13
    0b0101100011, // 14
    0b1011000011, // 15
};

#define GUARD_BAND_SYMBOL 0x133u // 0b0100110011

// ============================================================================
// BCH Encoding
// ============================================================================

static const uint8_t bch_table[256] = {
    0x00, 0xd9, 0xb5, 0x6c, 0x6d, 0xb4, 0xd8, 0x01, 0xda, 0x03, 0x6f, 0xb6, 0xb7, 0x6e, 0x02, 0xdb, 0xb3, 0x6a, 0x06,
    0xdf, 0xde, 0x07, 0x6b, 0xb2, 0x69, 0xb0, 0xdc, 0x05, 0x04, 0xdd, 0xb1, 0x68, 0x61, 0xb8, 0xd4, 0x0d, 0x0c, 0xd5,
    0xb9, 0x60, 0xbb, 0x62, 0x0e, 0xd7, 0xd6, 0x0f, 0x63, 0xba, 0xd2, 0x0b, 0x67, 0xbe, 0xbf, 0x66, 0x0a, 0xd3, 0x08,
    0xd1, 0xbd, 0x64, 0x65, 0xbc, 0xd0, 0x09, 0xc2, 0x1b, 0x77, 0xae, 0xaf, 0x76, 0x1a, 0xc3, 0x18, 0xc1, 0xad, 0x74,
    0x75, 0xac, 0xc0, 0x19, 0x71, 0xa8, 0xc4, 0x1d, 0x1c, 0xc5, 0xa9, 0x70, 0xab, 0x72, 0x1e, 0xc7, 0xc6, 0x1f, 0x73,
    0xaa, 0xa3, 0x7a, 0x16, 0xcf, 0xce, 0x17, 0x7b, 0xa2, 0x79, 0xa0, 0xcc, 0x15, 0x14, 0xcd, 0xa1, 0x78, 0x10, 0xc9,
    0xa5, 0x7c, 0x7d, 0xa4, 0xc8, 0x11, 0xca, 0x13, 0x7f, 0xa6, 0xa7, 0x7e, 0x12, 0xcb, 0x83, 0x5a, 0x36, 0xef, 0xee,
    0x37, 0x5b, 0x82, 0x59, 0x80, 0xec, 0x35, 0x34, 0xed, 0x81, 0x58, 0x30, 0xe9, 0x85, 0x5c, 0x5d, 0x84, 0xe8, 0x31,
    0xea, 0x33, 0x5f, 0x86, 0x87, 0x5e, 0x32, 0xeb, 0xe2, 0x3b, 0x57, 0x8e, 0x8f, 0x56, 0x3a, 0xe3, 0x38, 0xe1, 0x8d,
    0x54, 0x55, 0x8c, 0xe0, 0x39, 0x51, 0x88, 0xe4, 0x3d, 0x3c, 0xe5, 0x89, 0x50, 0x8b, 0x52, 0x3e, 0xe7, 0xe6, 0x3f,
    0x53, 0x8a, 0x41, 0x98, 0xf4, 0x2d, 0x2c, 0xf5, 0x99, 0x40, 0x9b, 0x42, 0x2e, 0xf7, 0xf6, 0x2f, 0x43, 0x9a, 0xf2,
    0x2b, 0x47, 0x9e, 0x9f, 0x46, 0x2a, 0xf3, 0x28, 0xf1, 0x9d, 0x44, 0x45, 0x9c, 0xf0, 0x29, 0x20, 0xf9, 0x95, 0x4c,
    0x4d, 0x94, 0xf8, 0x21, 0xfa, 0x23, 0x4f, 0x96, 0x97, 0x4e, 0x22, 0xfb, 0x93, 0x4a, 0x26, 0xff, 0xfe, 0x27, 0x4b,
    0x92, 0x49, 0x90, 0xfc, 0x25, 0x24, 0xfd, 0x91, 0x48,
};

static const uint8_t parity_table[32] = {0x96, 0x69, 0x69, 0x96, 0x69, 0x96, 0x96, 0x69, 0x69, 0x96, 0x96,
                                         0x69, 0x96, 0x69, 0x69, 0x96, 0x69, 0x96, 0x96, 0x69, 0x96, 0x69,
                                         0x69, 0x96, 0x96, 0x69, 0x69, 0x96, 0x69, 0x96, 0x96, 0x69};

static inline bool compute_parity(uint8_t v) {
    return (parity_table[v / 8] >> (v % 8)) & 1;
}

static inline bool compute_parity3(uint8_t a, uint8_t b, uint8_t c) {
    return compute_parity(a) ^ compute_parity(b) ^ compute_parity(c);
}

static uint8_t encode_bch_3(const uint8_t *p) {
    uint8_t v = bch_table[p[0]];
    v = bch_table[p[1] ^ v];
    v = bch_table[p[2] ^ v];
    return v;
}

static uint8_t encode_bch_7(const uint8_t *p) {
    uint8_t v = bch_table[p[0]];
    v = bch_table[p[1] ^ v];
    v = bch_table[p[2] ^ v];
    v = bch_table[p[3] ^ v];
    v = bch_table[p[4] ^ v];
    v = bch_table[p[5] ^ v];
    v = bch_table[p[6] ^ v];
    return v;
}

static void compute_header_parity(hstx_packet_t *p) {
    p->header[3] = encode_bch_3(p->header);
}

static void compute_subpacket_parity(hstx_packet_t *p, int idx) {
    p->subpacket[idx][7] = encode_bch_7(p->subpacket[idx]);
}

static void compute_all_parity(hstx_packet_t *p) {
    compute_header_parity(p);
    for (int i = 0; i < 4; i++) {
        compute_subpacket_parity(p, i);
    }
}

static void compute_infoframe_checksum(hstx_packet_t *p) {
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += p->header[i];
    }
    int len = p->header[2] + 1;
    for (int j = 0; j < 4 && len > 0; j++) {
        for (int i = 0; i < 7 && len > 0; i++, len--) {
            sum += p->subpacket[j][i];
        }
    }
    p->subpacket[0][0] = (uint8_t)(-sum);
}

// ============================================================================
// Public API
// ============================================================================

void hstx_packet_init(hstx_packet_t *packet) {
    memset(packet, 0, sizeof(hstx_packet_t));
}

void hstx_packet_set_acr(hstx_packet_t *packet, uint32_t n, uint32_t cts) {
    hstx_packet_init(packet);
    packet->header[0] = 0x01;
    packet->header[1] = 0x00;
    packet->header[2] = 0x00;
    compute_header_parity(packet);

    packet->subpacket[0][0] = 0;
    packet->subpacket[0][1] = (cts >> 16) & 0x0F;
    packet->subpacket[0][2] = (cts >> 8) & 0xFF;
    packet->subpacket[0][3] = cts & 0xFF;
    packet->subpacket[0][4] = (n >> 16) & 0x0F;
    packet->subpacket[0][5] = (n >> 8) & 0xFF;
    packet->subpacket[0][6] = n & 0xFF;
    compute_subpacket_parity(packet, 0);

    memcpy(packet->subpacket[1], packet->subpacket[0], 8);
    memcpy(packet->subpacket[2], packet->subpacket[0], 8);
    memcpy(packet->subpacket[3], packet->subpacket[0], 8);
}

void hstx_packet_set_audio_infoframe(hstx_packet_t *packet, uint32_t sample_rate, uint8_t channels,
    uint8_t bits_per_sample) {
    hstx_packet_init(packet);
    packet->header[0] = 0x84;
    packet->header[1] = 0x01;
    packet->header[2] = 0x0A;

    uint8_t cc = (channels - 1) & 0x07;
    uint8_t ct = 0x01;
    uint8_t ss =
        (bits_per_sample == 16) ? 0x01 : (bits_per_sample == 20 ? 0x02 : (bits_per_sample == 24 ? 0x03 : 0x00));
    uint8_t sf = (sample_rate == 32000) ? 0x01 : (sample_rate == 44100 ? 0x02 : (sample_rate == 48000 ? 0x03 : 0x00));

    packet->subpacket[0][1] = cc | (ct << 4);
    packet->subpacket[0][2] = ss | (sf << 2);
    packet->subpacket[0][3] = 0x00;
    packet->subpacket[0][4] = 0x00;
    packet->subpacket[0][5] = 0x00;

    compute_infoframe_checksum(packet);
    compute_all_parity(packet);
}

void hstx_packet_set_avi_infoframe(hstx_packet_t *packet, uint8_t vic, uint8_t pixel_repetition) {
    hstx_packet_init(packet);
    packet->header[0] = 0x82;
    packet->header[1] = 0x02;
    packet->header[2] = 0x0D;

    packet->subpacket[0][1] = 0x10; // A=1: active format information is valid.
    packet->subpacket[0][2] = 0x18; // 4:3 picture, same active format.
    // PB3: Q (bits 3:2) = 10 declares full-range RGB quantization.
    packet->subpacket[0][3] = 0x08;
    packet->subpacket[0][4] = vic;
    packet->subpacket[0][5] = pixel_repetition & 0x0F;

    compute_infoframe_checksum(packet);
    compute_all_parity(packet);
}

int hstx_packet_set_audio_samples(hstx_packet_t *packet, const audio_sample_t *samples, int num_samples,
    int frame_count) {
    hstx_packet_init(packet);
    if (num_samples < 1) {
        num_samples = 1;
    }
    if (num_samples > 4) {
        num_samples = 4;
    }

    uint8_t sample_present = (1 << num_samples) - 1;
    uint8_t b_flags = 0;

    int temp_frame_count = frame_count;
    for (int i = 0; i < num_samples; i++) {
        if (temp_frame_count == 0) {
            b_flags |= (1 << i);
        }
        temp_frame_count = (temp_frame_count + 1) % 192;
    }

    packet->header[0] = 0x02;
    packet->header[1] = sample_present;
    packet->header[2] = b_flags << 4;
    compute_header_parity(packet);

    for (int i = 0; i < num_samples; i++) {
        uint8_t *d = packet->subpacket[i];
        int16_t left = samples[i].left;
        int16_t right = samples[i].right;

        d[0] = 0x00;
        d[1] = left & 0xFF;
        d[2] = (left >> 8) & 0xFF;
        d[3] = 0x00;
        d[4] = right & 0xFF;
        d[5] = (right >> 8) & 0xFF;

        // IEC 60958 byte 0 = 0x04 (copy permitted), byte 3 = 0x02 (48 kHz).
        int frame = (frame_count + i) % 192;
        uint8_t status = frame / 8 == 0 ? 0x04 : (frame / 8 == 3 ? 0x02 : 0);
        uint8_t c = (status >> (frame % 8)) & 1;
        bool p_left = compute_parity3(d[1], d[2], c);
        bool p_right = compute_parity3(d[4], d[5], c);
        d[6] = (c << 2) | (p_left << 3) | (c << 6) | (p_right << 7);
        compute_subpacket_parity(packet, i);
    }

    return temp_frame_count;
}

static inline uint32_t make_hstx_word(uint16_t lane0, uint16_t lane1, uint16_t lane2) {
    return (lane0 & 0x3FF) | ((lane1 & 0x3FF) << 10) | ((lane2 & 0x3FF) << 20);
}

static void encode_header_to_lane0(const hstx_packet_t *packet, uint16_t *lane0, int hv, bool first_packet) {
    int hv1 = hv | 0x08;
    if (!first_packet) {
        hv = hv1;
    }

    int idx = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t h = packet->header[i];
        lane0[idx++] = ter_c4[((h << 2) & 4) | hv];
        hv = hv1;
        lane0[idx++] = ter_c4[((h << 1) & 4) | hv];
        lane0[idx++] = ter_c4[(h & 4) | hv];
        lane0[idx++] = ter_c4[((h >> 1) & 4) | hv];
        lane0[idx++] = ter_c4[((h >> 2) & 4) | hv];
        lane0[idx++] = ter_c4[((h >> 3) & 4) | hv];
        lane0[idx++] = ter_c4[((h >> 4) & 4) | hv];
        lane0[idx++] = ter_c4[((h >> 5) & 4) | hv];
    }
}

static void encode_subpackets_to_lanes(const hstx_packet_t *packet, uint16_t *lane1, uint16_t *lane2) {
    for (int i = 0; i < 8; i++) {
        uint32_t v = ((uint32_t)packet->subpacket[0][i] << 0) | ((uint32_t)packet->subpacket[1][i] << 8) | ((uint32_t)packet->subpacket[2][i] << 16) |
            ((uint32_t)packet->subpacket[3][i] << 24);
        uint32_t t = (v ^ (v >> 7)) & 0x00aa00aa;
        v = v ^ t ^ (t << 7);
        t = (v ^ (v >> 14)) & 0x0000cccc;
        v = v ^ t ^ (t << 14);

        lane1[(i * 4) + 0] = ter_c4[(v >> 0) & 0xF];
        lane1[(i * 4) + 1] = ter_c4[(v >> 16) & 0xF];
        lane1[(i * 4) + 2] = ter_c4[(v >> 4) & 0xF];
        lane1[(i * 4) + 3] = ter_c4[(v >> 20) & 0xF];

        lane2[(i * 4) + 0] = ter_c4[(v >> 8) & 0xF];
        lane2[(i * 4) + 1] = ter_c4[(v >> 24) & 0xF];
        lane2[(i * 4) + 2] = ter_c4[(v >> 12) & 0xF];
        lane2[(i * 4) + 3] = ter_c4[(v >> 28) & 0xF];
    }
}

void hstx_encode_data_island(hstx_data_island_t *out, const hstx_packet_t *packet, bool vsync_active, bool hsync_active) {
    // vsync_active/hsync_active indicate pulse region, not wire level.
    // The hv field encodes the actual wire bits (bit1=vsync, bit0=hsync),
    // inverted because the 640-wide modes use negative sync.
    int hv = (vsync_active ? 0 : 2) | (hsync_active ? 0 : 1);
    uint16_t lane0[32];
    uint16_t lane1[32];
    uint16_t lane2[32];

    encode_header_to_lane0(packet, lane0, hv, true);
    encode_subpackets_to_lanes(packet, lane1, lane2);

    uint16_t gb_lane0 = ter_c4[0xC | hv];
    uint32_t guard_word = make_hstx_word(gb_lane0, GUARD_BAND_SYMBOL, GUARD_BAND_SYMBOL);

    out->words[0] = guard_word;
    out->words[1] = guard_word;
    for (int i = 0; i < 32; i++) {
        out->words[i + 2] = make_hstx_word(lane0[i], lane1[i], lane2[i]);
    }
    out->words[34] = guard_word;
    out->words[35] = guard_word;
}
