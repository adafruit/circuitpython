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
 * @file picoos.h
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
/**
 * @addtogroup picoos

 * <b> Operating system generalization module </b>\n
 *
*/

#ifndef PICOOS_H_
#define PICOOS_H_

#include "picodefs.h"
#include "picopal.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* some "switch"  used in picopal and in picocep ... */
#define PICOOS_DIV_USE_INV PICOPAL_DIV_USE_INV

/* *************************************************/
/* types                                           */
/* *************************************************/

typedef picopal_uint8   picoos_uint8;
typedef picopal_uint16  picoos_uint16;
typedef picopal_uint32  picoos_uint32;

typedef picopal_int8    picoos_int8;
typedef picopal_int16   picoos_int16;
typedef picopal_int32   picoos_int32;

typedef picopal_double  picoos_double;
typedef picopal_single  picoos_single;

typedef picopal_char    picoos_char;
typedef picopal_uchar   picoos_uchar;

typedef picopal_uint8   picoos_bool;

typedef picopal_objsize_t picoos_objsize_t;
typedef picopal_ptrdiff_t picoos_ptrdiff_t;

/* *************************************************/
/* functions                                       */
/* *************************************************/


picoos_int32 picoos_atoi(const picoos_char *);
picoos_int8 picoos_strcmp(const picoos_char *, const picoos_char *);
picoos_int8 picoos_strncmp(const picoos_char *a, const picoos_char *b, picoos_objsize_t siz);
picoos_uint32 picoos_strlen(const picoos_char *);
picoos_char * picoos_strchr(const picoos_char *, picoos_char);
picoos_char *picoos_strstr(const picoos_char *s, const picoos_char *substr);
picoos_int16 picoos_slprintf(picoos_char * b, picoos_uint32 bsize, const picoos_char *f, ...);
picoos_char * picoos_strcpy(picoos_char *, const picoos_char *);
picoos_char * picoos_strcat(picoos_char *, const picoos_char *);

/* copies 'length' bytes from 'src' to 'dest'. (regions may be overlapping) no error checks! */
void * picoos_mem_copy(const void * src, void * dst,  picoos_objsize_t length);

/* compare 'length' bytes from 'src' to 'dest' */
picoos_int32 picoos_mem_cmp(const void * src, void * dst,  picoos_objsize_t length);


/* safe versions */
picoos_objsize_t picoos_strlcpy(picoos_char *dst, const picoos_char *src, picoos_objsize_t siz);
void * picoos_mem_set(void * dest, picoos_uint8 byte_val, picoos_objsize_t length);

picoos_double picoos_cos(const picoos_double cos_arg);
picoos_double picoos_sin(const picoos_double sin_arg);
picoos_double picoos_fabs(const picoos_double fabs_arg);

picoos_double picoos_quick_exp(const picoos_double y);


void picoos_get_sep_part_str (picoos_char string[], picoos_int32 stringlen, picoos_int32 * ind, picoos_char sepCh, picoos_char part[], picoos_int32 maxsize, picoos_uint8 * done);
pico_status_t picoos_string_to_uint32 (picoos_char str[], picoos_uint32 * res);
pico_status_t picoos_string_to_int32 (picoos_char str[], picoos_int32 * res);

/* *****************************************************************/
/* "Common"                                                        */
/* *****************************************************************/
/* picoos_common is a collection of basic functionalities that must be globally accesible from every "big" function.
 * It includes pointers to the MemoryManasger, ExceptionManager and a list of open files. */

typedef struct memory_manager * picoos_MemoryManager;
typedef struct picoos_exception_manager * picoos_ExceptionManager;
typedef struct picoos_file * picoos_File;


/**  object   : Common
 *   shortcut : common
 *
 */
typedef struct picoos_common * picoos_Common;

/* the picoos_common structure itself is exported so no access functions are needed. Handle with care! (might be changed later) */
typedef struct picoos_common {
    picoos_ExceptionManager em;
    picoos_MemoryManager mm;
    picoos_File fileList;
} picoos_common_t;

picoos_Common picoos_newCommon(picoos_MemoryManager mm);

void picoos_disposeCommon(picoos_MemoryManager mm, picoos_Common * this);


/* *****************************************************************/
/* Memory Management                                               */
/* *****************************************************************/

typedef picoos_char * byte_ptr_t;

#define PICOOS_ALIGN_SIZE 8



void * picoos_raw_malloc(byte_ptr_t raw_mem,
        picoos_objsize_t raw_mem_size, picoos_objsize_t alloc_size,
        byte_ptr_t * rest_mem, picoos_objsize_t * rest_mem_size);

/**
 * Creates a new memory manager object for the specified raw memory
 * block. 'enableProtMem' enables or disables memory protection
 * functionality; if disabled, picoos_protectMem() has no effect.
 */
picoos_MemoryManager picoos_newMemoryManager(
        void *raw_memory,
        picoos_objsize_t size,
        picoos_bool enableMemProt);



void picoos_disposeMemoryManager(picoos_MemoryManager * mm);


void * picoos_allocate(picoos_MemoryManager this, picoos_objsize_t byteSize);
void picoos_deallocate(picoos_MemoryManager this, void * * adr);

/* the following memory manager routines are for testing and
   debugging purposes */

/**
 * Same as picoos_allocate, but write access to the memory block may be
 * prohibited by a subsequent call to picoos_protectMem().
 */
void *picoos_allocProtMem(picoos_MemoryManager mm, picoos_objsize_t byteSize);

/**
 * Releases a memory block previously allocated by picoos_allocProtMem().
 */
void picoos_deallocProtMem(picoos_MemoryManager mm, void **addr);

/**
 * Enables or disables write protection of a memory block previously
 * allocated by picoos_allocProtMem(). If write protection is enabled,
 * any subsequent write access will cause a segmentation fault.
 */
void picoos_protectMem(
        picoos_MemoryManager mm,
        void *addr,
        picoos_objsize_t len,
        picoos_bool enable);

void picoos_getMemUsage(
        picoos_MemoryManager this,
        picoos_bool resetIncremental,
        picoos_int32 *usedBytes,
        picoos_int32 *incrUsedBytes,
        picoos_int32 *maxUsedBytes);

void picoos_showMemUsage(
        picoos_MemoryManager this,
        picoos_bool incremental,
        picoos_bool resetIncremental);

/* *****************************************************************/
/* Exception Management                                                */
/* *****************************************************************/
/**  object   : ExceptionManager
 *   shortcut : em
 *
 */


#define PICOOS_MAX_EXC_MSG_LEN 512
#define PICOOS_MAX_WARN_MSG_LEN 64
#define PICOOS_MAX_NUM_WARNINGS 8

void picoos_setErrorMsg(picoos_char * dst, picoos_objsize_t siz,
        picoos_int16 code, picoos_char * base, const picoos_char *fmt, ...);


picoos_ExceptionManager picoos_newExceptionManager(picoos_MemoryManager mm);

void picoos_disposeExceptionManager(picoos_MemoryManager mm,
        picoos_ExceptionManager * this);


void picoos_emReset(picoos_ExceptionManager this);

/* For convenience, this function returns the resulting exception code of 'this'
 * (as would be returned by emGetExceptionCode).
 * The return value therefore is NOT the status of raising
 * the error! */
pico_status_t picoos_emRaiseException(picoos_ExceptionManager this,
        pico_status_t exceptionCode, picoos_char * baseMessage, picoos_char * fmt, ...);

pico_status_t picoos_emGetExceptionCode(picoos_ExceptionManager this);

void picoos_emGetExceptionMessage(picoos_ExceptionManager this, picoos_char * msg, picoos_uint16 maxsize);

void picoos_emRaiseWarning(picoos_ExceptionManager this,
        pico_status_t warningCode, picoos_char * baseMessage, picoos_char * fmt, ...);

picoos_uint8 picoos_emGetNumOfWarnings(picoos_ExceptionManager this);

pico_status_t picoos_emGetWarningCode(picoos_ExceptionManager this, picoos_uint8 warnNum);

void picoos_emGetWarningMessage(picoos_ExceptionManager this, picoos_uint8 warnNum, picoos_char * msg, picoos_uint16 maxsize);




/* *****************************************************************/
/* File Access                                                     */
/* *****************************************************************/

#define picoos_MaxFileNameLen 512
#define picoos_MaxKeyLen 512
#define picoos_MaxPathLen 512
#define picoos_MaxPathListLen 2048

typedef picoos_char picoos_Key[picoos_MaxKeyLen];
typedef picoos_char picoos_FileName[picoos_MaxFileNameLen];
typedef picoos_char picoos_Path[picoos_MaxPathLen];
typedef picoos_char picoos_PathList[picoos_MaxPathListLen];


/* ***** Sequential binary file access ******/

/* Remark: 'ReadByte', 'ReadBytes' and 'ReadVar' may be mixed;
 'WriteByte', 'WriteBytes' and 'WriteVar' may be mixed. */

/* Open existing binary file for read access. */
picoos_uint8 picoos_OpenBinary(picoos_Common g, picoos_File * f, picoos_char name[]);


/* Read next byte from file 'f'. */
picoos_uint8  picoos_ReadByte(picoos_File f, picoos_uint8 * by);

/* Read next 'len' bytes from 'f' into 'bytes'; 'len' returns the
 number of bytes actually read (may be smaller than requested
 length if 'bytes' is too small to hold all bytes or at end of file).
 Remark: 'bytes' is compabtible with any variable of any size. */
picoos_uint8  picoos_ReadBytes(picoos_File f, picoos_uint8 bytes[],
        picoos_uint32 * len);


/* Create new binary file.
 If 'key' is not empty, the file is encrypted with 'key'. */
picoos_uint8 picoos_CreateBinary(picoos_Common g, picoos_File * f, picoos_char name[]);

picoos_uint8  picoos_WriteByte(picoos_File f, picoos_char by);

/* Writes 'len' bytes from 'bytes' onto file 'f'; 'len' returns
 the number of bytes actually written. */
picoos_uint8  picoos_WriteBytes(picoos_File f, const picoos_char bytes[],
        picoos_int32 * len);


/* Close previously opened binary file. */
picoos_uint8 picoos_CloseBinary(picoos_Common g, picoos_File * f);






pico_status_t picoos_read_le_int16 (picoos_File file, picoos_int16 * val);
pico_status_t picoos_read_le_uint16 (picoos_File file, picoos_uint16 * val);
pico_status_t picoos_read_le_uint32 (picoos_File file, picoos_uint32 * val);


pico_status_t picoos_read_pi_uint16 (picoos_File file, picoos_uint16 * val);
pico_status_t picoos_read_pi_uint32 (picoos_File file, picoos_uint32 * val);

pico_status_t picoos_write_le_uint16 (picoos_File file, picoos_uint16 val);
pico_status_t picoos_write_le_uint32 (picoos_File file, picoos_uint32 val);

/*
pico_status_t picoos_write_pi_uint32 (picoos_File file, const picoos_uint32 val);

pico_status_t picoos_write_pi_uint16 (picoos_File file, const picoos_uint16 val);
*/


/* **************************************************************************************/
/* *** general file routines *****/

/* Returns whether end of file was encountered in previous
 read operation. */
picoos_bool picoos_Eof(picoos_File f);

/*  sets the file pointer to
 'pos' bytes from beginning (first byte = byte 0). This
 routine should only be used for binary files. */
picoos_bool  picoos_SetPos(picoos_File f, picoos_int32 pos);

/* Get position from file 'f'. */
picoos_bool picoos_GetPos(picoos_File f, picoos_uint32 * pos);

/* Returns the length of the file in bytes. */
picoos_bool picoos_FileLength(picoos_File f, picoos_uint32 * len);

/* Return full name of file 'f'. */
picoos_bool picoos_Name(picoos_File f, picoos_char name[], picoos_uint32 maxsize);

/* Returns whether file 'name' exists or not. */
picoos_bool picoos_FileExists(picoos_Common g, picoos_char name[] /*, picoos_char ckey[] */);

/* Delete a file. */
picoos_bool  picoos_Delete(picoos_char name[]);

/* Rename a file. */
picoos_bool  picoos_Rename(picoos_char oldName[], picoos_char newName[]);


/* *****************************************************************/
/* Sampled Data Files                                                    */
/* *****************************************************************/

#define SAMPLE_FREQ_16KHZ (picoos_uint32) 16000

typedef enum {
    FILE_TYPE_WAV,
    FILE_TYPE_AU,
    FILE_TYPE_RAW,
    FILE_TYPE_OTHER
} wave_file_type_t;

typedef enum {
    FORMAT_TAG_LIN = 1, /**< linear 16-bit encoding */
    FORMAT_TAG_ALAW = 6, /**< a-law encoding, 8 bit */
    FORMAT_TAG_ULAW = 7 /**< u-law encoding, 8 bit */
    /* there are many more */
} wave_format_tag_t;


typedef enum {
    /* values corresponding RIFF wFormatTag */
    PICOOS_ENC_LIN = FORMAT_TAG_LIN,  /**< linear 16-bit encoding; standard */
    PICOOS_ENC_ALAW = FORMAT_TAG_ALAW, /**< a-law encoding, 8 bit */
    PICOOS_ENC_ULAW = FORMAT_TAG_ULAW, /**< u-law encoding, 8 bit */
    /* values outside RIFF wFormatTag values (above 4100) */
    PICOOS_ENC_OTHER = 5000  /**< other; (treated as raw) */
    }  picoos_encoding_t;

typedef struct picoos_sd_file * picoos_SDFile;

/* SDFile input functions */

/* orig. comment from SDInOut.def
             Opens sampled data file 'fileName' for input and returns
             the encoding 'enc' of the file, sampling rate 'sf',
             nr of samples 'nrSamples', and a handle to the opened file
             in 'sdFile'.

             If 'fileName' is empty, the input is taken from the direct
             acoustic input using the sampling rate specified by
             "SetRawDefaults". In this case, 'encoding' returns 'EncLin',
             and 'nrSamples' returns 0.

             The file format is taken from the file name extension:
                ".wav"           --> wav file
                ".au"            --> au file
                other extensions --> headerless files

             For wav and au files, the sampling rate and encoding are taken
             from the file header and returned in 'sf' and 'enc'. For
             headerless files, 'sf' and 'enc' are taken from the
             most recently set default values (procedure SetRawDefaults).

             'done' returns whether the sampled data file was successfully
             opened. */
extern picoos_bool picoos_sdfOpenIn (picoos_Common g, picoos_SDFile * sdFile, picoos_char fileName[], picoos_uint32 * sf, picoos_encoding_t * enc, picoos_uint32 * nrSamples);


extern picoos_bool picoos_sdfGetSamples (picoos_SDFile sdFile, picoos_uint32 start, picoos_uint32 * nrSamples, picoos_int16 samples[]);


extern picoos_bool picoos_sdfCloseIn (picoos_Common g, picoos_SDFile * sdFile);


/* SDFile output functions*/

extern picoos_bool picoos_sdfOpenOut (picoos_Common g, picoos_SDFile * sdFile, picoos_char fileName[], int sf, picoos_encoding_t enc);


extern picoos_bool picoos_sdfPutSamples (picoos_SDFile sdFile, picoos_uint32 nrSamples, picoos_int16 samples[]);

/*
extern picoos_bool picoos_AbortOutput (picoos_SDFile sdFile);


extern picoos_bool picoos_ResumeOutput (picoos_SDFile sdFile);


extern picoos_bool picoos_FlushOutput (picoos_SDFile sdFile);
*/

extern picoos_bool picoos_sdfCloseOut (picoos_Common g, picoos_SDFile * sdFile);


/* *****************************************************************/
/* File Headers                                                    */
/* *****************************************************************/

#define PICOOS_MAX_FIELD_STRING_LEN 32 /* including terminating char */

#define PICOOS_MAX_NUM_HEADER_FIELDS 10
#define PICOOS_NUM_BASIC_HEADER_FIELDS 5

#define PICOOS_HEADER_NAME 0
#define PICOOS_HEADER_VERSION 1
#define PICOOS_HEADER_DATE 2
#define PICOOS_HEADER_TIME 3
#define PICOOS_HEADER_CONTENT_TYPE 4

#define PICOOS_MAX_HEADER_STRING_LEN (PICOOS_MAX_NUM_HEADER_FIELDS * (2 * PICOOS_MAX_FIELD_STRING_LEN))

typedef picoos_char picoos_field_string_t[PICOOS_MAX_FIELD_STRING_LEN];

typedef picoos_char picoos_header_string_t[PICOOS_MAX_HEADER_STRING_LEN];

typedef enum {PICOOS_FIELD_IGNORE, PICOOS_FIELD_EQUAL, PICOOS_FIELD_COMPAT} picoos_compare_op_t;

/* private */
typedef struct picoos_file_header_field {
    picoos_field_string_t key;
    picoos_field_string_t value;
    picoos_compare_op_t op;
} picoos_file_header_field_t;

/* public */
typedef struct picoos_file_header * picoos_FileHeader;
typedef struct picoos_file_header {
    picoos_uint8 numFields;
    picoos_file_header_field_t  field[PICOOS_MAX_NUM_HEADER_FIELDS];
} picoos_file_header_t;


pico_status_t picoos_clearHeader(picoos_FileHeader header);

pico_status_t picoos_setHeaderField(picoos_FileHeader header, picoos_uint8 index, picoos_char * key, picoos_char * value, picoos_compare_op_t op);

/* caller has to make sure allocated space at key and value are large enough to hold a picoos_field_string */
pico_status_t picoos_getHeaderField(picoos_FileHeader header, picoos_uint8 index, picoos_field_string_t key, picoos_field_string_t value, picoos_compare_op_t * op);

/* caller has to make sure allocated space at str is large enough to hold the header in question */
/*
pico_status_t picoos_hdrToString(picoos_FileHeader header, picoos_header_string_t str);
*/

pico_status_t picoos_hdrParseHeader(picoos_FileHeader header, picoos_header_string_t str);

pico_status_t picoos_getSVOXHeaderString(picoos_char * str, picoos_uint8 * len, picoos_uint32 maxlen);

pico_status_t picoos_readPicoHeader(picoos_File f, picoos_uint32 * headerlen);
pico_status_t picoos_skipPicoHeader(picoos_uint8 *memoryAddresss, picoos_uint32 * headerlen);



/* *****************************************************************/
/* String search and compare operations                            */
/* *****************************************************************/


picoos_uint8 picoos_has_extension(const picoos_char *str, const picoos_char *suf);

/* *****************************************************************/
/* String/Number Manipulations  (might be moved to picopal)          */
/* *****************************************************************/

pico_status_t picoos_string_to_int32(picoos_char str[],
        picoos_int32 * res);

pico_status_t picoos_string_to_uint32(picoos_char str[],
        picoos_uint32 * res);

/* 'stringlen' is the part of input string to be considered, possibly not containing NULLC (e.g. result of strlen).
 *  'maxsize' is the maximal size of 'part' including a byte for the terminating NULLC! */
void picoos_get_sep_part_str(picoos_char string[],
        picoos_int32 stringlen, picoos_int32 * ind, picoos_char sepCh,
        picoos_char part[], picoos_int32 maxsize, picoos_uint8 * done);

/* searches for the first contiguous string of printable characters (> ' ') inside fromStr, possibly skipping
 * chars <= ' ') and returns it in toStr.
 * fromStr is assumed to be NULLC terminated and toStr is forced to be NULLC terminated within maxsize.
 * The search is started at *pos inside fromStr and at return, *pos is the position within fromStr after the
 * found string, or the position after the end of fromStr, if no string was found.
 * the function returns TRUE if a string was found and fitted toStr, or FALSE otherwise. */
picoos_uint8 picoos_get_str (picoos_char * fromStr, picoos_uint32 * pos, picoos_char * toStr, picoos_objsize_t maxsize);


pico_status_t picoos_read_mem_pi_uint16 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint16 * val);

pico_status_t picoos_read_mem_pi_uint32 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint32 * val);

pico_status_t picoos_write_mem_pi_uint16 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint16 val);


/* *****************************************************************/
/* timer function          */
/* *****************************************************************/

void picoos_get_timer(picopal_uint32 * sec, picopal_uint32 * usec);

#ifdef __cplusplus
}
#endif


#endif /*PICOOS_H_*/
