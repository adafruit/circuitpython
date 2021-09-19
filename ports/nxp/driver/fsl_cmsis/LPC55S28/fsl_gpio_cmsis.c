/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_gpio_cmsis.h"
#include "fsl_gpio.h"
#include "fsl_pint.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.lpc_gpio_cmsis"
#endif

#if defined(GPIO) && ((defined(RTE_GPIO_PORT0) && RTE_GPIO_PORT0) || (defined(RTE_GPIO_PORT1) && RTE_GPIO_PORT1))

/*!
 * @brief Status of the GPIO inteface
 *
 */
typedef struct _gpio_cmsis_status
{
    bool is_inited;
} gpio_cmsis_status_t;

/*!
 * @brief Type define for the pin state variable
 *
 */
typedef uint32_t gpio_cmsis_pin_state_t;

/*!
 * @brief Map structure between GPIO pin and PINT instance
 *
 */
typedef struct _gpio_cmsis_map
{
    uint32_t pin_index;
    pint_pin_int_t pint_index;
    void (*pinmux_init_func)(void);
    void (*pinmux_deinit_func)(void);
} gpio_cmsis_map_t;

/*!
 * @brief Configuration for CMSIS GPIO Inteface instance
 *
 */
typedef struct _gpio_cmsis_config
{
    GPIO_Type *gpio_base;
    PINT_Type *pint_base;
    uint8_t port_index;
    uint8_t size_of_map;
    uint8_t size_of_context;
} gpio_cmsis_config_t;

/*!
 * @brief Context for GPIO with interrupt enabled
 *
 */
typedef struct _gpio_cmsis_contexts
{
    bool is_occupied;
    uint8_t pin_index;
    pint_pin_int_t pint_index;
    ARM_GPIO_SignalEvent_t callback;
} gpio_cmsis_context_t;

/*!
 * @brief Handle for CMSIS GPIO inteface instance
 *
 */
typedef struct _gpio_cmsis_handle
{
    gpio_cmsis_config_t const *config;
    gpio_cmsis_status_t status;
    gpio_cmsis_pin_state_t interrupt_pins;
    gpio_cmsis_context_t *contexts;
    gpio_cmsis_map_t const *maps;
} gpio_cmsis_handle_t;

/*
 * CMSIS GPIO Version
 */
#define ARM_CMSIS_GPIO_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1U, 0U) /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION s_gpioDriverVersion = {ARM_GPIO_API_VERSION, ARM_CMSIS_GPIO_DRV_VERSION};

/* Driver Capabilities */
static const ARM_GPIO_CAPABILITIES s_gpioDriverCapabilities = {
    1, /* IRQ Supported */
};

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif /* defined(__CC_ARM) || defined(__ARMCC_VERSION) */

static gpio_cmsis_map_t const *CMSIS_GPIO_GetMapResource(gpio_cmsis_handle_t *handle, uint32_t pin) {
    for (uint32_t i = 0; i < handle->config->size_of_map; i++)
    {
        if (pin == handle->maps[i].pin_index) {
            return &(handle->maps[i]);
        }
    }

    return NULL;
}

static gpio_cmsis_context_t *CMSIS_GPIO_GetContextResource(gpio_cmsis_handle_t *handle, uint32_t pintr) {
    for (uint32_t i = 0; i < handle->config->size_of_context; i++)
    {
        gpio_cmsis_context_t *cxt = &(handle->contexts[i]);

        if (cxt->is_occupied == false) {
            continue;
        }

        if ((pintr == (uint32_t)cxt->pint_index)) {
            return cxt;
        }
    }

    return NULL;
}

static gpio_cmsis_context_t *CMSIS_GPIO_GetEmptyContext(gpio_cmsis_handle_t *handle) {
    for (uint32_t i = 0; i < handle->config->size_of_context; i++)
    {
        gpio_cmsis_context_t *cxt = &(handle->contexts[i]);

        if (cxt->is_occupied == false) {
            return cxt;
        }
    }

    return NULL;
}

static pint_pin_enable_t CMSIS_GPIO_MapIRQTypeToPINTType(uint32_t irq_type) {
    pint_pin_enable_t pin_type = kPINT_PinIntEnableNone;

    switch (irq_type)
    {
        case ARM_GPIO_INTERRUPT_DISABLE:
            pin_type = kPINT_PinIntEnableNone;
            break;
        case ARM_GPIO_INTERRUPT_LOGIC_ONE:
            pin_type = kPINT_PinIntEnableHighLevel;
            break;
        case ARM_GPIO_INTERRUPT_LOGIC_ZERO:
            pin_type = kPINT_PinIntEnableLowLevel;
            break;
        case ARM_GPIO_INTERRUPT_RISING_EDGE:
            pin_type = kPINT_PinIntEnableRiseEdge;
            break;
        case ARM_GPIO_INTERRUPT_FALLING_EDGE:
            pin_type = kPINT_PinIntEnableFallEdge;
            break;
        case ARM_GPIO_INTERRUPT_RISING_FALLING_EDGE:
            pin_type = kPINT_PinIntEnableBothEdges;
            break;

        default:
            /* Must be one of the interrupt type above */
            assert(false);
            break;
    }

    return pin_type;
}

static int32_t CMSIS_GPIO_Initialize(gpio_cmsis_handle_t *handle) {
    if (handle->status.is_inited != true) {
        GPIO_PortInit(handle->config->gpio_base, handle->config->port_index);
        handle->status.is_inited = true;
    }

    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_DeinitPin(gpio_cmsis_handle_t *handle, uint32_t pin) {
    /* Do pin MUX configuration if provided pinmux funciton */
    gpio_cmsis_map_t const *map = CMSIS_GPIO_GetMapResource(handle, pin);
    uint32_t mask = 0x01UL << pin;

    if ((map != NULL) && (map->pinmux_deinit_func != NULL)) {
        map->pinmux_deinit_func();
    }

    /* Disable Interrupt if interrupt is configured previously */
    if ((map != NULL) && (((handle->interrupt_pins & mask) != 0U))) {
        PINT_DisableCallbackByIndex(handle->config->pint_base, map->pint_index);
        /* Reset the software state */
        handle->interrupt_pins &= ~mask;
    }

    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_Uninitialize(gpio_cmsis_handle_t *handle) {
    if (handle->status.is_inited == true) {
        for (uint32_t i = 0U; i < 8U * sizeof(gpio_cmsis_pin_state_t); i++)
        {
            (void)CMSIS_GPIO_DeinitPin(handle, i);
        }

        handle->status.is_inited = false;
    }

    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_InitPinAsOutput(gpio_cmsis_handle_t *handle, uint32_t pin, uint32_t output_logic) {
    assert(handle->status.is_inited == true);

    /* Init Pin as output */
    gpio_pin_config_t pin_config = {kGPIO_DigitalOutput, (uint8_t)output_logic};
    GPIO_PinInit(handle->config->gpio_base, handle->config->port_index, pin, &pin_config);

    /* Do pin MUX configuration if provided pinmux funciton */
    gpio_cmsis_map_t const *map = CMSIS_GPIO_GetMapResource(handle, pin);
    if ((map != NULL) && (map->pinmux_init_func != NULL)) {
        map->pinmux_init_func();
    }

    return ARM_DRIVER_OK;
}

static void CMSIS_GPIO_Callback(gpio_cmsis_handle_t *handle, pint_pin_int_t pintr, uint32_t pmatch_status) {
    /* Iterate all registered callback context */
    gpio_cmsis_context_t *cxt = CMSIS_GPIO_GetContextResource(handle, (uint32_t)pintr);

    if ((cxt != NULL) && (cxt->callback != NULL)) {
        cxt->callback(cxt->pin_index); // EVENT
    }
}

static int32_t CMSIS_GPIO_InitPinAsInput(gpio_cmsis_handle_t *handle,
    uint32_t pin,
    uint32_t irq_type,
    ARM_GPIO_SignalEvent_t cb_event,
    pint_cb_t pint_callback) {
    assert(handle->status.is_inited == true);

    gpio_cmsis_map_t const *map = CMSIS_GPIO_GetMapResource(handle, pin);

    /* Configure the interrupt */
    if (irq_type != ARM_GPIO_INTERRUPT_NONE) {
        /* Assert to remind user to provide the map between PORT and interrupt */
        if (map == NULL) {
            assert(false);
            return ARM_DRIVER_ERROR;
        }

        gpio_cmsis_context_t *cxt = CMSIS_GPIO_GetEmptyContext(handle);

        /* Make sure there are still empty slot for this pin interrupt */
        if (cxt == NULL) {
            return ARM_DRIVER_ERROR;
        }

        /* Fill the context data */
        cxt->is_occupied = true;
        cxt->pin_index = (uint8_t)pin;
        cxt->pint_index = map->pint_index;
        cxt->callback = cb_event;

        /* Set the state */
        handle->interrupt_pins |= 0x01UL << pin;

        /* Configure the PINT */
        PINT_PinInterruptConfig(handle->config->pint_base, map->pint_index, CMSIS_GPIO_MapIRQTypeToPINTType(irq_type),
            pint_callback);
        PINT_EnableCallbackByIndex(handle->config->pint_base, map->pint_index);
    }

    /* Do pin MUX configuration if provided pinmux funciton */
    if ((map != NULL) && (map->pinmux_init_func != NULL)) {
        map->pinmux_init_func();
    }

    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_PowerControl(gpio_cmsis_handle_t *handle, ARM_POWER_STATE state) {
    assert(handle->status.is_inited == true);
    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_PinWrite(gpio_cmsis_handle_t *handle, uint32_t pin, uint32_t logic_value) {
    assert(handle->status.is_inited == true);
    GPIO_PinWrite(handle->config->gpio_base, handle->config->port_index, pin, (uint8_t)logic_value);
    return ARM_DRIVER_OK;
}

static bool CMSIS_GPIO_PinRead(gpio_cmsis_handle_t *handle, uint32_t pin) {
    assert(handle->status.is_inited == true);
    return GPIO_PinRead(handle->config->gpio_base, handle->config->port_index, pin) != 0U;
}

static int32_t CMSIS_GPIO_PortToggle(gpio_cmsis_handle_t *handle, uint32_t ored_pins) {
    assert(handle->status.is_inited == true);
    GPIO_PortToggle(handle->config->gpio_base, handle->config->port_index, ored_pins);
    return ARM_DRIVER_OK;
}

static int32_t CMSIS_GPIO_PinToggle(gpio_cmsis_handle_t *handle, uint32_t pin) {
    assert(handle->status.is_inited == true);
    return CMSIS_GPIO_PortToggle(handle, 0x01UL << pin);
}

static int32_t CMSIS_GPIO_PortWrite(gpio_cmsis_handle_t *handle, uint32_t ored_pins, uint32_t logic_value) {
    assert(handle->status.is_inited == true);
    if (logic_value == 0U) {
        GPIO_PortClear(handle->config->gpio_base, handle->config->port_index, ored_pins);
    } else {
        GPIO_PortSet(handle->config->gpio_base, handle->config->port_index, ored_pins);
    }
    return ARM_DRIVER_OK;
}

static uint32_t CMSIS_GPIO_PortRead(gpio_cmsis_handle_t *handle) {
    assert(handle->status.is_inited == true);
    return GPIO_PortRead(handle->config->gpio_base, handle->config->port_index);
}

static int32_t CMSIS_GPIO_Control(gpio_cmsis_handle_t *handle, uint32_t pin, uint32_t control, uint32_t arg) {
    assert(handle->status.is_inited == true);
    int32_t returncode = ARM_DRIVER_OK;

    switch (control)
    {
        case ARM_GPIO_CONTROL_INTERRUPT: {
            gpio_cmsis_map_t const *map = CMSIS_GPIO_GetMapResource(handle, pin);

            if (map == NULL) {
                returncode = ARM_DRIVER_ERROR_PARAMETER;
            } else {
                switch (arg)
                {
                    case ARM_GPIO_INTERRUPT_DISABLE:
                        PINT_DisableCallbackByIndex(handle->config->pint_base, map->pint_index);
                        break;

                    case ARM_GPIO_INTERRUPT_ENABLE:
                        PINT_EnableCallbackByIndex(handle->config->pint_base, map->pint_index);
                        break;

                    default:
                        returncode = ARM_DRIVER_ERROR_PARAMETER;
                        break;
                }
            }
            break;
        }

        default:
            returncode = ARM_DRIVER_ERROR_PARAMETER;
            break;
    }
    return returncode;
}

static ARM_DRIVER_VERSION CMSIS_GPIO_GetVersion(void) {
    return s_gpioDriverVersion;
}

static ARM_GPIO_CAPABILITIES CMSIS_GPIO_GetCapabilities(void) {
    return s_gpioDriverCapabilities;
}

#endif // (RTE_GPIO_PORT0 && defined(GPIO)) || (RTE_GPIO_PORT1 && defined(GPIO))

/******************************* GPIO PORT 0 **********************************************/
#if defined(GPIO) && defined(RTE_GPIO_PORT0) && RTE_GPIO_PORT0
static const gpio_cmsis_map_t s_gpio_port0_cmsis_maps[] = RTE_GPIO_PORT0_MAPS;

static const gpio_cmsis_config_t s_gpio_port0_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 0,
    .size_of_map = RTE_GPIO_PORT0_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT0_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port0_contexts[RTE_GPIO_PORT0_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port0_handle = {
    .config = &s_gpio_port0_cmsis_config,
    .maps = s_gpio_port0_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port0_contexts,
};

static int32_t GPIO_PORT0_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port0_handle);
}

static int32_t GPIO_PORT0_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port0_handle);
}

static int32_t GPIO_PORT0_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port0_handle, pin, output_logic);
}

static void GPIO_PORT0_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port0_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT0_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port0_handle, pin, irq_type, cb_event, GPIO_PORT0_Callback);
}

static int32_t GPIO_PORT0_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port0_handle, state);
}

static int32_t GPIO_PORT0_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port0_handle, pin, logic_value);
}

static int32_t GPIO_PORT0_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port0_handle, pin);
}

static bool GPIO_PORT0_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port0_handle, pin);
}

static int32_t GPIO_PORT0_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port0_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT0_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port0_handle, ored_pins);
}

static uint32_t GPIO_PORT0_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port0_handle);
}

static int32_t GPIO_PORT0_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port0_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT0 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT0_Initialize,
    .Uninitialize = GPIO_PORT0_Uninitialize,
    .InitPinAsOutput = GPIO_PORT0_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT0_InitPinAsInput,
    .PowerControl = GPIO_PORT0_PowerControl,
    .PinWrite = GPIO_PORT0_PinWrite,
    .PinRead = GPIO_PORT0_PinRead,
    .PinToggle = GPIO_PORT0_PinToggle,
    .PortWrite = GPIO_PORT0_PortWrite,
    .PortToggle = GPIO_PORT0_PortToggle,
    .PortRead = GPIO_PORT0_PortRead,
    .Control = GPIO_PORT0_Control,
};
#endif

#if defined(GPIO) && RTE_GPIO_PORT1
static const gpio_cmsis_map_t s_gpio_port1_cmsis_maps[] = RTE_GPIO_PORT1_MAPS;

static const gpio_cmsis_config_t s_gpio_port1_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 1,
    .size_of_map = RTE_GPIO_PORT1_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT1_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port1_contexts[RTE_GPIO_PORT1_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port1_handle = {
    .config = &s_gpio_port1_cmsis_config,
    .maps = s_gpio_port1_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port1_contexts,
};

static int32_t GPIO_PORT1_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port1_handle);
}

static int32_t GPIO_PORT1_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port1_handle);
}

static int32_t GPIO_PORT1_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port1_handle, pin, output_logic);
}

static void GPIO_PORT1_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port1_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT1_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port1_handle, pin, irq_type, cb_event, GPIO_PORT1_Callback);
}

static int32_t GPIO_PORT1_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port1_handle, state);
}

static int32_t GPIO_PORT1_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port1_handle, pin, logic_value);
}

static int32_t GPIO_PORT1_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port1_handle, pin);
}

static bool GPIO_PORT1_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port1_handle, pin);
}

static int32_t GPIO_PORT1_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port1_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT1_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port1_handle, ored_pins);
}

static uint32_t GPIO_PORT1_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port1_handle);
}

static int32_t GPIO_PORT1_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port1_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT1 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT1_Initialize,
    .Uninitialize = GPIO_PORT1_Uninitialize,
    .InitPinAsOutput = GPIO_PORT1_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT1_InitPinAsInput,
    .PowerControl = GPIO_PORT1_PowerControl,
    .PinWrite = GPIO_PORT1_PinWrite,
    .PinRead = GPIO_PORT1_PinRead,
    .PinToggle = GPIO_PORT1_PinToggle,
    .PortWrite = GPIO_PORT1_PortWrite,
    .PortToggle = GPIO_PORT1_PortToggle,
    .PortRead = GPIO_PORT1_PortRead,
    .Control = GPIO_PORT1_Control,
};

#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT2) && RTE_GPIO_PORT2
static const gpio_cmsis_map_t s_gpio_port2_cmsis_maps[] = RTE_GPIO_PORT2_MAPS;

static const gpio_cmsis_config_t s_gpio_port2_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 2,
    .size_of_map = RTE_GPIO_PORT2_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT2_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port2_contexts[RTE_GPIO_PORT2_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port2_handle = {
    .config = &s_gpio_port2_cmsis_config,
    .maps = s_gpio_port2_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port2_contexts,
};

static int32_t GPIO_PORT2_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port2_handle);
}

static int32_t GPIO_PORT2_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port2_handle);
}

static int32_t GPIO_PORT2_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port2_handle, pin, output_logic);
}

static void GPIO_PORT2_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port2_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT2_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port2_handle, pin, irq_type, cb_event, GPIO_PORT2_Callback);
}

static int32_t GPIO_PORT2_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port2_handle, state);
}

static int32_t GPIO_PORT2_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port2_handle, pin, logic_value);
}

static int32_t GPIO_PORT2_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port2_handle, pin);
}

static bool GPIO_PORT2_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port2_handle, pin);
}

static int32_t GPIO_PORT2_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port2_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT2_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port2_handle, ored_pins);
}

static uint32_t GPIO_PORT2_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port2_handle);
}

static int32_t GPIO_PORT2_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port2_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT2 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT2_Initialize,
    .Uninitialize = GPIO_PORT2_Uninitialize,
    .InitPinAsOutput = GPIO_PORT2_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT2_InitPinAsInput,
    .PowerControl = GPIO_PORT2_PowerControl,
    .PinWrite = GPIO_PORT2_PinWrite,
    .PinRead = GPIO_PORT2_PinRead,
    .PinToggle = GPIO_PORT2_PinToggle,
    .PortWrite = GPIO_PORT2_PortWrite,
    .PortToggle = GPIO_PORT2_PortToggle,
    .PortRead = GPIO_PORT2_PortRead,
    .Control = GPIO_PORT2_Control,
};

#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT3) && RTE_GPIO_PORT3
static const gpio_cmsis_map_t s_gpio_port3_cmsis_maps[] = RTE_GPIO_PORT3_MAPS;

static const gpio_cmsis_config_t s_gpio_port3_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 3,
    .size_of_map = RTE_GPIO_PORT3_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT3_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port3_contexts[RTE_GPIO_PORT3_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port3_handle = {
    .config = &s_gpio_port3_cmsis_config,
    .maps = s_gpio_port3_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port3_contexts,
};

static int32_t GPIO_PORT3_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port3_handle);
}

static int32_t GPIO_PORT3_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port3_handle);
}

static int32_t GPIO_PORT3_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port3_handle, pin, output_logic);
}

static void GPIO_PORT3_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port3_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT3_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port3_handle, pin, irq_type, cb_event, GPIO_PORT3_Callback);
}

static int32_t GPIO_PORT3_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port3_handle, state);
}

static int32_t GPIO_PORT3_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port3_handle, pin, logic_value);
}

static int32_t GPIO_PORT3_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port3_handle, pin);
}

static bool GPIO_PORT3_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port3_handle, pin);
}

static int32_t GPIO_PORT3_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port3_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT3_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port3_handle, ored_pins);
}

static uint32_t GPIO_PORT3_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port3_handle);
}

static int32_t GPIO_PORT3_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port3_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT3 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT3_Initialize,
    .Uninitialize = GPIO_PORT3_Uninitialize,
    .InitPinAsOutput = GPIO_PORT3_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT3_InitPinAsInput,
    .PowerControl = GPIO_PORT3_PowerControl,
    .PinWrite = GPIO_PORT3_PinWrite,
    .PinRead = GPIO_PORT3_PinRead,
    .PinToggle = GPIO_PORT3_PinToggle,
    .PortWrite = GPIO_PORT3_PortWrite,
    .PortToggle = GPIO_PORT3_PortToggle,
    .PortRead = GPIO_PORT3_PortRead,
    .Control = GPIO_PORT3_Control,
};

#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT4) && RTE_GPIO_PORT4
static const gpio_cmsis_map_t s_gpio_port4_cmsis_maps[] = RTE_GPIO_PORT4_MAPS;

static const gpio_cmsis_config_t s_gpio_port4_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 4,
    .size_of_map = RTE_GPIO_PORT4_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT4_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port4_contexts[RTE_GPIO_PORT4_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port4_handle = {
    .config = &s_gpio_port4_cmsis_config,
    .maps = s_gpio_port4_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port4_contexts,
};

static int32_t GPIO_PORT4_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port4_handle);
}

static int32_t GPIO_PORT4_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port4_handle);
}

static int32_t GPIO_PORT4_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port4_handle, pin, output_logic);
}

static void GPIO_PORT4_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port4_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT4_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port4_handle, pin, irq_type, cb_event, GPIO_PORT4_Callback);
}

static int32_t GPIO_PORT4_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port4_handle, state);
}

static int32_t GPIO_PORT4_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port4_handle, pin, logic_value);
}

static int32_t GPIO_PORT4_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port4_handle, pin);
}

static bool GPIO_PORT4_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port4_handle, pin);
}

static int32_t GPIO_PORT4_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port4_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT4_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port4_handle, ored_pins);
}

static uint32_t GPIO_PORT4_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port4_handle);
}

static int32_t GPIO_PORT4_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port4_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT4 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT4_Initialize,
    .Uninitialize = GPIO_PORT4_Uninitialize,
    .InitPinAsOutput = GPIO_PORT4_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT4_InitPinAsInput,
    .PowerControl = GPIO_PORT4_PowerControl,
    .PinWrite = GPIO_PORT4_PinWrite,
    .PinRead = GPIO_PORT4_PinRead,
    .PinToggle = GPIO_PORT4_PinToggle,
    .PortWrite = GPIO_PORT4_PortWrite,
    .PortToggle = GPIO_PORT4_PortToggle,
    .PortRead = GPIO_PORT4_PortRead,
    .Control = GPIO_PORT4_Control,
};

#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT5) && RTE_GPIO_PORT5
const gpio_cmsis_map_t s_gpio_port5_cmsis_maps[] = RTE_GPIO_PORT5_MAPS;

const gpio_cmsis_config_t s_gpio_port5_cmsis_config = {
    .gpio_base = GPIO,
    .pint_base = PINT,
    .port_index = 5,
    .size_of_map = RTE_GPIO_PORT5_SIZE_OF_MAP,
    .size_of_context = RTE_GPIO_PORT5_MAX_INTERRUPT_CONTEXTS,
};

static gpio_cmsis_context_t s_gpio_port5_contexts[RTE_GPIO_PORT5_MAX_INTERRUPT_CONTEXTS];

static gpio_cmsis_handle_t s_gpio_port5_handle = {
    .config = &s_gpio_port5_cmsis_config,
    .maps = s_gpio_port5_cmsis_maps,
    .status = {0},
    .interrupt_pins = 0x0U,
    .contexts = s_gpio_port5_contexts,
};

static int32_t GPIO_PORT5_Initialize(void) {
    return CMSIS_GPIO_Initialize(&s_gpio_port5_handle);
}

static int32_t GPIO_PORT5_Uninitialize(void) {
    return CMSIS_GPIO_Uninitialize(&s_gpio_port5_handle);
}

static int32_t GPIO_PORT5_InitPinAsOutput(uint32_t pin, uint32_t output_logic) {
    return CMSIS_GPIO_InitPinAsOutput(&s_gpio_port5_handle, pin, output_logic);
}

static void GPIO_PORT5_Callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
    CMSIS_GPIO_Callback(&s_gpio_port5_handle, pintr, pmatch_status);
}

static int32_t GPIO_PORT5_InitPinAsInput(uint32_t pin, uint32_t irq_type, ARM_GPIO_SignalEvent_t cb_event) {
    return CMSIS_GPIO_InitPinAsInput(&s_gpio_port5_handle, pin, irq_type, cb_event, GPIO_PORT5_Callback);
}

static int32_t GPIO_PORT5_PowerControl(ARM_POWER_STATE state) {
    return CMSIS_GPIO_PowerControl(&s_gpio_port5_handle, state);
}

static int32_t GPIO_PORT5_PinWrite(uint32_t pin, uint32_t logic_value) {
    return CMSIS_GPIO_PinWrite(&s_gpio_port5_handle, pin, logic_value);
}

static int32_t GPIO_PORT5_PinToggle(uint32_t pin) {
    return CMSIS_GPIO_PinToggle(&s_gpio_port5_handle, pin);
}

static bool GPIO_PORT5_PinRead(uint32_t pin) {
    return CMSIS_GPIO_PinRead(&s_gpio_port5_handle, pin);
}

static int32_t GPIO_PORT5_PortWrite(uint32_t ored_pins, uint32_t logic_value) {
    return CMSIS_GPIO_PortWrite(&s_gpio_port5_handle, ored_pins, logic_value);
}

static int32_t GPIO_PORT5_PortToggle(uint32_t ored_pins) {
    return CMSIS_GPIO_PortToggle(&s_gpio_port5_handle, ored_pins);
}

static uint32_t GPIO_PORT5_PortRead(void) {
    return CMSIS_GPIO_PortRead(&s_gpio_port5_handle);
}

static int32_t GPIO_PORT5_Control(uint32_t pin, uint32_t control, uint32_t arg) {
    return CMSIS_GPIO_Control(&s_gpio_port5_handle, pin, control, arg);
}

ARM_DRIVER_GPIO Driver_GPIO_PORT5 = {
    .GetVersion = CMSIS_GPIO_GetVersion,
    .GetCapabilities = CMSIS_GPIO_GetCapabilities,
    .Initialize = GPIO_PORT5_Initialize,
    .Uninitialize = GPIO_PORT5_Uninitialize,
    .InitPinAsOutput = GPIO_PORT5_InitPinAsOutput,
    .InitPinAsInput = GPIO_PORT5_InitPinAsInput,
    .PowerControl = GPIO_PORT5_PowerControl,
    .PinWrite = GPIO_PORT5_PinWrite,
    .PinRead = GPIO_PORT5_PinRead,
    .PinToggle = GPIO_PORT5_PinToggle,
    .PortWrite = GPIO_PORT5_PortWrite,
    .PortToggle = GPIO_PORT5_PortToggle,
    .PortRead = GPIO_PORT5_PortRead,
    .Control = GPIO_PORT5_Control,
};

#endif
