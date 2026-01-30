// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_RM690B0_IMAGE_CONVERTER_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_RM690B0_IMAGE_CONVERTER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// =============================================================================
// Error Codes
// =============================================================================

typedef enum {
    IMG_OK = 0,                     // Success
    IMG_ERR_NULL_POINTER = -1,      // NULL pointer passed
    IMG_ERR_INVALID_SIZE = -2,      // Invalid image dimensions
    IMG_ERR_BUFFER_TOO_SMALL = -3,  // Output buffer too small
    IMG_ERR_INVALID_FORMAT = -4,    // Malformed data (bad signature, corrupted headers)
    IMG_ERR_CORRUPTED_DATA = -5,    // Corrupted image data (truncated, checksum mismatch)
    IMG_ERR_UNSUPPORTED = -6,       // Valid format but unsupported feature (bit depth, compression)
    IMG_ERR_OUT_OF_MEMORY = -7,     // Memory allocation failed
    IMG_ERR_IO_ERROR = -8           // File I/O error
} img_error_t;

// =============================================================================
// Image Information Structure
// =============================================================================

typedef struct {
    uint32_t width;         // Image width in pixels
    uint32_t height;        // Image height in pixels
    uint32_t data_size;     // Size of RGB565 data in bytes
    uint8_t bit_depth;      // Original bit depth
    uint8_t channels;       // Number of color channels
    bool has_alpha;         // Whether image has alpha channel
} img_info_t;

// =============================================================================
// RGB Conversion Macros
// =============================================================================

// Convert 24-bit RGB888 to 16-bit RGB565
// Format: RRRR RGGG GGGB BBBB (5 bits R, 6 bits G, 5 bits B)
//
// This is an intentional lossy conversion:
//   - Red:   8 bits → 5 bits (loses 3 LSBs, 0xF8 mask keeps bits 7:3)
//   - Green: 8 bits → 6 bits (loses 2 LSBs, 0xFC mask keeps bits 7:2)
//   - Blue:  8 bits → 5 bits (loses 3 LSBs, right-shift discards bits 2:0)
//
// Color precision is reduced from 16.7M colors (24-bit) to 65K colors (16-bit).
#define RGB888_TO_RGB565(r, g, b) \
    ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

// Extract components from RGB565 back to 8-bit (with zero-filled LSBs)
// Note: Extracted values have lower bits set to 0, not interpolated
#define RGB565_TO_R(rgb565) (((rgb565) >> 8) & 0xF8)
#define RGB565_TO_G(rgb565) (((rgb565) >> 3) & 0xFC)
#define RGB565_TO_B(rgb565) (((rgb565) << 3) & 0xF8)

// Note: RGB565 data is stored in little-endian byte order:
//   byte[0] = low byte (GGGBBBBB)
//   byte[1] = high byte (RRRRRGGG)
// This matches the byte order expected by most display controllers.



// =============================================================================
// BMP Format Converter
// =============================================================================

/**
 * @brief Convert BMP image to RGB565 buffer
 *
 * Supports uncompressed BMP files with 16, 24, or 32 bits per pixel.
 * Handles bottom-up and top-down orientations.
 *
 * @param bmp_data      Pointer to BMP file data (including headers)
 * @param bmp_size      Size of BMP data in bytes
 * @param rgb565_buffer Output buffer for RGB565 data
 * @param buffer_size   Size of output buffer in bytes
 * @param info          Optional pointer to receive image info (can be NULL)
 * @return              IMG_OK on success, error code otherwise
 */
img_error_t img_bmp_to_rgb565(
    const uint8_t *bmp_data,
    size_t bmp_size,
    uint8_t *rgb565_buffer,
    size_t buffer_size,
    img_info_t *info
    );

/**
 * @brief Parse BMP header and extract image information
 *
 * @param bmp_data Pointer to BMP file data
 * @param bmp_size Size of BMP data in bytes
 * @param info     Pointer to receive image info
 * @return         IMG_OK on success, error code otherwise
 */
img_error_t img_bmp_parse_header(
    const uint8_t *bmp_data,
    size_t bmp_size,
    img_info_t *info
    );

// =============================================================================
// JPEG Format Converter
// =============================================================================

/**
 * @brief Convert JPEG image to RGB565 buffer
 *
 * Uses the ESP-IDF `esp_jpeg` decoder on supported ESP32-class ports. Builds without the esp_jpeg headers will
 * return IMG_ERR_UNSUPPORTED for JPEG requests.
 *
 * @param jpg_data      Pointer to JPEG file data
 * @param jpg_size      Size of JPEG data in bytes
 * @param rgb565_buffer Output buffer for RGB565 data
 * @param buffer_size   Size of output buffer in bytes
 * @param info          Optional pointer to receive image info (can be NULL)
 * @return              IMG_OK on success, IMG_ERR_UNSUPPORTED if not compiled with JPEG support
 */

img_error_t img_jpg_to_rgb565(
    const uint8_t *jpg_data,
    size_t jpg_size,
    uint8_t *rgb565_buffer,
    size_t buffer_size,
    img_info_t *info
    );

/**
 * @brief Parse JPEG header and extract image information
 *
 * @param jpg_data Pointer to JPEG file data
 * @param jpg_size Size of JPEG data in bytes
 * @param info     Pointer to receive image info
 * @return         IMG_OK on success, error code otherwise
 */
img_error_t img_jpg_parse_header(
    const uint8_t *jpg_data,
    size_t jpg_size,
    img_info_t *info
    );

// =============================================================================
// Utility Functions
// =============================================================================

/**
 * @brief Calculate required buffer size for RGB565 data
 *
 * @param width  Image width in pixels
 * @param height Image height in pixels
 * @return       Required buffer size in bytes, or SIZE_MAX on overflow
 *
 * @note Caller should check if return value == SIZE_MAX to detect overflow.
 *       RGB565 uses 2 bytes per pixel, so buffer size = width * height * 2.
 */
static inline size_t img_rgb565_buffer_size(uint32_t width, uint32_t height) {
    // Check for overflow: width * height must not exceed SIZE_MAX / 2
    // First check width * height overflow
    if (width != 0 && height > SIZE_MAX / width) {
        return SIZE_MAX;  // Overflow in width * height
    }
    size_t pixel_count = (size_t)width * height;

    // Check for overflow when multiplying by 2 (bytes per pixel)
    if (pixel_count > SIZE_MAX / 2) {
        return SIZE_MAX;  // Overflow in pixel_count * 2
    }

    return pixel_count * 2;
}

/**
 * @brief Get error message string
 *
 * @param error Error code
 * @return      Human-readable error message
 */
const char *img_error_string(img_error_t error);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_RM690B0_IMAGE_CONVERTER_H
