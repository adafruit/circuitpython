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
 * @file picopal.h
 *
 * pico plattform abstraction layer
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

 * <b> Operating system Platform Specific implementation module </b>\n
 *
*/
#ifndef PICOPAL_H_
#define PICOPAL_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stddef.h>
#include "picopltf.h"
#include "picodefs.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* *********************************************************/
/* general defines and typedefs (used to be in picodefs.h) */
/* *********************************************************/

#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

#define NULLC '\000'


/* "strange" defines to switch variants... */
#define PICOPAL_DIV_USE_INV 0


/*---------------Externals-----------------------*/
/* used by picocep*/
#if defined(PICO_DEBUG)
    extern int numlongmult, numshortmult;
#endif


typedef signed int pico_status_t;


/* unfortunately, ANSI-C uses eof for results and exceptional results .. */
/* in the context of reading from a CharBuffer, eof means "no more
   input available FOR NOW" */

#define PICO_EOF                        (pico_status_t)    -1


/* *************************************************/
/* constants                                       */
/* *************************************************/


  /* operating system identifications */
#define PICOPAL_OS_NIL        0  /* just an unchangeable first value */
#define PICOPAL_OS_WINDOWS    1
/* ... */
#define PICOPAL_OS_GENERIC   99 /* must always be the last value */

/* *************************************************/
/* types                                           */
/* *************************************************/

typedef unsigned char   picopal_uint8;
typedef unsigned short  picopal_uint16;
typedef unsigned int    picopal_uint32;

typedef signed char     picopal_int8;
typedef signed short    picopal_int16;
typedef signed int      picopal_int32;

typedef float           picopal_single;
typedef double          picopal_double;

typedef unsigned char     picopal_char;

typedef unsigned char   picopal_uchar;

typedef size_t    picopal_objsize_t;
typedef ptrdiff_t picopal_ptrdiff_t;

/* *************************************************/
/* functions                                       */
/* *************************************************/

picopal_int32 picopal_atoi(const picopal_char *);

picopal_int32 picopal_strcmp(const picopal_char *, const picopal_char *);
picopal_int32 picopal_strncmp(const picopal_char *a, const picopal_char *b, picopal_objsize_t siz);
picopal_objsize_t picopal_strlen(const picopal_char *);
picopal_char * picopal_strchr(const picopal_char *, picopal_char);
picopal_char * picopal_strcpy(picopal_char *d, const picopal_char *s);
picopal_char *picopal_strstr(const picopal_char *s, const picopal_char *substr);
picopal_char *picopal_strcat(picopal_char *dest, const picopal_char *src);
picopal_int16 picopal_sprintf(picopal_char * dst, const picopal_char *fmt, ...);

/* copies 'length' bytes from 'src' to 'dest'. (regions may be overlapping) no error checks! */
void * picopal_mem_copy(const void * src, void * dst,  picopal_objsize_t length);

/* compares 'length' bytes from 'src' to 'dest' */
picopal_int32 picopal_mem_cmp(const void * src, void * dst,  picopal_objsize_t length);

/* sets 'length' bytes starting at dest[0] to 'byte_val' */
void * picopal_mem_set(void * dest, picopal_uint8 byte_val, picopal_objsize_t length);

/* safe versions */
picopal_objsize_t picopal_vslprintf(picopal_char * dst, picopal_objsize_t siz, const picopal_char *fmt, va_list args);
picopal_objsize_t picopal_slprintf(picopal_char * dst, picopal_objsize_t siz, const picopal_char *fmt, /*args*/ ...);
picopal_objsize_t picopal_strlcpy(picopal_char *dst, const picopal_char *src, picopal_objsize_t siz);

/*Fixed point computation*/
/*
picopal_int32 picopal_fixptdiv(picopal_int32 a, picopal_int32 b, picopal_uint8 bigpow);
picopal_int32 picopal_fixptmult(picopal_int32 x, picopal_int32 y, picopal_uint8 bigpow);
picopal_int32 picopal_fixptdivORinv(picopal_int32 a, picopal_int32 b, picopal_int32 invb, picopal_uint8 bigpow);
picopal_int32 picopal_fixptmultdouble(picopal_int32 x, picopal_int32 y, picopal_uint8 bigpow);
picopal_uint8 picopal_highestBit(picopal_int32 x);
*/

/* *************************************************/
/* math                                            */
/* *************************************************/

picopal_double picopal_cos (const picopal_double cos_arg);
picopal_double picopal_sin (const picopal_double sin_arg);
picopal_double picopal_fabs (const picopal_double fabs_arg);




/* *************************************************/
/* file access                                     */
/* *************************************************/

extern picopal_char picopal_eol(void);

#define picopal_FILE      FILE


/* seek modes to be used with the 'FSeek' procedure */
#define PICOPAL_SEEK_SET     0   /* absolut seek position */
#define PICOPAL_SEEK_CUR     1   /* relative to current */
#define PICOPAL_SEEK_END     2   /* relative to the end of the file */


typedef enum {PICOPAL_BINARY_READ, PICOPAL_BINARY_WRITE, PICOPAL_TEXT_READ, PICOPAL_TEXT_WRITE}  picopal_access_mode;

typedef picopal_FILE * picopal_File;

extern picopal_File picopal_fopen (picopal_char fileName[], picopal_access_mode mode);
/* 'FOpen' opens the file with name 'filename'. Depending on
   'mode' :
      'TextRead'    : Opens an existing text file for reading.
                      The file is positioned at the beginning of the file.
      'TextWrite'   : Opens and truncates an existing file or creates a new
                      text file for writing. The file is positioned at the
                      beginning.
      'BinaryRead'  : Opens an existing binary file for reading.
                      The file is positioned at the beginning of the file.
      'BinaryWrite' : Opens and truncates an existing file or creates a new
                      binary file for writing. The file is positioned at the
                      beginning.
    If the opening of the file is successful a file pointer is given
    back. Otherwise a NIL-File is given back.
*/


extern picopal_File picopal_get_fnil (void);


extern  picopal_int8 picopal_is_fnil (picopal_File f);


extern pico_status_t picopal_fclose (picopal_File f);


extern picopal_uint32 picopal_flength (picopal_File f);


extern  picopal_uint8 picopal_feof (picopal_File f);


extern pico_status_t picopal_fseek (picopal_File f, picopal_uint32 offset, picopal_int8 seekmode);


extern pico_status_t picopal_fget_char (picopal_File f, picopal_char * ch);


extern picopal_objsize_t picopal_fread_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj);

extern picopal_objsize_t picopal_fwrite_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj);


extern pico_status_t picopal_fflush (picopal_File f);

/*
extern pico_status_t picopal_fput_char (picopal_File f, picopal_char ch);
*/


/*
extern pico_status_t picopal_remove (picopal_char filename[]);


extern pico_status_t picopal_rename (picopal_char oldname[], picopal_char newname[]);

*/

/* *************************************************/
/* functions for debugging/testing purposes only   */
/* *************************************************/

/**
 * Returns a pointer to a newly allocated chunk of 'size' bytes, aligned
 * to the system page size.
 * Memory allocated by this routine may be protected by calling function
 * picopal_mrp_protect().
 */
void *picopal_mpr_alloc(picopal_objsize_t size);

/**
 * Releases the chunk of memory pointed to by '*p'. 'p' must be previously
 * allocated by a call to picopal_mpr_alloc().
 */
void picopal_mpr_free(void **p);

#define PICOPAL_PROT_NONE   0   /* the memory cannot be accessed at all */
#define PICOPAL_PROT_READ   1   /* the memory can be read */
#define PICOPAL_PROT_WRITE  2   /* the memory can be written to */

/**
 * Specifies the desired protection 'prot' for the memory page(s) containing
 * part or all of the interval [addr, addr+len-1]. If an access is disallowed
 * by the protection given it, the program receives a SIGSEGV.
 */
pico_status_t picopal_mpr_protect(void *addr, picopal_objsize_t len, picopal_int16 prot);

/* Fast, Compact Approximation of the Exponential Function */
picopal_double picopal_quick_exp(const picopal_double y);

/* *************************************************/
/* types functions for time measurement            */
/* *************************************************/

extern void picopal_get_timer(picopal_uint32 * sec, picopal_uint32 * usec);

#ifdef __cplusplus
}
#endif


#endif /*PICOPAL_H_*/
