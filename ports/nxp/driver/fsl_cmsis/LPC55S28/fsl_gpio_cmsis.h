/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_LPC_GPIO_CMSISI_H_
#define _FSL_LPC_GPIO_CMSISI_H_

#include "fsl_common.h"
#include "RTE_Device.h"
#include "Driver_GPIO.h"

#if defined(GPIO) && defined(RTE_GPIO_PORT0) && RTE_GPIO_PORT0
extern ARM_DRIVER_GPIO Driver_GPIO_PORT0;
#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT1) && RTE_GPIO_PORT1
extern ARM_DRIVER_GPIO Driver_GPIO_PORT1;
#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT2) && RTE_GPIO_PORT2
extern ARM_DRIVER_GPIO Driver_GPIO_PORT2;
#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT3) && RTE_GPIO_PORT3
extern ARM_DRIVER_GPIO Driver_GPIO_PORT3;
#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT4) && RTE_GPIO_PORT4
extern ARM_DRIVER_GPIO Driver_GPIO_PORT4;
#endif

#if defined(GPIO) && defined(RTE_GPIO_PORT5) && RTE_GPIO_PORT5
extern ARM_DRIVER_GPIO Driver_GPIO_PORT5;
#endif
#endif
