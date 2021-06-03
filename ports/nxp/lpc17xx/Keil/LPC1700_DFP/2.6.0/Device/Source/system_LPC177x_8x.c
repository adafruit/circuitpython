/**********************************************************************
* $Id$      system_LPC177x_8x.c         2011-06-02
*//**
* @file     system_LPC177x_8x.c
* @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File
*           for the NXP LPC177x_8x Device Series
*
*           ARM Limited (ARM) is supplying this software for use with
*           Cortex-M processor based microcontrollers.  This file can be
*           freely distributed within development tools that are supporting
*           such ARM based processors.
*
* @version  1.0
* @date     02. June. 2011
* @author   NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
* modified by KEIL:
* 2016-12-07:
* - moved clock macros from system_LPC177x_8x.h to system_LPC177x_8x.c
* 2015-04-16:
* - added external memory controller configuration
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#include <stdint.h>
#include "LPC177x_8x.h"
#include "system_LPC177x_8x.h"

#define __CLK_DIV(x,y) ((y == 0) ? 0: x/y)

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/
/*--------------------- Clock Configuration ----------------------------------
//
//  <e> Clock Configuration
//    <h> System Controls and Status Register (SCS - address 0x400F C1A0)
//      <o1.0>  EMC Shift Control Bit
//          <i>   Controls how addresses are output on the EMC address pins for static memories
//          <0=>  Static CS addresses match bus width; AD[1] = 0 for 32 bit, AD[0] = 0 for 16+32 bit (Bit 0 is 0)
//          <1=>  Static CS addresses start at LSB 0 regardless of memory width (Bit 0 is 1)
//
//      <o1.1>  EMC Reset Disable Bit
//          <i>   If 0 (zero), all registers and functions of the EMC are initialized upon any reset condition
//          <i>   If 1, EMC is still retained its state through a warm reset
//          <0=>  Both EMC resets are asserted when any type of chip reset event occurs (Bit 1 is 0)
//          <1=>  Portions of EMC will only be reset by POR or BOR event (Bit 1 is 1)
//
//      <o1.2>  EMC Burst Control
//          <i>   Set to 1 to prevent multiple sequential accesses to memory via EMC static memory chip selects
//          <0=>  Burst enabled (Bit 2 is 0)
//          <1=>  Burst disabled (Bit 2 is 1)
//
//      <o1.3>  MCIPWR Active Level
//          <i>   Selects the active level for the SD card interface signal SD_PWR
//          <0=>  SD_PWR is active low (inverted output of the SD Card interface block) (Bit 3 is 0)
//          <1=>  SD_PWR is active high (follows the output of the SD Card interface block) (Bit 3 is 1)
//
//      <o1.4>  Main Oscillator Range Select
//          <0=>  In Range 1 MHz to 20 MHz (Bit 4 is 0)
//          <1=>  In Range 15 MHz to 25 MHz (Bit 4 is 1)
//
//      <o1.5>  Main Oscillator enable
//          <i>   0 (zero) means disabled, 1 means enable
//
//      <o1.6>  Main Oscillator status (Read-Only)
//    </h>
//
//    <h> Clock Source Select Register (CLKSRCSEL - address 0x400F C10C)
//      <o2.0>  CLKSRC: Select the clock source for sysclk to PLL0 clock
//          <0=>  Internal RC oscillator (Bit 0 is 0)
//          <1=>  Main oscillator (Bit 0 is 1)
//    </h>
//
//    <e3>PLL0 Configuration (Main PLL PLL0CFG - address 0x400F C084)
//      <i>     F_in  is in the range of 1 MHz to 25 MHz
//      <i>     F_cco = (F_in * M * 2 * P) is in range of 156 MHz to 320 MHz
//      <i>     PLL out clock = (F_cco / (2 * P)) is in range of 9.75 MHz to 160 MHz
//
//      <o4.0..4>   MSEL: PLL Multiplier Value
//            <i>       M Value
//            <1-32><#-1>
//
//      <o4.5..6> PSEL: PLL Divider Value
//            <i>       P Value
//            <0=>      1
//            <1=>      2
//            <2=>      4
//            <3=>      8
//    </e>
//
//    <e5>PLL1 Configuration (Alt PLL PLL1CFG - address 0x400F C0A4)
//      <i>     F_in  is in the range of 1 MHz to 25 MHz
//      <i>     F_cco = (F_in * M * 2 * P) is in range of 156 MHz to 320 MHz
//      <i>     PLL out clock = (F_cco / (2 * P)) is in range of 9.75 MHz to 160 MHz
//
//      <o6.0..4>   MSEL: PLL Multiplier Value
//            <i>       M Value
//            <1-32><#-1>
//
//      <o6.5..6> PSEL: PLL Divider Value
//            <i>   P Value
//            <0=>  1
//            <1=>  2
//            <2=>  4
//            <3=>  8
//    </e>
//
//    <h> CPU Clock Selection Register (CCLKSEL - address 0x400F C104)
//      <o7.0..4> CCLKDIV: Select the value for divider of CPU clock (CCLK)
//            <i>   0: The divider is turned off. No clock will be provided to the CPU
//            <i>   n: The input clock is divided by n to produce the CPU clock
//            <0-31>
//
//      <o7.8>    CCLKSEL: Select the input to the divider of CPU clock
//            <0=>  sysclk clock is used
//            <1=>  Main PLL0 clock is used
//    </h>
//
//    <h> USB Clock Selection Register (USBCLKSEL - 0x400F C108)
//      <o8.0..4> USBDIV: USB clock (source PLL0) divider selection
//            <0=>  Divider is off and no clock provides to USB subsystem
//            <1=>  Divider value is 1 (The source clock is divided by 1)
//            <4=>  Divider value is 4 (The source clock is divided by 4)
//            <6=>  Divider value is 6 (The source clock is divided by 6)
//
//      <o8.8..9> USBSEL: Select the source for USB clock divider
//            <i>   When CPU clock is selected, the USB can be accessed
//            <i>   by software but cannot perform USB functions
//            <0=>  sysclk clock (the clock input to PLL0)
//            <1=>  The clock output from PLL0
//            <2=>  The clock output from PLL1
//    </h>
//
//    <h> EMC Clock Selection Register (EMCCLKSEL - address 0x400F C100)
//      <o9.0>  EMCDIV: Set the divider for EMC clock
//          <0=> Divider value is 1
//          <1=> Divider value is 2 (EMC clock is equal a half of input clock)
//    </h>
//
//    <h> Peripheral Clock Selection Register (PCLKSEL - address 0x400F C1A8)
//      <o10.0..4>  PCLKDIV: APB Peripheral clock divider
//            <i> 0: The divider is turned off. No clock will be provided to APB peripherals
//            <i> n: The input clock is divided by n to produce the APB peripheral clock
//            <0-31>
//    </h>
//
//    <h> Power Control for Peripherals Register (PCONP - address 0x400F C1C8)
//      <o11.0>   PCLCD: LCD controller power/clock enable (bit 0)
//      <o11.1>   PCTIM0: Timer/Counter 0 power/clock enable (bit 1)
//      <o11.2>   PCTIM1: Timer/Counter 1 power/clock enable (bit 2)
//      <o11.3>   PCUART0: UART 0 power/clock enable (bit 3)
//      <o11.4>   PCUART1: UART 1 power/clock enable (bit 4)
//      <o11.5>   PCPWM0: PWM0 power/clock enable (bit 5)
//      <o11.6>   PCPWM1: PWM1 power/clock enable (bit 6)
//      <o11.7>   PCI2C0: I2C 0 interface power/clock enable (bit 7)
//      <o11.8>   PCUART4: UART 4 power/clock enable (bit 8)
//      <o11.9>   PCRTC: RTC and Event Recorder power/clock enable (bit 9)
//      <o11.10>  PCSSP1: SSP 1 interface power/clock enable (bit 10)
//      <o11.11>  PCEMC: External Memory Controller power/clock enable (bit 11)
//      <o11.12>  PCADC: A/D converter power/clock enable (bit 12)
//      <o11.13>  PCCAN1: CAN controller 1 power/clock enable (bit 13)
//      <o11.14>  PCCAN2: CAN controller 2 power/clock enable (bit 14)
//      <o11.15>  PCGPIO: IOCON, GPIO, and GPIO interrupts power/clock enable (bit 15)
//      <o11.17>  PCMCPWM: Motor Control PWM power/clock enable (bit 17)
//      <o11.18>  PCQEI: Quadrature encoder interface power/clock enable (bit 18)
//      <o11.19>  PCI2C1: I2C 1 interface power/clock enable (bit 19)
//      <o11.20>  PCSSP2: SSP 2 interface power/clock enable (bit 20)
//      <o11.21>  PCSSP0: SSP 0 interface power/clock enable (bit 21)
//      <o11.22>  PCTIM2: Timer 2 power/clock enable (bit 22)
//      <o11.23>  PCTIM3: Timer 3 power/clock enable (bit 23)
//      <o11.24>  PCUART2: UART 2 power/clock enable (bit 24)
//      <o11.25>  PCUART3: UART 3 power/clock enable (bit 25)
//      <o11.26>  PCI2C2: I2C 2 interface power/clock enable (bit 26)
//      <o11.27>  PCI2S: I2S interface power/clock enable (bit 27)
//      <o11.28>  PCSDC: SD Card interface power/clock enable (bit 28)
//      <o11.29>  PCGPDMA: GPDMA function power/clock enable (bit 29)
//      <o11.30>  PCENET: Ethernet block power/clock enable (bit 30)
//      <o11.31>  PCUSB: USB interface power/clock enable (bit 31)
//    </h>
//
//    <h> Clock Output Configuration Register (CLKOUTCFG)
//      <o12.0..3>  CLKOUTSEL: Clock Source for CLKOUT Selection
//            <0=>  CPU clock
//            <1=>  Main Oscillator
//            <2=>  Internal RC Oscillator
//            <3=>  USB clock
//            <4=>  RTC Oscillator
//            <5=>  unused
//            <6=>  Watchdog Oscillator
//
//      <o12.4..7>  CLKOUTDIV: Output Clock Divider
//            <1-16><#-1>
//
//      <o12.8>   CLKOUT_EN: CLKOUT enable
//    </h>
//
//  </e>
*/

#define CLOCK_SETUP           1
#define SCS_Val               0x00000021
#define CLKSRCSEL_Val         0x00000001
#define PLL0_SETUP            1
//#if defined(__GNUC__)
// Getting strange errors running at 120 MHz when using LPCXpresso IDE
// - ImpreciseErr
// - UndefInstr
// - ...
// Workaround while trying to figure out why is to lower the frequency
//#define PLL0CFG_Val           0x00000009 /* 6 */
//#else
#define PLL0CFG_Val           0x00000009
//#endif

#define PLL1_SETUP            1
#define PLL1CFG_Val           0x00000023
#define CCLKSEL_Val           0x00000101
#define USBCLKSEL_Val         0x00000201
#define EMCCLKSEL_Val         0x00000001
#define PCLKSEL_Val           0x00000002
#define PCONP_Val             0x042887DE
#define CLKOUTCFG_Val         0x00000100


/*--------------------- Flash Accelerator Configuration ----------------------
//
//  <e> Flash Accelerator Configuration register (FLASHCFG - address 0x400F C000)
//    <o1.12..15> FLASHTIM: Flash Access Time
//          <0=>  1 CPU clock (for CPU clock up to 20 MHz)
//          <1=>  2 CPU clocks (for CPU clock up to 40 MHz)
//          <2=>  3 CPU clocks (for CPU clock up to 60 MHz)
//          <3=>  4 CPU clocks (for CPU clock up to 80 MHz)
//          <4=>  5 CPU clocks (for CPU clock up to 100 MHz)
//          <5=>  6 CPU clocks (for any CPU clock)
//  </e>
*/

#define FLASH_SETUP           1
#define FLASHCFG_Val          0x00005000

// Added by Keil 2015-04-16
//     External Memory Controller Setup (EMC) ---------------------------------
// <e> External Memory Controller Setup (EMC)
#define EMC_SETUP             1

//   <h> EMC Bus Configuration
//     <o> Data bus pins
//       <i>Select EMC data bus width
//       <0=>EMC_D[0 .. 7]
//       <1=>EMC_D[0 .. 15]
//       <2=>EMC_D[0 .. 31]
#define EMC_DATA_BUS          2

//     <h> Address bus pins
//       <i>Select used address bus lines
//       <o0.0>  EMC_A0
//       <o0.1>  EMC_A1
//       <o0.2>  EMC_A2
//       <o0.3>  EMC_A3
//       <o0.4>  EMC_A4
//       <o0.5>  EMC_A5
//       <o0.6>  EMC_A6
//       <o0.7>  EMC_A7
//       <o0.8>  EMC_A8
//       <o0.9>  EMC_A9
//       <o0.10> EMC_A10
//       <o0.11> EMC_A11
//       <o0.12> EMC_A12
//       <o0.13> EMC_A13
//       <o0.14> EMC_A14
//       <o0.15> EMC_A15
//       <o0.16> EMC_A16
//       <o0.17> EMC_A17
//       <o0.18> EMC_A18
//       <o0.19> EMC_A19
//       <o0.20> EMC_A20
//       <o0.21> EMC_A21
//       <o0.22> EMC_A22
//       <o0.23> EMC_A23
//       <o0.24> EMC_A24
//       <o0.25> EMC_A25
//     </h>
#define EMC_ADDR_BUS          0x03FFFFFF

//     <h> Control pins
//       <i>Enable or disable EMC_CLK1:0 pins to provide EMCCLK to external devices.
//       <i>Per chip select control pins will be configured automatically.
//       <q0> EMC_CLK0
//       <q1> EMC_CLK1
//     </h>
#define EMC_CTRL_PIN_CLK0     1
#define EMC_CTRL_PIN_CLK1     1
//   </h>

//   <h> EMC Control Register (EMCControl)
//     <i> Controls operation of the memory controller
//     <o0.2> L: Low-power mode enable
//     <o0.1> M: Address mirror enable
//     <o0.0> E: EMC enable
//   </h>
#define EMC_CTRL_Val          0x00000001

//   <h> EMC Configuration Register (EMCConfig)
//     <i> Configures operation of the memory controller
//     <o0.0> Endian mode
//       <0=> Little-endian
//       <1=> Big-endian
//   </h>
#define EMC_CONFIG_Val        0x00000000

//       Dynamic Memory Interface Setup ---------------------------------------
//   <e> Dynamic Memory Interface Setup
#define EMC_DYNAMIC_SETUP     0

//     <h> Dynamic Memory Refresh Timer Register (EMCDynamicRefresh)
//       <i> Configures dynamic memory refresh operation
//       <o0.0..10> REFRESH: Refresh timer <0x000-0x7FF>
//         <i> 0 = refresh disabled, 0x01-0x7FF: value * 16 CCLKS
//     </h>
#define EMC_DYN_RFSH_Val      0x00000000

//     <h> Dynamic Memory Read Configuration Register (EMCDynamicReadConfig)
//       <i> Configures the dynamic memory read strategy
//       <o0.0..1> RD: Read data strategy
//                  <0=> Clock out delayed strategy
//         <1=> Command delayed strategy
//         <2=> Command delayed strategy plus one clock cycle
//         <3=> Command delayed strategy plus two clock cycles
//     </h>
#define EMC_DYN_RD_CFG_Val    0x00000000

//     <h> Dynamic Memory Timings
//       <h> Dynamic Memory Percentage Command Period Register (EMCDynamictRP)
//         <o0.0..3> tRP: Precharge command period <1-16> <#-1>
//           <i> The delay is in EMCCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tRP
//       </h>
//       <h> Dynamic Memory Active to Precharge Command Period Register (EMCDynamictRAS)
//         <o1.0..3> tRAS: Active to precharge command period <1-16> <#-1>
//           <i> The delay is in EMCCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tRAS
//       </h>
//       <h> Dynamic Memory Self-refresh Exit Time Register (EMCDynamictSREX)
//         <o2.0..3> tSREX: Self-refresh exit time <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tSREX,
//           <i> for devices without this parameter you use the same value as tXSR
//       </h>
//       <h> Dynamic Memory Last Data Out to Active Time Register (EMCDynamictAPR)
//         <o3.0..3> tAPR: Last-data-out to active command time <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tAPR
//       </h>
//       <h> Dynamic Memory Data-in to Active Command Time Register (EMCDynamictDAL)
//         <o4.0..3> tDAL: Data-in to active command time <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tDAL or tAPW
//       </h>
//       <h> Dynamic Memory Write Recovery Time Register (EMCDynamictWR)
//         <o5.0..3> tWR: Write recovery time <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tWR, tDPL, tRWL, or tRDL
//       </h>
//       <h> Dynamic Memory Active to Active Command Period Register (EMCDynamictRC)
//         <o6.0..4> tRC: Active to active command period <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tRC
//       </h>
//       <h> Dynamic Memory Auto-refresh Period Register (EMCDynamictRFC)
//         <o7.0..4> tRFC: Auto-refresh period and auto-refresh to active command period <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tRFC or  tRC
//       </h>
//       <h> Dynamic Memory Exit Self-refresh Register (EMCDynamictXSR)
//         <o8.0..4> tXSR: Exit self-refresh to active command time <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tXSR
//       </h>
//       <h> Dynamic Memory Active Bank A to Active Bank B Time Register (EMCDynamicRRD)
//         <o9.0..3> tRRD: Active bank A to active bank B latency <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tRRD
//       </h>
//       <h> Dynamic Memory Load Mode Register to Active Command Time (EMCDynamictMRD)
//         <o10.0..3> tMRD: Load mode register to active command time <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//           <i> This value is normally found in SDRAM data sheets as tMRD or tRSA
//       </h>
//     </h>
#define EMC_DYN_RP_Val        0x0000000F
#define EMC_DYN_RAS_Val       0x0000000F
#define EMC_DYN_SREX_Val      0x0000000F
#define EMC_DYN_APR_Val       0x0000000F
#define EMC_DYN_DAL_Val       0x0000000F
#define EMC_DYN_WR_Val        0x0000000F
#define EMC_DYN_RC_Val        0x0000001F
#define EMC_DYN_RFC_Val       0x0000001F
#define EMC_DYN_XSR_Val       0x0000001F
#define EMC_DYN_RRD_Val       0x0000000F
#define EMC_DYN_MRD_Val       0x0000000F

//     <e> Configure External Bus Behaviour for Dynamic CS0 Area
#define EMC_DYNCS0_SETUP      0

//       <h> Dynamic Memory Configuration Register (EMCDynamicConfig0)
//         <i> Defines the configuration information for the dynamic memory CS0
//         <o0.20> P: Write protect
//         <o0.19> B: Buffer enable
//         <o0.14> AM 14: External bus data width
//           <0=> 16 bit
//           <1=> 32 bit
//         <o0.12> AM 12: External bus memory type
//           <0=> High-performance
//           <1=> Low-power SDRAM
//         <o0.7..11> AM 11..7: External bus address mapping (Row, Bank, Column)
//           <0x00=> 16 Mb = 2MB (2Mx8), 2 banks, row length = 11, column length = 9
//           <0x01=> 16 Mb = 2MB (1Mx16), 2 banks, row length = 11, column length = 8
//           <0x04=> 64 Mb = 8MB (8Mx8), 4 banks, row length = 12, column length = 9
//           <0x05=> 64 Mb = 8MB (4Mx16), 4 banks, row length = 12, column length = 8
//           <0x08=> 128 Mb = 16MB (16Mx8), 4 banks, row length = 12, column length = 10
//           <0x09=> 128 Mb = 16MB (8Mx16), 4 banks, row length = 12, column length = 9
//           <0x0C=> 256 Mb = 32MB (32Mx8), 4 banks, row length = 13, column length = 10
//           <0x0D=> 256 Mb = 32MB (16Mx16), 4 banks, row length = 13, column length = 9
//           <0x10=> 512 Mb = 64MB (64Mx8), 4 banks, row length = 13, column length = 11
//           <0x11=> 512 Mb = 64MB (32Mx16), 4 banks, row length = 13, column length = 10
//         <o0.3..4> MD: Memory device
//           <0=> SDRAM
//           <1=> Low-power SDRAM
//       </h>
#define EMC_DYN_CFG0_Val      0x00000000

//       <h> Dynamic Memory RAS & CAS Delay register (EMCDynamicRASCAS0)
//         <i> Controls the RAS and CAS latencies for the dynamic memory CS0
//         <o0.8..9> CAS: CAS latency
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//         <o0.0..1> RAS: RAS latency (active to read/write delay)
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//       </h>
#define EMC_DYN_RASCAS0_Val   0x00000303

//     </e> End of Dynamic Setup for CS0 Area

#if   (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x00)
  #define DYNCS0_BANK_BITS    1
  #define DYNCS0_COL_BITS     9
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x01)
  #define DYNCS0_BANK_BITS    1
  #define DYNCS0_COL_BITS     8
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x04)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     9
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x05)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     8
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x08)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     10
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x09)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     9
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x0C)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     10
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x0D)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     9
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x10)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     11
#elif (((EMC_DYN_CFG0_Val >> 7) & 0x1F) == 0x11)
  #define DYNCS0_BANK_BITS    2
  #define DYNCS0_COL_BITS     10
#else
  #error "Invalid external bus address mapping for DYNCS0 in system_LPC177x_8x.c."
#endif

#if ((EMC_DYN_CFG0_Val >> 14) & 1)
  #define DYNCS0_BUS_WIDTH    32
  #define DYNCS0_SDRAM_MODE   (0x02 | ((EMC_DYN_RASCAS0_Val >> 4) & 0x30))
#else
  #define DYNCS0_BUS_WIDTH    16
  #define DYNCS0_SDRAM_MODE   (0x03 | ((EMC_DYN_RASCAS0_Val >> 4) & 0x30))
#endif


//     <e> Configure External Bus Behaviour for Dynamic CS1 Area
#define EMC_DYNCS1_SETUP      0

//       <h> Dynamic Memory Configuration Register (EMCDynamicConfig1)
//         <i> Defines the configuration information for the dynamic memory CS1
//         <o0.20> P: Write protect
//         <o0.19> B: Buffer enable
//         <o0.14> AM 14: External bus data width
//           <0=> 16 bit
//           <1=> 32 bit
//         <o0.12> AM 12: External bus memory type
//           <0=> High-performance
//           <1=> Low-power SDRAM
//         <o0.7..11> AM 11..7: External bus address mapping (Row, Bank, Column)
//           <0x00=> 16 Mb = 2MB (2Mx8), 2 banks, row length = 11, column length = 9
//           <0x01=> 16 Mb = 2MB (1Mx16), 2 banks, row length = 11, column length = 8
//           <0x04=> 64 Mb = 8MB (8Mx8), 4 banks, row length = 12, column length = 9
//           <0x05=> 64 Mb = 8MB (4Mx16), 4 banks, row length = 12, column length = 8
//           <0x08=> 128 Mb = 16MB (16Mx8), 4 banks, row length = 12, column length = 10
//           <0x09=> 128 Mb = 16MB (8Mx16), 4 banks, row length = 12, column length = 9
//           <0x0C=> 256 Mb = 32MB (32Mx8), 4 banks, row length = 13, column length = 10
//           <0x0D=> 256 Mb = 32MB (16Mx16), 4 banks, row length = 13, column length = 9
//           <0x10=> 512 Mb = 64MB (64Mx8), 4 banks, row length = 13, column length = 11
//           <0x11=> 512 Mb = 64MB (32Mx16), 4 banks, row length = 13, column length = 10
//         <o0.3..4> MD: Memory device
//           <0=> SDRAM
//           <1=> Low-power SDRAM
//           <2=> Micron SyncFlash
//       </h>
#define EMC_DYN_CFG1_Val      0x00000000

//       <h> Dynamic Memory RAS & CAS Delay register (EMCDynamicRASCAS1)
//         <i> Controls the RAS and CAS latencies for the dynamic memory CS1
//         <o0.8..9> CAS: CAS latency
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//         <o0.0..1> RAS: RAS latency (active to read/write delay)
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//       </h>
#define EMC_DYN_RASCAS1_Val   0x00000303

//     </e> End of Dynamic Setup for CS1 Area

#if   (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x00)
  #define DYNCS1_BANK_BITS    1
  #define DYNCS1_COL_BITS     9
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x01)
  #define DYNCS1_BANK_BITS    1
  #define DYNCS1_COL_BITS     8
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x04)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     9
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x05)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     8
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x08)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     10
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x09)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     9
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x0C)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     10
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x0D)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     9
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x10)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     11
#elif (((EMC_DYN_CFG1_Val >> 7) & 0x1F) == 0x11)
  #define DYNCS1_BANK_BITS    2
  #define DYNCS1_COL_BITS     10
#else
  #error "Invalid external bus address mapping for DYNCS1 in system_LPC177x_8x.c."
#endif

#if ((EMC_DYN_CFG1_Val >> 14) & 1)
  #define DYNCS1_BUS_WIDTH    32
  #define DYNCS1_SDRAM_MODE   (0x02 | ((EMC_DYN_RASCAS1_Val >> 4) & 0x30))
#else
  #define DYNCS1_BUS_WIDTH    16
  #define DYNCS1_SDRAM_MODE   (0x03 | ((EMC_DYN_RASCAS1_Val >> 4) & 0x30))
#endif

//     <e> Configure External Bus Behaviour for Dynamic CS2 Area
#define EMC_DYNCS2_SETUP      0

//       <h> Dynamic Memory Configuration Register (EMCDynamicConfig2)
//         <i> Defines the configuration information for the dynamic memory CS2
//         <o0.20> P: Write protect
//         <o0.19> B: Buffer enable
//         <o0.14> AM 14: External bus data width
//           <0=> 16 bit
//           <1=> 32 bit
//         <o0.12> AM 12: External bus memory type
//           <0=> High-performance
//           <1=> Low-power SDRAM
//         <o0.7..11> AM 11..7: External bus address mapping (Row, Bank, Column)
//           <0x00=> 16 Mb = 2MB (2Mx8), 2 banks, row length = 11, column length = 9
//           <0x01=> 16 Mb = 2MB (1Mx16), 2 banks, row length = 11, column length = 8
//           <0x04=> 64 Mb = 8MB (8Mx8), 4 banks, row length = 12, column length = 9
//           <0x05=> 64 Mb = 8MB (4Mx16), 4 banks, row length = 12, column length = 8
//           <0x08=> 128 Mb = 16MB (16Mx8), 4 banks, row length = 12, column length = 10
//           <0x09=> 128 Mb = 16MB (8Mx16), 4 banks, row length = 12, column length = 9
//           <0x0C=> 256 Mb = 32MB (32Mx8), 4 banks, row length = 13, column length = 10
//           <0x0D=> 256 Mb = 32MB (16Mx16), 4 banks, row length = 13, column length = 9
//           <0x10=> 512 Mb = 64MB (64Mx8), 4 banks, row length = 13, column length = 11
//           <0x11=> 512 Mb = 64MB (32Mx16), 4 banks, row length = 13, column length = 10
//         <o0.3..4> MD: Memory device
//           <0=> SDRAM
//           <1=> Low-power SDRAM
//           <2=> Micron SyncFlash
//       </h>
#define EMC_DYN_CFG2_Val      0x00000000

//       <h> Dynamic Memory RAS & CAS Delay register (EMCDynamicRASCAS2)
//         <i> Controls the RAS and CAS latencies for the dynamic memory CS2
//         <o0.8..9> CAS: CAS latency
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//         <o0.0..1> RAS: RAS latency (active to read/write delay)
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//       </h>
#define EMC_DYN_RASCAS2_Val   0x00000303

//     </e> End of Dynamic Setup for CS2 Area

#if   (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x00)
  #define DYNCS2_BANK_BITS    1
  #define DYNCS2_COL_BITS     9
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x01)
  #define DYNCS2_BANK_BITS    1
  #define DYNCS2_COL_BITS     8
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x04)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     9
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x05)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     8
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x08)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     10
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x09)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     9
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x0C)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     10
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x0D)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     9
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x10)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     11
#elif (((EMC_DYN_CFG2_Val >> 7) & 0x1F) == 0x11)
  #define DYNCS2_BANK_BITS    2
  #define DYNCS2_COL_BITS     10
#else
  #error "Invalid external bus address mapping for DYNCS2 in system_LPC177x_8x.c."
#endif

#if ((EMC_DYN_CFG2_Val >> 14) & 1)
  #define DYNCS2_BUS_WIDTH    32
  #define DYNCS2_SDRAM_MODE   (0x02 | ((EMC_DYN_RASCAS2_Val >> 4) & 0x30))
#else
  #define DYNCS2_BUS_WIDTH    16
  #define DYNCS2_SDRAM_MODE   (0x03 | ((EMC_DYN_RASCAS2_Val >> 4) & 0x30))
#endif

//     <e> Configure External Bus Behaviour for Dynamic CS3 Area
#define EMC_DYNCS3_SETUP      0

//       <h> Dynamic Memory Configuration Register (EMCDynamicConfig3)
//         <i> Defines the configuration information for the dynamic memory CS3
//         <o0.20> P: Write protect
//         <o0.19> B: Buffer enable
//         <o0.14> AM 14: External bus data width
//           <0=> 16 bit
//           <1=> 32 bit
//         <o0.12> AM 12: External bus memory type
//           <0=> High-performance
//           <1=> Low-power SDRAM
//         <o0.7..11> AM 11..7: External bus address mapping (Row, Bank, Column)
//           <0x00=> 16 Mb = 2MB (2Mx8), 2 banks, row length = 11, column length = 9
//           <0x01=> 16 Mb = 2MB (1Mx16), 2 banks, row length = 11, column length = 8
//           <0x04=> 64 Mb = 8MB (8Mx8), 4 banks, row length = 12, column length = 9
//           <0x05=> 64 Mb = 8MB (4Mx16), 4 banks, row length = 12, column length = 8
//           <0x08=> 128 Mb = 16MB (16Mx8), 4 banks, row length = 12, column length = 10
//           <0x09=> 128 Mb = 16MB (8Mx16), 4 banks, row length = 12, column length = 9
//           <0x0C=> 256 Mb = 32MB (32Mx8), 4 banks, row length = 13, column length = 10
//           <0x0D=> 256 Mb = 32MB (16Mx16), 4 banks, row length = 13, column length = 9
//           <0x10=> 512 Mb = 64MB (64Mx8), 4 banks, row length = 13, column length = 11
//           <0x11=> 512 Mb = 64MB (32Mx16), 4 banks, row length = 13, column length = 10
//         <o0.3..4> MD: Memory device
//           <0=> SDRAM
//           <1=> Low-power SDRAM
//           <2=> Micron SyncFlash
//       </h>
#define EMC_DYN_CFG3_Val      0x00000000

//       <h> Dynamic Memory RAS & CAS Delay register (EMCDynamicRASCAS3)
//         <i> Controls the RAS and CAS latencies for the dynamic memory CS3
//         <o0.8..9> CAS: CAS latency
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//         <o0.0..1> RAS: RAS latency (active to read/write delay)
//           <1=> One CCLK cycle
//           <2=> Two CCLK cycles
//           <3=> Three CCLK cycles
//       </h>
#define EMC_DYN_RASCAS3_Val   0x00000303

//     </e> End of Dynamic Setup for CS3 Area

#if   (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x00)
  #define DYNCS3_BANK_BITS    1
  #define DYNCS3_COL_BITS     9
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x01)
  #define DYNCS3_BANK_BITS    1
  #define DYNCS3_COL_BITS     8
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x04)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     9
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x05)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     8
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x08)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     10
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x09)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     9
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x0C)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     10
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x0D)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     9
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x10)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     11
#elif (((EMC_DYN_CFG3_Val >> 7) & 0x1F) == 0x11)
  #define DYNCS3_BANK_BITS    2
  #define DYNCS3_COL_BITS     10
#else
  #error "Invalid external bus address mapping for DYNCS3 in system_LPC177x_8x.c."
#endif

#if ((EMC_DYN_CFG3_Val >> 14) & 1)
  #define DYNCS3_BUS_WIDTH    32
  #define DYNCS3_SDRAM_MODE   (0x02 | ((EMC_DYN_RASCAS3_Val >> 4) & 0x30))
#else
  #define DYNCS3_BUS_WIDTH    16
  #define DYNCS3_SDRAM_MODE   (0x03 | ((EMC_DYN_RASCAS3_Val >> 4) & 0x30))
#endif

//   </e> End of Dynamic Setup

//       Static Memory Interface Setup ----------------------------------------
//   <e> Static Memory Interface Setup
#define EMC_STATIC_SETUP      0

//         Configure External Bus Behaviour for Static CS0 Area ---------------
//     <e> Configure External Bus Behaviour for Static CS0 Area
#define EMC_STACS0_SETUP      0

//       <h> Static Memory Configuration Register (EMCStaticConfig0)
//         <i> Defines the configuration information for the static memory CS0
//         <o0.20> WP: Write protect
//         <o0.19> B: Buffer enable
//         <o0.8> EW: Extended wait enable
//         <o0.7> PB: Byte lane state
//           <0=> For reads BLSn are HIGH, for writes BLSn are LOW
//           <1=> For reads BLSn are LOW, for writes BLSn are LOW
//         <o0.6> PC: Chip select polarity
//           <0=> Active LOW chip select
//           <1=> Active HIGH chip select
//         <o0.3> PM: Page mode enable
//         <o0.0..1> MW: Memory width
//           <0=> 8 bit
//           <1=> 16 bit
//           <2=> 32 bit
//       </h>
#define EMC_STA_CFG0_Val      0x00000000

//       <h> Static Memory Write Enable Delay Register (EMCStaticWaitWen0)
//         <i> Selects the delay from CS0 to write enable
//         <o.0..3> WAITWEN: Wait write enable <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWEN0_Val     0x00000000

//       <h> Static Memory Output Enable Delay register (EMCStaticWaitOen0)
//         <i> Selects the delay from CS0 or address change, whichever is later, to output enable
//         <o.0..3> WAITOEN: Wait output enable <0-15>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WOEN0_Val     0x00000000

//       <h> Static Memory Read Delay Register (EMCStaticWaitRd0)
//         <i> Selects the delay from CS0 to a read access
//         <o.0..4> WAITRD: Non-page mode read wait states or asynchronous page mode read first access wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WRD0_Val      0x0000001F

//       <h> Static Memory Page Mode Read Delay Register (EMCStaticWaitPage0)
//         <i> Selects the delay for asynchronous page mode sequential accesses for CS0
//         <o.0..4> WAITPAGE: Asynchronous page mode read after the first read wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WPAGE0_Val    0x0000001F

//       <h> Static Memory Write Delay Register (EMCStaticWaitWr0)
//         <i> Selects the delay from CS0 to a write access
//         <o.0..4> WAITWR: Write wait states <2-33> <#-2>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWR0_Val      0x0000001F

//       <h> Static Memory Turn Round Delay Register (EMCStaticWaitTurn0)
//         <i> Selects the number of bus turnaround cycles for CS0
//         <o.0..4> WAITTURN: Bus turnaround cycles <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WTURN0_Val    0x0000000F

//     </e> End of Static Setup for Static CS0 Area

#if   ((EMC_STA_CFG0_Val & 3) == 0)
  #define STACS0_BUS_WIDTH    8
#elif ((EMC_STA_CFG0_Val & 3) == 1)
  #define STACS0_BUS_WIDTH    16
#else
  #define STACS0_BUS_WIDTH    32
#endif

//         Configure External Bus Behaviour for Static CS1 Area ---------------
//     <e> Configure External Bus Behaviour for Static CS1 Area
#define EMC_STACS1_SETUP      0

//       <h> Static Memory Configuration Register (EMCStaticConfig1)
//         <i> Defines the configuration information for the static memory CS1
//         <o0.20> WP: Write protect
//         <o0.19> B: Buffer enable
//         <o0.8> EW: Extended wait enable
//         <o0.7> PB: Byte lane state
//           <0=> For reads BLSn are HIGH, for writes BLSn are LOW
//           <1=> For reads BLSn are LOW, for writes BLSn are LOW
//         <o0.6> PC: Chip select polarity
//           <0=> Active LOW chip select
//           <1=> Active HIGH chip select
//         <o0.3> PM: Page mode enable
//         <o0.0..1> MW: Memory width
//           <0=> 8 bit
//           <1=> 16 bit
//           <2=> 32 bit
//       </h>
#define EMC_STA_CFG1_Val      0x00000000

//       <h> Static Memory Write Enable Delay Register (EMCStaticWaitWen1)
//         <i> Selects the delay from CS1 to write enable
//         <o.0..3> WAITWEN: Wait write enable <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWEN1_Val     0x00000000

//       <h> Static Memory Output Enable Delay register (EMCStaticWaitOen1)
//         <i> Selects the delay from CS1 or address change, whichever is later, to output enable
//         <o.0..3> WAITOEN: Wait output enable <0-15>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WOEN1_Val     0x00000000

//       <h> Static Memory Read Delay Register (EMCStaticWaitRd1)
//         <i> Selects the delay from CS1 to a read access
//         <o.0..4> WAITRD: Non-page mode read wait states or asynchronous page mode read first access wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WRD1_Val      0x0000001F

//       <h> Static Memory Page Mode Read Delay Register (EMCStaticWaitPage1)
//         <i> Selects the delay for asynchronous page mode sequential accesses for CS1
//         <o.0..4> WAITPAGE: Asynchronous page mode read after the first read wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WPAGE1_Val    0x0000001F

//       <h> Static Memory Write Delay Register (EMCStaticWaitWr1)
//         <i> Selects the delay from CS1 to a write access
//         <o.0..4> WAITWR: Write wait states <2-33> <#-2>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWR1_Val      0x0000001F

//       <h> Static Memory Turn Round Delay Register (EMCStaticWaitTurn1)
//         <i> Selects the number of bus turnaround cycles for CS1
//         <o.0..4> WAITTURN: Bus turnaround cycles <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WTURN1_Val    0x0000000F

//     </e> End of Static Setup for Static CS1 Area

#if   ((EMC_STA_CFG1_Val & 3) == 0)
  #define STACS1_BUS_WIDTH    8
#elif ((EMC_STA_CFG1_Val & 3) == 1)
  #define STACS1_BUS_WIDTH    16
#else
  #define STACS1_BUS_WIDTH    32
#endif

//         Configure External Bus Behaviour for Static CS2 Area ---------------
//     <e> Configure External Bus Behaviour for Static CS2 Area
#define EMC_STACS2_SETUP      0

//       <h> Static Memory Configuration Register (EMCStaticConfig2)
//         <i> Defines the configuration information for the static memory CS2
//         <o0.20> WP: Write protect
//         <o0.19> B: Buffer enable
//         <o0.8> EW: Extended wait enable
//         <o0.7> PB: Byte lane state
//           <0=> For reads BLSn are HIGH, for writes BLSn are LOW
//           <1=> For reads BLSn are LOW, for writes BLSn are LOW
//         <o0.6> PC: Chip select polarity
//           <0=> Active LOW chip select
//           <1=> Active HIGH chip select
//         <o0.3> PM: Page mode enable
//         <o0.0..1> MW: Memory width
//           <0=> 8 bit
//           <1=> 16 bit
//           <2=> 32 bit
//       </h>
#define EMC_STA_CFG2_Val      0x00000000

//       <h> Static Memory Write Enable Delay Register (EMCStaticWaitWen2)
//         <i> Selects the delay from CS2 to write enable
//         <o.0..3> WAITWEN: Wait write enable <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWEN2_Val     0x00000000

//       <h> Static Memory Output Enable Delay register (EMCStaticWaitOen2)
//         <i> Selects the delay from CS2 or address change, whichever is later, to output enable
//         <o.0..3> WAITOEN: Wait output enable <0-15>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WOEN2_Val     0x00000000

//       <h> Static Memory Read Delay Register (EMCStaticWaitRd2)
//         <i> Selects the delay from CS2 to a read access
//         <o.0..4> WAITRD: Non-page mode read wait states or asynchronous page mode read first access wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WRD2_Val      0x0000001F

//       <h> Static Memory Page Mode Read Delay Register (EMCStaticWaitPage2)
//         <i> Selects the delay for asynchronous page mode sequential accesses for CS2
//         <o.0..4> WAITPAGE: Asynchronous page mode read after the first read wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WPAGE2_Val    0x0000001F

//       <h> Static Memory Write Delay Register (EMCStaticWaitWr2)
//         <i> Selects the delay from CS2 to a write access
//         <o.0..4> WAITWR: Write wait states <2-33> <#-2>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWR2_Val      0x0000001F

//       <h> Static Memory Turn Round Delay Register (EMCStaticWaitTurn2)
//         <i> Selects the number of bus turnaround cycles for CS2
//         <o.0..4> WAITTURN: Bus turnaround cycles <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WTURN2_Val    0x0000000F

//     </e> End of Static Setup for Static CS2 Area

#if   ((EMC_STA_CFG2_Val & 3) == 0)
  #define STACS2_BUS_WIDTH    8
#elif ((EMC_STA_CFG2_Val & 3) == 1)
  #define STACS2_BUS_WIDTH    16
#else
  #define STACS2_BUS_WIDTH    32
#endif

//         Configure External Bus Behaviour for Static CS3 Area ---------------
//     <e> Configure External Bus Behaviour for Static CS3 Area
#define EMC_STACS3_SETUP      0

//       <h> Static Memory Configuration Register (EMCStaticConfig3)
//         <i> Defines the configuration information for the static memory CS3
//         <o0.20> WP: Write protect
//         <o0.19> B: Buffer enable
//         <o0.8> EW: Extended wait enable
//         <o0.7> PB: Byte lane state
//           <0=> For reads BLSn are HIGH, for writes BLSn are LOW
//           <1=> For reads BLSn are LOW, for writes BLSn are LOW
//         <o0.6> PC: Chip select polarity
//           <0=> Active LOW chip select
//           <1=> Active HIGH chip select
//         <o0.3> PM: Page mode enable
//         <o0.0..1> MW: Memory width
//           <0=> 8 bit
//           <1=> 16 bit
//           <2=> 32 bit
//       </h>
#define EMC_STA_CFG3_Val      0x00000000

//       <h> Static Memory Write Enable Delay Register (EMCStaticWaitWen3)
//         <i> Selects the delay from CS3 to write enable
//         <o.0..3> WAITWEN: Wait write enable <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWEN3_Val     0x00000000

//       <h> Static Memory Output Enable Delay register (EMCStaticWaitOen3)
//         <i> Selects the delay from CS3 or address change, whichever is later, to output enable
//         <o.0..3> WAITOEN: Wait output enable <0-15>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WOEN3_Val     0x00000000

//       <h> Static Memory Read Delay Register (EMCStaticWaitRd3)
//         <i> Selects the delay from CS3 to a read access
//         <o.0..4> WAITRD: Non-page mode read wait states or asynchronous page mode read first access wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WRD3_Val      0x0000001F

//       <h> Static Memory Page Mode Read Delay Register (EMCStaticWaitPage3)
//         <i> Selects the delay for asynchronous page mode sequential accesses for CS3
//         <o.0..4> WAITPAGE: Asynchronous page mode read after the first read wait states <1-32> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WPAGE3_Val    0x0000001F

//       <h> Static Memory Write Delay Register (EMCStaticWaitWr3)
//         <i> Selects the delay from CS3 to a write access
//         <o.0..4> WAITWR: Write wait states <2-33> <#-2>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WWR3_Val      0x0000001F

//       <h> Static Memory Turn Round Delay Register (EMCStaticWaitTurn3)
//         <i> Selects the number of bus turnaround cycles for CS3
//         <o.0..4> WAITTURN: Bus turnaround cycles <1-16> <#-1>
//           <i> The delay is in CCLK cycles
//       </h>
#define EMC_STA_WTURN3_Val    0x0000000F

//     </e> End of Static Setup for Static CS3 Area

#if   ((EMC_STA_CFG3_Val & 3) == 0)
  #define STACS3_BUS_WIDTH    8
#elif ((EMC_STA_CFG3_Val & 3) == 1)
  #define STACS3_BUS_WIDTH    16
#else
  #define STACS3_BUS_WIDTH    32
#endif

//     <h> Static Memory Extended Wait Register (EMCStaticExtendedWait)
//       <i> Time long static memory read and write transfers
//       <o.0..9> EXTENDEDWAIT: Extended wait time out <0-1023>
//         <i> The delay is in (16 * CCLK) cycles
//     </h>
#define EMC_STA_EXT_W_Val     0x00000000

//   </e> End of Static Setup

// </e> End of EMC Setup
// End: Added by Keil 2015-04-16

/*----------------------------------------------------------------------------
  Check the register settings
 *----------------------------------------------------------------------------*/
#define CHECK_RANGE(val, min, max)                ((val < min) || (val > max))
#define CHECK_RSVD(val, mask)                     (val & mask)

/* Clock Configuration -------------------------------------------------------*/
#if (CHECK_RSVD((SCS_Val),       ~0x0000003F))
   #error "SCS: Invalid values of reserved bits!"
#endif

#if (CHECK_RANGE((CLKSRCSEL_Val), 0, 1))
   #error "CLKSRCSEL: Value out of range!"
#endif

#if (CHECK_RSVD((PLL0CFG_Val),   ~0x0000007F))
   #error "PLL0CFG: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PLL1CFG_Val),   ~0x0000007F))
   #error "PLL1CFG: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CCLKSEL_Val),   ~0x0000011F))
   #error "CCLKSEL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((USBCLKSEL_Val), ~0x0000031F))
   #error "USBCLKSEL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((EMCCLKSEL_Val), ~0x00000001))
   #error "EMCCLKSEL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PCLKSEL_Val), ~0x0000001F))
   #error "PCLKSEL: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((PCONP_Val), ~0xFFFEFFFF))
   #error "PCONP: Invalid values of reserved bits!"
#endif

#if (CHECK_RSVD((CLKOUTCFG_Val), ~0x000001FF))
   #error "CLKOUTCFG: Invalid values of reserved bits!"
#endif

/* Flash Accelerator Configuration -------------------------------------------*/
#if (CHECK_RSVD((FLASHCFG_Val), ~0x0000F000))
   #warning "FLASHCFG: Invalid values of reserved bits!"
#endif


/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32768UL)        /* RTC oscillator frequency           */
#define IRC_OSC     (12000000UL)        /* Internal RC oscillator frequency   */
#define WDT_OSC		  (  500000UL)		/* Internal WDT oscillator frequency  */


/* pll_out_clk = F_cco / (2 * P)
   F_cco = pll_in_clk * M * 2 * P */
#define __M                   ((PLL0CFG_Val & 0x1F) + 1)
#define __PLL0_CLK(__F_IN)    (__F_IN * __M)
#define __CCLK_DIV            (CCLKSEL_Val & 0x1F)
#define __PCLK_DIV            (PCLKSEL_Val & 0x1F)
#define __ECLK_DIV            ((EMCCLKSEL_Val & 0x01) + 1)

/* Determine core clock frequency according to settings */
#if (CLOCK_SETUP)                       /* Clock Setup                        */

  #if ((CLKSRCSEL_Val & 0x01) == 1) && ((SCS_Val & 0x20)== 0)
   #error "Main Oscillator is selected as clock source but is not enabled!"
  #endif

  #if ((CCLKSEL_Val & 0x100) == 0x100) && (PLL0_SETUP == 0)
   #error "Main PLL is selected as clock source but is not enabled!"
  #endif

  #if ((CCLKSEL_Val & 0x100) == 0)      /* cclk = sysclk */
    #if ((CLKSRCSEL_Val & 0x01) == 0)   /* sysclk = irc_clk */
        #define __CORE_CLK (IRC_OSC / __CCLK_DIV)
        #define __PER_CLK  (IRC_OSC/  __PCLK_DIV)
        #define __EMC_CLK  (IRC_OSC/  __ECLK_DIV)
    #else                               /* sysclk = osc_clk */
        #define __CORE_CLK (OSC_CLK / __CCLK_DIV)
        #define __PER_CLK  (OSC_CLK/  __PCLK_DIV)
        #define __EMC_CLK  (__CORE_CLK/  __ECLK_DIV)
    #endif
  #else                                 /* cclk = pll_clk */
    #if ((CLKSRCSEL_Val & 0x01) == 0)   /* sysclk = irc_clk */
        #define __CORE_CLK (__PLL0_CLK(IRC_OSC) / __CCLK_DIV)
        #define __PER_CLK  (__PLL0_CLK(IRC_OSC) / __PCLK_DIV)
        #define __EMC_CLK  (__PLL0_CLK(IRC_OSC) / __ECLK_DIV)
    #else                               /* sysclk = osc_clk */
        #define __CORE_CLK (__PLL0_CLK(OSC_CLK) / __CCLK_DIV)
        #define __PER_CLK  (__PLL0_CLK(OSC_CLK) / __PCLK_DIV)
        #define __EMC_CLK  (__PLL0_CLK(OSC_CLK) / __ECLK_DIV)
    #endif
  #endif

#else
        #define __CORE_CLK (IRC_OSC)
        #define __PER_CLK  (IRC_OSC)
        #define __EMC_CLK  (IRC_OSC)
#endif

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = __CORE_CLK;  /* System Clock Frequency (Core Clock)*/
uint32_t PeripheralClock = __PER_CLK;   /* Peripheral Clock Frequency (Pclk)  */
uint32_t EMCClock        = __EMC_CLK;   /* EMC Clock Frequency                */
uint32_t USBClock        = (48000000UL);/* USB Clock Frequency - this value will
                   be updated after call SystemCoreClockUpdate, should be 48MHz*/


/*----------------------------------------------------------------------------
  SystemCoreClockUpdate
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)               /* Get Core Clock Frequency   */
{
  /* Determine clock frequency according to clock register values             */
  if ((LPC_SC->CCLKSEL &0x100) == 0) {            /* cclk = sysclk    */
    if ((LPC_SC->CLKSRCSEL & 0x01) == 0) {    /* sysclk = irc_clk */
          SystemCoreClock = __CLK_DIV(IRC_OSC , (LPC_SC->CCLKSEL & 0x1F));
          PeripheralClock = __CLK_DIV(IRC_OSC , (LPC_SC->PCLKSEL & 0x1F));
          EMCClock        = (IRC_OSC / ((LPC_SC->EMCCLKSEL & 0x01)+1));
    }
    else {                                        /* sysclk = osc_clk */
      if ((LPC_SC->SCS & 0x40) == 0) {
          SystemCoreClock = 0;                      /* this should never happen! */
          PeripheralClock = 0;
          EMCClock        = 0;
      }
      else {
          SystemCoreClock = __CLK_DIV(OSC_CLK , (LPC_SC->CCLKSEL & 0x1F));
          PeripheralClock = __CLK_DIV(OSC_CLK , (LPC_SC->PCLKSEL & 0x1F));
          EMCClock        = (OSC_CLK / ((LPC_SC->EMCCLKSEL & 0x01)+1));
      }
    }
  }
  else {                                          /* cclk = pll_clk */
    if ((LPC_SC->PLL0STAT & 0x100) == 0) {        /* PLL0 not enabled */
          SystemCoreClock = 0;                      /* this should never happen! */
          PeripheralClock = 0;
          EMCClock        = 0;
    }
    else {
      if ((LPC_SC->CLKSRCSEL & 0x01) == 0) {    /* sysclk = irc_clk */
          SystemCoreClock = __CLK_DIV(IRC_OSC * ((LPC_SC->PLL0STAT & 0x1F) + 1) , (LPC_SC->CCLKSEL & 0x1F));
          PeripheralClock = __CLK_DIV(IRC_OSC * ((LPC_SC->PLL0STAT & 0x1F) + 1) , (LPC_SC->PCLKSEL & 0x1F));
          EMCClock        = (IRC_OSC * ((LPC_SC->PLL0STAT & 0x1F) + 1) / ((LPC_SC->EMCCLKSEL & 0x01)+1));
      }
      else {                                        /* sysclk = osc_clk */
        if ((LPC_SC->SCS & 0x40) == 0) {
          SystemCoreClock = 0;                      /* this should never happen! */
          PeripheralClock = 0;
          EMCClock        = 0;
        }
        else {
          SystemCoreClock = __CLK_DIV(OSC_CLK * ((LPC_SC->PLL0STAT & 0x1F) + 1) , (LPC_SC->CCLKSEL & 0x1F));
          PeripheralClock = __CLK_DIV(OSC_CLK * ((LPC_SC->PLL0STAT & 0x1F) + 1) , (LPC_SC->PCLKSEL & 0x1F));
          EMCClock        = (OSC_CLK * ((LPC_SC->PLL0STAT & 0x1F) + 1) / ((LPC_SC->EMCCLKSEL & 0x01)+1));
        }
      }
    }
  }
  /* ---update USBClock------------------*/
  if(LPC_SC->USBCLKSEL & (0x01<<8))//Use PLL0 as the input to the USB clock divider
  {
    switch (LPC_SC->USBCLKSEL & 0x1F)
    {
      case 0:
        USBClock = 0; //no clock will be provided to the USB subsystem
      break;
      case 4:
      case 6:
        if(LPC_SC->CLKSRCSEL & 0x01)  //pll_clk_in = main_osc
          USBClock = (OSC_CLK * ((LPC_SC->PLL0STAT & 0x1F) + 1) / (LPC_SC->USBCLKSEL & 0x1F));
        else //pll_clk_in = irc_clk
          USBClock = (IRC_OSC * ((LPC_SC->PLL0STAT & 0x1F) + 1) / (LPC_SC->USBCLKSEL & 0x1F));
      break;
      default:
        USBClock = 0;  /* this should never happen! */
    }
  }
  else if(LPC_SC->USBCLKSEL & (0x02<<8))//usb_input_clk = alt_pll (pll1)
  {
    if(LPC_SC->CLKSRCSEL & 0x01)  //pll1_clk_in = main_osc
      USBClock = (OSC_CLK * ((LPC_SC->PLL1STAT & 0x1F) + 1));
    else //pll1_clk_in = irc_clk
      USBClock = (IRC_OSC * ((LPC_SC->PLL0STAT & 0x1F) + 1));
  }
  else
    USBClock = 0; /* this should never happen! */
}

#if (EMC_SETUP != 0)

/*----------------------------------------------------------------------------
  Delay function in ms
 *----------------------------------------------------------------------------*/
static void _DelayMs(uint32_t ms) {
  ms *= (SystemCoreClock/10000);
  while (ms--) { __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); }
}

/*----------------------------------------------------------------------------
  Initialize external memory controller
 *----------------------------------------------------------------------------*/
void SystemInit_ExtMemCtl(void) {
  uint32_t i;

  /* Data and Address Bus configuration */
  #if (EMC_DATA_BUS >= 0)
  LPC_IOCON->P3_0  = 1;  // D0
  LPC_IOCON->P3_1  = 1;  // D1
  LPC_IOCON->P3_2  = 1;  // D2
  LPC_IOCON->P3_3  = 1;  // D3
  LPC_IOCON->P3_4  = 1;  // D4
  LPC_IOCON->P3_5  = 1;  // D5
  LPC_IOCON->P3_6  = 1;  // D6
  LPC_IOCON->P3_7  = 1;  // D7
  #endif

  #if (EMC_DATA_BUS > 0)
  LPC_IOCON->P3_8  = 1;  // D8
  LPC_IOCON->P3_9  = 1;  // D9
  LPC_IOCON->P3_10 = 1;  // D10
  LPC_IOCON->P3_11 = 1;  // D11
  LPC_IOCON->P3_12 = 1;  // D12
  LPC_IOCON->P3_13 = 1;  // D13
  LPC_IOCON->P3_14 = 1;  // D14
  LPC_IOCON->P3_15 = 1;  // D15
  #endif

  #if (EMC_DATA_BUS > 1)
  LPC_IOCON->P3_16 = 1;  // D16
  LPC_IOCON->P3_17 = 1;  // D17
  LPC_IOCON->P3_18 = 1;  // D18
  LPC_IOCON->P3_19 = 1;  // D19
  LPC_IOCON->P3_20 = 1;  // D20
  LPC_IOCON->P3_21 = 1;  // D21
  LPC_IOCON->P3_22 = 1;  // D22
  LPC_IOCON->P3_23 = 1;  // D23
  LPC_IOCON->P3_24 = 1;  // D24
  LPC_IOCON->P3_25 = 1;  // D25
  LPC_IOCON->P3_26 = 1;  // D26
  LPC_IOCON->P3_27 = 1;  // D27
  LPC_IOCON->P3_28 = 1;  // D28
  LPC_IOCON->P3_29 = 1;  // D29
  LPC_IOCON->P3_30 = 1;  // D30
  LPC_IOCON->P3_31 = 1;  // D31
  #endif

  #if (EMC_ADDR_BUS & 0x00000001)
  LPC_IOCON->P4_0  = 1;  // A0
  #endif
  #if (EMC_ADDR_BUS & 0x00000002)
  LPC_IOCON->P4_1  = 1;  // A1
  #endif
  #if (EMC_ADDR_BUS & 0x00000004)
  LPC_IOCON->P4_2  = 1;  // A2
  #endif
  #if (EMC_ADDR_BUS & 0x00000008)
  LPC_IOCON->P4_3  = 1;  // A3
  #endif
  #if (EMC_ADDR_BUS & 0x00000010)
  LPC_IOCON->P4_4  = 1;  // A4
  #endif
  #if (EMC_ADDR_BUS & 0x00000020)
  LPC_IOCON->P4_5  = 1;  // A5
  #endif
  #if (EMC_ADDR_BUS & 0x00000040)
  LPC_IOCON->P4_6  = 1;  // A6
  #endif
  #if (EMC_ADDR_BUS & 0x00000080)
  LPC_IOCON->P4_7  = 1;  // A7
  #endif
  #if (EMC_ADDR_BUS & 0x00000100)
  LPC_IOCON->P4_8  = 1;  // A8
  #endif
  #if (EMC_ADDR_BUS & 0x00000200)
  LPC_IOCON->P4_9  = 1;  // A9
  #endif
  #if (EMC_ADDR_BUS & 0x00000400)
  LPC_IOCON->P4_10 = 1;  // A10
  #endif
  #if (EMC_ADDR_BUS & 0x00000800)
  LPC_IOCON->P4_11 = 1;  // A11
  #endif
  #if (EMC_ADDR_BUS & 0x00001000)
  LPC_IOCON->P4_12 = 1;  // A12
  #endif
  #if (EMC_ADDR_BUS & 0x00002000)
  LPC_IOCON->P4_13 = 1;  // A13
  #endif
  #if (EMC_ADDR_BUS & 0x00004000)
  LPC_IOCON->P4_14 = 1;  // A14
  #endif
  #if (EMC_ADDR_BUS & 0x00008000)
  LPC_IOCON->P4_15 = 1;  // A15
  #endif
  #if (EMC_ADDR_BUS & 0x00010000)
  LPC_IOCON->P4_16 = 1;  // A16
  #endif
  #if (EMC_ADDR_BUS & 0x00020000)
  LPC_IOCON->P4_17 = 1;  // A17
  #endif
  #if (EMC_ADDR_BUS & 0x00040000)
  LPC_IOCON->P4_18 = 1;  // A18
  #endif
  #if (EMC_ADDR_BUS & 0x00080000)
  LPC_IOCON->P4_19 = 1;  // A19
  #endif
  #if (EMC_ADDR_BUS & 0x00100000)
  LPC_IOCON->P4_20 = 1;  // A20
  #endif
  #if (EMC_ADDR_BUS & 0x00200000)
  LPC_IOCON->P4_21 = 1;  // A21
  #endif
  #if (EMC_ADDR_BUS & 0x00400000)
  LPC_IOCON->P4_22 = 1;  // A22
  #endif
  #if (EMC_ADDR_BUS & 0x00800000)
  LPC_IOCON->P4_23 = 1;  // A23
  #endif
  #if (EMC_ADDR_BUS & 0x01000000)
  LPC_IOCON->P5_0  = 1;  // A24
  #endif
  #if (EMC_ADDR_BUS & 0x02000000)
  LPC_IOCON->P5_1  = 1;  // A25
  #endif

  LPC_IOCON->P4_25 = 1;  // WE

  #if (EMC_CTRL_PIN_CLK0 != 0)
  LPC_IOCON->P2_18 = 1;  // CLKOUT0
  #endif
  #if (EMC_CTRL_PIN_CLK1 != 0)
  LPC_IOCON->P2_19 = 1;  // CLKOUT1
  #endif

#if (EMC_DYNAMIC_SETUP != 0)
  /* Configure dynamic memory control pins */
  LPC_IOCON->P2_16 = 1;  // CAS
  LPC_IOCON->P2_17 = 1;  // RAS

  LPC_IOCON->P2_28 = 1;  // DQMOUT0
  LPC_IOCON->P2_29 = 1;  // DQMOUT1
  #if ((DYNCS0_BUS_WIDTH == 32) || \
       (DYNCS1_BUS_WIDTH == 32) || \
       (DYNCS1_BUS_WIDTH == 32) || \
       (DYNCS2_BUS_WIDTH == 32))
  LPC_IOCON->P2_30 = 1;  // DQMOUT2
  LPC_IOCON->P2_31 = 1;  // DQMOUT3
  #endif

  #if (EMC_DYNCS0_SETUP != 0)
  LPC_IOCON->P2_20 = 1;  // DYCS0
  LPC_IOCON->P2_24 = 1;  // CKEOUT0
  #endif

  #if (EMC_DYNCS1_SETUP != 0)
  LPC_IOCON->P2_21 = 1;  // DYCS1
  LPC_IOCON->P2_25 = 1;  // CKEOUT1
  #endif

  #if (EMC_DYNCS2_SETUP != 0)
  LPC_IOCON->P2_22 = 1;  // DYCS2
  LPC_IOCON->P2_26 = 1;  // CKEOUT2
  #endif

  #if (EMC_DYNCS3_SETUP != 0)
  LPC_IOCON->P2_23 = 1;  // DYCS3
  LPC_IOCON->P2_27 = 1;  // CKEOUT3
  #endif
#endif

#if (EMC_STATIC_SETUP != 0)
  /* Configure static memory control pins */
  LPC_IOCON->P4_24 = 1;  // OE

  LPC_IOCON->P4_26 = 1;  // BLS0
  #if ((STACS0_BUS_WIDTH >= 16) || \
       (STACS1_BUS_WIDTH >= 16) || \
       (STACS2_BUS_WIDTH >= 16) || \
       (STACS3_BUS_WIDTH >= 16))
  LPC_IOCON->P4_27 = 1;  // BLS1
  #endif

  #if ((STACS0_BUS_WIDTH == 32) || \
       (STACS1_BUS_WIDTH == 32) || \
       (STACS2_BUS_WIDTH == 32) || \
       (STACS3_BUS_WIDTH == 32))
  LPC_IOCON->P4_28 = 1;  // BLS2
  LPC_IOCON->P4_29 = 1;  // BLS3
  #endif

  #if (EMC_STACS0_SETUP != 0)
  LPC_IOCON->P4_30 = 1;  // CS0
  #endif
  #if (EMC_STACS1_SETUP != 0)
  LPC_IOCON->P4_31 = 1;  // CS1
  #endif
  #if (EMC_STACS2_SETUP != 0)
  LPC_IOCON->P2_14 = 1;  // CS2
  #endif
  #if (EMC_STACS3_SETUP != 0)
  LPC_IOCON->P2_15 = 1;  // CS3
  #endif
#endif

  LPC_SC->EMCDLYCTL |= 0x1111;      // CMDDLY = 0x11 , FBDELAY = 0x11
  LPC_SC->PCONP     |= (1 << 11);   // Turn on EMC peripheral clock

  LPC_EMC->Control = EMC_CTRL_Val;
  LPC_EMC->Config  = EMC_CONFIG_Val;

#if (EMC_DYNAMIC_SETUP != 0)

  LPC_EMC->DynamicRefresh    = EMC_DYN_RFSH_Val;
  LPC_EMC->DynamicReadConfig = EMC_DYN_RD_CFG_Val;

  LPC_EMC->DynamicRP   = EMC_DYN_RP_Val;
  LPC_EMC->DynamicRAS  = EMC_DYN_RAS_Val;
  LPC_EMC->DynamicSREX = EMC_DYN_SREX_Val;
  LPC_EMC->DynamicAPR  = EMC_DYN_APR_Val;
  LPC_EMC->DynamicDAL  = EMC_DYN_DAL_Val;
  LPC_EMC->DynamicWR   = EMC_DYN_WR_Val;
  LPC_EMC->DynamicRC   = EMC_DYN_RC_Val;
  LPC_EMC->DynamicRFC  = EMC_DYN_RFC_Val;
  LPC_EMC->DynamicXSR  = EMC_DYN_XSR_Val;
  LPC_EMC->DynamicRRD  = EMC_DYN_RRD_Val;
  LPC_EMC->DynamicMRD  = EMC_DYN_MRD_Val;

#if (EMC_DYNCS0_SETUP != 0)
  LPC_EMC->DynamicConfig0 = EMC_DYN_CFG0_Val & ~(1 << 19);
  LPC_EMC->DynamicRasCas0 = EMC_DYN_RASCAS0_Val;
#endif

#if (EMC_DYNCS1_SETUP != 0)
  LPC_EMC->DynamicConfig1 = EMC_DYN_CFG1_Val & ~(1 << 19);
  LPC_EMC->DynamicRasCas1 = EMC_DYN_RASCAS1_Val;
#endif

#if (EMC_DYNCS2_SETUP != 0)
  LPC_EMC->DynamicConfig2 = EMC_DYN_CFG2_Val & ~(1 << 19);
  LPC_EMC->DynamicRasCas2 = EMC_DYN_RASCAS2_Val;
#endif

#if (EMC_DYNCS3_SETUP != 0)
  LPC_EMC->DynamicConfig3 = EMC_DYN_CFG3_Val & ~(1 << 19);
  LPC_EMC->DynamicRasCas3 = EMC_DYN_RASCAS3_Val;
#endif

  /* Start SDRAM initialization procedure */
  _DelayMs(100);
  LPC_EMC->DynamicControl = 0x00000183;  // Issue NOP command
  _DelayMs(200);
  LPC_EMC->DynamicControl = 0x00000103;  // Issue PALL command
  LPC_EMC->DynamicRefresh = 0x00000002;  // n * 16 clock cycles
  for (i = 0; i < 0x80; i++);            // Wait 128 AHB clock cycles
  LPC_EMC->DynamicRefresh = 0x0000003A;  // n * 16 clock cycles

  LPC_EMC->DynamicControl = 0x00000083;  // Issue MODE command

  #if (EMC_DYNCS0_SETUP != 0)
  *((volatile uint32_t*)(0xA0000000 | (DYNCS0_SDRAM_MODE << (DYNCS0_BANK_BITS + DYNCS0_COL_BITS + DYNCS0_BUS_WIDTH/16))));
  LPC_EMC->DynamicConfig0 = EMC_DYN_CFG0_Val;
  #endif

  #if (EMC_DYNCS1_SETUP != 0)
  *((volatile uint32_t*)(0xB0000000 | (DYNCS1_SDRAM_MODE << (DYNCS1_BANK_BITS + DYNCS1_COL_BITS + DYNCS1_BUS_WIDTH/16))));
  LPC_EMC->DynamicConfig1 = EMC_DYN_CFG1_Val;
  #endif

  #if (EMC_DYNCS2_SETUP != 0)
  *((volatile uint32_t*)(0xC0000000 | (DYNCS2_SDRAM_MODE << (DYNCS2_BANK_BITS + DYNCS2_COL_BITS + DYNCS2_BUS_WIDTH/16))));
  LPC_EMC->DynamicConfig2 = EMC_DYN_CFG2_Val;
  #endif

  #if (EMC_DYNCS3_SETUP != 0)
  *((volatile uint32_t*)(0xD0000000 | (DYNCS3_SDRAM_MODE << (DYNCS3_BANK_BITS + DYNCS3_COL_BITS + DYNCS3_BUS_WIDTH/16))));
  LPC_EMC->DynamicConfig3 = EMC_DYN_CFG3_Val;
  #endif

  LPC_EMC->DynamicControl = 0x00000000;  // Issue NORMAL command

#endif /* (EMC_DYNAMIC_SETUP != 0) */

#if (EMC_STATIC_SETUP != 0)

#if (EMC_STACS0_SETUP != 0)
  LPC_EMC->StaticConfig0   = EMC_STA_CFG0_Val;
  LPC_EMC->StaticWaitWen0  = EMC_STA_WWEN0_Val;
  LPC_EMC->StaticWaitOen0  = EMC_STA_WOEN0_Val;
  LPC_EMC->StaticWaitRd0   = EMC_STA_WRD0_Val;
  LPC_EMC->StaticWaitPage0 = EMC_STA_WPAGE0_Val;
  LPC_EMC->StaticWaitWr0   = EMC_STA_WWR0_Val;
  LPC_EMC->StaticWaitTurn0 = EMC_STA_WTURN0_Val;
#endif

#if (EMC_STACS1_SETUP != 0)
  LPC_EMC->StaticConfig1   = EMC_STA_CFG1_Val;
  LPC_EMC->StaticWaitWen1  = EMC_STA_WWEN1_Val;
  LPC_EMC->StaticWaitOen1  = EMC_STA_WOEN1_Val;
  LPC_EMC->StaticWaitRd1   = EMC_STA_WRD1_Val;
  LPC_EMC->StaticWaitPage1 = EMC_STA_WPAGE1_Val;
  LPC_EMC->StaticWaitWr1   = EMC_STA_WWR1_Val;
  LPC_EMC->StaticWaitTurn1 = EMC_STA_WTURN1_Val;
#endif

#if (EMC_STACS2_SETUP != 0)
  LPC_EMC->StaticConfig2   = EMC_STA_CFG2_Val;
  LPC_EMC->StaticWaitWen2  = EMC_STA_WWEN2_Val;
  LPC_EMC->StaticWaitOen2  = EMC_STA_WOEN2_Val;
  LPC_EMC->StaticWaitRd2   = EMC_STA_WRD2_Val;
  LPC_EMC->StaticWaitPage2 = EMC_STA_WPAGE2_Val;
  LPC_EMC->StaticWaitWr2   = EMC_STA_WWR2_Val;
  LPC_EMC->StaticWaitTurn2 = EMC_STA_WTURN2_Val;
#endif

#if (EMC_STACS3_SETUP != 0)
  LPC_EMC->StaticConfig3   = EMC_STA_CFG3_Val;
  LPC_EMC->StaticWaitWen3  = EMC_STA_WWEN3_Val;
  LPC_EMC->StaticWaitOen3  = EMC_STA_WOEN3_Val;
  LPC_EMC->StaticWaitRd3   = EMC_STA_WRD3_Val;
  LPC_EMC->StaticWaitPage3 = EMC_STA_WPAGE3_Val;
  LPC_EMC->StaticWaitWr3   = EMC_STA_WWR3_Val;
  LPC_EMC->StaticWaitTurn3 = EMC_STA_WTURN3_Val;
#endif

  LPC_EMC->StaticExtendedWait = EMC_STA_EXT_W_Val;

#endif /* (EMC_STATIC_SETUP != 0) */
}
#endif /* (EMC_SETUP != 0) */

  /* Determine clock frequency according to clock register values             */

/*----------------------------------------------------------------------------
  SystemInit
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{
#if (CLOCK_SETUP)                       /* Clock Setup                        */
  LPC_SC->SCS       = SCS_Val;
  if (SCS_Val & (1 << 5)) {             /* If Main Oscillator is enabled      */
    while ((LPC_SC->SCS & (1<<6)) == 0);/* Wait for Oscillator to be ready    */
  }

  LPC_SC->CLKSRCSEL = CLKSRCSEL_Val;    /* Select Clock Source for sysclk/PLL0*/

#if (PLL0_SETUP)
  LPC_SC->PLL0CFG   = PLL0CFG_Val;
  LPC_SC->PLL0CON   = 0x01;             /* PLL0 Enable                        */
  LPC_SC->PLL0FEED  = 0xAA;
  LPC_SC->PLL0FEED  = 0x55;
  while (!(LPC_SC->PLL0STAT & (1<<10)));/* Wait for PLOCK0                    */
#endif

#if (PLL1_SETUP)
  LPC_SC->PLL1CFG   = PLL1CFG_Val;
  LPC_SC->PLL1CON   = 0x01;             /* PLL1 Enable                        */
  LPC_SC->PLL1FEED  = 0xAA;
  LPC_SC->PLL1FEED  = 0x55;
  while (!(LPC_SC->PLL1STAT & (1<<10)));/* Wait for PLOCK1                    */
#endif

  LPC_SC->CCLKSEL   = CCLKSEL_Val;      /* Setup Clock Divider                */
  LPC_SC->USBCLKSEL = USBCLKSEL_Val;    /* Setup USB Clock Divider            */
  LPC_SC->EMCCLKSEL = EMCCLKSEL_Val;    /* EMC Clock Selection                */
  LPC_SC->PCLKSEL   = PCLKSEL_Val;      /* Peripheral Clock Selection         */
  LPC_SC->PCONP     = PCONP_Val;        /* Power Control for Peripherals      */
  LPC_SC->CLKOUTCFG = CLKOUTCFG_Val;    /* Clock Output Configuration         */
#endif

  LPC_SC->PBOOST   |= 0x03;             /* Power Boost control                */

#if (FLASH_SETUP == 1)                  /* Flash Accelerator Setup            */
  LPC_SC->FLASHCFG  = FLASHCFG_Val|0x03A;
#endif
#ifdef  __RAM_MODE__
  SCB->VTOR  = 0x10000000 & 0x3FFFFF80;
#else
  SCB->VTOR  = 0x00000000 & 0x3FFFFF80;
#endif

  /* Update SystemCoreClock variable */
  SystemCoreClockUpdate();

#if (EMC_SETUP != 0)
  /* Configure External Memory Controller */
  SystemInit_ExtMemCtl ();
#endif
}
