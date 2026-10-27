// LVGL port for espressif: a dedicated FreeRTOS task pumps lv_timer_handler().
#include "py/mpconfig.h"
#if CIRCUITPY_DISPLAYIO_LVGL
#include "lvgl.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_task.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "py/runtime.h"
#include "core/lv_global.h"
#include "shared-module/displayio/lvgl/lvgl_internal.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/tick.h"

static const char *lvgl_tag = "LVGL";

static esp_timer_handle_t lvgl_tick_timer = NULL;

// Recursive so the shared display code can nest displayio_lvgl_lock() calls and so the render
// task can take the same lock. FreeRTOS recursive mutexes also give priority inheritance, which
// keeps the low-priority CircuitPython task from starving rendering while it holds the lock.
static SemaphoreHandle_t lvgl_mutex = NULL;
static TaskHandle_t lvgl_task_handle = NULL;

// Incremented by a framebuffer's vsync ISR. The task coalesces multiple boundaries into one
// render, so a burst of vsyncs while it was busy doesn't cause extra frames.
static volatile uint32_t lvgl_frame_generation = 0;
static uint32_t lvgl_frame_seen = 0;

// BusDisplay flushes stay on the CircuitPython thread. Its LVGL refresh timer is proxied to this
// background callback, so no supervisor tick is involved.
static background_callback_t lvgl_cp_refresh_cb;

// LVGL's allocator hooks (LV_USE_STDLIB_MALLOC == LV_STDLIB_CUSTOM). Displays and
// their buffers outlive the VM, so they come from the port heap.
void lv_mem_init(void) {
}

void lv_mem_deinit(void) {
}

lv_mem_pool_t lv_mem_add_pool(void *mem, size_t bytes) {
    // Pools aren't supported with a custom allocator.
    LV_UNUSED(mem);
    LV_UNUSED(bytes);
    return NULL;
}

void lv_mem_remove_pool(lv_mem_pool_t pool) {
    LV_UNUSED(pool);
}

// LVGL allocates and frees small blocks (draw tasks, objects, styles) on every frame.
// port_malloc() aligns every PSRAM allocation to MICROPY_BYTES_PER_GC_BLOCK so that heap areas,
// and therefore the bytearrays VideoBuffers stream from, are cache-line aligned. Getting that
// alignment through heap_caps_aligned_alloc() over-allocates and splits off a free remainder on
// every call, which wastes ~64 bytes and shatters PSRAM when it happens millions of times. LVGL
// aligns its own draw buffers inside the allocation (LV_DRAW_BUF_ALIGN), so give it plain
// allocations and leave the aligned port_malloc() path to the GC and DMA buffers.
static void *lvgl_heap_alloc(size_t size) {
    void *ptr = NULL;
    #ifdef CONFIG_SPIRAM
    ptr = heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    #endif
    if (ptr == NULL) {
        ptr = heap_caps_malloc(size, MALLOC_CAP_8BIT);
    }
    return ptr;
}

// A NULL from lv_malloc_core is fatal in most of LVGL (the callers dereference the result), so
// record why it happened and reset into safe mode before LVGL can dereference it.
static MP_NORETURN void lvgl_heap_log_oom(const char *what, size_t size) {
    ESP_LOGE(lvgl_tag, "%s(%u) failed: internal %u free (%u largest), PSRAM %u free (%u largest)",
        what, (unsigned)size,
        (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
        (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
        (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
        (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    reset_into_safe_mode(SAFE_MODE_LVGL_OUT_OF_MEMORY);
}

void *lv_malloc_core(size_t size) {
    void *ptr = lvgl_heap_alloc(size);
    if (ptr == NULL) {
        lvgl_heap_log_oom("lv_malloc", size);
    }
    return ptr;
}

void *lv_realloc_core(void *p, size_t new_size) {
    void *grown = NULL;
    #ifdef CONFIG_SPIRAM
    grown = heap_caps_realloc(p, new_size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
    #endif
    if (grown == NULL) {
        // A failed realloc leaves p alone, so trying the internal heap can't lose it.
        grown = heap_caps_realloc(p, new_size, MALLOC_CAP_8BIT);
    }
    if (grown == NULL) {
        lvgl_heap_log_oom("lv_realloc", new_size);
    }
    return grown;
}

void lv_free_core(void *p) {
    heap_caps_free(p);
}

void lv_mem_monitor_core(lv_mem_monitor_t *mon_p) {
    // LVGL only uses this for its own logging. Report the whole port heap (internal RAM plus
    // PSRAM) rather than pretending to track LVGL's share of it.
    size_t total = heap_caps_get_total_size(MALLOC_CAP_8BIT);
    size_t free_size = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    size_t largest = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    lv_memzero(mon_p, sizeof(*mon_p));
    mon_p->total_size = total;
    mon_p->free_size = free_size;
    mon_p->free_biggest_size = largest;
    mon_p->max_used = total > free_size ? total - free_size : 0;
    mon_p->used_pct = total > 0 ? (uint8_t)((total - free_size) * 100 / total) : 0;
    mon_p->frag_pct = free_size > 0 ? (uint8_t)(100 - largest * 100 / free_size) : 0;
}

lv_result_t lv_mem_test_core(void) {
    return LV_RESULT_OK;
}

static void lvgl_tick_cb(void *arg) {
    lv_tick_inc(5);
}

void displayio_lvgl_port_lock(void) {
    if (lvgl_mutex != NULL) {
        xSemaphoreTakeRecursive(lvgl_mutex, portMAX_DELAY);
    }
}

void displayio_lvgl_port_unlock(void) {
    // The shared code used to count lock depth and made an unmatched unlock a no-op. Keep that
    // property at the port so a future early return can't assert inside FreeRTOS.
    if (lvgl_mutex != NULL && xSemaphoreGetMutexHolder(lvgl_mutex) == (void *)xTaskGetCurrentTaskHandle()) {
        xSemaphoreGiveRecursive(lvgl_mutex);
    }
}

static void lvgl_cp_refresh(void *unused) {
    LV_UNUSED(unused);
    displayio_lvgl_refresh_unsafe_displays();
}

// BusDisplay's LVGL refresh timer normally flushes on the render task. Replace its callback with
// one that stops the timer and asks the CircuitPython thread to run lv_refr_now() instead. The
// timer is resumed by LVGL whenever the display is invalidated, so this stays event driven.
static void lvgl_busdisplay_refr_timer_cb(lv_timer_t *timer) {
    lv_timer_pause(timer);
    background_callback_add(&lvgl_cp_refresh_cb, lvgl_cp_refresh, NULL);
}

void displayio_lvgl_port_install_busdisplay_refresh(lv_display_t *display) {
    lv_timer_t *refr_timer = lv_display_get_refr_timer(display);
    if (refr_timer != NULL) {
        lv_timer_set_cb(refr_timer, lvgl_busdisplay_refr_timer_cb);
    }
}

IRAM_ATTR void displayio_lvgl_port_frame_boundary(void) {
    if (xPortInIsrContext()) {
        lvgl_frame_generation++;
        BaseType_t woken = pdFALSE;
        if (lvgl_task_handle != NULL) {
            vTaskNotifyGiveFromISR(lvgl_task_handle, &woken);
        }
        if (woken == pdTRUE) {
            portYIELD_FROM_ISR();
        }
        return;
    }
    // UVC invokes its frame callback from a background task on the CircuitPython thread. Render
    // directly there; there is no task handoff.
    displayio_lvgl_refresh_paced_displays();
}

static void lvgl_task(void *arg) {
    LV_UNUSED(arg);
    for (;;) {
        uint32_t wait = LV_DEF_REFR_PERIOD;
        if (displayio_lvgl_auto_refresh()) {
            displayio_lvgl_port_lock();
            wait = lv_timer_handler();
            displayio_lvgl_port_unlock();
            // Keep animations and the REPL responsive even when LVGL has nothing to do for a
            // long time. The display refresh timer itself runs at LV_DEF_REFR_PERIOD.
            if (wait == LV_NO_TIMER_READY || wait > LV_DEF_REFR_PERIOD) {
                wait = LV_DEF_REFR_PERIOD;
            }
            if (wait == 0) {
                wait = 1;
            }
        }
        // Invalidations do not wake the task: they are batched until the next timer tick or the
        // framebuffer's frame boundary.
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(wait));
        uint32_t frame_generation = lvgl_frame_generation;
        if (frame_generation != lvgl_frame_seen) {
            lvgl_frame_seen = frame_generation;
            displayio_lvgl_refresh_paced_displays();
        }
    }
}

void displayio_lvgl_port_init(void) {
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;
    // The lock has to exist before the task starts and before any display constructor takes it.
    lvgl_mutex = xSemaphoreCreateRecursiveMutex();
    if (lvgl_mutex == NULL) {
        ESP_LOGE(lvgl_tag, "LVGL mutex create failed");
    }
    lv_init();
    // The LVGL compositor is pumped by the dedicated task below. The supervisor tick stays
    // enabled for filesystem, USB and BLE background work; it does not pump LVGL.
    supervisor_enable_tick();
    // Show which rendering backends LVGL registered. With PPA enabled this includes
    // "ESP_PPA" and confirms the P4/S31 accelerator is available.
    lv_draw_unit_t *unit = LV_GLOBAL_DEFAULT()->draw_info.unit_head;
    while (unit != NULL) {
        ESP_LOGI(lvgl_tag, "draw unit: %s", unit->name);
        unit = unit->next;
    }
    const esp_timer_create_args_t args = {
        .callback = lvgl_tick_cb,
        .name = "lvgl_tick",
    };
    esp_timer_create(&args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 5000); // 5ms

    // The task must run above the CircuitPython task so a tight Python loop can't starve
    // rendering. Pin it to the CircuitPython core as well: background_callback_add() (used by the
    // BusDisplay proxy) is interrupt-safe on a single core, and sharing the core keeps it from
    // racing the CircuitPython thread's callback list. A recursive mutex (priority inheritance)
    // keeps that from blocking the main task for long. 16 KB covers the draw paths plus LVGL's
    // internal compositor state; 8 KB overflowed on complex screens.
    BaseType_t ok = xTaskCreatePinnedToCore(lvgl_task, "lvgl", 16384, NULL,
        ESP_TASK_MAIN_PRIO + 1, &lvgl_task_handle, CONFIG_ESP_MAIN_TASK_AFFINITY);
    if (ok != pdPASS) {
        lvgl_task_handle = NULL;
        ESP_LOGE(lvgl_tag, "LVGL task create failed");
    } else {
        ESP_LOGI(lvgl_tag, "render task started");
    }
}
#endif
