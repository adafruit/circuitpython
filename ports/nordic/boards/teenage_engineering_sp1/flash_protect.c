// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "flash_protect.h"

#include "py/mpconfig.h"

#include "nrfx/hal/nrf_acl.h"

// protect the MBR at 0x0 and the bootloader above it, up to the
// address the bootloader jumps to.
#define PROTECT_START_ADDR (MBR_START_ADDR)
#define PROTECT_SIZE       (ISR_START_ADDR - MBR_START_ADDR)

#if PROTECT_SIZE == 0
#error The bootloader for this board is not in low flash; nothing for the ACL to protect.
#endif

#if PROTECT_SIZE > NRF_ACL_REGION_SIZE_MAX
#error The bootloader region is larger than a single ACL region can cover.
#endif

#if (PROTECT_START_ADDR % FLASH_PAGE_SIZE) != 0 || (PROTECT_SIZE % FLASH_PAGE_SIZE) != 0
#error ACL regions must start and end on a flash page boundary.
#endif

void board_flash_protect(void) {
    for (uint32_t region = 0; region < ACL_REGIONS_COUNT; region++) {
        // A region's registers only take their first write after a reset, so a
        // region the bootloader has already configured has to be left alone.
        if (nrf_acl_region_size_get(NRF_ACL, region) != 0 ||
            nrf_acl_region_perm_get(NRF_ACL, region) != 0) {
            continue;
        }

        // Read stays enabled. The MBR's vector table is read out of this
        // region on every interrupt, and the bootloader is executed in place
        // out of it on the way back in.
        nrf_acl_region_set(NRF_ACL, region, PROTECT_START_ADDR, PROTECT_SIZE,
            NRF_ACL_PERM_READ_NO_WRITE);

        // Read back rather than trust the write: an already-claimed region
        // would have ignored it.
        if (nrf_acl_region_address_get(NRF_ACL, region) == PROTECT_START_ADDR &&
            nrf_acl_region_size_get(NRF_ACL, region) == PROTECT_SIZE) {
            return;
        }
    }
}
