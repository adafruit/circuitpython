/* Template for SAMD21/SAMD51 linking. dollar-sign-curly-bracket items are replaced with strings. */

/* Specify the memory areas */
MEMORY
{
    FLASH_BOOTLOADER (rx) : ORIGIN = ${BOOTLOADER_START_ADDR}, LENGTH = ${BOOTLOADER_SIZE}

    FLASH_FIRMWARE (rx)   : ORIGIN = ${CIRCUITPY_FIRMWARE_START_ADDR}, LENGTH = ${CIRCUITPY_FIRMWARE_SIZE}
    FLASH_FILESYSTEM (r)  : ORIGIN = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_FLASH_FILESYSTEM_SIZE}
    FLASH_CONFIG (r)      : ORIGIN = ${CIRCUITPY_INTERNAL_CONFIG_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_CONFIG_SIZE}
    FLASH_NVM (r)         : ORIGIN = ${CIRCUITPY_INTERNAL_NVM_START_ADDR}, LENGTH = ${CIRCUITPY_INTERNAL_NVM_SIZE}

    RAM (rw)              : ORIGIN = ${RAM_ADDR}, LENGTH = ${RAM_SIZE}
    SRAM (rw)             : ORIGIN = ${SRAM_ADDR}, LENGTH = ${SRAM_SIZE}
    AHBRAM (rw)           : ORIGIN = ${AHBRAM_ADDR}, LENGTH = ${AHBRAM_SIZE}
}

/* top end of the stack */
/* stack must be double-word (8 byte) aligned */
_estack = ORIGIN(AHBRAM) + LENGTH(AHBRAM) - 8;
_bootloader_dbl_tap = ORIGIN(SRAM) + LENGTH(SRAM) - 4;

/* define output sections */
ENTRY(ResetISR)

SECTIONS
{
  .text :
  {
    KEEP(*(.vectors))
    KEEP(*(.isr_vector))
    *(.text*)

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    *(.rodata*)

    KEEP(*(.eh_frame*))
  } > FLASH_FIRMWARE

  /*
   * SG veneers:
   * All SG veneers are placed in the special output section .gnu.sgstubs. Its start address
   * must be set, either with the command line option �--section-start� or in a linker script,
   * to indicate where to place these veneers in memory.
   */
/*
  .gnu.sgstubs :
  {
    . = ALIGN(32);
  } > FLASH_FIRMWARE
*/
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH_FIRMWARE

  __exidx_start = .;
  .ARM.exidx :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > FLASH_FIRMWARE
  __exidx_end = .;

  .copy.table :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;
    __data_section_table = .;
    LONG (__etext)
    LONG (__data_start__)
    LONG (__data_end__ - __data_start__)
    /* Add each additional data section here */
/*
    LONG (__etext2)
    LONG (__data2_start__)
    LONG (__data2_end__ - __data2_start__)
*/
    __copy_table_end__ = .;
    __data_section_table_end = .;
  } > FLASH_FIRMWARE

  .zero.table :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;
    __bss_section_table = .;
    /* Add each additional bss section here */
/*
    LONG (__bss2_start__)
    LONG (__bss2_end__ - __bss2_start__)
*/
    LONG (__bss_start__)
    LONG (__bss_end__ - __bss_start__)
    __zero_table_end__ = .;
    __bss_section_table_end = .;
  } > FLASH_FIRMWARE

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in RAM
   * which must be 4byte aligned
   */
  __etext = ALIGN (4);

  .data : AT (__etext)
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data.*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);


    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))
    . = ALIGN(4);
    /* All data end */
    __data_end__ = .;

  } > SRAM

  /*
   * Secondary data section, optional
   *
   * Remember to add each additional data section
   * to the .copy.table above to asure proper
   * initialization during startup.
   */
/*
  __etext2 = ALIGN (4);

  .data2 : AT (__etext2)
  {
    . = ALIGN(4);
    __data2_start__ = .;
    *(.data2)
    *(.data2.*)
    . = ALIGN(4);
    __data2_end__ = .;

  } > SRAM2
*/

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    _ebss = .;
  } > SRAM AT > SRAM

  /*
   * Secondary bss section, optional
   *
   * Remember to add each additional bss section
   * to the .zero.table above to asure proper
   * initialization during startup.
   */
/*
  .bss2 :
  {
    . = ALIGN(4);
    __bss2_start__ = .;
    *(.bss2)
    *(.bss2.*)
    . = ALIGN(4);
    __bss2_end__ = .;
  } > SRAM2 AT > SRAM2
*/

  .heap (ORIGIN(AHBRAM)) (NOLOAD) :
  {
    . = ALIGN(8);
    __HeapBottom = .;
    PROVIDE(end = .);
    . = . + ${CIRCUITPY_DEFAULT_HEAP_SIZE};
    . = ALIGN(8);
    __HeapLimit = .;
  } > AHBRAM

  .stack (ORIGIN(AHBRAM) + LENGTH(AHBRAM) - ${CIRCUITPY_DEFAULT_STACK_SIZE}) (NOLOAD) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + ${CIRCUITPY_DEFAULT_STACK_SIZE};
    . = ALIGN(8);
    __StackTop = .;
    _vStackTop = .;
  } > AHBRAM
  PROVIDE(__stack = __StackTop);

  /* Check if data + heap + stack exceeds RAM limit */
/*
  ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
*/
}
