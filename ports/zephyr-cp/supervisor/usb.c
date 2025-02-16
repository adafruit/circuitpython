#include "supervisor/usb.h"

#include "tusb_option.h"

#if CFG_TUSB_MCU == OPT_MCU_STM32U5
#include <stm32_ll_pwr.h>
#endif

#if CFG_TUSB_MCU == OPT_MCU_NRF5X
#include <zephyr/dt-bindings/regulator/nrf5x.h>
#include <nrfx_power.h>
#endif

#include <zephyr/devicetree.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/irq.h>
#include <zephyr/kernel.h>

#if DT_HAS_COMPAT_STATUS_OKAY(st_stm32_otghs)
#define UDC_IRQ_NAME     otghs
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_otgfs)
#define UDC_IRQ_NAME     otgfs
#elif DT_HAS_COMPAT_STATUS_OKAY(st_stm32_usb)
#define UDC_IRQ_NAME     usb
#elif DT_HAS_COMPAT_STATUS_OKAY(renesas_ra_usb)
#define UDC_IRQ_NAME     usbhs_ir
#endif

#if DT_HAS_COMPAT_STATUS_OKAY(renesas_ra_usb)
#define USB_NAME usbhs
#else
#define USB_NAME zephyr_udc0
#endif

#define USB_DEVICE DT_NODELABEL(USB_NAME)

#ifdef UDC_IRQ_NAME
#define UDC_IRQ       DT_IRQ_BY_NAME(USB_DEVICE, UDC_IRQ_NAME, irq)
#define UDC_IRQ_PRI   DT_IRQ_BY_NAME(USB_DEVICE, UDC_IRQ_NAME, priority)
#else
#define UDC_IRQ       DT_IRQ(USB_DEVICE, irq)
#define UDC_IRQ_PRI   DT_IRQ(USB_DEVICE, priority)
#endif

PINCTRL_DT_DEFINE(USB_DEVICE);
static const struct pinctrl_dev_config *usb_pcfg =
    PINCTRL_DT_DEV_CONFIG_GET(USB_DEVICE);

#if CFG_TUSB_MCU == OPT_MCU_NRF5X
// Value is chosen to be as same as NRFX_POWER_USB_EVT_* in nrfx_power.h
enum {
    USB_EVT_DETECTED = 0,
    USB_EVT_REMOVED = 1,
    USB_EVT_READY = 2
};

#ifdef NRF5340_XXAA
  #define LFCLK_SRC_RC CLOCK_LFCLKSRC_SRC_LFRC
  #define VBUSDETECT_Msk USBREG_USBREGSTATUS_VBUSDETECT_Msk
  #define OUTPUTRDY_Msk USBREG_USBREGSTATUS_OUTPUTRDY_Msk
  #define GPIOTE_IRQn GPIOTE1_IRQn
#else
  #define LFCLK_SRC_RC CLOCK_LFCLKSRC_SRC_RC
  #define VBUSDETECT_Msk POWER_USBREGSTATUS_VBUSDETECT_Msk
  #define OUTPUTRDY_Msk POWER_USBREGSTATUS_OUTPUTRDY_Msk
#endif

// tinyusb function that handles power event (detected, ready, removed)
// We must call it within SD's SOC event handler, or set it as power event handler if SD is not enabled.
extern void tusb_hal_nrf_power_event(uint32_t event);

// nrf power callback, could be unused if SD is enabled or usb is disabled (board_test example)
TU_ATTR_UNUSED static void power_event_handler(nrfx_power_usb_evt_t event) {
    tusb_hal_nrf_power_event((uint32_t)event);
}
#endif

void init_usb_hardware(void) {
    #if CFG_TUSB_MCU == OPT_MCU_RAXXX
    #if !USBHS_PHY_CLOCK_SOURCE_IS_XTAL
    if (data->udc_cfg.usb_speed == USBD_SPEED_HS) {
        LOG_ERR("High-speed operation is not supported in case PHY clock source is not "
            "XTAL");
        return;
    }
    #endif

    R_ICU->IELSR[UDC_IRQ] = ELC_EVENT_USBHS_USB_INT_RESUME;
    #endif


    IRQ_CONNECT(UDC_IRQ, UDC_IRQ_PRI, usb_irq_handler, 0, 0);

    /* Configure USB GPIOs */
    int err = pinctrl_apply_state(usb_pcfg, PINCTRL_STATE_DEFAULT);
    if (err < 0) {
        printk("USB pinctrl setup failed (%d)\n", err);
    } else {
        printk("USB pins setup\n");
    }

// #ifdef USB_DRD_FS
//   // STM32U535/STM32U545

//   /* Enable USB power on Pwrctrl CR2 register */
//   HAL_PWREx_EnableVddUSB();

//   /* USB clock enable */
//   __HAL_RCC_USB_FS_CLK_ENABLE();

// #endif

    #if CFG_TUSB_MCU == OPT_MCU_STM32U5 && defined(USB_OTG_FS)
    /* Enable USB power on Pwrctrl CR2 register */
    // HAL_PWREx_EnableVddUSB();
    LL_PWR_EnableVddUSB();

    /* USB clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    #endif

// #ifdef USB_OTG_HS
//   // STM59x/Ax/Fx/Gx only have 1 USB HS port

//   #if CFG_TUSB_OS == OPT_OS_FREERTOS
//   // If freeRTOS is used, IRQ priority is limit by max syscall ( smaller is higher )
//   NVIC_SetPriority(OTG_HS_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
//   #endif

//   /* USB clock enable */
//   __HAL_RCC_USB_OTG_HS_CLK_ENABLE();
//   __HAL_RCC_USBPHYC_CLK_ENABLE();

//   /* Enable USB power on Pwrctrl CR2 register */
//   HAL_PWREx_EnableVddUSB();
//   HAL_PWREx_EnableUSBHSTranceiverSupply();

//   /*Configuring the SYSCFG registers OTG_HS PHY*/
//   HAL_SYSCFG_EnableOTGPHY(SYSCFG_OTG_HS_PHY_ENABLE);

//   // Disable VBUS sense (B device)
//   USB_OTG_HS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

//   // B-peripheral session valid override enable
//   USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBVALEXTOEN;
//   USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBVALOVAL;
// #endif // USB_OTG_FS


    #if CFG_TUSB_MCU == OPT_MCU_NRF5X
    #ifdef CONFIG_HAS_HW_NRF_USBREG
    /* Use CLOCK/POWER priority for compatibility with other series where
     * USB events are handled by CLOCK interrupt handler.
     */
    IRQ_CONNECT(USBREGULATOR_IRQn,
        DT_IRQ(DT_INST(0, nordic_nrf_clock), priority),
        nrfx_isr, nrfx_usbreg_irq_handler, 0);
    irq_enable(USBREGULATOR_IRQn);
    #endif
    // USB power may already be ready at this time -> no event generated
    // We need to invoke the handler based on the status initially
    uint32_t usb_reg;
    {
        // Power module init
        static const nrfx_power_config_t pwr_cfg = {
            .dcdcen = (DT_PROP(DT_INST(0, nordic_nrf5x_regulator), regulator_initial_mode)
                == NRF5X_REG_MODE_DCDC),
            #if NRFX_POWER_SUPPORTS_DCDCEN_VDDH
            .dcdcenhv = COND_CODE_1(CONFIG_SOC_SERIES_NRF52X,
                (DT_NODE_HAS_STATUS_OKAY(DT_INST(0, nordic_nrf52x_regulator_hv))),
                (DT_NODE_HAS_STATUS_OKAY(DT_INST(0, nordic_nrf53x_regulator_hv)))),
            #endif
        };
        nrfx_power_init(&pwr_cfg);

        // Register tusb function as USB power handler
        // cause cast-function-type warning
        const nrfx_power_usbevt_config_t config = {.handler = power_event_handler};
        nrfx_power_usbevt_init(&config);
        nrfx_power_usbevt_enable();

        // USB power may already be ready at this time -> no event generated
        // We need to invoke the handler based on the status initially
        #ifdef NRF5340_XXAA
        usb_reg = NRF_USBREGULATOR->USBREGSTATUS;
        #else
        usb_reg = NRF_POWER->USBREGSTATUS;
        #endif
    }

    if (usb_reg & VBUSDETECT_Msk) {
        tusb_hal_nrf_power_event(USB_EVT_DETECTED);
    }
    if (usb_reg & OUTPUTRDY_Msk) {
        tusb_hal_nrf_power_event(USB_EVT_READY);
    }

    printk("usb started hopefully\n");
    #endif

}
