/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * $Date:        02. March 2016
 * $Revision:    V2.4.0
 *
 * Project:      RTE Device Configuration for NXP LPC17xx
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


// <e> USB Controller [Driver_USBD and Driver_USBH]
// <i> Configuration settings for Driver_USBD in component ::Drivers:USB Device
// <i> Configuration settings for Driver_USBH in component ::Drivers:USB Host
#define   RTE_USB_USB0                  0

//   <h> Pin Configuration
//     <o> USB_PPWR (Host) <0=>Not used <1=>P1_19
//     <i> VBUS drive signal (towards external charge pump or power management unit).
#define   RTE_USB_PPWR_ID               1
#if      (RTE_USB_PPWR_ID == 0)
  #define RTE_USB_PPWR_PIN_EN           0
#elif    (RTE_USB_PPWR_ID == 1)
  #define RTE_USB_PPWR_PIN_EN           1
#else
  #error "Invalid RTE_USB_PPWR Pin Configuration!"
#endif

//     <o> USB_PWRD (Host) <0=>Not used <1=>P1_22
//     <i> Power Status for USB port.
#define   RTE_USB_PWRD_ID               1
#if      (RTE_USB_PWRD_ID == 0)
  #define RTE_USB_PWRD_PIN_EN           0
#elif    (RTE_USB_PWRD_ID == 1)
  #define RTE_USB_PWRD_PIN_EN           1
#else
  #error "Invalid RTE_USB_PWRD Pin Configuration!"
#endif

//     <o> USB_OVRCR (Host) <0=>Not used <1=>P1_27
//     <i> Port power fault signal indicating overcurrent condition.
//     <i> This signal monitors over-current on the USB bus
//        (external circuitry required to detect over-current condition).
#define   RTE_USB_OVRCR_ID              0
#if      (RTE_USB_OVRCR_ID == 0)
  #define RTE_USB_OVRCR_PIN_EN          0
#elif    (RTE_USB_OVRCR_ID == 1)
  #define RTE_USB_OVRCR_PIN_EN          1
#else
  #error "Invalid RTE_USB_OVRCR Pin Configuration!"
#endif

//     <o> USB_CONNECT (Device) <0=>Not used <1=>P2_9
//     <i> SoftConnect control signal
#define   RTE_USB_CONNECT_ID            1
#if      (RTE_USB_CONNECT_ID == 0)
  #define RTE_USB_CONNECT_PIN_EN        0
#elif    (RTE_USB_CONNECT_ID == 1)
  #define RTE_USB_CONNECT_PIN_EN        1
#else
  #error "Invalid RTE_USB_CONNECT Pin Configuration!"
#endif

//     <o> USB_VBUS (Device) <0=>Not used <1=>P1_30
//     <i> VBUS status input.
//     <i> When this function is not enabled via its corresponding PINSEL register, it is driven HIGH internally.
#define   RTE_USB_VBUS_ID               1
#if      (RTE_USB_VBUS_ID == 0)
  #define RTE_USB_VBUS_PIN_EN           0
#elif    (RTE_USB_VBUS_ID == 1)
  #define RTE_USB_VBUS_PIN_EN           1
#else
  #error "Invalid RTE_USB_VBUS Pin Configuration!"
#endif

//     <o> USB_UP_LED <0=>Not used <1=>P1_18
//     <i> GoodLink LED control signal.
#define   RTE_USB_UP_LED_ID             1
#if      (RTE_USB_UP_LED_ID == 0)
  #define RTE_USB_UP_LED_PIN_EN         0
#elif    (RTE_USB_UP_LED_ID == 1)
  #define RTE_USB_UP_LED_PIN_EN         1
#else
  #error "Invalid RTE_USB_UP_LED Pin Configuration!"
#endif

//  </h> Pin Configuration
// </e> USB Controller [Driver_USBD and Driver_USBH]


// <e> ENET (Ethernet Interface) [Driver_ETH_MAC0]
// <i> Configuration settings for Driver_ETH_MAC0 in component ::Drivers:Ethernet MAC
#define RTE_ENET                        0


//   <h> RMII (Reduced Media Independent Interface)
#define RTE_ENET_RMII                   1

//     <o> ENET_TXD0 Pin <0=>P1_0
#define RTE_ENET_RMII_TXD0_PORT_ID      0
#if    (RTE_ENET_RMII_TXD0_PORT_ID == 0)
#define RTE_ENET_RMII_TXD0_PORT         1
#define RTE_ENET_RMII_TXD0_PIN          0
#define RTE_ENET_RMII_TXD0_FUNC         1
#else
#error "Invalid ENET_TXD0 Pin Configuration!"
#endif
//     <o> ENET_TXD1 Pin <0=>P1_1
#define RTE_ENET_RMII_TXD1_PORT_ID      0
#if    (RTE_ENET_RMII_TXD1_PORT_ID == 0)
#define RTE_ENET_RMII_TXD1_PORT         1
#define RTE_ENET_RMII_TXD1_PIN          1
#define RTE_ENET_RMII_TXD1_FUNC         1
#else
#error "Invalid ENET_TXD1 Pin Configuration!"
#endif
//     <o> ENET_REF_CLK Pin <0=>P1_15
#define RTE_ENET_RMII_REF_CLK_PORT_ID   0
#if    (RTE_ENET_RMII_REF_CLK_PORT_ID == 0)
#define RTE_ENET_RMII_REF_CLK_PORT      1
#define RTE_ENET_RMII_REF_CLK_PIN       15
#define RTE_ENET_RMII_REF_CLK_FUNC      1
#else
#error "Invalid ENET_REF_CLK Pin Configuration!"
#endif
//     <o> ENET_TX_EN Pin <0=>P1_4
#define RTE_ENET_RMII_TX_EN_PORT_ID     0
#if    (RTE_ENET_RMII_TX_EN_PORT_ID == 0)
#define RTE_ENET_RMII_TX_EN_PORT        1
#define RTE_ENET_RMII_TX_EN_PIN         4
#define RTE_ENET_RMII_TX_EN_FUNC        1
#else
#error "Invalid ENET_TX_EN Pin Configuration!"
#endif
//     <o> ENET_CRS Pin <0=>P1_8
#define RTE_ENET_RMII_CRS_PORT_ID       0
#if    (RTE_ENET_RMII_CRS_PORT_ID == 0)
#define RTE_ENET_RMII_CRS_PORT          1
#define RTE_ENET_RMII_CRS_PIN           8
#define RTE_ENET_RMII_CRS_FUNC          1
#else
#error "Invalid ENET_CRS Pin Configuration!"
#endif
//     <o> ENET_RXD0 Pin <0=>P1_9
#define RTE_ENET_RMII_RXD0_PORT_ID      0
#if    (RTE_ENET_RMII_RXD0_PORT_ID == 0)
#define RTE_ENET_RMII_RXD0_PORT         1
#define RTE_ENET_RMII_RXD0_PIN          9
#define RTE_ENET_RMII_RXD0_FUNC         1
#else
#error "Invalid ENET_RXD0 Pin Configuration!"
#endif
//     <o> ENET_RXD1 Pin <0=>P1_10
#define RTE_ENET_RMII_RXD1_PORT_ID      0
#if    (RTE_ENET_RMII_RXD1_PORT_ID == 0)
#define RTE_ENET_RMII_RXD1_PORT         1
#define RTE_ENET_RMII_RXD1_PIN          10
#define RTE_ENET_RMII_RXD1_FUNC         1
#else
#error "Invalid ENET_RXD1 Pin Configuration!"
#endif
//     <o> ENET_RX_ER Pin <0=>P1_14
#define RTE_ENET_RMII_RX_ER_PORT_ID     0
#if    (RTE_ENET_RMII_RX_ER_PORT_ID == 0)
#define RTE_ENET_RMII_RX_ER_PORT        1
#define RTE_ENET_RMII_RX_ER_PIN         14
#define RTE_ENET_RMII_RX_ER_FUNC        1
#else
#error "Invalid ENET_REF_CLK Pin Configuration!"
#endif
//   </h>

//   <h> MIIM (Management Data Interface)
//     <o> ENET_MDC Pin <0=>P1_16 <1=>P2_8
#define RTE_ENET_MDI_MDC_PORT_ID        0
#if    (RTE_ENET_MDI_MDC_PORT_ID == 0)
#define RTE_ENET_MDI_MDC_PORT           1
#define RTE_ENET_MDI_MDC_PIN            16
#define RTE_ENET_MDI_MDC_FUNC           1
#elif  (RTE_ENET_MDI_MDC_PORT_ID == 1)  
#define RTE_ENET_MDI_MDC_PORT           2
#define RTE_ENET_MDI_MDC_PIN            8
#define RTE_ENET_MDI_MDC_FUNC           3
#else
#error "Invalid ENET_MDC Pin Configuration!"
#endif
//     <o> ENET_MDIO Pin <0=>P1_17 <1=>P2_9
#define RTE_ENET_MDI_MDIO_PORT_ID       0
#if    (RTE_ENET_MDI_MDIO_PORT_ID == 0)
#define RTE_ENET_MDI_MDIO_PORT          1
#define RTE_ENET_MDI_MDIO_PIN           17
#define RTE_ENET_MDI_MDIO_FUNC          1
#elif  (RTE_ENET_MDI_MDIO_PORT_ID == 1)
#define RTE_ENET_MDI_MDIO_PORT          2
#define RTE_ENET_MDI_MDIO_PIN           9
#define RTE_ENET_MDI_MDIO_FUNC          3
#else
#error "Invalid ENET_MDIO Pin Configuration!"
#endif
//   </h>

// </e>


// <e> I2C0 (Inter-integrated Circuit Interface 0) [Driver_I2C0]
// <i> Configuration settings for Driver_I2C0 in component ::Drivers:I2C
#define RTE_I2C0                        0

//   <o> I2C0_SCL Pin <0=>P0_28
#define RTE_I2C0_SCL_PORT_ID            0
#if    (RTE_I2C0_SCL_PORT_ID == 0)
#define RTE_I2C0_SCL_PORT               0
#define RTE_I2C0_SCL_PIN                28
#define RTE_I2C0_SCL_FUNC               1
#else
#error "Invalid I2C0_SCL Pin Configuration!"
#endif

//   <o> I2C0_SDA Pin <0=>P0_27
#define RTE_I2C0_SDA_PORT_ID            0
#if    (RTE_I2C0_SDA_PORT_ID == 0)
#define RTE_I2C0_SDA_PORT               0
#define RTE_I2C0_SDA_PIN                27
#define RTE_I2C0_SDA_FUNC               1
#else
#error "Invalid I2C0_SDA Pin Configuration!"
#endif

// </e>


// <e> I2C1 (Inter-integrated Circuit Interface 1) [Driver_I2C1]
// <i> Configuration settings for Driver_I2C1 in component ::Drivers:I2C
#define RTE_I2C1                        0

//   <o> I2C1_SCL Pin <0=>P0_1 <1=>P0_20
#define RTE_I2C1_SCL_PORT_ID            0
#if    (RTE_I2C1_SCL_PORT_ID == 0)
#define RTE_I2C1_SCL_PORT               0
#define RTE_I2C1_SCL_PIN                1
#define RTE_I2C1_SCL_FUNC               3
#elif  (RTE_I2C1_SCL_PORT_ID == 1)
#define RTE_I2C1_SCL_PORT               0
#define RTE_I2C1_SCL_PIN                20
#define RTE_I2C1_SCL_FUNC               3
#else
#error "Invalid I2C1_SCL Pin Configuration!"
#endif

//   <o> I2C1_SDA Pin <0=>P0_0 <1=>P0_19
#define RTE_I2C1_SDA_PORT_ID            0
#if    (RTE_I2C1_SDA_PORT_ID == 0)
#define RTE_I2C1_SDA_PORT               0
#define RTE_I2C1_SDA_PIN                0
#define RTE_I2C1_SDA_FUNC               3
#elif  (RTE_I2C1_SDA_PORT_ID == 1)
#define RTE_I2C1_SDA_PORT               0
#define RTE_I2C1_SDA_PIN                19
#define RTE_I2C1_SDA_FUNC               3
#else
#error "Invalid I2C1_SDA Pin Configuration!"
#endif

// </e>


// <e> I2C2 (Inter-integrated Circuit Interface 2) [Driver_I2C2]
// <i> Configuration settings for Driver_I2C2 in component ::Drivers:I2C
#define RTE_I2C2                        0

//   <o> I2C2_SCL Pin <0=>P0_11
#define RTE_I2C2_SCL_PORT_ID            0
#if    (RTE_I2C2_SCL_PORT_ID == 0)
#define RTE_I2C2_SCL_PORT               0
#define RTE_I2C2_SCL_PIN                11
#define RTE_I2C2_SCL_FUNC               2
#else
#error "Invalid I2C2_SCL Pin Configuration!"
#endif

//   <o> I2C2_SDA Pin <0=>P0_10
#define RTE_I2C2_SDA_PORT_ID            0
#if    (RTE_I2C2_SDA_PORT_ID == 0)
#define RTE_I2C2_SDA_PORT               0
#define RTE_I2C2_SDA_PIN                10
#define RTE_I2C2_SDA_FUNC               2
#else
#error "Invalid I2C2_SDA Pin Configuration!"
#endif

// </e>

// <e> UART0 (Universal asynchronous receiver transmitter)
#define RTE_UART0                       0

//   <o> UART0_TX Pin <0=>P0_2
//   <i> UART0 Serial Output pin
#define RTE_UART0_TX_ID                 0
#if    (RTE_UART0_TX_ID == 0)
#define RTE_UART0_TX_PORT               0
#define RTE_UART0_TX_BIT                2
#define RTE_UART0_TX_FUNC               1
#else
#error "Invalid UART0_TX Pin Configuration!"
#endif

//   <o> UART0_RX Pin <0=>P0_3
//   <i> UART0 Serial Input pin
#define RTE_UART0_RX_ID                 0
#if    (RTE_UART0_RX_ID == 0)
#define RTE_UART0_RX_PORT               0
#define RTE_UART0_RX_BIT                3
#define RTE_UART0_RX_FUNC               1
#else
#error "Invalid UART0_RX Pin Configuration!"
#endif
//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART0_DMA_TX_EN           1
#define   RTE_UART0_DMA_TX_CH           0
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART0_DMA_RX_EN           1
#define   RTE_UART0_DMA_RX_CH           1
//   </h> DMA

//     </e>
// <e> UART1 (Universal asynchronous receiver transmitter)
#define RTE_UART1                       0

//   <o> U1_TX Pin <0=>P0_15 <1=>P2_0
//   <i> UART1 Serial Output pin
#define RTE_UART1_TX_ID                 1
#if    (RTE_UART1_TX_ID == 0)
#define RTE_UART1_TX_PORT               0
#define RTE_UART1_TX_BIT                15
#define RTE_UART1_TX_FUNC               1
#elif  (RTE_UART1_TX_ID == 1)
#define RTE_UART1_TX_PORT               2
#define RTE_UART1_TX_BIT                0
#define RTE_UART1_TX_FUNC               2
#else
#error "Invalid U1_TX Pin Configuration!"
#endif

//   <o> U1_RX Pin <0=>P0_16 <1=>P2_1
//   <i> UART1 Serial Input pin
#define RTE_UART1_RX_ID                 1
#if    (RTE_UART1_RX_ID == 0)
#define RTE_UART1_RX_PORT               0
#define RTE_UART1_RX_BIT                16
#define RTE_UART1_RX_FUNC               1
#elif  (RTE_UART1_RX_ID == 1)
#define RTE_UART1_RX_PORT               2
#define RTE_UART1_RX_BIT                1
#define RTE_UART1_RX_FUNC               2
#else
#error "Invalid U1_RX Pin Configuration!"
#endif

//     <h> Modem Lines
//      <o> CTS <0=>Not used <1=>P0_17 <2=>P2_2
#define RTE_UART1_CTS_ID                0
#if    (RTE_UART1_CTS_ID == 0)
#define RTE_UART1_CTS_PIN_EN            0
#elif  (RTE_UART1_CTS_ID == 1)
#define RTE_UART1_CTS_PORT              0
#define RTE_UART1_CTS_BIT               17
#define RTE_UART1_CTS_FUNC              1
#elif  (RTE_UART1_CTS_ID == 2)
#define RTE_UART1_CTS_PORT              2
#define RTE_UART1_CTS_BIT               2
#define RTE_UART1_CTS_FUNC              2
#else
#error "Invalid U1_CTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_CTS_PIN_EN
#define RTE_UART1_CTS_PIN_EN            1
#endif

//       
//      <o> DCD <0=>Not used <1=>P0_18 <2=>P2_3
#define RTE_UART1_DCD_ID                0
#if    (RTE_UART1_DCD_ID == 0)
#define RTE_UART1_DCD_PIN_EN            0
#elif  (RTE_UART1_DCD_ID == 1)
#define RTE_UART1_DCD_PORT              0
#define RTE_UART1_DCD_BIT               18
#define RTE_UART1_DCD_FUNC              1
#elif  (RTE_UART1_DCD_ID == 2)
#define RTE_UART1_DCD_PORT              2
#define RTE_UART1_DCD_BIT               3
#define RTE_UART1_DCD_FUNC              2
#else
#error "Invalid UART1_DCD Pin Configuration!"
#endif
#ifndef   RTE_UART1_DCD_PIN_EN
#define RTE_UART1_DCD_PIN_EN            1
#endif

//      <o> DSR <0=>Not used <1=>P0_19  <2=>P2_4
#define RTE_UART1_DSR_ID                0
#if    (RTE_UART1_DSR_ID == 0)
#define RTE_UART1_DSR_PIN_EN            0
#elif  (RTE_UART1_DSR_ID == 1)
#define RTE_UART1_DSR_PORT              0
#define RTE_UART1_DSR_BIT               19
#define RTE_UART1_DSR_FUNC              1
#elif  (RTE_UART1_DSR_ID == 2)
#define RTE_UART1_DSR_PORT              2
#define RTE_UART1_DSR_BIT               4
#define RTE_UART1_DSR_FUNC              2
#else
#error "Invalid UART1_DSR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DSR_PIN_EN
#define RTE_UART1_DSR_PIN_EN            1
#endif

//      <o> DTR <0=>Not used <1=>P0_20  <2=>P2_5
#define RTE_UART1_DTR_ID                0
#if    (RTE_UART1_DTR_ID == 0)
#define RTE_UART1_DTR_PIN_EN            0
#elif  (RTE_UART1_DTR_ID == 1)
#define RTE_UART1_DTR_PORT              0
#define RTE_UART1_DTR_BIT               20
#define RTE_UART1_DTR_FUNC              1
#elif  (RTE_UART1_DTR_ID == 2)
#define RTE_UART1_DTR_PORT              2
#define RTE_UART1_DTR_BIT               5
#define RTE_UART1_DTR_FUNC              2
#else
#error "Invalid UART1_DTR Pin Configuration!"
#endif
#ifndef   RTE_UART1_DTR_PIN_EN
#define RTE_UART1_DTR_PIN_EN            1
#endif

//       <o> RI <0=>Not used <1=>P0_21 <2=>P2_6
#define RTE_UART1_RI_ID                 0
#if    (RTE_UART1_RI_ID == 0)
#define RTE_UART1_RI_PIN_EN             0
#elif  (RTE_UART1_RI_ID == 1)
#define RTE_UART1_RI_PORT               0
#define RTE_UART1_RI_BIT                21
#define RTE_UART1_RI_FUNC               1
#elif  (RTE_UART1_RI_ID == 2)
#define RTE_UART1_RI_PORT               2
#define RTE_UART1_RI_BIT                6
#define RTE_UART1_RI_FUNC               2
#else
#error "Invalid UART1_RI Pin Configuration!"
#endif
#ifndef   RTE_UART1_RI_PIN_EN
#define RTE_UART1_RI_PIN_EN             1
#endif

//       <o> RTS <0=>Not used <1=>P0_22  <2=>P2_7
#define RTE_UART1_RTS_ID                0
#if    (RTE_UART1_RTS_ID == 0)
#define RTE_UART1_RTS_PIN_EN            0
#elif  (RTE_UART1_RTS_ID == 1)
#define RTE_UART1_RTS_PORT              0
#define RTE_UART1_RTS_BIT               22
#define RTE_UART1_RTS_FUNC              1
#elif  (RTE_UART1_RTS_ID == 2)
#define RTE_UART1_RTS_PORT              2
#define RTE_UART1_RTS_BIT               7
#define RTE_UART1_RTS_FUNC              2
#else
#error "Invalid UART1_RTS Pin Configuration!"
#endif
#ifndef   RTE_UART1_RTS_PIN_EN
#define RTE_UART1_RTS_PIN_EN            1
#endif

//     </h>

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART1_DMA_TX_EN           1
#define   RTE_UART1_DMA_TX_CH           0
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART1_DMA_RX_EN           1
#define   RTE_UART1_DMA_RX_CH           1
//   </h> DMA

// </e>

// <e> UART2 (Universal asynchronous receiver transmitter)
#define RTE_UART2                       0

//   <o> UART2_TX Pin <0=>P0_10 <1=>P2_8
//   <i> UART2 Serial Output pin
#define RTE_UART2_TX_ID                 0
#if    (RTE_UART2_TX_ID == 0)
#define RTE_UART2_TX_PORT               0
#define RTE_UART2_TX_BIT                10
#define RTE_UART2_TX_FUNC               1
#elif  (RTE_UART2_TX_ID == 1)
#define RTE_UART2_TX_PORT               2
#define RTE_UART2_TX_BIT                8
#define RTE_UART2_TX_FUNC               2
#else
#error "Invalid UART2_TX Pin Configuration!"
#endif

//   <o> UART2_RX Pin <0=>P0_11 <1=>P2_9
//   <i> UART2 Serial Input pin
#define RTE_UART2_RX_ID                 0
#if    (RTE_UART2_RX_ID == 0)
#define RTE_UART2_RX_PORT               0
#define RTE_UART2_RX_BIT                11
#define RTE_UART2_RX_FUNC               1
#elif  (RTE_UART2_RX_ID == 1)
#define RTE_UART2_RX_PORT               2
#define RTE_UART2_RX_BIT                9
#define RTE_UART2_RX_FUNC               2
#else
#error "Invalid UART2_RX Pin Configuration!"
#endif

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART2_DMA_TX_EN           1
#define   RTE_UART2_DMA_TX_CH           0
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART2_DMA_RX_EN           1
#define   RTE_UART2_DMA_RX_CH           1
//   </h> DMA

//     </e>

// <e> UART3 (Universal asynchronous receiver transmitter)
#define RTE_UART3                       0

//   <o> UART3_TX Pin <0=>P0_0 <1=>P0_25 <2=>P4_28
//   <i> UART3 Serial Output pin
#define RTE_UART3_TX_ID                 0
#if    (RTE_UART3_TX_ID == 0)
#define RTE_UART3_TX_PORT               0
#define RTE_UART3_TX_BIT                0
#define RTE_UART3_TX_FUNC               2
#elif  (RTE_UART3_TX_ID == 1)
#define RTE_UART3_TX_PORT               0
#define RTE_UART3_TX_BIT                25
#define RTE_UART3_TX_FUNC               3
#elif  (RTE_UART3_TX_ID == 2)
#define RTE_UART3_TX_PORT               4
#define RTE_UART3_TX_BIT                28
#define RTE_UART3_TX_FUNC               3
#else
#error "Invalid UART3_TX Pin Configuration!"
#endif

//   <o> UART3_RX Pin <0=>P0_1 <1=>P0_26 <2=>P4_29
//   <i> UART3 Serial Input pin
#define RTE_UART3_RX_ID                 0
#if    (RTE_UART3_RX_ID == 0)
#define RTE_UART3_RX_PORT               0
#define RTE_UART3_RX_BIT                1
#define RTE_UART3_RX_FUNC               2
#elif  (RTE_UART3_RX_ID == 1)
#define RTE_UART3_RX_PORT               0
#define RTE_UART3_RX_BIT                26
#define RTE_UART3_RX_FUNC               3
#elif  (RTE_UART3_RX_ID == 2)
#define RTE_UART3_RX_PORT               4
#define RTE_UART3_RX_BIT                29
#define RTE_UART3_RX_FUNC               3
#else
#error "Invalid UART3_RX Pin Configuration!"
#endif

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART3_DMA_TX_EN           1
#define   RTE_UART3_DMA_TX_CH           0
//     <e> Rx
//       <o1> Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_UART3_DMA_RX_EN           1
#define   RTE_UART3_DMA_RX_CH           1
//   </h> DMA

// </e>

// <e> CAN1 Controller [Driver_CAN1]
// <i> Configuration settings for Driver_CAN1 in component ::Drivers:CAN
#define   RTE_CAN_CAN1                  0

//   <h> Pin Configuration
//     <o> CAN1_RD <0=>Not used <1=>P0_0 <2=>P0_21
//     <i> CAN1 receiver input.
#define   RTE_CAN1_RD_ID                0
#if      (RTE_CAN1_RD_ID == 0)
  #define RTE_CAN1_RD_PIN_EN            0
#elif    (RTE_CAN1_RD_ID == 1)
  #define RTE_CAN1_RD_PORT              0
  #define RTE_CAN1_RD_BIT               0
  #define RTE_CAN1_RD_FUNC              1
#elif    (RTE_CAN1_RD_ID == 2)
  #define RTE_CAN1_RD_PORT              0
  #define RTE_CAN1_RD_BIT               21
  #define RTE_CAN1_RD_FUNC              3
#else
  #error "Invalid RTE_CAN1_RD Pin Configuration!"
#endif
#ifndef   RTE_CAN1_RD_PIN_EN
  #define RTE_CAN1_RD_PIN_EN            1
#endif
//     <o> CAN1_TD <0=>Not used <1=>P0_1 <2=>P0_22
//     <i> CAN1 transmitter output.
#define   RTE_CAN1_TD_ID                0
#if      (RTE_CAN1_TD_ID == 0)
  #define RTE_CAN1_TD_PIN_EN            0
#elif    (RTE_CAN1_TD_ID == 1)
  #define RTE_CAN1_TD_PORT              0
  #define RTE_CAN1_TD_BIT               1
  #define RTE_CAN1_TD_FUNC              1
#elif    (RTE_CAN1_TD_ID == 2)
  #define RTE_CAN1_TD_PORT              0
  #define RTE_CAN1_TD_BIT               22
  #define RTE_CAN1_TD_FUNC              3
#else
  #error "Invalid RTE_CAN1_TD Pin Configuration!"
#endif
#ifndef   RTE_CAN1_TD_PIN_EN
  #define RTE_CAN1_TD_PIN_EN            1
#endif
//   </h> Pin Configuration
// </e> CAN1 Controller [Driver_CAN1]

// <e> CAN2 Controller [Driver_CAN2]
// <i> Configuration settings for Driver_CAN2 in component ::Drivers:CAN
#define   RTE_CAN_CAN2                  0

//   <h> Pin Configuration
//     <o> CAN2_RD <0=>Not used <1=>P0_4 <2=>P2_7
//     <i> CAN2 receiver input.
#define   RTE_CAN2_RD_ID                0
#if      (RTE_CAN2_RD_ID == 0)
  #define RTE_CAN2_RD_PIN_EN            0
#elif    (RTE_CAN2_RD_ID == 1)
  #define RTE_CAN2_RD_PORT              0
  #define RTE_CAN2_RD_BIT               4
  #define RTE_CAN2_RD_FUNC              2
#elif    (RTE_CAN2_RD_ID == 2)
  #define RTE_CAN2_RD_PORT              2
  #define RTE_CAN2_RD_BIT               7
  #define RTE_CAN2_RD_FUNC              1
#else
  #error "Invalid RTE_CAN2_RD Pin Configuration!"
#endif
#ifndef   RTE_CAN2_RD_PIN_EN
  #define RTE_CAN2_RD_PIN_EN            1
#endif
//     <o> CAN2_TD <0=>Not used <1=>P0_5 <2=>P2_8
//     <i> CAN2 transmitter output.
#define   RTE_CAN2_TD_ID                0
#if      (RTE_CAN2_TD_ID == 0)
  #define RTE_CAN2_TD_PIN_EN            0
#elif    (RTE_CAN2_TD_ID == 1)
  #define RTE_CAN2_TD_PORT              0
  #define RTE_CAN2_TD_BIT               5
  #define RTE_CAN2_TD_FUNC              2
#elif    (RTE_CAN2_TD_ID == 2)
  #define RTE_CAN2_TD_PORT              2
  #define RTE_CAN2_TD_BIT               8
  #define RTE_CAN2_TD_FUNC              1
#else
  #error "Invalid RTE_CAN2_TD Pin Configuration!"
#endif
#ifndef   RTE_CAN2_TD_PIN_EN
  #define RTE_CAN2_TD_PIN_EN            1
#endif
//   </h> Pin Configuration
// </e> CAN2 Controller [Driver_CAN2]


// <e> SSP0 (Synchronous Serial Port 0) [Driver_SPI0]
// <i> Configuration settings for Driver_SPI0 in component ::Drivers:SPI
#define RTE_SSP0                        0

//   <h> Pin Configuration
//     <o> SSP0_SSEL <0=>Not used <1=>P0_16 <2=>P1_21
//     <i> Slave Select for SSP0
#define   RTE_SSP0_SSEL_PIN_SEL         1
#if      (RTE_SSP0_SSEL_PIN_SEL == 0)
#define   RTE_SSP0_SSEL_PIN_EN          0
#elif    (RTE_SSP0_SSEL_PIN_SEL == 1)
  #define RTE_SSP0_SSEL_PORT            0
  #define RTE_SSP0_SSEL_BIT             16
  #define RTE_SSP0_SSEL_FUNC            2
#elif    (RTE_SSP0_SSEL_PIN_SEL == 2)
  #define RTE_SSP0_SSEL_PORT            1
  #define RTE_SSP0_SSEL_BIT             21
  #define RTE_SSP0_SSEL_FUNC            3
#else
  #error "Invalid SSP0 SSP0_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP0_SSEL_PIN_EN
#define   RTE_SSP0_SSEL_PIN_EN          1
#endif

//     <o> SSP0_SCK <0=>P0_15 <1=>P1_20
//     <i> Serial clock for SSP0
#define   RTE_SSP0_SCK_PIN_SEL          0
#if      (RTE_SSP0_SCK_PIN_SEL == 0)
  #define RTE_SSP0_SCK_PORT             0
  #define RTE_SSP0_SCK_BIT              15
  #define RTE_SSP0_SCK_FUNC             2
#elif    (RTE_SSP0_SCK_PIN_SEL == 1)
  #define RTE_SSP0_SCK_PORT             1
  #define RTE_SSP0_SCK_BIT              20
  #define RTE_SSP0_SCK_FUNC             3
#else
  #error "Invalid SSP0 SSP0_SCK Pin Configuration!"
#endif

//     <o> SSP0_MISO <0=>P0_17 <1=>P1_23
//     <i> Master In Slave Out for SSP0
#define   RTE_SSP0_MISO_PIN_SEL         0
#if      (RTE_SSP0_MISO_PIN_SEL == 0)
  #define RTE_SSP0_MISO_PORT            0
  #define RTE_SSP0_MISO_BIT             17
  #define RTE_SSP0_MISO_FUNC            2
#elif    (RTE_SSP0_MISO_PIN_SEL == 1)
  #define RTE_SSP0_MISO_PORT            1
  #define RTE_SSP0_MISO_BIT             23
  #define RTE_SSP0_MISO_FUNC            3
#else
  #error "Invalid SSP0 SSP0_MISO Pin Configuration!"
#endif

//     <o> SSP0_MOSI <0=>P0_18 <1=>P1_24
//     <i> Master Out Slave In for SSP0
#define   RTE_SSP0_MOSI_PIN_SEL         0
#if      (RTE_SSP0_MOSI_PIN_SEL == 0)
  #define RTE_SSP0_MOSI_PORT            0
  #define RTE_SSP0_MOSI_BIT             18
  #define RTE_SSP0_MOSI_FUNC            2
#elif    (RTE_SSP0_MOSI_PIN_SEL == 1)
  #define RTE_SSP0_MOSI_PORT            1
  #define RTE_SSP0_MOSI_BIT             24
  #define RTE_SSP0_MOSI_FUNC            3
#else
  #error "Invalid SSP0 SSP0_MOSI Pin Configuration!"
#endif

//   </h>
//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
// </e>
#define   RTE_SSP0_DMA_TX_EN            0
#define   RTE_SSP0_DMA_TX_CH            0
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_SSP0_DMA_RX_EN            0
#define   RTE_SSP0_DMA_RX_CH            1
//   </h> DMA
// </e>

// <e> SSP1 (Synchronous Serial Port 1) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::Drivers:SPI
#define RTE_SSP1                        0

//   <h> Pin Configuration
//     <o> SSP1_SSEL <0=>Not used <1=>P0_6
//     <i> Slave Select for SSP1
#define   RTE_SSP1_SSEL_PIN_SEL         1
#if      (RTE_SSP1_SSEL_PIN_SEL == 0)
  #define RTE_SSP1_SSEL_PIN_EN          0
#elif    (RTE_SSP1_SSEL_PIN_SEL == 1)
  #define RTE_SSP1_SSEL_PORT            0
  #define RTE_SSP1_SSEL_BIT             6
  #define RTE_SSP1_SSEL_FUNC            2
#else
  #error "Invalid SSP1 SSP1_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SSP1_SSEL_PIN_EN
#define   RTE_SSP1_SSEL_PIN_EN          1
#endif

//     <o> SSP1_SCK <0=>P0_7 <1=>P1_31
//     <i> Serial clock for SSP1
#define   RTE_SSP1_SCK_PIN_SEL          0
#if      (RTE_SSP1_SCK_PIN_SEL == 0)
  #define RTE_SSP1_SCK_PORT             0
  #define RTE_SSP1_SCK_BIT              7
  #define RTE_SSP1_SCK_FUNC             2
#elif    (RTE_SSP1_SCK_PIN_SEL == 1)
  #define RTE_SSP1_SCK_PORT             1
  #define RTE_SSP1_SCK_BIT              31
  #define RTE_SSP1_SCK_FUNC             2
#else
  #error "Invalid SSP1 SSP1_SCK Pin Configuration!"
#endif

//     <o> SSP1_MISO <0=>P0_8
//     <i> Master In Slave Out for SSP1
#define   RTE_SSP1_MISO_PIN_SEL         0
#if      (RTE_SSP1_MISO_PIN_SEL == 0)
  #define RTE_SSP1_MISO_PORT            0
  #define RTE_SSP1_MISO_BIT             8
  #define RTE_SSP1_MISO_FUNC            2
#else
  #error "Invalid SSP1 SSP1_MISO Pin Configuration!"
#endif

//     <o> SSP1_MOSI <0=>P0_9
//     <i> Master Out Slave In for SSP0
#define   RTE_SSP1_MOSI_PIN_SEL         0
#if      (RTE_SSP1_MOSI_PIN_SEL == 0)
  #define RTE_SSP1_MOSI_PORT            0
  #define RTE_SSP1_MOSI_BIT             9
  #define RTE_SSP1_MOSI_FUNC            2
#else
  #error "Invalid SSP1 SSP1_MOSI Pin Configuration!"
#endif

//   </h>
//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_SSP1_DMA_TX_EN            0
#define   RTE_SSP1_DMA_TX_CH            2
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//     </e>
#define   RTE_SSP1_DMA_RX_EN            0
#define   RTE_SSP1_DMA_RX_CH            3
//   </h> DMA
// </e>


// <e> SPI (Serial Peripheral Interface) [Driver_SPI2]
// <i> Configuration settings for Driver_SPI2 in component ::Drivers:SPI
#define   RTE_SPI                       0

//   <h> Pin Configuration
//     <o> SPI_SSEL <0=>Not used <1=>P0_16
//     <i> Slave Select for SPI
#define   RTE_SPI_SSEL_PIN_SEL          0
#if      (RTE_SPI_SSEL_PIN_SEL == 0)
#define   RTE_SPI_SSEL_PIN_EN           0
#elif    (RTE_SPI_SSEL_PIN_SEL == 1)
  #define RTE_SPI_SSEL_PORT             0
  #define RTE_SPI_SSEL_BIT              16
  #define RTE_SPI_SSEL_FUNC             3
#else
  #error "Invalid SPI SPI_SSEL Pin Configuration!"
#endif
#ifndef   RTE_SPI_SSEL_PIN_EN
#define   RTE_SPI_SSEL_PIN_EN           1
#endif
//     <o> SPI_SCK <0=>P0_15
//     <i> Serial clock for SPI
#define   RTE_SPI_SCK_PIN_SEL           0
#if      (RTE_SPI_SCK_PIN_SEL == 0)
  #define RTE_SPI_SCK_PORT              0
  #define RTE_SPI_SCK_BIT               15
  #define RTE_SPI_SCK_FUNC              3
#else
  #error "Invalid SPI SPI_SCK Pin Configuration!"
#endif
//     <o> SPI_MISO <0=>P0_17
//     <i> Master In Slave Out for SPI
#define   RTE_SPI_MISO_PIN_SEL          0
#if      (RTE_SPI_MISO_PIN_SEL == 0)
  #define RTE_SPI_MISO_PORT             0
  #define RTE_SPI_MISO_BIT              17
  #define RTE_SPI_MISO_FUNC             3
#else
  #error "Invalid SPI SPI_MISO Pin Configuration!"
#endif
//     <o> SPI_MOSI <0=>P0_18
//     <i> Master Out Slave In for SPI
#define   RTE_SPI_MOSI_PIN_SEL          0
#if      (RTE_SPI_MOSI_PIN_SEL == 0)
  #define RTE_SPI_MOSI_PORT             0
  #define RTE_SPI_MOSI_BIT              18
  #define RTE_SPI_MOSI_FUNC             3
#else
  #error "Invalid SPI SPI_MOSI Pin Configuration!"
#endif
//   </h> Pin Configuration
// </e> SPI (Serial Peripheral Interface) [Driver_SPI2]


// <e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]
// <i> Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define   RTE_I2S0                      0

//   <h> Pin Configuration
//     <o> I2S0_RX_SCK <0=>Not used <1=>P0_4 <2=>P0_23
//     <i> Receive clock for I2S0
#define   RTE_I2S0_RX_SCK_PIN_SEL       1
#if      (RTE_I2S0_RX_SCK_PIN_SEL == 0)
#define   RTE_I2S0_RX_SCK_PIN_EN        0
#elif    (RTE_I2S0_RX_SCK_PIN_SEL == 1)
  #define RTE_I2S0_RX_SCK_PORT          0
  #define RTE_I2S0_RX_SCK_BIT           4
  #define RTE_I2S0_RX_SCK_FUNC          1
#elif    (RTE_I2S0_RX_SCK_PIN_SEL == 2)
  #define RTE_I2S0_RX_SCK_PORT          0
  #define RTE_I2S0_RX_SCK_BIT           23
  #define RTE_I2S0_RX_SCK_FUNC          2
#else
  #error "Invalid I2S0 I2S0_RX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_SCK_PIN_EN
#define   RTE_I2S0_RX_SCK_PIN_EN        1
#endif
//     <o> I2S0_RX_WS <0=>Not used <1=>P0_5 <2=>P0_24
//     <i> Receive word select for I2S0
#define   RTE_I2S0_RX_WS_PIN_SEL        1
#if      (RTE_I2S0_RX_WS_PIN_SEL == 0)
#define   RTE_I2S0_RX_WS_PIN_EN         0
#elif    (RTE_I2S0_RX_WS_PIN_SEL == 1)
  #define RTE_I2S0_RX_WS_PORT           0
  #define RTE_I2S0_RX_WS_BIT            5
  #define RTE_I2S0_RX_WS_FUNC           1
#elif    (RTE_I2S0_RX_WS_PIN_SEL == 2)
  #define RTE_I2S0_RX_WS_PORT           0
  #define RTE_I2S0_RX_WS_BIT            24
  #define RTE_I2S0_RX_WS_FUNC           2
#else
  #error "Invalid I2S0 I2S0_RX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_WS_PIN_EN
#define   RTE_I2S0_RX_WS_PIN_EN         1
#endif
//     <o> I2S0_RX_SDA <0=>Not used <1=>P0_6 <2=>P0_25
//     <i> Receive master clock for I2S0
#define   RTE_I2S0_RX_SDA_PIN_SEL       1
#if      (RTE_I2S0_RX_SDA_PIN_SEL == 0)
#define   RTE_I2S0_RX_SDA_PIN_EN        0
#elif    (RTE_I2S0_RX_SDA_PIN_SEL == 1)
  #define RTE_I2S0_RX_SDA_PORT          0
  #define RTE_I2S0_RX_SDA_BIT           6
  #define RTE_I2S0_RX_SDA_FUNC          1
#elif    (RTE_I2S0_RX_SDA_PIN_SEL == 2)
  #define RTE_I2S0_RX_SDA_PORT          0
  #define RTE_I2S0_RX_SDA_BIT           25
  #define RTE_I2S0_RX_SDA_FUNC          2
#else
  #error "Invalid I2S0 I2S0_RX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_SDA_PIN_EN
#define   RTE_I2S0_RX_SDA_PIN_EN       1
#endif
//     <o> I2S0_RX_MCLK <0=>Not used <1=>P4_28
//     <i> Receive master clock for I2S0
#define   RTE_I2S0_RX_MCLK_PIN_SEL      0
#if      (RTE_I2S0_RX_MCLK_PIN_SEL == 0)
#define   RTE_I2S0_RX_MCLK_PIN_EN       0
#elif    (RTE_I2S0_RX_MCLK_PIN_SEL == 1)
  #define RTE_I2S0_RX_MCLK_PORT         4
  #define RTE_I2S0_RX_MCLK_BIT          28
  #define RTE_I2S0_RX_MCLK_FUNC         1
#else
  #error "Invalid I2S0 I2S0_RX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_RX_MCLK_PIN_EN
#define   RTE_I2S0_RX_MCLK_PIN_EN       1
#endif
//     <o> I2S0_TX_SCK <0=>Not used <1=>P0_7 <2=>P2_11
//     <i> Transmit clock for I2S0
#define   RTE_I2S0_TX_SCK_PIN_SEL       1
#if      (RTE_I2S0_TX_SCK_PIN_SEL == 0)
#define   RTE_I2S0_TX_SCK_PIN_EN        0
#elif    (RTE_I2S0_TX_SCK_PIN_SEL == 1)
  #define RTE_I2S0_TX_SCK_PORT          0
  #define RTE_I2S0_TX_SCK_BIT           7
  #define RTE_I2S0_TX_SCK_FUNC          1
#elif    (RTE_I2S0_TX_SCK_PIN_SEL == 2)
  #define RTE_I2S0_TX_SCK_PORT          2
  #define RTE_I2S0_TX_SCK_BIT           11
  #define RTE_I2S0_TX_SCK_FUNC          3
#else
  #error "Invalid I2S0 I2S0_TX_SCK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_SCK_PIN_EN
#define   RTE_I2S0_TX_SCK_PIN_EN        1
#endif
//     <o> I2S0_TX_WS <0=>Not used <1=>P0_8 <2=>P2_12
//     <i> Transmit word select for I2S0
#define   RTE_I2S0_TX_WS_PIN_SEL        1
#if      (RTE_I2S0_TX_WS_PIN_SEL == 0)
#define   RTE_I2S0_TX_WS_PIN_EN         0
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 1)
  #define RTE_I2S0_TX_WS_PORT           0
  #define RTE_I2S0_TX_WS_BIT            8
  #define RTE_I2S0_TX_WS_FUNC           1
#elif    (RTE_I2S0_TX_WS_PIN_SEL == 2)
  #define RTE_I2S0_TX_WS_PORT           2
  #define RTE_I2S0_TX_WS_BIT            12
  #define RTE_I2S0_TX_WS_FUNC           3
#else
  #error "Invalid I2S0 I2S0_TX_WS Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_WS_PIN_EN
#define   RTE_I2S0_TX_WS_PIN_EN         1
#endif
//     <o> I2S0_TX_SDA <0=>Not used <1=>P0_9 <2=>P2_13
//     <i> Transmit data for I2S0
#define   RTE_I2S0_TX_SDA_PIN_SEL       1
#if      (RTE_I2S0_TX_SDA_PIN_SEL == 0)
#define   RTE_I2S0_TX_SDA_PIN_EN        0
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 1)
  #define RTE_I2S0_TX_SDA_PORT          0
  #define RTE_I2S0_TX_SDA_BIT           9
  #define RTE_I2S0_TX_SDA_FUNC          1
#elif    (RTE_I2S0_TX_SDA_PIN_SEL == 2)
  #define RTE_I2S0_TX_SDA_PORT          2
  #define RTE_I2S0_TX_SDA_BIT           13
  #define RTE_I2S0_TX_SDA_FUNC          3
#else
  #error "Invalid I2S0 I2S0_TX_SDA Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_SDA_PIN_EN
#define   RTE_I2S0_TX_SDA_PIN_EN        1
#endif
//     <o> I2S0_TX_MCLK <0=>Not used <1=>P4_29
//     <i> Transmit master clock for I2S0
#define   RTE_I2S0_TX_MCLK_PIN_SEL      1
#if      (RTE_I2S0_TX_MCLK_PIN_SEL == 0)
#define   RTE_I2S0_TX_MCLK_PIN_EN       0
#elif    (RTE_I2S0_TX_MCLK_PIN_SEL == 1)
  #define RTE_I2S0_TX_MCLK_PORT         4
  #define RTE_I2S0_TX_MCLK_BIT          29
  #define RTE_I2S0_TX_MCLK_FUNC         1
#else
  #error "Invalid I2S0 I2S0_TX_MCLK Pin Configuration!"
#endif
#ifndef   RTE_I2S0_TX_MCLK_PIN_EN
#define   RTE_I2S0_TX_MCLK_PIN_EN       1
#endif
//   </h> Pin Configuration

//   <h> DMA
//     <e> Tx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>9 (DMAMUXPER9)
//     </e>
#define   RTE_I2S0_DMA_TX_EN            1
#define   RTE_I2S0_DMA_TX_CH            0
//     <e> Rx
//       <o1> Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
//       <o2> Peripheral  <0=>10 (DMAMUXPER10)
//     </e>
#define   RTE_I2S0_DMA_RX_EN            1
#define   RTE_I2S0_DMA_RX_CH            1
//   </h> DMA
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]

#endif  /* __RTE_DEVICE_H */
