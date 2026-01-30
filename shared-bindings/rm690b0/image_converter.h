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
    IMG_ERR_INVALID_FORMAT = -4,    // Invalid or unsupported format
    IMG_ERR_CORRUPTED_DATA = -5,    // Corrupted image data
    IMG_ERR_UNSUPPORTED = -6,       // Unsupported feature
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

// Convert 24-bit RGB to 16-bit RGB565
// Format: RRRR RGGG GGGB BBBB (5 bits R, 6 bits G, 5 bits B)
#define RGB888_TO_RGB565(r, g, b) \
    ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

// Extract components from RGB565
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
 * @return       Required buffer size in bytes
 */
static inline size_t img_rgb565_buffer_size(uint32_t width, uint32_t height) {
    return (size_t)width * height * 2;
}

/**
 * @brief Get error message string
 *
 * @param error Error code
 * @return      Human-readable error message
 */
const char *img_error_string(img_error_t error);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_RM690B0_IMAGE_CONVERTER_H
