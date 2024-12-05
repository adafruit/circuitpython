// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices, Inc.
//
// SPDX-License-Identifier: MIT

#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "supervisor/filesystem.h"
#include "supervisor/flash.h"
#include "supervisor/shared/safe_mode.h"
#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"
#endif

#include "mpconfigboard.h"

// MAX32 HAL Includes
#include "flc.h"
#include "flc_reva.h"
#include "icc.h" // includes icc_<dietype>.c for MSDK die type
#include "mxc_device.h"

/**
 *  NOTE:
 *      ANY function which modifies flash contents must execute from a crit section.
 *      This is because FLC functions are loc'd in RAM, and an ISR executing
 *      from Flash will trigger a HardFault.
 *
 *      An alternative would be to initialize with NVIC_SetRAM(),
 *      which makes ISRs execute from RAM.
 *
 * NOTE:
 *      Additionally, any code that modifies flash contents must disable the
 *      cache. Turn off ICC0 any time flash is to be modified. Remember to re-enable if using.
 *
 *      For Maxim devices which include an additional RISC-V processor, this shall be ignored.
 *      Therefore only ICC0 need be used for the purpose of these functions.
 */

typedef struct {
    const uint32_t base_addr;
    const uint32_t sector_size;
    const uint32_t num_sectors;
} flash_layout_t;

#ifdef MAX32690
// struct layout is the actual layout of flash
// FS Code will use INTERNAL_FLASH_FILESYSTEM_START_ADDR
// and won't conflict with ISR vector in first 16 KiB of flash
static const flash_layout_t flash_layout[] = {
    { 0x10000000, FLASH_PAGE_SIZE, 192},
    // { 0x10300000, 0x2000, 32 }, // RISC-V flash
};
// must be able to hold a full page (for re-writing upon erase)
static uint32_t page_buffer[FLASH_PAGE_SIZE / 4] = {0x0};

#else
#error "Invalid BOARD. Please set BOARD equal to any board under 'boards/'."
#endif

static inline int32_t block2addr(uint32_t block) {
    if (block >= 0 && block < INTERNAL_FLASH_FILESYSTEM_NUM_BLOCKS) {
        return CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR + block * FILESYSTEM_BLOCK_SIZE;
    } else {
        return -1;
    }
}

// Get index, start addr, & size of the flash sector where addr lies
int flash_get_sector_info(uint32_t addr, uint32_t *start_addr, uint32_t *size) {
    // This function should return -1 in the event of errors.
    if (addr >= flash_layout[0].base_addr) {
        uint32_t sector_index = 0;
        if (MP_ARRAY_SIZE(flash_layout) == 1) {
            sector_index = (addr - flash_layout[0].base_addr) / flash_layout[0].sector_size;
            if (sector_index >= flash_layout[0].num_sectors) {
                return -1; // addr is not in flash
            }
            if (start_addr) {
                *start_addr = flash_layout[0].base_addr + (sector_index * flash_layout[0].sector_size);
            } else {
                return -1; // start_addr is NULL
            }
            if (size) {
                *size = flash_layout[0].sector_size;
            } else {
                return -1; // size is NULL
            }
            return sector_index;
        }

        // algorithm for multiple flash sections
        for (uint8_t i = 0; i < MP_ARRAY_SIZE(flash_layout); ++i) {
            for (uint8_t j = 0; j < flash_layout[i].num_sectors; ++j) {
                uint32_t sector_start_next = flash_layout[i].base_addr
                    + (j + 1) * flash_layout[i].sector_size;
                if (addr < sector_start_next) {
                    if (start_addr) {
                        *start_addr = flash_layout[i].base_addr
                            + j * flash_layout[i].sector_size;
                    }
                    if (size) {
                        *size = flash_layout[i].sector_size;
                    }
                    return sector_index;
                }
                ++sector_index;
            }
        }
    }
    return -1;
}

void supervisor_flash_init(void) {
    // No initialization needed.
    // Pay attention to the note at the top of this file!
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return INTERNAL_FLASH_FILESYSTEM_NUM_BLOCKS;
}

void port_internal_flash_flush(void) {

    // Flush all instruction cache
    // ME18 has bug where top-level sysctrl flush bit only works one.
    // Have to use low-level flush bits for each ICC instance.
    MXC_ICC_Flush(MXC_ICC0);
    MXC_ICC_Flush(MXC_ICC1);

    // Clear the line fill buffer by reading 2 pages from flash
    volatile uint32_t *line_addr;
    volatile uint32_t line;
    line_addr = (uint32_t *)(MXC_FLASH_MEM_BASE);
    line = *line_addr;
    line_addr = (uint32_t *)(MXC_FLASH_MEM_BASE + MXC_FLASH_PAGE_SIZE);
    line = *line_addr;
    (void)line; // Silence build warnings that this variable is not used.
}

// Read flash blocks, using cache if it contains the right data
// return 0 on success, non-zero on error
mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    // Find the address of the block we want to read
    int src_addr = block2addr(block);
    if (src_addr == -1) {
        // bad block num
        return 1;
    }

    uint32_t sector_size, sector_start;
    if (flash_get_sector_info(src_addr, &sector_start, &sector_size) == -1) {
        // bad sector idx
        return 2;
    }

    /** NOTE:   The MXC_FLC_Read function executes from SRAM and does some more error checking
    *          than memcpy does. Will use it for now.
    */
    MXC_FLC_Read(src_addr, dest, FILESYSTEM_BLOCK_SIZE * num_blocks);

    return 0; // success
}

// Write to flash blocks
// return 0 on success, non-zero on error
mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    uint32_t error, blocks_left, count, page_start, page_size = 0;

    while (num_blocks > 0) {
        int dest_addr = block2addr(block_num);
        // bad block number passed in
        if (dest_addr == -1) {
            return 1;
        }

        if (flash_get_sector_info(dest_addr, &page_start, &page_size) == -1) {
            reset_into_safe_mode(SAFE_MODE_FLASH_WRITE_FAIL);
        }

        // Find the number of blocks left within this sector
        // BLOCKS_LEFT = (SECTOR_SIZE - BLOCK_OFFSET within sector)) / BLOCK_SIZE
        blocks_left = (page_size - (dest_addr - page_start)) / FILESYSTEM_BLOCK_SIZE;
        count = MIN(num_blocks, blocks_left);

        MXC_ICC_Disable(MXC_ICC0);

        // Buffer the page of flash to erase
        MXC_FLC_Read(page_start, page_buffer, page_size);

        // Erase flash page
        MXC_CRITICAL(
            error = MXC_FLC_PageErase(dest_addr);
            );
        if (error != E_NO_ERROR) {
            // lock flash & reset
            MXC_FLC0->ctrl = (MXC_FLC0->ctrl & ~MXC_F_FLC_REVA_CTRL_UNLOCK) | MXC_S_FLC_REVA_CTRL_UNLOCK_LOCKED;
            reset_into_safe_mode(SAFE_MODE_FLASH_WRITE_FAIL);
        }

        // Copy new src data into the page buffer
        // fill the new data in at the offset dest_addr - page_start
        // account for uint32_t page_buffer vs uint8_t src
        memcpy((page_buffer + (dest_addr - page_start) / 4), src, count * FILESYSTEM_BLOCK_SIZE);

        // Write new page buffer back into flash
        MXC_CRITICAL(
            error = MXC_FLC_Write(page_start, page_size, page_buffer);
            );
        if (error != E_NO_ERROR) {
            // lock flash & reset
            MXC_FLC0->ctrl = (MXC_FLC0->ctrl & ~MXC_F_FLC_REVA_CTRL_UNLOCK) | MXC_S_FLC_REVA_CTRL_UNLOCK_LOCKED;
            reset_into_safe_mode(SAFE_MODE_FLASH_WRITE_FAIL);
        }

        MXC_ICC_Enable(MXC_ICC0);

        block_num += count;
        src += count * FILESYSTEM_BLOCK_SIZE;
        num_blocks -= count;
    }
    return 0; // success
}

// Empty the fs cache
void supervisor_flash_release_cache(void) {
    supervisor_flash_flush();
}
