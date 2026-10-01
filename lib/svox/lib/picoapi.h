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
 * @file picoapi.h
 *
 * SVOX Pico application programming interface
 * (SVOX Pico version 1.0 and later)
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 */


/**
 * @addtogroup picoapi
 *
@b Basic_Concepts

@e SVOX_Pico_System

The SVOX Pico 'system' is the entity that manages data common to all
SVOX Pico engines, e.g. linguistic data needed to do text-to-speech
(TTS) synthesis, license key, etc.  All API functions on the Pico
system level take a 'pico_System' handle as the first parameter.

@e SVOX_Pico_Engine

A SVOX Pico 'engine' provides the functions needed to perform actual
synthesis. Currently there can be only one engine instance at a time
(concurrent engines will be possible in the future). All API functions
at the engine level take a 'pico_Engine' handle as the first
parameter.

@e SVOX_Pico_Resource

A SVOX Pico 'resource' denotes all the language- and speaker-dependent
data needed to do TTS synthesis. In the following, the term 'resource'
may be used interchangeably with the term 'lingware'. A resource file
contains a set of knowledge bases for an entire TTS voice or parts of
it.


@b Basic_Usage

In its most basic form, an application must call the following
functions in order to perform TTS synthesis:

   - pico_initialize
   - pico_loadResource
   - pico_createVoiceDefinition
   - pico_addResourceToVoiceDefinition
   - pico_newEngine
   - pico_putTextUtf8
   - pico_getData (several times)
   - pico_disposeEngine
   - pico_releaseVoiceDefinition
   - pico_unloadResource
   - pico_terminate

It is possible to repeatedly run the above sequence, i.e., the SVOX
Pico system may be initialized and terminated multiple times. This may
be useful in applications that need TTS functionality only from time
to time.


@b Conventions

@e Function_arguments

All arguments that only return values are marked by a leading 'out...'
in their name. All arguments that are used as input and output values
are marked by a leading 'inout...'. All other arguments are read-only
(input) arguments.

@e Error_handling

All API functions return a status code which is one of the status
constants defined in picodefs.h. In case of an error, a more detailed
description of the status can be retrieved by calling function
'pico_getSystemStatusMessage' (or 'pico_getEngineStatusMessage'
if the error happened on the SVOX Pico engine level).

Unlike errors, warnings do not prevent an API function from performing
its function, but output might not be as intended. Functions
'pico_getNrSystemWarnings' and 'pico_getNrEngineWarnings' respectively
can be used to determine whether an API function caused any
warnings. Details about warnings can be retrieved by calling
'pico_getSystemWarning' and 'pico_getEngineWarning' respectively.

*/



#ifndef PICOAPI_H_
#define PICOAPI_H_



#include "picodefs.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


#ifdef _WIN32
#  define PICO_EXPORT  __declspec( dllexport )
#else
#  define PICO_EXPORT extern
#endif

#define PICO_FUNC PICO_EXPORT pico_Status



/* ********************************************************************/
/* PICO data types                                                    */
/* ********************************************************************/

/* Handle types (opaque) for Pico system, resource, engine ************/

typedef struct pico_system   *pico_System;
typedef struct pico_resource *pico_Resource;
typedef struct pico_engine   *pico_Engine;


/* Signed/unsigned integer data types *********************************/

#define PICO_INT16_MAX   32767
#define PICO_UINT16_MAX  0xffff
#define PICO_INT32_MAX   2147483647
#define PICO_UINT32_MAX  0xffffffff

#include <limits.h>

#if (SHRT_MAX == PICO_INT16_MAX)
typedef short pico_Int16;
#else
#error "platform not supported"
#endif

#if (USHRT_MAX == PICO_UINT16_MAX)
typedef unsigned short pico_Uint16;
#else
#error "platform not supported"
#endif

#if (INT_MAX == PICO_INT32_MAX)
typedef int pico_Int32;
#else
#error "platform not supported"
#endif

#if (UINT_MAX == PICO_UINT32_MAX)
typedef unsigned int pico_Uint32;
#else
#error "platform not supported"
#endif


/* Char data type *****************************************************/

typedef unsigned char pico_Char;


/* String type to be used when ASCII string values are returned *******/

#define PICO_RETSTRINGSIZE 200  /* maximum length of returned strings */

typedef char pico_Retstring[PICO_RETSTRINGSIZE];



/* ********************************************************************/
/* System-level API functions                                         */
/* ********************************************************************/

/* System initialization and termination functions ********************/

/**
   Initializes the Pico system and returns its handle in 'outSystem'.
   'memory' and 'size' define the location and maximum size of memory
   in number of bytes that the Pico system will use. The minimum size
   required depends on the number of engines and configurations of
   lingware to be used. No additional memory will be allocated by the
   Pico system. This function must be called before any other API
   function is called. It may only be called once (e.g. at application
   startup), unless a call to 'pico_terminate'.
*/
PICO_FUNC pico_initialize(
        void *memory,
        const pico_Uint32 size,
        pico_System *outSystem
        );

/**
   Terminates the Pico system. Lingware resources still being loaded
   are unloaded automatically. The memory area provided to Pico in
   'pico_initialize' is released. The system handle becomes
   invalid. It is not allowed to call this function as long as Pico
   engine instances are existing. No API function may be called after
   this function, except for 'pico_initialize', which reinitializes
   the system.
*/
PICO_FUNC pico_terminate(
        pico_System *system
        );


/* System status and error/warning message retrieval ******************/

/**
   Returns in 'outMessage' a description of the system status or of an
   error that occurred with the most recently called system-level API
   function.
*/
PICO_FUNC pico_getSystemStatusMessage(
        pico_System system,
        pico_Status errCode,
        pico_Retstring outMessage
        );

/**
   Returns in 'outNrOfWarnings' the number of warnings that occurred
   with the most recently called system-level API function.
*/
PICO_FUNC pico_getNrSystemWarnings(
        pico_System system,
        pico_Int32 *outNrOfWarnings
        );

/**
   Returns in 'outMessage' a description of a warning that occurred
   with the most recently called system-level API function.
   'warningIndex' must be in the range 0..N-1 where N is the number of
   warnings returned by 'pico_getNrSystemWarnings'. 'outCode' returns
   the warning as an integer code (cf. PICO_WARN_*).
*/
PICO_FUNC pico_getSystemWarning(
        pico_System system,
        const pico_Int32 warningIndex,
        pico_Status *outCode,
        pico_Retstring outMessage
        );


/* Resource loading and unloading functions ***************************/

/**
   Loads a resource file into the Pico system. The number of resource
   files loaded in parallel is limited by PICO_MAX_NUM_RESOURCES.
   Loading of a resource file may be done at any time (even in
   parallel to a running engine doing TTS synthesis), but with the
   general restriction that functions taking a system handle as their
   first argument must be called in a mutually exclusive fashion. The
   loaded resource will be available only to engines started after the
   resource is fully loaded, i.e., not to engines currently
   running.
*/
PICO_FUNC pico_loadResource(
        pico_System system,
        const pico_Char *resourceFileName,
        pico_Resource *outResource
        );

/**
   Adds a preloaded resource into the Pico system. The number of resource
   files loaded in parallel is limited by PICO_MAX_NUM_RESOURCES.
   Loading of a resource file may be done at any time (even in
   parallel to a running engine doing TTS synthesis), but with the
   general restriction that functions taking a system handle as their
   first argument must be called in a mutually exclusive fashion. The
   loaded resource will be available only to engines started after the
   resource is fully loaded, i.e., not to engines currently
   running.
*/
PICO_FUNC pico_loadMemoryResource(
        pico_System system,
        const pico_Char *resourceAddress,
        pico_Resource *outResource
        );


/**
   Unloads a resource file from the Pico system. If no engine uses the
   resource file, the resource is removed immediately and its
   associated internal memory is released, otherwise
   PICO_EXC_RESOURCE_BUSY is returned.
*/
PICO_FUNC pico_unloadResource(
        pico_System system,
        pico_Resource *inoutResource
        );

/* *** Resource inspection functions *******************************/

/**
 Gets the unique resource name of a loaded resource
*/
PICO_FUNC pico_getResourceName(
        pico_System system,
        pico_Resource resource,
        pico_Retstring outName);


/* Voice definition ***************************************************/

/**
   Creates a voice definition. Resources must be added to the created
   voice with 'pico_addResourceToVoiceDefinition' before using the
   voice in 'pico_newEngine'. It is an error to create a voice
   definition with a previously defined voice name. In that case use
   'pico_releaseVoiceName' first.
*/
PICO_FUNC pico_createVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName
        );

/**
   Adds a mapping pair ('voiceName', 'resourceName') to the voice
   definition. Multiple mapping pairs can added to a voice defintion.
   When calling 'pico_newEngine' with 'voiceName', the corresponding
   resources from the mappings will be used with that engine. */

PICO_FUNC pico_addResourceToVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName,
        const pico_Char *resourceName
        );


/**
  Releases the voice definition 'voiceName'.

*/
PICO_FUNC pico_releaseVoiceDefinition(
        pico_System system,
        const pico_Char *voiceName
        );


/* Engine creation and deletion functions *****************************/

/**
   Creates and initializes a new Pico engine instance and returns its
   handle in 'outEngine'. Only one instance per system is currently
   possible.
*/
PICO_FUNC pico_newEngine(
        pico_System system,
        const pico_Char *voiceName,
        pico_Engine *outEngine
        );


/**
 Disposes a Pico engine and releases all memory it occupied. The
 engine handle becomes invalid.
*/
PICO_FUNC pico_disposeEngine(
        pico_System system,
        pico_Engine *inoutEngine
        );



/* ********************************************************************/
/* Engine-level API functions                                         */
/* ********************************************************************/

/**
   Puts text 'text' encoded in UTF8 into the Pico text input buffer.
   'textSize' is the maximum size in number of bytes accessible in
   'text'. The input text may also contain text-input commands to
   change, for example, speed or pitch of the resulting speech
   output. The number of bytes actually copied to the Pico text input
   buffer is returned in 'outBytesPut'. Sentence ends are
   automatically detected. '\0' characters may be embedded in 'text'
   to finish text input or separate independently to be synthesized
   text parts from each other. Repeatedly calling 'pico_getData' will
   result in the content of the text input buffer to be synthesized
   (up to the last sentence end or '\0' character detected). To empty
   the internal buffers without finishing synthesis, use the function
   'pico_resetEngine'.
*/
PICO_FUNC pico_putTextUtf8(
        pico_Engine engine,
        const pico_Char *text,
        const pico_Int16 textSize,
        pico_Int16 *outBytesPut
        );

/**
   Gets speech data from the engine. Every time this function is
   called, the engine performs, within a short time slot, a small
   amount of processing its input text, and then gives control back to
   the calling application. Ie. after calling 'pico_putTextUtf8'
   (incl. a final embedded '\0'), this function needs to be called
   repeatedly till 'outBytesReceived' bytes are returned in
   'outBuffer'. The type of data returned in 'outBuffer' (e.g. 8 or 16
   bit PCM samples) is returned in 'outDataType' and depends on the
   lingware resources. Possible 'outDataType' values are listed in
   picodefs.h (PICO_DATA_*).
   This function returns PICO_STEP_BUSY while processing input and
   producing speech output. Once all data is returned and there is no
   more input text available in the Pico text input buffer,
   PICO_STEP_IDLE is returned.  All other function return values
   indicate a system error.
*/
PICO_FUNC pico_getData(
        pico_Engine engine,
        void *outBuffer,
        const pico_Int16 bufferSize,
        pico_Int16 *outBytesReceived,
        pico_Int16 *outDataType
        );

/**
   Resets the engine and clears all engine-internal buffers, in
   particular text input and signal data output buffers.
   'resetMode' is one of 'PICO_RESET_SOFT', to be used to flush the engine,
   or 'PICO_RESET_FULL', to reset the engine after an engine error.
*/
PICO_FUNC pico_resetEngine(
        pico_Engine engine,
        pico_Int32 resetMode
);


/* Engine status and error/warning message retrieval ******************/

/**
   Returns in 'outMessage' a description of the engine status or of an
   error that occurred with the most recently called engine-level API
   function.
*/
PICO_FUNC pico_getEngineStatusMessage(
        pico_Engine engine,
        pico_Status errCode,
        pico_Retstring outMessage
        );

/**
   Returns in 'outNrOfWarnings' the number of warnings that occurred
   with the most recently called engine-level API function.
*/
PICO_FUNC pico_getNrEngineWarnings(
        pico_Engine engine,
        pico_Int32 *outNrOfWarnings
        );

/**
   Returns in 'outMessage' a description of a warning that occurred
   with the most recently called engine-level API function.
   'warningIndex' must be in the range 0..N-1 where N is the number of
   warnings returned by 'pico_getNrEngineWarnings'. 'outCode' returns
   the warning as an integer code (cf. PICO_WARN_*).
*/
PICO_FUNC pico_getEngineWarning(
        pico_Engine engine,
        const pico_Int32 warningIndex,
        pico_Status *outCode,
        pico_Retstring outMessage
        );


#define PICO_VOICE_EN_GB ((const pico_Char *) "en_GB")
#define PICO_VOICE_EN_US ((const pico_Char *) "en_US")
#define PICO_VOICE_FR_FR ((const pico_Char *) "fr_FR")
#define PICO_VOICE_DE_DE ((const pico_Char *) "de_DE")
#define PICO_VOICE_IT_IT ((const pico_Char *) "it_IT")
#define PICO_VOICE_ES_ES ((const pico_Char *) "es_ES")

#include "picopltf.h"
#if PICO_PLATFORM == PICO_ESP
// We'll need the partiion table API
#include "esp_partition.h"
#endif

/**
*/
PICO_FUNC pico_defineVoice(pico_System system,
                           const pico_Char *name,
                           const uint8_t sg[],
                           const uint8_t ta[]);

#if PICO_PLATFORM == PICO_ESP
/**
*/
PICO_FUNC pico_loadVoiceFromPart(pico_System system,
                                 const pico_Char *name,
                                 esp_partition_subtype_t sg,
                                 esp_partition_subtype_t ta);

/**
*/
PICO_FUNC pico_loadVoiceFromFile(pico_System system,
                                 const pico_Char *name,
                                 const char *sg,
                                 const char *ta);
/**
*/
PICO_FUNC pico_loadVoices(pico_System system);
#endif /* PICO_PLATFORM == PICO_ESP */

#ifdef __cplusplus
}
#endif

#endif /*PICOAPI_H_*/
