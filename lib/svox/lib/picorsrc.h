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
 * @file picorsrc.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
/**
 * @addtogroup picorsrc
 *
 * <b> Pico Resource Management module </b>\n
 *
*/

#ifndef PICORSRC_H_
#define PICORSRC_H_

#include "picodefs.h"
#include "picoos.h"
#include "picoknow.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


#define PICORSRC_MAX_RSRC_NAME_SIZ PICO_MAX_RESOURCE_NAME_SIZE /* including terminating NULLC */

#define PICORSRC_MAX_NUM_VOICES 64

/* size of kb array of a voice */
#define PICORSRC_KB_ARRAY_SIZE 64

typedef picoos_char picorsrc_resource_name_t[PICORSRC_MAX_RSRC_NAME_SIZ];

typedef enum picorsrc_resource_type {
    PICORSRC_TYPE_NULL,
    PICORSRC_TYPE_TEXTANA,
    PICORSRC_TYPE_SIGGEN,
    PICORSRC_TYPE_USER_LEX,
    PICORSRC_TYPE_USER_PREPROC,
    PICORSRC_TYPE_OTHER
} picorsrc_resource_type_t;


#define PICORSRC_FIELD_VALUE_TEXTANA (picoos_char *) "TEXTANA"
#define PICORSRC_FIELD_VALUE_SIGGEN (picoos_char *) "SIGGEN"
#define PICORSRC_FIELD_VALUE_USERLEX (picoos_char *) "USERLEX"
#define PICORSRC_FIELD_VALUE_USERTPP (picoos_char *) "USERTPP"



typedef struct picorsrc_resource_manager * picorsrc_ResourceManager;
typedef struct picorsrc_voice            * picorsrc_Voice;
typedef struct picorsrc_resource         * picorsrc_Resource;


/* **************************************************************************
 *
 *          file name extensions
 *
 ****************************************************************************/

#define PICO_BIN_EXTENSION      ".bin"
#define PICO_INPLACE_EXTENSION  ".inp"



/* **************************************************************************
 *
 *          construct/destruct resource manager
 *
 ****************************************************************************/

/* create resource manager, given a config file name (or default name, if empty) */

picorsrc_ResourceManager picorsrc_newResourceManager(picoos_MemoryManager mm, picoos_Common common /* , picoos_char * configFile */);

void picorsrc_disposeResourceManager(picoos_MemoryManager mm, picorsrc_ResourceManager * this);


/* **************************************************************************
 *
 *          resources
 *
 ****************************************************************************/

/**
 * Returns non-zero if 'resource' is a valid resource handle, zero otherwise.
 */
picoos_int16 picoctrl_isValidResourceHandle(picorsrc_Resource resource);

/* load resource file. the type of resource file, magic numbers, checksum etc. are in the header, then follows the directory
 * (with fixed structure per resource type), then the knowledge bases themselves (as byte streams) */
pico_status_t picorsrc_loadResource(picorsrc_ResourceManager this,
        picoos_char * fileName, picorsrc_Resource * resource);

/* add preloaded resource. the type of resource file, magic numbers, checksum etc. are in the header, then follows the directory
 * (with fixed structure per resource type), then the knowledge bases themselves (as byte streams) */
pico_status_t picorsrc_loadMemoryResource(picorsrc_ResourceManager that,
        picoos_char * memoryAddress, picorsrc_Resource * resource);

/* unload resource file. (warn if resource file is busy) */
pico_status_t picorsrc_unloadResource(picorsrc_ResourceManager this, picorsrc_Resource * rsrc);

pico_status_t picorsrc_createDefaultResource(picorsrc_ResourceManager this /*,
        picorsrc_Resource * resource */);


pico_status_t picorsrc_rsrcGetName(picorsrc_Resource resource,
        picoos_char * name, picoos_uint32 maxlen);

/* **************************************************************************
 *
 *          voice definitions
 *
 ****************************************************************************/


pico_status_t picorsrc_createVoiceDefinition(picorsrc_ResourceManager this,
        picoos_char * voiceName);


pico_status_t picorsrc_releaseVoiceDefinition(picorsrc_ResourceManager this,
        picoos_char * voiceName);

pico_status_t picorsrc_addResourceToVoiceDefinition(picorsrc_ResourceManager this,
        picoos_char * voiceName, picoos_char * resourceName);

/* **************************************************************************
 *
 *          voices
 *
 ****************************************************************************/

/**  object   : Voice
 *   shortcut : voice
 *
 */

typedef struct picorsrc_voice {

    picorsrc_Voice next;

    picoknow_KnowledgeBase kbArray[PICORSRC_KB_ARRAY_SIZE];

    picoos_uint8 numResources;

    picorsrc_Resource resourceArray[PICO_MAX_NUM_RSRC_PER_VOICE];


} picorsrc_voice_t;



/* create voice, given a voice name. the corresponding lock counts are incremented */
pico_status_t picorsrc_createVoice(picorsrc_ResourceManager this, const picoos_char * voiceName, picorsrc_Voice * voice);

/* dispose voice. the corresponding lock counts are decremented. */
pico_status_t picorsrc_releaseVoice(picorsrc_ResourceManager this, picorsrc_Voice * voice);

#ifdef __cplusplus
}
#endif



#endif /*PICORSRC_H_*/
