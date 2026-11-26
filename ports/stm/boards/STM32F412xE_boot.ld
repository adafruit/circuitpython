/*
    GNU linker script for STM32F412xE behind the uf2-stm32f bootloader (such as
    the ELECFREAKS Retro Arcade). The bootloader occupies sectors 0-1 and starts
    the application at 0x08010000. The filesystem uses sectors 2-3, and the
    firmware runs contiguously from the vector table through sector 7.
*/

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)          : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_FS (rx)       : ORIGIN = 0x08008000, LENGTH = 32K  /* sectors 2,3 are 16K */
    FLASH_ISR (rx)      : ORIGIN = 0x08010000, LENGTH = 1K   /* start of sector 4, where the bootloader jumps */
    FLASH_FIRMWARE (rx) : ORIGIN = 0x08010400, LENGTH = 447K /* rest of sector 4 (64K), sectors 5,6,7 are 128K */
    RAM (xrw)           : ORIGIN = 0x20000000, LENGTH = 256K
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 24K;
_minimum_heap_size = 16K;

/* Define tho top end of the stack.  The stack is full descending so begins just
   above last byte of RAM.  Note that EABI requires the stack to be 8-byte
   aligned for a call. */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);
