#pragma once

#include "hardware/regs/addressmap.h"
#include "shared-module/memorymap/__init__.h"

// RP2 address map ranges, must be arranged in order by ascending start address
#if PICO_RP2040
const static memorymap_range_t memorymap_ranges[] = {
    {(uint8_t *)ROM_BASE,           0x00004000, 1},        // boot ROM
    {(uint8_t *)XIP_BASE,           0x00100000},        // XIP normal cache operation
    {(uint8_t *)XIP_NOALLOC_BASE,   0x00100000},        // XIP check for hit, no update on miss
    {(uint8_t *)XIP_NOCACHE_BASE,   0x00100000},        // XIP don't check for hit, no update on miss
    {(uint8_t *)XIP_NOCACHE_NOALLOC_BASE, 0x00100000},  // XIP bypass cache completely
    {(uint8_t *)XIP_CTRL_BASE,      0x00004000},         // XIP control registers
    {(uint8_t *)XIP_SRAM_BASE,      0x00004000},       // XIP SRAM 16KB XIP cache
    {(uint8_t *)XIP_SSI_BASE,       0x00004000},         // XIP SSI registers
    {(uint8_t *)SRAM_BASE,          0x00042000},       // SRAM 256KB striped plus 16KB contiguous
    {(uint8_t *)SRAM0_BASE,         0x00040000},       // SRAM0 to SRAM3 256KB non-striped
    {(uint8_t *)SYSINFO_BASE,       0x00070000},         // APB peripherals
    {(uint8_t *)DMA_BASE,           0x00004000},         // DMA registers
    {(uint8_t *)USBCTRL_DPRAM_BASE, 0x00001000},       // USB DPSRAM 4KB
    {(uint8_t *)USBCTRL_REGS_BASE,  0x00004000},         // USB registers
    {(uint8_t *)PIO0_BASE,          0x00004000},         // PIO0 registers
    {(uint8_t *)PIO1_BASE,          0x00004000},         // PIO1 registers
    {(uint8_t *)SIO_BASE,           0x00001000},         // SIO registers, no aliases
    {(uint8_t *)PPB_BASE,           0x00004000}          // PPB registers
};
#endif
#if PICO_RP2350
const static memorymap_range_t memorymap_ranges[] = {
    {(uint8_t *)ROM_BASE,           0x00004000, 1},        // boot ROM
    {(uint8_t *)XIP_BASE,           0x00100000},        // XIP normal cache operation
    {(uint8_t *)XIP_NOCACHE_NOALLOC_BASE, 0x00100000},  // XIP bypass cache completely
    {(uint8_t *)XIP_MAINTENANCE_BASE,     0x00100000},  // XIP cache maintenance based on lower 3 address bits. Data is ignored
    {(uint8_t *)XIP_NOCACHE_NOALLOC_NOTRANSLATE_BASE, 0x00100000},  // XIP skip cache and address translation
    {(uint8_t *)SRAM_BASE,          SRAM_END - SRAM_BASE},       // SRAM 256KB striped plus 16KB contiguous
    {(uint8_t *)SYSINFO_BASE,       0x00070000},         // APB peripherals
    {(uint8_t *)XIP_CTRL_BASE,      0x00004000},         // XIP control registers
    {(uint8_t *)XIP_QMI_BASE,       0x00004000},         // XIP QMI registers
    {(uint8_t *)DMA_BASE,           0x00004000},         // DMA registers
    {(uint8_t *)USBCTRL_DPRAM_BASE, 0x00001000},       // USB DPSRAM 4KB
    {(uint8_t *)USBCTRL_REGS_BASE,  0x00004000},         // USB registers
    {(uint8_t *)PIO0_BASE,          0x00004000},         // PIO0 registers
    {(uint8_t *)PIO1_BASE,          0x00004000},         // PIO1 registers
    {(uint8_t *)PIO2_BASE,          0x00004000},         // PIO2 registers
    {(uint8_t *)SIO_BASE,           0x00001000},         // SIO registers, no aliases
    {(uint8_t *)PPB_BASE,           0x00004000}          // PPB registers
};
#endif
