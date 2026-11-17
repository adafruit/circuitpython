/**
 * LVGL config for CircuitPython on ESP32 P4/S31
 * Based on lv_conf_template.h with P4 optimizations
 */
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_OS LV_OS_NONE

/* Color depth: 16 for RGB565, 32 for ARGB8888 - individual displays set their own
 * format when they are created. */
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_RGB565

/* Memory: allocate from the port heap instead of the garbage collected VM heap.
 * Displays live outside the VM, so LVGL allocations must not be collected. */
#define LV_USE_STDLIB_MALLOC LV_STDLIB_CUSTOM
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB

/* Display and rendering */
#define LV_USE_DRAW_SW 1
#define LV_USE_IMAGE 1
#define LV_USE_LABEL 1
#define LV_USE_OBJ 1

/* Use the ESP32-P4/S31 Pixel Processing Accelerator for fills, blends and simple
 * images. PPA works on cache lines, so draw buffers must be 64-byte aligned. */
#define LV_USE_PPA 1
#define LV_USE_PPA_IMG 1
/* Leave regular PSRAM arbitration gaps for the DSI framebuffer scanout DMA. */
#define LV_PPA_BURST_LENGTH 64
/* LVGL 9.6 checks this before defining it, which trips -Wundef. */
#define LV_PPA_NONBLOCKING_OPS 0
#define LV_DRAW_BUF_ALIGN 64
#define CONFIG_LV_DRAW_BUF_ALIGN 64

/* displayio hands indexed bitmaps to LVGL as I1/I2/I4/I8 images. Decode them fully
 * (instead of line by line) so they can be scaled, and cache the decoded image so it is
 * only decoded again when the bitmap or palette changes. An image that doesn't fit in the
 * cache isn't drawn. */
#define LV_BIN_DECODER_RAM_LOAD 1

/* The image cache only evicts against its own size limit, and the decoded image is allocated
 * before the cache is consulted. A limit anywhere near the 16 MB of PSRAM therefore lets one
 * decode exhaust the heap before anything is evicted, and the next small allocation (a draw
 * task) fails. With LV_USE_ASSERT_MALLOC off that failure is a NULL dereference, not an error
 * message. Keep the limit far below PSRAM: it has to fit the largest single decode (a 720p
 * ARGB8888 image is 3.7 MB) with room left for the display buffers and the VM heap. */
#define LV_CACHE_DEF_SIZE (4 * 1024 * 1024)

/* A TileGrid composes a changed cell at a time under one batched invalidation, and scroll
 * invalidates one object, so a handful of pending areas is plenty. */
#define LV_INV_BUF_SIZE 64

/* Font: use built-in */
#define LV_FONT_MONTSERRAT_14 1

/* Logging */
#define LV_USE_LOG 0

/* displayio draws plain objects with explicit styles, so the themes aren't used.
 * The default theme pulls in widgets that this config disables. */
#define LV_USE_THEME_DEFAULT 0
#define LV_USE_THEME_SIMPLE 0
#define LV_USE_THEME_MONO 0

/* displayio uses lv_obj, lv_image, lv_canvas (the compose pass draws through a canvas
 * layer) and lv_label for text. The rest of the widgets are enabled by default, and many
 * of them require each other, so turn them all off to keep the build small and the
 * dependency checks happy. */
#define LV_USE_ANIMIMG 0
#define LV_USE_ARC 0
#define LV_USE_ARCLABEL 0
#define LV_USE_BAR 0
#define LV_USE_BUTTON 0
#define LV_USE_BUTTONMATRIX 0
#define LV_USE_CALENDAR 0
#define LV_USE_CANVAS 1
#define LV_USE_CHART 0
#define LV_USE_CHECKBOX 0
#define LV_USE_DROPDOWN 0
#define LV_USE_IMAGEBUTTON 0
#define LV_USE_KEYBOARD 0
#define LV_USE_LED 0
#define LV_USE_LINE 0
#define LV_USE_LIST 0
#define LV_USE_MENU 0
#define LV_USE_MSGBOX 0
#define LV_USE_ROLLER 0
#define LV_USE_SCALE 0
#define LV_USE_SLIDER 0
#define LV_USE_SPAN 0
#define LV_USE_SPINBOX 0
#define LV_USE_SPINNER 0
#define LV_USE_SWITCH 0
#define LV_USE_TABLE 0
#define LV_USE_TABVIEW 0
#define LV_USE_TEXTAREA 0
#define LV_USE_TILEVIEW 0
#define LV_USE_WIN 0

#endif
