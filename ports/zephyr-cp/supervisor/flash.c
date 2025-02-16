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

#define CIRCUITPY_PARTITION circuitpy_partition
static struct flash_area *filesystem_area = NULL;

#if !FIXED_PARTITION_EXISTS(CIRCUITPY_PARTITION)
static struct flash_area _dynamic_area;
#endif

// Auto generated in pins.c
extern const struct device *const flashes[];
extern const int circuitpy_flash_device_count;

// Size of an erase area
static size_t _page_size;

// Size of a write area
static size_t _row_size;

// Number of file system blocks in a page.
static size_t _blocks_per_page;
static size_t _rows_per_block;
static uint32_t _page_mask;

#define NO_PAGE_LOADED 0xFFFFFFFF

// The currently cached sector in the cache, ram or flash based.
static uint32_t _current_page_address;

static uint32_t _scratch_page_address;

// Track which blocks (up to 32) in the current sector currently live in the
// cache.
static uint32_t _dirty_mask;
static uint32_t _loaded_mask;

// Table of pointers to each cached block. Should be zero'd after allocation.
#define FLASH_CACHE_TABLE_NUM_ENTRIES (_blocks_per_page * _rows_per_block)
#define FLASH_CACHE_TABLE_SIZE (FLASH_CACHE_TABLE_NUM_ENTRIES * sizeof (uint8_t *))
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
    flash_area_foreach(fa_cb, covered_by_areas);
    for (int i = 0; i < circuitpy_flash_device_count; i++) {
        const struct device *d = flashes[i];

        printk("flash %p %s\n", d, d->name);
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

    const struct device *d = flash_area_get_device(filesystem_area);
    _row_size = flash_get_write_block_size(d);
    if (_row_size < 256) {
        if (256 % _row_size == 0) {
            _row_size = 256;
        } else {
            size_t new_row_size = _row_size;
            while (new_row_size < 256) {
                new_row_size += _row_size;
            }
            _row_size = new_row_size;
        }
    }
    struct flash_pages_info first_info;
    flash_get_page_info_by_offs(d, filesystem_area->fa_off, &first_info);
    struct flash_pages_info last_info;
    flash_get_page_info_by_offs(d, filesystem_area->fa_off + filesystem_area->fa_size - _row_size, &last_info);
    _page_size = first_info.size;
    if (_page_size < FILESYSTEM_BLOCK_SIZE) {
        _page_size = FILESYSTEM_BLOCK_SIZE;
    }
    printk("  erase page size %d\n", _page_size);
    // Makes sure that a cached page has 32 or fewer rows. Our dirty mask is
    // only 32 bits.
    while (_page_size / _row_size > 32) {
        _row_size *= 2;
    }
    printk("  write row size %d\n", _row_size);
    _blocks_per_page = _page_size / FILESYSTEM_BLOCK_SIZE;
    printk("  blocks per page %d\n", _blocks_per_page);
    _rows_per_block = FILESYSTEM_BLOCK_SIZE / _row_size;
    _page_mask = ~(_page_size - 1);
    // The last page is the scratch sector.
    _scratch_page_address = last_info.start_offset;
    _current_page_address = NO_PAGE_LOADED;
}

uint32_t supervisor_flash_get_block_size(void) {
    return 512;
}

uint32_t supervisor_flash_get_block_count(void) {
    if (filesystem_area == NULL) {
        return 0;
    }
    return (_scratch_page_address - filesystem_area->fa_off) / 512;
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
        for (uint16_t i = 0; i < 4; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
        }
    } else {
        return false;
    }

    // Now check the full length.
    uint8_t full_buffer[FILESYSTEM_BLOCK_SIZE];
    if (read_flash(sector_address, full_buffer, FILESYSTEM_BLOCK_SIZE)) {
        for (uint16_t i = 0; i < FILESYSTEM_BLOCK_SIZE; i++) {
            if (short_buffer[i] != 0xff) {
                return false;
            }
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

// Sector is really 24 bits.
static bool copy_block(uint32_t src_address, uint32_t dest_address) {
    // Copy row by row to minimize RAM buffer.
    uint8_t buffer[_row_size];
    for (uint32_t i = 0; i < FILESYSTEM_BLOCK_SIZE / _row_size; i++) {
        if (!read_flash(src_address + i * _row_size, buffer, _row_size)) {
            return false;
        }
        if (!write_flash(dest_address + i * _row_size, buffer, _row_size)) {
            return false;
        }
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
        if ((_dirty_mask & (1 << i)) == 0) {
            copy_to_scratch_ok = copy_to_scratch_ok &&
                copy_block(_current_page_address + i * FILESYSTEM_BLOCK_SIZE,
                _scratch_page_address + i * FILESYSTEM_BLOCK_SIZE);
        }
        _loaded_mask |= (1 << i);
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
    _loaded_mask = 0;
}

// Attempts to allocate a new set of page buffers for caching a full sector in
// ram. Each page is allocated separately so that the GC doesn't need to provide
// one huge block. We can free it as we write if we want to also.
static bool allocate_ram_cache(void) {
    flash_cache_table = port_malloc(FLASH_CACHE_TABLE_SIZE, false);
    if (flash_cache_table == NULL) {
        // Not enough space even for the cache table.
        return false;
    }

    // Clear all the entries so it's easy to find the last entry.
    memset(flash_cache_table, 0, FLASH_CACHE_TABLE_SIZE);

    bool success = true;
    for (size_t i = 0; i < _blocks_per_page && success; i++) {
        for (size_t j = 0; j < _rows_per_block && success; j++) {
            uint8_t *page_cache = port_malloc(_row_size, false);
            if (page_cache == NULL) {
                success = false;
                break;
            }
            flash_cache_table[i * _rows_per_block + j] = page_cache;
        }
    }

    // We couldn't allocate enough so give back what we got.
    if (!success) {
        release_ram_cache();
    }
    _loaded_mask = 0;
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

    if (_current_page_address == NO_PAGE_LOADED || _dirty_mask == 0) {
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
        if ((_loaded_mask & (1 << i)) == 0) {
            for (size_t j = 0; j < _rows_per_block; j++) {
                copy_to_ram_ok = read_flash(
                    _current_page_address + (i * _rows_per_block + j) * _row_size,
                    flash_cache_table[i * _rows_per_block + j],
                    _row_size);
                if (!copy_to_ram_ok) {
                    break;
                }
            }
        }
        if (!copy_to_ram_ok) {
            break;
        }
        _loaded_mask |= (1 << i);
    }

    if (!copy_to_ram_ok) {
        return false;
    }
    // Second, erase the current sector.
    erase_page(_current_page_address);
    // Lastly, write all the data in ram that we've cached.
    for (size_t i = 0; i < _blocks_per_page; i++) {
        for (size_t j = 0; j < _rows_per_block; j++) {
            write_flash(_current_page_address + (i * _rows_per_block + j) * _row_size,
                flash_cache_table[i * _rows_per_block + j],
                _row_size);
        }
    }
    // Nothing is dirty anymore. Some may already be in the cache cleanly.
    _dirty_mask = 0;

    // We're done with the cache for now so give it back.
    if (!keep_cache) {
        release_ram_cache();
    }
    return true;
}

// Delegates to the correct flash flush method depending on the existing cache.
// TODO Don't blink the status indicator if we don't actually do any writing (hard to tell right now).
static void _flush_keep_cache(bool keep_cache) {
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
    uint32_t mask = 1 << (block_index);
    // We're reading from the currently cached sector.
    if (_current_page_address == page_address && (mask & _loaded_mask) > 0) {
        if (flash_cache_table != NULL) {
            for (int i = 0; i < _rows_per_block; i++) {
                memcpy(dest + i * _row_size,
                    flash_cache_table[block_index * _rows_per_block + i],
                    _row_size);
            }
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
    uint32_t mask = 1 << (block_index);
    // Flush the cache if we're moving onto a different page.
    if (_current_page_address != page_address) {
        // Check to see if we'd write to an erased block and aren't writing to
        // our cache. In that case we can write directly.
        if (block_erased(address)) {
            return write_flash(address, data, FILESYSTEM_BLOCK_SIZE);
        }
        if (_current_page_address != NO_PAGE_LOADED) {
            supervisor_flash_flush();
        }
        if (flash_cache_table == NULL && !allocate_ram_cache()) {
            erase_page(_scratch_page_address);
        }
        _current_page_address = page_address;
        _dirty_mask = 0;
        _loaded_mask = 0;
    }
    _dirty_mask |= mask;
    _loaded_mask |= mask;

    // Copy the block to the appropriate cache.
    if (flash_cache_table != NULL) {
        for (int i = 0; i < _rows_per_block; i++) {
            memcpy(flash_cache_table[block_index * _rows_per_block + i],
                data + i * _row_size,
                _row_size);
        }
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
