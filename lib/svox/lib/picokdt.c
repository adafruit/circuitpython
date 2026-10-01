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
 * @file picokdt.c
 *
 * knowledge handling for decision trees
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#include "picoos.h"
#include "picodbg.h"
#include "picobase.h"
#include "picoknow.h"
#include "picodata.h"
#include "picokdt.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif


/* ************************************************************/
/* decision tree */
/* ************************************************************/

/**
 * @addtogroup picokdt
  * ---------------------------------------------------\n
 * <b> Pico KDT support </b>\n
 * ---------------------------------------------------\n
   overview extended binary tree file:
  - dt consists of optional attribute mapping tables and a non-empty
    tree part
  - using the attribute mapping tables an attribute value as used
    throughout the TTS can be mapped to its smaller representation
    used in the tree
  - multi-byte values always little endian

  -------------------------------------------------------------------
  - bin-file, decision tree knowledge base in binary form

    - dt-kb = header inputmaptables outputmaptables tree


    - header = INPMAPTABLEPOS2 OUTMAPTABLEPOS2 TREEPOS2

    - INPMAPTABLEPOS2: two bytes, equals offest in number of bytes from
                     the start of kb to the start of input map tables,
                     may not be 0
    - OUTMAPTABLEPOS2: two bytes, equals offest in number of bytes from
                     the start of kb to the start of outtables,
                     may not be 0
    - TREEPOS2: two bytes, equals offest in number of bytes from the
              start of kb to the start of the tree


    - inputmaptables = maptables
    - outputmaptables = maptables
    - maptables = NRMAPTABLES1 {maptable}=NRMAPTABLES1
    - maptable = LENTABLE2 TABLETYPE1 (   bytemaptable
                                      | wordmaptable
                                      | graphinmaptable
                                      | bytetovarmaptable )
    - bytemaptable (in or out, usage varies) =  NRBYTES2   {BYTE1}=NRBYTES2
    - wordmaptable (in or out, usage varies) =  NRWORDS2   {WORD2}=NRWORDS2
    - graphinmaptable (in only)              =  NRGRAPHS2  {GRAPH1:4}=NRGRAPHS2
    - bytetovarmaptable (out only)           =  NRINBYTES2 outvarsearchind
                                              outvaroutputs
    - outvarsearchind = {OUTVAROFFSET2}=NRINBYTES2
    - outvaroutputs = {VARVALID1:}=NRINBYTES2

    - bytemaptable: fixed size, *Map*Fixed \n
    - wordmaptable: fixed size, *Map*Fixed \n
    - graphinmaptable: search value is variable size (UTF8 grapheme), \n
                     value to be mapped to is fixed size, one byte \n
    - bytetovarmaptable: search value is fixed size, one byte, values \n
                       to be mapped to are of variable size (e.g. several \n
                       phones) \n

    - NRMAPTABLES1: one byte representing the number of map tables
    - LENTABLE2: two bytes, equals offset to the next table (or next
               part of kb, e.g. tree),
               if LENTABLE2 = 3, and
               TABLETYPE1 = EMPTY -> empty table, no mapping to be done
    - TABLETYPE1: one byte, type of map table (byte, word, or graph=utf8)
    - NRBYTES2: two bytes, number of bytes following in the table (one
              would be okay, to simplify some implementation also set
              to 2)
    - BYTE1: one btye, the sequence is used to determine the values
           being mapped to, starting with 0
    - NRWORDS2: two bytes, number of words (two btyes) following in the table
    - WORD2: two bytes, the sequence is used to determine the values
           being mapped to, starting with 0
    - NRGRAPHS2: two bytes, number of graphemes encoded in UTF8 following
               in table
    - GRAPH1:4: one to four bytes, UTF8 representation of a grapheme, the
              sequence of graphemes is used to determine the value being
              mapped to, starting with 0, the length information is
              encoded in UTF8, no need for extra length info
    - NRINBYTES2: two bytes, number of single byte IDs the tree can produce
    - OUTVAROFFSET2: two bytes, offset from the start of the
                   outvaroutputs to the start of the following output
                   phone ID group, ie. the first outvaroffset is the
                   offset to the start of the second PHONEID
                   group. Using the previous outvaroffset (or the start
                   of the outvaroutputs) the start and lenth of the
                   PHONEID group can be determined and we can get the
                   sequence of output values we map the chunk value to
    - VARVALID1:: one to several bytes, one byte each for an output phone ID

    - tree = treenodeinfos TREEBODYSIZE4 treebody
    - treenodeinfos = NRVFIELDS1 vfields NRATTRIBUTES1 NRQFIELDS1 qfields
    - vfields = {VFIELD1}=NRVFIELDS1
    - qfields = {QFIELD1}=NRATTRIBUTES1xNRQFIELDS1
    - treebody = "cf. code"

    - TREEBODYSIZE4: four bytes, size of treebody in number of bytes
    - NRVFIELDS1: one byte, number of node properties in the following
                vector (predefined and fixed sequence of properties)
    - VFIELD1: number of bits used to represent a node property
    - NRATTRIBUTES1: one byte, number of attributes (rows) in the
                   following matrix
    - NRQFIELDS1: one byte, number (columns) of question-dependent node
                properties per attribute in the following matrix
                (predefined and fixed sequence of properties)
    - QFIELD1: number of bits used to represent a question-dependent
             property in the matrix


    - Currently,
        - NRVFIELDS1 is fixed at 2 for all trees, ie.
        - vfields = 2 aVFIELD1 bVFIELD1
        - aVFIELD1: nr of bits for questions
        - bVFIELD1: nr of bits for decisions

        - NRQFIELDS1 is fixed at 5 for all trees, ie. \n
        - qfields = NRATTRIBUTES1 5 aQFIELD1 bQFIELD1 cQFIELD1 dQFIELD1 eQFIELD1 \n
            - aQFIELD1: nr of bits for fork count \n
            - bQFIELD1: nr of bits for start position for subsets \n
            - cQFIELD1: nr of bits for group size \n
            - dQFIELD1: nr of bits for offset to reach output \n
            - eQFIELD1: nr of bits for threshold (if continuous node) \n
*/


/* ************************************************************/
/* decision tree data defines */
/* may not be changed with current implementation */
/* ************************************************************/

/* maptables fields */
#define PICOKDT_MTSPOS_NRMAPTABLES   0

/* position of first byte of first maptable (for omt the only table */
#define PICOKDT_MTPOS_START          1

/* maptable fields */
#define PICOKDT_MTPOS_LENTABLE       0
#define PICOKDT_MTPOS_TABLETYPE      2
#define PICOKDT_MTPOS_NUMBER         3
#define PICOKDT_MTPOS_MAPSTART       5

/* treenodeinfos fields */
#define PICOKDT_NIPOS_NRVFIELDS      0
#define PICOKDT_NIPOS_NRATTS         3
#define PICOKDT_NIPOS_NRQFIELDS      4

/* fixed treenodeinfos number of fields */
#define PICOKDT_NODEINFO_NRVFIELDS   2
#define PICOKDT_NODEINFO_NRQFIELDS   5

/* fixed number of bits used */
#define PICOKDT_NODETYPE_NRBITS      2
#define PICOKDT_SUBSETTYPE_NRBITS    2
#define PICOKDT_ISDECIDE_NRBITS      1

/* number of inpmaptables for each tree. Since we have a possibly
   empty input map table for each att, currently these values must be
   equal to PICOKDT_NRATT* */
typedef enum {
    PICOKDT_NRINPMT_POSP = 12,
    PICOKDT_NRINPMT_POSD =  7,
    PICOKDT_NRINPMT_G2P  = 16,
    PICOKDT_NRINPMT_PHR  =  8,
    PICOKDT_NRINPMT_ACC  = 13,
    PICOKDT_NRINPMT_PAM  = 60
} kdt_nrinpmaptables_t;

/* number of outmaptables for each tree, at least one, possibly empty,
   output map table for each tree */
typedef enum {
    PICOKDT_NROUTMT_POSP =  1,
    PICOKDT_NROUTMT_POSD =  1,
    PICOKDT_NROUTMT_G2P  =  1,
    PICOKDT_NROUTMT_PHR  =  1,
    PICOKDT_NROUTMT_ACC  =  1,
    PICOKDT_NROUTMT_PAM  =  1
} kdt_nroutmaptables_t;

/* maptable types */
typedef enum {
    PICOKDT_MTTYPE_EMPTY     = 0,
    PICOKDT_MTTYPE_BYTE      = 1,
    PICOKDT_MTTYPE_WORD      = 2,
    PICOKDT_MTTYPE_GRAPH     = 3,
    PICOKDT_MTTYPE_BYTETOVAR = 4
} kdt_mttype_t;


/* ************************************************************/
/* decision tree types and loading */
/* ************************************************************/
/*  object       : Dt*KnowledgeBase
 *  shortcut     : kdt*
 *  derived from : picoknow_KnowledgeBase
 */

/* subobj shared by all decision trees */
typedef struct {
    picokdt_kdttype_t type;
    picoos_uint8 *inpmaptable;
    picoos_uint8 *outmaptable;
    picoos_uint8 *tree;
    picoos_uint32 beg_offset[128];  /* for efficiency */

    /* tree-internal details for faster processing */
    picoos_uint8 *vfields;
    picoos_uint8 *qfields;
    picoos_uint8  nrattributes;
    picoos_uint8 *treebody;
    /*picoos_uint8  nrvfields;*/  /* fix PICOKDT_NODEINFO_NRVFIELDS */
    /*picoos_uint8  nrqfields;*/  /* fix PICOKDT_NODEINFO_NRQFIELDS */

    /* direct output vector (no output mapping) */
    picoos_uint8 dset;    /* TRUE if class set, FALSE otherwise */
    picoos_uint16 dclass;
} kdt_subobj_t;

/* subobj specific for each decision tree type */
typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_POSP];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtposp_subobj_t;

typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_POSD];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtposd_subobj_t;

typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_G2P];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtg2p_subobj_t;

typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_PHR];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtphr_subobj_t;

typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_ACC];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtacc_subobj_t;

typedef struct {
    kdt_subobj_t dt;
    picoos_uint16 invec[PICOKDT_NRATT_PAM];    /* input vector */
    picoos_uint8 inveclen;  /* nr of ele set in invec; must be =nrattributes */
} kdtpam_subobj_t;


static pico_status_t kdtDtInitialize(register picoknow_KnowledgeBase this,
                                     picoos_Common common,
                                     kdt_subobj_t *dtp) {
    picoos_uint16 inppos;
    picoos_uint16 outpos;
    picoos_uint16 treepos;
    picoos_uint32 curpos = 0, pos;
    picoos_uint16 lentable;
    picoos_uint16 i;
    picoos_uint8 imtnr;

    PICODBG_DEBUG(("start"));

    /* get inmap, outmap, tree offsets */
    if ((PICO_OK == picoos_read_mem_pi_uint16(this->base, &curpos, &inppos))
        && (PICO_OK == picoos_read_mem_pi_uint16(this->base, &curpos, &outpos))
        && (PICO_OK == picoos_read_mem_pi_uint16(this->base, &curpos,
                                                 &treepos))) {

        /* all pos are mandatory, verify */
        if (inppos && outpos && treepos) {
            dtp->inpmaptable = this->base + inppos;
            dtp->outmaptable = this->base + outpos;
            dtp->tree = this->base + treepos;
            /* precalc beg offset table */
            imtnr=dtp->inpmaptable[0];
            pos=1;
            dtp->beg_offset[0] = 1;
            for (i = 0; i < imtnr; i++) {
                lentable = ((picoos_uint16)(dtp->inpmaptable[pos+1])) << 8 |
                    dtp->inpmaptable[pos];
                pos += lentable;
                dtp->beg_offset[i+1] = pos;
            }
        } else {
            dtp->inpmaptable = NULL;
            dtp->outmaptable = NULL;
            dtp->tree = NULL;
            PICODBG_ERROR(("invalid kb position info"));
            return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                           NULL, NULL);
        }

        /* nr of outmaptables is equal 1 for all trees, verify */
        if (dtp->outmaptable[PICOKDT_MTSPOS_NRMAPTABLES] != 1) {
            PICODBG_ERROR(("wrong number of outmaptables"));
            return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                           NULL, NULL);
        }

        /* check if this is an empty table, ie. len == 3 */
        if ((dtp->outmaptable[PICOKDT_MTPOS_START + PICOKDT_MTPOS_LENTABLE]
             == 3)
            && (dtp->outmaptable[PICOKDT_MTPOS_START + PICOKDT_MTPOS_LENTABLE
                                 + 1] == 0)) {
            /* verify that this is supposed to be an empty table and
               set outmaptable to NULL if so */
            if (dtp->outmaptable[PICOKDT_MTPOS_START + PICOKDT_MTPOS_TABLETYPE]
                == PICOKDT_MTTYPE_EMPTY) {
                dtp->outmaptable = NULL;
            } else {
                PICODBG_ERROR(("table length vs. type problem"));
                return picoos_emRaiseException(common->em,
                                               PICO_EXC_FILE_CORRUPT,
                                               NULL, NULL);
            }
        }

        dtp->vfields = dtp->tree + 1;
        dtp->qfields = dtp->tree + PICOKDT_NODEINFO_NRVFIELDS + 3;
        dtp->nrattributes = dtp->tree[PICOKDT_NIPOS_NRATTS];
        dtp->treebody = dtp->qfields + 4 +
            (dtp->nrattributes * PICOKDT_NODEINFO_NRQFIELDS); /* TREEBODYSIZE4*/

        /*dtp->nrvfields = dtp->tree[PICOKDT_NIPOS_NRVFIELDS]; <- is fix */
        /*dtp->nrqfields = dtp->tree[PICOKDT_NIPOS_NRQFIELDS]; <- is fix */
        /* verify that nrvfields ad nrqfields are correct */
        if ((PICOKDT_NODEINFO_NRVFIELDS != dtp->tree[PICOKDT_NIPOS_NRVFIELDS]) ||
            (PICOKDT_NODEINFO_NRQFIELDS != dtp->tree[PICOKDT_NIPOS_NRQFIELDS])) {
            PICODBG_ERROR(("problem with nr of vfields (%d) or qfields (%d)",
                           dtp->tree[PICOKDT_NIPOS_NRVFIELDS],
                           dtp->tree[PICOKDT_NIPOS_NRQFIELDS]));
            return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                           NULL, NULL);
        }
        dtp->dset = 0;
        dtp->dclass = 0;
        PICODBG_DEBUG(("tree init: nratt: %d, posomt: %d, postree: %d",
                       dtp->nrattributes, (dtp->outmaptable - dtp->inpmaptable),
                       (dtp->tree - dtp->inpmaptable)));
        return PICO_OK;
    } else {
        PICODBG_ERROR(("problem reading kb in memory"));
        return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                       NULL, NULL);
    }
}


static pico_status_t kdtDtCheck(register picoknow_KnowledgeBase this,
                                picoos_Common common,
                                kdt_subobj_t *dtp,
                                kdt_nratt_t nratt,
                                kdt_nrinpmaptables_t nrinpmt,
                                kdt_nroutmaptables_t nroutmt,
                                kdt_mttype_t mttype) {
    /* check nr attributes */
    /* check nr inpmaptables */
    /* check nr outmaptables */
    /* check outmaptable is word type */
    if ((nratt != dtp->nrattributes)
        || (dtp->inpmaptable == NULL)
        || (dtp->outmaptable == NULL)
        || (dtp->inpmaptable[PICOKDT_MTSPOS_NRMAPTABLES] != nrinpmt)
        || (dtp->outmaptable[PICOKDT_MTSPOS_NRMAPTABLES] != nroutmt)
        || (dtp->outmaptable[PICOKDT_MTPOS_START+PICOKDT_MTPOS_TABLETYPE]
            != mttype)) {
        PICODBG_ERROR(("check failed, nratt %d, nrimt %d, nromt %d, omttype %d",
                       dtp->nrattributes,
                       dtp->inpmaptable[PICOKDT_MTSPOS_NRMAPTABLES],
                       dtp->outmaptable[PICOKDT_MTSPOS_NRMAPTABLES],
                       dtp->outmaptable[PICOKDT_MTPOS_START +
                                        PICOKDT_MTPOS_TABLETYPE]));
        return picoos_emRaiseException(common->em, PICO_EXC_FILE_CORRUPT,
                                       NULL, NULL);
    }
    return PICO_OK;
}



static pico_status_t kdtPosPInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    pico_status_t status;
    kdtposp_subobj_t *dtposp;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtposp = (kdtposp_subobj_t *)this->subObj;
    dt = &(dtposp->dt);
    dt->type = PICOKDT_KDTTYPE_POSP;
    if ((status = kdtDtInitialize(this, common, dt)) != PICO_OK) {
        return status;
    }
    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_POSP,
                             PICOKDT_NRINPMT_POSP, PICOKDT_NROUTMT_POSP,
                             PICOKDT_MTTYPE_WORD)) != PICO_OK) {
        return status;
    }

    /* init specialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_POSP; i++) {
        dtposp->invec[i] = 0;
    }
    dtposp->inveclen = 0;
    PICODBG_DEBUG(("posp tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtPosDInitialize(register picoknow_KnowledgeBase this,
                                       picoos_Common common) {
    pico_status_t status;
    kdtposd_subobj_t *dtposd;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtposd = (kdtposd_subobj_t *)this->subObj;
    dt = &(dtposd->dt);
    dt->type = PICOKDT_KDTTYPE_POSD;
    if ((status = kdtDtInitialize(this, common, dt)) != PICO_OK) {
        return status;
    }
    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_POSD,
                             PICOKDT_NRINPMT_POSD, PICOKDT_NROUTMT_POSD,
                             PICOKDT_MTTYPE_WORD)) != PICO_OK) {
        return status;
    }

    /* init spezialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_POSD; i++) {
        dtposd->invec[i] = 0;
    }
    dtposd->inveclen = 0;
    PICODBG_DEBUG(("posd tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtG2PInitialize(register picoknow_KnowledgeBase this,
                                      picoos_Common common) {
    pico_status_t status;
    kdtg2p_subobj_t *dtg2p;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtg2p = (kdtg2p_subobj_t *)this->subObj;
    dt = &(dtg2p->dt);
    dt->type = PICOKDT_KDTTYPE_G2P;
    if ((status = kdtDtInitialize(this, common, dt)) != PICO_OK) {
        return status;
    }

    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_G2P,
                             PICOKDT_NRINPMT_G2P, PICOKDT_NROUTMT_G2P,
                             PICOKDT_MTTYPE_BYTETOVAR)) != PICO_OK) {
        return status;
    }

    /* init spezialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_G2P; i++) {
        dtg2p->invec[i] = 0;
    }
    dtg2p->inveclen = 0;
    PICODBG_DEBUG(("g2p tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtPhrInitialize(register picoknow_KnowledgeBase this,
                                      picoos_Common common) {
    pico_status_t status;
    kdtphr_subobj_t *dtphr;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtphr = (kdtphr_subobj_t *)this->subObj;
    dt = &(dtphr->dt);
    dt->type = PICOKDT_KDTTYPE_PHR;
    if ((status = kdtDtInitialize(this, common,dt)) != PICO_OK) {
        return status;
    }

    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_PHR,
                             PICOKDT_NRINPMT_PHR, PICOKDT_NROUTMT_PHR,
                             PICOKDT_MTTYPE_WORD)) != PICO_OK) {
        return status;
    }

    /* init spezialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_PHR; i++) {
        dtphr->invec[i] = 0;
    }
    dtphr->inveclen = 0;
    PICODBG_DEBUG(("phr tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtAccInitialize(register picoknow_KnowledgeBase this,
                                      picoos_Common common) {
    pico_status_t status;
    kdtacc_subobj_t *dtacc;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtacc = (kdtacc_subobj_t *)this->subObj;
    dt = &(dtacc->dt);
    dt->type = PICOKDT_KDTTYPE_ACC;
    if ((status = kdtDtInitialize(this, common, dt)) != PICO_OK) {
        return status;
    }

    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_ACC,
                             PICOKDT_NRINPMT_ACC, PICOKDT_NROUTMT_ACC,
                             PICOKDT_MTTYPE_WORD)) != PICO_OK) {
        return status;
    }

    /* init spezialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_ACC; i++) {
        dtacc->invec[i] = 0;
    }
    dtacc->inveclen = 0;
    PICODBG_DEBUG(("acc tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtPamInitialize(register picoknow_KnowledgeBase this,
                                      picoos_Common common) {
    pico_status_t status;
    kdtpam_subobj_t *dtpam;
    kdt_subobj_t *dt;
    picoos_uint8 i;

    if (NULL == this || NULL == this->subObj) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    dtpam = (kdtpam_subobj_t *)this->subObj;
    dt = &(dtpam->dt);
    dt->type = PICOKDT_KDTTYPE_PAM;
    if ((status = kdtDtInitialize(this, common, dt)) != PICO_OK) {
        return status;
    }

    if ((status = kdtDtCheck(this, common, dt, PICOKDT_NRATT_PAM,
                             PICOKDT_NRINPMT_PAM, PICOKDT_NROUTMT_PAM,
                             PICOKDT_MTTYPE_WORD)) != PICO_OK) {
        return status;
    }

    /* init spezialized subobj part */
    for (i = 0; i < PICOKDT_NRATT_PAM; i++) {
        dtpam->invec[i] = 0;
    }
    dtpam->inveclen = 0;
    PICODBG_DEBUG(("pam tree initialized"));
    return PICO_OK;
}


static pico_status_t kdtSubObjDeallocate(register picoknow_KnowledgeBase this,
                                         picoos_MemoryManager mm) {
    if (NULL != this) {
        picoos_deallocate(mm, (void *) &this->subObj);
    }
    return PICO_OK;
}


/* we don't offer a specialized constructor for a *KnowledgeBase but
 * instead a "specializer" of an allready existing generic
 * picoknow_KnowledgeBase */

pico_status_t picokdt_specializeDtKnowledgeBase(picoknow_KnowledgeBase this,
                                                picoos_Common common,
                                                const picokdt_kdttype_t kdttype) {
    pico_status_t status;

    if (NULL == this) {
        return picoos_emRaiseException(common->em, PICO_EXC_KB_MISSING,
                                       NULL, NULL);
    }
    this->subDeallocate = kdtSubObjDeallocate;
    switch (kdttype) {
        case PICOKDT_KDTTYPE_POSP:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtposp_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtPosPInitialize(this, common);
            break;
        case PICOKDT_KDTTYPE_POSD:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtposd_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtPosDInitialize(this, common);
            break;
        case PICOKDT_KDTTYPE_G2P:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtg2p_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtG2PInitialize(this, common);
            break;
        case PICOKDT_KDTTYPE_PHR:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtphr_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtPhrInitialize(this, common);
            break;
        case PICOKDT_KDTTYPE_ACC:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtacc_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtAccInitialize(this, common);
            break;
        case PICOKDT_KDTTYPE_PAM:
            this->subObj = picoos_allocate(common->mm,sizeof(kdtpam_subobj_t));
            if (NULL == this->subObj) {
                return picoos_emRaiseException(common->em, PICO_EXC_OUT_OF_MEM,
                                               NULL, NULL);
            }
            status = kdtPamInitialize(this, common);
            break;
        default:
            return picoos_emRaiseException(common->em, PICO_ERR_OTHER,
                                           NULL, NULL);
    }

    if (status != PICO_OK) {
        picoos_deallocate(common->mm, (void *) &this->subObj);
        return picoos_emRaiseException(common->em, status, NULL, NULL);
    }
    return PICO_OK;
}


/* ************************************************************/
/* decision tree getDt* */
/* ************************************************************/

picokdt_DtPosP picokdt_getDtPosP(picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtPosP) this->subObj));
}

picokdt_DtPosD picokdt_getDtPosD(picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtPosD) this->subObj));
}

picokdt_DtG2P  picokdt_getDtG2P (picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtG2P) this->subObj));
}

picokdt_DtPHR  picokdt_getDtPHR (picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtPHR) this->subObj));
}

picokdt_DtACC  picokdt_getDtACC (picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtACC) this->subObj));
}

picokdt_DtPAM  picokdt_getDtPAM (picoknow_KnowledgeBase this) {
    return ((NULL == this) ? NULL : ((picokdt_DtPAM) this->subObj));
}



/* ************************************************************/
/* decision tree support functions, tree */
/* ************************************************************/


typedef enum {
    eQuestion  = 0,   /* index to #bits to identify question */
    eDecide    = 1    /* index to #bits to identify decision */
} kdt_vfields_ind_t;

typedef enum {
    eForkCount = 0,   /* index to #bits for number of forks */
    eBitNo     = 1,   /* index to #bits for index of 1st element */
    eBitCount  = 2,   /* index to #bits for size of the group */
    eJump      = 3,   /* index to #bits for offset to reach output node */
    eCut       = 4    /* for contin. node: #bits for threshold checked */
} kdt_qfields_ind_t;

typedef enum {
    eNTerminal   = 0,
    eNBinary     = 1,
    eNContinuous = 2,
    eNDiscrete   = 3
} kdt_nodetypes_t;

typedef enum {
    eOneValue = 0,
    eTwoValues = 1,
    eWithoutBitMask = 2,
    eBitMask = 3
} kdt_subsettypes_t;


/* Name    :   kdt_jump
   Function:   maps the iJump offset to byte + bit coordinates
   Input   :   iJump   absolute bit offset (0..(nr-bytes-treebody)*8)
   Output  :   iByteNo the first byte containing the bits to extract
                       (0..(nr-bytes-treebody))
               iBitNo  the first bit to be extracted (0..7)
   Returns :   void
   Notes   :   updates the iByteNo + iBitNo fields
*/
static void kdt_jump(const picoos_uint32 iJump,
                     picoos_uint32 *iByteNo,
                     picoos_int8 *iBitNo) {
    picoos_uint32 iByteSize;

    iByteSize = (iJump / 8 );
    *iBitNo = (iJump - (iByteSize * 8)) + (7 - *iBitNo);
    *iByteNo += iByteSize;
    if (*iBitNo >= 8) {
        (*iByteNo)++;
        *iBitNo = 15 - *iBitNo;
    } else {
        *iBitNo = 7 - *iBitNo;
    }
}


/* replaced inline for speedup */
/* Name    :   kdtIsVal
   Function:   Returns the binary value of the bit pointed to by iByteNo, iBitNo
   Input   :   iByteNo ofsset to the byte containing the bits to extract
                       (0..sizeof(treebody))
               iBitNo  ofsset to the first bit to be extracted (0..7)
   Returns :   0/1 depending on the bit pointed to
*/
/*
static picoos_uint8 kdtIsVal(register kdt_subobj_t *this,
                             picoos_uint32 iByteNo,
                             picoos_int8 iBitNo) {
    return ((this->treebody[iByteNo] & ((1)<<iBitNo)) > 0);
}
*/


/* @todo : consider replacing inline for speedup */

/* Name    :   kdtGetQFieldsVal (was: m_QuestDependentFields)
   Function:   gets a byte from qfields
   Input   :   this      handle to a dt subobj
               attind    index of the attribute
               qind      index of the byte to be read
   Returns :   the requested byte
   Notes   :   check that attind < this->nrattributes needed before calling
               this function!
*/
static picoos_uint8 kdtGetQFieldsVal(register kdt_subobj_t *this,
                                     const picoos_uint8 attind,
                                     const kdt_qfields_ind_t qind) {
    /* check of qind done in initialize and (for some compilers) with typing */
    /* check of attind needed before calling this function */
    return this->qfields[(attind * PICOKDT_NODEINFO_NRQFIELDS) + qind];
}


/* Name    :   kdtGetShiftVal (was: get_shift_value)
   Function:   returns the (treebody) value pointed to by iByteNo, iBitNo,
               and with size iSize
   Input   :   this    reference to the processing unit struct
               iSize   number of bits to be extracted (0..N)
               iByteNo ofsset to the byte containing the bits to extract
                       (0..sizeof(treebody))
               iBitNo  ofsset to the first bit to be extracted (0..7)
   Returns :   the value requested (if size==0 --> 0 is returned)
*/
/*
static picoos_uint32 orig_kdtGetShiftVal(register kdt_subobj_t *this,
                                    const picoos_int16 iSize,
                                    picoos_uint32 *iByteNo,
                                    picoos_int8 *iBitNo) {
    picoos_uint32 iVal;
    picoos_int16 i;

    iVal = 0;
    for (i = iSize-1; i >= 0; i--) {
        if ( (this->treebody[*iByteNo] & ((1)<<(*iBitNo))) > 0) {
            iVal |= ( (1) << i );
        }
        (*iBitNo)--;
        if (*iBitNo < 0) {
            *iBitNo = 7;
            (*iByteNo)++;
        }
    }
    return iVal;
}
*/
/* refactor */
static picoos_uint32 kdtGetShiftVal(register kdt_subobj_t *this,
        const picoos_int16 iSize, picoos_uint32 *iByteNo, picoos_int8 *iBitNo)
{
    picoos_uint32 v, b, iVal;
    picoos_int16 i, j, len;
    picoos_uint8 val;

    if (iSize < 4) {
        iVal = 0;
        for (i = iSize - 1; i >= 0; i--) {
            /* no check that *iByteNo is within valid treebody range */
            if ((this->treebody[*iByteNo] & ((1) << (*iBitNo))) > 0) {
                iVal |= ((1) << i);
            }
            (*iBitNo)--;
            if (*iBitNo < 0) {
                *iBitNo = 7;
                (*iByteNo)++;
            }
        }
        return iVal;
    }

    b = *iByteNo;
    j = *iBitNo;
    len = iSize;
    *iBitNo = j - iSize;
    v = 0;
    while (*iBitNo < 0) {
        *iBitNo += 8;
        (*iByteNo)++;
    }

    val = this->treebody[b++];
    if (j < 7) {
        switch (j) {
            case 0:
                val &= 0x01;
                break;
            case 1:
                val &= 0x03;
                break;
            case 2:
                val &= 0x07;
                break;
            case 3:
                val &= 0x0f;
                break;
            case 4:
                val &= 0x1f;
                break;
            case 5:
                val &= 0x3f;
                break;
            case 6:
                val &= 0x7f;
                break;
        }
    }
    len -= j + 1;
    if (len < 0) {
        val >>= -len;
    }
    v = val;
    while (len > 0) {
        if (len >= 8) {
            j = 8;
        } else {
            j = len;
        }
        v <<= j;
        val = this->treebody[b++];
        if (j < 8) {
            switch (j) {
                case 1:
                    val &= 0x80;
                    val >>= 7;
                    break;
                case 2:
                    val &= 0xc0;
                    val >>= 6;
                    break;
                case 3:
                    val &= 0xe0;
                    val >>= 5;
                    break;
                case 4:
                    val &= 0xf0;
                    val >>= 4;
                    break;
                case 5:
                    val &= 0xf8;
                    val >>= 3;
                    break;
                case 6:
                    val &= 0xfc;
                    val >>= 2;
                    break;
                case 7:
                    val &= 0xfe;
                    val >>= 1;
                    break;
            }
        }
        v |= val;
        len -= j;
    }
    return v;
}


/* Name    :   kdtAskTree
   Function:   Tree Traversal routine
   Input   :   iByteNo ofsset to the first byte containing the bits
               to extract (0..sizeof(treebody))
               iBitNo  ofsset to the first bit to be extracted (0..7)
   Returns :   >0    continue, no solution yet found
               =0    solution found
               <0    error, no solution found
   Notes   :
*/
static picoos_int8 kdtAskTree(register kdt_subobj_t *this,
                              picoos_uint16 *invec,
                              const kdt_nratt_t invecmax,
                              picoos_uint32 *iByteNo,
                              picoos_int8 *iBitNo) {
    picoos_uint32 iNodeType;
    picoos_uint8 iQuestion;
    picoos_int32 iVal;
    picoos_int32 iForks;
    picoos_int32 iID;

    picoos_int32 iCut, iSubsetType, iBitPos, iBitCount, iPos, iJump, iDecision;
    picoos_int32 i;
    picoos_char iIsDecide;

    PICODBG_TRACE(("start"));

    /* get node type, value should be in kdt_nodetype_t range */
    iNodeType = kdtGetShiftVal(this, PICOKDT_NODETYPE_NRBITS, iByteNo, iBitNo);
    PICODBG_TRACE(("iNodeType: %d", iNodeType));

    /* get attribute to be used in question, check if in range, and get val */
    /* check of vfields argument done in initialize */
    iQuestion = kdtGetShiftVal(this, this->vfields[eQuestion], iByteNo, iBitNo);
    if ((iQuestion < this->nrattributes) && (iQuestion < invecmax)) {
        iVal = invec[iQuestion];
    } else {
        this->dset = FALSE;
        PICODBG_TRACE(("invalid question"));
        return -1;    /* iQuestion invalid */
    }
    iForks = 0;
    iID = -1;
    PICODBG_TRACE(("iQuestion: %d", iQuestion));

    switch (iNodeType) {
        case eNBinary: {
            iForks = 2;
            iID = iVal;
            break;
        }
        case eNContinuous: {
            iForks = 2;
            iID = 1;
            iCut = kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion, eCut),
                                  iByteNo, iBitNo); /*read the threshold*/
            if (iVal <= iCut) {
                iID = 0;
            }
            break;
        }
        case eNDiscrete: {
            iForks =
                kdtGetShiftVal(this,
                               kdtGetQFieldsVal(this, iQuestion, eForkCount),
                               iByteNo, iBitNo);

            for (i = 0; i < iForks-1; i++) {
                iSubsetType =
                    kdtGetShiftVal(this, PICOKDT_SUBSETTYPE_NRBITS,
                                   iByteNo, iBitNo);

                switch (iSubsetType) {
                    case eOneValue: {
                        if (iID > -1) {
                            kdt_jump(kdtGetQFieldsVal(this, iQuestion, eBitNo),
                                     iByteNo, iBitNo);
                            break;
                        }
                        iBitPos =
                            kdtGetShiftVal(this,
                                           kdtGetQFieldsVal(this, iQuestion,
                                                            eBitNo),
                                           iByteNo, iBitNo);
                        if (iVal == iBitPos) {
                            iID = i;
                        }
                        break;
                    }
                    case eTwoValues: {
                        if (iID > -1) {
                            kdt_jump((kdtGetQFieldsVal(this, iQuestion, eBitNo) +
                                      kdtGetQFieldsVal(this, iQuestion, eBitCount)),
                                     iByteNo, iBitNo);
                            break;
                        }

                        iBitPos =
                            kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion,
                                                                  eBitNo),
                                           iByteNo, iBitNo);
                        iBitCount =
                            kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion,
                                                                  eBitCount),
                                           iByteNo, iBitNo);
                        if ((iVal == iBitPos) || (iVal == iBitCount)) {
                            iID = i;
                        }
                        break;
                    }
                    case eWithoutBitMask: {
                        if (iID > -1) {
                            kdt_jump((kdtGetQFieldsVal(this, iQuestion, eBitNo) +
                                      kdtGetQFieldsVal(this, iQuestion, eBitCount)),
                                     iByteNo, iBitNo);
                            break;
                        }

                        iBitPos =
                            kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion,
                                                                  eBitNo),
                                           iByteNo, iBitNo);
                        iBitCount =
                            kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion,
                                                                  eBitCount),
                                           iByteNo, iBitNo);
                        if ((iVal >= iBitPos) && (iVal < (iBitPos + iBitCount))) {
                            iID = i;
                        }
                        break;
                    }
                    case eBitMask: {
                        iBitPos = 0;
                        if (iID > -1) {
                            kdt_jump(kdtGetQFieldsVal(this, iQuestion, eBitNo),
                                     iByteNo, iBitNo);
                        } else {
                            iBitPos =
                                kdtGetShiftVal(this,
                                               kdtGetQFieldsVal(this, iQuestion,
                                                                eBitNo),
                                               iByteNo, iBitNo);
                        }

                        iBitCount =
                            kdtGetShiftVal(this,
                                           kdtGetQFieldsVal(this, iQuestion,
                                                            eBitCount),
                                           iByteNo, iBitNo);
                        if (iID > -1) {
                            kdt_jump(iBitCount, iByteNo, iBitNo);
                            break;
                        }

                        if ((iVal >= iBitPos) && (iVal < (iBitPos + iBitCount))) {
                            iPos = iVal - iBitPos;
                            kdt_jump((iVal - iBitPos), iByteNo, iBitNo);
                         /* if (kdtIsVal(this, *iByteNo, *iBitNo))*/
                            if ((this->treebody[*iByteNo] & ((1)<<(*iBitNo))) > 0) {
                                iID = i;
                            }
                            kdt_jump((iBitCount - (iVal-iBitPos)), iByteNo, iBitNo);
                        } else {
                            kdt_jump(iBitCount, iByteNo, iBitNo);
                        }
                        break;
                    }/*end case eBitMask*/
                }/*end switch (iSubsetType)*/
            }/*end for ( i = 0; i < iForks-1; i++ ) */

            /*default tree branch*/
            if (-1 == iID) {
                iID = iForks-1;
            }
            break;
        }/*end case eNDiscrete*/
    }/*end switch (iNodeType)*/

    for (i = 0; i < iForks; i++) {
        iIsDecide = kdtGetShiftVal(this, PICOKDT_ISDECIDE_NRBITS, iByteNo, iBitNo);

        PICODBG_TRACE(("doing forks: %d", i));

        if (!iIsDecide) {
            if (iID == i) {
                iJump =
                    kdtGetShiftVal(this, kdtGetQFieldsVal(this, iQuestion, eJump),
                                   iByteNo, iBitNo);
                kdt_jump(iJump, iByteNo, iBitNo);
                this->dset = FALSE;
                return 1;    /* to be continued, no solution yet found */
            } else {
                kdt_jump(kdtGetQFieldsVal(this, iQuestion, eJump),
                         iByteNo, iBitNo);
            }
        } else {
            if (iID == i) {
                /* check of vfields argument done in initialize */
                iDecision = kdtGetShiftVal(this, this->vfields[eDecide],
                                           iByteNo, iBitNo);
                this->dclass = iDecision;
                this->dset = TRUE;
                return 0;    /* solution found */
            } else {
                /* check of vfields argument done in initialize */
                kdt_jump(this->vfields[eDecide], iByteNo, iBitNo);
            }
        }/*end if (!iIsDecide)*/
    }/*end for (i = 0; i < iForks; i++ )*/

    this->dset = FALSE;
    PICODBG_TRACE(("problem determining class"));
    return -1; /* solution not found, problem determining a class */
}



/* ************************************************************/
/* decision tree support functions, mappings */
/* ************************************************************/


/* size==1 -> MapInByte, size==2 -> MapInWord,
   size determined from table type contained in kb.
   if the inmaptable is empty, outval = inval */

static picoos_uint8 kdtMapInFixed(const kdt_subobj_t *dt,
                                  const picoos_uint8 imtnr,
                                  const picoos_uint16 inval,
                                  picoos_uint16 *outval,
                                  picoos_uint16 *outfallbackval) {
    picoos_uint8 size;
    picoos_uint32 pos;
    picoos_uint16 lentable;
    picoos_uint16 posbound;
    picoos_uint16 i;

    *outval = 0;
    *outfallbackval = 0;

    size = 0;
    pos = 0;

    /* check what can be checked */
    if (imtnr >= dt->inpmaptable[pos++]) {   /* outside tablenr range? */
        PICODBG_ERROR(("check failed: nrtab: %d, imtnr: %d",
                       dt->inpmaptable[pos-1], imtnr));
        return FALSE;
    }

    /* go forward to the needed tablenr */
    if (imtnr > 0) {
        pos = dt->beg_offset[imtnr];
    }

    /* get length */
    lentable = ((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
        dt->inpmaptable[pos];
    posbound = pos + lentable;
    pos += 2;

    /* check type of table and set size */
    if (dt->inpmaptable[pos] == PICOKDT_MTTYPE_EMPTY) {
        /* empty table no mapping needed */
        PICODBG_TRACE(("empty table: %d", imtnr));
        *outval = inval;
        return TRUE;
    } else if (dt->inpmaptable[pos] == PICOKDT_MTTYPE_BYTE) {
        size = 1;
    } else if (dt->inpmaptable[pos] == PICOKDT_MTTYPE_WORD) {
        size = 2;
    } else {
        /* wrong table type */
        PICODBG_ERROR(("wrong table type %d", dt->inpmaptable[pos]));
        return FALSE;
    }
    pos++;

    /* set fallback value in case of failed mapping, and set upper bound pos */
    *outfallbackval = ((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
        dt->inpmaptable[pos];
    pos += 2;

    /* size must be 1 or 2 here, keep 'redundant' so save time */
    if (size == 1) {
        for (i = 0; (i < *outfallbackval) && (pos < posbound); i++) {
            if (inval == dt->inpmaptable[pos]) {
                *outval = i;
                PICODBG_TRACE(("s1 %d in %d -> out %d", imtnr, inval, *outval));
                return TRUE;
            }
            pos++;
        }
    } else if (size == 2) {
        posbound--;
        for (i = 0; (i < *outfallbackval) && (pos < posbound); i++) {
            if (inval == (((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
                          dt->inpmaptable[pos])) {
                *outval = i;
                PICODBG_TRACE(("s2 %d in %d -> out %d", imtnr, inval, *outval));
                return TRUE;
            }
            pos += 2;
        }
    } else {
        /* impossible size */
        PICODBG_ERROR(("wrong size %d", size));
        return FALSE;
    }

    PICODBG_DEBUG(("no mapping found, fallback: %d", *outfallbackval));
    return FALSE;
}


static picoos_uint8 kdtMapInGraph(const kdt_subobj_t *dt,
                                  const picoos_uint8 imtnr,
                                  const picoos_uint8 *inval,
                                  const picoos_uint8 invalmaxlen,
                                  picoos_uint16 *outval,
                                  picoos_uint16 *outfallbackval) {
    picoos_uint8 ilen = 0;
    picoos_uint8 tlen;
    picoos_uint8 cont;
    picoos_uint32 pos;
    picoos_uint16 lentable;
    picoos_uint16 posbound;
    picoos_uint16 i;
    picoos_uint8 j;

    *outfallbackval = 0;

    pos = 0;
    /* check what can be checked */
    if ((imtnr >= dt->inpmaptable[pos++]) ||     /* outside tablenr range? */
        (invalmaxlen == 0) ||                    /* too short? */
        ((ilen = picobase_det_utf8_length(inval[0])) == 0) ||   /* invalid? */
        (ilen > invalmaxlen)) {                  /* not accessible? */
        PICODBG_ERROR(("check failed: nrtab: %d, imtnr: %d, invalmaxlen: %d, "
                       "ilen: %d",
                       dt->inpmaptable[pos-1], imtnr, invalmaxlen, ilen));
        return FALSE;
    }

    /* go forward to the needed tablenr */
    for (i = 0; i < imtnr; i++) {
        lentable = ((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
            dt->inpmaptable[pos];
        pos += lentable;
    }

    /* get length and check type of inpmaptable */
    lentable = ((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
        dt->inpmaptable[pos];
    posbound = pos + lentable;
    pos += 2;

#if defined(PICO_DEBUG)
    if (1) {
        int id;
        PICODBG_TRACE(("imtnr %d", imtnr));
        for (id = pos-2; id < posbound; id++) {
            PICODBG_TRACE(("imtbyte pos %d, %c %d", id - (pos-2),
                           dt->inpmaptable[id], dt->inpmaptable[id]));
        }
    }
#endif

    /* check type of table */
    if (dt->inpmaptable[pos] != PICOKDT_MTTYPE_GRAPH) {
        /* empty table does not make sense for graph */
        /* wrong table type */
        PICODBG_ERROR(("wrong table type"));
        return FALSE;
    }
    pos++;

    /* set fallback value in case of failed mapping, and set upper bound pos */
    *outfallbackval = ((picoos_uint16)(dt->inpmaptable[pos+1])) << 8 |
        dt->inpmaptable[pos];
    pos += 2;

    /* sequential search */
    for (i = 0; (i < *outfallbackval) && (pos < posbound); i++) {
        tlen = picobase_det_utf8_length(dt->inpmaptable[pos]);
        if ((pos + tlen) > posbound) {
            PICODBG_ERROR(("trying outside imt, posb: %d, pos: %d, tlen: %d",
                           posbound, pos, tlen));
            return FALSE;
        }
        if (ilen == tlen) {
            cont = TRUE;
            for (j = 0; cont && (j < ilen); j++) {
                if (dt->inpmaptable[pos + j] != inval[j]) {
                    cont = FALSE;
                }
            }
            if (cont && (j == ilen)) {    /* match found */
                *outval = i;
                PICODBG_TRACE(("found mapval, posb %d, pos %d, i %d, tlen %d",
                               posbound, pos, i, tlen));
                return TRUE;
            }
        }
        pos += tlen;
    }
    PICODBG_DEBUG(("outside imt %d, posb/pos/i: %d/%d/%d, fallback: %d",
                   imtnr, posbound, pos, i, *outfallbackval));
    return FALSE;
}


/* size==1 -> MapOutByte,    size==2 -> MapOutWord */
static picoos_uint8 kdtMapOutFixed(const kdt_subobj_t *dt,
                                   const picoos_uint16 inval,
                                   picoos_uint16 *outval) {
    picoos_uint8 size;
    picoos_uint16 nr;

    /* no check of lentable vs. nr in initialize done */

    size = 0;

    /* type */
    nr = dt->outmaptable[PICOKDT_MTPOS_START + PICOKDT_MTPOS_TABLETYPE];

    /* check type of table and set size */
    if (nr == PICOKDT_MTTYPE_EMPTY) {
        /* empty table no mapping needed */
        PICODBG_TRACE(("empty table"));
        *outval = inval;
        return TRUE;
    } else if (nr == PICOKDT_MTTYPE_BYTE) {
        size = 1;
    } else if (nr == PICOKDT_MTTYPE_WORD) {
        size = 2;
    } else {
        /* wrong table type */
        PICODBG_ERROR(("wrong table type %d", nr));
        return FALSE;
    }

    /* number of mapvalues */
    nr = ((picoos_uint16)(dt->outmaptable[PICOKDT_MTPOS_START +
                                          PICOKDT_MTPOS_NUMBER + 1])) << 8
        | dt->outmaptable[PICOKDT_MTPOS_START + PICOKDT_MTPOS_NUMBER];

    if (inval < nr) {
        if (size == 1) {
            *outval = dt->outmaptable[PICOKDT_MTPOS_START +
                                      PICOKDT_MTPOS_MAPSTART + (size * inval)];
        } else {
            *outval = ((picoos_uint16)(dt->outmaptable[PICOKDT_MTPOS_START +
                          PICOKDT_MTPOS_MAPSTART + (size * inval) + 1])) << 8
                                     | dt->outmaptable[PICOKDT_MTPOS_START +
                          PICOKDT_MTPOS_MAPSTART + (size * inval)];
        }
        return TRUE;
    } else {
        *outval = 0;
        return FALSE;
    }
}


/* size==1 -> ReverseMapOutByte,    size==2 -> ReverseMapOutWord */
/* outmaptable also used to map from decoded tree output domain to
   direct tree output domain */
static picoos_uint8 kdtReverseMapOutFixed(const kdt_subobj_t *dt,
                                          const picoos_uint16 inval,
                                          picoos_uint16 *outval,
                                          picoos_uint16 *outfallbackval) {
    picoos_uint8 size;
    picoos_uint32 pos;
    picoos_uint16 lentable;
    picoos_uint16 posbound;
    picoos_uint16 i;

    /* no check of lentable vs. nr in initialize done */

    size = 0;
    pos = 0;
    *outval = 0;
    *outfallbackval = 0;

    if (dt->outmaptable == NULL) {
        /* empty table no mapping needed */
        PICODBG_TRACE(("empty table"));
        *outval = inval;
        return TRUE;
    }

    /* check what can be checked */
    if (dt->outmaptable[pos++] != 1) {   /* only one omt possible */
        PICODBG_ERROR(("check failed: nrtab: %d", dt->outmaptable[pos-1]));
        return FALSE;
    }

    /* get length */
    lentable = ((picoos_uint16)(dt->outmaptable[pos+1])) << 8 |
        dt->outmaptable[pos];
    posbound = pos + lentable;
    pos += 2;

    /* check type of table and set size */
    /* if (dt->outmaptable[pos] == PICOKDT_MTTYPE_EMPTY), in
       ...Initialize the omt is set to NULL if not existing, checked
       above */

    if (dt->outmaptable[pos] == PICOKDT_MTTYPE_BYTE) {
        size = 1;
    } else if (dt->outmaptable[pos] == PICOKDT_MTTYPE_WORD) {
        size = 2;
    } else {
        /* wrong table type */
        PICODBG_ERROR(("wrong table type %d", dt->outmaptable[pos]));
        return FALSE;
    }
    pos++;

    /* set fallback value in case of failed mapping, and set upper bound pos */
    *outfallbackval = ((picoos_uint16)(dt->outmaptable[pos+1])) << 8 |
        dt->outmaptable[pos];
    pos += 2;

    /* size must be 1 or 2 here, keep 'redundant' so save time */
    if (size == 1) {
        for (i = 0; (i < *outfallbackval) && (pos < posbound); i++) {
            if (inval == dt->outmaptable[pos]) {
                *outval = i;
                PICODBG_TRACE(("s1 inval %d -> outval %d", inval, *outval));
                return TRUE;
            }
            pos++;
        }
    } else if (size == 2) {
        posbound--;
        for (i = 0; (i < *outfallbackval) && (pos < posbound); i++) {
            if (inval == (((picoos_uint16)(dt->outmaptable[pos+1])) << 8 |
                          dt->outmaptable[pos])) {
                *outval = i;
                PICODBG_TRACE(("s2 inval %d -> outval %d", inval, *outval));
                return TRUE;
            }
            pos += 2;
        }
    } else {
        /* impossible size */
        PICODBG_ERROR(("wrong size %d", size));
        return FALSE;
    }

    PICODBG_DEBUG(("no mapping found, fallback: %d", *outfallbackval));
    return FALSE;
}


picoos_uint8 picokdt_dtPosDreverseMapOutFixed(const picokdt_DtPosD this,
                                          const picoos_uint16 inval,
                                          picoos_uint16 *outval,
                                          picoos_uint16 *outfallbackval) {

    kdtposd_subobj_t * dtposd = (kdtposd_subobj_t *)this;
    kdt_subobj_t * dt = &(dtposd->dt);
    return kdtReverseMapOutFixed(dt,inval, outval, outfallbackval);
}

/* not yet impl. size==1 -> MapOutByteToVar,
   fix:  size==2 -> MapOutWordToVar */
static picoos_uint8 kdtMapOutVar(const kdt_subobj_t *dt,
                                 const picoos_uint16 inval,
                                 picoos_uint8 *nr,
                                 picoos_uint16 *outval,
                                 const picoos_uint16 outvalmaxlen) {
    picoos_uint16 pos;
    picoos_uint16 off2ind;
    picoos_uint16 lentable;
    picoos_uint16 nrinbytes;
    picoos_uint8 size;
    picoos_uint16 offset1;
    picoos_uint16 i;

    if (dt->outmaptable == NULL) {
        /* empty table not possible */
        PICODBG_ERROR(("no table found"));
        return FALSE;
    }

    /* nr of tables == 1 already checked in *Initialize, no need here, go
       directly to position 1 */
    pos = 1;

    /* get length of table */
    lentable = (((picoos_uint16)(dt->outmaptable[pos + 1])) << 8 |
                dt->outmaptable[pos]);
    pos += 2;

    /* check table type */
    if (dt->outmaptable[pos] != PICOKDT_MTTYPE_BYTETOVAR) {
        /* wrong table type */
        PICODBG_ERROR(("wrong table type %d", dt->outmaptable[pos]));
        return FALSE;
    }
    size = 2;
    pos++;

    /* get nr of ele in maptable (= nr of possible invals) */
    nrinbytes = (((picoos_uint16)(dt->outmaptable[pos+1])) << 8 |
                 dt->outmaptable[pos]);
    pos += 2;

    /* check what's checkable */
    if (nrinbytes == 0) {
        PICODBG_ERROR(("table with length zero"));
        return FALSE;
    } else if (inval >= nrinbytes) {
        PICODBG_ERROR(("inval %d outside valid range %d", inval, nrinbytes));
        return FALSE;
    }

    PICODBG_TRACE(("inval %d, lentable %d, nrinbytes %d, pos %d", inval,
                   lentable, nrinbytes, pos));

    /* set off2ind to the position of the start of offset2-val */
    /* offset2 points to start of next ele */
    off2ind = pos + (size*inval);

    /* get number of output values, offset2 - offset1 */
    if (inval == 0) {
        offset1 = 0;
    } else {
        offset1 = (((picoos_uint16)(dt->outmaptable[off2ind - 1])) << 8 |
                   dt->outmaptable[off2ind - 2]);
    }
    *nr = (((picoos_uint16)(dt->outmaptable[off2ind + 1])) << 8 |
           dt->outmaptable[off2ind]) - offset1;

    PICODBG_TRACE(("offset1 %d, nr %d, pos %d", offset1, *nr, pos));

    /* set pos to position of 1st value being mapped to */
    pos += (size * nrinbytes) + offset1;

    if ((pos + *nr - 1) > lentable) {
        /* outside table, should not happen */
        PICODBG_ERROR(("problem with table index, pos %d, nr %d, len %d",
                       pos, *nr, lentable));
        return FALSE;
    }
    if (*nr > outvalmaxlen) {
        /* not enough space in outval */
        PICODBG_ERROR(("overflow in outval, %d > %d", *nr, outvalmaxlen));
        return FALSE;
    }

    /* finally, copy outmap result to outval */
    for (i = 0; i < *nr; i++) {
        outval[i] = dt->outmaptable[pos++];
    }
    return TRUE;
}



/* ************************************************************/
/* decision tree POS prediction (PosP) functions */
/* ************************************************************/

/* number of prefix and suffix graphemes used to construct the input vector */
#define KDT_POSP_NRGRAPHPREFATT   4
#define KDT_POSP_NRGRAPHSUFFATT   6
#define KDT_POSP_NRGRAPHATT      10

/* positions of specgraph and nrgraphs attributes */
#define KDT_POSP_SPECGRAPHATTPOS 10
#define KDT_POSP_NRGRAPHSATTPOS  11


/* construct PosP input vector

   PosP invec: 12 elements

   prefix        0-3  prefix graphemes (encoded using tree inpmaptable 0-3)
   suffix        4-9  suffix graphemes (encoded using tree inpmaptable 4-9)
   isspecchar    10   is a special grapheme (e.g. hyphen) inside the word (0/1)?
   nr-utf-graphs 11   number of graphemes (ie. UTF8 chars)

   if there are less than 10 graphemes, each grapheme is used only
   once, with the suffix having higher priority, ie.  elements 0-9 are
   filled as follows:

    #graph
    1        0 0 0 0  0 0 0 0 0 1
    2        0 0 0 0  0 0 0 0 1 2
    3        0 0 0 0  0 0 0 1 2 3
    4        0 0 0 0  0 0 1 2 3 4
    5        0 0 0 0  0 1 2 3 4 5
    6        0 0 0 0  1 2 3 4 5 6
    7        1 0 0 0  2 3 4 5 6 7
    8        1 2 0 0  3 4 5 6 7 8
    9        1 2 3 0  4 5 6 7 8 9
    10       1 2 3 4  5 6 7 8 9 10
    11       1 2 3 4  6 7 8 9 10 11
    ...

    1-6: Fill chbuf
    7-10: front to invec 1st part, remove front, add rear
    >10: remove front, add rear
    no more graph ->
    while chbuflen>0:
      add rear to the last empty slot in 2nd part of invec, remove rear
*/


picoos_uint8 picokdt_dtPosPconstructInVec(const picokdt_DtPosP this,
                                          const picoos_uint8 *graph,
                                          const picoos_uint16 graphlen,
                                          const picoos_uint8 specgraphflag) {
    kdtposp_subobj_t *dtposp;

    /* utf8 circular char buffer, used as restricted input deque */
    /* 2nd part of graph invec has KDT_POSP_NRGRAPHSUFFATT elements, */
    /* max of UTF8_MAXLEN bytes per utf8 char */
    picoos_uint8 chbuf[KDT_POSP_NRGRAPHSUFFATT][PICOBASE_UTF8_MAXLEN];
    picoos_uint8 chbrear;   /* next free pos */
    picoos_uint8 chbfront;  /* next read pos */
    picoos_uint8 chblen;    /* empty=0; full=KDT_POSP_NRGRAPHSUFFATT */

    picoos_uint16 poscg;    /* position of current graph (= utf8 char) */
    picoos_uint16 lencg = 0;    /* length of current grapheme */
    picoos_uint16 nrutfg;   /* number of utf graphemes */
    picoos_uint8 invecpos;  /* next element to add in invec */
    picoos_uint16 fallback; /* fallback value for failed graph encodings */
    picoos_uint8 i;

    dtposp = (kdtposp_subobj_t *)this;
    chbrear = 0;
    chbfront = 0;
    chblen = 0;
    poscg = 0;
    nrutfg = 0;
    invecpos = 0;

    PICODBG_DEBUG(("graphlen %d", graphlen));

    /* not needed, since all elements are set
    for (i = 0; i < PICOKDT_NRATT_POSP; i++) {
        dtposp->invec[i] = '\x63';
    }
    */

    dtposp->inveclen = 0;

    while ((poscg < graphlen) &&
           ((lencg = picobase_det_utf8_length(graph[poscg])) > 0)) {
        if (chblen >= KDT_POSP_NRGRAPHSUFFATT) {      /* chbuf full */
            if (invecpos < KDT_POSP_NRGRAPHPREFATT) { /* prefix not full */
                /* att-encode front utf graph and add in invec */
                if (!kdtMapInGraph(&(dtposp->dt), invecpos,
                                   chbuf[chbfront], PICOBASE_UTF8_MAXLEN,
                                   &(dtposp->invec[invecpos]),
                                   &fallback)) {
                    if (fallback) {
                        dtposp->invec[invecpos] = fallback;
                    } else {
                        return FALSE;
                    }
                }
                invecpos++;
            }
            /* remove front utf graph */
            chbfront++;
            chbfront %= KDT_POSP_NRGRAPHSUFFATT;
            chblen--;
        }
        /* add current utf graph to chbuf */
        for (i=0; i<lencg; i++) {
            chbuf[chbrear][i] = graph[poscg++];
        }
        if (i < PICOBASE_UTF8_MAXLEN) {
            chbuf[chbrear][i] = '\0';
        }
        chbrear++;
        chbrear %= KDT_POSP_NRGRAPHSUFFATT;
        chblen++;
        /* increase utf graph count */
        nrutfg++;
    }

    if ((lencg == 0) || (chblen == 0)) {
        return FALSE;
    } else if (chblen > 0) {

        while (invecpos < KDT_POSP_NRGRAPHPREFATT) { /* fill up prefix */
            if (!kdtMapInGraph(&(dtposp->dt), invecpos,
                               PICOKDT_OUTSIDEGRAPH_DEFSTR,
                               PICOKDT_OUTSIDEGRAPH_DEFLEN,
                               &(dtposp->invec[invecpos]), &fallback)) {
                if (fallback) {
                    dtposp->invec[invecpos] = fallback;
                } else {
                    return FALSE;
                }
            }
            invecpos++;
        }

        for (i = (KDT_POSP_NRGRAPHATT - 1);
             i >= KDT_POSP_NRGRAPHPREFATT; i--) {
            if (chblen > 0) {
                if (chbrear == 0) {
                    chbrear = KDT_POSP_NRGRAPHSUFFATT - 1;
                } else {
                    chbrear--;
                }
                if (!kdtMapInGraph(&(dtposp->dt), i, chbuf[chbrear],
                                   PICOBASE_UTF8_MAXLEN,
                                   &(dtposp->invec[i]), &fallback)) {
                    if (fallback) {
                        dtposp->invec[i] = fallback;
                    } else {
                        return FALSE;
                    }
                }
                chblen--;
            } else {
                if (!kdtMapInGraph(&(dtposp->dt), i,
                                   PICOKDT_OUTSIDEGRAPH_DEFSTR,
                                   PICOKDT_OUTSIDEGRAPH_DEFLEN,
                                   &(dtposp->invec[i]), &fallback)) {
                    if (fallback) {
                        dtposp->invec[i] = fallback;
                    } else {
                        return FALSE;
                    }
                }
            }
        }

        /* set isSpecChar attribute, reuse var i */
        i = (specgraphflag ? 1 : 0);
        if (!kdtMapInFixed(&(dtposp->dt), KDT_POSP_SPECGRAPHATTPOS, i,
                           &(dtposp->invec[KDT_POSP_SPECGRAPHATTPOS]),
                           &fallback)) {
            if (fallback) {
                dtposp->invec[KDT_POSP_SPECGRAPHATTPOS] = fallback;
            } else {
                return FALSE;
            }
        }

        /* set nrGraphs attribute */
        if (!kdtMapInFixed(&(dtposp->dt), KDT_POSP_NRGRAPHSATTPOS, nrutfg,
                           &(dtposp->invec[KDT_POSP_NRGRAPHSATTPOS]),
                           &fallback)) {
            if (fallback) {
                dtposp->invec[KDT_POSP_NRGRAPHSATTPOS] = fallback;
            } else {
                return FALSE;
            }
        }
        PICODBG_DEBUG(("posp-invec: [%d,%d,%d,%d|%d,%d,%d,%d,%d,%d|%d|%d]",
                       dtposp->invec[0], dtposp->invec[1], dtposp->invec[2],
                       dtposp->invec[3], dtposp->invec[4], dtposp->invec[5],
                       dtposp->invec[6], dtposp->invec[7], dtposp->invec[8],
                       dtposp->invec[9], dtposp->invec[10],
                       dtposp->invec[11], dtposp->invec[12]));
        dtposp->inveclen = PICOKDT_NRINPMT_POSP;
        return TRUE;
    }

    return FALSE;
}


picoos_uint8 picokdt_dtPosPclassify(const picokdt_DtPosP this) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtposp_subobj_t *dtposp;
    kdt_subobj_t *dt;

    dtposp = (kdtposp_subobj_t *)this;
    dt = &(dtposp->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtposp->invec, PICOKDT_NRATT_POSP,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_DEBUG(("done: %d", dt->dclass));
    return ((rv == 0) && dt->dset);
}


picoos_uint8 picokdt_dtPosPdecomposeOutClass(const picokdt_DtPosP this,
                                             picokdt_classify_result_t *dtres) {
    kdtposp_subobj_t *dtposp;
    picoos_uint16 val;

    dtposp = (kdtposp_subobj_t *)this;

    if (dtposp->dt.dset &&
        kdtMapOutFixed(&(dtposp->dt), dtposp->dt.dclass, &val)) {
        dtres->set = TRUE;
        dtres->class = val;
        return TRUE;
    } else {
        dtres->set = FALSE;
        return FALSE;
    }
}



/* ************************************************************/
/* decision tree POS disambiguation (PosD) functions */
/* ************************************************************/


picoos_uint8 picokdt_dtPosDconstructInVec(const picokdt_DtPosD this,
                                          const picoos_uint16 * input) {
    kdtposd_subobj_t *dtposd;
    picoos_uint8 i;
    picoos_uint16 fallback = 0;

    dtposd = (kdtposd_subobj_t *)this;
    dtposd->inveclen = 0;

    PICODBG_DEBUG(("in: [%d,%d,%d|%d|%d,%d,%d]",
                   input[0], input[1], input[2],
                   input[3], input[4], input[5],
                   input[6]));
    for (i = 0; i < PICOKDT_NRATT_POSD; i++) {

        /* do the imt mapping for all inval */
        if (!kdtMapInFixed(&(dtposd->dt), i, input[i],
                           &(dtposd->invec[i]), &fallback)) {
            if (fallback) {
                PICODBG_DEBUG(("*** using fallback for input mapping: %i -> %i", input[i], fallback));
                dtposd->invec[i] = fallback;
            } else {
                PICODBG_ERROR(("problem doing input mapping"));
                return FALSE;
            }
        }
    }

    PICODBG_DEBUG(("out: [%d,%d,%d|%d|%d,%d,%d]",
                   dtposd->invec[0], dtposd->invec[1], dtposd->invec[2],
                   dtposd->invec[3], dtposd->invec[4], dtposd->invec[5],
                   dtposd->invec[6]));
    dtposd->inveclen = PICOKDT_NRINPMT_POSD;
    return TRUE;
}


picoos_uint8 picokdt_dtPosDclassify(const picokdt_DtPosD this,
                                    picoos_uint16 *treeout) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtposd_subobj_t *dtposd;
    kdt_subobj_t *dt;

    dtposd = (kdtposd_subobj_t *)this;
    dt = &(dtposd->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtposd->invec, PICOKDT_NRATT_POSD,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_DEBUG(("done: %d", dt->dclass));
    if ((rv == 0) && dt->dset) {
        *treeout = dt->dclass;
        return TRUE;
    } else {
        return FALSE;
    }
}


/* decompose the tree output and return the class in dtres
   dtres:         POS classification result
   returns:       TRUE if okay, FALSE otherwise
*/
picoos_uint8 picokdt_dtPosDdecomposeOutClass(const picokdt_DtPosD this,
                                             picokdt_classify_result_t *dtres) {
    kdtposd_subobj_t *dtposd;
    picoos_uint16 val;

    dtposd = (kdtposd_subobj_t *)this;

    if (dtposd->dt.dset &&
        kdtMapOutFixed(&(dtposd->dt), dtposd->dt.dclass, &val)) {
        dtres->set = TRUE;
        dtres->class = val;
        return TRUE;
    } else {
        dtres->set = FALSE;
        return FALSE;
    }
}



/* ************************************************************/
/* decision tree grapheme-to-phoneme (G2P) functions */
/* ************************************************************/


/* get the nr'th (starting at 0) utf char in utfgraph */
static picoos_uint8 kdtGetUTF8char(const picoos_uint8 *utfgraph,
                                   const picoos_uint16 graphlen,
                                   const picoos_uint16 nr,
                                   picoos_uint8 *utf8char) {
    picoos_uint16 i;
    picoos_uint32 pos;

    pos = 0;
    for (i = 0; i < nr; i++) {
        if (!picobase_get_next_utf8charpos(utfgraph, graphlen, &pos)) {
            return FALSE;
        }
    }
    return picobase_get_next_utf8char(utfgraph, graphlen, &pos, utf8char);
}

/* determine the utfchar count (starting at 1) of the utfchar starting at pos */
static picoos_uint16 kdtGetUTF8Nr(const picoos_uint8 *utfgraph,
                                  const picoos_uint16 graphlen,
                                  const picoos_uint16 pos) {
    picoos_uint32 postmp;
    picoos_uint16 count;

    count = 0;
    postmp = 0;
    while ((postmp <= pos) && (count < graphlen)) {
        if (!picobase_get_next_utf8charpos(utfgraph, graphlen, &postmp)) {
            PICODBG_ERROR(("invalid utf8 string, count: %d, pos: %d, post: %d",
                           count, pos, postmp));
            return count + 1;
        }
        count++;
    }
    return count;
}


picoos_uint8 picokdt_dtG2PconstructInVec(const picokdt_DtG2P this,
                                         const picoos_uint8 *graph,
                                         const picoos_uint16 graphlen,
                                         const picoos_uint8 count,
                                         const picoos_uint8 pos,
                                         const picoos_uint8 nrvow,
                                         const picoos_uint8 ordvow,
                                         picoos_uint8 *primstressflag,
                                         const picoos_uint16 phonech1,
                                         const picoos_uint16 phonech2,
                                         const picoos_uint16 phonech3) {
    kdtg2p_subobj_t *dtg2p;
    picoos_uint16 fallback = 0;
    picoos_uint8 iAttr;
    picoos_uint8 utf8char[PICOBASE_UTF8_MAXLEN + 1];
    picoos_uint16 inval;
    picoos_int16 cinv;
    picoos_uint8 retval;
    picoos_int32 utfgraphlen;
    picoos_uint16 utfcount;

    dtg2p = (kdtg2p_subobj_t *)this;
    retval = TRUE;
    inval = 0;

    PICODBG_TRACE(("in:  [%d,%d,%d|%d,%d|%d|%d,%d,%d]", graphlen, count, pos,
                   nrvow, ordvow, *primstressflag, phonech1, phonech2,
                   phonech3));

    dtg2p->inveclen = 0;

    /* many speed-ups possible */

    /* graph attributes */
    /*   count   >     =         <=     count
       iAttr lowbound eow     upbound  delta
         0     4      4       graphlen    5
         1     3      3       graphlen    4
         2     2      2       graphlen    3
         3     1      1       graphlen    2
         4     0      -       graphlen    1

         5     0  graphlen    graphlen-1  0
         6     0  graphlen-1  graphlen-2 -1
         7     0  graphlen-2  graphlen-3 -2
         8     0  graphlen-3  graphlen-4 -3
     */

    /* graph attributes left (context -4/-3/-2/-1) and current, MapInGraph */

    utfgraphlen = picobase_utf8_length(graph, graphlen);
    if (utfgraphlen <= 0) {
        utfgraphlen = 0;
    }
    utfcount = kdtGetUTF8Nr(graph, graphlen, count);

    cinv = 4;
    for (iAttr = 0; iAttr < 5; iAttr++) {
        if ((utfcount > cinv) && (utfcount <= utfgraphlen)) {

/*            utf8char[0] = graph[count - cinv - 1];*/
            if (!kdtGetUTF8char(graph, graphlen, utfcount-cinv-1,
                                utf8char)) {
                PICODBG_WARN(("problem getting UTF char %d", utfcount-cinv-1));
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_DEFCH;
                utf8char[1] = '\0';
            }
        } else {
            if ((utfcount == cinv) && (iAttr != 4)) {
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_EOW_DEFCH;
            } else {
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_DEFCH;
            }
            utf8char[1] = '\0';
        }

        if (!kdtMapInGraph(&(dtg2p->dt), iAttr,
                           utf8char, PICOBASE_UTF8_MAXLEN,
                           &(dtg2p->invec[iAttr]),
                           &fallback)) {
            if (fallback) {
                dtg2p->invec[iAttr] = fallback;
            } else {
                PICODBG_WARN(("setting attribute %d to zero", iAttr));
                dtg2p->invec[iAttr] = 0;
                retval = FALSE;
            }
        }
        PICODBG_TRACE(("invec %d %c", iAttr, utf8char[0]));
        cinv--;
    }

    /* graph attributes right (context 1/2/3/4), MapInGraph */
    cinv = utfgraphlen;
    for (iAttr = 5; iAttr < 9; iAttr++) {
        if ((utfcount > 0) && (utfcount <= (cinv - 1))) {
/*            utf8char[0] = graph[count + graphlen - cinv];*/
            if (!kdtGetUTF8char(graph, graphlen, utfcount+utfgraphlen-cinv,
                                utf8char)) {
                PICODBG_WARN(("problem getting UTF char %d",
                              utfcount+utfgraphlen-cinv-1));
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_DEFCH;
                utf8char[1] = '\0';
            }
        } else {
            if (utfcount == cinv) {
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_EOW_DEFCH;
                utf8char[1] = '\0';
            } else {
                utf8char[0] = PICOKDT_OUTSIDEGRAPH_DEFCH;
                utf8char[1] = '\0';
            }
        }
        if (!kdtMapInGraph(&(dtg2p->dt), iAttr,
                           utf8char, PICOBASE_UTF8_MAXLEN,
                           &(dtg2p->invec[iAttr]),
                           &fallback)) {
            if (fallback) {
                dtg2p->invec[iAttr] = fallback;
            } else {
                PICODBG_WARN(("setting attribute %d to zero", iAttr));
                dtg2p->invec[iAttr] = 0;
                retval = FALSE;
            }
        }
        PICODBG_TRACE(("invec %d %c", iAttr, utf8char[0]));
        cinv--;
    }

    /* other attributes, MapInFixed */
    for (iAttr = 9; iAttr < PICOKDT_NRATT_G2P; iAttr++) {
        switch (iAttr) {
            case 9:     /* word POS, Fix1 */
                inval = pos;
                break;
            case 10:    /* nr of vowel-like graphs in word, if vowel, Fix2  */
                inval = nrvow;
                break;
            case 11:    /* order of current vowel-like graph in word, Fix2 */
                inval = ordvow;
                break;
            case 12:    /* primary stress mark, Fix2 */
                if (*primstressflag == 1) {
                    /*already set previously*/
                    inval = 1;
                } else {
                    inval = 0;
                }
                break;
            case 13:    /* phone chunk right context +1, Hist */
                inval = phonech1;
                break;
            case 14:    /* phone chunk right context +2, Hist */
                inval = phonech2;
                break;
            case 15:    /* phone chunk right context +3, Hist */
                inval = phonech3;
                break;
        }

        PICODBG_TRACE(("invec %d %d", iAttr, inval));

        if (!kdtMapInFixed(&(dtg2p->dt), iAttr, inval,
                           &(dtg2p->invec[iAttr]), &fallback)) {
            if (fallback) {
                dtg2p->invec[iAttr] = fallback;
            } else {
                PICODBG_WARN(("setting attribute %d to zero", iAttr));
                dtg2p->invec[iAttr] = 0;
                retval = FALSE;
            }
        }
    }

    PICODBG_TRACE(("out: [%d,%d%,%d,%d|%d|%d,%d,%d,%d|%d,%d,%d,%d|"
                   "%d,%d,%d]", dtg2p->invec[0], dtg2p->invec[1],
                   dtg2p->invec[2], dtg2p->invec[3], dtg2p->invec[4],
                   dtg2p->invec[5], dtg2p->invec[6], dtg2p->invec[7],
                   dtg2p->invec[8], dtg2p->invec[9], dtg2p->invec[10],
                   dtg2p->invec[11], dtg2p->invec[12], dtg2p->invec[13],
                   dtg2p->invec[14], dtg2p->invec[15]));

    dtg2p->inveclen = PICOKDT_NRINPMT_G2P;
    return retval;
}




picoos_uint8 picokdt_dtG2Pclassify(const picokdt_DtG2P this,
                                   picoos_uint16 *treeout) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtg2p_subobj_t *dtg2p;
    kdt_subobj_t *dt;

    dtg2p = (kdtg2p_subobj_t *)this;
    dt = &(dtg2p->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtg2p->invec, PICOKDT_NRATT_G2P,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_TRACE(("done: %d", dt->dclass));
    if ((rv == 0) && dt->dset) {
        *treeout = dt->dclass;
        return TRUE;
    } else {
        return FALSE;
    }
}



picoos_uint8 picokdt_dtG2PdecomposeOutClass(const picokdt_DtG2P this,
                                  picokdt_classify_vecresult_t *dtvres) {
    kdtg2p_subobj_t *dtg2p;

    dtg2p = (kdtg2p_subobj_t *)this;

    if (dtg2p->dt.dset &&
        kdtMapOutVar(&(dtg2p->dt), dtg2p->dt.dclass, &(dtvres->nr),
                     dtvres->classvec, PICOKDT_MAXSIZE_OUTVEC)) {
        return TRUE;
    } else {
        dtvres->nr = 0;
        return FALSE;
    }
    return TRUE;
}



/* ************************************************************/
/* decision tree phrasing (PHR) functions */
/* ************************************************************/

picoos_uint8 picokdt_dtPHRconstructInVec(const picokdt_DtPHR this,
                                         const picoos_uint8 pre2,
                                         const picoos_uint8 pre1,
                                         const picoos_uint8 src,
                                         const picoos_uint8 fol1,
                                         const picoos_uint8 fol2,
                                         const picoos_uint16 nrwordspre,
                                         const picoos_uint16 nrwordsfol,
                                         const picoos_uint16 nrsyllsfol) {
    kdtphr_subobj_t *dtphr;
    picoos_uint8 i;
    picoos_uint16 inval = 0;
    picoos_uint16 fallback = 0;

    dtphr = (kdtphr_subobj_t *)this;
    PICODBG_DEBUG(("in:  [%d,%d|%d|%d,%d|%d,%d,%d]",
                   pre2, pre1, src, fol1, fol2,
                   nrwordspre, nrwordsfol, nrsyllsfol));
    dtphr->inveclen = 0;

    for (i = 0; i < PICOKDT_NRATT_PHR; i++) {
        switch (i) {
            case 0: inval = pre2; break;
            case 1: inval = pre1; break;
            case 2: inval = src; break;
            case 3: inval = fol1;  break;
            case 4: inval = fol2; break;
            case 5: inval = nrwordspre; break;
            case 6: inval = nrwordsfol; break;
            case 7: inval = nrsyllsfol; break;
            default:
                PICODBG_ERROR(("size mismatch"));
                return FALSE;
                break;
        }

        /* do the imt mapping for all inval */
        if (!kdtMapInFixed(&(dtphr->dt), i, inval,
                           &(dtphr->invec[i]), &fallback)) {
            if (fallback) {
                dtphr->invec[i] = fallback;
            } else {
                PICODBG_ERROR(("problem doing input mapping"));
                return FALSE;
            }
        }
    }

    PICODBG_DEBUG(("out: [%d,%d|%d|%d,%d|%d,%d,%d]",
                   dtphr->invec[0], dtphr->invec[1], dtphr->invec[2],
                   dtphr->invec[3], dtphr->invec[4], dtphr->invec[5],
                   dtphr->invec[6], dtphr->invec[7]));
    dtphr->inveclen = PICOKDT_NRINPMT_PHR;
    return TRUE;
}


picoos_uint8 picokdt_dtPHRclassify(const picokdt_DtPHR this) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtphr_subobj_t *dtphr;
    kdt_subobj_t *dt;

    dtphr = (kdtphr_subobj_t *)this;
    dt = &(dtphr->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtphr->invec, PICOKDT_NRATT_PHR,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_DEBUG(("done: %d", dt->dclass));
    return ((rv == 0) && dt->dset);
}


picoos_uint8 picokdt_dtPHRdecomposeOutClass(const picokdt_DtPHR this,
                                            picokdt_classify_result_t *dtres) {
    kdtphr_subobj_t *dtphr;
    picoos_uint16 val;

    dtphr = (kdtphr_subobj_t *)this;

    if (dtphr->dt.dset &&
        kdtMapOutFixed(&(dtphr->dt), dtphr->dt.dclass, &val)) {
        dtres->set = TRUE;
        dtres->class = val;
        return TRUE;
    } else {
        dtres->set = FALSE;
        return FALSE;
    }
}



/* ************************************************************/
/* decision tree phono-acoustical model (PAM) functions */
/* ************************************************************/

picoos_uint8 picokdt_dtPAMconstructInVec(const picokdt_DtPAM this,
                                         const picoos_uint8 *vec,
                                         const picoos_uint8 veclen) {
    kdtpam_subobj_t *dtpam;
    picoos_uint8 i;
    picoos_uint16 fallback = 0;

    dtpam = (kdtpam_subobj_t *)this;

    PICODBG_TRACE(("in0:  %d %d %d %d %d %d %d %d %d %d",
                   vec[0], vec[1], vec[2], vec[3], vec[4],
                   vec[5], vec[6], vec[7], vec[8], vec[9]));
    PICODBG_TRACE(("in1:  %d %d %d %d %d %d %d %d %d %d",
                   vec[10], vec[11], vec[12], vec[13], vec[14],
                   vec[15], vec[16], vec[17], vec[18], vec[19]));
    PICODBG_TRACE(("in2:  %d %d %d %d %d %d %d %d %d %d",
                   vec[20], vec[21], vec[22], vec[23], vec[24],
                   vec[25], vec[26], vec[27], vec[28], vec[29]));
    PICODBG_TRACE(("in3:  %d %d %d %d %d %d %d %d %d %d",
                   vec[30], vec[31], vec[32], vec[33], vec[34],
                   vec[35], vec[36], vec[37], vec[38], vec[39]));
    PICODBG_TRACE(("in4:  %d %d %d %d %d %d %d %d %d %d",
                   vec[40], vec[41], vec[42], vec[43], vec[44],
                   vec[45], vec[46], vec[47], vec[48], vec[49]));
    PICODBG_TRACE(("in5:  %d %d %d %d %d %d %d %d %d %d",
                   vec[50], vec[51], vec[52], vec[53], vec[54],
                   vec[55], vec[56], vec[57], vec[58], vec[59]));

    dtpam->inveclen = 0;

    /* check veclen */
    if (veclen != PICOKDT_NRINPMT_PAM) {
        PICODBG_ERROR(("wrong number of input vector elements"));
        return FALSE;
    }

    for (i = 0; i < PICOKDT_NRATT_PAM; i++) {

        /* do the imt mapping for all vec eles */
        if (!kdtMapInFixed(&(dtpam->dt), i, vec[i],
                           &(dtpam->invec[i]), &fallback)) {
            if (fallback) {
                dtpam->invec[i] = fallback;
            } else {
                PICODBG_ERROR(("problem doing input mapping, %d %d", i,vec[i]));
                return FALSE;
            }
        }
    }

    PICODBG_TRACE(("in0:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[0], dtpam->invec[1], dtpam->invec[2],
                   dtpam->invec[3], dtpam->invec[4], dtpam->invec[5],
                   dtpam->invec[6], dtpam->invec[7], dtpam->invec[8],
                   dtpam->invec[9]));
    PICODBG_TRACE(("in1:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[10], dtpam->invec[11], dtpam->invec[12],
                   dtpam->invec[13], dtpam->invec[14], dtpam->invec[15],
                   dtpam->invec[16], dtpam->invec[17], dtpam->invec[18],
                   dtpam->invec[19]));
    PICODBG_TRACE(("in2:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[20], dtpam->invec[21], dtpam->invec[22],
                   dtpam->invec[23], dtpam->invec[24], dtpam->invec[25],
                   dtpam->invec[26], dtpam->invec[27], dtpam->invec[28],
                   dtpam->invec[29]));
    PICODBG_TRACE(("in3:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[30], dtpam->invec[31], dtpam->invec[32],
                   dtpam->invec[33], dtpam->invec[34], dtpam->invec[35],
                   dtpam->invec[36], dtpam->invec[37], dtpam->invec[38],
                   dtpam->invec[39]));
    PICODBG_TRACE(("in4:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[40], dtpam->invec[41], dtpam->invec[42],
                   dtpam->invec[43], dtpam->invec[44], dtpam->invec[45],
                   dtpam->invec[46], dtpam->invec[47], dtpam->invec[48],
                   dtpam->invec[49]));
    PICODBG_TRACE(("in5:  %d %d %d %d %d %d %d %d %d %d",
                   dtpam->invec[50], dtpam->invec[51], dtpam->invec[52],
                   dtpam->invec[53], dtpam->invec[54], dtpam->invec[55],
                   dtpam->invec[56], dtpam->invec[57], dtpam->invec[58],
                   dtpam->invec[59]));

    dtpam->inveclen = PICOKDT_NRINPMT_PAM;
    return TRUE;
}


picoos_uint8 picokdt_dtPAMclassify(const picokdt_DtPAM this) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtpam_subobj_t *dtpam;
    kdt_subobj_t *dt;

    dtpam = (kdtpam_subobj_t *)this;
    dt = &(dtpam->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtpam->invec, PICOKDT_NRATT_PAM,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_DEBUG(("done: %d", dt->dclass));
    return ((rv == 0) && dt->dset);
}


picoos_uint8 picokdt_dtPAMdecomposeOutClass(const picokdt_DtPAM this,
                                            picokdt_classify_result_t *dtres) {
    kdtpam_subobj_t *dtpam;
    picoos_uint16 val;

    dtpam = (kdtpam_subobj_t *)this;

    if (dtpam->dt.dset &&
        kdtMapOutFixed(&(dtpam->dt), dtpam->dt.dclass, &val)) {
        dtres->set = TRUE;
        dtres->class = val;
        return TRUE;
    } else {
        dtres->set = FALSE;
        return FALSE;
    }
}



/* ************************************************************/
/* decision tree accentuation (ACC) functions */
/* ************************************************************/

picoos_uint8 picokdt_dtACCconstructInVec(const picokdt_DtACC this,
                                         const picoos_uint8 pre2,
                                         const picoos_uint8 pre1,
                                         const picoos_uint8 src,
                                         const picoos_uint8 fol1,
                                         const picoos_uint8 fol2,
                                         const picoos_uint16 hist1,
                                         const picoos_uint16 hist2,
                                         const picoos_uint16 nrwordspre,
                                         const picoos_uint16 nrsyllspre,
                                         const picoos_uint16 nrwordsfol,
                                         const picoos_uint16 nrsyllsfol,
                                         const picoos_uint16 footwordsfol,
                                         const picoos_uint16 footsyllsfol) {
    kdtacc_subobj_t *dtacc;
    picoos_uint8 i;
    picoos_uint16 inval = 0;
    picoos_uint16 fallback = 0;

    dtacc = (kdtacc_subobj_t *)this;
    PICODBG_DEBUG(("in:  [%d,%d,%d,%d,%d|%d,%d|%d,%d,%d,%d|%d,%d]",
                   pre2, pre1, src, fol1, fol2, hist1, hist2,
                   nrwordspre, nrsyllspre, nrwordsfol, nrsyllsfol,
                   footwordsfol, footsyllsfol));
    dtacc->inveclen = 0;

    for (i = 0; i < PICOKDT_NRATT_ACC; i++) {
        switch (i) {
            case 0: inval = pre2; break;
            case 1: inval = pre1; break;
            case 2: inval = src; break;
            case 3: inval = fol1;  break;
            case 4: inval = fol2; break;
            case 5: inval = hist1; break;
            case 6: inval = hist2; break;
            case 7: inval = nrwordspre; break;
            case 8: inval = nrsyllspre; break;
            case 9: inval = nrwordsfol; break;
            case 10: inval = nrsyllsfol; break;
            case 11: inval = footwordsfol; break;
            case 12: inval = footsyllsfol; break;
            default:
                PICODBG_ERROR(("size mismatch"));
                return FALSE;
                break;
        }

        if (((i == 5) || (i == 6)) && (inval == PICOKDT_HISTORY_ZERO)) {
            /* in input to this function the HISTORY_ZERO is used to
               mark the no-value-available case. For sparsity reasons
               this was not used in the training. For
               no-value-available cases, instead, do reverse out
               mapping of ACC0 to get tree domain for ACC0  */
            if (!kdtReverseMapOutFixed(&(dtacc->dt), PICODATA_ACC0,
                                       &inval, &fallback)) {
                if (fallback) {
                    inval = fallback;
                } else {
                    PICODBG_ERROR(("problem doing reverse output mapping"));
                    return FALSE;
                }
            }
        }

        /* do the imt mapping for all inval */
        if (!kdtMapInFixed(&(dtacc->dt), i, inval,
                           &(dtacc->invec[i]), &fallback)) {
            if (fallback) {
                dtacc->invec[i] = fallback;
            } else {
                PICODBG_ERROR(("problem doing input mapping"));
                return FALSE;
            }
        }
    }

    PICODBG_DEBUG(("out: [%d,%d,%d,%d,%d|%d,%d|%d,%d,%d,%d|%d,%d]",
                   dtacc->invec[0], dtacc->invec[1], dtacc->invec[2],
                   dtacc->invec[3], dtacc->invec[4], dtacc->invec[5],
                   dtacc->invec[6], dtacc->invec[7], dtacc->invec[8],
                   dtacc->invec[9], dtacc->invec[10], dtacc->invec[11],
                   dtacc->invec[12]));
    dtacc->inveclen = PICOKDT_NRINPMT_ACC;
    return TRUE;
}


picoos_uint8 picokdt_dtACCclassify(const picokdt_DtACC this,
                                   picoos_uint16 *treeout) {
    picoos_uint32 iByteNo;
    picoos_int8 iBitNo;
    picoos_int8 rv;
    kdtacc_subobj_t *dtacc;
    kdt_subobj_t *dt;

    dtacc = (kdtacc_subobj_t *)this;
    dt = &(dtacc->dt);
    iByteNo = 0;
    iBitNo = 7;
    while ((rv = kdtAskTree(dt, dtacc->invec, PICOKDT_NRATT_ACC,
                            &iByteNo, &iBitNo)) > 0) {
        PICODBG_TRACE(("asking tree"));
    }
    PICODBG_TRACE(("done: %d", dt->dclass));
    if ((rv == 0) && dt->dset) {
        *treeout = dt->dclass;
        return TRUE;
    } else {
        return FALSE;
    }
}


picoos_uint8 picokdt_dtACCdecomposeOutClass(const picokdt_DtACC this,
                                            picokdt_classify_result_t *dtres) {
    kdtacc_subobj_t *dtacc;
    picoos_uint16 val;

    dtacc = (kdtacc_subobj_t *)this;

    if (dtacc->dt.dset &&
        kdtMapOutFixed(&(dtacc->dt), dtacc->dt.dclass, &val)) {
        dtres->set = TRUE;
        dtres->class = val;
        return TRUE;
    } else {
        dtres->set = FALSE;
        return FALSE;
    }
}

#ifdef __cplusplus
}
#endif


/* end */
