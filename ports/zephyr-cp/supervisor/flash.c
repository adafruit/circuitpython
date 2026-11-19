// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT
#include "supervisor/flash.h"

#include <stdint.h>
#include <string.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"

#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>

// The filesystem partition node label comes from the Adaboot fork's layout
// dtsi (and the native simulator layouts). Prefer the explicit fatfs/littlefs
// names, then fall back to the older circuitpy_partition name some boards use,
// and finally to runtime discovery of an unpartitioned flash device.
#if FIXED_PARTITION_EXISTS(littlefs_partition)
#define CIRCUITPY_PARTITION littlefs_partition
#elif FIXED_PARTITION_EXISTS(fatfs_partition)
#define CIRCUITPY_PARTITION fatfs_partition
#elif FIXED_PARTITION_EXISTS(circuitpy_partition)
#define CIRCUITPY_PARTITION circuitpy_partition
#endif

// Check that the filesystem partition starts and ends on a layout page
// boundary: flash.c addresses erase pages relative to the start of the
// partition. The NOR flash drivers set their layout page size in Kconfig, so
// they can be checked at build time. (Other drivers report it only at run
// time.)
#if FIXED_PARTITION_EXISTS(CIRCUITPY_PARTITION)
#define CIRCUITPY_PARTITION_NODE DT_NODELABEL(CIRCUITPY_PARTITION)
#define CIRCUITPY_PARTITION_MTD DT_MTD_FROM_FIXED_PARTITION(CIRCUITPY_PARTITION_NODE)
#if defined(CONFIG_NORDIC_QSPI_NOR) && DT_NODE_HAS_COMPAT(CIRCUITPY_PARTITION_MTD, nordic_qspi_nor)
#define CIRCUITPY_LAYOUT_PAGE_SIZE CONFIG_NORDIC_QSPI_NOR_FLASH_LAYOUT_PAGE_SIZE
#elif defined(CONFIG_SPI_NOR) && DT_NODE_HAS_COMPAT(CIRCUITPY_PARTITION_MTD, jedec_spi_nor)
#define CIRCUITPY_LAYOUT_PAGE_SIZE CONFIG_SPI_NOR_FLASH_LAYOUT_PAGE_SIZE
#elif defined(CONFIG_FLASH_MSPI_NOR) && DT_NODE_HAS_COMPAT(CIRCUITPY_PARTITION_MTD, jedec_nor)
#define CIRCUITPY_LAYOUT_PAGE_SIZE CONFIG_FLASH_MSPI_NOR_LAYOUT_PAGE_SIZE
#endif
#ifdef CIRCUITPY_LAYOUT_PAGE_SIZE
BUILD_ASSERT(DT_REG_ADDR(CIRCUITPY_PARTITION_NODE) % CIRCUITPY_LAYOUT_PAGE_SIZE == 0 &&
    DT_REG_SIZE(CIRCUITPY_PARTITION_NODE) % CIRCUITPY_LAYOUT_PAGE_SIZE == 0,
    "filesystem partition must start and end on a flash layout page boundary");
#endif
#endif
static const struct flash_area *filesystem_area = NULL;

#if !FIXED_PARTITION_EXISTS(CIRCUITPY_PARTITION)
static struct flash_area _dynamic_area;
#endif

// Auto generated in pins.c
extern const struct device *const flashes[];
extern const int circuitpy_flash_device_count;

// Size of an erase page.
static size_t _page_size;

// The value a flash byte has after being erased (usually 0xFF, but can differ).
static uint8_t _erase_value;

// Number of FILESYSTEM_BLOCK_SIZE blocks in an erase page.
static size_t _blocks_per_page;
static uint32_t _page_mask;

#define NO_PAGE_LOADED 0xFFFFFFFF

// The currently cached sector in the cache, ram or flash based.
static uint32_t _current_page_address;

static uint32_t _scratch_page_address;

// Size of the filesystem in bytes: every whole page, less the scratch page if
// there is one.
static uint32_t _filesystem_size;

static bool allocate_ram_cache(void);

// Per-block flags packed into a uint8_t array (one byte per block).
#define ROW_LOADED  0x01  // Block data is present in the cache.
#define ROW_DIRTY   0x02  // Block has been modified since last flush.
#define ROW_ERASED  0x04  // Block is all 0xFF; no need to write after erase.

static uint8_t *_row_flags = NULL;

static inline void _clear_row_flags(void) {
    if (_row_flags != NULL) {
        memset(_row_flags, 0, _blocks_per_page);
    }
}

static inline bool _any_dirty(void) {
    for (size_t i = 0; i < _blocks_per_page; i++) {
        if (_row_flags[i] & ROW_DIRTY) {
            return true;
        }
    }
    return false;
}

// Check if a buffer is entirely the erase value.
static bool _buffer_is_erased(const uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (buf[i] != _erase_value) {
            return false;
        }
    }
    return true;
}

// Table of pointers to each cached block. Should be zero'd after allocation.
#define FLASH_CACHE_TABLE_NUM_ENTRIES (_blocks_per_page)
#define FLASH_CACHE_TABLE_SIZE (FLASH_CACHE_TABLE_NUM_ENTRIES * sizeof(uint8_t *))
static uint8_t **flash_cache_table = NULL;

static K_MUTEX_DEFINE(_mutex);

static void fa_cb(const struct flash_area *fa, void *user_data) {
    bool *covered_by_areas = user_data;

    const char *fa_label = flash_area_label(fa);

    if (fa_label == NULL) {
        fa_label = "-";
    }
    printk("%s %p %d dev %p\n", fa_label, fa, fa->fa_id, fa->fa_dev);

    for (int i = 0; i < circuitpy_flash_device_count; i++) {
        const struct device *d = flashes[i];

        if (d == fa->fa_dev) {
            covered_by_areas[i] = true;
        }
    }

    uint32_t count = 10;
    struct flash_sector sectors[count];
    if (flash_area_get_sectors(fa->fa_id, &count, sectors) != 0) {
        printk("Unable to get sectors\n");
    } else {
        for (int i = 0; i < count; i++) {
            printk("  0x%lx 0x%x\n", sectors[i].fs_off, sectors[i].fs_size);
        }
    }
}

void supervisor_flash_init(void) {
    if (filesystem_area != NULL) {
        return;
    }

    #if FIXED_PARTITION_EXISTS(CIRCUITPY_PARTITION)
    int open_res = flash_area_open(FIXED_PARTITION_ID(CIRCUITPY_PARTITION), &filesystem_area);
    if (open_res != 0) {
        printk("Unable to open CIRCUITPY flash area: %d\n", open_res);
    }
    #else
    // Use spi_nor if it exists and no
    // flash_area_open(FIXED_PARTITION_ID(storage_partition), &filesystem_area);
    // printk("flash area %d %d\n", filesystem_area->fa_id, filesystem_area->fa_size);
    printk("hello from flash init\n");
    bool covered_by_areas[circuitpy_flash_device_count];
    // fa_cb only sets entries to true, so start from all false.
    memset(covered_by_areas, 0, sizeof(covered_by_areas));
    flash_area_foreach(fa_cb, covered_by_areas);
    for (int i = 0; i < circuitpy_flash_device_count; i++) {
        const struct device *d = flashes[i];

        printk("flash %p %s\n", d, d->name);
        if (!device_is_ready(d)) {
            printk("  not ready\n");
            continue;
        }
        if (covered_by_areas[i]) {
            printk("  covered by flash area\n");
            continue;
        }
        if (d->api == NULL) {
            printk("  no api\n");
            continue;
        }
        size_t page_count = flash_get_page_count(d);
        printk("  %d pages\n", page_count);
        if (page_count == 0) {
            continue;
        }
        struct flash_pages_info first_info;
        flash_get_page_info_by_idx(d, 0, &first_info);
        printk("  page 0: %lx %x\n", first_info.start_offset, first_info.size);
        struct flash_pages_info last_info;
        flash_get_page_info_by_idx(d, page_count - 1, &last_info);
        printk("  page %d: %lx %x\n", page_count - 1, last_info.start_offset, last_info.size);

        // Assume uniform page sizes if the first and last are the same size.
        size_t uniform_page_count;
        if (first_info.size == last_info.size) {
            uniform_page_count = page_count;
        } else {
            for (size_t i = 1; i < page_count; i++) {
                struct flash_pages_info info;
                flash_get_page_info_by_idx(d, i, &info);
                if (info.size != first_info.size) {
                    uniform_page_count = i;
                    break;
                }
            }
        }
        if (uniform_page_count * first_info.size < 64 * 1024) {
            printk("Uniform region too small\n");
            continue;
        }

        printk("  %d uniform pages\n", uniform_page_count);
        _page_size = first_info.size;

        _dynamic_area.fa_dev = d;
        _dynamic_area.fa_id = 0;
        _dynamic_area.fa_off = 0;
        _dynamic_area.fa_size = uniform_page_count * first_info.size;
        filesystem_area = &_dynamic_area;
        printk("setup flash\n");
        break;
    }
    #endif
    if (filesystem_area == NULL) {
        printk("no flash found for filesystem\n");
        return;
    }

    struct flash_pages_info first_info;
    const struct device *d = flash_area_get_device(filesystem_area);
    const struct flash_parameters *fp = flash_get_parameters(d);
    _erase_value = fp->erase_value;
    flash_get_page_info_by_offs(d, filesystem_area->fa_off, &first_info);
    _page_size = first_info.size;
    if (_page_size < FILESYSTEM_BLOCK_SIZE) {
        _page_size = FILESYSTEM_BLOCK_SIZE;
    }
    _blocks_per_page = _page_size / FILESYSTEM_BLOCK_SIZE;
    _page_mask = ~(_page_size - 1);
    printk("  erase page size %d\n", _page_size);
    printk("  blocks per page %d\n", _blocks_per_page);

    // Page addresses are relative to the start of the area. If the area
    // doesn't end on a page boundary, its last partial page can't be erased
    // without going past the end, so it is left unused.
    size_t page_count = filesystem_area->fa_size / _page_size;
    size_t min_page_count = IS_ENABLED(CONFIG_CIRCUITPY_FLASH_SCRATCH_PAGE) ? 2 : 1;
    if (page_count < min_page_count) {
        printk("Filesystem area too small (%zu pages)\n", page_count);
        filesystem_area = NULL;
        return;
    }

    _row_flags = port_malloc(_blocks_per_page, false);
    if (_row_flags == NULL) {
        printk("Unable to allocate row flags (%d bytes)\n", _blocks_per_page);
        filesystem_area = NULL;
        return;
    }
    memset(_row_flags, 0, _blocks_per_page);

    _current_page_address = NO_PAGE_LOADED;

    #if CONFIG_CIRCUITPY_FLASH_SCRATCH_PAGE
    // The last whole page is the scratch page.
    _scratch_page_address = (page_count - 1) * _page_size;
    _filesystem_size = _scratch_page_address;
    #else
    // No scratch page: allocate the RAM cache now and keep it, so writes
    // never need one.
    if (!allocate_ram_cache()) {
        printk("Unable to allocate the filesystem ram cache\n");
        filesystem_area = NULL;
        return;
    }
    _filesystem_size = page_count * _page_size;
    #endif
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    if (filesystem_area == NULL) {
        return 0;
    }
    return _filesystem_size / FILESYSTEM_BLOCK_SIZE;
}


// Read data_length's worth of bytes starting at address into data.
static bool read_flash(uint32_t address, uint8_t *data, uint32_t data_length) {
    int res = flash_area_read(filesystem_area, address, data, data_length);
    if (res != 0) {
        printk("flash read failed: %d\n", res);
        printk("  address %x length %d\n", address, data_length);
    }
    return res == 0;
}

// Writes data_length's worth of bytes starting at address from data. Assumes
// that the sector that address resides in has already been erased. So make sure
// to run erase_page.
static bool write_flash(uint32_t address, const uint8_t *data, uint32_t data_length) {
    int res = flash_area_write(filesystem_area, address, data, data_length);
    if (res != 0) {
        printk("flash write failed: %d\n", res);
        printk("  address %x length %d\n", address, data_length);
    }
    return res == 0;
}

static bool block_erased(uint32_t sector_address) {
    uint8_t short_buffer[4];
    if (read_flash(sector_address, short_buffer, 4)) {
        if (!_buffer_is_erased(short_buffer, 4)) {
            return false;
        }
    } else {
        return false;
    }

    // Now check the full length.
    uint8_t full_buffer[FILESYSTEM_BLOCK_SIZE];
    if (read_flash(sector_address, full_buffer, FILESYSTEM_BLOCK_SIZE)) {
        if (!_buffer_is_erased(full_buffer, FILESYSTEM_BLOCK_SIZE)) {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

// Erases the given sector. Make sure you copied all of the data out of it you
// need! Also note, sector_address is really 24 bits.
static bool erase_page(uint32_t sector_address) {
    int res = flash_area_erase(filesystem_area, sector_address, _page_size);
    if (res != 0) {
        printk("Erase of %d failed: %d\n", sector_address, res);
    }
    return res == 0;
}

static bool copy_block(uint32_t src_address, uint32_t dest_address) {
    uint8_t buffer[FILESYSTEM_BLOCK_SIZE];
    if (!read_flash(src_address, buffer, FILESYSTEM_BLOCK_SIZE)) {
        return false;
    }
    if (!write_flash(dest_address, buffer, FILESYSTEM_BLOCK_SIZE)) {
        return false;
    }
    return true;
}

// Load a block into the ram cache and set its flags. Returns false on read error.
static bool _load_block_into_cache(size_t block_index) {
    if (!read_flash(_current_page_address + block_index * FILESYSTEM_BLOCK_SIZE,
        flash_cache_table[block_index], FILESYSTEM_BLOCK_SIZE)) {
        return false;
    }
    _row_flags[block_index] |= ROW_LOADED;
    if (_buffer_is_erased(flash_cache_table[block_index], FILESYSTEM_BLOCK_SIZE)) {
        _row_flags[block_index] |= ROW_ERASED;
    }
    return true;
}

// Flush the cache that was written to the scratch portion of flash. Only used
// when ram is tight.
static bool flush_scratch_flash(void) {
    if (_current_page_address == NO_PAGE_LOADED) {
        return true;
    }
    // First, copy out any blocks that we haven't touched from the sector we've
    // cached.
    bool copy_to_scratch_ok = true;
    for (size_t i = 0; i < _blocks_per_page; i++) {
        if (!(_row_flags[i] & ROW_DIRTY)) {
            copy_to_scratch_ok = copy_to_scratch_ok &&
                copy_block(_current_page_address + i * FILESYSTEM_BLOCK_SIZE,
                _scratch_page_address + i * FILESYSTEM_BLOCK_SIZE);
        }
        _row_flags[i] |= ROW_LOADED;
    }
    if (!copy_to_scratch_ok) {
        // TODO(tannewt): Do more here. We opted to not erase and copy bad data
        // in. We still risk losing the data written to the scratch sector.
        return false;
    }
    // Second, erase the current sector.
    erase_page(_current_page_address);
    // Finally, copy the new version into it.
    for (size_t i = 0; i < _blocks_per_page; i++) {
        copy_block(_scratch_page_address + i * FILESYSTEM_BLOCK_SIZE,
            _current_page_address + i * FILESYSTEM_BLOCK_SIZE);
    }
    // Every scratch block is now written, so the next write must erase the
    // scratch page first. Unload the page so that it does.
    _current_page_address = NO_PAGE_LOADED;
    _clear_row_flags();
    return true;
}

// Free all entries in the partially or completely filled flash_cache_table, and then free the table itself.
static void release_ram_cache(void) {
    if (flash_cache_table == NULL) {
        return;
    }

    for (size_t i = 0; i < FLASH_CACHE_TABLE_NUM_ENTRIES; i++) {
        // Table may not be completely full. Stop at first NULL entry.
        if (flash_cache_table[i] == NULL) {
            break;
        }
        port_free(flash_cache_table[i]);
    }
    port_free(flash_cache_table);
    flash_cache_table = NULL;
    _current_page_address = NO_PAGE_LOADED;
    _clear_row_flags();
}

// Attempts to allocate a new set of page buffers for caching a full sector in
// ram. Each page is allocated separately so that the GC doesn't need to provide
// one huge block. We can free it as we write if we want to also.
static bool allocate_ram_cache(void) {
    flash_cache_table = port_malloc(FLASH_CACHE_TABLE_SIZE, false);
    if (flash_cache_table == NULL) {
        printk("Unable to allocate ram cache table\n");
        return false;
    }

    // Clear all the entries so it's easy to find the last entry.
    memset(flash_cache_table, 0, FLASH_CACHE_TABLE_SIZE);

    bool success = true;
    for (size_t i = 0; i < _blocks_per_page && success; i++) {
        uint8_t *block_cache = port_malloc(FILESYSTEM_BLOCK_SIZE, false);
        if (block_cache == NULL) {
            success = false;
            break;
        }
        flash_cache_table[i] = block_cache;
    }

    // We couldn't allocate enough so give back what we got.
    if (!success) {
        printk("Unable to allocate ram cache pages\n");
        release_ram_cache();
    }
    _clear_row_flags();
    _current_page_address = NO_PAGE_LOADED;
    return success;
}

// Flush the cached sector from ram onto the flash. We'll free the cache unless
// keep_cache is true.
static bool flush_ram_cache(bool keep_cache) {
    if (flash_cache_table == NULL) {
        // Nothing to flush because there is no cache.
        return true;
    }

    if (_current_page_address == NO_PAGE_LOADED || !_any_dirty()) {
        if (!keep_cache) {
            release_ram_cache();
        }
        return true;
    }
    // First, copy out any blocks that we haven't touched from the sector
    // we've cached. If we don't do this we'll erase the data during the sector
    // erase below.
    bool copy_to_ram_ok = true;
    for (size_t i = 0; i < _blocks_per_page; i++) {
        if (!(_row_flags[i] & ROW_LOADED)) {
            if (!_load_block_into_cache(i)) {
                copy_to_ram_ok = false;
                break;
            }
        }
    }

    if (!copy_to_ram_ok) {
        return false;
    }
    // Second, erase the current sector.
    erase_page(_current_page_address);
    // Lastly, write all the data in ram that we've cached.
    for (size_t i = 0; i < _blocks_per_page; i++) {
        // Skip blocks that are entirely erased — the page erase already
        // set them to 0xFF so writing would be redundant.
        if (_row_flags[i] & ROW_ERASED) {
            continue;
        }
        write_flash(_current_page_address + i * FILESYSTEM_BLOCK_SIZE,
            flash_cache_table[i],
            FILESYSTEM_BLOCK_SIZE);
    }
    // Nothing is dirty anymore. Clear dirty but keep loaded and erased.
    for (size_t i = 0; i < _blocks_per_page; i++) {
        _row_flags[i] &= ~ROW_DIRTY;
    }

    // We're done with the cache for now so give it back.
    if (!keep_cache) {
        release_ram_cache();
    }
    return true;
}

// Delegates to the correct flash flush method depending on the existing cache.
// TODO Don't blink the status indicator if we don't actually do any writing (hard to tell right now).
static void _flush_keep_cache(bool keep_cache) {
    #if !CONFIG_CIRCUITPY_FLASH_SCRATCH_PAGE
    // Without a scratch page, the RAM cache is never released.
    keep_cache = true;
    #endif
    #ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, true);
    #endif
    // If we've cached to the flash itself flush from there.
    if (flash_cache_table == NULL) {
        flush_scratch_flash();
    } else {
        flush_ram_cache(keep_cache);
    }
    #ifdef MICROPY_HW_LED_MSC
    port_pin_set_output_level(MICROPY_HW_LED_MSC, false);
    #endif
}

void port_internal_flash_flush(void) {
    if (filesystem_area == NULL) {
        return;
    }
    k_mutex_lock(&_mutex, K_FOREVER);
    _flush_keep_cache(true);
    k_mutex_unlock(&_mutex);
}

void supervisor_flash_release_cache(void) {
    if (filesystem_area == NULL) {
        return;
    }
    k_mutex_lock(&_mutex, K_FOREVER);
    _flush_keep_cache(false);
    k_mutex_unlock(&_mutex);
}

static int32_t convert_block_to_flash_addr(uint32_t block) {
    if (0 <= block && block < supervisor_flash_get_block_count()) {
        // a block in partition 1
        return block * FILESYSTEM_BLOCK_SIZE;
    }
    // bad block
    return -1;
}

static bool _flash_read_block(uint8_t *dest, uint32_t block) {
    int32_t address = convert_block_to_flash_addr(block);
    if (address == -1) {
        // bad block number
        return false;
    }

    // Mask out the lower bits that designate the address within the sector.
    uint32_t page_address = address & _page_mask;
    size_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % _blocks_per_page;
    // We're reading from the currently cached sector.
    if (_current_page_address == page_address && (_row_flags[block_index] & ROW_LOADED)) {
        if (flash_cache_table != NULL) {
            memcpy(dest, flash_cache_table[block_index], FILESYSTEM_BLOCK_SIZE);
            return true;
        }
        uint32_t scratch_block_address = _scratch_page_address + block_index * FILESYSTEM_BLOCK_SIZE;
        return read_flash(scratch_block_address, dest, FILESYSTEM_BLOCK_SIZE);
    }
    return read_flash(address, dest, FILESYSTEM_BLOCK_SIZE);
}

static bool _flash_write_block(const uint8_t *data, uint32_t block) {
    // Non-MBR block, copy to cache
    int32_t address = convert_block_to_flash_addr(block);
    if (address == -1) {
        // bad block number
        return false;
    }
    // Mask out the lower bits that designate the address within the sector.
    uint32_t page_address = address & _page_mask;
    size_t block_index = (address / FILESYSTEM_BLOCK_SIZE) % _blocks_per_page;
    // Flush the cache if we're moving onto a different page, or if we're
    // writing the same block again while caching in the scratch page: flash
    // can't be rewritten without an erase, so the second write must start a
    // fresh scratch page.
    bool scratch_rewrite = flash_cache_table == NULL &&
        _current_page_address == page_address &&
        (_row_flags[block_index] & ROW_DIRTY);
    if (_current_page_address != page_address || scratch_rewrite) {
        // Check to see if we'd write to an erased block and aren't writing to
        // our cache. In that case we can write directly. Do not take this fast
        // path when the target block is part of the currently cached page: the
        // cache copy would go stale and later reads would return the old data.
        if (block_erased(address) &&
            !(_current_page_address == page_address && (_row_flags[block_index] & ROW_LOADED))) {
            return write_flash(address, data, FILESYSTEM_BLOCK_SIZE);
        }
        if (_current_page_address != NO_PAGE_LOADED) {
            // Flush only the old page. supervisor_flash_flush() would also
            // clear the dirty flag, which littlefs sets before writing.
            _flush_keep_cache(true);
        }
        if (flash_cache_table == NULL && !allocate_ram_cache()) {
            erase_page(_scratch_page_address);
        }
        _current_page_address = page_address;
        _clear_row_flags();
    }
    _row_flags[block_index] = ROW_DIRTY | ROW_LOADED;
    // Copy the block to the appropriate cache.
    if (flash_cache_table != NULL) {
        memcpy(flash_cache_table[block_index], data, FILESYSTEM_BLOCK_SIZE);
        return true;
    } else {
        uint32_t scratch_block_address = _scratch_page_address + block_index * FILESYSTEM_BLOCK_SIZE;
        return write_flash(scratch_block_address, data, FILESYSTEM_BLOCK_SIZE);
    }
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    if (filesystem_area == NULL) {
        return 1;
    }
    k_mutex_lock(&_mutex, K_FOREVER);
    for (size_t i = 0; i < num_blocks; i++) {
        if (!_flash_read_block(dest + i * FILESYSTEM_BLOCK_SIZE, block + i)) {
            k_mutex_unlock(&_mutex);
            printk("error reading block %04d\n", block + i);
            return 1; // error
        }
    }
    k_mutex_unlock(&_mutex);
    return 0; // success
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block, uint32_t num_blocks) {
    if (filesystem_area == NULL) {
        return 1;
    }
    k_mutex_lock(&_mutex, K_FOREVER);
    for (size_t i = 0; i < num_blocks; i++) {
        if (!_flash_write_block(src + i * FILESYSTEM_BLOCK_SIZE, block + i)) {
            printk("error writing block %04d\n", block + i);
            k_mutex_unlock(&_mutex);
            return 1; // error
        }
    }
    k_mutex_unlock(&_mutex);
    return 0; // success
}
