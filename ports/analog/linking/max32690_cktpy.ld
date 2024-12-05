/** This file is part of the CircuitPython project: https://circuitpython.org
*
* SPDX-FileCopyrightText: Copyright (c) 2024 Brandon Hurst, Analog Devices Inc.
*
* SPDX-License-Identifier: MIT
*/

MEMORY {
    ROM             (rx) : ORIGIN = 0x00000000,    LENGTH = 128K
    FLASH           (rx) : ORIGIN = 0x10000000,    LENGTH = 3M
    FLASH_FIRMWARE  (rx) : ORIGIN = 0x10000000,    LENGTH = 2992K
    FLASH_FS        (rx) : ORIGIN = 0x102E0000,    LENGTH = 128K
    RAM            (rwx) : ORIGIN = 0x20000000,    LENGTH = 1M
}
/* Minimum flash page is 16K */
/* FLASH FIRMWARE: 3072K [3MB] - 16K - 64K = 2992K */

SECTIONS {
    .rom :
    {
        KEEP(*(.rom_vector*))
        KEEP(*(.rom_handlers*))
    } > ROM


    /** FIXME:   can't place this in its own section for some reason
    *           system doesn't exit ROM code unless *(.isr_vector)
    *           is placed in the beginning of .text,
    *           even if .text is moved upward and *(.isr_vector) is
    *           placed at 0x10000000.
    **/

    /* Place ISR vector in a separate flash section */
    /* .isr_vector : */
    /* { */
        /* ISR Vector beginning of .text */
        /* KEEP(*(.isr_vector)) */
        /* KEEP(*(.isr_vector*)) */
    /* } > FLASH_ISR */

    .text :
    {
        . = ALIGN(4);
        _text = .;

        /* ISR Vector beginning of .text */
        /** fixme:  may want to move this to FLASH_ISR long-term */
        KEEP(*(.isr_vector))
        KEEP(*(.isr_vector*))

        . = ALIGN(4);

        /* program code; exclude RISCV code */
        EXCLUDE_FILE (*riscv.o) *(.text*)
        *(.rodata*)  /* read-only data: "const" */

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

        /* C++ Exception handling */
        KEEP(*(.eh_frame*))
        . = ALIGN(4);
        _etext = .;
    } > FLASH_FIRMWARE

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH_FIRMWARE

    /* Binary import */
    .bin_storage :
    {
       FILL(0xFF)
      _bin_start_ = .;
      KEEP(*(.bin_storage_img))
      _bin_end_ = .;
      . = ALIGN(4);
    } > FLASH_FIRMWARE

    /* it's used for C++ exception handling      */
    /* we need to keep this to avoid overlapping */
    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH_FIRMWARE

    .data :
    {
        . = ALIGN(4);
        _data = .;
        _sdata = .;

        *(vtable)
        *(.data*)           /*read-write initialized data: initialized global variable*/

        /* These array sections are used by __libc_init_array to call static C++ constructors */
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

        /* Run the flash programming functions from SRAM */
        *(.flashprog)

        . = ALIGN(4);
        _edata = .;
    } > RAM AT>FLASH_FIRMWARE
    __load_data = LOADADDR(.data);

    .bss :
    {
        . = ALIGN(4);
        _sbss = .; /* Provide _sbss for Cktpy */
        _bss = .;

        *(.bss*)     /*read-write zero initialized data: uninitialized global variable*/
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;
        _ezero = .; /* Provide _ezero /_ebss for CktPython (same as ebss) */
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);

    _stack = __StackTop;
    _estack = __StackLimit; /* Provide _estack for CktPython */

    .heap (COPY):
    {
        . = ALIGN(4);
        _heap = .;
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        *(.heap*)
        __HeapLimit = ABSOLUTE(__StackLimit);
    } > RAM

    _eheap = __HeapLimit;

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= _ebss, "region RAM overflowed with stack")
}
