// SPDX-FileCopyrightText: 2026 fliperama86
//
// SPDX-License-Identifier: Unlicense
// From https://github.com/fliperama86/pico_hdmi v0.0.6, with the AVI
// InfoFrame and channel status from v0.0.19.
// CircuitPython uses negative H/V sync at the 640-wide timings.

#pragma once

#include <stdbool.h>
#include <stdint.h>

// One HSTX word per symbol: 2 guard band, 32 packet, 2 guard band.
#define HSTX_DATA_ISLAND_WORDS 36

// Data island packet: header and four subpackets, each with BCH parity.
typedef struct {
    uint8_t header[4];       // 3 bytes header + 1 byte BCH parity
    uint8_t subpacket[4][8]; // 4 subpackets, each 7 bytes + 1 byte BCH parity
} hstx_packet_t;

// Pre-encoded data island for HSTX (36 words)
typedef struct {
    uint32_t words[HSTX_DATA_ISLAND_WORDS];
} hstx_data_island_t;

// Audio sample structure
typedef struct {
    int16_t left;
    int16_t right;
} audio_sample_t;

void hstx_packet_init(hstx_packet_t *packet);
void hstx_packet_set_acr(hstx_packet_t *packet, uint32_t n, uint32_t cts);
void hstx_packet_set_audio_infoframe(hstx_packet_t *packet, uint32_t sample_rate, uint8_t channels, uint8_t bits_per_sample);
void hstx_packet_set_avi_infoframe(hstx_packet_t *packet, uint8_t vic, uint8_t pixel_repetition);
int hstx_packet_set_audio_samples(hstx_packet_t *packet, const audio_sample_t *samples, int num_samples, int frame_count);
void hstx_encode_data_island(hstx_data_island_t *out, const hstx_packet_t *packet, bool vsync_active, bool hsync_active);
