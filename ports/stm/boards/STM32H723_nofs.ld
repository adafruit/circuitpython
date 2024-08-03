/*
    GNU linker script for STM32H723 with no filesystem, tcm
*/

/* Entry Point */
ENTRY(Reset_Handler)

_ld_default_stack_size = 24K;

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 1024K
    FLASH_ISR (rx)      : ORIGIN = 0x08000000, LENGTH = 128K    /* sector 0, 128K */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08020000, LENGTH = 896K   	/* sectors 1-15, 15*128 */
    DTCM (xrw)          : ORIGIN = 0x20000000, LENGTH = 128K
    RAM (xrw)           : ORIGIN = 0x24000000, LENGTH = 320K    /* AXI SRAM */
    SRAM_D2 (xrw)       : ORIGIN = 0x30000000, LENGTH = 32K     /* AHB1 SRAM */
    SRAM_D3 (xrw)       : ORIGIN = 0x38000000, LENGTH = 16K     /* AHB2 SRAM */
    ITCM (xrw)          : ORIGIN = 0x00000000, LENGTH = 64K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K; /*TODO: this can probably be bigger, but how big?*/
_minimum_heap_size = 16K;

/* brainless copy paste for stack code. Results in ambiguous hard crash */
/* _ld_default_stack_size = 20K; */

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(DTCM) + LENGTH(DTCM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
