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
 * @file picodefs.h
 *
 * SVOX Pico definitions
 * (SVOX Pico version 1.0 and later)
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 */


#ifndef PICODEFS_H_
#define PICODEFS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************/
/* SVOX Pico limits                                                   */
/* ********************************************************************/
/* maximum size of a voice name, including the terminating null
   character */
#define PICO_MAX_VOICE_NAME_SIZE        32

/* maximum size of a resource name, incl. the terminating null
   character */
#define PICO_MAX_RESOURCE_NAME_SIZE     32

/* maximum size of a data path name, incl. the terminating null
   character */
#define PICO_MAX_DATAPATH_NAME_SIZE    128

/* maximum size of a file name, incl. the terminating null
   character */
#define PICO_MAX_FILE_NAME_SIZE         64

/* maximum number of resources */
#define PICO_MAX_NUM_RESOURCES          64

/* maximum number of voice definitions */
#define PICO_MAX_NUM_VOICE_DEFINITIONS  64

/* maximum number of resources per voice */
#define PICO_MAX_NUM_RSRC_PER_VOICE     16

/* maximum length of foreign header prepended to PICO resource files
   (header length must be a multiple of 4 bytes) */
#define PICO_MAX_FOREIGN_HEADER_LEN     64



/* ********************************************************************/
/* SVOX PICO status codes                                             */
/* ********************************************************************/

typedef signed int pico_Status;


/* Okay ***************************************************************/
/* functions return PICO_OK if all is okay */

#define PICO_OK                         (pico_Status)     0


/* Exceptions and error codes *****************************************/

/* in case of exceptional events and errors (e.g. unexpected user
   input) that disrupt the normal flow of operation, PICO_EXC_* or
   PICO_ERR_* are returned. */

#define PICO_EXC_NUMBER_FORMAT          (pico_Status)   -10
#define PICO_EXC_MAX_NUM_EXCEED         (pico_Status)   -11
#define PICO_EXC_NAME_CONFLICT          (pico_Status)   -12
#define PICO_EXC_NAME_UNDEFINED         (pico_Status)   -13
#define PICO_EXC_NAME_ILLEGAL           (pico_Status)   -14

/* buffer interaction */
#define PICO_EXC_BUF_OVERFLOW           (pico_Status)   -20
#define PICO_EXC_BUF_UNDERFLOW          (pico_Status)   -21
#define PICO_EXC_BUF_IGNORE             (pico_Status)   -22

/* internal memory handling */
#define PICO_EXC_OUT_OF_MEM             (pico_Status)   -30

/* files */
#define PICO_EXC_CANT_OPEN_FILE         (pico_Status)   -40
#define PICO_EXC_UNEXPECTED_FILE_TYPE   (pico_Status)   -41
#define PICO_EXC_FILE_CORRUPT           (pico_Status)   -42
#define PICO_EXC_FILE_NOT_FOUND         (pico_Status)   -43

/* resources */
#define PICO_EXC_RESOURCE_BUSY          (pico_Status)   -50
#define PICO_EXC_RESOURCE_MISSING       (pico_Status)   -51

/* knowledge bases */
#define PICO_EXC_KB_MISSING             (pico_Status)   -60

/* runtime exceptions */
#define PICO_ERR_NULLPTR_ACCESS         (pico_Status)  -100
#define PICO_ERR_INVALID_HANDLE         (pico_Status)  -101
#define PICO_ERR_INVALID_ARGUMENT       (pico_Status)  -102
#define PICO_ERR_INDEX_OUT_OF_RANGE     (pico_Status)  -103

/* other errors ("external" errors, e.g. hardware failure). */
#define PICO_ERR_OTHER                  (pico_Status)  -999


/* Warnings ***********************************************************/

/* general */
#define PICO_WARN_INCOMPLETE            (pico_Status)    10
#define PICO_WARN_FALLBACK              (pico_Status)    11
#define PICO_WARN_OTHER                 (pico_Status)    19

/* resources */
#define PICO_WARN_KB_OVERWRITE          (pico_Status)    50
#define PICO_WARN_RESOURCE_DOUBLE_LOAD  (pico_Status)    51

/* classifiers */
#define PICO_WARN_INVECTOR              (pico_Status)    60
#define PICO_WARN_CLASSIFICATION        (pico_Status)    61
#define PICO_WARN_OUTVECTOR             (pico_Status)    62

/* processing units */
#define PICO_WARN_PU_IRREG_ITEM         (pico_Status)    70
#define PICO_WARN_PU_DISCARD_BUF        (pico_Status)    71



/* ********************************************************************/
/* Engine getData return values                                       */
/* ********************************************************************/

#define PICO_STEP_IDLE                  (pico_Status)   200
#define PICO_STEP_BUSY                  (pico_Status)   201

#define PICO_STEP_ERROR                 (pico_Status)  -200

/* ********************************************************************/
/* resetEngine reset modes                                            */
/* ********************************************************************/

#define PICO_RESET_FULL                                 0
#define PICO_RESET_SOFT                                 0x10


/* ********************************************************************/
/* Engine getData outDataType values                                  */
/* ********************************************************************/

/* 16 bit PCM samples, native endianness of platform */
#define PICO_DATA_PCM_16BIT             (pico_Int16)  1

#ifdef __cplusplus
}
#endif


#endif /*PICODEFS_H_*/
