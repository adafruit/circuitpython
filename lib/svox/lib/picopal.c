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
 * @file picopal.c
 *
 * pico platform abstraction layer
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

/* GCC does not supporte #pragma message */
#if !defined(__GNUC__)
#define PRAGMA_MESSAGE
#endif

#if defined(PRAGMA_MESSAGE) && defined(PICO_PLATFORM)
#pragma message("PICO_PLATFORM       : is defined externally")
#endif

#if defined(PRAGMA_MESSAGE) && defined(_WIN32)
#pragma message("_WIN32              : is defined")
#endif

#if defined(PRAGMA_MESSAGE) && defined(__APPLE__)
#pragma message("__APPLE__           : is defined")
#endif

#if defined(PRAGMA_MESSAGE) && defined(__MACH__)
#pragma message("__MACH__            : is defined")
#endif

#if defined(PRAGMA_MESSAGE) && defined(macintosh)
#pragma message("macintosh           : is defined")
#endif

#if defined(PRAGMA_MESSAGE) && defined(linux)
#pragma message("linux               : is defined")
#endif
#if defined(PRAGMA_MESSAGE) && defined(__linux__)
#pragma message("__linux__           : is defined")
#endif
#if defined(PRAGMA_MESSAGE) && defined(__linux)
#pragma message("__linux             : is defined")
#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "picodefs.h"
#include "picopal.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#if PICO_PLATFORM == PICO_Windows
/* use high-resolution timer functions on Windows platform */
#define USE_CLOCK 0
#else
/* use clock() function instead of high-resolution timer on
   non-Windows platforms */
#define USE_CLOCK 1
#endif

#include <time.h>
#if PICO_PLATFORM == PICO_Windows
#include <windows.h>
#endif

#if defined(PRAGMA_MESSAGE)
#pragma message("PICO_PLATFORM       : " PICO_PLATFORM_STRING)
#endif


picopal_int32 picopal_atoi(const picopal_char *s) {
  return (picopal_int32)atoi((const char *)s);
}

picopal_int32 picopal_strcmp(const picopal_char *a, const picopal_char *b) {
  return (picopal_int32)strcmp((const char *)a, (const char *)b);
}

picopal_int32 picopal_strncmp(const picopal_char *a, const picopal_char *b, picopal_objsize_t siz) {
  return (picopal_int32)strncmp((const char *)a, (const char *)b, (size_t) siz);
}

picopal_objsize_t picopal_strlen(const picopal_char *s) {
  return (picopal_objsize_t)strlen((const char *)s);
}

picopal_char *picopal_strchr(const picopal_char *s, picopal_char c) {
  return (picopal_char *)strchr((const char *)s, (int)c);
}

picopal_char *picopal_strstr(const picopal_char *s, const picopal_char *substr) {
  return (picopal_char *)strstr((const char *)s, (const char *)substr);
}

picopal_char *picopal_strcpy(picopal_char *d, const picopal_char *s) {
  return (picopal_char *)strcpy((char *)d, (const char *)s);
}

picopal_char *picopal_strcat(picopal_char *dest, const picopal_char *src) {
  return (picopal_char *)strcat((char *)dest, (const char *)src);
}


/* copy src into dst, but make sure that dst is not accessed beyond its size 'siz' and is allways NULLC-terminated.
 * 'siz' is the number of bytes of the destination, including one byte for NULLC!
 * returns the full length of the input string, not including termination NULLC (strlen(src)).
 * the copy is successfull without truncation if picopal_strlcpy(dst,src,siz) < siz */
picopal_objsize_t picopal_strlcpy(picopal_char *dst, const picopal_char *src, picopal_objsize_t siz)
{
        picopal_char *d = dst;
        const picopal_char *s = src;
        picopal_objsize_t n = siz;

        /* Copy as many bytes as will fit */
        if (n != 0) {
                while (--n != 0) {
                        if ((*(d++) = *(s++)) == NULLC) {
                                break;
                        }
                }
        }

        /* Not enough room in dst, add NULLC and traverse rest of src */
        if (n == 0) {
                if (siz != 0) {
                        *d = NULLC;                /* NULLC-terminate dst */
                }
                while (*(s++)) {}
                        ;
        }

        return(s - src - 1);        /* count does not include NULLC */
}


picopal_int16 picopal_sprintf(picopal_char * dst, const picopal_char *fmt, ...)
{
    picopal_int16 i;
    va_list args;

    va_start(args, fmt);
    i = (picopal_int16)vsprintf((char *) dst, (const char *)fmt, args);
    va_end(args);
    return i;
}


picopal_objsize_t picopal_vslprintf(picopal_char * dst, picopal_objsize_t siz, const picopal_char *fmt, va_list args) {
    picopal_char buf[21];
    picopal_char *d = dst;
    const picopal_char *f = fmt;
    picopal_char * b;
    picopal_objsize_t len, nnew, n = siz;
    picopal_int32 ival;
    picopal_char cval;
    picopal_objsize_t i = 0;

    if (!f) {
        f = (picopal_char *) "";
    }
    while (*f) {
        if (*f == '%') {
            switch (*(++f)) {
                case 'i':
                    f++;
                    ival = va_arg(args,int);
                    picopal_sprintf(buf,(picopal_char *)"%i",ival);
                    b = buf;
                    break;
                case 'c':
                    f++;
                    cval = va_arg(args,int);
                    picopal_sprintf(buf,(picopal_char *)"%c",cval);
                    b = buf;
                    break;
                case 's':
                    f++;
                    b = (picopal_char *) va_arg(args, char*);
                    break;
                default:
                    if (n > 0) {
                        (*d++) = '%';
                        n--;
                    }
                    i++;
                    b = NULL;
                    break;
            }
            if (b) {
                len = picopal_strlcpy(d,b,n); /* n1 is the actual length of sval */
                i += len;
                nnew = (n > len) ? n-len : 0; /* nnew is the new value of n */
                d += (n - nnew);
                n = nnew;
            }
        } else {
            if (n) {
                (*d++) = (*f);
                n--;
            }
            i++;
            f++;
        }
    }

    return i;
}

picopal_objsize_t picopal_slprintf(picopal_char * dst, picopal_objsize_t siz, const picopal_char *fmt, /*args*/ ...) {
    picopal_objsize_t i;
    va_list args;

    va_start(args, fmt);
    i = picopal_vslprintf(dst, siz, fmt, args);
    va_end(args);
    return i;

}


/* copies 'length' bytes from 'src' to 'dest'. (regions may be overlapping) no error checks! */
void * picopal_mem_copy(const void * src, void * dst, picopal_objsize_t length)
{
    return memmove(dst, src, (size_t) length);
}

/* compares 'length' bytes from 'src' to 'dest'. */
picopal_int32 picopal_mem_cmp(const void * src, void * dst,  picopal_objsize_t length)
{
    return memcmp(dst, src, (size_t) length);
}

/* sets 'length' bytes starting at dest[0] to 'byte_val' */
void * picopal_mem_set(void * dest, picopal_uint8 byte_val, picopal_objsize_t length)
{
    return memset(dest, (int) byte_val, (size_t) length);
}

/* *************************************************/
/* fixed-point math                                */
/* *************************************************/

/* *************************************************/
/* transcendent math                                */
/* *************************************************/


picopal_double picopal_cos(const picopal_double cos_arg)
{
    return (picopal_double) cos((double)cos_arg);
}
picopal_double picopal_sin(const picopal_double sin_arg)
{
    return (picopal_double) sin((double) sin_arg);
}
picopal_double picopal_fabs(const picopal_double fabs_arg)
{
    return (picopal_double) fabs((double) fabs_arg);
}


/* *************************************************/
/* file access                                     */
/* *************************************************/
#define PICOPAL_EOL '\n'

picopal_char picopal_eol(void)
{
    return PICOPAL_EOL;
}

#if PICO_PLATFORM == PICO_CIRCUITPY
/* No stdio files on CircuitPython: voices are read in place from flash with
   pico_loadMemoryResource, so every file operation fails cleanly. */
picopal_File picopal_fopen (picopal_char filename[], picopal_access_mode mode)
{
    (void)filename;
    (void)mode;
    return NULL;
}

picopal_File picopal_get_fnil (void)
{
    return NULL;
}

picopal_int8 picopal_is_fnil (picopal_File f)
{
    return NULL == f;
}

pico_status_t picopal_fflush (picopal_File f)
{
    (void)f;
    return PICO_EOF;
}

pico_status_t picopal_fclose (picopal_File f)
{
    (void)f;
    return PICO_EOF;
}

picopal_uint32 picopal_flength (picopal_File stream)
{
    (void)stream;
    return 0;
}

picopal_uint8 picopal_feof (picopal_File stream)
{
    (void)stream;
    return 1;
}

pico_status_t picopal_fseek (picopal_File f, picopal_uint32 offset, picopal_int8 seekmode)
{
    (void)f;
    (void)offset;
    (void)seekmode;
    return PICO_EOF;
}

pico_status_t picopal_fget_char (picopal_File f, picopal_char * ch)
{
    (void)f;
    (void)ch;
    return PICO_EOF;
}

picopal_objsize_t picopal_fread_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj)
{
    (void)f;
    (void)ptr;
    (void)objsize;
    (void)nobj;
    return 0;
}

picopal_objsize_t picopal_fwrite_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj)
{
    (void)f;
    (void)ptr;
    (void)objsize;
    (void)nobj;
    return 0;
}
#else
/* 'fopen' opens the file with name 'filename'. Depending on
   'mode' :
      'PICOPAL_TEXT_READ'    : Opens an existing text file for reading.
                      The file is positioned at the beginning of the file.
      'PICOPAL_TEXT_WRITE'   : Opens and truncates an existing file or creates a new
                      text file for writing. The file is positioned at the
                      beginning.
      'PICOPAL_BIN_READ'  : Opens an existing binary file for reading.
                      The file is positioned at the beginning of the file.
      'PICOPAL_BIN_WRITE' : Opens and truncates an existing file or creates a new
                      binary file for writing. The file is positioned at the
                      beginning.
    If the opening of the file is successful a file pointer is given
    back. Otherwise a NIL-File is given back.
*/
picopal_File picopal_fopen (picopal_char filename[], picopal_access_mode mode)
{
    picopal_File res;

     switch (mode) {
     case PICOPAL_TEXT_READ :
         res = (picopal_File) fopen((char *)filename, (char *)"r");
         break;
     case PICOPAL_TEXT_WRITE :
         res = (picopal_File) fopen((char *)filename, (char *)"w");
         break;
     case PICOPAL_BINARY_READ :
         res = (picopal_File) fopen((char *)filename, (char *)"rb");
         break;
     case PICOPAL_BINARY_WRITE :
         res = (picopal_File) fopen((char *)filename, (char *)"wb");
         break;
     default :
         res = (picopal_File) NULL;
     }
     return res;

}


picopal_File picopal_get_fnil (void)
{
    return (picopal_File) NULL;
}


picopal_int8 picopal_is_fnil (picopal_File f)
{
    return (NULL == f);
}

pico_status_t picopal_fflush (picopal_File f)
{
    return (0 == fflush((FILE *)f)) ? PICO_OK : PICO_EOF;
}


pico_status_t picopal_fclose (picopal_File f)
{
    return (0 == fclose((FILE *)f)) ? PICO_OK : PICO_EOF;
}



picopal_uint32 picopal_flength (picopal_File stream)
{
    fpos_t fpos;
    picopal_int32 len;

    fgetpos((FILE *)stream,&fpos);
    picopal_fseek(stream,0,SEEK_END);
    len = ftell((FILE *)stream);
    fsetpos((FILE *)stream,&fpos);
    clearerr((FILE *)stream);
    return len;

}

picopal_uint8 picopal_feof (picopal_File stream)
{
    return (0 != feof((FILE *)stream));

}

pico_status_t picopal_fseek (picopal_File f, picopal_uint32 offset, picopal_int8 seekmode)
{
    return (0 == fseek((FILE *)f, offset, seekmode)) ? PICO_OK : PICO_EOF;
}

pico_status_t picopal_fget_char (picopal_File f, picopal_char * ch)
{
    picopal_int16 res;

    res = fgetc((FILE *)f);
    if (res >= 0) {
      *ch = (picopal_char) res;
    }
    else {
      *ch = '\0';
    }
    return (res >= 0) ? PICO_OK : PICO_EOF;
}

picopal_objsize_t picopal_fread_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj)
{
    return (picopal_objsize_t) fread(ptr, objsize, nobj, (FILE *)f);
}

picopal_objsize_t picopal_fwrite_bytes (picopal_File f, void * ptr, picopal_objsize_t objsize, picopal_uint32 nobj){    return (picopal_objsize_t) fwrite(ptr, objsize, nobj, (FILE *)f);}
#endif /* PICO_PLATFORM == PICO_CIRCUITPY */
/* *************************************************/
/* functions for debugging/testing purposes only   */
/* *************************************************/

void *picopal_mpr_alloc(picopal_objsize_t size)
{
#if PICO_PLATFORM == PICO_Windows
    return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
#else
    /* not yet implemented for other platforms;  corresponding
       function on UNIX systems is pvalloc */
    return NULL;
#endif
    size = size;        /* avoid warning "var not used in this function"*/

}

void picopal_mpr_free(void **p)
{
#if PICO_PLATFORM == PICO_Windows
    VirtualFree(*p, 0, MEM_RELEASE);
#else
    /* not yet implemented for other platforms */
#endif
    *p = NULL;
}

pico_status_t picopal_mpr_protect(void *addr, picopal_objsize_t len, picopal_int16 prot)
{
    pico_status_t status = PICO_OK;

#if PICO_PLATFORM == PICO_Windows
    DWORD dwNewProtect, dwOldProtect;
    dwNewProtect = PICOPAL_PROT_NONE;
    if (prot & PICOPAL_PROT_READ) {
        if (prot & PICOPAL_PROT_WRITE) {
            dwNewProtect = PAGE_READWRITE;
        } else {
            dwNewProtect = PAGE_READONLY;
        }
    } else if (prot & PICOPAL_PROT_WRITE) {
        /* under Windows write-only is not possible */
        dwNewProtect = PAGE_READWRITE;
    }
    if (!VirtualProtect(addr, len, dwNewProtect, &dwOldProtect)) {
        status = PICO_ERR_OTHER;
    }
#else
    /* not yet implemented for other platforms */
    addr = addr;        /* avoid warning "var not used in this function"*/
    len = len;            /* avoid warning "var not used in this function"*/
    prot = prot;        /* avoid warning "var not used in this function"*/

#endif
    return status;
}
/*
 *  Reference:
 * A Fast, Compact Approximation of the Exponential Function by Nicol N. Schraudolph in Neural Computation, 11,853-862 (1999)
 * See also: http://www-h.eng.cam.ac.uk/help/tpl/programs/Matlab/mex.html
 *
 */
picopal_double picopal_quick_exp(const picopal_double y) {
    union {
        picopal_double d;
          struct {
            #if PICO_ENDIANNESS == ENDIANNESS_LITTLE /* little endian */
              picopal_int32 j,i;
            #else
              picopal_int32 i,j;
            #endif
          } n;

    } _eco;
    _eco.n.i = (picopal_int32)(1512775.3951951856938297995605697f * y) + 1072632447;
    return _eco.d;
}


/* *************************************************/
/* timer                                           */
/* *************************************************/

#if defined(IMPLEMENT_TIMER) && IMPLEMENT_TIMER

#define USEC_PER_SEC 1000000

typedef clock_t picopal_clock_t;


#if USE_CLOCK
picopal_clock_t startTime;
#else
int timerInit = 0;
LARGE_INTEGER startTime;
LARGE_INTEGER timerFreq;
#endif


picopal_clock_t picopal_clock(void)
{
    return (picopal_clock_t)clock();
}

#endif /* IMPLEMENT_TIMER */

void picopal_get_timer(picopal_uint32 * sec, picopal_uint32 * usec)
{
#if defined(IMPLEMENT_TIMER) && IMPLEMENT_TIMER
#if USE_CLOCK
    picopal_clock_t dt;
    dt = picopal_clock() - startTime;
    *sec = dt / CLOCKS_PER_SEC;
    *usec = USEC_PER_SEC * (dt % CLOCKS_PER_SEC) / CLOCKS_PER_SEC;
#else
    LARGE_INTEGER now;
    if (!timerInit) {
      QueryPerformanceFrequency(&timerFreq);
      timerInit = 1;
    }
    if (QueryPerformanceCounter(&now) && 0) {
/*
        LONGLONG dt, tf;
        dt = now.QuadPart - GLOB(startTime).QuadPart;
        tf = GLOB(timerFreq).QuadPart;
        *sec = (unsigned int) (dt / tf);
        *usec = (unsigned int) (USEC_PER_SEC * (dt % tf) / tf);
*/
        double dt, tf;
        dt = (double)(now.QuadPart - startTime.QuadPart);
        tf = (double)(timerFreq.QuadPart);
        *sec = (unsigned int) (dt /tf);
        *usec = (unsigned int) ((double)USEC_PER_SEC * (dt / tf)) % USEC_PER_SEC;
    } else {
        /* high freq counter not supported by system */
        DWORD dt;
        dt = GetTickCount() - startTime.LowPart;
        *sec = dt / 1000;
        *usec = 1000 * (dt % 1000);
    }
#endif /* USE_CLOCK */
#else
    *sec = 0;
    *usec = 0;
#endif /* IMPLEMENT_TIMER */
}

#ifdef __cplusplus
}
#endif

/* End picopal.c */
