/*
    GNU linker script for STM32H750
*/

/* Entry Point */
ENTRY(Reset_Handler)

_ld_default_stack_size = 24K;

/* Specify the memory areas -- CircuitPython is loaded on the external QSPI flash */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x90000000, LENGTH = 8M    /* 8M */
    FLASH_ISR (rx)      : ORIGIN = 0x90000000, LENGTH = 4K    /* sector 0 */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x90001000, LENGTH = 8M-4K
    DTCM (xrw)          : ORIGIN = 0x20000000, LENGTH = 128K
    RAM (xrw)           : ORIGIN = 0x24000000, LENGTH = 512K    /* AXI SRAM */
    SRAM_D2 (xrw)       : ORIGIN = 0x30000000, LENGTH = 288K    /* AHB1 SRAM */
    SRAM_D3 (xrw)       : ORIGIN = 0x30040000, LENGTH = 64K     /* AHB2 SRAM */
    ITCM (xrw)          : ORIGIN = 0x00000000, LENGTH = 64K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K; /*TODO: this can probably be bigger, but how big?*/
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(DTCM) + LENGTH(DTCM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
