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
 * @file picopam.c
 *
 * Phonetic to Acoustic Mapping PU - Implementation
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
#include "picodbg.h"
#include "picodata.h"
#include "picopam.h"
#include "picokdt.h"
#include "picokpdf.h"
#include "picoktab.h"
#include "picokdbg.h"
#include "picodsp.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

#define PICOPAM_IN_BUFF_SIZE PICODATA_BUFSIZE_PAM    /*input buffer size for PAM */
#define PICOPAM_OUT_PAM_SIZE PICODATA_BUFSIZE_PAM    /*output buffer size for PAM*/
#define PICOPAM_DT_NRLFZ    5    /* nr of lfz decision trees per phoneme */
#define PICOPAM_DT_NRMGC    5    /* nr of mgc decision trees per phoneme */
#define PICOPAM_NRSTPF      5    /* nr of states per phone */

#define PICOPAM_COLLECT     0
#define PICOPAM_SCHEDULE    1
#define PICOPAM_IMMEDIATE   2
#define PICOPAM_FORWARD     3
#define PICOPAM_FORWARD_FORCE_TERM 4
#define PICOPAM_PROCESS     5
#define PICOPAM_PLAY        6
#define PICOPAM_FEED        7

#define PICOPAM_CONTINUE       100
#define PICOPAM_GOTO_SCHEDULE  1
#define PICOPAM_FLUSH_RECEIVED 6
#define PICOPAM_GOTO_FEED      7
#define PICOPAM_PRE_SYLL_ENDED 10

#define PICOPAM_BREAK_ADD_SIZE 4        /*syllable feature vector increment dued to BREAK and SILENCE*/
#define PICOPAM_VECT_SIZE 64+PICOPAM_BREAK_ADD_SIZE /*syllable feature vector size (bytes)*/
#define PICOPAM_INVEC_SIZE 60           /*phone feature vector size */
#define PICOPAM_MAX_SYLL_PER_SENT 100   /*maximum number of syllables per sentece*/
#define PICOPAM_MAX_PH_PER_SENT 400     /*maximum number of phonemes  per sentece*/
#define PICOPAM_MAX_ITEM_PER_SENT 255   /*maximum number of attached items per sentence*/
#define PICOPAM_MAX_ITEM_SIZE_PER_SENT 4096 /*maximum size of attached items per sentence*/

#define PICOPAM_READY 20 /*PAM could start backward processing*/
#define PICOPAM_MORE  21 /*PAM has still to collect */
#define PICOPAM_NA    22 /*PAM has not to deal with this item*/
#define PICOPAM_ERR   23 /*input item is not a valid item*/

/*sentence types:cfr pam_map_sentence_type*/
#define PICOPAM_DECLARATIVE   0
#define PICOPAM_INTERROGATIVE 1
#define PICOPAM_EXCLAMATIVE   2

#define PICOPAM_T   0
#define PICOPAM_P   1
#define PICOPAM_p   2
#define PICOPAM_Y   3

#if 1
#define PAM_PHR2_WITH_PR1 1 /*deal with PHR2 boundaries as with PHR1*/
#else
#define PAM_PHR2_WITH_PR3 1 /*deal with PHR2 boundaries as with PHR3*/
#endif

#define PICOPAM_DONT_CARE_VALUE  250 /*don't care value for tree printout    */
#define PICOPAM_DONT_CARE_VAL    10  /*don't care value for tree feeding     */
#define PICOPAM_PH_DONT_CARE_VAL 7   /*don't care value for tree feeding (phonetic)*/

#define PICOPAM_MAX_STATES_PER_PHONE 5 /*number of states per phone    */
#define PICOPAM_STATE_SIZE_IN_ITEM   6 /*size of a state in frame item */
#define PICOPAM_FRAME_ITEM_SIZE      4+PICOPAM_MAX_STATES_PER_PHONE*PICOPAM_STATE_SIZE_IN_ITEM

#define PICOPAM_DIR_FORW 0 /*forward adapter processing*/
#define PICOPAM_DIR_BACK 1 /*backward adapter processing*/
#define PICOPAM_DIR_SIL  2 /*final silence attributes*/

#define PICOPAM_SYLL_PAUSE  0 /*syllable but containing a pause phone*/
#define PICOPAM_SYLL_SYLL   1 /*a real syllable with phonemes*/

#define PICOPAM_EVENT_P_BOUND 0 /*primary boundary*/
#define PICOPAM_EVENT_S_BOUND 1 /*secondary boundary*/
#define PICOPAM_EVENT_W_BOUND 3 /*word boundary*/
#define PICOPAM_EVENT_SYLL    4 /*syllable*/

/* ----- CONSTANTS FOR BREAK COMMAND SUPPORT ----- */
#define PICOPAM_PWIDX_SBEG 0
#define PICOPAM_PWIDX_PHR1 1
#define PICOPAM_PWIDX_PHR2 2
#define PICOPAM_PWIDX_SEND 3
#define PICOPAM_PWIDX_DEFA 4
#define PICOPAM_PWIDX_SIZE 5

/*----------------------------------------------------------------*/
/*structure related to the feature vectors for feeding the trees  */
/*NOTE : the same data structure is used to manage the syllables  */
/*       Using the first 8 fields for marking the boundaries      */
/*       and using the last 4 bytes as follows                    */
/*     byte 61 : 1st attached non PAM item id(0=no item attached) */
/*               in the "sSyllItemOffs" data structure            */
/*     byte 62 : last attached non PAM item id(0=no item attached)*/
/*               in the "sSyllItemOffs" data structure            */
/*     byte 63..64 : offset of the start of the syllable in       */
/*                   the "sPhIds" data structure                  */
typedef struct
{
    picopal_uint8 phoneV[PICOPAM_VECT_SIZE];
} sFtVect, *pSftVect;

/*----------------------------------------------------------
 Name    :   pam_subobj
 Function:   subobject definition for the pam processing
 Shortcut:   pam
 ---------------------------------------------------------*/
typedef struct pam_subobj
{
    /*----------------------PU voice management------------------------------*/
    /* picorsrc_Voice voice; */
    /*----------------------PU state management------------------------------*/
    picoos_uint8 procState; /* where to take up work at next processing step */
    picoos_uint8 retState; /* where to go back from feed state at next p.s. */
    picoos_uint8 needMoreInput; /* more data necessary to start processing   */
    /*----------------------PU input management------------------------------*/
    picoos_uint8 inBuf[PICOPAM_IN_BUFF_SIZE]; /* internal input buffer */
    picoos_uint16 inBufSize; /* actually allocated size */
    picoos_uint16 inReadPos, inWritePos; /* next pos to read/write from/to inBuf*/
    /*----------------------PU output management-----------------------------*/
    picoos_uint8 outBuf[PICOPAM_OUT_PAM_SIZE]; /* internal output buffer */
    picoos_uint16 outBufSize; /* actually allocated size */
    picoos_uint16 outReadPos, outWritePos; /* next pos to read/write from/to outBuf*/
    /*---------------------- adapter working buffers    --------------------*/
    picoos_uint8 *sPhFeats; /*feature vector for a single phone      */
    sFtVect *sSyllFeats; /*Syllable feature vector set for the
     full sentence                          */
    picoos_uint8 *sPhIds; /*phone ids for the full sentence        */
    picoos_uint8 *sSyllItems; /*items attached to the syllable         */
    picoos_int16 *sSyllItemOffs;/*offset of items attached to the syllable*/
    /*---------------------- adapter general variables ---------------------*/
    picoos_int16 nTotalPhonemes; /*number of phonemes in the sentence*/
    picoos_int16 nCurrPhoneme; /*current phoneme in the sentence   */
    picoos_int16 nSyllPhoneme; /*current phoneme in the syllable   */
    picoos_int16 nCurrSyllable; /*current syllable in the sentence  */
    picoos_int16 nTotalSyllables; /*number of syllables in the sentence -> J1*/
    picoos_uint8 nLastAttachedItemId;/*last attached item id*/
    picoos_uint8 nCurrAttachedItem; /*current attached item*/
    picoos_int16 nAttachedItemsSize; /*total size of the attached items*/
    picoos_uint8 sType; /*Sentence type*/
    picoos_uint8 pType; /*Phrase type*/
    picoos_single pMod; /*pitch modifier*/
    picoos_single dMod; /*Duration modifier*/
    picoos_single dRest; /*Duration modifier rest*/
    /*---------------------- adapter specific component variables ----------*/
    picoos_uint8 a3_overall_syllable; /* A3 */
    picoos_uint8 a3_primary_phrase_syllable;
    picoos_uint8 b4_b5_syllable; /* B4,B5 */
    picoos_uint8 b6_b7_syllable; /* B6,B7 */
    picoos_uint8 b6_b7_state;
    picoos_uint8 b8_b9_stressed_syllable; /* B8,B9 */
    picoos_uint8 b10_b11_accented_syllable; /* B10,B11 */
    picoos_uint8 b12_b13_syllable; /* B12,B13 */
    picoos_uint8 b12_b13_state;
    picoos_uint8 b14_b15_syllable; /* B14,B15 */
    picoos_uint8 b14_b15_state;
    picoos_uint8 b17_b19_syllable; /* B17,B19 */
    picoos_uint8 b17_b19_state;
    picoos_uint8 b18_b20_b21_syllable; /* B18,B20,B21 */
    picoos_uint8 b18_b20_b21_state;
    picoos_uint8 c3_overall_syllable; /* C3 */
    picoos_uint8 c3_primary_phrase_syllable;
    picoos_uint8 d2_syllable_in_word; /* D2 */
    picoos_uint8 d2_prev_syllable_in_word;
    picoos_uint8 d2_current_primary_phrase_word;
    picoos_int8 e1_syllable_word_start; /* E1 */
    picoos_int8 e1_syllable_word_end;
    picoos_uint8 e1_content;
    picoos_int8 e2_syllable_word_start; /* E2 */
    picoos_int8 e2_syllable_word_end;
    picoos_uint8 e3_e4_word; /* E3,E4 */
    picoos_uint8 e3_e4_state;
    picoos_uint8 e5_e6_content_word; /* E5,E6 */
    picoos_uint8 e5_e6_content;
    picoos_uint8 e7_e8_word; /* E7,E8 */
    picoos_uint8 e7_e8_content;
    picoos_uint8 e7_e8_state;
    picoos_uint8 e9_e11_word; /* E9,E11 */
    picoos_uint8 e9_e11_saw_word;
    picoos_uint8 e9_e11_state;
    picoos_uint8 e10_e12_e13_word; /* E10,E12,E13 */
    picoos_uint8 e10_e12_e13_state;
    picoos_uint8 e10_e12_e13_saw_word;
    picoos_uint8 f2_overall_word; /* F2 */
    picoos_uint8 f2_word_syllable;
    picoos_uint8 f2_next_word_syllable;
    picoos_uint8 f2_current_primary_phrase_word;
    picoos_int8 g1_current_secondary_phrase_syllable; /*G1 */
    picoos_int8 g1_current_syllable;
    picoos_int8 g2_current_secondary_phrase_word; /*G2 */
    picoos_int8 g2_current_word;
    picoos_uint8 h1_current_secondary_phrase_syll; /*H1 */
    picoos_uint8 h2_current_secondary_phrase_word; /*H2 */
    picoos_uint8 h3_h4_current_secondary_phrase_word; /*H3,H4 */
    picoos_uint8 h5_current_phrase_type; /*H5 */

    picoos_uint8 h5_syllable; /* H5 */
    picoos_uint8 h5_state;

    picoos_uint8 i1_secondary_phrase_syllable; /*I1 */
    picoos_uint8 i1_next_secondary_phrase_syllable;
    picoos_uint8 i2_secondary_phrase_word; /*I2 */
    picoos_uint8 i2_next_secondary_phrase_word;
    picoos_uint8 j1_utterance_syllable; /*J1 */
    picoos_uint8 j2_utterance_word; /*J2 */
    picoos_uint8 j3_utterance_sec_phrases; /*J3 */
    /*---------------------- constant data -------------------*/
    picoos_uint16 sil_weights[PICOPAM_PWIDX_SIZE][PICOPAM_MAX_STATES_PER_PHONE];
    /*---------------------- LINGWARE related data -------------------*/
    picokdt_DtPAM dtdur; /* dtdur knowledge base */
    picokdt_DtPAM dtlfz[PICOPAM_DT_NRLFZ]; /* dtlfz knowledge bases */
    picokdt_DtPAM dtmgc[PICOPAM_DT_NRMGC]; /* dtmgc knowledge bases */
    /*---------------------- Pdfs related data -------------------*/
    picokpdf_PdfDUR pdfdur; /* pdfdur knowledge base */
    picokpdf_PdfMUL pdflfz; /* pdflfz knowledge base */
    /*---------------------- Tree traversal related data -------------------*/
    picoos_uint16 durIndex;
    picoos_uint8 numFramesState[PICOPAM_DT_NRLFZ];
    picoos_uint16 lf0Index[PICOPAM_DT_NRLFZ];
    picoos_uint16 mgcIndex[PICOPAM_DT_NRMGC];
    /*---------------------- temps for updating the feature vector ---------*/
    picoos_uint16 phonDur;
    picoos_single phonF0[PICOPAM_DT_NRLFZ];
    /*---------------------- Phones related data  -------------------*/
    picoktab_Phones tabphones;
} pam_subobj_t;


/* ----- CONSTANTS FOR FEATURE VECTOR BUILDING (NOT PREFIXED WITH "PICOPAM_" FOR BREVITY) ----- */
#define  P1  0              /*field 1 of the input vector*/
#define  P2  1
#define  P3  2
#define  P4  3
#define  P5  4
#define  P6  5
#define  P7  6
#define  bnd 6              /*boundary type item associated to the syllable = P7 */
#define  P8  7
#define  A3  8
#define  B1  9
#define  B2  10
#define  B3  11
#define  B4  12
#define  B5  13
#define  B6  14
#define  B7  15
#define  B8  16
#define  B9  17
#define  B10 18
#define  B11 19
#define  B12 20
#define  B13 21
#define  B14 22
#define  B15 23
#define  B16 24
#define  B17 25
#define  B18 26
#define  B19 27
#define  B20 28
#define  B21 29
#define  C3  30
#define  D2  31
#define  E1  32
#define  E2  33
#define  E3  34
#define  E4  35
#define  E5  36
#define  E6  37
#define  E7  38
#define  E8  39
#define  E9  40
#define  E10 41
#define  E11 42
#define  E12 43
#define  E13 44
#define  F2  45
#define  G1  46
#define  G2  47
#define  H1  48
#define  H2  49
#define  H3  50
#define  H4  51
#define  H5  52
#define  I1  53
#define  I2  54
#define  J1  55
#define  J2  56
#define  J3  57
#define  DUR 58             /*duration component*/
#define  F0  59             /*F0 component*/
#define  ITM 60             /*Item Offset into sSyllItems item list*/
#define  itm 61             /*second byte of the Item Offset */
#define  FID 62             /*Phoneme offset in the sPhIds phoneme list*/
#define  fid 63             /*second byte of the Phoneme offset */
#define  Min 64             /*offset to min syllable duration (uint 16,pauses)*/
#define  Max 66             /*offset to max syllable duration (uint 16,pauses)*/
/* -------------------------------------------------------------------
 PAM feature vector indices position changes,
  ------------------------------------------------------------------- */
#define  T_B1  8
#define  T_B2  9
#define  T_B3  10
#define  T_B4  11
#define  T_B5  12
#define  T_B6  13
#define  T_B7  14
#define  T_B8  15
#define  T_B9  16
#define  T_B10 17
#define  T_B11 18
#define  T_B12 19
#define  T_B13 20
#define  T_B14 21
#define  T_B15 22
#define  T_B16 23
#define  T_B17 24
#define  T_B18 25
#define  T_B19 26
#define  T_B20 27
#define  T_B21 28
#define  T_E1  29
#define  T_E2  30
#define  T_E3  31
#define  T_E4  32
#define  T_E5  33
#define  T_E6  34
#define  T_E7  35
#define  T_E8  36
#define  T_E9  37
#define  T_E10 38
#define  T_E11 39
#define  T_E12 40
#define  T_E13 41
#define  T_A3  42
#define  T_C3  43
#define  T_D2  44
#define  T_F2  45
#define  T_G1  46
#define  T_I1  47
#define  T_G2  48
#define  T_I2  49
#define  T_H1  50
#define  T_H2  51
#define  T_H3  52
#define  T_H4  53
#define  T_H5  54

/*------------------------------------------------------------------
 Service routines :
 ------------------------------------------------------------------*/
static pico_status_t pam_initialize(register picodata_ProcessingUnit this, picoos_int32 resetMode);
static pico_status_t pam_terminate(register picodata_ProcessingUnit this);
static pico_status_t pam_allocate(picoos_MemoryManager mm, pam_subobj_t *pam);
static void pam_deallocate(picoos_MemoryManager mm, pam_subobj_t *pam);
static pico_status_t pam_subobj_deallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm);
/*------------------------------------------------------------------
 Processing routines :
 ------------------------------------------------------------------*/
static picodata_step_result_t pam_step(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput);
static pico_status_t pam_deal_with(const picoos_uint8 *item);
/*Utility*/
static picoos_uint8 pam_get_vowel_name(register picodata_ProcessingUnit this,
        picoos_uint8 *item, picoos_uint8 *pos);
static picoos_uint8 pam_get_pause_id(register picodata_ProcessingUnit this);

static picoos_uint8 pam_map_sentence_type(picoos_uint8 iteminfo1,
        picoos_uint8 iteminfo2);
static picoos_uint8 pam_map_phrase_type(picoos_uint8 iteminfo1,
        picoos_uint8 iteminfo2);

/*Adapter*/
static pico_status_t pam_reset_processors(register picodata_ProcessingUnit this);
static pico_status_t pam_reset_processors_back(
        register picodata_ProcessingUnit this);
static pico_status_t pam_create_syllable(register picodata_ProcessingUnit this,
        picoos_uint8 syllType, picoos_uint8 *sContent, picoos_uint8 sentType,
        picoos_uint8 phType, picoos_uint8 uBoundType, picoos_uint16 uMin,
        picoos_uint16 uMax);
static pico_status_t pam_process_event_feature(
        register picodata_ProcessingUnit this, picoos_uint8 nFeat,
        picoos_uint8 event_type, picoos_uint8 direction);
static pico_status_t pam_process_event(register picodata_ProcessingUnit this,
        picoos_uint8 event_type, picoos_uint8 direction);
static pico_status_t pam_adapter_forward_step(
        register picodata_ProcessingUnit this, picoos_uint8 *itemBase);
static pico_status_t pam_adapter_backward_step(
        register picodata_ProcessingUnit this);
static pico_status_t pam_do_pause(register picodata_ProcessingUnit this);
static pico_status_t pam_adapter_do_pauses(register picodata_ProcessingUnit this);
/*-------------- tree traversal ---------------------------------------*/
static pico_status_t pam_expand_vector(register picodata_ProcessingUnit this);
static picoos_uint8 pam_do_tree(register picodata_ProcessingUnit this,
        const picokdt_DtPAM dtpam, const picoos_uint8 *invec,
        const picoos_uint8 inveclen, picokdt_classify_result_t *dtres);
static pico_status_t pam_get_f0(register picodata_ProcessingUnit this,
        picoos_uint16 *lf0Index, picoos_uint8 nState, picoos_single *phonF0);
static pico_status_t pam_get_duration(register picodata_ProcessingUnit this,
        picoos_uint16 durIndex, picoos_uint16 *phonDur,
        picoos_uint8 *numFramesState);
static pico_status_t pam_update_vector(register picodata_ProcessingUnit this);
/*-------------- FINAL ITEM FEEDING -----------------------------------------*/
static pico_status_t pam_put_item(register picodata_ProcessingUnit this,
                picoos_uint8 *outBuff, picoos_uint16 outWritePos,
                picoos_uint8 *bytesWr);

static pico_status_t pam_put_term(picoos_uint8 *outBuff,
        picoos_uint16 outWritePos, picoos_uint8 *bytesWr);

static pico_status_t is_pam_command(const picoos_uint8 *qItem);

static void get_default_boundary_limit(picoos_uint8 uBoundType,
        picoos_uint16 *uMinDur, picoos_uint16 *uMaxDur);

/* -------------------------------------------------------------
 * Pico System functions
 * -------------------------------------------------------------
 */

/**
 * allocation for PAM memory on pam PU
 * @param    mm : handle to engine memory manager
 * @param    pam : handle to a pam struct
 * @return  PICO_OK : allocation successful
 * @return    PICO_ERR_OTHER : allocation errors
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_allocate(picoos_MemoryManager mm, pam_subobj_t *pam)
{
    picoos_uint8 *data;
    picoos_int16 *dataI;

    pam->sSyllFeats = NULL;
    pam->sPhIds = NULL;
    pam->sPhFeats = NULL;
    pam->sSyllItems = NULL;
    pam->sSyllItemOffs = NULL;

    /*-----------------------------------------------------------------
     * PAM Local buffers ALLOCATION
     ------------------------------------------------------------------*/
    /*PAM Local buffers*/
    data = (picopal_uint8 *) picoos_allocate(mm, sizeof(sFtVect)
            * PICOPAM_MAX_SYLL_PER_SENT);
    if (data == NULL)
        return PICO_ERR_OTHER;
    pam->sSyllFeats = (sFtVect*) data;

    data = (picopal_uint8 *) picoos_allocate(mm, sizeof(picopal_uint8)
            * PICOPAM_MAX_PH_PER_SENT);
    if (data == NULL) {
        pam_deallocate(mm, pam);
        return PICO_ERR_OTHER;
    }
    pam->sPhIds = (picopal_uint8*) data;

    data = (picopal_uint8 *) picoos_allocate(mm, sizeof(picopal_uint8)
            * PICOPAM_VECT_SIZE);
    if (data == NULL) {
        pam_deallocate(mm, pam);
        return PICO_ERR_OTHER;
    }
    pam->sPhFeats = (picopal_uint8*) data;

    data = (picopal_uint8 *) picoos_allocate(mm, sizeof(picopal_uint8)
            * PICOPAM_MAX_ITEM_SIZE_PER_SENT);
    if (data == NULL) {
        pam_deallocate(mm, pam);
        return PICO_ERR_OTHER;
    }
    pam->sSyllItems = (picopal_uint8*) data;

    dataI = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICOPAM_MAX_ITEM_PER_SENT);
    if (data == NULL) {
        pam_deallocate(mm, pam);
        return PICO_ERR_OTHER;
    }
    pam->sSyllItemOffs = (picoos_int16*) dataI;

    return PICO_OK;
}/*pam_allocate*/

/**
 * frees allocation for DSP memory on PAM PU
 * @param    mm : memory manager
 * @param    pam : pam PU internal sub-object
 * @return   void
 * @remarks  modified and inserted in sub obj removal PP 15.09.08
 * @callgraph
 * @callergraph
 */
static void pam_deallocate(picoos_MemoryManager mm, pam_subobj_t *pam)
{
    /*-----------------------------------------------------------------
     * Memory de-allocations
     * ------------------------------------------------------------------*/
    if (pam->sSyllFeats != NULL)
        picoos_deallocate(mm, (void *) &pam->sSyllFeats);
    if (pam->sPhIds != NULL)
        picoos_deallocate(mm, (void *) &pam->sPhIds);
    if (pam->sPhFeats != NULL)
        picoos_deallocate(mm, (void *) &pam->sPhFeats);
    if (pam->sSyllItems != NULL)
        picoos_deallocate(mm, (void *) &pam->sSyllItems);
    if (pam->sSyllItemOffs != NULL)
        picoos_deallocate(mm, (void *) &pam->sSyllItemOffs);

}/*pam_deallocate*/

/**
 * initialization of a pam PU
 * @param    this : handle to a PU struct
 * @return     PICO_OK : init OK
 * @return    PICO_ERR_OTHER : error on getting pkbs addresses
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_initialize(register picodata_ProcessingUnit this, picoos_int32 resetMode)
{
    pico_status_t nI, nJ;
    pam_subobj_t *pam;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;
    pam->inBufSize = PICOPAM_IN_BUFF_SIZE;
    pam->outBufSize = PICOPAM_OUT_PAM_SIZE;
    pam->inReadPos = 0;
    pam->inWritePos = 0;
    pam->outReadPos = 0;
    pam->outWritePos = 0;
    pam->needMoreInput = 0;
    pam->procState = 0;

    /*-----------------------------------------------------------------
     * MANAGE INTERNAL INITIALIZATION
     ------------------------------------------------------------------*/
    /*init the syllable structure*/
    for (nI = 0; nI < PICOPAM_MAX_SYLL_PER_SENT; nI++)
        for (nJ = 0; nJ < PICOPAM_VECT_SIZE; nJ++)
            pam->sSyllFeats[nI].phoneV[nJ] = 0;

    for (nI = 0; nI < PICOPAM_MAX_PH_PER_SENT; nI++)
        pam->sPhIds[nI] = 0;

    for (nI = 0; nI < PICOPAM_VECT_SIZE; nI++)
        pam->sPhFeats[nI] = 0;

    for (nI = 0; nI < PICOPAM_MAX_ITEM_SIZE_PER_SENT; nI++)
        pam->sSyllItems[nI] = 0;

    for (nI = 0; nI < PICOPAM_MAX_ITEM_PER_SENT; nI++)
        pam->sSyllItemOffs[nI] = 0;

    /*Other variables*/
    pam_reset_processors(this);
    pam->nLastAttachedItemId = pam->nCurrAttachedItem = 0;
    pam->nAttachedItemsSize = 0;

    if (resetMode == PICO_RESET_SOFT) {
        /*following initializations needed only at startup or after a full reset*/
        return PICO_OK;
    }

    /*pitch and duration modifiers*/
    pam->pMod = 1.0f;
    pam->dMod = 1.0f;
    pam->dRest = 0.0f;


    /* constant tables */
    {
        picoos_uint8 i, j;
        picoos_uint16 tmp_weights[PICOPAM_PWIDX_SIZE][PICOPAM_MAX_STATES_PER_PHONE] = {
        {10, 10, 10, 10, 1 }, /*SBEG*/
        { 1, 4, 8, 4, 1 }, /*PHR1*/
        { 1, 4, 8, 4, 1 }, /*PHR2*/
        { 1, 10, 10, 10, 10 },/*SEND*/
        { 1, 1, 1, 1, 1 } /*DEFAULT*/
        };
        for (i = 0; i < PICOPAM_PWIDX_SIZE; i++) {
            for (j = 0; j < PICOPAM_PWIDX_SIZE; j++) {
                pam->sil_weights[j][j] = tmp_weights[i][j];
            }
        }
    }


/*-----------------------------------------------------------------
     * MANAGE LINGWARE INITIALIZATION IF NEEDED
     ------------------------------------------------------------------*/
    /* kb dtdur */
    pam->dtdur = picokdt_getDtPAM(this->voice->kbArray[PICOKNOW_KBID_DT_DUR]);
    if (pam->dtdur == NULL) {
        picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING, NULL,
                NULL);
        return PICO_ERR_OTHER;
    }PICODBG_DEBUG(("got dtdur"));

    /* kb dtlfz* */
    pam->dtlfz[0] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_LFZ1]);
    pam->dtlfz[1] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_LFZ2]);
    pam->dtlfz[2] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_LFZ3]);
    pam->dtlfz[3] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_LFZ4]);
    pam->dtlfz[4] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_LFZ5]);
    for (nI = 0; nI < PICOPAM_DT_NRLFZ; nI++) {
        if (pam->dtlfz[nI] == NULL) {
            picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                    NULL, NULL);
            return PICO_ERR_OTHER;
        }PICODBG_DEBUG(("got dtlfz%d", nI+1));
    }

    /* kb dtmgc* */
    pam->dtmgc[0] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_MGC1]);
    pam->dtmgc[1] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_MGC2]);
    pam->dtmgc[2] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_MGC3]);
    pam->dtmgc[3] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_MGC4]);
    pam->dtmgc[4] = picokdt_getDtPAM(
            this->voice->kbArray[PICOKNOW_KBID_DT_MGC5]);
    for (nI = 0; nI < PICOPAM_DT_NRMGC; nI++) {
        if (pam->dtmgc[nI] == NULL) {
            picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING,
                    NULL, NULL);
            return PICO_ERR_OTHER;
        }PICODBG_DEBUG(("got dtmgc%d", nI+1));
    }

    /* kb pdfdur* */
    pam->pdfdur = picokpdf_getPdfDUR(
            this->voice->kbArray[PICOKNOW_KBID_PDF_DUR]);
    if (pam->pdfdur == NULL) {
        picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING, NULL,
                NULL);
        return PICO_ERR_OTHER;
    }PICODBG_DEBUG(("got pdfdur"));

    /* kb pdflfz* */
    pam->pdflfz = picokpdf_getPdfMUL(
            this->voice->kbArray[PICOKNOW_KBID_PDF_LFZ]);
    if (pam->pdflfz == NULL) {
        picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING, NULL,
                NULL);
        return PICO_ERR_OTHER;
    }PICODBG_DEBUG(("got pdflfz"));

    /* kb tabphones */
    pam->tabphones = picoktab_getPhones(
            this->voice->kbArray[PICOKNOW_KBID_TAB_PHONES]);
    if (pam->tabphones == NULL) {
        picoos_emRaiseException(this->common->em, PICO_EXC_KB_MISSING, NULL,
                NULL);
        return PICO_ERR_OTHER;
    }PICODBG_DEBUG(("got tabphones"));

    return PICO_OK;
}/*pam_initialize*/

/**
 * termination of a pam PU
 * @param    this : handle to a pam PU struct
 * @return PICO_OK
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_terminate(register picodata_ProcessingUnit this)
{

    pam_subobj_t *pam;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    return PICO_OK;
}/*pam_terminate*/

/**
 * deallocaton of a pam PU
 * @param    this : handle to a pam PU struct
 * @param    mm : engine memory manager
 * @return  PICO_OK
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_subobj_deallocate(register picodata_ProcessingUnit this,
        picoos_MemoryManager mm)
{

    pam_subobj_t* pam;

    if (NULL != this) {
        pam = (pam_subobj_t *) this->subObj;
        mm = mm; /* avoid warning "var not used in this function"*/
        /*-----------------------------------------------------------------
         * Memory de-allocations
         * ------------------------------------------------------------------*/
        if (pam->sSyllFeats != NULL) {
            picoos_deallocate(this->common->mm, (void *) &pam->sSyllFeats);
        }
        if (pam->sPhIds != NULL) {
            picoos_deallocate(this->common->mm, (void *) &pam->sPhIds);
        }
        if (pam->sPhFeats != NULL) {
            picoos_deallocate(this->common->mm, (void *) &pam->sPhFeats);
        }
        if (pam->sSyllItems != NULL) {
            picoos_deallocate(this->common->mm, (void *) &pam->sSyllItems);
        }
        if (pam->sSyllItemOffs != NULL) {
            picoos_deallocate(this->common->mm, (void *) &pam->sSyllItemOffs);
        }
        picoos_deallocate(this->common->mm, (void *) &this->subObj);
    }

    return PICO_OK;
}/*pam_subobj_deallocate*/

/**
 * creates a new pam processing unit
 * @param    mm    : engine memory manager
 * @param    common : engine common object pointer
 * @param    cbIn : pointer to input buffer
 * @param    cbOut : pointer to output buffer
 * @param    voice : pointer to voice structure
 * @return this : pam PU handle if success
 * @return    NULL : if error
 * @callgraph
 * @callergraph
 */
picodata_ProcessingUnit picopam_newPamUnit(picoos_MemoryManager mm,
        picoos_Common common, picodata_CharBuffer cbIn,
        picodata_CharBuffer cbOut, picorsrc_Voice voice)
{

    register pam_subobj_t * pam;

    picodata_ProcessingUnit this = picodata_newProcessingUnit(mm, common, cbIn,
            cbOut, voice);
    if (this == NULL) {
        return NULL;
    }
    this->initialize = pam_initialize;

    PICODBG_DEBUG(("picotok_newPamUnit -- set this->step to pam_step"));

    this->step = pam_step;
    this->terminate = pam_terminate;
    this->subDeallocate = pam_subobj_deallocate;
    this->subObj = picoos_allocate(mm, sizeof(pam_subobj_t));
    if (this->subObj == NULL) {
        PICODBG_ERROR(("Error in Pam Object allocation"));
        picoos_deallocate(mm, (void*) &this);
        return NULL;
    };

    /*-----------------------------------------------------------------
     * Allocate internal memory for PAM (only at PU creation time)
     * ------------------------------------------------------------------*/
    pam = (pam_subobj_t *) this->subObj;
    if (PICO_OK != pam_allocate(mm, pam)) {
        PICODBG_ERROR(("Error in Pam buffers Allocation"));
        picoos_deallocate(mm, (void *) &this->subObj);
        picoos_deallocate(mm, (void *) &this);
        return NULL;
    }

    /*-----------------------------------------------------------------
     * Initialize memory for PAM (this may be re-used elsewhere, e.g.Reset)
     * ------------------------------------------------------------------*/
    if (PICO_OK != pam_initialize(this, PICO_RESET_FULL)) {
        PICODBG_ERROR(("problem initializing the pam sub-object"));
    }
    return this;
}/*picopam_newPamUnit*/

/*-------------------------------------------------------------------------------
 PROCESSING AND INTERNAL FUNCTIONS
 --------------------------------------------------------------------------------*/

/**
 * initializes default duration limits for boundary items
 * @param    uBoundType : type of input boundary type
 * @param    *uMinDur, *uMaxDur : addresses of values to initialize
 * @return  void
 * @remarks    so far initializes to 0 both values; this will leave the values given by tree prediction
 * @callgraph
 * @callergraph
 */
static void get_default_boundary_limit(picoos_uint8 uBoundType,
        picoos_uint16 *uMinDur, picoos_uint16 *uMaxDur)
{
    switch (uBoundType) {
        case PICODATA_ITEMINFO1_BOUND_SBEG:
            *uMinDur = 0;
            *uMaxDur = 20;
            break;
        case PICODATA_ITEMINFO1_BOUND_SEND:
            *uMinDur = 550;
            *uMaxDur = 650;
            break;
        case PICODATA_ITEMINFO1_BOUND_TERM:
            *uMinDur = 0;
            *uMaxDur = 0;
            break;
        case PICODATA_ITEMINFO1_BOUND_PHR0:
            *uMinDur = 0;
            *uMaxDur = 0;
            break;
        case PICODATA_ITEMINFO1_BOUND_PHR1:
            *uMinDur = 275;
            *uMaxDur = 325;
            break;
        case PICODATA_ITEMINFO1_BOUND_PHR2:
            *uMinDur = 4;
            *uMaxDur = 60;
            break;
        case PICODATA_ITEMINFO1_BOUND_PHR3:
            *uMinDur = 0;
            *uMaxDur = 0;
            break;
        default:
            break;
    }

}/*get_default_boundary_limit*/

/**
 * checks if "neededSize" is available on "nCurrPhoneme"
 * @param    pam : pam subobj
 * @param    neededSize : the requested size
 * @return    PICO_OK : size is available
 * @return    !=PICO_OK : size not available
 * @callgraph
 * @callergraph
 */
static pico_status_t check_phones_size(pam_subobj_t *pam,
        picoos_int16 neededSize)
{
    if ((pam->nCurrPhoneme + neededSize) > PICOPAM_MAX_PH_PER_SENT - 1) {
        return PICO_ERR_OTHER;
    }
    return PICO_OK;
}/*check_phones_size*/

/**
 * checks if neededSize is available on "nCurrSyllable"
 * @param    pam : pam subobj
 * @param    neededSize : the requested size
 * @return    PICO_OK : size is available
 * @return    !=PICO_OK : size not available
 * @callgraph
 * @callergraph
 */
static pico_status_t check_syllables_size(pam_subobj_t *pam,
        picoos_int16 neededSize)
{
    if ((pam->nCurrSyllable + neededSize) > PICOPAM_MAX_SYLL_PER_SENT - 1) {
        return PICO_ERR_OTHER;
    }
    return PICO_OK;
}/*check_syllables_size*/

/**
 * verifies that local storage has enough space to receive 1 item
 * @param    this : pointer to current PU struct
 * @param    item : pointer to current item head
 * @return    TRUE : resource limits would be reached during processing of input item
 * @return    FALSE : item could be processed normally
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t pamCheckResourceLimits(
        register picodata_ProcessingUnit this, const picoos_uint8 *item)
{
    register pam_subobj_t * pam;
    picodata_itemhead_t head;
    pico_status_t sResult;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;
    sResult = TRUE; /*default : resource limits reached*/
    head.type = item[0];
    head.info1 = item[1];
    head.info2 = item[2];
    head.len = item[3];

    switch (head.type) {
        /*commands that generate syllables/phonemes*/
        case PICODATA_ITEM_SYLLPHON:
            if (pam->nCurrSyllable >= PICOPAM_MAX_SYLL_PER_SENT - 2) {
                return sResult; /*no room for more syllables*/
            }
            if ((pam->nCurrPhoneme + head.len) >= PICOPAM_MAX_PH_PER_SENT - 2) {
                return sResult; /*no room for more phoneme*/
            }
            break;
        case PICODATA_ITEM_BOUND:
            if ((head.info1 == PICODATA_ITEMINFO1_BOUND_SBEG) || (head.info1
                    == PICODATA_ITEMINFO1_BOUND_SEND) || (head.info1
                    == PICODATA_ITEMINFO1_BOUND_TERM) || (head.info1
                    == PICODATA_ITEMINFO1_BOUND_PHR1)
#ifdef PAM_PHR2_WITH_PR1
                    || (head.info1 == PICODATA_ITEMINFO1_BOUND_PHR2)
#endif
            ) {

                if (pam->nCurrSyllable >= PICOPAM_MAX_SYLL_PER_SENT - 2) {
                    return sResult; /*no room for more syllables*/
                }
                if ((pam->nCurrPhoneme + 1) >= PICOPAM_MAX_PH_PER_SENT - 2) {
                    return sResult; /*no room for more phoneme*/
                }
            }
            break;

        default:
            /*all other commands has to be queued*/
            if ((pam->nAttachedItemsSize + head.len)
                    >= PICOPAM_MAX_ITEM_SIZE_PER_SENT - 1) {
                return sResult; /*no room for more items*/
            }
            break;
    }
    return FALSE; /*no resource limits apply to current item*/
} /*pamCheckResourceLimits*/

/**
 * selects items to be sent to next PU immedately
 * @param    this : pointer to current PU struct
 * @param    item : pointer to current item head
 * @return    TRUE : item should be passed on next PU NOW
 * @return    FALSE : item should not be passed on next PU now but should be processed
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_check_immediate(register picodata_ProcessingUnit this,
        const picoos_uint8 *item)
{
    register pam_subobj_t * pam;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;

    if (pam->nCurrSyllable <= -1) {
        if (item[0] == PICODATA_ITEM_SYLLPHON)
            return FALSE;
        if ((item[0] == PICODATA_ITEM_BOUND) && (item[1]
                == PICODATA_ITEMINFO1_BOUND_SBEG))
            return FALSE;
        if (is_pam_command((picoos_uint8 *) item) == TRUE)
            return FALSE;
        return TRUE; /*no need to process data : send it*/
    }
    return FALSE; /*syllable struct not void : do standard processing*/

} /*pam_check_immediate*/

/**
 * checks if the input item has to be queued in local storage for later resynch
 * @param    this : pointer to current PU struct
 * @param    item : pointer to current item head
 * @return    TRUE : item should be queued
 * @return    FALSE : item should not be queued
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_hastobe_queued(register picodata_ProcessingUnit this,
        const picoos_uint8 *item)
{
    register pam_subobj_t * pam;
    picodata_itemhead_t head;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;
    head.type = item[0];
    head.info1 = item[1];

    switch (head.type) {
        /*commands that generate syllables/phonemes*/
        case PICODATA_ITEM_SYLLPHON:
            return FALSE; /*no queue needed*/
            break;
        case PICODATA_ITEM_BOUND:
            if ((head.info1 == PICODATA_ITEMINFO1_BOUND_PHR3)
#ifdef PAM_PHR2_WITH_PR3
                    ||(head.info1==PICODATA_ITEMINFO1_BOUND_PHR2)
#endif
                    || (head.info1 == PICODATA_ITEMINFO1_BOUND_PHR0)) {
                return FALSE; /*no queue needed*/
            }
            break;

        default:
            /*all other items has to be queued*/
            break;
    }
    return TRUE; /*item has to be queued*/
} /*pam_hastobe_queued*/

/**
 * queue item in local storage for later resynch
 * @param    this : pointer to current PU struct
 * @param    item : pointer to current item head
 * @return    TRUE : item queued
 * @return    FALSE : item not queued because of errors
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_queue(register picodata_ProcessingUnit this,
        const picoos_uint8 *item)
{
    register pam_subobj_t * pam;
    picodata_itemhead_t head;
    picoos_uint8 nI;
    pico_status_t sResult;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;
    sResult = TRUE; /*default : item queued*/
    head.type = item[0];
    head.info1 = item[1];
    head.info2 = item[2];
    head.len = item[3];

    /*test condition on enough room to store current item in the "sSyllItems" area*/
    if ((pam->nAttachedItemsSize + head.len + sizeof(picodata_itemhead_t))
            >= PICOPAM_MAX_ITEM_SIZE_PER_SENT - 1) {
        return FALSE; /*resource limit reached*/
    }
    /*store current offset*/
    pam->sSyllItemOffs[pam->nLastAttachedItemId] = pam->nAttachedItemsSize;
    /*store the item to the "sSyllItems" area*/
    for (nI = 0; nI < (head.len + sizeof(picodata_itemhead_t)); nI++) {
        pam->sSyllItems[pam->nAttachedItemsSize + nI] = item[nI];
    }
    /*increment the attached items area*/
    pam->nAttachedItemsSize += nI;

    /*increment id*/
    pam->nLastAttachedItemId++;
    /*set start(if not initialized) and end ids of queued items in sSyllFeats*/
    if (pam->nCurrSyllable > -1) {
        /*normal case : the item is attached to current syllable*/
        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM] == 0) {
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM]
                    = pam->nLastAttachedItemId;
        }
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm]
                = pam->nLastAttachedItemId;
    } else {
        /*special case : an item is requested to be queued even if no
         syllables has been assigned to the sentence structure :
         -->> use syll 0*/
        if (pam->sSyllFeats[0].phoneV[ITM] == 0) {
            pam->sSyllFeats[0].phoneV[ITM] = pam->nLastAttachedItemId;
        }
        pam->sSyllFeats[0].phoneV[itm] = pam->nLastAttachedItemId;
    }
    return TRUE; /*item queued successfully*/
} /*pam_queue*/

/**
 * selects items to be dealth with by the PU processing
 * @param    item : pointer to current item head
 * @return    TRUE : item should be processed
 * @return    FALSE : item should not be processed (maybe it ontains commands or items for other PUs)
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_deal_with(const picoos_uint8 *item)
{
    picodata_itemhead_t head;
    pico_status_t sResult;
    sResult = FALSE;
    head.type = item[0];
    head.info1 = item[1];
    head.info2 = item[2];
    head.len = item[3];
    switch (head.type) {
        case PICODATA_ITEM_SYLLPHON:
        case PICODATA_ITEM_BOUND:
            sResult = TRUE;
            break;
        default:
            break;
    }
    return sResult;
} /*pam_deal_with*/

/**
 * returns true if more items has to be produced for current syllable
 * @param    this : Pam object pointer
 * @return    TRUE : item is to be produced
 * @return    FALSE : item is not to be produced
 * @remarks item pointed to by *item should be already valid
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pamHasToProcess(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;
    picoos_uint8 nCond1, nCond2, nCond3;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;
    /*conditions originating a "NOT to be processed" result */
    nCond1 = pam->nCurrSyllable <= -1;
    nCond2 = pam->nCurrSyllable >= pam->nTotalSyllables;
    nCond3 = pam->nSyllPhoneme
            >= pam->sSyllFeats[pam->nCurrSyllable].phoneV[B3];

    if ((nCond1) || (nCond2) || (nCond3))
        return FALSE;

    return TRUE;
} /*pamHasToProcess*/

/**
 * modifies the process flags in order to point to next valid syllable phone or item to be produced
 * @param    this : Pam object pointer
 * @return    TRUE : item has to be produced
 * @return    FALSE : item has not to be produced
 * @callgraph
 * @callergraph
 */
static pico_status_t pamUpdateProcess(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;

    if (pam->nCurrSyllable == -1) {
        /*this to be able to manage sudden PU cleanup after FLUSH CMD*/
        return PICO_OK;
    }
    /*check number of phonemes for current syllable*/
    if (pam->nSyllPhoneme < pam->sSyllFeats[pam->nCurrSyllable].phoneV[B3] - 1) {
        pam->nSyllPhoneme++;
        return PICO_OK;
    }
    if (pam->nSyllPhoneme == pam->sSyllFeats[pam->nCurrSyllable].phoneV[B3] - 1) {
        /*this helps in identifyng the end of syllable condition in PamHasToProcess*/
        pam->nSyllPhoneme++;
    }
    /*previous syllable phonemes are complete: test if any items are tied to this syllable*/
    if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM] > 0) {
        /*there are items tied to this syllable*/
        if (pam->nCurrAttachedItem == 0) {
            /*if it is the first item to be regenerated initialize it*/
            pam->nCurrAttachedItem
                    = pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM];
            return PICO_OK;
        } else {
            /*not the first item : check if more*/
            if (pam->nCurrAttachedItem
                    < pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm]) {
                /*more tied items to be regenerated*/
                pam->nCurrAttachedItem++;
                return PICO_OK;
            }
        }
    }
    /*previous syllable phonemes and items are complete: switch to next syllable*/
    if (pam->nCurrSyllable < pam->nTotalSyllables - 1) {
        pam->nCurrSyllable++;
        pam->nSyllPhoneme = 0;
        pam->nCurrAttachedItem = 0;
        return PICO_OK;
    }
    /*no more phonemes or items to be produced*/
    pam->nCurrSyllable++;
    pam->nSyllPhoneme = 0;
    return PICO_ERR_OTHER;

} /*pamUpdateProcess*/

/**
 * returns true if more items has to be popped for current syllable
 * @param    this : Pam object pointer
 * @return    TRUE : item has to be popped
 * @return    FALSE : item has not to be popped
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pamHasToPop(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*Preliminary condition : at least 1 syllable*/
    if (pam->nCurrSyllable <= -1)
        return FALSE;

    /*Preliminary condition : not maximum number of syllables*/
    if (pam->nCurrSyllable >= pam->nTotalSyllables)
        return FALSE;

    /*Preliminary condition : start and end offset in current item > 0 */
    if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM] <= 0)
            || (pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm] <= 0))
        return FALSE;

    /*Final condition : current popped item less or eq to maximum*/
    if (pam->nCurrAttachedItem
            > pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm])
        return FALSE;

    return TRUE;
} /*pamHasToPop*/

/**
 * returns the address of an item to be popped from the current syllable queue
 * @param    this : Pam object pointer
 * @return    pop_address : item address
 * @return    NULL : item not poppable
 * @callgraph
 * @callergraph
 */
static picoos_uint8 *pamPopItem(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;
    picoos_uint8 nItem;
    if (NULL == this || NULL == this->subObj) {
        return NULL;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*Preliminary condition : at least 1 syllable*/
    if (pam->nCurrSyllable <= -1)
        return NULL;

    /*Preliminary condition : not maximum number of syllables*/
    if (pam->nCurrSyllable >= pam->nTotalSyllables)
        return NULL;

    /*Preliminary condition : start and end offset in current item > 0 */
    if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM] <= 0)
            || (pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm] <= 0))
        return NULL;

    /*Final condition : current popped item less than maximum*/
    if (pam->nCurrAttachedItem
            > pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm])
        return NULL;

    nItem = pam->nCurrAttachedItem;
    /*please note : nItem-1 should match with actions performed in function "pam_queue(..)" */
    return &(pam->sSyllItems[pam->sSyllItemOffs[nItem - 1]]);

} /*pamPopItem*/

/**
 * returns the address of an item popped from the syllable 0 queue
 * @param    this : Pam object pointer
 * @return    pop_address : item address
 * @return    NULL : item not poppable
 * @remarks the item is popped only if it has been inserted in the queue before the first
 * @remarks item assigned to the syllable 0 i.e.
 * @remarks AttachedItem<=pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm]-1
 * @callgraph
 * @callergraph
 */
static picoos_uint8 *pamPopAttachedSy0(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;
    picoos_uint8 nItem;
    if (NULL == this || NULL == this->subObj) {
        return NULL;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*should be syllable 0*/
    if (pam->nCurrSyllable != 0)
        return NULL;

    /*start and end offset in current item > 0 */
    if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[ITM] <= 0)
            || (pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm] <= 0))
        return NULL;

    /*if current popped item is > 0 test end condition*/
    if (pam->nCurrAttachedItem > 0) {
        /*Other condition : current popped item less than maximum*/
        if (pam->nCurrAttachedItem
                > pam->sSyllFeats[pam->nCurrSyllable].phoneV[itm] - 1)
            return NULL;
    }
    nItem = pam->nCurrAttachedItem;
    return &(pam->sSyllItems[pam->sSyllItemOffs[nItem]]);

} /*pamPopAttachedSy0*/

/**
 * pdf access for duration
 * @param    this : Pam object pointer
 * @param    durIndex : index of duration in the pdf
 * @param    phonDur : pointer to base of array where to store the duration values
 * @param    numFramesState : pointer to base of array where to store the number of frames per state
 * @return    PICO_OK : pdf retrieved
 * @return    PICO_ERR_OTHER : pdf not retrieved
 * @remarks Modifies phonDur (the requested duration value)
 * @remarks Modifies numFramesState (the requested number of frames per state (vector))
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_get_duration(register picodata_ProcessingUnit this,
        picoos_uint16 durIndex, picoos_uint16 *phonDur,
        picoos_uint8 *numFramesState)
{
    pam_subobj_t *pam;
    picokpdf_PdfDUR pdf;
    picoos_uint8 *durItem;
    picoos_uint16 nFrameSize, nI;
    picoos_single fValue;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pdf = pam->pdfdur;
    /*make the index 0 based*/
    if (durIndex > 0)
        durIndex--;

    /* check */
    if (durIndex > pdf->numframes - 1) {
        PICODBG_ERROR(("PAM durPdf access error, index overflow -> index: %d , numframes: %d", durIndex, pdf->numframes));
        return PICO_ERR_OTHER;
    }
    /* base pointer */
    durItem = &(pdf->content[durIndex * pdf->vecsize]);
    if (durItem == NULL) {
        PICODBG_ERROR(("PAM durPdf access error , frame pointer = NULL"));
        return PICO_ERR_OTHER;
    }
    nFrameSize = pdf->sampperframe / 16;
    *phonDur = ((pdf->phonquant[((*durItem) & 0xF0) >> 4]) * nFrameSize);
    numFramesState[0] = pdf->statequant[((*durItem) & 0x0F)];
    durItem++;
    numFramesState[1] = pdf->statequant[((*durItem) & 0xF0) >> 4];
    numFramesState[2] = pdf->statequant[((*durItem) & 0x0F)];
    durItem++;
    numFramesState[3] = pdf->statequant[((*durItem) & 0xF0) >> 4];
    numFramesState[4] = pdf->statequant[((*durItem) & 0x0F)];

    /*modification of the duration information based on the duration modifier*/
    *phonDur = (picoos_uint16) (((picoos_single) * phonDur) * pam->dMod);
    for (nI = 0; nI < 5; nI++) {
        fValue = pam->dRest + (picoos_single) numFramesState[nI] * pam->dMod;
        numFramesState[nI] = (picoos_uint8) (fValue);
        pam->dRest = fValue - (picoos_single) numFramesState[nI];
    }
    return PICO_OK;
}/*pam_get_duration*/

/**
 * pdf access for pitch
 * @param    this : Pam object pointer
 * @param    lf0Index : pointer to variable to receive index of pitch in the pdf
 * @param    nI : number of the phone's state
 * @param    phonF0 : pointer to variable to receive the pitch value
 * @return    PICO_OK : pdf retrieved
 * @return    PICO_ERR_OTHER : pdf not retrieved
 * @remarks Modifies phonDur (the requested duration value)
 * @remarks Modifies phonF0 (the requested pitch value (scalar))
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_get_f0(register picodata_ProcessingUnit this,
        picoos_uint16 *lf0Index, picoos_uint8 nI, picoos_single *phonF0)
{
    pam_subobj_t *pam;
    picoos_uint8 *lfItem, numstreams;
    picoos_uint16 lf0IndexOffset, sTemp;
    picoos_single lfum, lfivar, lfz;

    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    lf0IndexOffset = lf0Index[nI];

    /*make the index 0 based*/
    if (lf0IndexOffset > 0)
        lf0IndexOffset--;

    lf0IndexOffset += pam->pdflfz->stateoffset[nI];
    if (lf0IndexOffset > pam->pdflfz->numframes - 1) {
        PICODBG_ERROR(("PAM flfzPdf access error, index overflow -> index: %d , numframes: %d", lf0Index, pam->pdflfz->numframes));
        return PICO_ERR_OTHER;
    }
    /* base pointer */
    lf0IndexOffset *= pam->pdflfz->vecsize;

    lfItem = &(pam->pdflfz->content[lf0IndexOffset]);
    sTemp = (picoos_uint16) (((lfItem[1] << 8)) | lfItem[0]);

    lfum = (picoos_single) (sTemp << (pam->pdflfz->meanpowUm[0]));
    numstreams = 3;
    lfivar = (picoos_single) (((picoos_uint16) lfItem[numstreams * 2])
            << pam->pdflfz->ivarpow[0]);
    lfz = (picoos_single) lfum / (picoos_single) lfivar;
    lfz = (picoos_single) exp((double) lfz);
    phonF0[nI] = (picoos_single) lfz;

    /*pitch modoification*/
    phonF0[nI] *= pam->pMod;
    return PICO_OK;
}/*pam_get_f0*/

/**
 * elementary rounding function
 * @param    fIn : (real) input value
 * @return    the rounded value
 * @callgraph
 * @callergraph
 */
static picoos_single f_round(picoos_single fIn)
{
    picoos_int32 iVal;
    picoos_single fVal;

    iVal = (picoos_int32) fIn;
    fVal = (picoos_single) iVal;

    if (fIn > (picoos_single) 0.0f) {
        if ((fIn - fVal) < (picoos_single) 0.5f)
            return fVal;
        else
            return fVal + (picoos_single) 1.0f;
    } else {
        if ((fVal - fIn) < (picoos_single) 0.5f)
            return fVal;
        else
            return fVal - (picoos_single) 1.0f;
    }
}/*f_round*/

/**
 * updates the input vector for PAM
 * @param    this : Pam object pointer
 * @return    PICO_OK : update successful
 * @return    PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @remarks Modifies pam->sPhFeats[]
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_update_vector(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    picoos_uint8 numstates, nI;
    picoos_single fDur, f0avg, f0quant, minf0, maxf0, durquant1, durquant2,
            mindur, maxdur1, maxdur2;

    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    /*default init*/
    pam->sPhFeats[DUR] = 0;
    pam->sPhFeats[F0] = 0;
    /*
     Hard coded parameters for quantization
     */
    numstates = PICOPAM_NRSTPF;
    f0quant = 30.0f;
    minf0 = 90.0f;
    maxf0 = 360.0f;

    durquant1 = 20.0f;
    durquant2 = 100.0f;
    mindur = 40.0f;
    maxdur1 = 160.0f;
    maxdur2 = 600.0f;
    f0avg = 0.0f;
    for (nI = 0; nI < numstates; nI++)
        f0avg += pam->phonF0[nI];
    f0avg /= (picoos_single) numstates;

    f0avg = f_round(f0avg / f0quant) * f0quant;
    if (f0avg < minf0)
        f0avg = minf0;
    if (f0avg > maxf0)
        f0avg = maxf0;

    /*make initial silence of sentence shorter (see also pam_put_item)*/
    if ((pam->nCurrSyllable == 0) && (pam->nSyllPhoneme == 0)) {
        pam->phonDur = 2 * 4;
    }

    fDur = (picoos_single) pam->phonDur;
    fDur = f_round(fDur / durquant1) * durquant1;
    if (fDur < mindur)
        fDur = mindur;
    if (fDur > maxdur1) {
        fDur = f_round(fDur / durquant2) * durquant2;
        if (fDur > maxdur2)
            fDur = maxdur2;
    }
    pam->sPhFeats[DUR] = (picoos_uint8) (fDur / (picoos_single) 10.0f);
    pam->sPhFeats[F0] = (picoos_uint8) (f0avg / (picoos_single) 10.0f);

    return PICO_OK;
}/*pam_update_vector*/

/**
 * compress a single feature in the range 0..9
 * @param    inVal : the value to be compressed
 * @return    compVal : the compressed value
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pamCompressComponent(picoos_uint8 inVal)
{
    if (inVal <= 5)
        return inVal;
    if ((5 < inVal) && (inVal <= 10))
        return 6;
    if ((10 < inVal) && (inVal <= 20))
        return 7;
    if ((20 < inVal) && (inVal <= 30))
        return 8;
    return 9;
}/*pamCompressComponent*/

/**
 * prepares the input vector for tree feeding
 * @param    this : Pam object pointer
 * @return    PICO_OK : vector expanded
 * @return    PICO_ERR_OTHER : errors on expansion or retrieving the PU pointer
 * @remarks Modifies pam->sPhFeats[]
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_expand_vector(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    picoos_uint8 *inVect, *phonVect, *outVect, nI;
    picoos_int16 nOffs, nOffs1, nLen;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    inVect = &(pam->sSyllFeats[pam->nCurrSyllable].phoneV[0]);
    phonVect = &(pam->sPhIds[0]);
    outVect = &(pam->sPhFeats[0]);
    /*just copy back*/
    for (nI = 0; nI < PICOPAM_INVEC_SIZE; nI++)
        outVect[nI] = inVect[nI];
    /*now fill missing fields*/
    picoos_mem_copy((void*) &(inVect[FID]), &nOffs, sizeof(nOffs));
    /*offset to first phone of current syllable*/
    nOffs = nOffs + pam->nSyllPhoneme; /*offset to current phone of current syllable*/
    nLen = inVect[B3]; /*len of current syllable*/
    if (pam->nSyllPhoneme >= nLen) {
        /*error on addressing current phone*/
        return PICO_ERR_OTHER;
    }
    /*previous of the previous phone*/
    nOffs1 = nOffs - 2;
    if (nOffs1 >= 0)
        outVect[P1] = phonVect[nOffs1];
    else
        outVect[P1] = PICOPAM_PH_DONT_CARE_VAL;
    /*previous  phone*/
    nOffs1 = nOffs - 1;
    if (nOffs1 >= 0)
        outVect[P2] = phonVect[nOffs1];
    else
        outVect[P2] = PICOPAM_PH_DONT_CARE_VAL;
    /*^current phone*/
    outVect[P3] = phonVect[nOffs];

    /*next phone*/
    nOffs1 = nOffs + 1;
    if (nOffs1 < pam->nTotalPhonemes)
        outVect[P4] = phonVect[nOffs1];
    else
        outVect[P4] = PICOPAM_PH_DONT_CARE_VAL;
    /*next of the next phone*/
    nOffs1 = nOffs + 2;
    if (nOffs1 < pam->nTotalPhonemes)
        outVect[P5] = phonVect[nOffs1];
    else
        outVect[P5] = PICOPAM_PH_DONT_CARE_VAL;
    /*pos of curr phone with respect to left syllable boundary*/
    outVect[P6] = pam->nSyllPhoneme + 1;
    /*pos of curr phone with respect to right syllable boundary*/
    outVect[P7] = nLen - pam->nSyllPhoneme;
    /*is current phone in consonant syllable boundary? (1:yes)*/
    if (pam->nSyllPhoneme < inVect[P8])
        outVect[P8] = 1;
    else
        outVect[P8] = 0;
    return PICO_OK;
}/*pam_expand_vector*/

/**
 * compresses the input vector for PAM
 * @param    this : Pam object pointer
 * @return    PICO_OK : compression successful
 * @return    PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @remarks Modifies pam->sPhFeats[]
 * @callgraph
 * @callergraph
 */
static pico_status_t pamCompressVector(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    picoos_uint8 *outVect, nI;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    outVect = &(pam->sPhFeats[0]);
    for (nI = 0; nI < PICOPAM_INVEC_SIZE; nI++) {
        switch (nI) {
            case P1:
            case P2:
            case P3:
            case P4:
            case P5:
            case B1:
            case B2:
            case B16:
            case E1:
            case H5:
                /*don't do any compression*/
                break;
            default:
                /*do compression*/
                if (outVect[nI] != PICOPAM_DONT_CARE_VALUE)
                    outVect[nI] = pamCompressComponent(outVect[nI]);
                else
                    outVect[nI] = PICOPAM_DONT_CARE_VAL;
                break;
        }
    }
    return PICO_OK;
}/*pamCompressVector*/

/**
 * reorganizes the input vector for PAM
 * @param    this : Pam object pointer
 * @return    PICO_OK : reorganization successful
 * @return    PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @remarks Modifies pam->sPhFeats[]
 * @callgraph
 * @callergraph
 */
static pico_status_t pamReorgVector(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    picoos_uint8 *outVect, inVect[60], nI;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    outVect = &(pam->sPhFeats[0]);
    for (nI = 0; nI < PICOPAM_INVEC_SIZE; nI++) inVect[nI] = outVect[nI];
    /*reorganize*/
    for (nI = T_B1; nI <= T_H5; nI++) {
        switch (nI) {
            case T_B1:
                outVect[T_B1] = inVect[B1];
                break;
            case T_B2:
                outVect[T_B2] = inVect[B2];
                break;
            case T_B3:
                outVect[T_B3] = inVect[B3];
                break;
            case T_B4:
                outVect[T_B4] = inVect[B4];
                break;
            case T_B5:
                outVect[T_B5] = inVect[B5];
                break;
            case T_B6:
                outVect[T_B6] = inVect[B6];
                break;
            case T_B7:
                outVect[T_B7] = inVect[B7];
                break;
            case T_B8:
                outVect[T_B8] = inVect[B8];
                break;
            case T_B9:
                outVect[T_B9] = inVect[B9];
                break;
            case T_B10:
                outVect[T_B10] = inVect[B10];
                break;
            case T_B11:
                outVect[T_B11] = inVect[B11];
                break;
            case T_B12:
                outVect[T_B12] = inVect[B12];
                break;
            case T_B13:
                outVect[T_B13] = inVect[B13];
                break;
            case T_B14:
                outVect[T_B14] = inVect[B14];
                break;
            case T_B15:
                outVect[T_B15] = inVect[B15];
                break;
            case T_B16:
                outVect[T_B16] = inVect[B16];
                break;
            case T_B17:
                outVect[T_B17] = inVect[B17];
                break;
            case T_B18:
                outVect[T_B18] = inVect[B18];
                break;
            case T_B19:
                outVect[T_B19] = inVect[B19];
                break;
            case T_B20:
                outVect[T_B20] = inVect[B20];
                break;
            case T_B21:
                outVect[T_B21] = inVect[B21];
                break;

            case T_E1:
                outVect[T_E1] = inVect[E1];
                break;
            case T_E2:
                outVect[T_E2] = inVect[E2];
                break;
            case T_E3:
                outVect[T_E3] = inVect[E3];
                break;
            case T_E4:
                outVect[T_E4] = inVect[E4];
                break;
            case T_E5:
                outVect[T_E5] = inVect[E5];
                break;
            case T_E6:
                outVect[T_E6] = inVect[E6];
                break;
            case T_E7:
                outVect[T_E7] = inVect[E7];
                break;
            case T_E8:
                outVect[T_E8] = inVect[E8];
                break;
            case T_E9:
                outVect[T_E9] = inVect[E9];
                break;
            case T_E10:
                outVect[T_E10] = inVect[E10];
                break;
            case T_E11:
                outVect[T_E11] = inVect[E11];
                break;
            case T_E12:
                outVect[T_E12] = inVect[E12];
                break;
            case T_E13:
                outVect[T_E13] = inVect[E13];
                break;

            case T_A3:
                outVect[T_A3] = inVect[A3];
                break;
            case T_C3:
                outVect[T_C3] = inVect[C3];
                break;
            case T_D2:
                outVect[T_D2] = inVect[D2];
                break;
            case T_F2:
                outVect[T_F2] = inVect[F2];
                break;

            case T_G1:
                outVect[T_G1] = inVect[G1];
                break;
            case T_I1:
                outVect[T_I1] = inVect[I1];
                break;

            case T_G2:
                outVect[T_G2] = inVect[G2];
                break;
            case T_I2:
                outVect[T_I2] = inVect[I2];
                break;

            case T_H1:
                outVect[T_H1] = inVect[H1];
                break;
            case T_H2:
                outVect[T_H2] = inVect[H2];
                break;
            case T_H3:
                outVect[T_H3] = inVect[H3];
                break;
            case T_H4:
                outVect[T_H4] = inVect[H4];
                break;
            case T_H5:
                outVect[T_H5] = inVect[H5];
                break;
        }
    }
    return PICO_OK;
}/*pamReorgVector*/

/**
 * puts a PAM item into PU output buffer
 * @param    this : Pam object pointer
 * @param    outBuff    : output buffer base pointer
 * @param    outWritePos : offset in output buffer
 * @param    *bytesWr : actual bytes written
 * @return    PICO_OK : put successful
 * @return    PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_put_item(register picodata_ProcessingUnit this,
        picoos_uint8 *outBuff, picoos_uint16 outWritePos, picoos_uint8 *bytesWr)
{
    pam_subobj_t *pam;
    picoos_uint8 *sDest, nI, nType, nIdx, fde;
    picoos_uint32 pos, pos32;
    picoos_int16 ft, dt;
    picoos_uint16 uMinDur, uMaxDur;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    sDest = &(outBuff[outWritePos]);
    sDest[0] = PICODATA_ITEM_PHONE; /*Item type*/
    sDest[1] = pam->sPhFeats[P3]; /*phonetic id*/
    sDest[2] = PICOPAM_NRSTPF; /*number of states per phone*/
    sDest[3] = sizeof(picoos_uint16) * PICOPAM_NRSTPF * 3; /*size of the item*/
    pos = 4;
    /*make initial silence of sentence shorter (see also UpdateVector)*/
    if ((pam->nCurrSyllable == 0) && (pam->nSyllPhoneme == 0)) {
        for (nI = 0; nI < PICOPAM_NRSTPF - 1; nI++)
            pam->numFramesState[nI] = 0;
        pam->numFramesState[nI] = 2;
    } else {
        /*manage silence syllables with prescribed durations*/
        pos32 = Min;
        picoos_read_mem_pi_uint16(pam->sSyllFeats[pam->nCurrSyllable].phoneV,
                &pos32, &uMinDur);
        pos32 = Max;
        picoos_read_mem_pi_uint16(pam->sSyllFeats[pam->nCurrSyllable].phoneV,
                &pos32, &uMaxDur);

        if (uMaxDur > 0) {
            /* Select weights*/
            nType = pam->sSyllFeats[pam->nCurrSyllable].phoneV[bnd];
            switch (nType) {
                case PICODATA_ITEMINFO1_BOUND_SBEG:
                    nIdx = PICOPAM_PWIDX_SBEG;
                    break;
                case PICODATA_ITEMINFO1_BOUND_PHR1:
                    nIdx = PICOPAM_PWIDX_PHR1;
                    break;
                case PICODATA_ITEMINFO1_BOUND_PHR2:
                    nIdx = PICOPAM_PWIDX_PHR2;
                    break;
                case PICODATA_ITEMINFO1_BOUND_SEND:
                case PICODATA_ITEMINFO1_BOUND_TERM:
                    nIdx = PICOPAM_PWIDX_SEND;
                    break;
                default:
                    nIdx = PICOPAM_PWIDX_DEFA;
                    break;
            }
            fde = 2;
            ft = 0;
            dt = 0;
            picodata_transformDurations(
                    fde,            /* 2's exponent of frame duration in ms, e.g. 2 for 4ms, 3 for 8ms */
                    PICOPAM_NRSTPF, /* number of states per phone */
                    &(pam->numFramesState[0]), /* estimated durations */
                    pam->sil_weights[nIdx],  /* integer weights */
                    uMinDur,        /* minimum target duration in ms */
                    uMaxDur,        /* maximum target duration in ms */
                    ft,             /* factor to be multiplied to get the target */
                    &dt             /* in/out, rest in ms */
                    );
        }
    }
    /*put data*/
    for (nI = 0; nI < PICOPAM_NRSTPF; nI++) {
        picoos_write_mem_pi_uint16(sDest, &pos,
                (picoos_uint16) pam->numFramesState[nI]);
        picoos_write_mem_pi_uint16(sDest, &pos,
                (picoos_uint16) pam->lf0Index[nI]);
        picoos_write_mem_pi_uint16(sDest, &pos,
                (picoos_uint16) pam->mgcIndex[nI]);
    }
    *bytesWr = sizeof(picodata_itemhead_t) + sizeof(picoos_uint16)
            * PICOPAM_NRSTPF * 3;
    return PICO_OK;
}/*pam_put_item*/

/**
 * puts a non PAM (queued) item into PU output buffer
 * @param    qItem : pointer to item to put
 * @param    outBuff    : output buffer base pointer
 * @param    outWritePos : offset in output buffer
 * @param    *bytesWr : actual bytes written
 * @return    PICO_OK : put successful
 * @return    PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_put_qItem(picoos_uint8 *qItem, picoos_uint8 *outBuff,
        picoos_uint16 outWritePos, picoos_uint8 *bytesWr)
{
    picoos_uint8 *sDest, nI;
    sDest = &(outBuff[outWritePos]);
    *bytesWr = sizeof(picodata_itemhead_t);
    for (nI = 0; nI < (sizeof(picodata_itemhead_t) + qItem[3]); nI++) {
        sDest[nI] = qItem[nI];
    }
    *bytesWr = nI;
    return PICO_OK;
}/*pam_put_qItem*/

/**
 * tells if an item is a PAM command (except play)
 * @param    qItem : input item to test
 * @return    TRUE : qItem is a PAM command (except play)
 * @return    FALSE : qItem not a PAM command
 * @callgraph
 * @callergraph
 */
static pico_status_t is_pam_command(const picoos_uint8 * qItem)
{
    switch (qItem[0]) {

        case PICODATA_ITEM_CMD:
            switch (qItem[1]) {
                case PICODATA_ITEMINFO1_CMD_FLUSH:
                    /* flush is for all PU's and as such it is also for PAM*/
                case PICODATA_ITEMINFO1_CMD_PITCH:
                case PICODATA_ITEMINFO1_CMD_SPEED:
                    return TRUE;
                    break;
                default:
                    break;
            }
    }
    return FALSE;
}/*is_pam_command*/

/**
 * tells if an item is a PAM PLAY command
 * @param    qItem : input item to test
 * @return    TRUE : qItem is a PAM PLAY command
 * @return    FALSE : qItem not a PAM PLAY command
 * @callgraph
 * @callergraph
 */
static pico_status_t is_pam_play_command(picoos_uint8 *qItem)
{
    switch (qItem[0]) {

        case PICODATA_ITEM_CMD:
            switch (qItem[1]) {
                case PICODATA_ITEMINFO1_CMD_PLAY:
                    if (qItem[2] == PICODATA_ITEMINFO2_CMD_TO_PAM)
                        return TRUE;
                    break;
                default:
                    break;
            }
    }
    return FALSE;
}/*is_pam_play_command*/

/**
 * command processor for PAM pu
 * @param    this : Pam item subobject
 * @param    qItem : input item pointer
 * @return    PICOPAM_FLUSH_RECEIVED : when a FLUSH is received
 * @return    PICOPAM_CONTINUE : normal command processing
 * @return    PICODATA_PU_ERROR : errors in accessing data
 * @callgraph
 * @callergraph
 */
static pico_status_t pamDoCommand(register picodata_ProcessingUnit this,
        picoos_uint8 *qItem)
{
    pam_subobj_t *pam;
    picoos_single fValue;
    picoos_uint16 nValue;
    picoos_uint32 nPos;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    if (qItem[0] == PICODATA_ITEM_CMD) {
        switch (qItem[1]) {
            case PICODATA_ITEMINFO1_CMD_FLUSH:
                /* flush is for all PU's and as such it is also for PAM : implement the flush!!*/
                pam_reset_processors(this);
                pam->nLastAttachedItemId = pam->nCurrAttachedItem = 0;
                pam->nAttachedItemsSize = 0;
                return PICOPAM_FLUSH_RECEIVED;
                break;

            case PICODATA_ITEMINFO1_CMD_PITCH:
            case PICODATA_ITEMINFO1_CMD_SPEED:
                nPos = 4;
                picoos_read_mem_pi_uint16(qItem, &nPos, &nValue);
                if (qItem[2] == 'a') {
                    /*absloute modifier*/
                    fValue = (picoos_single) nValue / (picoos_single) 100.0f;
                    if (qItem[1] == PICODATA_ITEMINFO1_CMD_PITCH)
                        pam->pMod = fValue;
                    if (qItem[1] == PICODATA_ITEMINFO1_CMD_SPEED)
                        pam->dMod = (1.0f / fValue);
                }
                if (qItem[2] == 'r') {
                    /*relative modifier*/
                    fValue = (picoos_single) nValue / (picoos_single) 1000.0f;
                    if (qItem[1] == PICODATA_ITEMINFO1_CMD_PITCH)
                        pam->pMod *= (1.0f / fValue);
                    if (qItem[1] == PICODATA_ITEMINFO1_CMD_SPEED)
                        pam->dMod *= (1.0f / fValue);
                }
                return PICOPAM_CONTINUE;
                break;

            default:
                break;
        }/*end switch  switch (qItem[1])*/
    }/*end if (qItem[0]==PICODATA_ITEM_CMD)*/
    return PICOPAM_CONTINUE;
}/*pamDoCommand*/

/**
 * defines if an item has to be sent to following PUs
 * @param    qItem : input item pointer
 * @return    TRUE : item has to be transmitted to following PUs
 * @return    FALSE : item has to be consumed internallz on PAM
 * @callgraph
 * @callergraph
 */
static pico_status_t isItemToPut(picoos_uint8 *qItem)
{
    switch (qItem[0]) {
        case PICODATA_ITEM_CMD:
            /* is a command*/
            if (PICODATA_ITEMINFO1_CMD_SPEED == qItem[1]) {
                /* SPEED consumed here*/
                return FALSE;
            }
            break;
        case PICODATA_ITEM_BOUND:
            switch (qItem[1]) {
                case PICODATA_ITEMINFO1_BOUND_SBEG:
                case PICODATA_ITEMINFO1_BOUND_PHR0:
                case PICODATA_ITEMINFO1_BOUND_PHR1:
                case PICODATA_ITEMINFO1_BOUND_PHR2:
                case PICODATA_ITEMINFO1_BOUND_PHR3:
                    /*boudary items consumed here except SEND,TERM*/
                    return FALSE;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    /*all other items not explicitly mentioned here
     are transmitted to next PUs*/
    return TRUE;
}/*isItemToPut*/

/**
 * pushes a boundary TERM item into some buffer
 * @param    outBuff : output buffer base pointer
 * @param    outWritePos : offset in output buffer
 * @param    *bytesWr : actual bytes written
 * @return    PICO_OK
 * @remarks    used while forcing TERM input items in forward processing
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_put_term(picoos_uint8 *outBuff,
        picoos_uint16 outWritePos, picoos_uint8 *bytesWr)
{
    picoos_uint8 *sDest;
    sDest = &(outBuff[outWritePos]);
    sDest[0] = PICODATA_ITEM_BOUND; /*Item type*/
    sDest[1] = PICODATA_ITEMINFO1_BOUND_TERM;
    sDest[2] = PICODATA_ITEMINFO2_BOUNDTYPE_T;
    sDest[3] = 0; /*item size*/
    *bytesWr = 4;
    return PICO_OK;
}/*pam_put_term*/

/**
 * translates one full phone into a PHONE Item including DT Dur, F0 and CEP trees feature generation and traversal
 * @param    this : Pam item subobject pointer
 * @return    PICO_OK : processing successful
 * @return    PICODATA_PU_ERROR : error accessing PAM object
 * @return    !=PICO_OK : processing errors
 * @callgraph
 * @callergraph
 */
static pico_status_t pamPhoneProcess(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    pico_status_t sResult;
    picokdt_classify_result_t dTreeResult;
    picoos_uint8 nI, bWr;

    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    /*expands current phone in current syllable in the corresponding vector pam->sPhFeats[]*/
    sResult = pam_expand_vector(this);
    sResult = pamCompressVector(this);
    sResult = pamReorgVector(this);

    /*tree traversal for duration*/
    if (!pam_do_tree(this, pam->dtdur, &(pam->sPhFeats[0]), PICOPAM_INVEC_SIZE,
            &dTreeResult)) {
        PICODBG_WARN(("problem using pam tree dtdur, using fallback value"));
        dTreeResult.class = 0;
    }
    pam->durIndex = dTreeResult.class;
    sResult = pam_get_duration(this, pam->durIndex, &(pam->phonDur),
            &(pam->numFramesState[0]));

    /*tree traversal for pitch*/
    for (nI = 0; nI < PICOPAM_MAX_STATES_PER_PHONE; nI++) {
        if (!pam_do_tree(this, pam->dtlfz[nI], &(pam->sPhFeats[0]),
                PICOPAM_INVEC_SIZE, &dTreeResult)) {
            PICODBG_WARN(("problem using pam tree lf0Tree, using fallback value"));
            dTreeResult.class = 0;
        }
        pam->lf0Index[nI] = dTreeResult.class;
    }

    /*pdf access for pitch*/
    for (nI = 0; nI < PICOPAM_MAX_STATES_PER_PHONE; nI++) {
        sResult = pam_get_f0(this, &(pam->lf0Index[0]), nI, &(pam->phonF0[0]));
    }

    /*update vector with duration and pitch for cep tree traversal*/
    sResult = pam_update_vector(this);
    /*cep tree traversal*/
    for (nI = 0; nI < PICOPAM_MAX_STATES_PER_PHONE; nI++) {

        if (!pam_do_tree(this, pam->dtmgc[nI], &(pam->sPhFeats[0]),
                PICOPAM_INVEC_SIZE, &dTreeResult)) {
            PICODBG_WARN(("problem using pam tree lf0Tree, using fallback value"));
            dTreeResult.class = 0;
        }
        pam->mgcIndex[nI] = dTreeResult.class;
    }
    /*put item to output buffer*/
    sResult = pam_put_item(this, pam->outBuf, pam->outWritePos, &bWr);
    if (sResult == PICO_OK)
        pam->outWritePos += bWr;
    else
        return sResult;
    return PICO_OK;
}/*pamPhoneProcess*/

/**
 * manages first syllable attached items when seen before SBEG
 * @param    this  : Pam item subobject pointer
 * @return    PICO_OK (0) : default return code --> means no more items to be processed before 1st syllable
 * @return    PICOPAM_GOTO_FEED : go to feed state after this
 * @return    PICOPAM_GOTO_SCHEDULE : flush received
 * @return    PICODATA_PU_ERROR : errors
 * @callgraph
 * @callergraph
 */
static pico_status_t pamDoPreSyll(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    pico_status_t sResult;
    picoos_uint8 bWr, nRc;
    picoos_uint8 *qItem;
    nRc = PICOPAM_PRE_SYLL_ENDED;
    pam = (pam_subobj_t *) this->subObj;
    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    /*regenerate initial items before the phonemes*/
    if (((qItem = pamPopAttachedSy0(this)) != NULL) && !((qItem[0]
            == PICODATA_ITEM_BOUND) && (qItem[1]
            == PICODATA_ITEMINFO1_BOUND_SBEG))) {
         if (isItemToPut(qItem)) {
            pam_put_qItem(qItem, pam->outBuf, pam->outWritePos, &bWr);/*popped item has to be sent to next PU*/
            pam->outWritePos += bWr;
            nRc = PICOPAM_GOTO_FEED;
        }

        if (is_pam_command(qItem) == TRUE) {
            nRc = pamDoCommand(this, qItem); /*popped item is a PAM command : do it NOW!!*/
            if ((nRc == PICOPAM_FLUSH_RECEIVED) || (nRc == PICODATA_PU_ERROR)) {
                /*FLUSH command RECEIVED or errors: stop ALL PROCESSING*/
                return nRc;
            }
        }
        pam->nCurrAttachedItem++;
        if (nRc == 0)
            return PICOPAM_CONTINUE;
        else
            return nRc;
    }
    /*SBEG item management*/
    if ((qItem != NULL) && (qItem[0] == PICODATA_ITEM_BOUND) && (qItem[1]
            == PICODATA_ITEMINFO1_BOUND_SBEG)) {
        sResult = pam_put_qItem(qItem, pam->outBuf, pam->outWritePos, &bWr);
        pam->outWritePos += bWr;
        pam->nCurrAttachedItem++;
        nRc = PICOPAM_GOTO_FEED;
    }
    return nRc;
}/*pamDoPreSyll*/

/**
 * performs a step of the pam processing
 * @param    this : Pam item subobject pointer
 * @param    mode : mode for the PU
 * @param    *numBytesOutput : pointer to output number fo bytes produced
 * @return    PICODATA_PU_IDLE : nothing to do
 * @return    PICODATA_PU_BUSY : still tasks undergoing
 * @return    PICODATA_PU_ERROR : errors on processing
 * @callgraph
 * @callergraph
 */
static picodata_step_result_t pam_step(register picodata_ProcessingUnit this,
        picoos_int16 mode, picoos_uint16 * numBytesOutput)
{

    register pam_subobj_t * pam;

    pico_status_t sResult;
    picoos_uint16 blen, numinb, numoutb;
    pico_status_t rv;
    picoos_uint8 bWr;
    picoos_uint8 bForcedItem[4];
    picoos_uint8 *qItem;

    numinb = 0;
    numoutb = 0;
    rv = PICO_OK;

    if (NULL == this || NULL == this->subObj) {
        return PICODATA_PU_ERROR;
    }
    pam = (pam_subobj_t *) this->subObj;
    mode = mode; /* avoid warning "var not used in this function"*/
    /*Init number of output bytes*/
    *numBytesOutput = 0;

    while (1) { /* exit via return */

        PICODBG_DEBUG(("pam_step -- doing state %i",pam->procState));

        switch (pam->procState) {

            case PICOPAM_COLLECT:
                /* *************** item collector ***********************************/
                /*collecting items from the PU input buffer*/
                sResult = picodata_cbGetItem(this->cbIn,
                        &(pam->inBuf[pam->inWritePos]), pam->inBufSize
                                - pam->inWritePos, &blen);
                if (sResult != PICO_OK) {
                    if (sResult == PICO_EOF) {
                        /*no items available : remain in state 0 and return idle*/
                        return PICODATA_PU_IDLE;
                    } else {
                        /*errors : remain in state 0 and return error*/
                        PICODBG_DEBUG(("pam_step(PICOPAM_COLLECT) -- Errors on item buffer input, status: %d",sResult));
                        return PICODATA_PU_ERROR;
                    }
                }

                PICODBG_DEBUG(("pam_step -- got item, status: %d",sResult));
                sResult = picodata_is_valid_item(
                        &(pam->inBuf[pam->inWritePos]), blen);
                if (sResult != TRUE) {
                    /*input item is not valid : consume the input item and stay in COLLECT*/
                    pam->inWritePos += blen;
                    pam->inReadPos += blen;
                    if (pam->inReadPos >= pam->inWritePos) {
                        pam->inReadPos = 0;
                        pam->inWritePos = 0;
                    }PICODBG_DEBUG(("pam_step -- item is not valid, type: %d",pam->inBuf[pam->inWritePos]));
                    return PICODATA_PU_BUSY;
                }

                /*update input write pointer + move to "schedule" state*/
                pam->inWritePos += blen;
                pam->procState = PICOPAM_SCHEDULE;
                return PICODATA_PU_BUSY;

            case PICOPAM_SCHEDULE:
                /* check out if more items are available */
                if (pam->inReadPos >= pam->inWritePos) {
                    /*no more items : back to collect state*/
                    pam->procState = PICOPAM_COLLECT;
                    return PICODATA_PU_BUSY;
                }
                /* we have one full valid item, with len>0 starting at
                 pam->inBuf[pam->inReadPos]; here we decide how to elaborate it */

                /* PLAY management */
                if (is_pam_play_command(&(pam->inBuf[pam->inReadPos])) == TRUE) {
                    /*consume the input item : it has been managed*/
                    pam->inReadPos += pam->inBuf[pam->inReadPos + 3]
                            + sizeof(picodata_itemhead_t);
                    if (pam->inReadPos >= pam->inWritePos) {
                        pam->inReadPos = 0;
                        pam->inWritePos = 0;
                    }
                    /*stay in schedule*/
                    return PICODATA_PU_BUSY;
                }

                if (pam_check_immediate(this, &(pam->inBuf[pam->inReadPos]))) {
                    /* item has to be sent to next PU NOW : switch to "immediate" state */
                    pam->procState = PICOPAM_IMMEDIATE;
                    return PICODATA_PU_BUSY;
                }
                if (pamCheckResourceLimits(this, &(pam->inBuf[pam->inReadPos]))) {
                    /* item would not fit into local buffers -->> free some space -->>
                     switch to "force term" state */
                    pam->procState = PICOPAM_FORWARD_FORCE_TERM;
                    return PICODATA_PU_BUSY;
                }

                if (pam_deal_with(&(pam->inBuf[pam->inReadPos]))) {
                    /* item has to be managed by the "forward" state : switch to forward state*/
                    pam->procState = PICOPAM_FORWARD;
                    return PICODATA_PU_BUSY;
                }

                if (pam_hastobe_queued(this, &(pam->inBuf[pam->inReadPos]))) {
                    /* item is not for PAM so it has to be queued internally */
                    pam_queue(this, &(pam->inBuf[pam->inReadPos]));
                    /*consume the input item : it has been queued*/
                    pam->inReadPos += pam->inBuf[pam->inReadPos + 3]
                            + sizeof(picodata_itemhead_t);
                    if (pam->inReadPos >= pam->inWritePos) {
                        pam->inReadPos = 0;
                        pam->inWritePos = 0;
                    }
                    return PICODATA_PU_BUSY;
                }
                /*if we get here something wrong happened. Being the the item valid,
                 switch to "immediate" state -> send it to next PU -> */
                PICODBG_DEBUG(("pam_step (PICOPAM_SCHEDULE) -- unexpected item is sent to next PU !!"));
                pam->procState = PICOPAM_IMMEDIATE;
                return PICODATA_PU_BUSY;
                break; /*PICOPAM_SCHEDULE*/

            case PICOPAM_FORWARD:
                /*we have one full valid item, with len>0 starting at pam->inBuf[pam->inReadPos].
                 furthermore this item should be in the set {BOUND,SYLL}.
                 No other items should arrive here*/
                sResult = pam_adapter_forward_step(this,
                        &(pam->inBuf[pam->inReadPos]));
                /*decide if this item has to be queued for later re-synchronization
                 normally this is only done for SEND/TERM items*/
                if (pam_hastobe_queued(this, &(pam->inBuf[pam->inReadPos]))) {
                    /*item has to be queued iternally in local storage*/
                    pam_queue(this, &(pam->inBuf[pam->inReadPos]));
                }
                /*now assign next state according to Forward results*/
                switch (sResult) {
                    case PICOPAM_READY:
                        pam->needMoreInput = FALSE;
                        /*consume the input item : it has already been stored*/
                        pam->inReadPos += pam->inBuf[pam->inReadPos + 3]
                                + sizeof(picodata_itemhead_t);
                        if (pam->inReadPos >= pam->inWritePos) {
                            pam->inReadPos = 0;
                            pam->inWritePos = 0;
                        }
                        /*activate backward processing*/
                        sResult = pam_adapter_backward_step(this);
                        if (sResult == PICO_OK) {
                            pam->procState = PICOPAM_PROCESS;
                            return PICODATA_PU_BUSY;
                        } else {
                            PICODBG_DEBUG(("pam_step (PICOPAM_FORWARD) -- wrong return from BackwardStep: %d -- Buffered sentence will be discarded",sResult));
                            pam_reset_processors(this);
                            pam->nLastAttachedItemId = pam->nCurrAttachedItem
                                    = 0;
                            pam->nAttachedItemsSize = 0;

                            pam->procState = PICOPAM_SCHEDULE;
                            return PICODATA_PU_BUSY;
                        }
                        break;

                    case PICOPAM_MORE:
                        pam->needMoreInput = TRUE;
                        /*consume the input item : it has already been stored*/
                        pam->inReadPos += pam->inBuf[pam->inReadPos + 3]
                                + sizeof(picodata_itemhead_t);
                        if (pam->inReadPos >= pam->inWritePos) {
                            /*input is finished and PAM need more data :
                             clenaup input buffer + switch state back to "schedule state"
                             */
                            pam->inReadPos = 0;
                            pam->inWritePos = 0;
                            pam->procState = PICOPAM_SCHEDULE;
                            return PICODATA_PU_ATOMIC;
                        } else {
                            /*input is not finished and need more data :
                             remain in state "PICOPAM_FORWARD" */
                            return PICODATA_PU_ATOMIC;
                        }
                        break;

                    case PICOPAM_NA:
                    default:
                        /*this item has not been stored in internal buffers:
                         assign this item to the management of
                         "immediate" state*/
                        pam->procState = PICOPAM_IMMEDIATE;
                        return PICODATA_PU_BUSY;
                        break;
                } /*end switch sResult*/
                break; /*PICOPAM_FORWARD*/

            case PICOPAM_FORWARD_FORCE_TERM:
                /*we have one full valid item, with len>0
                 starting at pam->inBuf[pam->inReadPos] but we decided
                 to force a TERM item before, without losing the item in
                 inBuf[inReadPos] : --> generate a TERM item and do the
                 forward processing */
                pam_put_term(bForcedItem, 0, &bWr);
                sResult = pam_adapter_forward_step(this, &(bForcedItem[0]));
                switch (sResult) {
                    case PICOPAM_READY:
                        pam_queue(this, &(bForcedItem[0]));
                        /*activate backward processing*/
                        sResult = pam_adapter_backward_step(this);
                        if (sResult == PICO_OK) {
                            pam->procState = PICOPAM_PROCESS;
                            return PICODATA_PU_BUSY;
                        } else {
                            PICODBG_DEBUG(("pam_step (PICOPAM_FORWARD_FORCE_TERM) -- wrong return from BackwardStep: %d -- Buffered sentence will be discarded",sResult));
                            pam_reset_processors(this);
                            pam->nLastAttachedItemId = pam->nCurrAttachedItem
                                    = 0;
                            pam->nAttachedItemsSize = 0;

                            pam->procState = PICOPAM_SCHEDULE;
                            return PICODATA_PU_BUSY;
                        }
                        break;

                    default:
                        PICODBG_DEBUG(("pam_step (PICOPAM_FORWARD_FORCE_TERM) -- Forced a TERM but processing do not appear to end -- Buffered sentence will be discarded",sResult));
                        pam_reset_processors(this);
                        pam->nLastAttachedItemId = pam->nCurrAttachedItem = 0;
                        pam->nAttachedItemsSize = 0;

                        pam->procState = PICOPAM_SCHEDULE;
                        return PICODATA_PU_BUSY;
                        break;

                } /*end switch sResult*/
                break; /*PICOPAM_FORWARD_FORCE_TERM*/

            case PICOPAM_PROCESS:

                if ((PICOPAM_FRAME_ITEM_SIZE + 4) > (pam->outBufSize
                        - pam->outWritePos)) {
                    /*WARNING (buffer overflow): leave status unchanged until output buffer free */
                    return PICODATA_PU_BUSY;
                }

                if (pam->nCurrSyllable == 0) {
                    sResult = pamDoPreSyll(this);
                    if (sResult == PICOPAM_GOTO_FEED) {
                        /*
                         items pushed to output buffer :
                         switch to "feed" but then back
                         to "process"
                         */
                        pam->retState = PICOPAM_PROCESS;
                        pam->procState = PICOPAM_FEED;
                        return PICODATA_PU_BUSY;
                    }
                    if (sResult == PICOPAM_CONTINUE) {
                        /*
                         items processed (maybe commands) :
                         return (maybe we need to process other
                         items in pre_syll) and then back to "process"
                         */
                        pam->retState = PICOPAM_PROCESS;
                        pam->procState = PICOPAM_PROCESS;
                        return PICODATA_PU_BUSY;
                    }

                    if ((sResult == PICOPAM_FLUSH_RECEIVED) || (sResult
                            == PICODATA_PU_ERROR)) {
                        /*
                         items processed were a flush or
                         problems found: switch to "schedule"
                         and abort all processing
                         */
                        pam->retState = PICOPAM_SCHEDULE;
                        pam->procState = PICOPAM_SCHEDULE;
                        return PICODATA_PU_BUSY;
                    }
                    if (sResult == PICOPAM_PRE_SYLL_ENDED) {
                        /*
                         we get here when     pam->nCurrSyllable==0 and
                         no more items to be processed before the syllable
                         */
                        sResult = sResult;
                    }
                }

                if (pamHasToProcess(this)) {
                    if (pamPhoneProcess(this) == PICO_OK) {
                        sResult = pamUpdateProcess(this);
                        pam->procState = PICOPAM_FEED; /*switch to feed*/
                        return PICODATA_PU_BUSY;
                    } else {
                        PICODBG_DEBUG(("pam_step(PICOPAM_PROCESS) --- NULL return from pamPhoneProcess"));
                        return PICODATA_PU_ERROR;
                    }
                }

                if (pamHasToPop(this) != FALSE) {
                    if ((qItem = pamPopItem(this)) == NULL) {
                        PICODBG_DEBUG(("pam_step(PICOPAM_PROCESS) --- NULL return from pamPopItem"));
                        return PICODATA_PU_ERROR;
                    }

                    if (isItemToPut(qItem)) {
                        /*popped item has to be sent to next PU*/
                        sResult = pam_put_qItem(qItem, pam->outBuf,
                                pam->outWritePos, &bWr);
                        if (sResult != PICO_OK) {
                            PICODBG_DEBUG(("pam_step(PICOPAM_PROCESS) --- Error on writing item to output buffer"));
                            return PICODATA_PU_ERROR;
                        }
                        pam->outWritePos += bWr; /*item write ok*/
                        pam->procState = PICOPAM_FEED; /*switch to feed*/
                    }

                    /*moved command processing here (after pam_put_qItem) because of FLUSH command could erase
                     * the syllable structure and make it impossible to transmit the flush to other PUs*/
                    if (is_pam_command(qItem) == TRUE) {
                        sResult = pamDoCommand(this, qItem); /*popped item is a PAM command : do it NOW!!*/
                        if ((sResult == PICOPAM_FLUSH_RECEIVED) || (sResult
                                == PICODATA_PU_ERROR)) {
                            pam->retState = PICOPAM_SCHEDULE;
                            pam->procState = PICOPAM_SCHEDULE; /*switch to schedule */
                            return PICODATA_PU_BUSY;
                        }
                    }
                    /*update PAM status: if more items attached to the current syllable
                     stay in current syllable, otherwise move to next syllable and switch
                     to processing phones */
                    sResult = pamUpdateProcess(this); /*both "doCommand" or "put" : update PAM status*/
                    return PICODATA_PU_BUSY;
                } else {
                    pam->procState = PICOPAM_SCHEDULE; /*switch to schedule */
                    return PICODATA_PU_BUSY;
                }

                break; /*PICOPAM_PROCESS*/

            case PICOPAM_IMMEDIATE:
                /* *** item is output NOW!!! */
                /*context: full valid item, with len> starting at pam->inBuf[pam->inReadPos]*/
                numinb = PICODATA_ITEM_HEADSIZE
                        + pam->inBuf[pam->inReadPos + 3];
                sResult = picodata_copy_item(&(pam->inBuf[pam->inReadPos]),
                        numinb, &(pam->outBuf[pam->outWritePos]),
                        pam->outBufSize - pam->outWritePos, &numoutb);

                if (sResult == PICO_OK) {
                    pam->inReadPos += numinb;
                    if (pam->inReadPos >= pam->inWritePos) {
                        pam->inReadPos = 0;
                        pam->inWritePos = 0;
                        pam->needMoreInput = FALSE;
                    }
                    pam->outWritePos += numoutb;
                    pam->procState = PICOPAM_FEED; /*switch to FEED state*/
                    pam->retState = PICOPAM_SCHEDULE; /*back to SCHEDULE after FEED*/
                } else {
                    /*
                     PICO_EXC_BUF_IGNORE
                     PICO_EXC_BUF_UNDERFLOW
                     PICO_EXC_BUF_OVERFLOW
                     */
                    PICODBG_DEBUG(("pam_step(PICOPAM_IMMEDIATE) --- wrong return from picodata_copy_item:%d",sResult));
                    return PICODATA_PU_ERROR;
                }
                return PICODATA_PU_BUSY;
                break; /*PICOPAM_IMMEDIATE*/

            case PICOPAM_FEED:
                /* *************** item output/feeding ***********************************/
                /*feeding items to PU output buffer*/
                sResult = picodata_cbPutItem(this->cbOut,
                        &(pam->outBuf[pam->outReadPos]), pam->outWritePos
                                - pam->outReadPos, &numoutb);
                PICODBG_DEBUG(("pam_step -- put item, status: %d",sResult));
                if (PICO_OK == sResult) {

                    PICODATA_INFO_ITEM(this->voice->kbArray[PICOKNOW_KBID_DBG],
                            (picoos_uint8 *)"pam: ",
                            pam->outBuf + pam->outReadPos, pam->outBufSize);

                    pam->outReadPos += numoutb;
                    *numBytesOutput = numoutb;
                    if (pam->outReadPos >= pam->outWritePos) {
                        /*reset the output pointers*/
                        pam->outReadPos = 0;
                        pam->outWritePos = 0;
                        /*switch to appropriate state*/
                        switch (pam->retState) {
                            case PICOPAM_IMMEDIATE:
                                pam->procState = PICOPAM_IMMEDIATE;
                                pam->retState = PICOPAM_SCHEDULE;
                                return PICODATA_PU_BUSY;
                                break;
                            case PICOPAM_PLAY:
                                pam->procState = PICOPAM_PLAY;
                                pam->retState = PICOPAM_SCHEDULE;
                                return PICODATA_PU_BUSY;
                                break;
                            default:
                                break;
                        }
                        /*Define next state
                         a)process (if current sentence has more data to process)
                         b)schedule (no more data to process in current sentence)
                         NOTE : case b)also happens when dealing with non BOUND/SYLL items*/
                        if ((pamHasToProcess(this)) || (pamHasToPop(this))) {
                            pam->procState = PICOPAM_PROCESS;
                        } else {
                            pam->nCurrSyllable = -1;
                            pam_reset_processors(this);
                            pam->nLastAttachedItemId = pam->nCurrAttachedItem
                                    = 0;
                            pam->nAttachedItemsSize = 0;

                            pam->nSyllPhoneme = 0;
                            pam->procState = PICOPAM_SCHEDULE;
                        }
                    }
                    return PICODATA_PU_BUSY;

                } else if (PICO_EXC_BUF_OVERFLOW == sResult) {

                    PICODBG_DEBUG(("pam_step ** feeding, overflow, PICODATA_PU_OUT_FULL"));
                    return PICODATA_PU_OUT_FULL;

                } else if ((PICO_EXC_BUF_UNDERFLOW == sResult)
                        || (PICO_ERR_OTHER == sResult)) {

                    PICODBG_DEBUG(("pam_step ** feeding problem, discarding item"));
                    pam->outReadPos = 0;
                    pam->outWritePos = 0;
                    pam->procState = PICOPAM_COLLECT;
                    return PICODATA_PU_ERROR;

                }
                break; /*PICOPAM_FEED*/

            default:
                /*NOT feeding items*/
                sResult = PICO_EXC_BUF_IGNORE;
                break;
        }/*end switch*/
        return PICODATA_PU_BUSY; /*check if there is more data to process after feeding*/

    }/*end while*/
    return PICODATA_PU_IDLE;
}/*pam_step*/

/**
 * performs one step of a PamTree
 * @param    this : Pam item subobject pointer
 * @param    dtpam : the Pam decision tree
 * @param    *invec : the input vector pointer
 * @param    inveclen : length of the input vector
 * @param    *dtres : the classification result
 * @return    dtres->set : the result of tree traversal
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pam_do_tree(register picodata_ProcessingUnit this,
        const picokdt_DtPAM dtpam, const picoos_uint8 *invec,
        const picoos_uint8 inveclen, picokdt_classify_result_t *dtres)
{
    picoos_uint8 okay;

    okay = TRUE;
    /* construct input vector, which is set in dtpam */
    if (!picokdt_dtPAMconstructInVec(dtpam, invec, inveclen)) {
        /* error constructing invec */
        PICODBG_WARN(("problem with invec"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_INVECTOR, NULL, NULL);
        okay = FALSE;
    }
    /* classify */
    if (okay && (!picokdt_dtPAMclassify(dtpam))) {
        /* error doing classification */
        PICODBG_WARN(("problem classifying"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_CLASSIFICATION, NULL,
                NULL);
        okay = FALSE;
    }
    /* decompose */
    if (okay && (!picokdt_dtPAMdecomposeOutClass(dtpam, dtres))) {
        /* error decomposing */
        PICODBG_WARN(("problem decomposing"));
        picoos_emRaiseWarning(this->common->em, PICO_WARN_OUTVECTOR, NULL, NULL);
        okay = FALSE;
    }

    PICODBG_TRACE(("dtpam output class: %d", dtres->class));

    return dtres->set;
}/*pam_do_tree*/

/**
 * returns the carrier vowel id inside a syllable
 * @param    this : Pam item subobject pointer
 * @param    item : the full syllable item
 * @param    *pos : pointer to the variable to receive the position of the carrier vowel
 * @return    the phonetic id for the carrier vowel inside the syllable
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pam_get_vowel_name(register picodata_ProcessingUnit this,
        picoos_uint8 *item, picoos_uint8 *pos)
{
    pam_subobj_t *pam;
    picoos_uint8 *phon, nI, nCond1;
    if (NULL == this || NULL == this->subObj) {
        return 0;
    }
    pam = (pam_subobj_t *) this->subObj;

    if (item == NULL)
        return 0;
    if (item[3] == 0)
        return 0;
    phon = &item[4];
    for (nI = 0; nI < item[3]; nI++) {
        nCond1 = picoktab_isSyllCarrier(pam->tabphones, phon[nI]);
        if (nCond1) {
            *pos = nI;
            return phon[nI];
        }
    }
    return 0;
}/*pam_get_vowel_name */

/**
 * returns the pause phone id in the current ph.alphabet
 * @param    this : Pam sub object pointer
 * @return    the (numeric) phonetic id of the pause phone in current phonetic alphabet
 * @return    0 :  errors on getting the pam subobject pointer
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pam_get_pause_id(register picodata_ProcessingUnit this)
{
    picoos_uint8 nVal1;
    /*picoos_uint8 nVal2; */
    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return 0;
    }
    pam = (pam_subobj_t *) this->subObj;
    nVal1 = picoktab_getPauseID(pam->tabphones);
    return nVal1;
}/*pam_get_pause_id */

/**
 * returns the pam sentence type (declarative, interrogative...)
 * @param    iteminfo1 : the boundary item info 1
 * @param    iteminfo2 : the boundary item info 2
 * @return    the sentence type suitably encoded for trees
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pam_map_sentence_type(picoos_uint8 iteminfo1,
        picoos_uint8 iteminfo2)
{
    switch (iteminfo2) {
        case PICODATA_ITEMINFO2_BOUNDTYPE_P:
            return PICOPAM_DECLARATIVE;
        case PICODATA_ITEMINFO2_BOUNDTYPE_T:
            return PICOPAM_DECLARATIVE;
        case PICODATA_ITEMINFO2_BOUNDTYPE_Q:
            return PICOPAM_INTERROGATIVE;
        case PICODATA_ITEMINFO2_BOUNDTYPE_E:
            return PICOPAM_DECLARATIVE;
        default:
            return PICOPAM_DECLARATIVE;
    }
    iteminfo1 = iteminfo1; /* avoid warning "var not used in this function"*/
    return PICOPAM_DECLARATIVE;
}/*pam_map_sentence_type */

/**
 * returns the pam phrase type
 * @param    iteminfo1 : the boundary item info 1
 * @param    iteminfo2 : the boundary item info 2
 * @return    the phrase type suitably encoded for trees
 * @callgraph
 * @callergraph
 */
static picoos_uint8 pam_map_phrase_type(picoos_uint8 iteminfo1,
        picoos_uint8 iteminfo2)
{

    switch (iteminfo2) {
        case PICODATA_ITEMINFO2_BOUNDTYPE_P:
            switch (iteminfo1) {
                case PICODATA_ITEMINFO1_BOUND_PHR1:
#                ifdef PAM_PHR2_WITH_PR1
                case PICODATA_ITEMINFO1_BOUND_PHR2:
#                endif
                    return PICOPAM_P; /*current_prhase type = "P" (encoded to 1) */
                    break;
                case PICODATA_ITEMINFO1_BOUND_PHR3:
#                ifdef PAM_PHR2_WITH_PR3
                    case PICODATA_ITEMINFO1_BOUND_PHR2 :
#                endif
                    return PICOPAM_p; /*current_prhase type = "p" (encoded to 2) */
                    break;
                case PICODATA_ITEMINFO1_BOUND_SBEG:
                    return PICOPAM_P; /*current_prhase type = "P" (encoded to 1) */
                    break;
                default:
                    PICODBG_DEBUG(("Map pam_map_phrase_type : unexpected iteminfo1"));
                    return PICOPAM_P; /*current_prhase type = "P" (encoded to 1) */
                    break;
            }
        case PICODATA_ITEMINFO2_BOUNDTYPE_T:
            return PICOPAM_T; /*current_prhase type = "T" (encoded to 0) */
            break;
        case PICODATA_ITEMINFO2_BOUNDTYPE_E:
            return PICOPAM_T; /*current_prhase type = "T" (encoded to 0) */
            break;
        case PICODATA_ITEMINFO2_BOUNDTYPE_Q:
            return PICOPAM_Y; /*current_prhase type = "T" (encoded to 0) */
            break;
        default:
            PICODBG_DEBUG(("Map pam_map_phrase_type : unexpected iteminfo2"));
            return PICOPAM_T; /*current_prhase type = "T" (encoded to 0) */
            break;
    }PICODBG_DEBUG(("Map pam_map_phrase_type : unexpected iteminfo2"));
    return PICOPAM_T; /*current_prhase type = "T" (encoded to 0) */

}/*pam_map_phrase_type */

/**
 * does the cleanup of the sub object processors flags at sentence start
 * @param    this : pointer to PAM PU sub object pointer
 * @return    PICO_OK : reset OK
 * @return    PICO_ERR_OTHER : errors on getting pam sub obj pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_reset_processors(register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    pam->nCurrSyllable = -1;
    pam->nTotalPhonemes = pam->nSyllPhoneme = pam->nCurrPhoneme
            = pam->nTotalSyllables = pam->sType = pam->pType = 0;
    pam->dRest = 0.0f;
    /*set all to 0*/
    pam->a3_overall_syllable    = pam->a3_primary_phrase_syllable   = pam->b4_b5_syllable =
        pam->b6_b7_syllable     = pam->b6_b7_state                  = pam->b8_b9_stressed_syllable =
        pam->b10_b11_accented_syllable  = pam->b12_b13_syllable     = pam->b12_b13_state =
        pam->b14_b15_syllable   = pam->b14_b15_state                = pam->b17_b19_syllable =
        pam->b17_b19_state      = pam->b18_b20_b21_syllable         = pam->b18_b20_b21_state =
        pam->c3_overall_syllable= pam->c3_primary_phrase_syllable   = pam->d2_syllable_in_word =
        pam->d2_prev_syllable_in_word = pam->d2_current_primary_phrase_word = pam->e1_syllable_word_start =
        pam->e1_syllable_word_end= pam->e1_content                  = pam->e2_syllable_word_start =
        pam->e2_syllable_word_end= pam->e3_e4_word                  = pam->e3_e4_state =
        pam->e5_e6_content_word = pam->e5_e6_content                = pam->e7_e8_word =
        pam->e7_e8_content      = pam->e7_e8_state                  = pam->e9_e11_word =
        pam->e9_e11_saw_word    = pam->e9_e11_state                 = pam->e10_e12_e13_word =
        pam->e10_e12_e13_state  = pam->e10_e12_e13_saw_word         = pam->f2_overall_word =
        pam->f2_word_syllable   = pam->f2_next_word_syllable        = pam->f2_current_primary_phrase_word =
        pam->g1_current_secondary_phrase_syllable                   = pam->g1_current_syllable =
        pam->g2_current_secondary_phrase_word                       = pam->g2_current_word =
        pam->h1_current_secondary_phrase_syll                       = pam->h2_current_secondary_phrase_word =
        pam->h3_h4_current_secondary_phrase_word                    = pam->h5_current_phrase_type =
        pam->h5_syllable        = pam->h5_state                     = pam->i1_secondary_phrase_syllable =
        pam->i1_next_secondary_phrase_syllable                      = pam->i2_secondary_phrase_word =
        pam->i2_next_secondary_phrase_word                          = pam->j1_utterance_syllable =
        pam->j2_utterance_word  = pam->j3_utterance_sec_phrases     = 0;
    /*Override 0 with 1*/
    pam->b4_b5_syllable         = pam->b17_b19_syllable             = pam->b18_b20_b21_syllable =
        pam->e9_e11_word        = pam->e10_e12_e13_word             = pam->e7_e8_word =
        pam->h2_current_secondary_phrase_word                       = 1;
    /*Override 0 with -1*/
    pam->e1_syllable_word_start = pam->e1_syllable_word_end         = pam->e2_syllable_word_start =
        pam->e2_syllable_word_end                                   = -1;

    return PICO_OK;
}/*pam_reset_processors*/

/**
 * does the cleanup of the sub object processors flags before the backward step
 * @param    this : pointer to PAM PU sub object pointer
 * @return    PICO_OK : reset OK
 * @return    PICO_ERR_OTHER : errors on getting pam sub obj pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_reset_processors_back(
        register picodata_ProcessingUnit this)
{
    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*set all to 0*/
    pam->a3_overall_syllable
        = pam->a3_primary_phrase_syllable
        = pam->b4_b5_syllable
        = pam->b6_b7_syllable
        = pam->b6_b7_state
        = pam->b8_b9_stressed_syllable
        = pam->b10_b11_accented_syllable
        = pam->b12_b13_syllable
        = pam->b12_b13_state
        = pam->b14_b15_syllable
        = pam->b14_b15_state
        = pam->b17_b19_syllable
        = pam->b17_b19_state
        = pam->b18_b20_b21_syllable
        = pam->b18_b20_b21_state
        = pam->c3_overall_syllable
        = pam->c3_primary_phrase_syllable
        = pam->d2_syllable_in_word
        = pam->d2_prev_syllable_in_word
        = pam->d2_current_primary_phrase_word
        = pam->e1_syllable_word_start
        = pam->e1_syllable_word_end
        = pam->e1_content
        = pam->e2_syllable_word_start
        = pam->e2_syllable_word_end
        = pam->e3_e4_word
        = pam->e3_e4_state
        = pam->e5_e6_content_word
        = pam->e5_e6_content
        = pam->e7_e8_word
        = pam->e7_e8_content
        = pam->e7_e8_state
        = pam->e9_e11_word
        = pam->e9_e11_saw_word
        = pam->e9_e11_state
        = pam->e10_e12_e13_word
        = pam->e10_e12_e13_state
        = pam->e10_e12_e13_saw_word
        = pam->f2_overall_word
        = pam->f2_word_syllable
        = pam->f2_next_word_syllable
        = pam->f2_current_primary_phrase_word
        = pam->g1_current_secondary_phrase_syllable
        = pam->g1_current_syllable
        = pam->g2_current_secondary_phrase_word
        = pam->g2_current_word
        = pam->h1_current_secondary_phrase_syll
        = pam->h2_current_secondary_phrase_word
        = pam->h3_h4_current_secondary_phrase_word
        = pam->h5_current_phrase_type
        = pam->h5_state
        = pam->i1_secondary_phrase_syllable
        = pam->i1_next_secondary_phrase_syllable
        = pam->i2_secondary_phrase_word
        = pam->i2_next_secondary_phrase_word
        = 0;
    /*Override 0 with 1*/
    pam->b4_b5_syllable = pam->b17_b19_syllable = pam->b18_b20_b21_syllable
        = pam->e9_e11_word = pam->e10_e12_e13_word = pam->e7_e8_word
        = pam->h2_current_secondary_phrase_word = 1;
    /*Override 0 with -1*/
    pam->e1_syllable_word_start = pam->e1_syllable_word_end
        = pam->e2_syllable_word_start = pam->e2_syllable_word_end = -1;

    return PICO_OK;
}/*pam_reset_processors_back*/

/**
 * processes an input event for a specific feature
 * @param    this : pointer to PAM PU sub object pointer
 * @param    nFeat : feature column to process
 * @param    event_type : event id among syll/boundprim/boundsec/boundword
 * @param    direction : forward(0)/backward(1)
 * @return    PICO_OK : process OK
 * @return    PICO_ERR_OTHER : errors on getting pam sub obj pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_process_event_feature(
        register picodata_ProcessingUnit this, picoos_uint8 nFeat,
        picoos_uint8 event_type, picoos_uint8 direction)
{
    picoos_uint8 sDest, nI;
    picoos_uint16 syllCurr;
    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;
    syllCurr = pam->nCurrSyllable;
    switch (nFeat) {
        case A3:
            /*processor for A3*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1]
                                == 1) || (pam->a3_primary_phrase_syllable >= 1)) {
                            if (pam->a3_overall_syllable < 1)
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[A3]
                                        = 0;
                            else
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[A3]
                                        = pam->sSyllFeats[pam->nCurrSyllable
                                                - 1].phoneV[B3];
                        } else {
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[A3] = 0;
                        }
                        pam->a3_primary_phrase_syllable++;
                        pam->a3_overall_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->a3_primary_phrase_syllable = 0;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    /*do nothing*/
                    break;
            }
            break;
        case B1:
        case B2:
        case B3:
            /*done in createSyllable*/
            break;
        case B4:/*processor for B4,B5*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B4;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B5;
                    break;
                default:
                    sDest = B4;
                    break;
            }
            if (event_type == PICOPAM_EVENT_SYLL) {
                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1] == 0) {
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                            = pam->b4_b5_syllable;
                    pam->b4_b5_syllable++;
                } else {
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest] = 0;
                }
            }
            if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                    == PICOPAM_EVENT_S_BOUND) || (event_type
                    == PICOPAM_EVENT_P_BOUND)) {
                pam->b4_b5_syllable = 1;
            }
            break;
        case B5:/*processor for B5 : done in B4*/
            break;
        case B6:/*processor for B6,B7*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B6;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B7;
                    break;
                default:
                    sDest = B6;
                    break;
            }
            switch (pam->b6_b7_state) {
                case 0:
                    if (event_type == PICOPAM_EVENT_SYLL)
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = PICOPAM_DONT_CARE_VALUE;
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->b6_b7_syllable = 1;
                        pam->b6_b7_state = 1;
                    }
                    break;
                case 1:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->b6_b7_syllable;
                        pam->b6_b7_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->b6_b7_syllable = 1;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->b6_b7_state = 0;
                    }
                    break;
                default:
                    break;
            }
            break;
        case B7:/*Done in B6*/
            break;
        case B8:/*processor for B8,B9*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B8;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B9;
                    break;
                default:
                    sDest = B8;
                    break;
            }
            if (event_type == PICOPAM_EVENT_SYLL) {
                pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                        = pam->b8_b9_stressed_syllable;
                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B1] == 1)
                    pam->b8_b9_stressed_syllable++;
            }
            if (event_type == PICOPAM_EVENT_P_BOUND) {
                pam->b8_b9_stressed_syllable = 0;
            }

            break;
        case B9:/*done in B8*/
            break;
        case B10:/*processor for B10, B11*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B10;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B11;
                    break;
                default:
                    sDest = B10;
                    break;
            }
            if (event_type == PICOPAM_EVENT_SYLL) {
                pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                        = pam->b10_b11_accented_syllable;
                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B2] == 1)
                    pam->b10_b11_accented_syllable++;
            }
            if (event_type == PICOPAM_EVENT_P_BOUND) {
                pam->b10_b11_accented_syllable = 0;
            }
            break;
        case B11:/*done in B10*/
            break;
        case B12:/*processor for B12,B13*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B12;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B13;
                    break;
                default:
                    sDest = B12;
                    break;
            }
            switch (pam->b12_b13_state) {
                case 0:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B1] == 0)
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                    = PICOPAM_DONT_CARE_VALUE;
                        else {
                            pam->b12_b13_syllable = 0;
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                    = PICOPAM_DONT_CARE_VALUE;
                            pam->b12_b13_state = 1;
                        }
                    }
                    break;
                case 1:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->b12_b13_syllable;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B1] == 1)
                            pam->b12_b13_syllable = 0;
                        else
                            pam->b12_b13_syllable++;
                        pam->b12_b13_state = 2;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND)
                        pam->b12_b13_state = 0;
                    break;
                case 2:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->b12_b13_syllable;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B1] == 1)
                            pam->b12_b13_syllable = 0;
                        else
                            pam->b12_b13_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND)
                        pam->b12_b13_state = 0;

                    break;
                default:
                    break;
            }
            break;
        case B13:/*done in B12*/
            break;

        case B14:/*processor for B14, B15*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = B14;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = B15;
                    break;
                default:
                    sDest = B14;
                    break;
            }
            switch (pam->b14_b15_state) {
                case 0:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B2] == 0)
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                    = PICOPAM_DONT_CARE_VALUE;
                        else {
                            pam->b14_b15_syllable = 0;
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                    = PICOPAM_DONT_CARE_VALUE;
                            pam->b14_b15_state = 1;
                        }
                    }
                    break;
                case 1:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->b14_b15_syllable;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B2] == 1)
                            pam->b14_b15_syllable = 0;
                        else
                            pam->b14_b15_syllable++;
                        pam->b14_b15_state = 2;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->b14_b15_state = 0;
                    }
                    break;
                case 2:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->b14_b15_syllable;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[B2] == 1)
                            pam->b14_b15_syllable = 0;
                        else
                            pam->b14_b15_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->b14_b15_state = 0;
                    }
                    break;
                default:
                    break;
            }
            break;
        case B15:/*Processor for B15 : done in B14*/
            break;
        case B16:/*done in createSyllable*/
            break;
        case B17:/*processor for B17, B19 unified */
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    switch (pam->b17_b19_state) {
                        case 0:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B17]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B19]
                                        = pam->b17_b19_syllable;
                                pam->b17_b19_syllable++;
                            }
                            if (((event_type == PICOPAM_EVENT_P_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                    && (pam->b17_b19_syllable > 1)) {
                                if (event_type == PICOPAM_EVENT_P_BOUND)
                                    pam->b17_b19_syllable = 1;
                                pam->b17_b19_state = 1;
                            }
                            break;
                        case 1:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B17]
                                        = pam->b17_b19_syllable;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B19]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->b17_b19_syllable++;
                            }
                            if (event_type == PICOPAM_EVENT_P_BOUND) {
                                pam->b17_b19_syllable = 1;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    /*do nothing*/
                    break;
            }
            break;
        case B18:/*processor for B18, B20, B21 unfied*/
            switch (direction) {
                case PICOPAM_DIR_FORW:/*do nothing*/
                    break;
                case PICOPAM_DIR_BACK:
                    switch (pam->b18_b20_b21_state) {
                        case 0:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B18]
                                        = PICOPAM_DONT_CARE_VALUE;
                                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                                        == PICOPAM_DECLARATIVE) {
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[B20]
                                            = pam->b18_b20_b21_syllable;
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[B21]
                                            = PICOPAM_DONT_CARE_VALUE;
                                } else {
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[B20]
                                            = PICOPAM_DONT_CARE_VALUE;
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[B21]
                                            = pam->b18_b20_b21_syllable;
                                }
                                pam->b18_b20_b21_syllable++;
                            }
                            if (((event_type == PICOPAM_EVENT_P_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                    && (pam->b18_b20_b21_syllable > 1)) {
                                if (event_type == PICOPAM_EVENT_P_BOUND)
                                    pam->b18_b20_b21_syllable = 1;
                                pam->b18_b20_b21_state = 1;
                            }
                            break;
                        case 1:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B18]
                                        = pam->b18_b20_b21_syllable;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B20]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[B21]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->b18_b20_b21_syllable++;
                            }
                            if (event_type == PICOPAM_EVENT_P_BOUND) {
                                pam->b18_b20_b21_syllable = 1;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
            }
            break;
        case B19:/*processor for B19 : done in B17*/
            break;
        case B20:/*processor for B20 : done in B18*/
            break;
        case B21:/*processor for B21 : done in B18*/
            break;
        case C3:/*processor for C3*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    /*do nothing*/
                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1]
                                == 1) || (pam->c3_primary_phrase_syllable >= 1)) {
                            if (pam->c3_overall_syllable < 1)
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[C3]
                                        = 0;
                            else
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[C3]
                                        = pam->sSyllFeats[pam->nCurrSyllable
                                                + 1].phoneV[B3];
                        } else {
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[C3] = 0;
                        }
                        pam->c3_primary_phrase_syllable++;
                        pam->c3_overall_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->c3_primary_phrase_syllable = 0;
                    }
                    break;
            }
            break;
        case D2:/*processor for D2*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1]
                                == 1) || (pam->d2_current_primary_phrase_word
                                >= 1))
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[D2]
                                    = pam->d2_prev_syllable_in_word;
                        else
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[D2] = 0;

                        pam->d2_syllable_in_word++;
                    }
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND)) {
                        pam->d2_current_primary_phrase_word = 1;
                        pam->d2_prev_syllable_in_word
                                = pam->d2_syllable_in_word;
                        pam->d2_syllable_in_word = 0;
                        /*pam->d2_current_primary_phrase_word++;*/
                    }
                    if ((event_type == PICOPAM_EVENT_P_BOUND)) {
                        pam->d2_current_primary_phrase_word = 0;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    /*do nothing*/
                    break;
            }
            break;
        case E1:/*processor for E1*/
            switch (direction) {
                case PICOPAM_DIR_FORW: /*remember : content syllable indicator already on P5*/
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->e1_syllable_word_start == -1)
                            pam->e1_syllable_word_start
                                    = (picoos_int8) pam->nCurrSyllable;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P5] == 1)
                            pam->e1_content = 1;
                        pam->e1_syllable_word_end
                                = (picoos_int8) pam->nCurrSyllable;
                    }
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND)) {
                        if ((pam->e1_syllable_word_start != -1)
                                && (pam->e1_syllable_word_end != -1)) {
                            for (nI = pam->e1_syllable_word_start; nI
                                    <= pam->e1_syllable_word_end; nI++)
                                pam->sSyllFeats[nI].phoneV[E1]
                                        = pam->e1_content;
                        }
                        pam->e1_content = 0;
                        pam->e1_syllable_word_start = -1;
                        pam->e1_syllable_word_end = -1;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    /*do nothing*/
                    break;
            }
            break;
        case E2:/*processor for E2*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->e2_syllable_word_start == -1)
                            pam->e2_syllable_word_start
                                    = (picoos_int8) pam->nCurrSyllable;
                        pam->e2_syllable_word_end
                                = (picoos_int8) pam->nCurrSyllable;
                    }
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND)) {
                        if ((pam->e2_syllable_word_start != -1)
                                && (pam->e2_syllable_word_end != -1)) {
                            for (nI = pam->e2_syllable_word_start; nI
                                    <= pam->e2_syllable_word_end; nI++)
                                pam->sSyllFeats[nI].phoneV[E2]
                                        = pam->e2_syllable_word_end
                                                - pam->e2_syllable_word_start
                                                + 1;
                        }
                        pam->e1_content = 0;
                        pam->e2_syllable_word_start = -1;
                        pam->e2_syllable_word_end = -1;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    break;
            }
            break;
        case E3:/*processor for E3,E4*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = E3;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = E4;
                    break;
                default:
                    sDest = E3;
                    break;
            }
            switch (pam->e3_e4_state) {
                case 0:
                    if (event_type == PICOPAM_EVENT_SYLL)
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = PICOPAM_DONT_CARE_VALUE;
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->e3_e4_word = 1;
                        pam->e3_e4_state = 1;
                    }
                    break;
                case 1:
                    if (event_type == PICOPAM_EVENT_SYLL)
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->e3_e4_word;
                    if (event_type == PICOPAM_EVENT_S_BOUND)
                        pam->e3_e4_word = 1;
                    if (event_type == PICOPAM_EVENT_W_BOUND)
                        pam->e3_e4_word++;
                    if (event_type == PICOPAM_EVENT_P_BOUND)
                        pam->e3_e4_state = 0;
                    break;
                default:
                    break;
            }
            break;
        case E4:/*processor for E4 : done in E3*/
            break;
        case E5:/*processor for E5,E6*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = E5;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = E6;
                    break;
                default:
                    sDest = E5;
                    break;
            }
            if (event_type == PICOPAM_EVENT_SYLL) {
                pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                        = pam->e5_e6_content_word;
                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P5] == 1)
                    pam->e5_e6_content = 1;
            }
            if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                    == PICOPAM_EVENT_S_BOUND) || (event_type
                    == PICOPAM_EVENT_P_BOUND)) {
                if (pam->e5_e6_content == 1)
                    pam->e5_e6_content_word++;
                pam->e5_e6_content = 0;
                if (event_type == PICOPAM_EVENT_P_BOUND)
                    pam->e5_e6_content_word = 0;
            }
            break;
        case E6:/*processor for E6 : done in E5*/
            break;
        case E7:/*processor for E7,E8*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = E7;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = E8;
                    break;
                default:
                    sDest = E7;
                    break;
            }
            switch (pam->e7_e8_state) {
                case 0:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = PICOPAM_DONT_CARE_VALUE;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P5] == 1)
                            pam->e7_e8_content = 1;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->e7_e8_content = 0;
                    }

                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND)) {
                        if (pam->e7_e8_content == 1) {
                            pam->e7_e8_word = 0;
                            pam->e7_e8_content = 0;
                            pam->e7_e8_state = 1;
                        }
                    }
                    break;
                case 1:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                                = pam->e7_e8_word;
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P5] == 1)
                            pam->e7_e8_content = 1;
                    }
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND)) {
                        if (pam->e7_e8_content == 1) {
                            pam->e7_e8_word = 0;
                            pam->e7_e8_content = 0;
                        } else {
                            pam->e7_e8_word++;
                        }
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->e7_e8_state = 0;
                        pam->e7_e8_content = 0; /*<<<<<< added */
                    }

                default:
                    break;
            }
            break;
        case E8:/*processor for E8 : done in E7*/
            break;
        case E9:
            /*processor for E9, E11*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    switch (pam->e9_e11_state) {
                        case 0:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E9]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E11]
                                        = pam->e9_e11_word;
                                pam->e9_e11_saw_word = 1; /*new variable, needs to be initialized to 0*/
                            }
                            if (event_type == PICOPAM_EVENT_W_BOUND)
                                pam->e9_e11_word++;
                            if (((event_type == PICOPAM_EVENT_P_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                    && (pam->e9_e11_saw_word == 1)) { /* modified*/
                                if (event_type == PICOPAM_EVENT_P_BOUND)
                                    pam->e9_e11_word = 1;
                                else
                                    pam->e9_e11_word++; /*modified*/
                                pam->e9_e11_state = 1;
                            }
                            break;
                        case 1:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E9]
                                        = pam->e9_e11_word;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E11]
                                        = PICOPAM_DONT_CARE_VALUE;
                            }
                            if ((event_type == PICOPAM_EVENT_W_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                pam->e9_e11_word++;
                            if (event_type == PICOPAM_EVENT_P_BOUND)
                                pam->e9_e11_word = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    /*do nothing*/
                    break;
            }
            break;
        case E10:/*processor for E10, E12, E13 unified*/
            switch (direction) {
                case PICOPAM_DIR_FORW:/*do nothing*/
                    break;
                case PICOPAM_DIR_BACK:
                    switch (pam->e10_e12_e13_state) {
                        case 0:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E10]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->e10_e12_e13_saw_word = 1;
                                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                                        == PICOPAM_DECLARATIVE) {
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[E12]
                                            = pam->e10_e12_e13_word;
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[E13]
                                            = PICOPAM_DONT_CARE_VALUE;
                                } else {
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[E12]
                                            = PICOPAM_DONT_CARE_VALUE;
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[E13]
                                            = pam->e10_e12_e13_word;
                                }
                            }
                            if (event_type == PICOPAM_EVENT_W_BOUND)
                                pam->e10_e12_e13_word++;

                            /*if (((event_type==PICOPAM_EVENT_P_BOUND)||(event_type==PICOPAM_EVENT_S_BOUND))&&(pam->e10_e12_e13_word>1))    {*/
                            if (((event_type == PICOPAM_EVENT_P_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                    && (pam->e10_e12_e13_saw_word > 0)) {
                                if (event_type == PICOPAM_EVENT_P_BOUND)
                                    pam->e10_e12_e13_word = 1;
                                else
                                    pam->e10_e12_e13_word++;
                                pam->e10_e12_e13_state = 1;
                            }
                            break;
                        case 1:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E10]
                                        = pam->e10_e12_e13_word;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E12]
                                        = PICOPAM_DONT_CARE_VALUE;
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[E13]
                                        = PICOPAM_DONT_CARE_VALUE;
                            }
                            if ((event_type == PICOPAM_EVENT_W_BOUND)
                                    || (event_type == PICOPAM_EVENT_S_BOUND))
                                pam->e10_e12_e13_word++;
                            if (event_type == PICOPAM_EVENT_P_BOUND)
                                pam->e10_e12_e13_word = 1;
                            break;
                        default:
                            break;
                    }
                    break;
            }
            break;

        case E11:/*processor for E11 : done in E9*/
            break;
        case E12:/*processor for E12 : done in E10*/
            break;
        case E13:/*processor for E13 : done in E10*/
            break;

        case F2:
            switch (direction) {
                case PICOPAM_DIR_FORW:/*do nothing*/
                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->f2_current_primary_phrase_word >= 1)/*at least second word in current primary phrase*/
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[F2]
                                    = pam->f2_next_word_syllable;
                        else
                            /*first word in current primary phrase*/
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[F2] = 0;
                        pam->f2_word_syllable++;
                    }
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND)) {/*word - end : switch*/
                        pam->f2_next_word_syllable = pam->f2_word_syllable;
                        pam->f2_word_syllable = 0;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND)/*mark first word in current primary phrase*/
                        pam->f2_current_primary_phrase_word = 0;
                    else /*mark next word in current primary phrase(enables output in PICOPAM_EVENT_SYLL)*/
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND))
                        pam->f2_current_primary_phrase_word++;
                    break;
            }
            break;
        case G1:
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->g1_current_secondary_phrase_syllable > 0)
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[G1]
                                    = pam->g1_current_secondary_phrase_syllable;
                        else
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[G1] = 0;
                        pam->g1_current_syllable++;
                    }
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->g1_current_secondary_phrase_syllable
                                = pam->g1_current_syllable;
                        pam->g1_current_syllable = 0;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->g1_current_secondary_phrase_syllable = 0;
                        pam->g1_current_syllable = 0;
                    }
                case PICOPAM_DIR_BACK: /*do nothing*/
                    break;
            }
            break;
        case G2:
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->g2_current_secondary_phrase_word > 0)
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[G2]
                                    = pam->g2_current_secondary_phrase_word;
                        else
                            pam->sSyllFeats[pam->nCurrSyllable].phoneV[G2] = 0;
                    }
                    if (event_type == PICOPAM_EVENT_W_BOUND)
                        pam->g2_current_word++;

                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->g2_current_secondary_phrase_word
                                = pam->g2_current_word + 1;
                        pam->g2_current_word = 0;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->g2_current_secondary_phrase_word = 0;
                        pam->g2_current_word = 0;
                    }
                    break;
                case PICOPAM_DIR_BACK: /*do nothing*/
                    break;
            }
            break;
        case H1:
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->h1_current_secondary_phrase_syll++;
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H1]
                                = pam->h1_current_secondary_phrase_syll;
                    }
                    if ((event_type == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND))
                        pam->h1_current_secondary_phrase_syll = 0;
                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL)
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H1]
                                = pam->h1_current_secondary_phrase_syll;
                    if (event_type == PICOPAM_EVENT_S_BOUND)
                        pam->h1_current_secondary_phrase_syll
                                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H1];
                    if (event_type == PICOPAM_EVENT_P_BOUND)
                        pam->h1_current_secondary_phrase_syll
                                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H1];
                    break;
            }
            break;
        case H2:
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
                                = pam->h2_current_secondary_phrase_word;
                    }
                    if (event_type == PICOPAM_EVENT_W_BOUND) {
                        pam->h2_current_secondary_phrase_word++;
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
                                = pam->h2_current_secondary_phrase_word;
                    }
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
                                = pam->h2_current_secondary_phrase_word + 1;
                        pam->h2_current_secondary_phrase_word = 0;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        if (pam->nCurrSyllable > 1)
                            pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H2]
                                    = pam->h2_current_secondary_phrase_word + 1;
                        pam->h2_current_secondary_phrase_word = 0;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL)
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
                                = pam->h2_current_secondary_phrase_word;
                    if (event_type == PICOPAM_EVENT_S_BOUND)
                        pam->h2_current_secondary_phrase_word
                                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2];
                    if (event_type == PICOPAM_EVENT_P_BOUND)
                        pam->h2_current_secondary_phrase_word
                                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H2];
                    break;
            }
            break;
        case H3:/*processor for H3,H4 unified */
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    sDest = H3;
                    break;
                case PICOPAM_DIR_BACK:
                    sDest = H4;
                    break;
                default:
                    sDest = H3;
                    break;
            }
            if (event_type == PICOPAM_EVENT_SYLL) {
                pam->sSyllFeats[pam->nCurrSyllable].phoneV[sDest]
                        = pam->h3_h4_current_secondary_phrase_word;
            }
            if ((event_type == PICOPAM_EVENT_S_BOUND) || (event_type
                    == PICOPAM_EVENT_P_BOUND))
                pam->h3_h4_current_secondary_phrase_word++;
            break;
        case H4: /*processor for H4 : already in H3*/
            break;

        case H5:/*processor for H5*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    break;
                case PICOPAM_DIR_BACK:
                    switch (pam->h5_state) {
                        case 0:
                            if (event_type == PICOPAM_EVENT_SYLL)
                                pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5]
                                        = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5];
                            if (event_type == PICOPAM_EVENT_S_BOUND) {
                                pam->h5_state = 1;
                            }
                            break;
                        case 1:
                            if (event_type == PICOPAM_EVENT_SYLL) {
                                if ((pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5]
                                        == PICOPAM_P)
                                        && (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1]
                                                == 0))
                                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5]
                                            = PICOPAM_p;
                            }
                            if (event_type == PICOPAM_EVENT_P_BOUND) {
                                pam->h5_state = 0;
                            }
                            break;
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        case I1:/*processor for I1*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->i1_secondary_phrase_syllable++;
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I1]
                                = pam->i1_secondary_phrase_syllable;
                    }
                    if ((event_type == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND))
                        pam->i1_secondary_phrase_syllable = 0;
                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I1]
                                = pam->i1_next_secondary_phrase_syllable;
                    }
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->i1_next_secondary_phrase_syllable
                                = pam->i1_secondary_phrase_syllable;
                        pam->i1_secondary_phrase_syllable
                                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[I1];
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->i1_next_secondary_phrase_syllable = 0;
                        pam->i1_secondary_phrase_syllable
                                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[I1];
                    }
                    break;
            }
            break;
        case I2: /*processor for I2*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I2]
                                = pam->i2_secondary_phrase_word;
                    }
                    if (event_type == PICOPAM_EVENT_W_BOUND)
                        pam->i2_secondary_phrase_word++;

                    if ((event_type == PICOPAM_EVENT_P_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND))
                        pam->i2_secondary_phrase_word = 1;

                    break;
                case PICOPAM_DIR_BACK:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I2]
                                = pam->i2_next_secondary_phrase_word;
                    }
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->i2_next_secondary_phrase_word
                                = pam->i2_secondary_phrase_word;
                        pam->i2_secondary_phrase_word
                                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[I2];
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->i2_next_secondary_phrase_word = 0;
                        pam->i2_secondary_phrase_word
                                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[I2];
                    }
                    break;
            }
            break;
        case J1: /*processor for J1 */
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_SYLL) {
                        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1] != 1)
                            pam->j1_utterance_syllable++;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[J1]
                            = pam->j1_utterance_syllable;
                    break;
            }
            break;
        case J2: /*processor for J2*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if ((event_type == PICOPAM_EVENT_W_BOUND) || (event_type
                            == PICOPAM_EVENT_S_BOUND) || (event_type
                            == PICOPAM_EVENT_P_BOUND))
                        pam->j2_utterance_word++;
                    break;
                case PICOPAM_DIR_BACK:
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[J2]
                            = pam->j2_utterance_word - 1;
                    break;
            }
            break;
        case J3: /*processor for J3*/
            switch (direction) {
                case PICOPAM_DIR_FORW:
                    if (event_type == PICOPAM_EVENT_S_BOUND) {
                        pam->j3_utterance_sec_phrases++;
                        break;
                    }
                    if (event_type == PICOPAM_EVENT_P_BOUND) {
                        pam->j3_utterance_sec_phrases++;
                        break;
                    }
                    break;
                case PICOPAM_DIR_BACK:
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[J3]
                            = pam->j3_utterance_sec_phrases - 1;
                    break;
            }
            break;
    }
    return PICO_OK;
}/*pam_process_event_feature*/

/**
 * processes an input event spanning it to all column features
 * @param    this : pointer to PAM PU sub object pointer
 * @param    event_type : event id among syll/boundprim/boundsec/boundword
 * @param    direction : forward(0)/backward(1)
 * @return    PICO_OK : process OK
 * @return    PICO_ERR_OTHER : errors on getting pam sub obj pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_process_event(register picodata_ProcessingUnit this,
        picoos_uint8 event_type, picoos_uint8 direction)
{
    picoos_uint8 nFeat;
    pico_status_t nResult;

    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    if (direction == PICOPAM_DIR_FORW) {
        if (event_type == PICOPAM_EVENT_P_BOUND)
            /*primary boundary*/
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P2] = 1;
        if (event_type == PICOPAM_EVENT_S_BOUND)
            /*secondary boundary*/
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P3] = 1;
        if (event_type == PICOPAM_EVENT_W_BOUND)
            /*word boundary*/
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P4] = 1;
    }
    for (nFeat = A3; nFeat <= J3; nFeat++) {
        nResult = pam_process_event_feature(this, nFeat, event_type, direction);
        if (nResult != PICO_OK)
            return nResult;
    }
    return PICO_OK;
}/*pam_process_event*/

/**
 * inserts a syllable inside the subobj sentence data struct.
 * @param    this : pointer to PAM PU sub object pointer
 * @param    syllType  : the syllable type (pause/syllable)
 * @param    sContent : the item content
 * @param    sentType : the sentence type
 * @param    phType : the phrase type
 * @param    uBoundType : the boundary type (only for silence syllables)
 * @param    uMinDur, uMaxDur : the mimimum and maximum duration (only for silence syllables)
 * @return    PICO_OK : syllable creation successful
 * @return    PICO_ERR_OTHER : errors in one internal function (check_phones_size..)
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_create_syllable(register picodata_ProcessingUnit this,
        picoos_uint8 syllType, picoos_uint8 *sContent, picoos_uint8 sentType,
        picoos_uint8 phType, picoos_uint8 uBoundType, picoos_uint16 uMinDur,
        picoos_uint16 uMaxDur)
{
    pam_subobj_t *pam;
    picoos_uint8 nI;
    picoos_uint8 pos;
    /* picoos_uint8    *npUi16; */
    picoos_uint32 pos32;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;
    pos = 0;
    /*check buffer full condition on number of syllables*/
    if (check_syllables_size(pam, 1) != PICO_OK) {
        return PICO_ERR_OTHER;
    }

    if (syllType == PICOPAM_SYLL_PAUSE) {
        /*check buffer full condition on number of phonemes*/
        if (check_phones_size(pam, 1) != PICO_OK) {
            return PICO_ERR_OTHER;
        }
    }
    if (syllType == PICOPAM_SYLL_SYLL) {
        /*check item availability*/
        if (sContent == NULL) {
            return PICO_ERR_OTHER;
        }
        /*check buffer full condition  on number of phonemes*/
        if (check_phones_size(pam, sContent[3]) != PICO_OK) {
            return PICO_ERR_OTHER;
        }
    }

    /*open new syllable*/
    pam->nCurrSyllable = pam->nCurrSyllable + 1;
    /*cleanup*/
    for (nI = 0; nI < PICOPAM_VECT_SIZE; nI++) {
        if (pam->nCurrSyllable > 0) {
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[nI] = 0;
        } else {
            if ((nI >= ITM) && (nI <= itm)) {
                if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[nI] > 0) {
                    /*do not cleanup "attached item offset" fields (ITM, itm):
                     an already existing attached item could be lost*/
                } else {
                    /*cleanup "attached item offset"*/
                    pam->sSyllFeats[pam->nCurrSyllable].phoneV[nI] = 0;
                }
            } else {
                /*cleanup all fields except "attached item offset" (ITM, itm)*/
                pam->sSyllFeats[pam->nCurrSyllable].phoneV[nI] = 0;
            }
        }
    }
    /*set minimum and maximum duration values*/
    if ((uMinDur == 0) && (uMaxDur == 0) && (syllType == PICOPAM_SYLL_PAUSE)) {
        /*both 0 : use default duration limits for boundaries*/
        get_default_boundary_limit(uBoundType, &uMinDur, &uMaxDur);
    }
    if (uMinDur > 0) {
        pos32 = Min;
        picoos_write_mem_pi_uint16(pam->sSyllFeats[pam->nCurrSyllable].phoneV,
                &pos32, uMinDur);
    }
    if (uMaxDur > 0) {
        pos32 = Max;
        picoos_write_mem_pi_uint16(pam->sSyllFeats[pam->nCurrSyllable].phoneV,
                &pos32, uMaxDur);
    }
    /*END OF BREAK COMMAND SUPPORT*/

    if (syllType == PICOPAM_SYLL_PAUSE) {
        /*initialize a pause syllable*/
        if (sentType == PICOPAM_DECLARATIVE)
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                    = PICOPAM_DECLARATIVE;
        if (sentType == PICOPAM_INTERROGATIVE)
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                    = PICOPAM_INTERROGATIVE;

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[bnd] = uBoundType;
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[P1] = 1; /*this means the syllable contains a pause-silence*/
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[P8] = 1;

        /*b1,b2,b9,b11,b13,b15,e1,e6,e8,e10 already set to 0*/

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B3]
                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B4]
                        = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B5]
                                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B6]
                                        = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B7]
                                                = 1;

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B16]
                = PICOPAM_PH_DONT_CARE_VAL; /*name of the vowel in the syllable = NONE */

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E2]
                = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E3]
                        = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E4] = 1;

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5] = phType;

        /*Store current phonetic codes in input phonetic string*/
        pam->sPhIds[pam->nCurrPhoneme] = pam_get_pause_id(this);
        picoos_mem_copy((void*) &pam->nCurrPhoneme,
                &(pam->sSyllFeats[pam->nCurrSyllable].phoneV[FID]),
                sizeof(pam->nCurrPhoneme));
        pam->nCurrPhoneme++;
        pam->nTotalPhonemes++;
        /*add 1 to total number of syllables*/
        pam->nTotalSyllables++;

        return PICO_OK;
    }
    if (syllType == PICOPAM_SYLL_SYLL) {
        /*initialize a real syllable*/
        if (sContent[2] > PICODATA_ACC0)
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P5] = 1; /*set content syllable indicator*/
        if (sentType == PICOPAM_DECLARATIVE)
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                    = PICOPAM_DECLARATIVE;
        if (sentType == PICOPAM_INTERROGATIVE)
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[P6]
                    = PICOPAM_INTERROGATIVE;

        if ((sContent[2] >= PICODATA_ACC1) && (sContent[2] <= PICODATA_ACC4))
            /*stressed*/
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[B1] = 1;

        if ((sContent[2] >= PICODATA_ACC1) && (sContent[2] <= PICODATA_ACC2))
            /*accented*/
            pam->sSyllFeats[pam->nCurrSyllable].phoneV[B2] = 1;

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B3] = sContent[3];/*len*/

        if (pam->nCurrSyllable > 30)
            pam->nCurrSyllable = pam->nCurrSyllable;

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B16] = pam_get_vowel_name(this,
                sContent, &pos); /*name of the vowel in the syllable*/

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[P8] = pos; /*temp for storing the position of the vowel*/

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5] = phType;

        /*Store current phonetic codes in input phonetic string*/
        picoos_mem_copy((void*) &pam->nCurrPhoneme,
                &(pam->sSyllFeats[pam->nCurrSyllable].phoneV[FID]),
                sizeof(pam->nCurrPhoneme));
        for (nI = 0; nI < sContent[3]; nI++)
            pam->sPhIds[pam->nCurrPhoneme + nI] = sContent[4 + nI];
        pam->nCurrPhoneme += nI;
        pam->nTotalPhonemes += nI;
        /*add 1 to total number of syllables*/
        pam->nTotalSyllables++;
        return PICO_OK;
    }
    /*if no SyllType has been identified -->> error*/
    return PICO_ERR_OTHER;
}/*pam_create_syllable*/

/**
 * performs the forward step of the PAM adapter
 * @param    this : pointer to PAM PU sub object pointer
 * @param    itemBase : pointer to current item
 * @return    PICOPAM_READY : forward step ok, the sentence is complete
 * @return  PICOPAM_MORE : forward step ok, but more data needed to complete the sentence
 * @return    PICO_ERR_OTHER : errors in one internal function (CreateSyllable..)
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_adapter_forward_step(
        register picodata_ProcessingUnit this, picoos_uint8 *itemBase)
{
    register pam_subobj_t * pam;
    pico_status_t sResult;
    picoos_uint16 uMinDur, uMaxDur;
    picoos_uint32 nPos;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;
    uMinDur = uMaxDur = 0; /*default 0 : not initialized*/
    switch (itemBase[0]) {
        case PICODATA_ITEM_BOUND:
            /*received a boundary item*/
            switch (itemBase[1]) {
                case PICODATA_ITEMINFO1_BOUND_SBEG:
                case PICODATA_ITEMINFO1_BOUND_PHR1:
#ifdef PAM_PHR2_WITH_PR1
                case PICODATA_ITEMINFO1_BOUND_PHR2:
#endif
                case PICODATA_ITEMINFO1_BOUND_SEND:
                case PICODATA_ITEMINFO1_BOUND_TERM:
                    if (itemBase[3] == 2 * sizeof(picoos_uint16)) {
                        /*only when the item len duration is equal to 2 int16 --> get the values*/
                        nPos = 4;
                        picoos_read_mem_pi_uint16(itemBase, &nPos, &uMinDur);
                        picoos_read_mem_pi_uint16(itemBase, &nPos, &uMaxDur);
                    }
                    break;
                default:
                    break;
            }
            switch (itemBase[1]) {
                case PICODATA_ITEMINFO1_BOUND_SBEG:
                    /* received a sentence init boundary */
                    pam_reset_processors(this); /*reset all processor variables*/
                    pam->sType
                            = pam_map_sentence_type(itemBase[1], itemBase[2]);
                    pam->pType = pam_map_phrase_type(itemBase[1], itemBase[2]);
                    /*create silence syll and process P_BOUND event*/
                    sResult = pam_create_syllable(this, PICOPAM_SYLL_PAUSE, NULL,
                            pam->sType, pam->pType, itemBase[1], uMinDur,
                            uMaxDur);
                    if (sResult != PICO_OK)
                        return sResult;
                    sResult = pam_process_event(this, PICOPAM_EVENT_P_BOUND,
                            PICOPAM_DIR_FORW);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_MORE;
                    break;

                case PICODATA_ITEMINFO1_BOUND_PHR1:
#ifdef PAM_PHR2_WITH_PR1
                case PICODATA_ITEMINFO1_BOUND_PHR2:
#endif
                    /*received a primary boundary*/
                    pam->sType
                            = pam_map_sentence_type(itemBase[1], itemBase[2]);
                    pam->pType = pam_map_phrase_type(itemBase[1], itemBase[2]);
                    /*create silence syll and process P_BOUND event*/
                    sResult = pam_create_syllable(this, PICOPAM_SYLL_PAUSE, NULL,
                            pam->sType, pam->pType, itemBase[1], uMinDur,
                            uMaxDur);
                    if (sResult != PICO_OK)
                        return sResult;
                    sResult = pam_process_event(this, PICOPAM_EVENT_P_BOUND,
                            PICOPAM_DIR_FORW);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_MORE;
                    break;

#ifdef PAM_PHR2_WITH_PR3
                    case PICODATA_ITEMINFO1_BOUND_PHR2 :
#endif
                case PICODATA_ITEMINFO1_BOUND_PHR3:
                    /*received a secondary boundary*/
                    /*process S_BOUND event*/
                    sResult = pam_process_event(this, PICOPAM_EVENT_S_BOUND,
                            PICOPAM_DIR_FORW);
                    /*determine new sentence and Phrase types for following syllables*/
                    pam->sType
                            = pam_map_sentence_type(itemBase[1], itemBase[2]);
                    pam->pType = pam_map_phrase_type(itemBase[1], itemBase[2]);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_MORE;
                    break;

                case PICODATA_ITEMINFO1_BOUND_PHR0:
                    /*received a word end boundary*/
                    /*process W_BOUND event*/
                    sResult = pam_process_event(this, PICOPAM_EVENT_W_BOUND,
                            PICOPAM_DIR_FORW);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_MORE;
                    break;

                case PICODATA_ITEMINFO1_BOUND_SEND:
                    /*received a SEND boundary*/
                    /*insert a new silence syllable and process P_BOUND event*/
                    /*create silence syll and process P_BOUND event*/
                    sResult = pam_create_syllable(this, PICOPAM_SYLL_PAUSE, NULL,
                            pam->sType, pam->pType, itemBase[1], uMinDur,
                            uMaxDur);
                    if (sResult != PICO_OK)
                        return sResult;
                    sResult = pam_process_event(this, PICOPAM_EVENT_P_BOUND,
                            PICOPAM_DIR_FORW);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_READY;
                    break;

                case PICODATA_ITEMINFO1_BOUND_TERM:
                    /* received a flush boundary*/
                    if (pam->nCurrSyllable == -1) {
                        return PICOPAM_NA;
                    }
                    /*insert a new silence syllable and process P_BOUND event*/
                    /*create silence syll and process P_BOUND event*/
                    sResult = pam_create_syllable(this, PICOPAM_SYLL_PAUSE, NULL,
                            pam->sType, pam->pType, itemBase[1], uMinDur,
                            uMaxDur);
                    if (sResult != PICO_OK)
                        return sResult;
                    sResult = pam_process_event(this, PICOPAM_EVENT_P_BOUND,
                            PICOPAM_DIR_FORW);
                    if (sResult != PICO_OK)
                        return sResult;
                    return PICOPAM_READY;
                    break;

                default:
                    /*boundary type not known*/
                    return PICOPAM_NA;
                    break;
            }/*end switch (itemBase[1])*/
            break; /*end case PICODATA_ITEM_BOUND*/

        case PICODATA_ITEM_SYLLPHON:
            /*received a syllable item*/
            /*    ------------------------------------------------------------------
             following code has to be used if we do expect
             SYLL items arrive even without SBEG items starting the sentence.
             this may happen after a term has been issued to make room in local storage.
             */
            if (pam->nCurrSyllable == -1) {
                pam_reset_processors(this);
                /*insert an SBEG with sType and pType taken from previous sentence*/
                sResult = pam_create_syllable(this, PICOPAM_SYLL_PAUSE, NULL,
                        pam->sType, pam->pType, PICODATA_ITEMINFO1_BOUND_SBEG,
                        0, 0);
                if (sResult != PICO_OK)
                    return sResult;
                sResult = pam_process_event(this, PICOPAM_EVENT_P_BOUND,
                        PICOPAM_DIR_FORW);
                if (sResult != PICO_OK)
                    return sResult;
            }
            /* ------------------------------------------------------------------*/
            sResult = pam_create_syllable(this, PICOPAM_SYLL_SYLL, itemBase,
                    pam->sType, pam->pType, 0, 0, 0);
            if (sResult != PICO_OK)
                return sResult;
            sResult = pam_process_event(this, PICOPAM_EVENT_SYLL,
                    PICOPAM_DIR_FORW);
            if (sResult != PICO_OK)
                return sResult;
            return PICOPAM_MORE;
            break;

        default:
            return PICOPAM_NA;
            break;
    }
    return PICO_ERR_OTHER;
}/*pam_adapter_forward_step*/

/**
 * performs the backward step of the PAM adapter
 * @param    this : pointer to PAM PU sub object pointer
 * @return    PICO_OK : backward step complete
 * @return  PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @remarks derived in some parts from the pam forward code
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_adapter_backward_step(
        register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;
    picoos_uint8 nProcessed;
    picoos_uint16 nSyll;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*Resets the processors for the backward step*/
    pam_reset_processors_back(this);
    /*Do the backward step*/
    nSyll = pam->nCurrSyllable;
    while (pam->nCurrSyllable >= 0) {
        nProcessed = 0;
        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P2] == 1) {
            /*primary boundary*/
            pam_process_event(this, PICOPAM_EVENT_P_BOUND, PICOPAM_DIR_BACK);
            pam->nCurrSyllable--;
            nProcessed = 1;
        }
        if ((nProcessed == 0)
                && (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P3] == 1)) {
            /*secondary boundary*/
            pam_process_event(this, PICOPAM_EVENT_S_BOUND, PICOPAM_DIR_BACK);
            pam_process_event(this, PICOPAM_EVENT_SYLL, PICOPAM_DIR_BACK);
            pam->nCurrSyllable--;
            nProcessed = 1;
        }
        if ((nProcessed == 0)
                && (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P4] == 1)) {
            /*word boundary*/
            pam_process_event(this, PICOPAM_EVENT_W_BOUND, PICOPAM_DIR_BACK);
            pam_process_event(this, PICOPAM_EVENT_SYLL, PICOPAM_DIR_BACK);
            pam->nCurrSyllable--;
            nProcessed = 1;
        }
        if (nProcessed == 0) {
            /*non boundaried syllable*/
            pam_process_event(this, PICOPAM_EVENT_SYLL, PICOPAM_DIR_BACK);
            pam->nCurrSyllable--;
            nProcessed = 0;
        }
    }/*end while (pam->nCurrSyllable>=0)*/
    /*reset syllpointer to original value*/
    pam->nCurrSyllable = nSyll;
    /*Perform pause processing*/
    pam_adapter_do_pauses(this);
    pam->nCurrSyllable = 0;
    pam->nSyllPhoneme = 0;

    return PICO_OK;
}/*pam_adapter_backward_step*/

/**
 * processes a pause (silence) syllable after backward processing
 * @param    this : pointer to PAM PU sub object pointer : processes a pause (silence) syllable after backward processing
 * @return    PICO_OK : backward step complete
 * @return  PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @remarks pam->nCurrSyllable should point to a pause item
 * @remarks this function should be called after backward processing
 * @remarks this function corresponds to initializing silence phonemes with
 * @remarks values derived from previous or following syllables
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_do_pause(register picodata_ProcessingUnit this)
{
    picoos_uint16 syllCurr;
    pam_subobj_t *pam;
    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;
    syllCurr = pam->nCurrSyllable;

    /*processor for all features that can be inherited from previous syll (or word/phrase)*/
    if (pam->nCurrSyllable > 0) {
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[A3]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B3];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B8]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B8];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B10]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B10];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B12]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B12];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B14]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B14];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B17]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B17];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B19]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B19];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B20]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B20];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[B21]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[B21];

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[D2]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E2];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[G1]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H1];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[G2]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H2];

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E5]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E5];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E7]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E7];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E9]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E9];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E11]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E11];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E12]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E12];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E13]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E13];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[E13]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[E13];

        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H1]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H1];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H2];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H3]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H3];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H4]
                = pam->sSyllFeats[pam->nCurrSyllable - 1].phoneV[H4];

    } else {
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[A3]
            =pam->sSyllFeats[pam->nCurrSyllable].phoneV[B8]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B10]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B12]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B14]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B17]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B19]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B20]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[B21]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E5]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E9]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E11]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[E12]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H1]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H2]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[H3]
            = 0;

        /*init values different from 0*/
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H4]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[J3];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[H5] = PICOPAM_p;

    }

    /*processor for all features that can be inherited from next syll (or word/phrase)*/
    if (pam->nCurrSyllable < pam->nTotalSyllables - 1) {
        /*non last syllable*/
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[C3]
                = pam->sSyllFeats[pam->nCurrSyllable + 1].phoneV[B3];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[F2]
                = pam->sSyllFeats[pam->nCurrSyllable + 1].phoneV[E2];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I1]
                = pam->sSyllFeats[pam->nCurrSyllable + 1].phoneV[H1];
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[I2]
                = pam->sSyllFeats[pam->nCurrSyllable + 1].phoneV[H2];
    } else {
        /*last syllable*/
        pam->sSyllFeats[pam->nCurrSyllable].phoneV[C3]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[F2]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[I1]
            = pam->sSyllFeats[pam->nCurrSyllable].phoneV[I2]
            = 0;
    }

    /*Other fixed values derived from de-facto standard*/
    pam->sSyllFeats[pam->nCurrSyllable].phoneV[B18] = 0;

    return PICO_OK;
}/*pam_do_pause*/

/**
 * performs the initialization of pause "syllables"
 * @param    this : pointer to PAM PU sub object pointer : processes a pause (silence) syllable after backward processing
 * @return    PICO_OK : pause processing successful
 * @return  PICO_ERR_OTHER : errors on retrieving the PU pointer
 * @callgraph
 * @callergraph
 */
static pico_status_t pam_adapter_do_pauses(register picodata_ProcessingUnit this)
{
    register pam_subobj_t * pam;
    picoos_uint16 nSyll;

    if (NULL == this || NULL == this->subObj) {
        return PICO_ERR_OTHER;
    }
    pam = (pam_subobj_t *) this->subObj;

    /*Do the pause processing*/
    nSyll = pam->nCurrSyllable;
    while (pam->nCurrSyllable >= 0) {
        if (pam->sSyllFeats[pam->nCurrSyllable].phoneV[P2] == 1) {
            /*pause processing*/
            pam_do_pause(this);
        }
        pam->nCurrSyllable--;
    }/*end while (pam->nCurrSyllable>=0)*/
    /*reset syllpointer to original value*/
    pam->nCurrSyllable = nSyll;
    return PICOPAM_READY;
}/*pam_adapter_do_pauses*/

#ifdef __cplusplus
}
#endif

/* picopam.c end */
