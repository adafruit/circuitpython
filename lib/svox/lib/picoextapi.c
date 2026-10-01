/*
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file picoextapi.c
 *
 * API extension for development use
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
#include "picodefs.h"
#include "picoos.h"
#include "picoctrl.h"
#include "picodbg.h"
#include "picoapi.h"
#include "picoextapi.h"
#include "picoapid.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/* this is not used anymore. For the picosh banner, set
 * progv.progVers in picosh.c instead. */
#define PICO_VERSION_INFO  (picoos_char *)"invalid"


extern pico_Status pico_initialize_priv(
        void *memory,
        const pico_Uint32 size,
        pico_Int16 enableMemProt,
        pico_System *system);


/* System initialization and termination functions ****************************/


PICO_FUNC picoext_initialize(
        void *memory,
        const pico_Uint32 size,
        pico_Int16 enableMemProt,
        pico_System *outSystem
        )
{
    return pico_initialize_priv(memory, size, enableMemProt, outSystem);
}


/* System and lingware inspection functions ***********************************/

/* @todo : not supported yet */
PICO_FUNC picoext_getVersionInfo(
        pico_Retstring outInfo,
        const pico_Int16 outInfoMaxLen
        )
{
    if (outInfo == NULL) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    picoos_strlcpy((picoos_char *) outInfo, PICO_VERSION_INFO, outInfoMaxLen);
    return PICO_OK;
}


/* Debugging/testing support functions *****************************************/


PICO_FUNC picoext_setTraceLevel(
        pico_System system,
        pico_Int32 level
        )
{
    if (NULL == system) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    if (level < 0) {
        level = 0;
    }
    if (level > PICODBG_LOG_LEVEL_TRACE) {
        level = PICODBG_LOG_LEVEL_TRACE;
    }
    PICODBG_SET_LOG_LEVEL(level);
    return PICO_OK;
}


PICO_FUNC picoext_setTraceFilterFN(
        pico_System system,
        const pico_Char *name
        )
{

    if (NULL == system) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    name = name;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    PICODBG_SET_LOG_FILTERFN((const char *)name);
    return PICO_OK;
}


PICO_FUNC picoext_setLogFile(
        pico_System system,
        const pico_Char *name
        )
{
    if (NULL == system) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    name = name;        /*PP 13.10.08 : fix warning "var not used in this function"*/
    PICODBG_SET_LOG_FILE((const char *) name);
    return PICO_OK;
}


/* Memory usage ***************************************************************/


pico_Status getMemUsage(
        picoos_Common common,
        picoos_bool resetIncremental,
        picoos_int32 *usedBytes,
        picoos_int32 *incrUsedBytes,
        picoos_int32 *maxUsedBytes
        )
{
    pico_Status status = PICO_OK;

    if (common == NULL) {
        status =  PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_emReset(common->em);
        picoos_getMemUsage(common->mm, resetIncremental, usedBytes, incrUsedBytes, maxUsedBytes);
        status = picoos_emGetExceptionCode(common->em);
    }

    return status;
}


PICO_FUNC picoext_getSystemMemUsage(
        pico_System system,
        pico_Int16 resetIncremental,
        pico_Int32 *outUsedBytes,
        pico_Int32 *outIncrUsedBytes,
        pico_Int32 *outMaxUsedBytes
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if ((outUsedBytes == NULL) || (outIncrUsedBytes == NULL) || (outMaxUsedBytes == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_Common common = pico_sysGetCommon(system);
        status = getMemUsage(common, resetIncremental != 0, outUsedBytes, outIncrUsedBytes, outMaxUsedBytes);
    }

    return status;
}


PICO_FUNC picoext_getEngineMemUsage(
        pico_Engine engine,
        pico_Int16 resetIncremental,
        pico_Int32 *outUsedBytes,
        pico_Int32 *outIncrUsedBytes,
        pico_Int32 *outMaxUsedBytes
        )
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if ((outUsedBytes == NULL) || (outIncrUsedBytes == NULL) || (outMaxUsedBytes == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_Common common = picoctrl_engGetCommon((picoctrl_Engine) engine);
        status = getMemUsage(common, resetIncremental != 0, outUsedBytes, outIncrUsedBytes, outMaxUsedBytes);
    }

    return status;
}

PICO_FUNC picoext_getLastScheduledPU(
        pico_Engine engine
        )
{
    pico_Status status = PICO_OK;
    status = picoctrl_getLastScheduledPU((picoctrl_Engine) engine);
    return status;
}

PICO_FUNC picoext_getLastProducedItemType(
        pico_Engine engine
        )
{
    pico_Status status = PICO_OK;
    status = picoctrl_getLastProducedItemType((picoctrl_Engine) engine);
    return status;
}

#ifdef __cplusplus
}
#endif

/* end */
