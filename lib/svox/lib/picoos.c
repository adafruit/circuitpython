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
 * @file picoos.c
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include <stdarg.h>
#include "picodefs.h"
#include "picopal.h"
#include "picoos.h"
#include "picodbg.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


#define picoos_SVOXFileHeader (picoos_char *)" (C) SVOX AG "

/* **********************************************
 * default error and warning messages
 * **********************************************/


#define PICOOS_MSG_EXC_NUMBER_FORMAT  (picoos_char *)  "wrong number format"
#define PICOOS_MSG_EXC_MAX_NUM_EXCEED (picoos_char *)  "number exceeded"
#define PICOOS_MSG_EXC_NAME_CONFLICT  (picoos_char *)  "name conflict"
#define PICOOS_MSG_EXC_NAME_UNDEFINED (picoos_char *)  "name undefined"
#define PICOOS_MSG_EXC_NAME_ILLEGAL   (picoos_char *)  "illegal name"

/* buffer interaction */
#define PICOOS_MSG_EXC_BUF_OVERFLOW   (picoos_char *)   "buffer overflow"
#define PICOOS_MSG_EXC_BUF_UNDERFLOW  (picoos_char *)   "buffer underflow"
#define PICOOS_MSG_EXC_BUF_IGNORE     (picoos_char *)   "buffer error"

/* memory allocation */
#define PICOOS_MSG_EXC_OUT_OF_MEM     (picoos_char *)   "out of memory"

/* files */
#define PICOOS_MSG_EXC_CANT_OPEN_FILE       (picoos_char *) "cannot open file"
#define PICOOS_MSG_EXC_UNEXPECTED_FILE_TYPE (picoos_char *) "unexpected file type"
#define PICOOS_MSG_EXC_FILE_CORRUPT         (picoos_char *) "corrupt file"
#define PICOOS_MSG_EXC_FILE_NOT_FOUND       (picoos_char *) "file not found"

/* resources */
#define PICOOS_MSG_EXC_RESOURCE_BUSY         (picoos_char *)  "resource is busy"
#define PICOOS_MSG_EXC_RESOURCE_MISSING      (picoos_char *)  "cannot find resource"

/* knowledge bases */
#define PICOOS_MSG_EXC_KB_MISSING     (picoos_char *)  "knowledge base missing"

/* runtime exceptions (programming problems, usually a bug. E.g. trying to access null pointer) */
#define PICOOS_MSG_ERR_NULLPTR_ACCESS     (picoos_char *)   "access violation"
#define PICOOS_MSG_ERR_INVALID_HANDLE     (picoos_char *)   "invalid handle value"
#define PICOOS_MSG_ERR_INVALID_ARGUMENT   (picoos_char *)   "invalid argument supplied"
#define PICOOS_MSG_ERR_INDEX_OUT_OF_RANGE (picoos_char *)   "index out of range"


/* errors ("external" errors, e.g. hardware failure. Usually cannot be dealt with from inside pico.) */
#define PICOOS_MSG_ERR_OTHER         (picoos_char *) "other error"

#define PICOOS_MSG_ERR_PU            (picoos_char *) "error in processing unit"

/* WARNINGS */

/* general */
#define PICOOS_MSG_WARN_INCOMPLETE    (picoos_char *)  "incomplete output"
#define PICOOS_MSG_WARN_FALLBACK      (picoos_char *)  "using fall-back"
#define PICOOS_MSG_WARN_OTHER         (picoos_char *)  "other warning"

/* resources */
#define PICOOS_MSG_WARN_KB_OVERWRITE          (picoos_char *)  "overwriting knowledge base"
#define PICOOS_MSG_WARN_RESOURCE_DOUBLE_LOAD  (picoos_char *)  "resource already loaded"

/* decision trees */
#define PICOOS_MSG_WARN_INVECTOR        (picoos_char *)  "input vector not constructed"
#define PICOOS_MSG_WARN_CLASSIFICATION  (picoos_char *)  "output not classified"
#define PICOOS_MSG_WARN_OUTVECTOR       (picoos_char *)  "output vector not decomposed"

/* processing units */
#define PICOOS_MSG_WARN_PU_IRREG_ITEM   (picoos_char *)  "irregular item in processing unit"
#define PICOOS_MSG_WARN_PU_DISCARD_BUF  (picoos_char *)  "discarding processing unit buffer"


/* **********************************************
 * wrappers for picopal functions
 * **********************************************/

picoos_int32 picoos_atoi(const picoos_char *s)
{
    return (picoos_int32)picopal_atoi((const picoos_char *)s);
}


picoos_int8 picoos_strcmp(const picoos_char *a, const picoos_char *b)
{
    picopal_int32 res = picopal_strcmp((const picopal_char *)a,
            (const picopal_char *)b);
    return (picoos_int8) ((res < 0) ? -1 : (res > 0) ? 1 : 0);
}
picoos_int8 picoos_strncmp(const picoos_char *a, const picoos_char *b, picoos_objsize_t siz)
{
    picopal_int32 res = picopal_strncmp((const picopal_char *)a,
            (const picopal_char *)b, siz);
    return (picoos_int8) ((res < 0) ? -1 : (res > 0) ? 1 : 0);
}

picoos_uint32 picoos_strlen(const picoos_char *s)
{
    return (picoos_uint32)picopal_strlen((const picopal_char *)s);
}

picoos_char *picoos_strchr(const picoos_char *s, picoos_char c)
{
    return (picoos_char *)picopal_strchr((const picopal_char *)s,
            (picopal_char)c);
}

picoos_char *picoos_strstr(const picoos_char *s, const picoos_char * substr)
{
    return (picoos_char *)picopal_strstr((const picopal_char *)s,
            (const picopal_char *)substr);
}

picoos_int16 picoos_slprintf(picoos_char * b, picoos_uint32 bsize, const picoos_char *f, ...)
{
    picopal_int16 i;
    va_list args;

    va_start(args, f);
    i = (picoos_int16)picopal_vslprintf((picoos_char *) b, bsize, (const picoos_char *)f, args);
    va_end(args);
    return i;
}

picoos_char *picoos_strcpy(picoos_char *d, const picoos_char *s)
{
    return (picoos_char *)picopal_strcpy((picopal_char *)d,
            (const picopal_char *)s);
}

picoos_char *picoos_strcat(picoos_char *d, const picoos_char *s)
{
    return (picoos_char *)picopal_strcat((picopal_char *)d,
            (const picopal_char *)s);
}

picoos_objsize_t picoos_strlcpy(picoos_char *dst, const picoos_char *src, picoos_objsize_t siz)
{
    return (picoos_objsize_t) picopal_strlcpy((picopal_char *) dst, (const picopal_char *) src, (picopal_objsize_t) siz);
}

/* copies 'length' bytes from 'src' to 'dest'. (regions may be overlapping) no error checks! */
void * picoos_mem_copy(const void * src, void * dst,  picoos_objsize_t length)
{
    return picopal_mem_copy(src,dst,(picopal_objsize_t) length);
}

/* compares 'length' bytes from 'src' to 'dest'. */
picoos_int32 picoos_mem_cmp(const void * src, void * dst,  picoos_objsize_t length)
{
    return picopal_mem_cmp(src,dst,(picopal_objsize_t) length);
}

/* sets 'length' bytes starting at dest[0] to 'byte_val' */
void * picoos_mem_set(void * dest, picoos_uint8 byte_val, picoos_objsize_t length) {
          return picopal_mem_set(dest,(picopal_uint8)byte_val, (picopal_objsize_t)length);
}


picoos_double picoos_cos (const picoos_double cos_arg)
{
    return (picoos_double) picopal_cos ((picopal_double) cos_arg);
}


picoos_double picoos_sin (const picoos_double sin_arg)
{
    return (picoos_double) picopal_sin((picopal_double) sin_arg);
}
picoos_double picoos_fabs (const picoos_double fabs_arg)
{
    return (picoos_double) picopal_fabs((picopal_double) fabs_arg);
}

picoos_double picoos_quick_exp(const picoos_double y) {
    return (picoos_double) picopal_quick_exp ((picopal_double)y);
}


/* *****************************************************************/
/* "Common"                                                        */
/* *****************************************************************/
/* picoos_common is a collection of basic functionalities that must be globally
 * accessible from every "big" function. It includes pointers to the MemoryManasger,
 * ExceptionManager and a system-wide list of open files. */

picoos_Common picoos_newCommon(picoos_MemoryManager mm)
{
    picoos_Common this = (picoos_Common) picoos_allocate(mm,sizeof(*this));
    if (NULL != this) {
        /* initialize */
        this->em = NULL;
        this->mm = NULL;
        this->fileList = NULL;
    }
    return this;
}

void picoos_disposeCommon(picoos_MemoryManager mm, picoos_Common * this)
{
    if (NULL != (*this)) {
        /* terminate */
        picoos_deallocate(mm,(void *)this);
    }
}


/* *****************************************************************/
/* Memory Management                                               */
/* *****************************************************************/

typedef struct mem_block_hdr * MemBlockHdr;
typedef struct mem_block_hdr
{
    MemBlockHdr next;
    byte_ptr_t data;
    picoos_objsize_t size;
} mem_block_hdr_t;

typedef struct mem_cell_hdr * MemCellHdr;
typedef struct mem_cell_hdr
{
    /* size may be <0 if used */
    picoos_ptrdiff_t size;
    MemCellHdr leftCell;
    MemCellHdr prevFree, nextFree;
} mem_cell_hdr_t;

typedef struct memory_manager
{
    MemBlockHdr firstBlock, lastBlock; /* memory blockList */
    MemCellHdr freeCells, lastFree; /* free memory cells (first/last sentinel */
    /* "constants" */
    picoos_objsize_t fullCellHdrSize; /* aligned size of full cell header, including free-links */
    picoos_objsize_t usedCellHdrSize; /* aligned size of header part without free-links */
    picoos_objsize_t minContSize; /* minimum requestable application content size for allocation;
     must hold free-list info; = fullCellHdrSize-usedCellHdrSize */
    picoos_objsize_t minCellSize; /* minimum remaining cell size when a free cell is split */
    picoos_bool protMem;  /* true if memory protection is enabled */
    picoos_ptrdiff_t usedSize;
    picoos_ptrdiff_t prevUsedSize;
    picoos_ptrdiff_t maxUsedSize;
} memory_manager_t;

/** allocates 'alloc_size' bytes at start of raw memory block ('raw_mem',raw_mem_size)
 *  and returns pointer to allocated region. Returns remaining (correctly aligned) raw memory block
 *  in ('rest_mem','rest_mem_size').
 *  The allocated memory is not subject to memory management, so that it can never be freed again!
 *
 */
void * picoos_raw_malloc(byte_ptr_t raw_mem,
        picoos_objsize_t raw_mem_size, picoos_objsize_t alloc_size,
        byte_ptr_t * rest_mem, picoos_objsize_t * rest_mem_size)
{
    picoos_ptrdiff_t rest;
    if (raw_mem == NULL) {
        return NULL;
    } else {
        if (alloc_size < 1) {
            alloc_size = 1;
        }
        alloc_size = ((alloc_size + PICOOS_ALIGN_SIZE - 1) / PICOOS_ALIGN_SIZE)
                * PICOOS_ALIGN_SIZE;

        rest = raw_mem_size - alloc_size;
        if (rest < 0) {
            return NULL;
        } else {
            *rest_mem_size = rest;
            *rest_mem = raw_mem + alloc_size;
            return (void *) raw_mem;
        }
    }
}

/** initializes the last block of mm */
static int os_init_mem_block(picoos_MemoryManager this)
{
    int isFirstBlock;
    void * newBlockAddr;
    picoos_objsize_t size;
    MemCellHdr cbeg, cmid, cend;

    isFirstBlock = (this->freeCells == NULL);
    newBlockAddr = (void *) this->lastBlock->data;
    size = this->lastBlock->size;
    cbeg = (MemCellHdr) newBlockAddr;
    cmid = (MemCellHdr)((picoos_objsize_t)newBlockAddr + this->fullCellHdrSize);
    cend = (MemCellHdr)((picoos_objsize_t)newBlockAddr + size
            - this->fullCellHdrSize);
    cbeg->size = 0;

    cbeg->leftCell = NULL;
    cmid->size = size - 2 * this->fullCellHdrSize;
    cmid->leftCell = cbeg;
    cend->size = 0;
    cend->leftCell = cmid;
    if (isFirstBlock) {
        cbeg->nextFree = cmid;
        cbeg->prevFree = NULL;
        cmid->nextFree = cend;
        cmid->prevFree = cbeg;
        cend->nextFree = NULL;
        cend->prevFree = cmid;
        this->freeCells = cbeg;
        this->lastFree = cend;
    } else {
        /* add cmid to free cell list */
        cbeg->nextFree = NULL;
        cbeg->prevFree = NULL;
        cmid->nextFree = this->freeCells->nextFree;
        cmid->prevFree = this->freeCells;
        cmid->nextFree->prevFree = cmid;
        cmid->prevFree->nextFree = cmid;
        cend->nextFree = NULL;
        cbeg->prevFree = NULL;
    }
    return PICO_OK;
}


picoos_MemoryManager picoos_newMemoryManager(
        void *raw_memory,
        picoos_objsize_t size,
        picoos_bool enableMemProt)
{
    byte_ptr_t rest_mem;
    picoos_objsize_t rest_mem_size;
    picoos_MemoryManager this;
    picoos_objsize_t size2;
    mem_cell_hdr_t test_cell;

    this = picoos_raw_malloc(raw_memory, size, sizeof(memory_manager_t),
            &rest_mem, &rest_mem_size);
    if (this == NULL) {
        return NULL;
    }

    /* test if memory protection functionality is available on the current
       platform (if not, picopal_mpr_alloc() always returns NULL) */
    if (enableMemProt) {
        void *addr = picopal_mpr_alloc(100);
        if (addr == NULL) {
            enableMemProt = FALSE;
        } else {
            picopal_mpr_free(&addr);
        }
    }

    this->firstBlock = NULL;
    this->lastBlock = NULL;
    this->freeCells = NULL;
    this->lastFree = NULL;

    this->protMem = enableMemProt;
    this->usedSize = 0;
    this->prevUsedSize = 0;
    this->maxUsedSize = 0;

    /* get aligned full header size */
    this->fullCellHdrSize = ((sizeof(mem_cell_hdr_t) + PICOOS_ALIGN_SIZE - 1)
            / PICOOS_ALIGN_SIZE) * PICOOS_ALIGN_SIZE;
    /* get aligned size of header without free-list fields; the result may be compiler-dependent;
     the size is therefore computed by inspecting the end addresses of the fields 'size' and 'leftCell';
     the higher of the ending addresses is used to get the (aligned) starting address
     of the application contents */
    this->usedCellHdrSize = (picoos_objsize_t) &test_cell.size
            - (picoos_objsize_t) &test_cell + sizeof(picoos_objsize_t);
    size2 = (picoos_objsize_t) &test_cell.leftCell - (picoos_objsize_t)
            &test_cell + sizeof(MemCellHdr);
    if (size2 > this->usedCellHdrSize) {
        this->usedCellHdrSize = size2;
    }
    /* get minimum application-usable size; must be large enough to hold remainder of
     cell header (free-list links) when in free-list */
    this->minContSize = this->fullCellHdrSize - this->usedCellHdrSize;
    /* get minimum required size of a cell remaining after a cell split */
    this->minCellSize = this->fullCellHdrSize + PICOOS_ALIGN_SIZE;

    /* install remainder of raw memory block as first block */
    raw_memory = rest_mem;
    size = rest_mem_size;
    this->firstBlock = this->lastBlock = picoos_raw_malloc(raw_memory, size,
            sizeof(mem_block_hdr_t), &rest_mem, &rest_mem_size);
    if (this->lastBlock == NULL) {
        return NULL;
    }
    this->lastBlock->next = NULL;
    this->lastBlock->data = rest_mem;
    this->lastBlock->size = rest_mem_size;

    os_init_mem_block(this);

    return this;
}

void picoos_disposeMemoryManager(picoos_MemoryManager * mm)
{
    *mm = NULL;
}


/* the following memory manager routines are for testing and
   debugging purposes */


void *picoos_allocProtMem(picoos_MemoryManager mm, picoos_objsize_t byteSize)
{
    if (mm->protMem) {
        return picopal_mpr_alloc(byteSize);
    } else {
        return picoos_allocate(mm, byteSize);
    }
}


void picoos_deallocProtMem(picoos_MemoryManager mm, void **addr)
{
    if (mm->protMem) {
        picopal_mpr_free(addr);
    } else {
        picoos_deallocate(mm, addr);
    }
}


void picoos_protectMem(
        picoos_MemoryManager mm,
        void *addr,
        picoos_objsize_t len,
        picoos_bool enable)
{
    if (mm->protMem) {
        int prot = PICOPAL_PROT_READ;
        if (!enable) {
            prot |= PICOPAL_PROT_WRITE;
        }
        picopal_mpr_protect(addr, len, prot);
    } else {
        /* memory protection disabled; nothing to do */
    }
}

#define PICOPAL_PROT_NONE   0   /* the memory cannot be accessed at all */
#define PICOPAL_PROT_READ   1   /* the memory can be read */
#define PICOPAL_PROT_WRITE  2   /* the memory can be written to */

void picoos_getMemUsage(
        picoos_MemoryManager this,
        picoos_bool resetIncremental,
        picoos_int32 *usedBytes,
        picoos_int32 *incrUsedBytes,
        picoos_int32 *maxUsedBytes)
{
    *usedBytes = (picoos_int32) this->usedSize;
    *incrUsedBytes = (picoos_int32) (this->usedSize - this->prevUsedSize);
    *maxUsedBytes = (picoos_int32) this->maxUsedSize;
    if (resetIncremental) {
        this->prevUsedSize = this->usedSize;
    }
}


void picoos_showMemUsage(picoos_MemoryManager this, picoos_bool incremental,
        picoos_bool resetIncremental)
{
    picoos_int32 usedBytes, incrUsedBytes, maxUsedBytes;

    picoos_getMemUsage(this, resetIncremental, &usedBytes, &incrUsedBytes,
            &maxUsedBytes);
    if (incremental) {
        PICODBG_DEBUG(("additional memory used: %d", incrUsedBytes));
    } else {
        PICODBG_DEBUG(("memory used: %d, maximally used: %d", usedBytes, maxUsedBytes));
    }
}


void * picoos_allocate(picoos_MemoryManager this,
        picoos_objsize_t byteSize)
{

    picoos_objsize_t cellSize;
    MemCellHdr c, c2, c2r;
    void * adr;

    if (byteSize < this->minContSize) {
        byteSize = this->minContSize;
    }
    byteSize = ((byteSize + PICOOS_ALIGN_SIZE - 1) / PICOOS_ALIGN_SIZE)
            * PICOOS_ALIGN_SIZE;

    cellSize = byteSize + this->usedCellHdrSize;
    /*PICODBG_TRACE(("allocating %d", cellSize));*/
    c = this->freeCells->nextFree;
    while (
            (c != NULL) &&
            (c->size != (picoos_ptrdiff_t) cellSize) &&
            (c->size < (picoos_ptrdiff_t)(cellSize+ this->minCellSize))) {
        c = c->nextFree;
    }
    if (c == NULL) {
        return NULL;
    }
    if ((c->size == (picoos_ptrdiff_t) cellSize)) {
        c->prevFree->nextFree = c->nextFree;
        c->nextFree->prevFree = c->prevFree;
    } else {
        c2 = (MemCellHdr)((picoos_objsize_t)c + cellSize);
        c2->size = c->size - cellSize;
        c->size = cellSize;
        c2->leftCell = c;
        c2r = (MemCellHdr)((picoos_objsize_t)c2 + c2->size);
        c2r->leftCell = c2;
        c2->nextFree = c->nextFree;
        c2->nextFree->prevFree = c2;
        c2->prevFree = c->prevFree;
        c2->prevFree->nextFree = c2;
    }

    /* statistics */
    this->usedSize += cellSize;
    if (this->usedSize > this->maxUsedSize) {
        this->maxUsedSize = this->usedSize;
    }

    c->size = -(c->size);
    adr = (void *)((picoos_objsize_t)c + this->usedCellHdrSize);
    return adr;
}

void picoos_deallocate(picoos_MemoryManager this, void * * adr)
{
    MemCellHdr c;
    MemCellHdr cr;
    MemCellHdr cl;
    MemCellHdr crr;


    if ((*adr) != NULL) {
        c = (MemCellHdr)((picoos_objsize_t)(*adr) - this->usedCellHdrSize);
        c->size = -(c->size);

        /*PICODBG_TRACE(("deallocating %d", c->size));*/
        /* statistics */
        this->usedSize -= c->size;

        cr = (MemCellHdr)((picoos_objsize_t)c + c->size);
        cl = c->leftCell;
        if (cl->size > 0) {
            if (cr->size > 0) {
                crr = (MemCellHdr)((picoos_objsize_t)cr + cr->size);
                crr->leftCell = cl;
                cl->size = ((cl->size + c->size) + cr->size);
                cr->nextFree->prevFree = cr->prevFree;
                cr->prevFree->nextFree = cr->nextFree;
            } else {
                cl->size = (cl->size + c->size);
                cr->leftCell = cl;
            }
        } else {
            if ((cr->size > 0)) {
                crr = (MemCellHdr)((picoos_objsize_t)cr + cr->size);
                crr->leftCell = c;
                c->size = (c->size + cr->size);
                c->nextFree = cr->nextFree;
                c->prevFree = cr->prevFree;
                c->nextFree->prevFree = c;
                c->prevFree->nextFree = c;
            } else {
                c->nextFree = this->freeCells->nextFree;
                c->prevFree = this->freeCells;
                c->nextFree->prevFree = c;
                c->prevFree->nextFree = c;
            }
        }
    }
    *adr = NULL;
}

/* *****************************************************************/
/* Exception Management                                                */
/* *****************************************************************/
/**  object   : exceptionManager
 *   shortcut : em
 *
 */

typedef picoos_char picoos_exc_msg[PICOOS_MAX_EXC_MSG_LEN];
typedef picoos_char picoos_warn_msg[PICOOS_MAX_WARN_MSG_LEN];

typedef struct picoos_exception_manager
{
    picoos_int32 curExceptionCode;
    picoos_exc_msg curExceptionMessage;

    picoos_uint8 curNumWarnings;
    picoos_int32 curWarningCode[PICOOS_MAX_NUM_WARNINGS];
    picoos_warn_msg curWarningMessage[PICOOS_MAX_NUM_WARNINGS];

} picoos_exception_manager_t;

void picoos_emReset(picoos_ExceptionManager this)
{
    this->curExceptionCode = PICO_OK;
    this->curExceptionMessage[0] = '\0';
    this->curNumWarnings = 0;
}

picoos_ExceptionManager picoos_newExceptionManager(picoos_MemoryManager mm)
{
    picoos_ExceptionManager this = (picoos_ExceptionManager) picoos_allocate(
            mm, sizeof(*this));
    if (NULL != this) {
        /* initialize */
        picoos_emReset(this);
    }
    return this;
}

void picoos_disposeExceptionManager(picoos_MemoryManager mm,
        picoos_ExceptionManager * this)
{
    if (NULL != (*this)) {
        /* terminate */
        picoos_deallocate(mm, (void *)this);
    }
}

static void picoos_vSetErrorMsg(picoos_char * dst, picoos_objsize_t siz,
        picoos_int16 code, picoos_char * base, const picoos_char *fmt, va_list args)
{
    picoos_uint16 bsize;

    if (NULL == base) {
        switch (code) {
            case PICO_EXC_NUMBER_FORMAT:
                base = PICOOS_MSG_EXC_NUMBER_FORMAT;
                break;
            case PICO_EXC_MAX_NUM_EXCEED:
                base = PICOOS_MSG_EXC_MAX_NUM_EXCEED;
                break;
            case PICO_EXC_NAME_CONFLICT:
                base = PICOOS_MSG_EXC_NAME_CONFLICT;
                break;
            case PICO_EXC_NAME_UNDEFINED:
                base = PICOOS_MSG_EXC_NAME_UNDEFINED;
                break;
            case PICO_EXC_NAME_ILLEGAL:
                base = PICOOS_MSG_EXC_NAME_ILLEGAL;
                break;

                /* buffer interaction */
            case PICO_EXC_BUF_OVERFLOW:
                base = PICOOS_MSG_EXC_BUF_OVERFLOW;
                break;
            case PICO_EXC_BUF_UNDERFLOW:
                base = PICOOS_MSG_EXC_BUF_UNDERFLOW;
                break;
            case PICO_EXC_BUF_IGNORE:
                base = PICOOS_MSG_EXC_BUF_IGNORE;
                break;

                /* memory allocation */
            case PICO_EXC_OUT_OF_MEM:
                base = PICOOS_MSG_EXC_OUT_OF_MEM;
                break;

                /* files */
            case PICO_EXC_CANT_OPEN_FILE:
                base = PICOOS_MSG_EXC_CANT_OPEN_FILE;
                break;
            case PICO_EXC_UNEXPECTED_FILE_TYPE:
                base = PICOOS_MSG_EXC_UNEXPECTED_FILE_TYPE;
                break;
            case PICO_EXC_FILE_CORRUPT:
                base = PICOOS_MSG_EXC_FILE_CORRUPT;
                break;

            case PICO_EXC_FILE_NOT_FOUND:
                base = PICOOS_MSG_EXC_FILE_NOT_FOUND;
                break;

                /* resources */
            case PICO_EXC_RESOURCE_BUSY:
                base = PICOOS_MSG_EXC_RESOURCE_BUSY;
                break;
            case PICO_EXC_RESOURCE_MISSING:
                base = PICOOS_MSG_EXC_RESOURCE_MISSING;
                break;

                /* knowledge bases */
            case PICO_EXC_KB_MISSING:
                fmt = PICOOS_MSG_EXC_KB_MISSING;
                break;

                /* runtime exceptions (programming problems, usually a bug. E.g. trying to access null pointer) */
            case PICO_ERR_NULLPTR_ACCESS:
                base = PICOOS_MSG_ERR_NULLPTR_ACCESS;
                break;
            case PICO_ERR_INVALID_HANDLE:
                base = PICOOS_MSG_ERR_INVALID_HANDLE;
                break;
            case PICO_ERR_INVALID_ARGUMENT:
                base = PICOOS_MSG_ERR_INVALID_ARGUMENT;
                break;
            case PICO_ERR_INDEX_OUT_OF_RANGE:
                base = PICOOS_MSG_ERR_INDEX_OUT_OF_RANGE;
                break;

                /* errors ("external" errors, e.g. hardware failure. Usually cannot be dealt with from inside pico.) */
            case PICO_ERR_OTHER:
                base = PICOOS_MSG_ERR_OTHER;
                break;

                /* other error inside pu */
            case PICO_STEP_ERROR:
                base = PICOOS_MSG_ERR_PU;
                break;

                /* WARNINGS */

                /* general */
            case PICO_WARN_INCOMPLETE:
                base = PICOOS_MSG_WARN_INCOMPLETE;
                break;
            case PICO_WARN_FALLBACK:
                base = PICOOS_MSG_WARN_FALLBACK;
                break;

            case PICO_WARN_OTHER:
                base = PICOOS_MSG_WARN_OTHER;
                break;

                /* resources */
            case PICO_WARN_KB_OVERWRITE:
                base = PICOOS_MSG_WARN_KB_OVERWRITE;
                break;
            case PICO_WARN_RESOURCE_DOUBLE_LOAD:
                base = PICOOS_MSG_WARN_RESOURCE_DOUBLE_LOAD;
                break;

                /* decision trees */
            case PICO_WARN_INVECTOR:
                base = PICOOS_MSG_WARN_INVECTOR;
                break;
            case PICO_WARN_CLASSIFICATION:
                base = PICOOS_MSG_WARN_CLASSIFICATION;
                break;
            case PICO_WARN_OUTVECTOR:
                base = PICOOS_MSG_WARN_OUTVECTOR;
                break;

                /* processing units */
            case PICO_WARN_PU_IRREG_ITEM:
                base = PICOOS_MSG_WARN_PU_IRREG_ITEM;
                break;
            case PICO_WARN_PU_DISCARD_BUF:
                base = PICOOS_MSG_WARN_PU_DISCARD_BUF;
                break;

            default:
                base = (picoos_char *) "unknown error";
                break;
        }
    }
    bsize = picoos_strlcpy(dst,base,siz);
    if ((NULL != fmt) && (bsize < siz)) { /* there is something to add and more space to add it */
        if (bsize > 0) {
            dst += bsize;
            siz -= bsize;
            bsize = picoos_strlcpy(dst,(picoos_char *)": ",siz);
        }
        if (bsize < siz) {
            picopal_vslprintf((picopal_char *) dst + bsize, siz - bsize, (picopal_char *)fmt, args);
        }
    }
}

void picoos_setErrorMsg(picoos_char * dst, picoos_objsize_t siz,
        picoos_int16 code, picoos_char * base, const picoos_char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    picoos_vSetErrorMsg(dst,siz, code, base, fmt,args);
    va_end(args);
}

/* For convenience, this function returns the resulting current exception code. The return value therefore is NOT the status of raising
 * the error! */
pico_status_t picoos_emRaiseException(picoos_ExceptionManager this,
        pico_status_t exceptionCode, picoos_char * baseMessage, picoos_char * fmt, ...)
{
    va_list args;


    if (PICO_OK == this->curExceptionCode && PICO_OK != exceptionCode) {
        this->curExceptionCode = exceptionCode;
        va_start(args, fmt);
        picoos_vSetErrorMsg(this->curExceptionMessage,PICOOS_MAX_EXC_MSG_LEN, exceptionCode, baseMessage, fmt,args);
        PICODBG_DEBUG((
            "exit with exception code=%i, exception message='%s'",
            this->curExceptionCode, this->curExceptionMessage));

        va_end(args);

    }
    return this->curExceptionCode;
}

pico_status_t picoos_emGetExceptionCode(picoos_ExceptionManager this)
{
    return this->curExceptionCode;
}

void picoos_emGetExceptionMessage(picoos_ExceptionManager this, picoos_char * msg, picoos_uint16 maxsize)
{
        picoos_strlcpy(msg,this->curExceptionMessage,maxsize);
}

void picoos_emRaiseWarning(picoos_ExceptionManager this,
        pico_status_t warningCode, picoos_char * baseMessage, picoos_char * fmt, ...)
{
    va_list args;
    if ((this->curNumWarnings < PICOOS_MAX_NUM_WARNINGS) && (PICO_OK != warningCode)) {
        if (PICOOS_MAX_NUM_WARNINGS-1 == this->curNumWarnings) {
            this->curWarningCode[this->curNumWarnings] = PICO_EXC_MAX_NUM_EXCEED;
            picoos_strlcpy(this->curWarningMessage[this->curNumWarnings],(picoos_char *) "too many warnings",PICOOS_MAX_WARN_MSG_LEN);
        } else {
            this->curWarningCode[this->curNumWarnings] = warningCode;
            va_start(args, fmt);
            picoos_vSetErrorMsg(this->curWarningMessage[this->curNumWarnings],PICOOS_MAX_WARN_MSG_LEN, warningCode, baseMessage, fmt,args);
            va_end(args);
        }
        this->curNumWarnings++;
    }
    PICODBG_DEBUG((
        "exit with code=%i and message='%s', resulting in #warnings=%i",
        this->curWarningCode[this->curNumWarnings-1],
        this->curWarningMessage[this->curNumWarnings-1],
        this->curNumWarnings));
}

picoos_uint8 picoos_emGetNumOfWarnings(picoos_ExceptionManager this)
{
    return this->curNumWarnings;
}

pico_status_t picoos_emGetWarningCode(picoos_ExceptionManager this, picoos_uint8 index)
{
    if (index < this->curNumWarnings) {
      return this->curWarningCode[index];
    } else {
        return PICO_OK;
    }
}

void picoos_emGetWarningMessage(picoos_ExceptionManager this, picoos_uint8 index, picoos_char * msg, picoos_uint16 maxsize)
{
        if (index < this->curNumWarnings) {
            picoos_strlcpy(msg,this->curWarningMessage[index],maxsize);
        } else {
            msg[0] = NULLC;
        }
}




/* *****************************************************************/
/* File Access                                                     */
/* *****************************************************************/

#define picoos_MagicNumber 192837465
#define picoos_MaxBufSize 1000000
#define picoos_MaxNrOfBuffers 1000000
#define picoos_MaxBufLen 8192
#define picoos_HashFuncId0 0
#define picoos_HashTableSize0 101
#define picoos_HashTableSize1 1731
#define picoos_MaxHashTableSize HashTableSize1

#define cardinal_ptr_t picoos_uint32 *

typedef struct picoos_buffer
{
    picoos_char * data;
    int start; /* denotes the file position of the buffer beginning */
    int len; /* denotes the length of the buffer; -1 means invalid buffer */
    int pos; /* denotes the position in the buffer */
} picoos_buffer_t;

typedef picoos_buffer_t picoos_buffer_array_t[picoos_MaxNrOfBuffers];
typedef picoos_int32 picoos_buffer_index_array_t[picoos_MaxNrOfBuffers];

/**  object   : File
 *   shortcut : f
 *
 */
typedef struct picoos_file
{
    picoos_FileName name;
    picoos_uint8 binary;
    picoos_uint8 write;

    picopal_File nf;

    picoos_uint32 lFileLen;
    picoos_uint32 lPos;

    picoos_File next;
    picoos_File prev;

} picoos_file_t;

picoos_File picoos_newFile(picoos_MemoryManager mm)
{
    picoos_File this = (picoos_File) picoos_allocate(mm, sizeof(*this));
    if (NULL != this) {
        /* initialize */
    }
    return this;
}

void picoos_disposeFile(picoos_MemoryManager mm, picoos_File * this)
{
    if (NULL != (*this)) {
        /* terminate */
        picoos_deallocate(mm, (void *)this);
    }
}


/* ************************************************************
 * low-level file operations
 **************************************************************/

static picoos_int32 os_min(const picoos_int32 x, const picoos_int32 y)
{
    return (x < y) ? x : y;
}

/*
 static picoos_uint8 LReadChar (picoos_File f, picoos_char * ch);


 static picoos_uint8 LSetPos (picoos_File f, unsigned int pos);


 static picoos_uint8 LGetPos (picoos_File f, picoos_uint32 * pos);


 static picoos_uint8 LEof (picoos_File f);
 */

static picoos_bool LOpen(picoos_Common g, picoos_File * f,
        picoos_char fileName[], picopal_access_mode mode)
{
    picoos_bool done = TRUE;

    *f = picoos_newFile(g->mm);
    picopal_strcpy((*f)->name, fileName);
    (*f)->write = ((mode == PICOPAL_TEXT_WRITE) || (mode
            == PICOPAL_BINARY_WRITE));
    (*f)->binary = (mode
            == PICOPAL_BINARY_WRITE);
    (*f)->next = NULL;
    (*f)->prev = NULL;
    (*f)->nf = picopal_get_fnil();
    (*f)->lFileLen = 0;
    (*f)->lPos = 0;
    if (picopal_strlen((*f)->name)) {
       (*f)->nf = picopal_fopen((*f)->name, mode);
        done = !(picopal_is_fnil((*f)->nf));
        if (done) {
            (*f)->lFileLen = picopal_flength((*f)->nf);
        }
    }
    if (done) {
        (*f)->next = g->fileList;
        if (g->fileList != NULL) {
            g->fileList->prev = (*f);
        }
        g->fileList = (*f);
    } else {
        picoos_disposeFile(g->mm, f);
        (*f) = NULL;
    }
    return done;
}

static picoos_bool LClose(picoos_Common g, picoos_File * f)
{

    picoos_bool done;

    if (((*f) != NULL)) {
        done = (PICO_OK == picopal_fclose((*f)->nf));
        if (((*f)->next != NULL)) {
            (*f)->next->prev = (*f)->prev;
        }
        if (((*f)->prev != NULL)) {
            (*f)->prev->next = (*f)->next;
        } else {
            g->fileList = (*f)->next;
        }
        picoos_disposeFile(g->mm, f);

        done = TRUE;
    } else {
        done = FALSE;
    }
    return done;

}

/* caller must ensure that bytes[] has at least len allocated bytes */
static picoos_bool LReadBytes(picoos_File f, picoos_uint8 bytes[],
        picoos_uint32 * len)
{
    picoos_bool done;
    picoos_int32 res;

    PICODBG_TRACE(("trying to read %i bytes",*len));
    if ((f != NULL)) {
        res = picopal_fread_bytes(f->nf, (void *) &bytes[(0)], 1, (*len));
        PICODBG_TRACE(("res = %i",res));
        if (res < 0) { /* non-ansi */
            (*len) = 0;
            done = FALSE;
        } else if (((picoos_uint32)res != (*len))) {
            (*len) = res;
            done = FALSE;
        } else {
            done = TRUE;
        }
        f->lPos = (f->lPos + (*len));
    } else {
        (*len) = 0;
        done = FALSE;
    }
    return done;
}

 static picoos_bool LWriteBytes(picoos_File f, const picoos_char bytes[], int * len) {
    picoos_bool done;
    int res;
    /*int n;
    void * bptr; */

    if (f != NULL) {
        res = picopal_fwrite_bytes(f->nf, (void *) bytes, 1, *len);
        if ((res < 0)) {
            (*len) = 0;
            done = FALSE;
        } else if ((res != (*len))) {
            (*len) = res;
            done = FALSE;
        } else {
            done = TRUE;
        }
        f->lPos = (f->lPos + (unsigned int) (*len));
        if ((f->lPos > f->lFileLen)) {
            f->lFileLen = f->lPos;
        }
    } else {
        (*len) = 0;
        done = FALSE;
    }
    return done;
}


static picoos_bool LSetPos(picoos_File f, unsigned int pos)
{

    picoos_bool done;

    if ((f != NULL)) {
        if ((pos == f->lPos)) {
            done = TRUE;
        } else {
            done = (PICO_OK == picopal_fseek(f->nf, pos, PICOPAL_SEEK_SET));
            if (done) {
                f->lPos = pos;
            }
        }
    } else {
        done = FALSE;
    }
    return done;
}

static picoos_bool LGetPos(picoos_File f, picoos_uint32 * pos)
{
    picoos_bool done = TRUE;
    if ((f != NULL)) {
        (*pos) = f->lPos;
    } else {
        done = FALSE;
        (*pos) = 0;
    }
    return done;

}

static picoos_bool LEof(picoos_File f)
{
    picoos_bool isEof;

    if ((f != NULL)) {
        isEof = picopal_feof(f->nf);
    } else {
        isEof = TRUE;
    }
    return isEof;

}

/* **************************************************************************************/



/* read a given string 'str' from file. If no match was found, the read position is advanced until and including the first
 * non-matching character */
static picoos_bool picoos_StrRead (picoos_File f, picoos_char str[])
{
    picoos_uint32 i = 0;
    picoos_bool done = TRUE;
    picoos_char b;

    while (done && (str[i] != NULLC)) {
        done = done && picoos_ReadByte(f,(picoos_char *)&b);
        done = done && (b == str[i]);
        i++;
    }
    return done;
}

/* write 'str' to file */
static picoos_bool picoos_WriteStr (picoos_File f, picoos_char str[])
{
    picoos_uint32 i = 0;
    picoos_bool done = TRUE;

    while (done && (str[i] != NULLC)) {
        done = done && picoos_WriteByte(f,str[i]);
        i++;
    }
    return done;
}



/* **** Sequential binary file access ******/

/* Remark: 'ReadByte', 'ReadBytes' and 'ReadVar' may be mixed;
 'WriteByte', 'WriteBytes' and 'WriteVar' may be mixed. */

/* Open existing binary file for read access. Reading is buffered
 * with 'nrOfBufs' buffers of size 'bufSize'. If 'nrOfBufs' or
 * 'bufSize' is 0 reading is not buffered.
 * If 'key' is not empty, the file is decrypted with 'key'.
 * If the opened file is in an encrypted archive file, it
 */
picoos_uint8 picoos_OpenBinary(picoos_Common g, picoos_File * f,
        picoos_char fileName[])
{
    return LOpen(g, f, fileName, PICOPAL_BINARY_READ);
}


/* Read next byte from file 'f'. */
picoos_bool picoos_ReadByte(picoos_File f, picoos_uint8 * by)
{
    picoos_uint32 n = 1;

    return picoos_ReadBytes(f, by, &n) && (n == 1);

}

/* Read next 'len' bytes from 'f' into 'bytes'; 'len' returns the
 number of bytes actually read (may be smaller than requested
 length if at end of file). bytes[] must be big enough to hold at least len bytes.
*/
picoos_bool picoos_ReadBytes(picoos_File f, picoos_uint8 bytes[],
        picoos_uint32 * len)
{
    picoos_bool done = TRUE;
    /* unsigned int origPos; */

    if ((f != NULL)) {
        done = LReadBytes(f, bytes, len);
        /*if ((f->keyLen > 0)) {
         DecryptBytes(f->key,picoos_MaxKeyLen,f->keyLen,origPos,bytes,(*len));
         }*/
    }

    return done;
}


/* Create new binary file.
 If 'key' is not empty, the file is encrypted with 'key'. */
picoos_bool picoos_CreateBinary(picoos_Common g, picoos_File * f,
        picoos_char fileName[])
{
    return LOpen(g, f, fileName, PICOPAL_BINARY_WRITE);

}


picoos_uint8 picoos_WriteByte(picoos_File f, picoos_char by)
{
    int n = 1;

    return picoos_WriteBytes(f, (picoos_char *) &by, &n);
}


/* Writes 'len' bytes from 'bytes' onto file 'f'; 'len' returns
 the number of bytes actually written. */
picoos_bool picoos_WriteBytes(picoos_File f, const picoos_char bytes[],        picoos_int32 * len) {
    picoos_bool done = FALSE;

    if (f != NULL) {
        done = LWriteBytes(f, bytes, len);
    }

    return done;
}



/* Close previously opened binary file. */
picoos_uint8 picoos_CloseBinary(picoos_Common g, picoos_File * f)
{
    return LClose(g, f);

}

/* **************************************************************************************/
/* *** general routines *****/


/* Returns whether end of file was encountered in previous
 read operation. */
picoos_bool picoos_Eof(picoos_File f)
{
    if ((NULL != f)) {
        return LEof(f);
    } else {
        return TRUE;
    }

}

/* sets the file pointer to
 'pos' bytes from beginning (first byte = byte 0). This
 routine should only be used for binary files. */
picoos_bool picoos_SetPos(picoos_File f, picoos_int32 pos)
{
    picoos_bool done = TRUE;
    if ((NULL != f)) {
        done = LSetPos(f, pos);
    } else {
        done = FALSE;
    }
    return done;

}

/* Get position from file 'f'. */
picoos_bool picoos_GetPos(picoos_File f, picoos_uint32 * pos)
{
    if (NULL != f) {
        /* if (f->bFile) {
         (*pos) =  BGetPos(f);
         } else { */
        (*pos) =  LGetPos(f, pos);
        /* } */
        return TRUE;
    } else {
        (*pos) = 0;
        return FALSE;
    }
}

/* Returns the length of the file in bytes. */
picoos_bool picoos_FileLength(picoos_File f, picoos_uint32 * len)
{

    if (NULL != f) {
        *len = f->lFileLen;
        return TRUE;
    } else {
        *len = 0;
        return FALSE;
    }
}

/* Return full name of file 'f'. maxsize is the size of 'name[]' in bytes */
picoos_bool picoos_Name(picoos_File f, picoos_char name[], picoos_uint32 maxsize)
{
    picoos_bool done = TRUE;

    if (NULL != f) {
        done = (picoos_strlcpy(name, f->name,maxsize) < maxsize);
    } else {
        name[0] = (picoos_char)NULLC;
        done = FALSE;
    }

    return done;
}

/* Returns whether file 'name' exists or not. */
picoos_bool picoos_FileExists(picoos_Common g, picoos_char name[])
{
    picoos_File f;

    if (picoos_OpenBinary(g, & f,name)) {
        picoos_CloseBinary(g, & f);
        return TRUE;
    } else {
        return FALSE;
    }
}


/* ******************************************************************/
/* Array conversion operations: all procedures convert 'nrElems' values from
   'src' starting with index 'srcStartInd' into corresponding (possibly
   rounded) values in 'dst' starting at 'dstStartInd'. */

/* taking pi to be le, these are just the array versions of read_mem_pi_*int16 */
typedef picoos_uint8 two_byte_t[2];

static void arr_conv_le_int16 (picoos_uint8 src[], picoos_uint32 srcShortStartInd, picoos_uint32 nrElems, picoos_int16 dst[], picoos_uint32 dstStartInd)
{
    two_byte_t * src_p = (two_byte_t *) (src + (srcShortStartInd * 2));
    picoos_int16 * dst_p = dst + dstStartInd;
    picoos_uint32 i;

    for (i=0; i<nrElems; i++) {
        *(dst_p++) = (*src_p)[0] + (((*src_p)[1] & 0x7F) << 8) - (((*src_p)[1] & 0x80) ? 0x8000 : 0);
        src_p++;
    }
}



/* convert array of int16 into little-endian format */
static void arr_conv_int16_le (picoos_int16 src[], picoos_uint32 srcStartInd, picoos_uint32 nrElems, picoos_uint8 dst[], picoos_uint32 dstShortStartInd)
{
    two_byte_t * dst_p = (two_byte_t *) (dst + (dstShortStartInd * 2));
    picoos_int16 * src_p = src + srcStartInd;
    picoos_uint32 i;
    picoos_uint16 val;

    for (i=0; i<nrElems; i++) {
        val = (picoos_uint16) *(src_p++);
        (*dst_p)[0] = (picoos_uint8)(val & 0x00FF);
        (*dst_p)[1] = (picoos_uint8)((val & 0xFF00)>>8);
        dst_p++;
    }
}

/* *****************************************************************/
/* Sampled Data Files                                                    */
/* *****************************************************************/

#define PICOOS_SDF_BUF_LEN 1024

#define PICOOS_INT16_MIN   -32768
#define PICOOS_INT16_MAX   32767
#define PICOOS_UINT16_MAX  0xffff
#define PICOOS_INT32_MIN   -2147483648
#define PICOOS_INT32_MAX   2147483647
#define PICOOS_UINT32_MAX  0xffffffff

/**  object   : SDFile
 *   shortcut : sdf
 *
 */
/* typedef struct picoos_sd_file * picoos_SDFile */
typedef struct picoos_sd_file
{
    picoos_uint32 sf;
    wave_file_type_t fileType; /* (acoustic) wav, au, raw, other */
    picoos_uint32 hdrSize;
    picoos_encoding_t enc;
    picoos_File file;
    picoos_uint32 nrFileSamples;
    picoos_int16 buf[PICOOS_SDF_BUF_LEN];
    picoos_int32 bufPos;
    picoos_uint8 bBuf[2*PICOOS_SDF_BUF_LEN];
    picoos_bool aborted;
} picoos_sd_file_t;


/* Tries to read wav header at beginning of file 'f';
   returns sampling rate 'sf', encoding type 'enc',
   nr of samples in file 'nrSamples', header size 'hdrSize',
   and byte order 'bOrder'; returns whether a supported
   wav header and format was found. */
static picoos_bool picoos_readWavHeader(picoos_File f, picoos_uint32 * sf,
        picoos_encoding_t * enc, picoos_uint32 * nrSamples,
        picoos_uint32 * hdrSize) {
    picoos_uint16 n16;
    picoos_uint32 n32;
    picoos_uint16 formatTag;
    picoos_uint32 sampleRate;
    picoos_uint32 bytesPerSec;
    picoos_uint16 blockAlign;
    picoos_uint16 sampleSize;
    picoos_uint32 dataLength;
    picoos_uint32 fileLen;
    picoos_uint32 nrFileSamples;
    picoos_bool done;


    picoos_SetPos(f, 0);
    picoos_FileLength(f, &fileLen);
    done = picoos_StrRead(f, (picoos_char *) "RIFF");
    done = done && (PICO_OK == picoos_read_le_uint32(f, &n32)); /* length of riff chunk, unused */
    done = done && picoos_StrRead(f, (picoos_char *) "WAVE");
    done = done && picoos_StrRead(f, (picoos_char *) "fmt ");
    done = done && (PICO_OK == picoos_read_le_uint32(f, &n32)); /* length of fmt chunk in bytes; must be 16 */
    done = done && (n32 == 16);
    done = done && (PICO_OK == picoos_read_le_uint16(f, &formatTag));
    done = done && (PICO_OK == picoos_read_le_uint16(f, &n16)); /* number of channels; must be mono */
    done = done && (n16 == 1);
    done = done && (PICO_OK == picoos_read_le_uint32(f, &sampleRate));
    done = done && (PICO_OK == picoos_read_le_uint32(f, &bytesPerSec));
    done = done && (PICO_OK == picoos_read_le_uint16(f, &blockAlign));
    done = done && (PICO_OK == picoos_read_le_uint16(f, &sampleSize));
    done = done && picoos_StrRead(f, (picoos_char *) "data");
    done = done && (PICO_OK == picoos_read_le_uint32(f, &dataLength)); /* length of data chunk in bytes */
    (*hdrSize) = 44;
    if (done) {
        (*sf) = sampleRate;
        (*nrSamples) = 0;
        switch (formatTag) {
        case FORMAT_TAG_LIN:
            (*enc) = PICOOS_ENC_LIN;
            done = ((blockAlign == 2) && (sampleSize == 16));
            (*nrSamples) = (dataLength / 2);
            nrFileSamples = ((fileLen - (*hdrSize)) / 2);
            break;
        case FORMAT_TAG_ULAW:
            (*enc) = PICOOS_ENC_ULAW;
            done = ((blockAlign == 1) && (sampleSize == 8));
            (*nrSamples) = dataLength;
            nrFileSamples = (fileLen - (*hdrSize));
            break;
        case FORMAT_TAG_ALAW:
            (*enc) = PICOOS_ENC_ALAW;
            done = ((blockAlign == 1) && (sampleSize == 8));
            (*nrSamples) = dataLength;
            nrFileSamples = (fileLen - (*hdrSize));
            break;
        default:
            done = FALSE;
            break;
        }
        if (!done) {
            /* communicate "unsupported format" */
            PICODBG_WARN(("unsupported wav format"));
        } else {
            if (nrFileSamples != (*nrSamples)) {
                /* warn "inconsistent number of samples" */
                PICODBG_WARN(("inconsistent number of samples in wav file: %d vs. %d",nrFileSamples,(*nrSamples)));
                (*nrSamples) = nrFileSamples;
            }
        }
    }
    return done;
}



extern picoos_bool picoos_sdfOpenIn(picoos_Common g, picoos_SDFile * sdFile,
        picoos_char fileName[], picoos_uint32 * sf, picoos_encoding_t * enc,
        picoos_uint32 * numSamples)
{
    picoos_bool done = FALSE;
    picoos_sd_file_t * sdf = NULL;
    wave_file_type_t fileType = FILE_TYPE_OTHER;

    (*sf) = 0;
    (*numSamples) = 0;
    (*enc) = PICOOS_ENC_LIN;
    (*sdFile) = NULL;

    sdf = picoos_allocate(g->mm,sizeof(picoos_sd_file_t));
    if (NULL == sdf) {
        picoos_emRaiseWarning(g->em,PICO_EXC_OUT_OF_MEM,NULL,NULL);
        return FALSE;
    }

    /* buffered access not supported, yet */
    if (picoos_OpenBinary(g,&(sdf->file),fileName)) {
        if (picoos_has_extension(fileName,(picoos_char *) ".wav")) {
            fileType = FILE_TYPE_WAV;
            done = picoos_readWavHeader(sdf->file,&(sdf->sf),&(sdf->enc),&(sdf->nrFileSamples),&(sdf->hdrSize));
        } else {
            /* we prefer not to treat other formats, rather than treat it as raw */
            /* fileType = FILE_TYPE_RAW; */
            fileType = FILE_TYPE_OTHER;
            done = FALSE;
        }

        if (FILE_TYPE_OTHER == fileType) {
            picoos_emRaiseWarning(g->em,PICO_EXC_UNEXPECTED_FILE_TYPE,(picoos_char *)"unsupported filename suffix",NULL);
        } else if (!done) {
            picoos_emRaiseWarning(g->em,PICO_EXC_UNEXPECTED_FILE_TYPE,(picoos_char *)"non-conforming header",NULL);
        } else {
            (*numSamples) = sdf->nrFileSamples;
            (*sf) = sdf->sf;
            (*enc) = sdf->enc;
            /* check whether sd file properties are supported */
            if (PICOOS_ENC_LIN != sdf->enc) {
                done = FALSE;
                picoos_emRaiseWarning(g->em,PICO_EXC_UNEXPECTED_FILE_TYPE,NULL,(picoos_char *)"encoding not supported");
            }
            if (SAMPLE_FREQ_16KHZ != sdf->sf) {
                done = FALSE;
                picoos_emRaiseWarning(g->em,PICO_EXC_UNEXPECTED_FILE_TYPE,NULL,(picoos_char *)"sample frequency not supported");
            }
            (*sdFile) = sdf;
        }
        if (!done){
            picoos_CloseBinary(g,&(sdf->file));
        }
    } else {
        picoos_emRaiseException(g->em,PICO_EXC_CANT_OPEN_FILE,NULL,NULL);
    }
    if (!done) {
        picoos_deallocate(g->mm,(void *)&sdf);
        (*sdFile) = NULL;
    }
    return done;
}


static void picoos_sdfLoadSamples(picoos_SDFile sdFile,
        picoos_uint32 * nrSamples) {
    picoos_uint32 len;
    picoos_sd_file_t * sdf = sdFile;

    switch (sdFile->enc) {
    case PICOOS_ENC_LIN:
        if ((*nrSamples) > PICOOS_SDF_BUF_LEN) {
            (*nrSamples) = PICOOS_SDF_BUF_LEN;
        }
        len = 2 * (*nrSamples);
        picoos_ReadBytes(sdf->file, sdf->bBuf, &len);
        (*nrSamples) = len / 2;
        arr_conv_le_int16(sdf->bBuf, 0, (*nrSamples), sdf->buf, 0);
        break;
        /* @todo : may be useful */
    case PICOOS_ENC_ULAW:
    case PICOOS_ENC_ALAW:
    default:
        (*nrSamples) = 0;
    }

}

extern picoos_bool picoos_sdfGetSamples (
        picoos_SDFile sdFile,
        picoos_uint32 start,
        picoos_uint32 * nrSamples,
        picoos_int16 samples[])
{
    picoos_uint32 b;
    picoos_uint32 rem;
    picoos_uint32 n;
    picoos_uint32 i;
    picoos_uint32 j;
    picoos_bool done = FALSE;

    if (NULL == sdFile) {
        (*nrSamples) = 0;
    } else {
            if (start >= sdFile->nrFileSamples) {
                if (start > sdFile->nrFileSamples) {
                    PICODBG_WARN(("start has to be <= sdFile->nrFileSamples"));
                }
                (*nrSamples) = 0;
            } else {
                if (((start + (*nrSamples)) > sdFile->nrFileSamples)) {
                    (*nrSamples) = (sdFile->nrFileSamples - start);
                }
                if ((sdFile->enc == PICOOS_ENC_LIN)) {
                    b = 2;
                } else {
                    b = 1;
                }
                picoos_SetPos(sdFile->file,(sdFile->hdrSize + (b * start)));
                j = 0;
                rem = (*nrSamples);
                n = rem;
                while ((rem > 0) && (n > 0)) {
                    /* set n=min(rem,buffer_length) and try loading next n samples */
                    n = (rem < PICOOS_SDF_BUF_LEN) ? rem : PICOOS_SDF_BUF_LEN;
                    picoos_sdfLoadSamples(sdFile, &n);
                    /* n may be smaller now */
                    for (i = 0; i < n; i++) {
                        samples[j] = sdFile->buf[i];
                        j++;
                    }
                    rem -= n;
                    start += n;
                }
                (*nrSamples) = j;
                done = ((*nrSamples) > 0);
            }
    }
    return done;
}


extern picoos_bool picoos_sdfCloseIn (picoos_Common g, picoos_SDFile * sdFile)
{
    if (NULL != (*sdFile)) {
        picoos_CloseBinary(g,&((*sdFile)->file));
        picoos_deallocate(g->mm,(void *)sdFile);
    }
    return TRUE;
}


static picoos_bool picoos_writeWavHeader(picoos_File f, picoos_uint32 sf,
        picoos_encoding_t enc, picoos_uint32 nrSamples,
        picoos_uint32 * hdrSize) {
    picoos_uint16 formatTag = FORMAT_TAG_LIN;
    picoos_uint32 sampleRate;
    picoos_uint32 bytesPerSec;
    picoos_uint32 bytesPerSample = 2;
    picoos_uint16 blockAlign;
    picoos_uint16 sampleSize = 16;
    picoos_uint32 dataLength;
    picoos_bool done = TRUE;

    picoos_SetPos(f, 0);

    switch (enc) {
        case PICOOS_ENC_LIN:
            formatTag = FORMAT_TAG_LIN;
            bytesPerSample = 2;
            sampleSize = 16;
            break;
        case PICOOS_ENC_ULAW:
            formatTag = FORMAT_TAG_ULAW;
            bytesPerSample = 1;
            sampleSize = 8;
            break;
        case PICOOS_ENC_ALAW:
            formatTag = FORMAT_TAG_ALAW;
            bytesPerSample = 1;
            sampleSize = 8;
            break;
        default:
            done = FALSE;
            break;
    }

    bytesPerSec = (sf * bytesPerSample);
    blockAlign = bytesPerSample;
    sampleRate = sf;
    dataLength = (bytesPerSample * nrSamples);
    done = done && picoos_WriteStr(f,(picoos_char *)"RIFF");
    done = done && picoos_write_le_uint32(f,dataLength + 36);
    done = done && picoos_WriteStr(f,(picoos_char *)"WAVE");
    done = done && picoos_WriteStr(f,(picoos_char *)"fmt ");
    done = done && picoos_write_le_uint32(f,16);
    done = done && picoos_write_le_uint16(f,formatTag);
    done = done && picoos_write_le_uint16(f,1);
    done = done && picoos_write_le_uint32(f,sampleRate);
    done = done && picoos_write_le_uint32(f,bytesPerSec);
    done = done && picoos_write_le_uint16(f,blockAlign);
    done = done && picoos_write_le_uint16(f,sampleSize);
    done = done && picoos_WriteStr(f,(picoos_char *)"data");
    done = done && picoos_write_le_uint32(f,dataLength);
    (*hdrSize) = 44;
    return done;
}


#define DummyLen 100000000

extern picoos_bool picoos_sdfOpenOut(picoos_Common g, picoos_SDFile * sdFile,
        picoos_char fileName[], int sf, picoos_encoding_t enc)
{
    picoos_bool done = TRUE;
    picoos_sd_file_t * sdf = NULL;

    (*sdFile) = NULL;
    sdf = picoos_allocate(g->mm, sizeof(picoos_sd_file_t));
    if (NULL == sdf) {
        picoos_emRaiseWarning(g->em, PICO_EXC_OUT_OF_MEM, NULL, NULL);
        return FALSE;
    }
    sdf->sf = sf;
    sdf->enc = enc;
    /* check whether sd file properties are supported */
    if (PICOOS_ENC_LIN != sdf->enc) {
        done = FALSE;
        picoos_emRaiseWarning(g->em, PICO_EXC_UNEXPECTED_FILE_TYPE, NULL,
                (picoos_char *) "encoding not supported");
    }
    if (SAMPLE_FREQ_16KHZ != sdf->sf) {
        done = FALSE;
        picoos_emRaiseWarning(g->em, PICO_EXC_UNEXPECTED_FILE_TYPE, NULL,
                (picoos_char *) "sample frequency not supported");
    }
    if (done) {
        sdf->nrFileSamples = 0;
        sdf->bufPos = 0;
        sdf->aborted = FALSE;
        if (picoos_CreateBinary(g, &(sdf->file), fileName)) {
            if (picoos_has_extension(fileName, (picoos_char *) ".wav")) {
                sdf->fileType = FILE_TYPE_WAV;
                done = picoos_writeWavHeader(sdf->file, sdf->sf, sdf->enc,
                        DummyLen, &(sdf->hdrSize));
            } else {
                /* we prefer not to treat other formats, rather than treat it as raw */
                /* fileType = FILE_TYPE_RAW; */
                sdf->fileType = FILE_TYPE_OTHER;
                done = FALSE;
            }

            if (FILE_TYPE_OTHER == sdf->fileType) {
                picoos_emRaiseWarning(g->em, PICO_EXC_UNEXPECTED_FILE_TYPE,
                        (picoos_char *) "unsupported filename suffix", NULL);
            } else if (!done) {
                picoos_emRaiseWarning(g->em, PICO_EXC_UNEXPECTED_FILE_TYPE,
                        (picoos_char *) "non-conforming header", NULL);
            } else {
                (*sdFile) = sdf;
            }
            if (!done) {
                picoos_CloseBinary(g, &(sdf->file));
            }
        } else {
            picoos_emRaiseException(g->em, PICO_EXC_CANT_OPEN_FILE, NULL, NULL);
        }
    }
    if (!done) {
        picoos_deallocate(g->mm, (void *) &sdf);
        (*sdFile) = NULL;
    }
    return done;
}

static picoos_bool picoos_sdfFlushOutBuf(picoos_SDFile sdFile)
{
    picoos_bool done = FALSE;
    picoos_int32 len;
    picoos_int32 nrSamples;

    if (!(sdFile->aborted)) {
        nrSamples = sdFile->bufPos;
        switch (sdFile->enc) {
            case PICOOS_ENC_LIN:
                arr_conv_int16_le(sdFile->buf, 0, nrSamples, sdFile->bBuf, 0);
                len = (nrSamples * 2);
                done = picoos_WriteBytes(sdFile->file, sdFile->bBuf, &len)
                        && ((nrSamples * 2) == len);
                break;
            case PICOOS_ENC_ULAW:
            case PICOOS_ENC_ALAW:
            default:
                nrSamples = 0;
                break;
        }
        sdFile->nrFileSamples = (sdFile->nrFileSamples + nrSamples);
    }

    sdFile->bufPos = 0;
    return done;
}

extern picoos_bool picoos_sdfFlushOutput(picoos_SDFile sdFile)
{
    if ((sdFile != NULL) &&  !(sdFile->aborted) && (sdFile->bufPos > 0)) {
        return picoos_sdfFlushOutBuf(sdFile);
    }
    return TRUE;
}



extern picoos_bool picoos_sdfPutSamples (picoos_SDFile sdFile, picoos_uint32 nrSamples, picoos_int16 samples[])
{
    picoos_uint32 i;
    picoos_int32 s;
    picoos_bool done = FALSE;

    if ((sdFile != NULL) &&  !(sdFile->aborted)) {
        done = TRUE;
        for (i = 0; i < nrSamples; i++) {
            s = samples[i];
            if ((s > PICOOS_INT16_MAX)) {
                s = PICOOS_INT16_MAX;
            } else if (s < PICOOS_INT16_MIN) {
                s = PICOOS_INT16_MIN;
            }
            sdFile->buf[sdFile->bufPos++] = s;
            if (sdFile->bufPos >= PICOOS_SDF_BUF_LEN) {
                done = picoos_sdfFlushOutBuf(sdFile);
            }
        }
    } else {
        done = FALSE;
    }
    return done;
}


extern picoos_bool picoos_sdfCloseOut (picoos_Common g, picoos_SDFile * sdFile)
{

    picoos_bool done = TRUE;
    picoos_uint32 hdrSize;

    if (NULL != (*sdFile)) {
        if (!((*sdFile)->aborted) && ((*sdFile)->bufPos > 0)) {
            done = picoos_sdfFlushOutBuf(*sdFile);
        }
        if (FILE_TYPE_WAV == (*sdFile)->fileType) {
            done = picoos_writeWavHeader((*sdFile)->file, (*sdFile)->sf,
                    (*sdFile)->enc, (*sdFile)->nrFileSamples, &hdrSize);
        }
        done = picoos_CloseBinary(g, &((*sdFile)->file));
        picoos_deallocate(g->mm, (void *) sdFile);
    }
    return done;
}


/* *****************************************************************/
/* FileHeader                                                      */
/* *****************************************************************/



pico_status_t picoos_clearHeader(picoos_FileHeader header)
{
    picoos_uint8 i;
    for (i=0; i < PICOOS_MAX_NUM_HEADER_FIELDS; i++) {
        header->field[i].key[0] = NULLC;
        header->field[i].value[0] = NULLC;
        header->field[i].op = PICOOS_FIELD_IGNORE;
    }
    header->numFields = 0;
    return PICO_OK;
}

pico_status_t picoos_setHeaderField(picoos_FileHeader header,
        picoos_uint8 index, picoos_char * key, picoos_char * value,
        picoos_compare_op_t op)
{
    if (index >= header->numFields) {
        return PICO_ERR_INDEX_OUT_OF_RANGE;
    }
    header->field[index].op = op;
    if ((picoos_strlcpy(header->field[index].key, key,
            PICOOS_MAX_FIELD_STRING_LEN) < PICOOS_MAX_FIELD_STRING_LEN)
            && (picoos_strlcpy(header->field[index].value, value,
                    PICOOS_MAX_FIELD_STRING_LEN)) < PICOOS_MAX_FIELD_STRING_LEN) {
        return PICO_OK;
    } else {
        return PICO_ERR_INDEX_OUT_OF_RANGE;
    }
}


/* caller has to make sure allocated space at key and value are large enough to hold a picoos_field_string */
pico_status_t picoos_getHeaderField(picoos_FileHeader header, picoos_uint8 index, picoos_field_string_t key, picoos_field_string_t value, picoos_compare_op_t * op)
{
    if (index >= header->numFields) {
        return PICO_ERR_INDEX_OUT_OF_RANGE;
    }
    *op = header->field[index].op;
    if ((picoos_strlcpy(key,header->field[index].key,
            PICOOS_MAX_FIELD_STRING_LEN) < PICOOS_MAX_FIELD_STRING_LEN)
            && (picoos_strlcpy(value,header->field[index].value,
                    PICOOS_MAX_FIELD_STRING_LEN)) < PICOOS_MAX_FIELD_STRING_LEN) {
        return PICO_OK;
    } else {
        return PICO_ERR_INDEX_OUT_OF_RANGE;
    }
    return PICO_OK;
}


/* check whether 'str' of length strlen matches contents in circular buffer buf, located in the first strlen bytes and ending
 * position bufpos. */
static picoos_uint8 os_matched( picoos_char * str,  picoos_uint32 strlen, picoos_char * buf,  picoos_int32 bufpos) {
    picoos_int32 i = strlen-1;
    while (i >= 0 && buf[bufpos] == str[i]) {
        i--;
        bufpos--;
        if (bufpos < 0) {
            bufpos = strlen-1;
        }
    }
    return (i<0);
}

pico_status_t picoos_getSVOXHeaderString(picoos_char * str, picoos_uint8 * len, picoos_uint32 maxlen)
{
    picoos_char * ch;
    *len = picoos_strlcpy(str,picoos_SVOXFileHeader,maxlen);
    if (*len < maxlen) {
        ch = str;
        /* SVOX header is made less readable */
        while (*ch) {
            *ch -= ' ';
            ch++;
        }
        return PICO_OK;
    } else {
        return PICO_ERR_OTHER;
    }
}

pico_status_t picoos_readPicoHeader(picoos_File f, picoos_uint32 * headerlen)
{
    picoos_char str[32];
    picoos_char buf[32];
    picoos_uint8 strlen, bufpos;
    picoos_uint32 n;
    picoos_uint8 done;

    picoos_getSVOXHeaderString(str,&strlen,32);
    /* search for svox header somewhere near the file start. This allows for initial
     * non-svox-header bytes for a customer-specific header and/or filling bytes for alignment */
    *headerlen = 0;
    /* read in initial chunk of length strlen */
    n = strlen;
    done = picoos_ReadBytes(f,(picoos_uint8 *)buf,&n) && (n == strlen);
    if (done) {
        *headerlen = n;
        bufpos = strlen-1; /* last legal buf position */
        done = os_matched(str,strlen,buf,bufpos);
        while (!done && *headerlen < PICO_MAX_FOREIGN_HEADER_LEN) {
            n = 1;
            bufpos = (bufpos + 1) % strlen;
            done = picoos_ReadBytes(f,(picoos_uint8 *)buf+bufpos,&n) && 1 == n;
            done = done && os_matched(str,strlen,buf,bufpos);
            headerlen++;
        }
    }
    if (done) {
        return PICO_OK;
    } else {
        return PICO_EXC_UNEXPECTED_FILE_TYPE;
    }
}

pico_status_t picoos_skipPicoHeader(picoos_uint8 * memoryAddress, picoos_uint32 * headerlen)
{
    picoos_char str[32];
    picoos_uint8 strlen;
    picoos_uint8 done;

    picoos_getSVOXHeaderString(str,&strlen,32);
    /* search for svox header somewhere near the start. This allows for initial
     * non-svox-header bytes for a customer-specific header and/or filling bytes for alignment */
    *headerlen = strlen;
    done = (picopal_mem_cmp(str,memoryAddress,strlen) == 0);
    if (!done) {
        memoryAddress += strlen;
        while (!done && *headerlen < PICO_MAX_FOREIGN_HEADER_LEN) {
            done = (picopal_mem_cmp(str,memoryAddress,strlen) == 0);
            memoryAddress++;
            (*headerlen)++;
        }
    }
    if (done) {
        return PICO_OK;
    } else {
        return PICO_EXC_UNEXPECTED_FILE_TYPE;
    }
}


picoos_uint8 picoos_get_str (picoos_char * fromStr, picoos_uint32 * pos, picoos_char * toStr, picoos_objsize_t maxsize)
{
    picoos_uint8 i = 0;
    /* skip non-printables */

    while ((fromStr[*pos] != NULLC) && (fromStr[*pos] <= ' ')) {
        (*pos)++;
    }
    /* copy printable portion */
    while ((fromStr[*pos] != NULLC) && (fromStr[*pos] > ' ') && (i < maxsize-1)) {
        toStr[i++] = fromStr[(*pos)++];
    }
    toStr[i] = NULLC;
    return (i > 0) && (fromStr[*pos] <= ' ');
}

pico_status_t picoos_hdrParseHeader(picoos_FileHeader header, picoos_header_string_t str)
{
    picoos_uint32 curpos = 0;
    picoos_uint8 i, numFields;


    /* read number of fields */
    numFields = str[curpos++];
    numFields = os_min(numFields,PICOOS_MAX_NUM_HEADER_FIELDS);
    /* read in all field pairs */
    PICODBG_DEBUG(("number of fields = %i", numFields));
    for (i = 0; i < numFields; i++) {
        picoos_get_str(str,&curpos,header->field[i].key,PICOOS_MAX_FIELD_STRING_LEN);
        picoos_get_str(str,&curpos,header->field[i].value,PICOOS_MAX_FIELD_STRING_LEN);
    }
    return PICO_OK;
}





/* **************************************************************************/
/* Read  little-endian / platform-independent integers from file or memory  */
/* **************************************************************************/

/* read little-endian */
pico_status_t picoos_read_le_uint16 (picoos_File file, picoos_uint16 * val)
{
    picoos_uint8 by[2];
    picoos_uint32 n = 2;
    if (picoos_ReadBytes(file, by, &n) && 2 == n) {
        /* little-endian */
        *val = (picoos_uint16) by[1] << 8 | (picoos_uint16) by[0];
        return PICO_OK;
    } else {
        *val = 0;
        return PICO_ERR_OTHER;
    }
}

pico_status_t picoos_read_le_int16 (picoos_File file, picoos_int16 * val)
{
    return picoos_read_le_uint16(file, (picoos_uint16 *)val);
}

pico_status_t picoos_read_le_uint32 (picoos_File file, picoos_uint32 * val)
{
    picoos_uint8 by[4];
    picoos_uint32 n = 4;
    if (picoos_ReadBytes(file, by, &n) && (4 == n)) {
        /* little-endian */
        PICODBG_TRACE(("reading uint 32:  %i %i %i %i",
                       by[0], by[1], by[2], by[3]));
        *val = (((picoos_uint32) by[3] << 8 | (picoos_uint32) by[2]) << 8 | (picoos_uint32) by[1]) << 8 | (picoos_uint32) by[0];
        PICODBG_TRACE(("uint 32:  %i %i %i %i corresponds %i",
                       by[0], by[1], by[2], by[3], *val));
        return PICO_OK;
    } else {
        *val = 0;
        return PICO_ERR_OTHER;
    }
}

/* platform-independent */
/* our convention is that pi is little-endian. */

/** @todo : direct implementation if too slow */

pico_status_t picoos_read_pi_uint16 (picoos_File file, picoos_uint16 * val)
{
    return picoos_read_le_uint16(file,val);
}

pico_status_t picoos_read_pi_uint32 (picoos_File file, picoos_uint32 * val)
{
    return picoos_read_le_uint32(file, val);
}

pico_status_t picoos_read_pi_int32 (picoos_File file, picoos_int32 * val)
{
    return picoos_read_le_uint32(file, (picoos_uint32 *)val);
}

/* read pi from memory */

pico_status_t picoos_read_mem_pi_uint16 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint16 * val)
{
    picoos_uint8 * by = data + *pos;

    /* little-endian */
    *val = (picoos_uint16) by[1] << 8 | (picoos_uint16) by[0];
    (*pos) += 2;
    return PICO_OK;
}

pico_status_t picoos_read_mem_pi_uint32 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint32 * val)
{
        picoos_uint8 * by = data + *pos;

        /* little-endian */
        *val = (((picoos_uint32) by[3] << 8 | (picoos_uint32) by[2]) << 8 | (picoos_uint32) by[1]) << 8 | (picoos_uint32) by[0];
        (*pos) += 4;
        return PICO_OK;
}

/* **************************************************************************/
/* Write little-endian / platform-independent integers into file or memory  */
/* **************************************************************************/
/* write little-endian */
pico_status_t picoos_write_le_uint16 (picoos_File file, picoos_uint16 val)
{
    picoos_int32 len = 2;
    picoos_uint8 by[2];

    by[0]  = (picoos_uint8)((val) & 0x00FF);
    by[1]  = (picoos_uint8)(((val) & 0xFF00)>>8);
    return (picoos_WriteBytes(file,by,&len) && (2 == len));
}
pico_status_t picoos_write_le_uint32 (picoos_File file, picoos_uint32 val)
{
    picoos_int32 len = 4;
    picoos_uint8 by[4];

    by[0]  = (picoos_uint8)(val & 0x000000FF);
    by[1]  = (picoos_uint8)((val & 0x0000FF00)>>8);
    by[2]  = (picoos_uint8)((val & 0x00FF0000)>>16);
    by[3]  = (picoos_uint8)((val & 0xFF000000)>>24);
    return (picoos_WriteBytes(file,by,&len) && (4 == len));
}

/* write pi to mem */
pico_status_t picoos_write_mem_pi_uint16 (picoos_uint8 * data, picoos_uint32 * pos, picoos_uint16 val)
{
    picoos_uint8 * by = data + *pos;
    /* little-endian */
    by[0]  = (picoos_uint8)((val) & 0x00FF);
    by[1]  = (picoos_uint8)(((val) & 0xFF00)>>8);
    (*pos) += 2;
    return PICO_OK;
}

/* *****************************************************************/
/* String search and compare operations                            */
/* *****************************************************************/

/* this function is case-sensitive */
picoos_uint8 picoos_has_extension(const picoos_char *str, const picoos_char *suf)
{
    picoos_int32 istr = picoos_strlen(str)-1;
    picoos_int32 isuf = picoos_strlen(suf)-1;
    while ((istr >= 0) && (isuf >=0) && (str[istr] == suf[isuf])) {
        istr--;
        isuf--;
    }
    return (isuf < 0);
}


/* *****************************************************************/
/* String/Number Conversions  (may be moved to picopal)            */
/* *****************************************************************/

pico_status_t picoos_string_to_int32(picoos_char str[],
        picoos_int32 * res)
{
    /* syntax: [+|-] dig {dig} */

    int i;
    int neg;
    int val;
    int err;

    err = 0;
    i = 0;
    while ((str[i] <= ' ') && (str[i] != '\0')) {
        i++;
    }
    neg = 0;
    if (str[i] == '-') {
        neg = 1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    val = 0;
    if ((str[i] < '0') || (str[i]> '9')) {
        err = 1;
    }
    while ((str[i] >= '0') && (str[i] <= '9')) {
        val = val * 10 + (str[i] - '0');
        i++;
    }
    while ((str[i] <= ' ') && (str[i] != '\0')) {
        i++;
    }
    if (neg == 1) {
        val = -val;
    }
    if ((err == 0) && (str[i] == '\0')) {
        (*res) = val;
        return PICO_OK;
    } else {
        (*res) = 0;
        return PICO_EXC_NUMBER_FORMAT;
    }
}

pico_status_t picoos_string_to_uint32(picoos_char str[],
        picoos_uint32 * res)
{
    /* syntax: [+] dig {dig} */

    int i;
    int val;
    int err;

    err = 0;
    i = 0;
    while ((str[i] <= ' ') && (str[i] != '\0')) {
        i++;
    }
    if (str[i] == '+') {
        i++;
    }
    val = 0;
    if ((str[i] < '0') || (str[i]> '9')) {
        err = 1;
    }
    while ((str[i] >= '0') && (str[i] <= '9')) {
        val = val * 10 + (str[i] - '0');
        i++;
    }
    while ((str[i] <= ' ') && (str[i] != '\0')) {
        i++;
    }
    if ((err == 0) && (str[i] == '\0')) {
        (*res) = val;
        return PICO_OK;
    } else {
        (*res) = 0;
        return PICO_EXC_NUMBER_FORMAT;
    }
}

/* 'stringlen' is the part of input string to be considered,
 * possibly not containing NULLC (e.g. result of strlen).
 * 'maxsize' is the maximal size of 'part' including a byte
 * for the terminating NULLC! */
void picoos_get_sep_part_str(picoos_char string[],
        picoos_int32 stringlen, picoos_int32 * ind, picoos_char sepCh,
        picoos_char part[], picoos_int32 maxsize, picoos_uint8 * done)
{

    picoos_int32 j;
    picoos_uint8 done1;

    if (((*ind) >= stringlen)) {
        (*done) = 0;
        part[0] = (picoos_char) NULLC;
    } else {
        done1 = 1;
        j = 0;
        while ((((*ind) < stringlen) && (string[(*ind)] != sepCh)) && (string[((*ind))] != (picoos_char)NULLC)) {
            if ((j < maxsize-1)) {
                part[(j)] = string[(*ind)];
                j++;
            } else {
                done1 = 0;
            }
            (*ind)++;
        }
        part[j] = (picoos_char)NULLC;
        if ((*ind) < stringlen) {
            if ((string[(*ind)] == sepCh)) {
                (*ind)++; /* skip separator character */
            } else if (string[(*ind)] == (picoos_char)NULLC) {
                /* reached end of input; set ind to stringlen so that no
                 more (empty) partial strings will be found */
                (*ind) = stringlen;
            }
        }
        (*done) = done1;
    }
}

/* *****************************************************************/
/* timer function                                                  */
/* *****************************************************************/

extern void picoos_get_timer(picopal_uint32 * sec, picopal_uint32 * usec)
{
    picopal_get_timer(sec, usec);
}

#ifdef __cplusplus
}
#endif


/* end */
