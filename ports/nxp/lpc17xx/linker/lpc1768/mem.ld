/*
 * Memory Spaces Definitions.
 *
 * Need modifying for a specific board.
 *   FLASH.ORIGIN: starting address of flash
 *   FLASH.LENGTH: length of flash
 *   RAM.ORIGIN: starting address of RAM bank 0
 *   RAM.LENGTH: length of RAM bank 0
 *
 * The values below can be addressed in further linker scripts
 * using functions like 'ORIGIN(RAM)' or 'LENGTH(RAM)'.
 */

MEMORY
{
  RAM (xrw)     : ORIGIN = 0x10000000, LENGTH = 32K
  CCMRAM (xrw)  : ORIGIN = 0x2007C000, LENGTH = 32K
  FLASH (rx)    : ORIGIN = 0x00000000, LENGTH = 512K
}
