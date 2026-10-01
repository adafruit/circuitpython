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
 * @file picoapi.c
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 */
#include "picodefs.h"
#include "picoos.h"
#include "picodbg.h"
#include "picorsrc.h"
#include "picoctrl.h"
#include "picoapi.h"
#include "picoapid.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ****************************************************************************/
/* System-level API functions                                                 */
/* ****************************************************************************/

#define MAGIC_MASK 0x5069636F  /* Pico */

#define SET_MAGIC_NUMBER(sys) \
    (sys)->magic = ((picoos_uint32) (uintptr_t) (sys)) ^ MAGIC_MASK

#define CHECK_MAGIC_NUMBER(sys) \
    ((sys)->magic == (((picoos_uint32) (uintptr_t) (sys)) ^ MAGIC_MASK))



/* *** Auxiliary routines (may also be called from picoextapi.c) **************/


int is_valid_system_handle(pico_System system)
{
    return (system != NULL) && CHECK_MAGIC_NUMBER(system);
}


picoos_Common pico_sysGetCommon(pico_System this)
{
    if (this != NULL) {
        return this->common;
    } else {
        return NULL;
    }
}



/* *** System initialization and termination functions ************************/
pico_Status pico_initialize_priv(
        void *memory,
        const pico_Uint32 size,
        pico_Int16 enableMemProt,
        pico_System *system
        )
{
    pico_Status status = PICO_OK;

    PICODBG_INITIALIZE(PICODBG_LOG_LEVEL_INFO);
    PICODBG_ENABLE_COLORS(0);
    /*PICODBG_SET_OUTPUT_FORMAT((PICODBG_SHOW_LEVEL | PICODBG_SHOW_SRCNAME));*/

    if (memory == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (size == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else if (system == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        byte_ptr_t rest_mem;
        picoos_objsize_t rest_mem_size;
        pico_System sys;
        picoos_MemoryManager sysMM;
        picoos_ExceptionManager sysEM;

        sys = (pico_System) picoos_raw_malloc(memory, size, sizeof(pico_system_t),
                &rest_mem, &rest_mem_size);
        if (sys != NULL) {
            sysMM = picoos_newMemoryManager(rest_mem, rest_mem_size, enableMemProt ? TRUE : FALSE);
            if (sysMM != NULL) {
                sysEM = picoos_newExceptionManager(sysMM);
                sys->common = picoos_newCommon(sysMM);
                sys->rm = picorsrc_newResourceManager(sysMM, sys->common);
                if ((sysEM != NULL) && (sys->common != NULL) && (sys->rm != NULL)) {
                    sys->common->em = sysEM;
                    sys->common->mm = sysMM;
                    sys->engine = NULL;

                    picorsrc_createDefaultResource(sys->rm /*,&defaultResource */);

                    SET_MAGIC_NUMBER(sys);
                    status = PICO_OK;
                } else {
                    status = PICO_EXC_OUT_OF_MEM;
                }
            } else {
                status = PICO_EXC_OUT_OF_MEM;
            }
        } else {
            status = PICO_EXC_OUT_OF_MEM;
        }
        *system = sys;
    }

    if (status != PICO_OK) {
        if (system != NULL) {
            *system = NULL;
        }
        PICODBG_TERMINATE();
    }

    return status;
}
/**
 * pico_initialize : initializes the pico system private memory
 * @param    memory : pointer to a free and already allocated memory area
 * @param    size : size of the memory area
 * @param    system : pointer to a pico_System struct
 * @return  PICO_OK : successful init, !PICO_OK : error on allocating private memory
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_initialize(
        void *memory,
        const pico_Uint32 size,
        pico_System *system
        )
{
    return pico_initialize_priv(memory, size, /*enableMemProt*/ FALSE, system);
}

/**
 * pico_terminate : deallocates the pico system private memory
 * @param    system : pointer to a pico_System struct
 * @return  PICO_OK : successful de-init, !PICO_OK : error on de-allocating private memory
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_terminate(
        pico_System *system
        )
{
    pico_Status status = PICO_OK;

    if ((system == NULL) || !is_valid_system_handle(*system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else {
        pico_System sys = *system;

        /* close engine(s) */
        picoctrl_disposeEngine(sys->common->mm, sys->rm, &sys->engine);

        /* close all resources */
        picorsrc_disposeResourceManager(sys->common->mm, &sys->rm);

        sys->magic ^= 0xFFFEFDFC;
        *system = NULL;
    }

    PICODBG_TERMINATE();

    return status;
}



/* *** System status and error/warning message retrieval function *************/

/**
 * pico_getSystemStatusMessage : Returns a description of the system status or errors
 * @param    system : pointer to a pico_System struct
 * @param    errCode : pico_System error code
 * @param    outMessage : memory area where to return a string
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getSystemStatusMessage(
        pico_System system,
        pico_Status errCode,
        pico_Retstring outMessage
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outMessage != NULL) {
            picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "'system' not initialized", PICO_RETSTRINGSIZE);
        }
    } else if (outMessage == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        if (picoos_emGetExceptionCode(system->common->em) == PICO_OK) {
            if (errCode == PICO_OK) {
                picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "system ok", PICO_RETSTRINGSIZE);
            } else {
                /* exceptionManager was not informed yet; produce default message */
                picoos_setErrorMsg((picoos_char *) outMessage, PICO_RETSTRINGSIZE, errCode, NULL, NULL, NULL);
            }
        } else {
            picoos_emGetExceptionMessage(system->common->em, (picoos_char *) outMessage, PICO_RETSTRINGSIZE);
        }
    }

    return status;
}

/**
 * pico_getSystemStatusMessage : Returns the number of warnings
 * @param    system : pointer to a pico_System struct
 * @param    *outNrOfWarnings : pointer to location to receive number of warnings
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getNrSystemWarnings(
        pico_System system,
        pico_Int32 *outNrOfWarnings
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outNrOfWarnings != NULL) {
            *outNrOfWarnings = 0;
        }
    } else if (outNrOfWarnings == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        *outNrOfWarnings = picoos_emGetNumOfWarnings(system->common->em);
    }

    return status;
}

/**
 * pico_getSystemWarning : Returns a description of a warning
 * @param    system : pointer to a pico_System struct
 * @param    warningIndex : warning index
 * @param    *outCode : pointer to receive the warning code
 * @param    outMessage : pointer to receive the output message
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getSystemWarning(
        pico_System system,
        const pico_Int32 warningIndex,
        pico_Status *outCode,
        pico_Retstring outMessage
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outMessage != NULL) {
            picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "'system' not initialized", PICO_RETSTRINGSIZE);
        }
    } else if (warningIndex < 0) {
        status = PICO_ERR_INDEX_OUT_OF_RANGE;
    } else if ((outCode == NULL) || (outMessage == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        *outCode = picoos_emGetWarningCode(system->common->em, warningIndex);
        picoos_emGetWarningMessage(system->common->em, warningIndex, (picoos_char *) outMessage, (picoos_uint16) PICO_RETSTRINGSIZE);
    }

    return status;
}



/* *** Resource loading and unloading functions *******************************/

/**
 * pico_loadResource : Loads a resource file into the Pico system
 * @param    system : pointer to a pico_System struct
 * @param    *lingwareFileName : lingware resource file name
 * @param    *outLingware : pointer to receive the loaded lingware resource memory area address
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_loadResource(
        pico_System system,
        const pico_Char *lingwareFileName,
        pico_Resource *outLingware
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if ((lingwareFileName == NULL) || (outLingware == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        PICODBG_DEBUG(("memory usage before resource loading"));
        picoos_showMemUsage(system->common->mm, FALSE, TRUE);
        picoos_emReset(system->common->em);
        status = picorsrc_loadResource(system->rm, (picoos_char *) lingwareFileName, (picorsrc_Resource *) outLingware);
        PICODBG_DEBUG(("memory used to load resource %s", lingwareFileName));
        picoos_showMemUsage(system->common->mm, TRUE, FALSE);
    }

    return status;
}

/**
 * pico_loadMemoryResource : Adds a preloaded resource into the Pico system
 * @param    system : pointer to a pico_System struct
 * @param    *lingwareFileName : lingware resource file name
 * @param    *outLingware : pointer to receive the loaded lingware resource memory area address
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_loadMemoryResource(
        pico_System system,
        const pico_Char *memoryAddress,
        pico_Resource *outLingware
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if ((memoryAddress == NULL) || (outLingware == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        PICODBG_DEBUG(("memory usage before resource loading"));
        picoos_showMemUsage(system->common->mm, FALSE, TRUE);
        picoos_emReset(system->common->em);
        status = picorsrc_loadMemoryResource(system->rm, (picoos_char *) memoryAddress, (picorsrc_Resource *) outLingware);
        PICODBG_DEBUG(("memory used to load resource %p", memoryAddress));
        picoos_showMemUsage(system->common->mm, TRUE, FALSE);
    }

    return status;
}


/**
 * pico_unloadResource : unLoads a resource file from the Pico system
 * @param    system : pointer to a pico_System struct
 * @param    *inoutLingware : pointer to the loaded lingware resource memory area address
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_unloadResource(
        pico_System system,
        pico_Resource *inoutLingware
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (inoutLingware == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (!picoctrl_isValidResourceHandle(*((picorsrc_Resource *) inoutLingware))) {
        status = PICO_ERR_INVALID_HANDLE;
    } else {
        PICODBG_DEBUG(("memory usage before resource unloading"));
        picoos_showMemUsage(system->common->mm, FALSE, TRUE);
        picoos_emReset(system->common->em);
        status = picorsrc_unloadResource(system->rm, (picorsrc_Resource *) inoutLingware);
        PICODBG_DEBUG(("memory released by resource unloading"));
        picoos_showMemUsage(system->common->mm, TRUE, FALSE);
    }

    return status;
}

/* *** Resource inspection functions *******************************/
/**
 * pico_getResourceName : Gets a resource name
 * @param    system : pointer to a pico_System struct
 * @param    resource : pointer to the loaded resource memory area address
 * @param    outName : pointer to the area to receuive the resource name
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getResourceName(
        pico_System system,
        pico_Resource resource,
        pico_Retstring outName) {

    if (!is_valid_system_handle(system)) {
        return PICO_ERR_INVALID_HANDLE;
    } else if (NULL == outName) {
        return PICO_ERR_NULLPTR_ACCESS;
    }
    return picorsrc_rsrcGetName((picorsrc_Resource)resource, (picoos_char *) outName, PICO_RETSTRINGSIZE);
}


/* *** Voice definition functions *********************************************/

/**
 * pico_createVoiceDefinition : Creates a voice definition
 * @param    system : pointer to a pico_System struct
 * @param    *voiceName : pointer to the area to receive the voice definition
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_createVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (voiceName == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (picoos_strlen((picoos_char *) voiceName) == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else {
        picoos_emReset(system->common->em);
        status = picorsrc_createVoiceDefinition(system->rm, (picoos_char *) voiceName);
    }

    return status;
}

/**
 * pico_addResourceToVoiceDefinition : Adds a mapping pair to a voice definition
 * @param    system : pointer to a pico_System struct
 * @param    *voiceName : pointer to the area containing the voice definition
 * @param    *resourceName : pointer to the area containing the resource name
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_addResourceToVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName,
        const pico_Char *resourceName
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (voiceName == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (picoos_strlen((picoos_char *) voiceName) == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else if (resourceName == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (picoos_strlen((picoos_char *) resourceName) == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else {
        picoos_emReset(system->common->em);
        status = picorsrc_addResourceToVoiceDefinition(system->rm, (picoos_char *) voiceName, (picoos_char *) resourceName);
    }

    return status;
}

/**
 * pico_releaseVoiceDefinition : Releases a voice definition
 * @param    system : pointer to a pico_System struct
 * @param    *voiceName : pointer to the area containing the voice definition
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_releaseVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (voiceName == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (picoos_strlen((picoos_char *) voiceName) == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else {
        picoos_emReset(system->common->em);
        status = picorsrc_releaseVoiceDefinition(system->rm, (picoos_char *) voiceName);
    }

    return status;
}
/* *** Engine creation and deletion functions *********************************/

/**
 * pico_newEngine : Creates and initializes a new Pico engine
 * @param    system : pointer to a pico_System struct
 * @param    *voiceName : pointer to the area containing the voice definition
 * @param    *outEngine : pointer to the Pico engine handle
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_newEngine(
        pico_System system,
        const pico_Char *voiceName,
        pico_Engine *outEngine
        )
{
    pico_Status status = PICO_OK;

    PICODBG_DEBUG(("creating engine for voice '%s'", (picoos_char *) voiceName));

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (voiceName == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (picoos_strlen((picoos_char *) voiceName) == 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else if (outEngine == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_emReset(system->common->em);
        if (system->engine == NULL) {
            *outEngine = (pico_Engine) picoctrl_newEngine(system->common->mm, system->rm, voiceName);
            if (*outEngine != NULL) {
                system->engine = (picoctrl_Engine) *outEngine;
            } else {
                status = picoos_emRaiseException(system->common->em, PICO_EXC_OUT_OF_MEM,
                            (picoos_char *) "out of memory creating new engine", NULL);
            }
        } else {
            status = picoos_emRaiseException(system->common->em, PICO_EXC_MAX_NUM_EXCEED,
                        NULL, (picoos_char *) "no more than %i engines", 1);
        }
    }

    return status;
}

/**
 * pico_disposeEngine : Disposes a Pico engine
 * @param    system : pointer to a pico_System struct
 * @param    *inoutEngine : pointer to the Pico engine handle
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_disposeEngine(
        pico_System system,
        pico_Engine *inoutEngine
        )
{
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (inoutEngine == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (!picoctrl_isValidEngineHandle(*((picoctrl_Engine *) inoutEngine))) {
        status = PICO_ERR_INVALID_HANDLE;
    } else {
        picoos_emReset(system->common->em);
        picoctrl_disposeEngine(system->common->mm, system->rm, (picoctrl_Engine *) inoutEngine);
        system->engine = NULL;
        status = picoos_emGetExceptionCode(system->common->em);
    }

    return status;
}



/* ****************************************************************************/
/* Engine-level API functions                                                 */
/* ****************************************************************************/

/**
 * pico_putTextUtf8 : Puts UTF8 text into Pico text input buffer
 * @param    engine : pointer to a Pico engine handle
 * @param    *text : pointer to the text buffer
 * @param    textSize : text buffer size
 * @param    *bytesPut : pointer to variable to receive the number of bytes put
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
 */
PICO_FUNC pico_putTextUtf8(
        pico_Engine engine,
        const pico_Char *text,
        const pico_Int16 textSize,
        pico_Int16 *bytesPut)
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else if (text == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else if (textSize < 0) {
        status = PICO_ERR_INVALID_ARGUMENT;
    } else if (bytesPut == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoctrl_engResetExceptionManager((picoctrl_Engine) engine);
        status = picoctrl_engFeedText((picoctrl_Engine) engine, (picoos_char *)text, textSize, bytesPut);
    }

    return status;
}

/**
 * pico_getData : Gets speech data from the engine.
 * @param    engine : pointer to a Pico engine handle
 * @param    *buffer : pointer to output buffer
 * @param    bufferSize : out buffer size
 * @param    *bytesReceived : pointer to a variable to receive the number of bytes received
 * @param    *outDataType : pointer to a variable to receive the type of buffer received
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getData(
        pico_Engine engine,
        void *buffer,
        const pico_Int16 bufferSize,
        pico_Int16 *bytesReceived,
        pico_Int16 *outDataType
        )
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_STEP_ERROR;
    } else if (buffer == NULL) {
        status = PICO_STEP_ERROR;
    } else if (bufferSize < 0) {
        status = PICO_STEP_ERROR;
    } else if (bytesReceived == NULL) {
        status = PICO_STEP_ERROR;
    } else {
        picoctrl_engResetExceptionManager((picoctrl_Engine) engine);
        status = picoctrl_engFetchOutputItemBytes((picoctrl_Engine) engine, (picoos_char *)buffer, bufferSize, bytesReceived);
        if ((status != PICO_STEP_IDLE) && (status != PICO_STEP_BUSY)) {
            status = PICO_STEP_ERROR;
        }
    }

    *outDataType = PICO_DATA_PCM_16BIT;
    return status;
}

/**
 * pico_resetEngine : Resets the engine
 * @param    engine : pointer to a Pico engine handle
 * @param resetMode : reset mode; one of PICO_RESET_FULL or PICO_RESET_SOFT
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_resetEngine(
        pico_Engine engine,
        pico_Int32 resetMode)
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
    } else {
        picoctrl_engResetExceptionManager((picoctrl_Engine) engine);

        resetMode = (PICO_RESET_SOFT == resetMode) ? PICO_RESET_SOFT : PICO_RESET_FULL;

        status = picoctrl_engReset((picoctrl_Engine) engine, (picoos_int32)resetMode);
    }

    return status;
}

/**
 * pico_getEngineStatusMessage : Returns the engine status or error description
 * @param    engine : pointer to a Pico engine handle
 * @param    errCode : error code
 * @param    outMessage : pointer to a memory area to receive the output message
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getEngineStatusMessage(
        pico_Engine engine,
        pico_Status errCode,
        pico_Retstring outMessage
        )
{
    pico_Status status = PICO_OK;

    PICODBG_DEBUG(("got error code %i", errCode));

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outMessage != NULL) {
            picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "'engine' not initialized", PICO_RETSTRINGSIZE);
        }
    } else if (outMessage == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_Common common = picoctrl_engGetCommon((picoctrl_Engine) engine);
        if (picoos_emGetExceptionCode(common->em) == PICO_OK) {
            if (errCode == PICO_OK) {
                picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "engine ok", PICO_RETSTRINGSIZE);
            } else {
                /* exceptionManager was not informed yet; produce default message */
                picoos_setErrorMsg((picoos_char *) outMessage, PICO_RETSTRINGSIZE, errCode, NULL, NULL, NULL);
            }
        } else {
            picoos_emGetExceptionMessage(common->em, (picoos_char *) outMessage, PICO_RETSTRINGSIZE);
        }
    }

    return status;
}

/**
 * pico_getNrEngineWarnings : Returns the number of warnings
 * @param    engine : pointer to a Pico engine handle
 * @param    *outNrOfWarnings: pointer to a variable to receive the number of warnings
 * @return  PICO_OK : successful
 * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
 * @callgraph
 * @callergraph
*/
PICO_FUNC pico_getNrEngineWarnings(
        pico_Engine engine,
        pico_Int32 *outNrOfWarnings
        )
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outNrOfWarnings != NULL) {
            *outNrOfWarnings = 0;
        }
    } else if (outNrOfWarnings == NULL) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_Common common = picoctrl_engGetCommon((picoctrl_Engine) engine);
        *outNrOfWarnings = picoos_emGetNumOfWarnings(common->em);
    }

    return status;
}

/**
     * pico_getEngineWarning : Returns a description of a warning
     * @param    engine : pointer to a Pico engine handle
     * @param    warningIndex : warning index
     * @param    *outCode: pointer to a variable to receive the warning code
     * @param    outMessage: pointer to a memory area to receive the warning description
     * @return  PICO_OK : successful
     * @return     PICO_ERR_INVALID_HANDLE, PICO_ERR_NULLPTR_ACCESS : errors
     * @callgraph
     * @callergraph
*/
PICO_FUNC pico_getEngineWarning(
        pico_Engine engine,
        const pico_Int32 warningIndex,
        pico_Status *outCode,
        pico_Retstring outMessage
        )
{
    pico_Status status = PICO_OK;

    if (!picoctrl_isValidEngineHandle((picoctrl_Engine) engine)) {
        status = PICO_ERR_INVALID_HANDLE;
        if (outMessage != NULL) {
            picoos_strlcpy((picoos_char *) outMessage, (picoos_char *) "'engine' not initialized", PICO_RETSTRINGSIZE);
        }
    } else if (warningIndex < 0) {
        status = PICO_ERR_INDEX_OUT_OF_RANGE;
    } else if ((outCode == NULL) || (outMessage == NULL)) {
        status = PICO_ERR_NULLPTR_ACCESS;
    } else {
        picoos_Common common = picoctrl_engGetCommon((picoctrl_Engine) engine);
        *outCode = picoos_emGetWarningCode(common->em, warningIndex);
        picoos_emGetWarningMessage(common->em, warningIndex, (picoos_char *) outMessage, (picoos_uint16) PICO_RETSTRINGSIZE);
    }

    return status;
}

// ============================================================================
// Additional functions specific to the ESP platform
//
// The "pico_loadMemoryResource()" function above is required for loading
// resource files that have been embedded or from partitions.
// ============================================================================

// Define all of the externs for voices that are to be embedded automatically
#if defined(CONFIG_PICOTTS_EN_GB_EMBED)
extern const uint8_t PICOTTS_EMBED_EN_GB_SG[] asm("_binary_en_GB_kh0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_EN_GB_TA[] asm("_binary_en_GB_ta_bin_start");
#endif

#if defined(CONFIG_PICOTTS_EN_US_EMBED)
extern const uint8_t PICOTTS_EMBED_EN_US_SG[] asm("_binary_en_US_lh0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_EN_US_TA[] asm("_binary_en_US_ta_bin_start");
#endif

#if defined(CONFIG_PICOTTS_FR_FR_EMBED)
extern const uint8_t PICOTTS_EMBED_FR_FR_SG[] asm("_binary_en_fr_FR_nk0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_FR_FR_TA[] asm("_binary_en_fr_FR_ta_bin_start");
#endif

#if defined(CONFIG_PICOTTS_DE_DE_EMBED)
extern const uint8_t PICOTTS_EMBED_DE_DE_SG[] asm("_binary_en_de_DE_gl0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_DE_DE_TA[] asm("_binary_en_de_DE_ta_bin_start");
#endif

#if defined(CONFIG_PICOTTS_IT_IT_EMBED)
extern const uint8_t PICOTTS_EMBED_IT_IT_SG[] asm("_binary_en_it_IT_cm0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_IT_IT_TA[] asm("_binary_en_it_IT_ta_bin_start");
#endif

#if defined(CONFIG_PICOTTS_ES_ES_EMBED)
extern const uint8_t PICOTTS_EMBED_ES_ES_SG[] asm("_binary_en_es_ES_zl0_sg_bin_start");
extern const uint8_t PICOTTS_EMBED_ES_ES_TA[] asm("_binary_en_es_ES_ta_bin_start");
#endif

// ============================================================================
// Create a voice using the memory addres of the preloaded SG and TA resource
// files.
// ============================================================================
PICO_FUNC pico_defineVoice(pico_System system,
                           const pico_Char *name,
                           const uint8_t sg[],
                           const uint8_t ta[])
{
    const uint8_t *rsrcs[2] = {sg, ta};
    pico_Status status = PICO_OK;

    // Validate picoSystem handle
    if (!is_valid_system_handle(system))
    {
        return PICO_ERR_INVALID_HANDLE;
    }

    // Create a new voice definition with the supplied name
    status = pico_createVoiceDefinition(system, name);
    if (status != PICO_OK)
    {
        PICODBG_DEBUG(("Unable to create voice definition for %s", name));
        return status;
    }

    // Process the SG and TA resource files
    for (int i = 0; i < 2; ++i)
    {
        pico_Resource resource;
        pico_Retstring rsrcName;
        const uint8_t *addr = rsrcs[i];

        // Parse the preloaded resource file
        status = pico_loadMemoryResource(system, addr, &resource);
        if (status)
        {
            PICODBG_DEBUG(("Can't load memory resource file for %s", name));
            break;
        }

        // Get it's name
        status = pico_getResourceName(system, resource, rsrcName);
        if (status)
        {
            PICODBG_DEBUG(("Can't get the name of the loaded resource for %s", name));
            break;
        }

        // And add it to the voice definition
        status = pico_addResourceToVoiceDefinition(system, name, (pico_Char *) rsrcName);
        if (status)
        {
            PICODBG_DEBUG(("Can't add the '%s' resource to the '%s' voice", rsrcName, name));
            break;
        }
    }

    return status;
}

#if PICO_PLATFORM == PICO_ESP
// ============================================================================
// Create a voice after mapping the SG and TA resources residing in partitions
// with the given subtypes.
// ============================================================================
PICO_FUNC pico_loadVoiceFromPart(pico_System system,
                                 const pico_Char *name,
                                 esp_partition_subtype_t sg,
                                 esp_partition_subtype_t ta)
{
    const esp_partition_t *sg_part;
    const esp_partition_t *ta_part;
    const void *sg_addr;
    const void *ta_addr;
    esp_partition_mmap_handle_t sg_hnd;
    esp_partition_mmap_handle_t ta_hnd;
    esp_err_t ret;

    // Validate picoSystem handle
    if (!is_valid_system_handle(system))
    {
        return PICO_ERR_INVALID_HANDLE;
    }

    // Attempt to find a partition with the SG subtype
    sg_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, sg, NULL);
    if (sg_part == NULL)
    {
        PICODBG_DEBUG(("SG partition not found for %s", name));
        return PICO_EXC_FILE_NOT_FOUND; 
    }

    // Attempt to find a partition with the SG subtype
    ta_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ta, NULL);
    if (ta_part == NULL)
    {
        PICODBG_DEBUG(("TA partition not found for %s", name));
        return PICO_EXC_FILE_NOT_FOUND; 
    }

    // Map the SG partition into the address space
    ret = esp_partition_mmap(sg_part,
                             0,
                             sg_part->size,
                             ESP_PARTITION_MMAP_DATA,
                             &sg_addr,
                             &sg_hnd);
    if (ret != ESP_OK)
    {
        PICODBG_DEBUG(("mmap() failed for SG partition: %s", esp_err_to_name(ret)));
        return PICO_EXC_CANT_OPEN_FILE;
    }

    // Map the SG partition into the address space
    ret = esp_partition_mmap(ta_part,
                             0,
                             ta_part->size,
                             ESP_PARTITION_MMAP_DATA,
                             &ta_addr,
                             &ta_hnd);
    if (ret != ESP_OK)
    {
        // Unmap the SG partition
        esp_partition_munmap(sg_hnd);

        PICODBG_DEBUG(("mmap() failed for TA partition: %s", esp_err_to_name(ret)));
        return PICO_EXC_CANT_OPEN_FILE;
    }

    // Go define the voice using the newly mapped partitions
    return pico_defineVoice(system, name, sg_addr, ta_addr);
}

// ============================================================================
// ============================================================================
PICO_FUNC pico_loadVoiceFromFile(pico_System system,
                                 const pico_Char *name,
                                 const char *sg,
                                 const char *ta)
{
    const pico_Char *rsrcs[2] = {(pico_Char *)sg, (pico_Char *)ta};
    pico_Status status = PICO_OK;

    if (!is_valid_system_handle(system))
    {
        return PICO_ERR_INVALID_HANDLE;
    }

    // Validate picoSystem handle
    if (!is_valid_system_handle(system))
    {
        return PICO_ERR_INVALID_HANDLE;
    }

    // Create a new voice definition with the supplied name
    status = pico_createVoiceDefinition(system, name);
printf("create voice %d\n", status);
    if (status != PICO_OK)
    {
        PICODBG_DEBUG(("Unable to create voice definition for %s", name));
        return status;
    }

    // Process the SG and TA resource files
    for (int i = 0; i < 2; ++i)
    {
        pico_Resource resource;
        pico_Retstring rsrcName;
        const pico_Char *path = rsrcs[i];

        // Parse the preloaded resource file
        status = pico_loadResource(system, path, &resource);
    pico_Retstring outMessage;
        pico_getSystemStatusMessage(system, status, outMessage);
printf("load voice %d %s\n", status, outMessage);
        if (status)
        {
            PICODBG_DEBUG(("Can't load %s resource file for %s", path, name));
            break;
        }

        // Get it's name
        status = pico_getResourceName(system, resource, rsrcName);
printf("load name %s\n", rsrcName);
        if (status)
        {
            PICODBG_DEBUG(("Can't get the name of the loaded resource for %s", name));
            break;
        }

        // And add it to the voice definition
        status = pico_addResourceToVoiceDefinition(system, name, (pico_Char *) rsrcName);
printf("add resource %d\n", status);
        if (status)
        {
            PICODBG_DEBUG(("Can't add the '%s' resource to the '%s' voice", rsrcName, name));
            break;
        }
    }

    return status;
}

// Create some shorthand defines to make life easier
#define F_MEM(n, l) \
    status = pico_defineVoice(system, (const pico_Char *) n, \
                              PICOTTS_EMBED_ ## l ## _SG, \
                              PICOTTS_EMBED_ ## l ## _TA); \
    if (status) \
        return status;


#define F_PART(n, l) \
    status = pico_loadVoiceFromPart(system, (const pico_Char *) n, \
                                    ESP_PARTITION_SUBTYPE_DATA_ ## l ## _SG, \
                                    ESP_PARTITION_SUBTYPE_DATA_ ## l ## _TA); \
    if (status) \
        return status;

#define F_FILE(n, l) \
    status = pico_loadVoiceFromFile(system, (const pico_Char *) n, \
                                    CONFIG_PICOTTS_ ## l ## _SG_FILE_NAME, \
                                    CONFIG_PICOTTS_ ## l ## _TA_FILE_NAME); \
    if (status) \
        return status;

// ============================================================================
// Define all configured voices
// ============================================================================
PICO_FUNC pico_loadVoices(pico_System system)
{
    pico_Status status = PICO_OK;

    // Validate picoSystem handle
    if (!is_valid_system_handle(system))
    {
        return PICO_ERR_INVALID_HANDLE;
    }

    // Process the German language
#if defined(CONFIG_PICOTTS_DE_DE_EMBED)
    F_MEM("de_DE", DE_DE);
#elif defined(CONFIG_PICOTTS_DE_DE_PART)
    F_PART("de_DE", DE_DE);
#elif defined(CONFIG_PICOTTS_DE_DE_FILE)
    F_FILE("de_DE", DE_DE);
#endif

    // Process the British English language
#if defined(CONFIG_PICOTTS_EN_GB_EMBED)
    F_MEM("en_GB", EN_GB);
#elif defined(CONFIG_PICOTTS_EN_GB_PART)
    F_PART("en_GB", EN_GB);
#elif defined(CONFIG_PICOTTS_EN_GB_FILE)
    F_FILE("en_GB", EN_GB);
#endif

    // Process the US English language
#if defined(CONFIG_PICOTTS_EN_US_EMBED)
    F_MEM("en_US", EN_US);
#elif defined(CONFIG_PICOTTS_EN_US_PART)
    F_PART("en_US", EN_US);
#elif defined(CONFIG_PICOTTS_EN_US_FILE)
    F_FILE("en_US", EN_US);
#endif

    // Process the Spanish language
#if defined(CONFIG_PICOTTS_ES_ES_EMBED)
    F_MEM("es_ES", ES_ES);
#elif defined(CONFIG_PICOTTS_ES_ES_PART)
    F_PART("es_ES", ES_ES);
#elif defined(CONFIG_PICOTTS_ES_ES_FILE)
    F_FILE("es_ES", ES_ES);
#endif

    // Process the French language
#if defined(CONFIG_PICOTTS_FR_FR_EMBED)
    F_MEM("fr_FR", FR_FR);
#elif defined(CONFIG_PICOTTS_FR_FR_PART)
    F_PART("fr_FR", FR_FR);
#elif defined(CONFIG_PICOTTS_FR_FR_FILE)
    F_FILE("fr_FR", FR_FR);
#endif

    // Process the Italian language
#if defined(CONFIG_PICOTTS_IT_IT_EMBED)
    F_MEM("it_IT", IT_IT);
#elif defined(CONFIG_PICOTTS_IT_IT_PART)
    F_PART("it_IT", IT_IT);
#elif defined(CONFIG_PICOTTS_IT_IT_FILE)
    F_FILE("it_IT", IT_IT);
#endif

    return status;
}

#endif /* PICO_PLATFORM == PICO_ESP */

/* end */
