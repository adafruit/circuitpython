/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "py/mpconfig.h"
#include "cmsis5/CMSIS/Driver/Include/Driver_SPI.h"
#include "CMSIS/Driver/Config/RTE_Device.h"

#if (RTE_SSP0)
extern ARM_DRIVER_SPI Driver_SPI0;
#else
#define Driver_SPI0  (spi_inst_t *)(0U)
#endif

#if (RTE_SSP1)
extern ARM_DRIVER_SPI Driver_SPI1;
#else
#define Driver_SPI1  (spi_inst_t *)(0U)
#endif

#if (RTE_SSP2)
extern ARM_DRIVER_SPI Driver_SPI2;
#else
#define Driver_SPI2  (spi_inst_t *)(0U)
#endif


const STATIC spi_inst_t *spi_insts[3U] = {&Driver_SPI0, &Driver_SPI1, &Driver_SPI2};
