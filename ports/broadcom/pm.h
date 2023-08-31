#pragma once
#include "bcm283x.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC                 (PM_BASE + 0x1c)
#define PM_RSTC_WRCFG_CLR       0xffffffcf
#define PM_WDOG                 (PM_BASE + 0x24)
#define PM_WDOG_MASK            0x00000fff
