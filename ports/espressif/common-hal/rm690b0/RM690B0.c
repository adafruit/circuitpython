// SPDX-FileCopyrightText: Copyright (c) 2025 Przemyslaw Patrick Socha
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/rm690b0/RM690B0.h"
#include "common-hal/rm690b0/RM690B0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "py/mperrno.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_heap_caps.h"
#include "esp_rom_sys.h"

static const char *TAG = "rm690b0";

#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_io.h"
#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_vendor.h"
#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_ops.h"
#include "esp-idf/components/esp_lcd/include/esp_lcd_panel_commands.h"
#include "esp_lcd_rm690b0.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include "esp_attr.h"
#include "esp_jpeg.h"
#include "fonts/rm690b0_font_8x8.h"
#include "fonts/rm690b0_font_16x16.h"
#include "fonts/rm690b0_font_16x24.h"
#include "fonts/rm690b0_font_24x24.h"
#include "fonts/rm690b0_font_24x32.h"
#include "fonts/rm690b0_font_32x32.h"
#include "fonts/rm690b0_font_32x48.h"

#if !defined(CIRCUITPY_RM690B0_QSPI_CS) || !defined(CIRCUITPY_RM690B0_QSPI_CLK) || \
    !defined(CIRCUITPY_RM690B0_QSPI_D0) || !defined(CIRCUITPY_RM690B0_QSPI_D1) || \
    !defined(CIRCUITPY_RM690B0_QSPI_D2) || !defined(CIRCUITPY_RM690B0_QSPI_D3) || \
    !defined(CIRCUITPY_RM690B0_RESET) || !defined(CIRCUITPY_RM690B0_WIDTH) || \
    !defined(CIRCUITPY_RM690B0_HEIGHT) || !defined(CIRCUITPY_RM690B0_BITS_PER_PIXEL)
#error "Board must define CIRCUITPY_RM690B0_* macros to describe the RM690B0 hardware"
#endif

#ifndef CIRCUITPY_RM690B0_POWER
#define CIRCUITPY_RM690B0_POWER (NULL)
#endif

#ifndef CIRCUITPY_RM690B0_POWER_ON_LEVEL
#define CIRCUITPY_RM690B0_POWER_ON_LEVEL (1)  // GPIO level: 1=high, 0=low
#endif

#ifndef CIRCUITPY_RM690B0_USE_QSPI
#define CIRCUITPY_RM690B0_USE_QSPI (0)
#endif

#ifndef CIRCUITPY_RM690B0_X_GAP
#define CIRCUITPY_RM690B0_X_GAP (0)
#endif

#ifndef CIRCUITPY_RM690B0_Y_GAP
#define CIRCUITPY_RM690B0_Y_GAP (16)
#endif

#ifndef CIRCUITPY_RM690B0_PIXEL_CLOCK_HZ
#define CIRCUITPY_RM690B0_PIXEL_CLOCK_HZ (80 * 1000 * 1000)
#endif

#define PIN_GPIO(pin_obj) ((pin_obj) == NULL ? (gpio_num_t)GPIO_NUM_NC : (gpio_num_t)(pin_obj)->number)

#define LCD_CS_PIN          PIN_GPIO(CIRCUITPY_RM690B0_QSPI_CS)
#define LCD_SCK_PIN         PIN_GPIO(CIRCUITPY_RM690B0_QSPI_CLK)
#define LCD_D0_PIN          PIN_GPIO(CIRCUITPY_RM690B0_QSPI_D0)
#define LCD_D1_PIN          PIN_GPIO(CIRCUITPY_RM690B0_QSPI_D1)
#define LCD_D2_PIN          PIN_GPIO(CIRCUITPY_RM690B0_QSPI_D2)
#define LCD_D3_PIN          PIN_GPIO(CIRCUITPY_RM690B0_QSPI_D3)
#define LCD_RST_PIN         PIN_GPIO(CIRCUITPY_RM690B0_RESET)
#define LCD_PWR_PIN         PIN_GPIO(CIRCUITPY_RM690B0_POWER)
#define LCD_PWR_ON_LEVEL    (CIRCUITPY_RM690B0_POWER_ON_LEVEL)

#define LCD_H_RES           (CIRCUITPY_RM690B0_WIDTH)
#define LCD_V_RES           (CIRCUITPY_RM690B0_HEIGHT)
#define LCD_BIT_PER_PIXEL   (CIRCUITPY_RM690B0_BITS_PER_PIXEL)
#define LCD_USE_QSPI        (CIRCUITPY_RM690B0_USE_QSPI)
#define LCD_PIXEL_CLOCK_HZ  (CIRCUITPY_RM690B0_PIXEL_CLOCK_HZ)

#define RGB565_SWAP_GB(c) (__builtin_bswap16(c))

#define RM690B0_OPCODE_WRITE_CMD   (0x02U)

#define RM690B0_PANEL_WIDTH          LCD_H_RES
#define RM690B0_PANEL_HEIGHT         LCD_V_RES
#define RM690B0_X_GAP                (CIRCUITPY_RM690B0_X_GAP)
#define RM690B0_Y_GAP                (CIRCUITPY_RM690B0_Y_GAP)
#define RM690B0_MAX_CHUNK_ROWS       (24)
#define RM690B0_MAX_CHUNK_PIXELS     (LCD_H_RES * RM690B0_MAX_CHUNK_ROWS)
#define RM690B0_MAX_DIAMETER         ((RM690B0_PANEL_WIDTH * 2) + 1)
#define RM690B0_PANEL_IO_QUEUE_DEPTH (10)

#define RM690B0_PENDING_BUFFER_FRAMEBUFFER   (0xFF)
#define RM690B0_PENDING_BUFFER_ALLOC         (0xFE)
#define RM690B0_PENDING_BUFFER_TEMP          (0xFD)

// Built-in font identifiers (must match shared-bindings docs)
#define RM690B0_FONT_8x8_MONO       (0)
#define RM690B0_FONT_16x16_MONO     (1)
#define RM690B0_FONT_16x24_MONO     (2)
#define RM690B0_FONT_24x24_MONO     (3)
#define RM690B0_FONT_24x32_MONO     (4)
#define RM690B0_FONT_32x32_MONO     (5)
#define RM690B0_FONT_32x48_MONO     (6)

// Macro to check if display is initialized
#define CHECK_INITIALIZED() \
    do { \
        if (!self->initialized) { \
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Display not initialized. Call init_display() first")); \
            return; \
        } \
    } while (0)

// Internal implementation structure
typedef struct {
    mp_int_t top;
    mp_int_t row_count;
    int16_t *left;
    int16_t *right;
} rm690b0_span_accumulator_t;

typedef struct {
    size_t head;
    size_t tail;
    size_t count;
    uint8_t ids[RM690B0_PANEL_IO_QUEUE_DEPTH];
} rm690b0_dma_pending_list_t;

typedef struct rm690b0_impl {
    esp_lcd_panel_io_handle_t io_handle;
    esp_lcd_panel_handle_t panel_handle;
    bool bus_initialized;
    uint16_t *chunk_buffers[2];
    size_t chunk_buffer_pixels;
    uint16_t *framebuffer;
    size_t framebuffer_pixels;
    uint16_t *framebuffer_front;
    bool double_buffered;
    bool dirty_region_valid;
    mp_int_t dirty_x;
    mp_int_t dirty_y;
    mp_int_t dirty_w;
    mp_int_t dirty_h;
    SemaphoreHandle_t transfer_done_sem;
    bool dma_buffer_in_use[2];
    bool dma_alloc_buffer_in_use;
    size_t dma_inflight;
    rm690b0_dma_pending_list_t dma_pending;
    int16_t *circle_span_cache;
    size_t circle_span_capacity;
} rm690b0_impl_t;
typedef struct {
    rm690b0_rm690b0_obj_t *self;
    rm690b0_impl_t *impl;
    mp_int_t origin_x;
    mp_int_t origin_y;
    mp_int_t clip_x;
    mp_int_t clip_y;
    mp_int_t clip_w;
    mp_int_t clip_h;
    bool rotation_zero;
} rm690b0_jpeg_draw_ctx_t;


static portMUX_TYPE rm690b0_spinlock = portMUX_INITIALIZER_UNLOCKED;

static inline void rm690b0_dma_pending_init(rm690b0_dma_pending_list_t *list) {
    list->head = 0;
    list->tail = 0;
    list->count = 0;
}

static inline void rm690b0_dma_pending_push(rm690b0_dma_pending_list_t *list, uint8_t id) {
    portENTER_CRITICAL(&rm690b0_spinlock);
    list->ids[list->tail] = id;
    list->tail = (list->tail + 1) % RM690B0_PANEL_IO_QUEUE_DEPTH;
    list->count++;
    portEXIT_CRITICAL(&rm690b0_spinlock);
}

static inline uint8_t rm690b0_dma_pending_pop(rm690b0_dma_pending_list_t *list) {
    portENTER_CRITICAL(&rm690b0_spinlock);
    uint8_t id = list->ids[list->head];
    list->head = (list->head + 1) % RM690B0_PANEL_IO_QUEUE_DEPTH;
    list->count--;
    portEXIT_CRITICAL(&rm690b0_spinlock);
    return id;
}

static inline void rm690b0_wait_for_dma_completion(rm690b0_impl_t *impl) {
    if (impl->dma_inflight == 0) {
        return;
    }

    if (impl->transfer_done_sem) {
        if (xSemaphoreTake(impl->transfer_done_sem, pdMS_TO_TICKS(1000)) != pdTRUE) {
            ESP_LOGE("RM690B0", "DMA wait timeout! Halting to prevent memory corruption.");
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA transfer timed out - hardware requires reset"));
            return;
        }
    } else {
        // Fallback - should not happen because semaphore allocation is mandatory
        esp_rom_delay_us(50);
    }

    if (impl->dma_inflight > 0) {
        impl->dma_inflight--;
    }

    if (impl->dma_pending.count > 0) {
        uint8_t id = rm690b0_dma_pending_pop(&impl->dma_pending);
        if (id < 2) {
            impl->dma_buffer_in_use[id] = false;
        } else if (id == RM690B0_PENDING_BUFFER_ALLOC) {
            impl->dma_alloc_buffer_in_use = false;
        }
        // RM690B0_PENDING_BUFFER_TEMP is explicitly ignored - it's locally managed
    }

}

static inline void rm690b0_wait_for_dma_slot(rm690b0_impl_t *impl) {
    while (impl->dma_inflight >= RM690B0_PANEL_IO_QUEUE_DEPTH) {
        rm690b0_wait_for_dma_completion(impl);
    }
}

static inline void rm690b0_wait_for_all_dma(rm690b0_impl_t *impl) {
    while (impl->dma_inflight > 0) {
        rm690b0_wait_for_dma_completion(impl);
    }
}

// (rm690b0_text_state_t typedefs removed – not used yet)

// Forward declarations for functions used by font rendering
static void mark_dirty_region(rm690b0_impl_t *impl, mp_int_t x, mp_int_t y, mp_int_t w, mp_int_t h);
static esp_err_t rm690b0_flush_region(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, bool skip_final_delay);
static void rm690b0_fill_rect_framebuffer(rm690b0_impl_t *impl,
    mp_int_t bx, mp_int_t by, mp_int_t bw, mp_int_t bh, uint16_t swapped_color);

// Callback for LCD IO transfer completion
static bool IRAM_ATTR rm690b0_on_color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    rm690b0_impl_t *impl = (rm690b0_impl_t *)user_ctx;
    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(impl->transfer_done_sem, &high_task_awoken);
    return high_task_awoken == pdTRUE;
}



static inline void rm690b0_span_update(rm690b0_span_accumulator_t *acc, mp_int_t row_y, mp_int_t x_val) {
    mp_int_t idx = row_y - acc->top;
    if (idx < 0 || idx >= acc->row_count) {
        return;
    }
    if (x_val < acc->left[idx]) {
        acc->left[idx] = (int16_t)x_val;
    }
    if (x_val > acc->right[idx]) {
        acc->right[idx] = (int16_t)x_val;
    }
}

static inline int16_t *rm690b0_acquire_span_cache(rm690b0_impl_t *impl, size_t needed_rows) {
    if (impl == NULL || needed_rows == 0) {
        return NULL;
    }

    if (impl->circle_span_capacity < needed_rows) {
        size_t new_capacity = needed_rows;
        size_t total_entries = new_capacity * 2;
        int16_t *new_cache = (int16_t *)heap_caps_realloc(
            impl->circle_span_cache,
            total_entries * sizeof(int16_t),
            MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (new_cache == NULL) {
            return NULL;
        }
        impl->circle_span_cache = new_cache;
        impl->circle_span_capacity = new_capacity;
    }
    return impl->circle_span_cache;
}

static inline void rm690b0_fill_span_fast(uint16_t *dest, size_t span_width, uint16_t color) {
    if (span_width == 0) {
        return;
    }

    if (((uintptr_t)dest & 0x2) != 0) {
        *dest++ = color;
        span_width--;
        if (span_width == 0) {
            return;
        }
    }

    while (span_width >= 2) {
        dest[0] = color;
        dest[1] = color;
        dest += 2;
        span_width -= 2;
    }

    if (span_width & 1) {
        *dest = color;
    }
}

static bool map_rect_for_rotation(const rm690b0_rm690b0_obj_t *self,
    mp_int_t *x, mp_int_t *y,
    mp_int_t *width, mp_int_t *height) {
    mp_int_t rx = *x;
    mp_int_t ry = *y;
    mp_int_t rw = *width;
    mp_int_t rh = *height;

    switch (self->rotation) {
        case 0:
            break;
        case 90:
            *x = RM690B0_PANEL_WIDTH - (ry + rh);
            *y = rx;
            *width = rh;
            *height = rw;
            break;
        case 180:
            *x = RM690B0_PANEL_WIDTH - (rx + rw);
            *y = RM690B0_PANEL_HEIGHT - (ry + rh);
            break;
        case 270:
            *x = ry;
            *y = RM690B0_PANEL_HEIGHT - (rx + rw);
            *width = rh;
            *height = rw;
            break;
        default:
            return false;
    }

    if (*width <= 0 || *height <= 0) {
        return false;
    }
    return true;
}

// Helper function to safely check bitmap size and detect overflow
// Uses 64-bit arithmetic to accurately detect overflow on both 32-bit and 64-bit systems
static inline bool check_bitmap_size(size_t width, size_t height, size_t *out_bytes) {
    // Use 64-bit arithmetic for accurate overflow detection
    uint64_t pixels = (uint64_t)width * (uint64_t)height;
    uint64_t bytes = pixels * sizeof(uint16_t);

    // Check if result fits in size_t
    if (bytes > SIZE_MAX) {
        return false;
    }

    *out_bytes = (size_t)bytes;
    return true;
}

static bool clip_logical_rect(const rm690b0_rm690b0_obj_t *self,
    mp_int_t *x, mp_int_t *y,
    mp_int_t *width, mp_int_t *height) {
    if (*width <= 0 || *height <= 0) {
        return false;
    }

    mp_int_t x0 = *x;
    mp_int_t y0 = *y;
    mp_int_t x1 = x0 + *width;
    mp_int_t y1 = y0 + *height;

    if (x1 <= 0 || y1 <= 0 || x0 >= self->width || y0 >= self->height) {
        return false;
    }

    if (x0 < 0) {
        x0 = 0;
    }
    if (y0 < 0) {
        y0 = 0;
    }
    if (x1 > self->width) {
        x1 = self->width;
    }
    if (y1 > self->height) {
        y1 = self->height;
    }

    mp_int_t new_width = x1 - x0;
    mp_int_t new_height = y1 - y0;
    if (new_width <= 0 || new_height <= 0) {
        return false;
    }

    *x = x0;
    *y = y0;
    *width = new_width;
    *height = new_height;
    return true;
}

static inline mp_int_t clamp_int(mp_int_t v, mp_int_t lo, mp_int_t hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Compile-time assertions to ensure fallback character '?' exists in all fonts
_Static_assert('?' >= 32 && '?' <= 127, "Fallback character '?' must be in font range");
_Static_assert(sizeof(rm690b0_font_8x8_data) / sizeof(rm690b0_font_8x8_data[0]) == 96,
    "Font 8x8 must have exactly 96 glyphs (0x20-0x7F)");
_Static_assert(sizeof(rm690b0_font_16x16_data) / sizeof(rm690b0_font_16x16_data[0]) == 95,
    "Font 16x16 must have exactly 95 glyphs (0x20-0x7E)");

static inline const uint8_t *rm690b0_get_8x8_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7F (127) = 96 characters
    if (codepoint < 32 || codepoint > 127) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 127]
    }
    // Safe array access: index range [0, 95]
    return rm690b0_font_8x8_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_16x16_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_16x16_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_16x24_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_16x24_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_24x24_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_24x24_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_24x32_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_24x32_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_32x32_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_32x32_data[codepoint - 32];
}

static inline const uint8_t *rm690b0_get_32x48_glyph(uint32_t codepoint) {
    // Validate codepoint range and use fallback for invalid characters
    // Font range: 0x20 (32) to 0x7E (126) = 95 characters
    if (codepoint < 32 || codepoint > 126) {
        codepoint = '?';  // ASCII 63, guaranteed to be in range [32, 126]
    }
    // Safe array access: index range [0, 94]
    return rm690b0_font_32x48_data[codepoint - 32];
}

// ============================================================================
// OPTIMIZED FONT RENDERING - Universal rotation-aware batch write
// ============================================================================

/**
 * Write a pixel to framebuffer with rotation support.
 * This inline helper handles all 4 rotations in one place.
 * Compiler will optimize this heavily for rotation=0 case.
 */
static inline void rm690b0_write_pixel_rotated(
    rm690b0_rm690b0_obj_t *self,
    rm690b0_impl_t *impl,
    mp_int_t logical_x, mp_int_t logical_y,
    uint16_t color) {

    uint16_t *framebuffer = impl->framebuffer;
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    mp_int_t phys_x, phys_y;

    // Transform logical coordinates to physical based on rotation
    switch (self->rotation) {
        case 0:
            phys_x = logical_x;
            phys_y = logical_y;
            break;
        case 90:
            phys_x = RM690B0_PANEL_WIDTH - logical_y - 1;
            phys_y = logical_x;
            break;
        case 180:
            phys_x = RM690B0_PANEL_WIDTH - logical_x - 1;
            phys_y = RM690B0_PANEL_HEIGHT - logical_y - 1;
            break;
        case 270:
            phys_x = logical_y;
            phys_y = RM690B0_PANEL_HEIGHT - logical_x - 1;
            break;
        default:
            return;  // Invalid rotation
    }

    // Bounds check
    if (phys_x < 0 || phys_x >= RM690B0_PANEL_WIDTH ||
        phys_y < 0 || phys_y >= RM690B0_PANEL_HEIGHT) {
        return;
    }

    // Direct write to framebuffer
    framebuffer[phys_y * fb_stride + phys_x] = color;
}

static void rm690b0_draw_glyph_8x8(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 8 <= 0 || y + 8 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 8 > self->width) ? (self->width - x) : 8;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 8 > self->height) ? (self->height - y) : 8;

    // Pre-swap colors once (not per-pixel)
    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    // Batch write to framebuffer with rotation support (only visible pixels)
    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint8_t bits = glyph[row];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;

        // 90deg: phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1
        //        phys_y = x + col
        //        index = (x + col) * stride + (PANEL_WIDTH - y - row - 1)

        for (int row = row_start; row < row_end; row++) {
            uint8_t bits = glyph[row];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            // Base index for col=0. As col increases, phys_y increases, so index increases by stride.
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;

            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride; // phys_y increments
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;

        // 180deg: phys_x = RM690B0_PANEL_WIDTH - (x + col) - 1
        //         phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1

        for (int row = row_start; row < row_end; row++) {
            uint8_t bits = glyph[row];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;

            // As col increases, phys_x decreases, index decreases by 1
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;

            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--; // phys_x decrements
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;

        // 270deg: phys_x = y + row
        //         phys_y = RM690B0_PANEL_HEIGHT - (x + col) - 1

        for (int row = row_start; row < row_end; row++) {
            uint8_t bits = glyph[row];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;

            // As col increases, phys_y decreases, index decreases by stride
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;

            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride; // phys_y decrements
            }
        }
    }

    // Mark dirty region and flush once (not per-pixel)
    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 8, dirty_h = 8;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_16x16(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 16 <= 0 || y + 16 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 16 > self->width) ? (self->width - x) : 16;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 16 > self->height) ? (self->height - y) : 16;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 16, dirty_h = 16;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 16x16 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_16x24(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 16 <= 0 || y + 24 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 16 > self->width) ? (self->width - x) : 16;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 24 > self->height) ? (self->height - y) : 24;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint16_t bits = ((uint16_t)glyph[row * 2] << 8) | glyph[row * 2 + 1];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x8000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 16, dirty_h = 24;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 16x24 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_24x24(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 24 <= 0 || y + 24 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 24 > self->width) ? (self->width - x) : 24;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 24 > self->height) ? (self->height - y) : 24;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 24, dirty_h = 24;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 24x24 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_24x32(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 24 <= 0 || y + 32 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 24 > self->width) ? (self->width - x) : 24;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 32 > self->height) ? (self->height - y) : 32;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 3] << 16) |
                ((uint32_t)glyph[row * 3 + 1] << 8) |
                glyph[row * 3 + 2];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x800000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 24, dirty_h = 32;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 24x32 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_32x32(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 32 <= 0 || y + 32 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 32 > self->width) ? (self->width - x) : 32;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 32 > self->height) ? (self->height - y) : 32;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 32, dirty_h = 32;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 32x32 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

static void rm690b0_draw_glyph_32x48(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y,
    const uint8_t *glyph,
    uint16_t fg, bool has_bg, uint16_t bg, bool auto_flush) {

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    // Early exit: check if glyph is completely off-screen
    if (x >= self->width || y >= self->height || x + 32 <= 0 || y + 48 <= 0) {
        return;
    }

    // Calculate clipping bounds
    mp_int_t col_start = (x < 0) ? -x : 0;
    mp_int_t col_end = (x + 32 > self->width) ? (self->width - x) : 32;
    mp_int_t row_start = (y < 0) ? -y : 0;
    mp_int_t row_end = (y + 48 > self->height) ? (self->height - y) : 48;

    uint16_t fg_swapped = RGB565_SWAP_GB(fg);
    uint16_t bg_swapped = has_bg ? RGB565_SWAP_GB(bg) : 0;

    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            size_t row_offset = (size_t)(y + row) * fb_stride + (x + col_start);
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[row_offset + (col - col_start)] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[row_offset + (col - col_start)] = bg_swapped;
                }
            }
        }
    } else if (self->rotation == 90) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_x = RM690B0_PANEL_WIDTH - (y + row) - 1;
            size_t start_index = (size_t)(x + col_start) * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index += fb_stride;
            }
        }
    } else if (self->rotation == 180) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_y = RM690B0_PANEL_HEIGHT - (y + row) - 1;
            mp_int_t phys_x_start = RM690B0_PANEL_WIDTH - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y * fb_stride + phys_x_start;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index--;
            }
        }
    } else if (self->rotation == 270) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb_ptr = impl->framebuffer;
        for (int row = row_start; row < row_end; row++) {
            uint32_t bits = ((uint32_t)glyph[row * 4] << 24) |
                ((uint32_t)glyph[row * 4 + 1] << 16) |
                ((uint32_t)glyph[row * 4 + 2] << 8) |
                glyph[row * 4 + 3];
            mp_int_t phys_x = y + row;
            mp_int_t phys_y_start = RM690B0_PANEL_HEIGHT - (x + col_start) - 1;
            size_t start_index = (size_t)phys_y_start * fb_stride + phys_x;
            for (int col = col_start; col < col_end; col++) {
                bool on = (bits & (0x80000000 >> col)) != 0;
                if (on) {
                    fb_ptr[start_index] = fg_swapped;
                } else if (has_bg) {
                    fb_ptr[start_index] = bg_swapped;
                }
                start_index -= fb_stride;
            }
        }
    }

    mp_int_t dirty_x = x, dirty_y = y, dirty_w = 32, dirty_h = 48;
    if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
        mark_dirty_region(impl, dirty_x, dirty_y, dirty_w, dirty_h);

        if (auto_flush && !impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Glyph 32x48 flush failed: %s", esp_err_to_name(ret));
            }
        }
    }
}

// NOTE: Text state is currently stateless per-instance in C; we use a static variable
// for font id to avoid changing struct layout. 0 = 8x8 monospace.
void common_hal_rm690b0_rm690b0_set_font(rm690b0_rm690b0_obj_t *self, mp_int_t font_id) {
    // Clamp to known range
    if (font_id < RM690B0_FONT_8x8_MONO || font_id > RM690B0_FONT_32x48_MONO) {
        font_id = RM690B0_FONT_8x8_MONO;
    }
    // Store in object instance
    self->font_id = font_id;
}

static inline mp_int_t rm690b0_get_current_font(const rm690b0_rm690b0_obj_t *self) {
    return self->font_id;
}

void common_hal_rm690b0_rm690b0_text(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y,
    const char *text, size_t text_len, uint16_t fg, bool has_bg, uint16_t bg) {

    CHECK_INITIALIZED();

    if (text_len == 0) {
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    mp_int_t font_id = rm690b0_get_current_font(self);
    mp_int_t cursor_x = x;
    mp_int_t cursor_y = y;

    // Determine font dimensions based on ID
    mp_int_t font_width, font_height;
    switch (font_id) {
        case RM690B0_FONT_16x16_MONO:
            font_width = 16;
            font_height = 16;
            break;
        case RM690B0_FONT_16x24_MONO:
            font_width = 16;
            font_height = 24;
            break;
        case RM690B0_FONT_24x24_MONO:
            font_width = 24;
            font_height = 24;
            break;
        case RM690B0_FONT_24x32_MONO:
            font_width = 24;
            font_height = 32;
            break;
        case RM690B0_FONT_32x32_MONO:
            font_width = 32;
            font_height = 32;
            break;
        case RM690B0_FONT_32x48_MONO:
            font_width = 32;
            font_height = 48;
            break;
        default: // RM690B0_FONT_8x8_MONO
            font_width = 8;
            font_height = 8;
            break;
    }

    // Initialize bounding box for batch flushing
    // min_x is always x because newlines reset cursor_x to x.
    mp_int_t min_x = x;
    mp_int_t max_x = x;
    mp_int_t min_y = y;
    mp_int_t max_y = y + font_height;

    for (size_t i = 0; i < text_len; i++) {
        uint8_t ch = (uint8_t)text[i];

        if (ch == '\n') {
            cursor_x = x;
            cursor_y += font_height;
            // Extend bounding box height to cover the new line
            if (cursor_y + font_height > max_y) {
                max_y = cursor_y + font_height;
            }
            continue;
        } else if (ch == '\r') {
            // Ignore carriage return
            continue;
        }

        // Draw character based on font ID (no flush)
        switch (font_id) {
            case RM690B0_FONT_16x16_MONO: {
                const uint8_t *glyph = rm690b0_get_16x16_glyph(ch);
                rm690b0_draw_glyph_16x16(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            case RM690B0_FONT_16x24_MONO: {
                const uint8_t *glyph = rm690b0_get_16x24_glyph(ch);
                rm690b0_draw_glyph_16x24(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            case RM690B0_FONT_24x24_MONO: {
                const uint8_t *glyph = rm690b0_get_24x24_glyph(ch);
                rm690b0_draw_glyph_24x24(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            case RM690B0_FONT_24x32_MONO: {
                const uint8_t *glyph = rm690b0_get_24x32_glyph(ch);
                rm690b0_draw_glyph_24x32(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            case RM690B0_FONT_32x32_MONO: {
                const uint8_t *glyph = rm690b0_get_32x32_glyph(ch);
                rm690b0_draw_glyph_32x32(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            case RM690B0_FONT_32x48_MONO: {
                const uint8_t *glyph = rm690b0_get_32x48_glyph(ch);
                rm690b0_draw_glyph_32x48(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
            default: { // RM690B0_FONT_8x8_MONO
                const uint8_t *glyph = rm690b0_get_8x8_glyph(ch);
                rm690b0_draw_glyph_8x8(self, cursor_x, cursor_y, glyph, fg, has_bg, bg, false);
                break;
            }
        }
        cursor_x += font_width;

        // Update bounding box width
        if (cursor_x > max_x) {
            max_x = cursor_x;
        }

        if (cursor_x >= RM690B0_PANEL_WIDTH) {
            cursor_x = x;
            cursor_y += font_height;
            if (cursor_y + font_height > max_y) {
                max_y = cursor_y + font_height;
            }
        }
        if (cursor_y >= RM690B0_PANEL_HEIGHT) {
            break;
        }
    }

    // Final flush of the entire text bounding box
    if (!impl->double_buffered) {
        mp_int_t dirty_x = min_x;
        mp_int_t dirty_y = min_y;
        mp_int_t dirty_w = max_x - min_x;
        mp_int_t dirty_h = max_y - min_y;

        if (dirty_w > 0 && dirty_h > 0) {
            if (map_rect_for_rotation(self, &dirty_x, &dirty_y, &dirty_w, &dirty_h)) {
                esp_err_t ret = rm690b0_flush_region(self, dirty_x, dirty_y, dirty_w, dirty_h, false);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Batch text flush failed: %s", esp_err_to_name(ret));
                }
            }
        }
    }
}

static bool expand_even_region(mp_int_t *x, mp_int_t *y, mp_int_t *width, mp_int_t *height) {
    mp_int_t start_x = *x;
    mp_int_t start_y = *y;
    mp_int_t end_x = start_x + *width;
    mp_int_t end_y = start_y + *height;

    if (start_x < 0) {
        start_x = 0;
    }
    if (start_y < 0) {
        start_y = 0;
    }
    if (end_x > RM690B0_PANEL_WIDTH) {
        end_x = RM690B0_PANEL_WIDTH;
    }
    if (end_y > RM690B0_PANEL_HEIGHT) {
        end_y = RM690B0_PANEL_HEIGHT;
    }

    if (start_x >= end_x || start_y >= end_y) {
        return false;
    }

    if (start_x & 1) {
        if (start_x > 0) {
            start_x -= 1;
        } else if (end_x < RM690B0_PANEL_WIDTH) {
            end_x += 1;
        }
    }
    if (end_x & 1) {
        if (end_x < RM690B0_PANEL_WIDTH) {
            end_x += 1;
        } else if (start_x > 0) {
            start_x -= 1;
        }
    }
    if (((end_x - start_x) & 1) != 0) {
        if (end_x < RM690B0_PANEL_WIDTH) {
            end_x += 1;
        } else if (start_x > 0) {
            start_x -= 1;
        }
    }

    if (start_y & 1) {
        if (start_y > 0) {
            start_y -= 1;
        } else if (end_y < RM690B0_PANEL_HEIGHT) {
            end_y += 1;
        }
    }
    if (end_y & 1) {
        if (end_y < RM690B0_PANEL_HEIGHT) {
            end_y += 1;
        } else if (start_y > 0) {
            start_y -= 1;
        }
    }
    if (((end_y - start_y) & 1) != 0) {
        if (end_y < RM690B0_PANEL_HEIGHT) {
            end_y += 1;
        } else if (start_y > 0) {
            start_y -= 1;
        }
    }

    if ((end_y - start_y) < 2) {
        if (end_y < RM690B0_PANEL_HEIGHT) {
            end_y = start_y + 2;
        } else if (start_y >= 1) {
            start_y = end_y - 2;
        }
    }

    if (start_x >= end_x || start_y >= end_y) {
        return false;
    }

    *x = start_x;
    *y = start_y;
    *width = end_x - start_x;
    *height = end_y - start_y;
    return true;
}



static const rm690b0_lcd_init_cmd_t lcd_init_cmds[] = {
    {0xFE, (uint8_t []) {0x20}, 1, 0},
    {0x26, (uint8_t []) {0x0A}, 1, 0},
    {0x24, (uint8_t []) {0x80}, 1, 0},
    {0xFE, (uint8_t []) {0x13}, 1, 0},
    {0xEB, (uint8_t []) {0x0E}, 1, 0},
    {0xFE, (uint8_t []) {0x00}, 1, 0},
    {0x3A, (uint8_t []) {0x55}, 1, 0},
    {0xC2, (uint8_t []) {0x00}, 1, 10},
    {0x35, (uint8_t []) {0x00}, 0, 0},
    {0x51, (uint8_t []) {0x00}, 1, 10},
    {0x11, (uint8_t []) {0x00}, 0, 80},
    {0x2A, (uint8_t []) {0x00, 0x10, 0x01, 0xD1}, 4, 0},
    {0x2B, (uint8_t []) {0x00, 0x00, 0x02, 0x57}, 4, 0},
    {0x29, (uint8_t []) {0x00}, 0, 10},
    {0x36, (uint8_t []) {0x30}, 1, 10}, // MADCTL for Landscape with RGB color order
    {0x51, (uint8_t []) {0xFF}, 1, 0},
};

// rm690b0_impl_t structure is defined earlier in the file (before font rendering functions)

// Singleton instance tracker for static deinit support
static rm690b0_rm690b0_obj_t *rm690b0_singleton = NULL;

static esp_err_t rm690b0_tx_param(const rm690b0_impl_t *impl, uint8_t cmd, const void *param, size_t param_size) {
    if (impl == NULL || impl->io_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t packed_cmd = ((uint32_t)RM690B0_OPCODE_WRITE_CMD << 24) | ((uint32_t)cmd << 8);
    return esp_lcd_panel_io_tx_param(impl->io_handle, packed_cmd, param, param_size);
}

// Mark a region as dirty for next swap_buffers()
static void mark_dirty_region(rm690b0_impl_t *impl, mp_int_t x, mp_int_t y, mp_int_t w, mp_int_t h) {
    if (!impl->double_buffered) {
        // Single-buffer mode: no dirty tracking needed (flush happens immediately)
        return;
    }

    if (w <= 0 || h <= 0) {
        return;
    }

    if (!impl->dirty_region_valid) {
        // First dirty region
        impl->dirty_x = x;
        impl->dirty_y = y;
        impl->dirty_w = w;
        impl->dirty_h = h;
        impl->dirty_region_valid = true;
    } else {
        // Expand to include new region
        mp_int_t x1 = impl->dirty_x;
        mp_int_t y1 = impl->dirty_y;
        mp_int_t x2 = impl->dirty_x + impl->dirty_w;
        mp_int_t y2 = impl->dirty_y + impl->dirty_h;

        mp_int_t new_x1 = (x < x1) ? x : x1;
        mp_int_t new_y1 = (y < y1) ? y : y1;
        mp_int_t new_x2 = (x + w > x2) ? x + w : x2;
        mp_int_t new_y2 = (y + h > y2) ? y + h : y2;

        impl->dirty_x = new_x1;
        impl->dirty_y = new_y1;
        impl->dirty_w = new_x2 - new_x1;
        impl->dirty_h = new_y2 - new_y1;
    }
}

static esp_err_t rm690b0_flush_region(rm690b0_rm690b0_obj_t *self,
    mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, bool skip_final_delay) {
    (void)skip_final_delay;

    if (width <= 0 || height <= 0) {
        return ESP_OK;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->panel_handle == NULL || impl->framebuffer == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    mp_int_t fx = x;
    mp_int_t fy = y;
    mp_int_t fw = width;
    mp_int_t fh = height;

    if (!expand_even_region(&fx, &fy, &fw, &fh)) {
        return ESP_OK;
    }

    size_t fb_stride = RM690B0_PANEL_WIDTH;
    size_t fw_sz = (size_t)fw;
    size_t fh_sz = (size_t)fh;

    size_t available_pixels = RM690B0_MAX_CHUNK_PIXELS;
    if (impl->chunk_buffers[0] != NULL) {
        available_pixels = impl->chunk_buffer_pixels;
    }

    size_t max_chunk_height = available_pixels / fw_sz;
    if (max_chunk_height == 0) {
        max_chunk_height = 1;
    }

    size_t chunk_height = fh_sz;
    if (chunk_height > max_chunk_height) {
        chunk_height = max_chunk_height;
    }
    if (chunk_height == 0) {
        chunk_height = 1;
    }
    if (chunk_height & 1) {
        if (chunk_height < fh_sz) {
            chunk_height += 1;
        } else if (chunk_height > 1) {
            chunk_height -= 1;
        }
    }

    size_t max_chunk_pixels = fw_sz * chunk_height;

    bool use_static_buffers = (impl->chunk_buffers[0] != NULL &&
        impl->chunk_buffers[1] != NULL &&
        impl->chunk_buffer_pixels >= max_chunk_pixels);

    uint16_t *alloc_buffer = NULL;
    if (!use_static_buffers) {
        alloc_buffer = heap_caps_malloc(max_chunk_pixels * sizeof(uint16_t), MALLOC_CAP_DMA);
        if (alloc_buffer == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    esp_err_t ret = ESP_OK;
    uint16_t *framebuffer = impl->framebuffer;
    int buf_idx = 0;
    bool direct_dma = (fx == 0 && fw == RM690B0_PANEL_WIDTH && x == 0 && width == RM690B0_PANEL_WIDTH);

    for (mp_int_t start_y = fy; start_y < fy + fh && ret == ESP_OK; start_y += (mp_int_t)chunk_height) {
        size_t rows_this_chunk = (size_t)((fy + fh) - start_y);
        if (rows_this_chunk > chunk_height) {
            rows_this_chunk = chunk_height;
        }

        rm690b0_wait_for_dma_slot(impl);

        const void *dma_buffer = NULL;
        uint8_t pending_id = RM690B0_PENDING_BUFFER_FRAMEBUFFER;

        if (direct_dma) {
            dma_buffer = framebuffer + (size_t)start_y * fb_stride;
        } else {
            uint16_t *current_buffer = NULL;
            if (use_static_buffers) {
                uint8_t current_idx = (uint8_t)buf_idx;
                while (impl->dma_buffer_in_use[current_idx]) {
                    rm690b0_wait_for_dma_completion(impl);
                }
                current_buffer = impl->chunk_buffers[current_idx];
                impl->dma_buffer_in_use[current_idx] = true;
                pending_id = current_idx;
                buf_idx = (buf_idx + 1) % 2;
            } else {
                while (impl->dma_alloc_buffer_in_use) {
                    rm690b0_wait_for_dma_completion(impl);
                }
                current_buffer = alloc_buffer;
                impl->dma_alloc_buffer_in_use = true;
                pending_id = RM690B0_PENDING_BUFFER_ALLOC;
            }

            dma_buffer = current_buffer;

            if (x == 0 && width == RM690B0_PANEL_WIDTH) {
                size_t bytes_per_row = fb_stride * sizeof(uint16_t);
                size_t chunk_bytes = rows_this_chunk * bytes_per_row;
                const uint16_t *src = framebuffer + (size_t)start_y * fb_stride;
                memcpy(current_buffer, src, chunk_bytes);
            } else {
                size_t dest_index = 0;
                for (size_t row = 0; row < rows_this_chunk; row++) {
                    mp_int_t phys_row = start_y + (mp_int_t)row;
                    mp_int_t src_row = phys_row;
                    if (src_row < 0) {
                        src_row = 0;
                    } else if (src_row >= RM690B0_PANEL_HEIGHT) {
                        src_row = RM690B0_PANEL_HEIGHT - 1;
                    }

                    const uint16_t *row_base = framebuffer + (size_t)src_row * fb_stride;
                    mp_int_t dest_col = 0;
                    mp_int_t flush_end_x = fx + fw;

                    mp_int_t src_left = x;
                    if (src_left < 0) {
                        src_left = 0;
                    }
                    mp_int_t src_right = x + width - 1;
                    if (src_right >= RM690B0_PANEL_WIDTH) {
                        src_right = RM690B0_PANEL_WIDTH - 1;
                    }

                    for (mp_int_t phys_col = fx; phys_col < src_left; phys_col++) {
                        mp_int_t safe_col = (phys_col < 0) ? 0 : ((phys_col >= RM690B0_PANEL_WIDTH) ? RM690B0_PANEL_WIDTH - 1 : phys_col);
                        current_buffer[dest_index + dest_col++] = row_base[safe_col];
                    }

                    if (src_left <= src_right) {
                        size_t middle_len = (size_t)(src_right - src_left + 1);
                        memcpy(&current_buffer[dest_index + dest_col], &row_base[src_left], middle_len * sizeof(uint16_t));
                        dest_col += (mp_int_t)middle_len;
                    }

                    for (mp_int_t phys_col = src_right + 1; phys_col < flush_end_x; phys_col++) {
                        mp_int_t safe_col = (phys_col < 0) ? 0 : ((phys_col >= RM690B0_PANEL_WIDTH) ? RM690B0_PANEL_WIDTH - 1 : phys_col);
                        current_buffer[dest_index + dest_col++] = row_base[safe_col];
                    }
                    dest_index += fw_sz;
                }
            }
        }

        ret = esp_lcd_panel_draw_bitmap(
            impl->panel_handle,
            fx,
            start_y,
            fx + fw,
            start_y + (mp_int_t)rows_this_chunk,
            dma_buffer);

        if (ret != ESP_OK) {
            if (!direct_dma) {
                if (pending_id < 2) {
                    impl->dma_buffer_in_use[pending_id] = false;
                } else if (pending_id == RM690B0_PENDING_BUFFER_ALLOC) {
                    impl->dma_alloc_buffer_in_use = false;
                }
            }
            break;
        }

        rm690b0_dma_pending_push(&impl->dma_pending, pending_id);
        impl->dma_inflight++;
        if (!use_static_buffers && !direct_dma) {
            rm690b0_wait_for_dma_completion(impl);
        }
    }

    if (!use_static_buffers && alloc_buffer != NULL) {
        // Ensure the last dynamic transfer is complete before freeing.
        while (impl->dma_alloc_buffer_in_use) {
            rm690b0_wait_for_dma_completion(impl);
        }
    }

    if (alloc_buffer != NULL) {
        heap_caps_free(alloc_buffer);
    }

    return ret;
}

void common_hal_rm690b0_rm690b0_construct(rm690b0_rm690b0_obj_t *self) {
    self->initialized = false;
    // Start in landscape mode (rotation 0) - display is 600x450
    self->width = RM690B0_PANEL_WIDTH;
    self->height = RM690B0_PANEL_HEIGHT;
    self->rotation = 0;
    self->brightness_raw = 0xFF;
    self->font_id = RM690B0_FONT_8x8_MONO;  // Initialize with default font
    self->impl = m_malloc(sizeof(rm690b0_impl_t));
    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    impl->io_handle = NULL;
    impl->panel_handle = NULL;
    impl->bus_initialized = false;
    impl->chunk_buffers[0] = NULL;
    impl->chunk_buffers[1] = NULL;
    impl->chunk_buffer_pixels = 0;
    impl->framebuffer = NULL;
    impl->framebuffer_pixels = 0;
    impl->framebuffer_front = NULL;
    impl->double_buffered = false;
    impl->circle_span_cache = NULL;
    impl->circle_span_capacity = 0;

    // Initialize dirty region tracking
    impl->dirty_region_valid = false;
    impl->dirty_x = 0;
    impl->dirty_y = 0;
    impl->dirty_w = 0;
    impl->dirty_h = 0;

    // Create synchronization semaphore (counting, one slot per queued transfer)
    impl->transfer_done_sem = xSemaphoreCreateCounting(RM690B0_PANEL_IO_QUEUE_DEPTH, 0);
    if (impl->transfer_done_sem == NULL) {
        m_free(impl);
        self->impl = NULL;
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to create DMA semaphore"));
    }
    impl->dma_buffer_in_use[0] = false;
    impl->dma_buffer_in_use[1] = false;
    impl->dma_alloc_buffer_in_use = false;
    impl->dma_inflight = 0;
    rm690b0_dma_pending_init(&impl->dma_pending);

    // Track this instance as the singleton
    rm690b0_singleton = self;

    ESP_LOGI(TAG, "RM690B0 module constructed");
}

void common_hal_rm690b0_rm690b0_deinit(rm690b0_rm690b0_obj_t *self) {
    // Early safety checks
    if (!self) {
        ESP_LOGW(TAG, "deinit called with NULL self pointer");
        return;
    }

    if (!self->initialized) {
        ESP_LOGI(TAG, "deinit called on already deinitialized instance");
        return;
    }

    ESP_LOGI(TAG, "Starting RM690B0 deinit");

    // Check if impl is valid
    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (!impl) {
        ESP_LOGW(TAG, "deinit: impl pointer is NULL, nothing to free");
        self->initialized = false;
        if (rm690b0_singleton == self) {
            rm690b0_singleton = NULL;
        }
        return;
    }

    // Clear singleton reference
    if (rm690b0_singleton == self) {
        rm690b0_singleton = NULL;
    }

    // Mark as uninitialized early to prevent re-entry
    self->initialized = false;

    // Clear screen to black before freeing resources
    if (impl->panel_handle != NULL && impl->framebuffer != NULL) {
        ESP_LOGI(TAG, "Clearing display to black before deinit");

        // Fill framebuffer with black
        size_t framebuffer_pixels = (size_t)RM690B0_PANEL_WIDTH * RM690B0_PANEL_HEIGHT;
        memset(impl->framebuffer, 0, framebuffer_pixels * sizeof(uint16_t));

        // Flush to display
        esp_err_t ret = rm690b0_flush_region(self, 0, 0, RM690B0_PANEL_WIDTH, RM690B0_PANEL_HEIGHT, false);
        if (ret == ESP_OK) {
            // Brief delay to ensure screen update completes
            rm690b0_wait_for_all_dma(impl);
            vTaskDelay(pdMS_TO_TICKS(10));
            ESP_LOGI(TAG, "Screen cleared successfully");
        } else {
            ESP_LOGW(TAG, "Failed to clear screen before deinit (non-critical)");
        }
    }

    // Free memory buffers
    rm690b0_wait_for_all_dma(impl);

    if (impl->framebuffer_front) {
        ESP_LOGI(TAG, "Freeing front framebuffer");
        heap_caps_free(impl->framebuffer_front);
        impl->framebuffer_front = NULL;
        impl->double_buffered = false;
    }

    if (impl->framebuffer) {
        ESP_LOGI(TAG, "Freeing back framebuffer");
        heap_caps_free(impl->framebuffer);
        impl->framebuffer = NULL;
        impl->framebuffer_pixels = 0;
    }

    if (impl->chunk_buffers[0]) {
        ESP_LOGI(TAG, "Freeing chunk buffer A");
        heap_caps_free(impl->chunk_buffers[0]);
        impl->chunk_buffers[0] = NULL;
    }
    if (impl->chunk_buffers[1]) {
        ESP_LOGI(TAG, "Freeing chunk buffer B");
        heap_caps_free(impl->chunk_buffers[1]);
        impl->chunk_buffers[1] = NULL;
    }
    impl->chunk_buffer_pixels = 0;

    if (impl->circle_span_cache) {
        ESP_LOGI(TAG, "Freeing cached circle spans");
        heap_caps_free(impl->circle_span_cache);
        impl->circle_span_cache = NULL;
        impl->circle_span_capacity = 0;
    }

    if (impl->transfer_done_sem) {
        vSemaphoreDelete(impl->transfer_done_sem);
        impl->transfer_done_sem = NULL;
    }

    // Turn off display power before freeing hardware resources
    if (LCD_PWR_PIN != GPIO_NUM_NC) {
        ESP_LOGI(TAG, "Turning off display power");
        int off_level = LCD_PWR_ON_LEVEL ? 0 : 1;
        gpio_set_level(LCD_PWR_PIN, off_level);
        vTaskDelay(pdMS_TO_TICKS(5));
    }

    // Delete LCD panel (must be before IO handle)
    if (impl->panel_handle != NULL) {
        ESP_LOGI(TAG, "Deleting LCD panel handle");
        esp_err_t ret = esp_lcd_panel_del(impl->panel_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to delete LCD panel: %s", esp_err_to_name(ret));
        }
        impl->panel_handle = NULL;
    }

    // Delete LCD IO handle (must be before SPI bus)
    if (impl->io_handle != NULL) {
        ESP_LOGI(TAG, "Deleting LCD IO handle");
        esp_err_t ret = esp_lcd_panel_io_del(impl->io_handle);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to delete LCD IO: %s", esp_err_to_name(ret));
        }
        impl->io_handle = NULL;
    }

    // Free SPI bus (must be LAST, after all users are deleted)
    if (impl->bus_initialized) {
        ESP_LOGI(TAG, "Freeing SPI bus");
        esp_err_t ret = spi_bus_free(SPI2_HOST);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to free SPI bus: %s (may be in use)", esp_err_to_name(ret));
        }
        impl->bus_initialized = false;
    }

    // Prevent use-after-free
    self->impl = NULL;

    ESP_LOGI(TAG, "RM690B0 deinit complete - all resources freed");
}

void common_hal_rm690b0_rm690b0_deinit_all(void) {
    // Static deinit for REPL convenience - cleans up any active instance
    if (rm690b0_singleton != NULL) {
        ESP_LOGI(TAG, "Static deinit: cleaning up singleton instance");
        common_hal_rm690b0_rm690b0_deinit(rm690b0_singleton);
        rm690b0_singleton = NULL;
        ESP_LOGI(TAG, "Static deinit: singleton cleaned up");
    } else {
        ESP_LOGI(TAG, "Static deinit: no active instance to clean up");
    }
}

// Helper function to expose panel handle for LVGL integration
esp_lcd_panel_handle_t rm690b0_get_panel_handle(void) {
    if (rm690b0_singleton == NULL) {
        ESP_LOGW(TAG, "rm690b0_get_panel_handle: no active display instance");
        return NULL;
    }

    if (!rm690b0_singleton->initialized) {
        ESP_LOGW(TAG, "rm690b0_get_panel_handle: display not initialized");
        return NULL;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)rm690b0_singleton->impl;
    if (impl == NULL) {
        ESP_LOGE(TAG, "rm690b0_get_panel_handle: impl is NULL");
        return NULL;
    }

    return impl->panel_handle;
}

void common_hal_rm690b0_rm690b0_init_display(rm690b0_rm690b0_obj_t *self) {
    if (self->initialized) {
        ESP_LOGW(TAG, "Display already initialized");
        return;
    }

    ESP_LOGI(TAG, "Initializing RM690B0 display (%s mode)", LCD_USE_QSPI ? "QSPI" : "SPI");

    // Enable display power if a dedicated control pin is provided
    if (LCD_PWR_PIN != GPIO_NUM_NC) {
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = (1ULL << LCD_PWR_PIN),
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
        gpio_set_level(LCD_PWR_PIN, LCD_PWR_ON_LEVEL);
        ESP_LOGI(TAG, "Display power enabled on GPIO%d", LCD_PWR_PIN);
    } else {
        ESP_LOGI(TAG, "No display power control pin configured; assuming panel is already powered");
    }

    // Wait for power to stabilize (longer wait for display power-up)
    vTaskDelay(pdMS_TO_TICKS(200));

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    esp_err_t ret = ESP_OK;

    if (!impl->bus_initialized) {
        // Initialize SPI bus using configuration macros from esp_lcd_rm690b0.h
        // These macros correctly configure either QSPI (4 data lines) or standard SPI (1 data line)
        // QSPI: Uses data0-data3 for 4-bit parallel data transfer (faster)
        // SPI:  Uses mosi (data0) for single-bit serial data transfer (compatible)
        size_t max_transfer_bytes = RM690B0_MAX_CHUNK_PIXELS * sizeof(uint16_t);
        const spi_bus_config_t buscfg = LCD_USE_QSPI ?
            RM690B0_PANEL_BUS_QSPI_CONFIG(LCD_SCK_PIN, LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN, max_transfer_bytes) :
            RM690B0_PANEL_BUS_SPI_CONFIG(LCD_SCK_PIN, LCD_D0_PIN, max_transfer_bytes);
        ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
        if (ret != ESP_OK) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize SPI bus: %s"), esp_err_to_name(ret));
            return;
        }
        impl->bus_initialized = true;
        ESP_LOGI(TAG, "%s bus initialized", LCD_USE_QSPI ? "QSPI" : "SPI");
    }

    // Configure panel I/O layer using macros from esp_lcd_rm690b0.h
    // These macros set up the correct communication protocol:
    // - QSPI mode: 32-bit commands, no DC pin, quad_mode flag enabled
    // - SPI mode:  8-bit commands, DC pin required, standard SPI
    // Both use 8-bit parameters and 10-depth transaction queue
    // We provide a callback to release the semaphore when transfer completes
    if (LCD_USE_QSPI) {
        const esp_lcd_panel_io_spi_config_t io_config = RM690B0_PANEL_IO_QSPI_CONFIG(LCD_CS_PIN, rm690b0_on_color_trans_done, impl, LCD_PIXEL_CLOCK_HZ);
        ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &impl->io_handle);
    } else {
        const esp_lcd_panel_io_spi_config_t io_config = RM690B0_PANEL_IO_SPI_CONFIG(LCD_CS_PIN, -1, rm690b0_on_color_trans_done, impl, LCD_PIXEL_CLOCK_HZ);
        ret = esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &impl->io_handle);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create panel I/O: %s", esp_err_to_name(ret));
        spi_bus_free(SPI2_HOST);
        impl->bus_initialized = false;
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to create panel I/O"));
        return;
    }
    ESP_LOGI(TAG, "Panel I/O created");

    // Configure vendor-specific settings for RM690B0
    rm690b0_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(rm690b0_lcd_init_cmd_t),
        .flags = {
            .use_qspi_interface = LCD_USE_QSPI,
        },
    };

    // Configure panel device - exact configuration from board.c.old
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_RST_PIN,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_BIT_PER_PIXEL,
        .flags = {
            .reset_active_high = false,
        },
        .vendor_config = &vendor_config,
    };

    ret = esp_lcd_new_panel_rm690b0(impl->io_handle, &panel_config, &impl->panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RM690B0 panel: %s", esp_err_to_name(ret));
        esp_lcd_panel_io_del(impl->io_handle);
        impl->io_handle = NULL;
        spi_bus_free(SPI2_HOST);
        impl->bus_initialized = false;
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to create RM690B0 panel"));
        return;
    }
    ESP_LOGI(TAG, "RM690B0 panel created");

    // Initialize the display - exact sequence from board.c.old
    ret = esp_lcd_panel_reset(impl->panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset panel: %s", esp_err_to_name(ret));
        goto cleanup;
    }
    ESP_LOGI(TAG, "Panel reset complete");

    ret = esp_lcd_panel_init(impl->panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize panel: %s", esp_err_to_name(ret));
        goto cleanup;
    }
    ESP_LOGI(TAG, "Panel initialization complete");

    // Set the gap after init - exact values from board.c.old
    esp_lcd_panel_set_gap(impl->panel_handle, RM690B0_X_GAP, RM690B0_Y_GAP);
    ESP_LOGI(TAG, "Display gap set to (0, 16)");

    // Turn on the display
    ret = esp_lcd_panel_disp_on_off(impl->panel_handle, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        goto cleanup;
    }
    ESP_LOGI(TAG, "Display turned on");

    // Attempt to allocate static chunk buffers with a downgrade strategy
    // Priority 1: Full size in Internal RAM (fastest) - unlikely for 450px wide high colors
    // Priority 2: Full size in PSRAM (standard)
    // Priority 3: Reduced size in Internal RAM or PSRAM
    if (impl->chunk_buffers[0] == NULL) {
        size_t current_chunk_rows = RM690B0_MAX_CHUNK_ROWS;

        while (current_chunk_rows >= 2) {
            size_t buf_pixels = (size_t)RM690B0_PANEL_WIDTH * current_chunk_rows;
            size_t buf_size = buf_pixels * sizeof(uint16_t);

            // Try explicit internal first for smaller sizes or just best effort caps?
            // We use MALLOC_CAP_DMA which allows both internal and PSRAM, but
            // heap_caps_malloc usually prefers internal if available and small enough.
            // For large buffers, we might want to let it go to PSRAM to save internal for stack/heap.
            // Let's rely on MALLOC_CAP_DMA.

            impl->chunk_buffers[0] = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
            impl->chunk_buffers[1] = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);

            if (impl->chunk_buffers[0] != NULL && impl->chunk_buffers[1] != NULL) {
                // Success
                impl->chunk_buffer_pixels = buf_pixels;
                ESP_LOGI(TAG, "Allocated dual DMA chunk buffers (%zu pixels each, %zu rows)",
                    impl->chunk_buffer_pixels, current_chunk_rows);
                break;
            }

            // Allocation failed, cleanup partial and try smaller
            if (impl->chunk_buffers[0]) {
                heap_caps_free(impl->chunk_buffers[0]);
            }
            if (impl->chunk_buffers[1]) {
                heap_caps_free(impl->chunk_buffers[1]);
            }
            impl->chunk_buffers[0] = NULL;
            impl->chunk_buffers[1] = NULL;

            // Reduce size
            current_chunk_rows /= 2;
        }

        if (impl->chunk_buffers[0] == NULL) {
            impl->chunk_buffer_pixels = 0;
            ESP_LOGW(TAG, "Unable to allocate dual DMA chunk buffers; operations will use slower allocation per draw");
        }
    }

    size_t framebuffer_pixels = (size_t)RM690B0_PANEL_WIDTH * RM690B0_PANEL_HEIGHT;
    if (impl->framebuffer == NULL) {
        impl->framebuffer = heap_caps_malloc(framebuffer_pixels * sizeof(uint16_t),
            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (impl->framebuffer == NULL) {
            ESP_LOGE(TAG, "Unable to allocate PSRAM framebuffer");
            goto cleanup;
        }
        impl->framebuffer_pixels = framebuffer_pixels;
        ESP_LOGI(TAG, "Allocated back framebuffer (%zu pixels, %zu KB)",
            framebuffer_pixels, (framebuffer_pixels * sizeof(uint16_t)) / 1024);
    }

    // Front buffer will be allocated lazily on first swap_buffers() call
    // This avoids 540KB memory overhead unless tear-free rendering is actually used

    // Initialize back buffer with black
    for (size_t i = 0; i < framebuffer_pixels; i++) {
        impl->framebuffer[i] = 0x0000;
    }
    esp_err_t clear_ret = rm690b0_flush_region(self, 0, 0, RM690B0_PANEL_WIDTH, RM690B0_PANEL_HEIGHT, false);
    if (clear_ret != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to clear display: %s"), esp_err_to_name(clear_ret));
        return;
    }
    ESP_LOGI(TAG, "Display filled with black");

    self->rotation = 0;
    self->width = RM690B0_PANEL_WIDTH;
    self->height = RM690B0_PANEL_HEIGHT;
    self->brightness_raw = 0xFF;
    self->initialized = true;
    ESP_LOGI(TAG, "RM690B0 display initialization complete");
    return;

cleanup:
    // Free resources in reverse order
    if (impl->chunk_buffers[0]) {
        heap_caps_free(impl->chunk_buffers[0]);
        impl->chunk_buffers[0] = NULL;
    }
    if (impl->chunk_buffers[1]) {
        heap_caps_free(impl->chunk_buffers[1]);
        impl->chunk_buffers[1] = NULL;
    }
    impl->chunk_buffer_pixels = 0;

    // Panel handle must be valid if we got here (checked before reset/init)
    if (impl->panel_handle) {
        esp_lcd_panel_del(impl->panel_handle);
        impl->panel_handle = NULL;
    }

    if (impl->io_handle) {
        esp_lcd_panel_io_del(impl->io_handle);
        impl->io_handle = NULL;
    }

    if (impl->bus_initialized) {
        spi_bus_free(SPI2_HOST);
        impl->bus_initialized = false;
    }

    if (ret == ESP_OK) {
        // Must be memory error if ret is OK but we jumped to cleanup
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Display initialization failed: Out of memory"));
    } else {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Display initialization failed: %s"), esp_err_to_name(ret));
    }
}

static void rm690b0_fill_color_direct(rm690b0_rm690b0_obj_t *self, uint16_t color) {
    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->panel_handle == NULL || impl->transfer_done_sem == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    uint16_t swapped = RGB565_SWAP_GB(color);
    size_t dma_lines = RM690B0_MAX_CHUNK_ROWS;
    if (dma_lines > RM690B0_PANEL_HEIGHT) {
        dma_lines = RM690B0_PANEL_HEIGHT;
    }
    const size_t line_pixels = RM690B0_PANEL_WIDTH;
    const size_t dma_pixels = line_pixels * dma_lines;
    const size_t dma_bytes = dma_pixels * sizeof(uint16_t);

    uint16_t *dma_buffer = heap_caps_malloc(dma_bytes, MALLOC_CAP_DMA);
    if (dma_buffer == NULL) {
        mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("fill_color: unable to allocate DMA buffer"));
        return;
    }

    for (size_t i = 0; i < dma_pixels; i++) {
        dma_buffer[i] = swapped;
    }

    size_t rows_remaining = RM690B0_PANEL_HEIGHT;
    mp_int_t current_y = 0;
    esp_err_t ret = ESP_OK;

    while (rows_remaining > 0 && ret == ESP_OK) {
        size_t rows_this_pass = rows_remaining > dma_lines ? dma_lines : rows_remaining;

        // Wait for queue slot before submitting
        rm690b0_wait_for_dma_slot(impl);

        ret = esp_lcd_panel_draw_bitmap(
            impl->panel_handle,
            0,
            current_y,
            RM690B0_PANEL_WIDTH,
            current_y + (mp_int_t)rows_this_pass,
            dma_buffer);

        if (ret != ESP_OK) {
            break;
        }

        rm690b0_dma_pending_push(&impl->dma_pending, RM690B0_PENDING_BUFFER_TEMP);
        impl->dma_inflight++;
        current_y += (mp_int_t)rows_this_pass;
        rows_remaining -= rows_this_pass;
    }

    rm690b0_wait_for_all_dma(impl);
    if (ret == ESP_OK && impl->framebuffer != NULL) {
        rm690b0_fill_rect_framebuffer(impl, 0, 0,
            RM690B0_PANEL_WIDTH, RM690B0_PANEL_HEIGHT, swapped);
        impl->dirty_region_valid = false;
    }

    heap_caps_free(dma_buffer);

    if (ret != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("fill_color failed: %s"), esp_err_to_name(ret));
    }
}

static esp_err_t rm690b0_fill_rect_direct_fullwidth(rm690b0_rm690b0_obj_t *self,
    mp_int_t start_y, mp_int_t rows, uint16_t swapped_color) {

    if (rows <= 0) {
        return ESP_OK;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->panel_handle == NULL || impl->transfer_done_sem == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t dma_lines = RM690B0_MAX_CHUNK_ROWS;
    if (dma_lines > (size_t)rows) {
        dma_lines = rows;
    }
    const size_t line_pixels = RM690B0_PANEL_WIDTH;
    const size_t dma_pixels = line_pixels * dma_lines;
    const size_t dma_bytes = dma_pixels * sizeof(uint16_t);

    uint16_t *dma_buffer = heap_caps_malloc(dma_bytes, MALLOC_CAP_DMA);
    if (dma_buffer == NULL) {
        return ESP_ERR_NO_MEM;
    }

    for (size_t i = 0; i < dma_pixels; i++) {
        dma_buffer[i] = swapped_color;
    }

    size_t rows_remaining = (size_t)rows;
    mp_int_t current_y = start_y;
    esp_err_t ret = ESP_OK;

    while (rows_remaining > 0 && ret == ESP_OK) {
        size_t rows_this_pass = rows_remaining > dma_lines ? dma_lines : rows_remaining;

        rm690b0_wait_for_dma_slot(impl);

        ret = esp_lcd_panel_draw_bitmap(
            impl->panel_handle,
            0,
            current_y,
            RM690B0_PANEL_WIDTH,
            current_y + (mp_int_t)rows_this_pass,
            dma_buffer);

        if (ret != ESP_OK) {
            break;
        }

        rm690b0_dma_pending_push(&impl->dma_pending, RM690B0_PENDING_BUFFER_TEMP);
        impl->dma_inflight++;
        current_y += (mp_int_t)rows_this_pass;
        rows_remaining -= rows_this_pass;
    }

    rm690b0_wait_for_all_dma(impl);
    heap_caps_free(dma_buffer);

    if (ret == ESP_OK && impl->framebuffer != NULL) {
        rm690b0_fill_rect_framebuffer(impl, 0, start_y, RM690B0_PANEL_WIDTH, rows, swapped_color);
    }

    return ret;
}

void common_hal_rm690b0_rm690b0_fill_color(rm690b0_rm690b0_obj_t *self, uint16_t color) {
    CHECK_INITIALIZED();

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl != NULL && !impl->double_buffered) {
        rm690b0_fill_color_direct(self, color);
        return;
    }

    common_hal_rm690b0_rm690b0_fill_rect(self, 0, 0, self->width, self->height, color);
}

void common_hal_rm690b0_rm690b0_pixel(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t color) {
    CHECK_INITIALIZED();

    mp_int_t bx = x;
    mp_int_t by = y;
    mp_int_t bw = 1;
    mp_int_t bh = 1;

    if (!clip_logical_rect(self, &bx, &by, &bw, &bh)) {
        return;
    }
    if (!map_rect_for_rotation(self, &bx, &by, &bw, &bh)) {
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    uint16_t swapped_color = RGB565_SWAP_GB(color);
    size_t fb_stride = RM690B0_PANEL_WIDTH;
    impl->framebuffer[(size_t)by * fb_stride + bx] = swapped_color;

    // Mark region as dirty for next swap
    mark_dirty_region(impl, bx, by, bw, bh);

    // Only flush immediately if not double-buffered
    // When double-buffered, swap_buffers() will handle the flush
    if (!impl->double_buffered) {
        esp_err_t ret = rm690b0_flush_region(self, bx, by, bw, bh, false);
        if (ret != ESP_OK) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw pixel: %s"), esp_err_to_name(ret));
        }
    }
}

static void rm690b0_fill_rect_framebuffer(rm690b0_impl_t *impl,
    mp_int_t bx, mp_int_t by, mp_int_t bw, mp_int_t bh, uint16_t swapped_color) {

    uint16_t *base_ptr = impl->framebuffer + (size_t)by * RM690B0_PANEL_WIDTH + bx;

    for (mp_int_t col = 0; col < bw; col++) {
        base_ptr[col] = swapped_color;
    }

    mp_int_t filled_rows = 1;
    size_t row_bytes = (size_t)bw * sizeof(uint16_t);
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    while (filled_rows < bh) {
        mp_int_t rows_to_copy = filled_rows;
        if (filled_rows + rows_to_copy > bh) {
            rows_to_copy = bh - filled_rows;
        }

        for (mp_int_t i = 0; i < rows_to_copy; i++) {
            uint16_t *src_row = base_ptr + (size_t)i * fb_stride;
            uint16_t *dest_row = base_ptr + (size_t)(filled_rows + i) * fb_stride;
            memcpy(dest_row, src_row, row_bytes);
        }

        filled_rows += rows_to_copy;
    }
}

void common_hal_rm690b0_rm690b0_fill_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color) {
    CHECK_INITIALIZED();

    mp_int_t bx = x;
    mp_int_t by = y;
    mp_int_t bw = width;
    mp_int_t bh = height;

    if (!clip_logical_rect(self, &bx, &by, &bw, &bh)) {
        return;
    }
    if (!map_rect_for_rotation(self, &bx, &by, &bw, &bh)) {
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    uint16_t swapped_color = RGB565_SWAP_GB(color);

    if (!impl->double_buffered && bx == 0 && bw == RM690B0_PANEL_WIDTH) {
        esp_err_t direct_ret = rm690b0_fill_rect_direct_fullwidth(self, by, bh, swapped_color);
        if (direct_ret == ESP_OK) {
            mark_dirty_region(impl, bx, by, bw, bh);
            return;
        }
        ESP_LOGW(TAG, "Full-width fast fill_rect path failed (%s) – falling back",
            esp_err_to_name(direct_ret));
    }

    rm690b0_fill_rect_framebuffer(impl, bx, by, bw, bh, swapped_color);

    // Mark region as dirty for next swap
    mark_dirty_region(impl, bx, by, bw, bh);

    // Only flush immediately if not double-buffered
    // When double-buffered, swap_buffers() will handle the flush
    if (!impl->double_buffered) {
        esp_err_t ret = rm690b0_flush_region(self, bx, by, bw, bh, false);
        if (ret != ESP_OK) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw fill_rect: %s"), esp_err_to_name(ret));
        }
    }
}

// ============================================================================
// IMAGE SUPPORT
// ============================================================================

#pragma pack(push, 1)
typedef struct {
    uint16_t type;             // Magic identifier: 0x4d42
    uint32_t size;             // File size in bytes
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;           // Offset to image data in bytes from beginning of file
    uint32_t header_size;      // Header size in bytes
    int32_t width;             // Width of the image
    int32_t height;            // Height of the image
    uint16_t planes;           // Number of color planes
    uint16_t bpp;              // Bits per pixel
    uint32_t compression;      // Compression type
    uint32_t image_size;       // Image size in bytes
    int32_t x_res;             // Pixels per meter
    int32_t y_res;             // Pixels per meter
    uint32_t n_colors;         // Number of colors
    uint32_t important_colors; // Important colors
} bmp_header_t;
#pragma pack(pop)

void common_hal_rm690b0_rm690b0_blit_bmp(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t bmp_data) {
    CHECK_INITIALIZED();

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(bmp_data, &bufinfo, MP_BUFFER_READ);

    if (bufinfo.len < sizeof(bmp_header_t)) {
        mp_raise_ValueError(MP_ERROR_TEXT("BMP data too small"));
        return;
    }

    const bmp_header_t *header = (const bmp_header_t *)bufinfo.buf;

    if (header->type != 0x4D42) { // 'BM'
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid BMP header"));
        return;
    }

    if (header->bpp != 24 && header->bpp != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only 16-bit and 24-bit BMP supported"));
        return;
    }

    if (header->compression != 0 && header->compression != 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("Compressed BMP not supported"));
        return;
    }

    mp_int_t width = header->width;
    mp_int_t height = abs(header->height);
    bool top_down = (header->height < 0);
    size_t data_offset = header->offset;

    if (data_offset >= bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid BMP data offset"));
        return;
    }

    // Clip to screen (logical coordinates)
    mp_int_t clip_x = x;
    mp_int_t clip_y = y;
    mp_int_t clip_w = width;
    mp_int_t clip_h = height;

    if (!clip_logical_rect(self, &clip_x, &clip_y, &clip_w, &clip_h)) {
        return;
    }

    // Calculate source offset based on clipping
    mp_int_t x_offset = clip_x - x;
    mp_int_t y_offset = clip_y - y;

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    const uint8_t *src_data = (const uint8_t *)bufinfo.buf + data_offset;

    // BMP rows are padded to 4 bytes
    int row_padding = (4 - ((width * (header->bpp / 8)) % 4)) % 4;
    int src_stride = width * (header->bpp / 8) + row_padding;

    // Optimized path for rotation 0
    if (self->rotation == 0) {
        size_t fb_stride = RM690B0_PANEL_WIDTH;
        uint16_t *fb = impl->framebuffer;

        for (int row = 0; row < clip_h; row++) {
            // Source row calculation
            int src_y = y_offset + row;
            int src_row_idx = top_down ? src_y : (height - 1 - src_y);
            const uint8_t *row_ptr = src_data + (size_t)src_row_idx * src_stride + (size_t)x_offset * (header->bpp / 8);

            // Destination row
            uint16_t *dst_ptr = fb + (size_t)(clip_y + row) * fb_stride + clip_x;

            if (header->bpp == 24) {
                for (int col = 0; col < clip_w; col++) {
                    uint8_t b = row_ptr[col * 3];
                    uint8_t g = row_ptr[col * 3 + 1];
                    uint8_t r = row_ptr[col * 3 + 2];
                    // Convert RGB888 to RGB565 (swapped for display)
                    uint16_t rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                    dst_ptr[col] = RGB565_SWAP_GB(rgb);
                }
            } else { // 16-bit
                for (int col = 0; col < clip_w; col++) {
                    uint16_t val = row_ptr[col * 2] | (row_ptr[col * 2 + 1] << 8);
                    dst_ptr[col] = RGB565_SWAP_GB(val);
                }
            }
        }
    } else {
        // Rotated path (pixel by pixel, but clipped)
        for (int row = 0; row < clip_h; row++) {
            int src_y = y_offset + row;
            int src_row_idx = top_down ? src_y : (height - 1 - src_y);
            const uint8_t *row_ptr = src_data + (size_t)src_row_idx * src_stride + (size_t)x_offset * (header->bpp / 8);

            for (int col = 0; col < clip_w; col++) {
                uint16_t color565;
                if (header->bpp == 24) {
                    uint8_t b = row_ptr[col * 3];
                    uint8_t g = row_ptr[col * 3 + 1];
                    uint8_t r = row_ptr[col * 3 + 2];
                    uint16_t rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                    color565 = RGB565_SWAP_GB(rgb);
                } else {
                    uint16_t val = row_ptr[col * 2] | (row_ptr[col * 2 + 1] << 8);
                    color565 = RGB565_SWAP_GB(val);
                }
                rm690b0_write_pixel_rotated(self, impl, clip_x + col, clip_y + row, color565);
            }
        }
    }

    // Mark dirty region
    mp_int_t bx = clip_x, by = clip_y, bw = clip_w, bh = clip_h;
    if (map_rect_for_rotation(self, &bx, &by, &bw, &bh)) {
        mark_dirty_region(impl, bx, by, bw, bh);
        if (!impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, bx, by, bw, bh, false);
            if (ret != ESP_OK) {
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw BMP: %s"), esp_err_to_name(ret));
            }
        }
    }
}

static esp_err_t rm690b0_jpeg_on_block(intptr_t ctx_ptr,
    uint32_t block_top, uint32_t block_left,
    uint32_t block_bottom, uint32_t block_right,
    const uint16_t *pixels) {

    rm690b0_jpeg_draw_ctx_t *ctx = (rm690b0_jpeg_draw_ctx_t *)ctx_ptr;
    rm690b0_impl_t *impl = ctx->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    mp_int_t dest_left = ctx->origin_x + (mp_int_t)block_left;
    mp_int_t dest_top = ctx->origin_y + (mp_int_t)block_top;
    mp_int_t dest_right = ctx->origin_x + (mp_int_t)block_right;
    mp_int_t dest_bottom = ctx->origin_y + (mp_int_t)block_bottom;

    mp_int_t clip_right = ctx->clip_x + ctx->clip_w - 1;
    mp_int_t clip_bottom = ctx->clip_y + ctx->clip_h - 1;

    mp_int_t draw_left = dest_left > ctx->clip_x ? dest_left : ctx->clip_x;
    mp_int_t draw_top = dest_top > ctx->clip_y ? dest_top : ctx->clip_y;
    mp_int_t draw_right = dest_right < clip_right ? dest_right : clip_right;
    mp_int_t draw_bottom = dest_bottom < clip_bottom ? dest_bottom : clip_bottom;

    if (draw_left > draw_right || draw_top > draw_bottom) {
        return ESP_OK;
    }

    mp_int_t block_w = (mp_int_t)(block_right - block_left + 1);
    if (block_w <= 0) {
        return ESP_OK;
    }

    size_t fb_stride = RM690B0_PANEL_WIDTH;

    for (mp_int_t row = draw_top; row <= draw_bottom; row++) {
        mp_int_t src_row = row - dest_top;
        const uint16_t *row_src = pixels + (size_t)src_row * block_w + (draw_left - dest_left);

        if (ctx->rotation_zero) {
            uint16_t *dst = impl->framebuffer + (size_t)row * fb_stride + draw_left;
            for (mp_int_t col = draw_left; col <= draw_right; col++) {
                dst[col - draw_left] = RGB565_SWAP_GB(*row_src++);
            }
        } else {
            for (mp_int_t col = draw_left; col <= draw_right; col++) {
                uint16_t color = RGB565_SWAP_GB(*row_src++);
                rm690b0_write_pixel_rotated(ctx->self, impl, col, row, color);
            }
        }
    }

    return ESP_OK;
}


void common_hal_rm690b0_rm690b0_blit_jpeg(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_obj_t jpeg_data) {
    CHECK_INITIALIZED();

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(jpeg_data, &bufinfo, MP_BUFFER_READ);

    esp_jpeg_image_cfg_t jpeg_cfg = {
        .indata = (uint8_t *)bufinfo.buf,
        .indata_size = bufinfo.len,
        .out_format = JPEG_IMAGE_FORMAT_RGB565,
        .out_scale = JPEG_IMAGE_SCALE_0,
        .flags = { .swap_color_bytes = 0 },  // Do not swap in decoder, we handle it manually
        .outbuf = NULL,
        .outbuf_size = 0,
        .user_data = 0,
        .on_block = NULL,
    };

    esp_jpeg_image_output_t jpeg_out;
    esp_err_t ret = esp_jpeg_get_image_info(&jpeg_cfg, &jpeg_out);
    if (ret != ESP_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid JPEG data"));
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    int width = jpeg_out.width;
    int height = jpeg_out.height;

    // Clip to screen (logical coordinates)
    mp_int_t clip_x = x;
    mp_int_t clip_y = y;
    mp_int_t clip_w = width;
    mp_int_t clip_h = height;

    if (clip_logical_rect(self, &clip_x, &clip_y, &clip_w, &clip_h)) {
        rm690b0_jpeg_draw_ctx_t draw_ctx = {
            .self = self,
            .impl = impl,
            .origin_x = x,
            .origin_y = y,
            .clip_x = clip_x,
            .clip_y = clip_y,
            .clip_w = clip_w,
            .clip_h = clip_h,
            .rotation_zero = (self->rotation == 0),
        };

        jpeg_cfg.user_data = (intptr_t)&draw_ctx;
        jpeg_cfg.on_block = rm690b0_jpeg_on_block;

        ret = esp_jpeg_decode(&jpeg_cfg, NULL);
        if (ret != ESP_OK) {
            mp_raise_ValueError(MP_ERROR_TEXT("JPEG decode failed"));
            return;
        }

        // Mark dirty
        mp_int_t bx = clip_x, by = clip_y, bw = clip_w, bh = clip_h;
        if (map_rect_for_rotation(self, &bx, &by, &bw, &bh)) {
            mark_dirty_region(impl, bx, by, bw, bh);
            if (!impl->double_buffered) {
                esp_err_t flush_ret = rm690b0_flush_region(self, bx, by, bw, bh, false);
                if (flush_ret != ESP_OK) {
                    mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw JPEG: %s"), esp_err_to_name(flush_ret));
                }
            }
        }
    }
}

void common_hal_rm690b0_rm690b0_hline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t color) {
    common_hal_rm690b0_rm690b0_fill_rect(self, x, y, width, 1, color);
}

void common_hal_rm690b0_rm690b0_vline(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t height, mp_int_t color) {
    common_hal_rm690b0_rm690b0_fill_rect(self, x, y, 1, height, color);
}

//|     def blit_buffer(self, x: int, y: int, width: int, height: int, bitmap_data: bytearray, *, dest_is_swapped: bool = False) -> None:
//|         """Draw a bitmap
//|
//|         :param int x: X coordinate of the top-left corner
//|         :param int y: Y coordinate of the top-left corner
//|         :param int width: Width of the bitmap
//|         :param int height: Height of the bitmap
//|         :param bytearray bitmap_data: Bitmap data (buffer protocol)
//|         :param bool dest_is_swapped: If True, data is assumed to be in destination byte order (Big Endian) and will not be swapped.
//|                                      Useful for JpegDecoder output which is already swapped. Default is False (Little Endian input).
//|         """
//|         ...
//|


void common_hal_rm690b0_rm690b0_rect(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_int_t color) {
    common_hal_rm690b0_rm690b0_hline(self, x, y, width, color);
    common_hal_rm690b0_rm690b0_hline(self, x, y + height - 1, width, color);
    common_hal_rm690b0_rm690b0_vline(self, x, y, height, color);
    common_hal_rm690b0_rm690b0_vline(self, x + width - 1, y, height, color);
}

static void rm690b0_draw_line_segment(rm690b0_rm690b0_obj_t *self, mp_int_t x0, mp_int_t y0, mp_int_t x1, mp_int_t y1, mp_int_t color) {
    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        return;
    }

    uint16_t swapped_color = RGB565_SWAP_GB(color);
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    mp_int_t px0, py0, px1, py1;
    switch (self->rotation) {
        case 90:
            px0 = RM690B0_PANEL_WIDTH - y0 - 1;
            py0 = x0;
            px1 = RM690B0_PANEL_WIDTH - y1 - 1;
            py1 = x1;
            break;
        case 180:
            px0 = RM690B0_PANEL_WIDTH - x0 - 1;
            py0 = RM690B0_PANEL_HEIGHT - y0 - 1;
            px1 = RM690B0_PANEL_WIDTH - x1 - 1;
            py1 = RM690B0_PANEL_HEIGHT - y1 - 1;
            break;
        case 270:
            px0 = y0;
            py0 = RM690B0_PANEL_HEIGHT - x0 - 1;
            px1 = y1;
            py1 = RM690B0_PANEL_HEIGHT - x1 - 1;
            break;
        default:
            px0 = x0;
            py0 = y0;
            px1 = x1;
            py1 = y1;
            break;
    }

    mp_int_t pdx = labs(px1 - px0);
    mp_int_t pdy = labs(py1 - py0);
    mp_int_t sx = (px0 < px1) ? 1 : -1;
    mp_int_t sy = (py0 < py1) ? 1 : -1;
    mp_int_t err = pdx - pdy;

    mp_int_t x = px0;
    mp_int_t y = py0;
    mp_int_t dirty_min_x = x, dirty_min_y = y;
    mp_int_t dirty_max_x = x, dirty_max_y = y;

    while (1) {
        if (x < dirty_min_x) {
            dirty_min_x = x;
        }
        if (x > dirty_max_x) {
            dirty_max_x = x;
        }
        if (y < dirty_min_y) {
            dirty_min_y = y;
        }
        if (y > dirty_max_y) {
            dirty_max_y = y;
        }

        if (x >= 0 && x < RM690B0_PANEL_WIDTH && y >= 0 && y < RM690B0_PANEL_HEIGHT) {
            impl->framebuffer[(size_t)y * fb_stride + x] = swapped_color;
        }

        if (x == px1 && y == py1) {
            break;
        }
        mp_int_t e2 = 2 * err;
        if (e2 > -pdy) {
            err -= pdy;
            x += sx;
        }
        if (e2 < pdx) {
            err += pdx;
            y += sy;
        }
    }

    mp_int_t bx = dirty_min_x;
    mp_int_t by = dirty_min_y;
    mp_int_t bw = dirty_max_x - dirty_min_x + 1;
    mp_int_t bh = dirty_max_y - dirty_min_y + 1;

    if (bx < 0) {
        bw += bx;
        bx = 0;
    }
    if (by < 0) {
        bh += by;
        by = 0;
    }
    if (bx + bw > RM690B0_PANEL_WIDTH) {
        bw = RM690B0_PANEL_WIDTH - bx;
    }
    if (by + bh > RM690B0_PANEL_HEIGHT) {
        bh = RM690B0_PANEL_HEIGHT - by;
    }

    if (bw > 0 && bh > 0) {
        mark_dirty_region(impl, bx, by, bw, bh);
        if (!impl->double_buffered) {
            rm690b0_flush_region(self, bx, by, bw, bh, false);
        }
    }
}

void common_hal_rm690b0_rm690b0_line(rm690b0_rm690b0_obj_t *self, mp_int_t x0, mp_int_t y0, mp_int_t x1, mp_int_t y1, mp_int_t color) {
    CHECK_INITIALIZED();

    if (x0 == x1) {
        if (y1 < y0) {
            mp_int_t tmp = y0;
            y0 = y1;
            y1 = tmp;
        }
        common_hal_rm690b0_rm690b0_vline(self, x0, y0, y1 - y0 + 1, color);
        return;
    }
    if (y0 == y1) {
        if (x1 < x0) {
            mp_int_t tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        common_hal_rm690b0_rm690b0_hline(self, x0, y0, x1 - x0 + 1, color);
        return;
    }

    mp_int_t dx = labs(x1 - x0);
    mp_int_t dy = labs(y1 - y0);
    mp_int_t line_length = (dx > dy) ? dx : dy;

    const mp_int_t SPLIT_THRESHOLD = 100;
    const mp_int_t TARGET_SEGMENT = 50;

    if (line_length > SPLIT_THRESHOLD) {
        mp_int_t num_segments = (line_length + TARGET_SEGMENT - 1) / TARGET_SEGMENT;
        for (mp_int_t i = 0; i < num_segments; i++) {
            mp_int_t seg_x0 = x0 + (x1 - x0) * i / num_segments;
            mp_int_t seg_y0 = y0 + (y1 - y0) * i / num_segments;
            mp_int_t seg_x1 = x0 + (x1 - x0) * (i + 1) / num_segments;
            mp_int_t seg_y1 = y0 + (y1 - y0) * (i + 1) / num_segments;
            rm690b0_draw_line_segment(self, seg_x0, seg_y0, seg_x1, seg_y1, color);
        }
        return;
    }

    rm690b0_draw_line_segment(self, x0, y0, x1, y1, color);
}

void common_hal_rm690b0_rm690b0_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color) {
    CHECK_INITIALIZED();
    if (radius < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("radius must be non-negative"));
        return;
    }
    if (radius == 0) {
        common_hal_rm690b0_rm690b0_pixel(self, x, y, color);
        return;
    }



    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    uint16_t swapped_color = RGB565_SWAP_GB(color);
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    mp_int_t bx = x - radius;
    mp_int_t by = y - radius;
    mp_int_t bw = radius * 2 + 1;
    mp_int_t bh = radius * 2 + 1;
    bool circle_fully_inside = (bx >= 0 && by >= 0 &&
        bx + bw <= self->width && by + bh <= self->height);

    #define RM690B0_DRAW_CIRCLE_LOOP() \
    do { \
        mp_int_t x0 = 0; \
        mp_int_t y0 = radius; \
        mp_int_t d = 1 - radius; \
        while (x0 <= y0) { \
            WRITE_CIRCLE_PIXEL(x + x0, y + y0); \
            WRITE_CIRCLE_PIXEL(x - x0, y + y0); \
            WRITE_CIRCLE_PIXEL(x + x0, y - y0); \
            WRITE_CIRCLE_PIXEL(x - x0, y - y0); \
            WRITE_CIRCLE_PIXEL(x + y0, y + x0); \
            WRITE_CIRCLE_PIXEL(x - y0, y + x0); \
            WRITE_CIRCLE_PIXEL(x + y0, y - x0); \
            WRITE_CIRCLE_PIXEL(x - y0, y - x0); \
            x0 += 1; \
            if (d < 0) { \
                d += (x0 << 1) + 1; \
            } else { \
                y0 -= 1; \
                d += ((x0 - y0) << 1) + 1; \
            } \
        } \
    } while (0)

    if (circle_fully_inside) {
        #define WRITE_CIRCLE_PIXEL(px, py) \
    impl->framebuffer[(size_t)(py) * fb_stride + (px)] = swapped_color
        RM690B0_DRAW_CIRCLE_LOOP();
#undef WRITE_CIRCLE_PIXEL
    } else {
        #define WRITE_CIRCLE_PIXEL(px, py) do { \
        mp_int_t px_val = (px); \
        mp_int_t py_val = (py); \
        if (px_val >= 0 && px_val < self->width && \
            py_val >= 0 && py_val < self->height) { \
            impl->framebuffer[(size_t)py_val * fb_stride + px_val] = swapped_color; \
        } \
} while (0)
        RM690B0_DRAW_CIRCLE_LOOP();
#undef WRITE_CIRCLE_PIXEL
    }

#undef RM690B0_DRAW_CIRCLE_LOOP

    mp_int_t clip_bx = bx;
    mp_int_t clip_by = by;
    mp_int_t clip_bw = bw;
    mp_int_t clip_bh = bh;

    if (!circle_fully_inside) {
        if (clip_bx < 0) {
            clip_bw += clip_bx;
            clip_bx = 0;
        }
        if (clip_by < 0) {
            clip_bh += clip_by;
            clip_by = 0;
        }
        if (clip_bx + clip_bw > self->width) {
            clip_bw = self->width - clip_bx;
        }
        if (clip_by + clip_bh > self->height) {
            clip_bh = self->height - clip_by;
        }
    }

    if (clip_bw > 0 && clip_bh > 0) {
        // Mark region as dirty for next swap
        mark_dirty_region(impl, clip_bx, clip_by, clip_bw, clip_bh);

        // Only flush immediately if not double-buffered
        // When double-buffered, swap_buffers() will handle the flush
        if (!impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, clip_bx, clip_by, clip_bw, clip_bh, false);
            if (ret != ESP_OK) {
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw circle: %s"), esp_err_to_name(ret));
            }
        }
    }
}

void common_hal_rm690b0_rm690b0_fill_circle(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t radius, mp_int_t color) {
    CHECK_INITIALIZED();
    if (radius < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("radius must be non-negative"));
        return;
    }
    if (radius == 0) {
        common_hal_rm690b0_rm690b0_pixel(self, x, y, color);
        return;
    }


    mp_int_t max_radius = (RM690B0_MAX_DIAMETER - 1) / 2;
    if (radius > max_radius) {
        radius = max_radius;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    mp_int_t top = y - radius;
    mp_int_t row_count = radius * 2 + 1;
    if (row_count <= 0) {
        return;
    }

    mp_int_t bx = x - radius;
    mp_int_t by = y - radius;
    mp_int_t bw = row_count;
    mp_int_t bh = row_count;
    bool circle_fully_inside = (bx >= 0 && by >= 0 &&
        bx + bw <= self->width && by + bh <= self->height);

    #define STACK_ALLOC_THRESHOLD 128
    int16_t left_stack[STACK_ALLOC_THRESHOLD];
    int16_t right_stack[STACK_ALLOC_THRESHOLD];
    int16_t *left = left_stack;
    int16_t *right = right_stack;
    int16_t *heap_span = NULL;

    if (row_count > STACK_ALLOC_THRESHOLD) {
        int16_t *cache = rm690b0_acquire_span_cache(impl, (size_t)row_count);
        if (cache != NULL) {
            left = cache;
            right = cache + (mp_int_t)impl->circle_span_capacity;
        } else {
            size_t span_entries = (size_t)row_count * 2;
            heap_span = (int16_t *)heap_caps_malloc(span_entries * sizeof(int16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (heap_span == NULL) {
                mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to allocate memory for circle"));
                return;
            }
            left = heap_span;
            right = heap_span + row_count;
        }
    }

    for (mp_int_t i = 0; i < row_count; i++) {
        left[i] = INT16_MAX;
        right[i] = INT16_MIN;
    }

    rm690b0_span_accumulator_t acc = {
        .top = top,
        .row_count = row_count,
        .left = left,
        .right = right,
    };

    mp_int_t xi = 0;
    mp_int_t yi = radius;
    mp_int_t d = 1 - radius;

    while (xi <= yi) {
        rm690b0_span_update(&acc, y + yi, x + xi);
        rm690b0_span_update(&acc, y + yi, x - xi);
        rm690b0_span_update(&acc, y - yi, x + xi);
        rm690b0_span_update(&acc, y - yi, x - xi);

        if (yi != xi) {
            rm690b0_span_update(&acc, y + xi, x + yi);
            rm690b0_span_update(&acc, y + xi, x - yi);
            rm690b0_span_update(&acc, y - xi, x + yi);
            rm690b0_span_update(&acc, y - xi, x - yi);
        }

        xi += 1;
        if (d < 0) {
            d += (xi << 1) + 1;
        } else {
            yi -= 1;
            d += ((xi - yi) << 1) + 1;
        }
    }

    uint16_t swapped_color = RGB565_SWAP_GB(color);
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    if (circle_fully_inside) {
        for (mp_int_t row = 0; row < row_count; row++) {
            int16_t span_left = left[row];
            int16_t span_right = right[row];
            if (span_left > span_right) {
                continue;
            }
            mp_int_t yy = top + row;
            size_t span_width = (size_t)(span_right - span_left + 1);
            uint16_t *dest = impl->framebuffer + (size_t)yy * fb_stride + span_left;
            rm690b0_fill_span_fast(dest, span_width, swapped_color);
        }
    } else {
        for (mp_int_t row = 0; row < row_count; row++) {
            int16_t span_left = left[row];
            int16_t span_right = right[row];
            if (span_left > span_right) {
                continue;
            }
            mp_int_t yy = top + row;
            if (yy < 0 || yy >= self->height) {
                continue;
            }
            mp_int_t span_left_i = (mp_int_t)span_left;
            mp_int_t span_right_i = (mp_int_t)span_right;

            if (span_left_i < 0) {
                span_left_i = 0;
            }
            if (span_right_i >= self->width) {
                span_right_i = self->width - 1;
            }

            mp_int_t span_width = span_right_i - span_left_i + 1;
            if (span_width <= 0) {
                continue;
            }

            uint16_t *dest = impl->framebuffer + (size_t)yy * fb_stride + span_left_i;
            rm690b0_fill_span_fast(dest, (size_t)span_width, swapped_color);
        }
    }

    mp_int_t clip_bx = bx;
    mp_int_t clip_by = by;
    mp_int_t clip_bw = bw;
    mp_int_t clip_bh = bh;

    if (!circle_fully_inside) {
        if (clip_bx < 0) {
            clip_bw += clip_bx;
            clip_bx = 0;
        }
        if (clip_by < 0) {
            clip_bh += clip_by;
            clip_by = 0;
        }
        if (clip_bx + clip_bw > self->width) {
            clip_bw = self->width - clip_bx;
        }
        if (clip_by + clip_bh > self->height) {
            clip_bh = self->height - clip_by;
        }
    }

    if (clip_bw > 0 && clip_bh > 0) {
        // Mark region as dirty for next swap
        mark_dirty_region(impl, clip_bx, clip_by, clip_bw, clip_bh);

        // Only flush immediately if not double-buffered
        // When double-buffered, swap_buffers() will handle the flush
        if (!impl->double_buffered) {
            esp_err_t ret = rm690b0_flush_region(self, clip_bx, clip_by, clip_bw, clip_bh, false);
            if (ret != ESP_OK) {
                if (heap_span != NULL) {
                    heap_caps_free(heap_span);
                }
                mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw fill_circle: %s"), esp_err_to_name(ret));
            }
        }
    }

    if (heap_span != NULL) {
        heap_caps_free(heap_span);
    }
}

void common_hal_rm690b0_rm690b0_set_rotation(rm690b0_rm690b0_obj_t *self, mp_int_t degrees) {
    CHECK_INITIALIZED();

    mp_int_t normalized = ((degrees % 360) + 360) % 360;
    switch (normalized) {
        case 0:
        case 180:
            self->width = RM690B0_PANEL_WIDTH;
            self->height = RM690B0_PANEL_HEIGHT;
            break;
        case 90:
        case 270:
            self->width = RM690B0_PANEL_HEIGHT;
            self->height = RM690B0_PANEL_WIDTH;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("Rotation must be 0, 90, 180, or 270"));
            return;
    }

    self->rotation = normalized;
    ESP_LOGI(TAG, "Display rotation set to %d (logical size %dx%d)", (int)normalized, (int)self->width, (int)self->height);
}

mp_int_t common_hal_rm690b0_rm690b0_get_rotation(const rm690b0_rm690b0_obj_t *self) {
    return self->rotation;
}

void common_hal_rm690b0_rm690b0_set_brightness(rm690b0_rm690b0_obj_t *self, mp_float_t value) {
    CHECK_INITIALIZED();
    if (value < 0.0f || value > 1.0f) {
        mp_raise_ValueError(MP_ERROR_TEXT("Brightness must be between 0.0 and 1.0"));
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->io_handle == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    uint8_t brightness;
    if (value <= 0.0f) {
        brightness = 0;
    } else if (value >= 1.0f) {
        brightness = 0xFF;
    } else {
        brightness = (uint8_t)(value * 255.0f + 0.5f);
    }

    if (brightness == self->brightness_raw) {
        return;
    }

    uint8_t page_cmd = 0x00;
    esp_err_t err = rm690b0_tx_param(impl, 0xFE, &page_cmd, 1);
    if (err != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to select brightness page: %s"), esp_err_to_name(err));
        return;
    }

    err = rm690b0_tx_param(impl, 0x51, &brightness, 1);
    if (err != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to write brightness: %s"), esp_err_to_name(err));
        return;
    }

    uint8_t ctrl_display = (brightness == 0) ? 0x20 : 0x2C;
    err = rm690b0_tx_param(impl, 0x53, &ctrl_display, 1);
    if (err != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to latch brightness: %s"), esp_err_to_name(err));
        return;
    }

    self->brightness_raw = brightness;
    ESP_LOGI(TAG, "Set brightness to %u/255 (%.3f)", brightness, (double)value);
}

mp_float_t common_hal_rm690b0_rm690b0_get_brightness(const rm690b0_rm690b0_obj_t *self) {
    uint8_t raw = self->brightness_raw;
    return (mp_float_t)raw / 255.0f;
}

void common_hal_rm690b0_rm690b0_blit_buffer(rm690b0_rm690b0_obj_t *self, mp_int_t x, mp_int_t y, mp_int_t width, mp_int_t height, mp_obj_t bitmap_data, bool dest_is_swapped) {
    CHECK_INITIALIZED();

    if (width <= 0 || height <= 0) {
        return;
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(bitmap_data, &bufinfo, MP_BUFFER_READ);

    size_t src_width = (size_t)width;
    size_t src_height = (size_t)height;

    // Check for overflow in bitmap size calculation
    size_t expected_bytes;
    if (!check_bitmap_size(src_width, src_height, &expected_bytes)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Bitmap dimensions too large (max ~32767x32767 on 32-bit systems)"));
        return;
    }

    if (bufinfo.len < expected_bytes) {
        mp_raise_ValueError(MP_ERROR_TEXT("Bitmap data too small for width * height"));
        return;
    }

    mp_int_t logical_x = x;
    mp_int_t logical_y = y;
    mp_int_t logical_w = width;
    mp_int_t logical_h = height;

    if (!clip_logical_rect(self, &logical_x, &logical_y, &logical_w, &logical_h)) {
        return;
    }

    mp_int_t crop_left = logical_x - x;
    mp_int_t crop_top = logical_y - y;
    if (crop_left < 0) {
        crop_left = 0;
    }
    if (crop_top < 0) {
        crop_top = 0;
    }

    mp_int_t phys_x = logical_x;
    mp_int_t phys_y = logical_y;
    mp_int_t phys_w = logical_w;
    mp_int_t phys_h = logical_h;

    if (!map_rect_for_rotation(self, &phys_x, &phys_y, &phys_w, &phys_h)) {
        return;
    }

    if (phys_w <= 0 || phys_h <= 0) {
        return;
    }

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;
    if (impl == NULL || impl->framebuffer == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Invalid display handle"));
        return;
    }

    const uint16_t *src_base = (const uint16_t *)bufinfo.buf;
    size_t src_stride = src_width;
    const uint16_t *src_pixels = src_base + (size_t)crop_top * src_stride + (size_t)crop_left;

    uint16_t *framebuffer = impl->framebuffer;
    size_t fb_stride = RM690B0_PANEL_WIDTH;

    // If source is already swapped (BE) and we need BE for display, we skip the swap.
    // If source is normal (LE) and we need BE (display), we swap.
    // Standard blit_buffer assumes LE input and swaps to BE.
    // IF dest_is_swapped is TRUE, it means the SOURCE is already in destination format (BE).
    
    switch (self->rotation) {
        case 0:
            for (mp_int_t row = 0; row < logical_h; row++) {
                const uint16_t *src_row = src_pixels + (size_t)row * src_stride;
                uint16_t *dst_row = framebuffer + (size_t)(phys_y + row) * fb_stride + phys_x;
                if (dest_is_swapped) {
                    memcpy(dst_row, src_row, logical_w * sizeof(uint16_t));
                } else {
                    for (mp_int_t col = 0; col < logical_w; col++) {
                        dst_row[col] = RGB565_SWAP_GB(src_row[col]);
                    }
                }
            }
            break;
        case 180:
            for (mp_int_t row = 0; row < logical_h; row++) {
                const uint16_t *src_row = src_pixels + (size_t)(logical_h - 1 - row) * src_stride;
                uint16_t *dst_row = framebuffer + (size_t)(phys_y + row) * fb_stride + phys_x;
                for (mp_int_t col = 0; col < logical_w; col++) {
                    uint16_t val = src_row[logical_w - 1 - col];
                    dst_row[col] = dest_is_swapped ? val : RGB565_SWAP_GB(val);
                }
            }
            break;
        case 90: {
            mp_int_t phys_h_rows = phys_h;
            mp_int_t phys_w_cols = phys_w;
            for (mp_int_t row = 0; row < phys_h_rows; row++) {
                uint16_t *dst_row = framebuffer + (size_t)(phys_y + row) * fb_stride + phys_x;
                for (mp_int_t col = 0; col < phys_w_cols; col++) {
                    mp_int_t src_row_idx = logical_h - 1 - col;
                    mp_int_t src_col_idx = row;
                    const uint16_t *src_row = src_pixels + (size_t)src_row_idx * src_stride;
                    uint16_t val = src_row[src_col_idx];
                    dst_row[col] = dest_is_swapped ? val : RGB565_SWAP_GB(val);
                }
            }
            break;
        }
        case 270: {
            mp_int_t phys_h_rows = phys_h;
            mp_int_t phys_w_cols = phys_w;
            for (mp_int_t row = 0; row < phys_h_rows; row++) {
                uint16_t *dst_row = framebuffer + (size_t)(phys_y + row) * fb_stride + phys_x;
                for (mp_int_t col = 0; col < phys_w_cols; col++) {
                    mp_int_t src_row_idx = col;
                    mp_int_t src_col_idx = logical_w - 1 - row;
                    const uint16_t *src_row = src_pixels + (size_t)src_row_idx * src_stride;
                    uint16_t val = src_row[src_col_idx];
                    dst_row[col] = dest_is_swapped ? val : RGB565_SWAP_GB(val);
                }
            }
            break;
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("Unsupported rotation"));
            return;
    }

    // Mark region as dirty for next swap
    mark_dirty_region(impl, phys_x, phys_y, phys_w, phys_h);

    // Only flush immediately if not double-buffered
    // When double-buffered, swap_buffers() will handle the flush
    if (!impl->double_buffered) {
        esp_err_t ret = rm690b0_flush_region(self, phys_x, phys_y, phys_w, phys_h, false);
        if (ret != ESP_OK) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to draw bitmap: %s"), esp_err_to_name(ret));
        }
    }
}

void common_hal_rm690b0_rm690b0_swap_buffers(rm690b0_rm690b0_obj_t *self, bool copy) {
    CHECK_INITIALIZED();

    rm690b0_impl_t *impl = (rm690b0_impl_t *)self->impl;

    // Lazy allocation: allocate front buffer on first call
    if (!impl->double_buffered && impl->framebuffer_front == NULL) {
        size_t framebuffer_pixels = (size_t)RM690B0_PANEL_WIDTH * RM690B0_PANEL_HEIGHT;
        impl->framebuffer_front = heap_caps_malloc(framebuffer_pixels * sizeof(uint16_t),
            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

        if (impl->framebuffer_front == NULL) {
            ESP_LOGW(TAG, "Unable to allocate front framebuffer - falling back to single-buffered refresh");
            // Fall through to single-buffer mode below
        } else {
            impl->double_buffered = true;
            ESP_LOGI(TAG, "Allocated front framebuffer (%zu KB) - double-buffering enabled",
                (framebuffer_pixels * sizeof(uint16_t)) / 1024);

            // Copy current back buffer to front buffer so swap has valid data
            memcpy(impl->framebuffer_front, impl->framebuffer,
                framebuffer_pixels * sizeof(uint16_t));
        }
    }

    if (!impl->double_buffered || impl->framebuffer_front == NULL) {
        // Single-buffered mode - just flush current framebuffer
        esp_err_t ret = rm690b0_flush_region(self, 0, 0, self->width, self->height, false);
        if (ret != ESP_OK) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("Failed to refresh display: %s (0x%x)"),
                esp_err_to_name(ret), ret);
        }
        return;
    }

    // Double-buffered mode: flush dirty region
    // If no dirty region, flush everything
    mp_int_t flush_x = 0;
    mp_int_t flush_y = 0;
    mp_int_t flush_w = self->width;
    mp_int_t flush_h = self->height;

    if (impl->dirty_region_valid) {
        // Only flush the dirty region for better performance
        flush_x = impl->dirty_x;
        flush_y = impl->dirty_y;
        flush_w = impl->dirty_w;
        flush_h = impl->dirty_h;
        ESP_LOGI(TAG, "Flushing dirty region: %d,%d %dx%d", flush_x, flush_y, flush_w, flush_h);
    } else {
        ESP_LOGI(TAG, "Flushing full screen (no dirty region)");
    }

    // Skip final delay if we're doing a copy - the memcpy will protect the SPI transfer
    esp_err_t ret = rm690b0_flush_region(self, flush_x, flush_y, flush_w, flush_h, copy);


    // Now swap the buffer pointers
    // After swap: framebuffer becomes the old front buffer (ready for new drawing)
    //             framebuffer_front becomes the old back buffer (now being displayed)
    uint16_t *temp = impl->framebuffer_front;
    impl->framebuffer_front = impl->framebuffer;
    impl->framebuffer = temp;

    // Reset dirty region after flush
    impl->dirty_region_valid = false;
    impl->dirty_x = 0;
    impl->dirty_y = 0;
    impl->dirty_w = 0;
    impl->dirty_h = 0;

    // Optionally copy front buffer to back buffer for incremental drawing
    // Standard double-buffering: back buffer inherits current display content
    // Skip copy for better performance when doing full redraws (animations)
    if (copy) {
        size_t framebuffer_pixels = (size_t)RM690B0_PANEL_WIDTH * RM690B0_PANEL_HEIGHT;
        memcpy(impl->framebuffer, impl->framebuffer_front,
            framebuffer_pixels * sizeof(uint16_t));
        ESP_LOGI(TAG, "Buffers swapped and back buffer updated");
    } else {
        ESP_LOGI(TAG, "Buffers swapped (no copy)");
    }
    if (ret != ESP_OK) {
        mp_raise_msg_varg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Failed to refresh display: %s (0x%x)"),
            esp_err_to_name(ret), ret);
    }
}

void common_hal_rm690b0_rm690b0_convert_bmp(rm690b0_rm690b0_obj_t *self, mp_obj_t src_data, mp_obj_t dest_bitmap) {
    mp_buffer_info_t src_info;
    mp_get_buffer_raise(src_data, &src_info, MP_BUFFER_READ);

    mp_buffer_info_t dest_info;
    mp_get_buffer_raise(dest_bitmap, &dest_info, MP_BUFFER_WRITE);

    if (src_info.len < sizeof(bmp_header_t)) {
        mp_raise_ValueError(MP_ERROR_TEXT("BMP data too small"));
        return;
    }

    const bmp_header_t *header = (const bmp_header_t *)src_info.buf;

    if (header->type != 0x4D42) { // 'BM'
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid BMP header"));
        return;
    }

    if (header->bpp != 24 && header->bpp != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only 16-bit and 24-bit BMP supported"));
        return;
    }

    if (header->compression != 0 && header->compression != 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("Compressed BMP not supported"));
        return;
    }

    mp_int_t width = header->width;
    mp_int_t height = abs(header->height);
    bool top_down = (header->height < 0);
    size_t data_offset = header->offset;

    if (data_offset >= src_info.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("Invalid BMP data offset"));
        return;
    }
    
    // Check destination size implicitly by buffer length
    // We assume the user created the bitmap correctly. We will not overflow the buffer.
    size_t max_dest_pixels = dest_info.len / sizeof(uint16_t);
    if ((size_t)(width * height) > max_dest_pixels) {
         mp_raise_ValueError(MP_ERROR_TEXT("Destination bitmap too small"));
         return;
    }
    
    // Pointers
    uint16_t *dest_buf = (uint16_t *)dest_info.buf;
    const uint8_t *src_pixels = (const uint8_t *)src_info.buf + data_offset;
    
    int row_padding = (4 - ((width * (header->bpp / 8)) % 4)) % 4;
    int src_stride = width * (header->bpp / 8) + row_padding;
    
    for (int row = 0; row < height; row++) {
         int src_row_idx = top_down ? row : (height - 1 - row);
         const uint8_t *row_ptr = src_pixels + (size_t)src_row_idx * src_stride;
         uint16_t *dst_row_ptr = dest_buf + (size_t)row * width; // Dense packing in Bitmap
         
         if (header->bpp == 24) {
             for (int col = 0; col < width; col++) {
                 uint8_t b = row_ptr[col * 3];
                 uint8_t g = row_ptr[col * 3 + 1];
                 uint8_t r = row_ptr[col * 3 + 2];
                 uint16_t rgb = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                 dst_row_ptr[col] = RGB565_SWAP_GB(rgb); 
             }
         } else {
             for (int col = 0; col < width; col++) {
                 uint16_t val = row_ptr[col * 2] | (row_ptr[col * 2 + 1] << 8);
                 dst_row_ptr[col] = RGB565_SWAP_GB(val);
             }
         }
    }
}
