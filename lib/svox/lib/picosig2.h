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
 * @file picosig2.h
 *
 * Signal Generation PU - Internal functions - header file
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */

#ifndef PICOSIG2_H_
#define PICOSIG2_H_

#include "picoos.h"
#include "picodsp.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

/*----------------------------------------------------------
 // Name    :   sig_innerobj
 // Function:   innerobject definition for the sig processing
 // Shortcut:   sig
 //---------------------------------------------------------*/
typedef struct sig_innerobj
{

    /*-----------------------Definition of the local storage for this PU--------*/
    picoos_int16 *idx_vect1; /*reserved for bit reversal tables*/
    picoos_int16 *idx_vect2; /*reserved for table lookup "A" vector*/
    picoos_int16 *idx_vect4; /*reserved for max peak index arrax in pchip*/
    picoos_int16 *idx_vect5; /*reserved for min index arrax in sigana_singleIMF*/
    picoos_int16 *idx_vect6; /*reserved for max index arrax in sigana_singleIMF*/
    picoos_int16 *idx_vect7; /*reserved for dispersed phase */
    picoos_int16 *idx_vect8; /*reserved for LocV*/
    picoos_int16 *idx_vect9; /*reserved for LocU*/

    picoos_int32 *int_vec22; /*reserved for normalized hanning window - fixed point */
    picoos_int32 *int_vec23; /*reserved for impresp  - fixed point */
    picoos_int32 *int_vec24; /*reserved for impresp  - fixed point */
    picoos_int32 *int_vec25; /*reserved for window  - fixed point */
    picoos_int32 *int_vec26; /*reserved for wavBuf  - fixed point */
    picoos_int32 *int_vec28; /*reserved for cepstral vectors input - fixed point */
    picoos_int32 *int_vec29; /*reserved for cepstral vectors input - fixed point */
    picoos_int32 *int_vec38; /*reserved for cepstral vectors input - fixed point */
    picoos_int32 *int_vec30; /*reserved for cepstral vectors input - fixed point */
    picoos_int32 *int_vec31; /*reserved for cepstral vectors input - fixed point */

    picoos_int32 *int_vec32; /*reserved for cepstral vectors input - fixed point */
    picoos_int32 *int_vec33; /*reserved for cepstral vectors input - fixed point */

    picoos_int32 *int_vec34; /* reserved for sin table- fixed point */
    picoos_int32 *int_vec35; /* reserved for cos table - fixed point */
    picoos_int32 *int_vec36; /* reserved for currently used sin table- fixed point */
    picoos_int32 *int_vec37; /* reserved for currently used cos table - fixed point */

    picoos_int32 *int_vec39; /* reserved for ang - fixed point */
    picoos_int32 *int_vec40; /* reserved for cos table - fixed point */

    picoos_int32 *int_vec41[CEPST_BUFF_SIZE]; /*reserved for phase smoothing - cepstrum buffers */
    picoos_int32 *int_vec42[PHASE_BUFF_SIZE]; /*reserved for phase smoothing - phase buffers */

    picoos_int16 idx_vect10[CEPST_BUFF_SIZE]; /*reserved for pitch value buffering before phase smoothing*/
    picoos_int16 idx_vect11[CEPST_BUFF_SIZE]; /*reserved for phonetic value bufferingid before phase smoothing*/
    picoos_int16 idx_vect12[CEPST_BUFF_SIZE]; /*reserved for voicing value bufferingbefore phase smoothing*/
    picoos_int16 idx_vect13[CEPST_BUFF_SIZE]; /*reserved for unrectified pitch value bufferingbefore phase smoothing*/
    picoos_int16 idx_vect14[PHASE_BUFF_SIZE]; /*reserved for vox_bnd value buffering before phase smoothing*/

    picoos_int32 *sig_vec1;

    picoos_single bvalue1; /*reserved for warp*/
    picoos_int32 ibvalue2; /*reserved for voxbnd*/
    picoos_int32 ibvalue3; /*reserved for voxbnd2*/
    picoos_single bvalue4; /*reserved for E*/
    picoos_single bvalue5; /*reserved for F0*/
    picoos_single bvalue6; /*reserved for sMod*/

    picoos_single bvalue7; /*reserved for voicing*/
    picoos_single bvalue8; /*reserved for unrectified pitch*/

    picoos_int16 ivalue1; /*reserved for m1,ceporder*/
    picoos_int16 ivalue2; /*reserved for m2,fftorder,windowlen*/
    picoos_int16 ivalue3; /*reserved for fftorder/2*/
    picoos_int16 ivalue4; /*reserved for framelen, displacement*/
    picoos_int16 ivalue5; /*reserved for voiced*/
    picoos_int16 ivalue6; /*reserved for generic result code*/
    picoos_int16 ivalue7; /*reserved for i*/
    picoos_int16 ivalue8; /*reserved for j*/
    picoos_int16 ivalue9; /*reserved for hop*/
    picoos_int16 ivalue10; /*reserved for nextPeak*/
    picoos_int16 ivalue11; /*reserved for nFrame*/
    picoos_int16 ivalue12; /*reserved for raw*/
    picoos_int16 ivalue13; /*reserved for hts engine flag*/
    picoos_int16 ivalue14; /*reserved for ph id*/
    picoos_int16 ivalue15; /*reserved for Voiced*/
    picoos_int16 ivalue16; /*reserved for prevVoiced*/
    picoos_int16 ivalue17; /*reserved for nV (size of LocV)*/
    picoos_int16 ivalue18; /*reserved for nU (size of LocU)*/

    picoos_int16 ivalue19; /*reserved for voicTrans*/

    picoos_int16 ivalue20; /*reserved for n_availabe index*/

    picoos_int32 lvalue1; /*reserved for sampling rate*/
    picoos_int32 lvalue2; /*reserved for VCutoff*/
    picoos_int32 lvalue3; /*reserved for UVCutoff*/
    picoos_int32 lvalue4; /*reserved for fMax */

    picoos_int32 iRand; /*reserved for phase random table poointer ())*/

} sig_innerobj_t;

/*------------------------------------------------------------------
 Exported (to picosig.c) Service routines :
 routine name and I/O parameters are to be maintained for PICO compatibility!!
 ------------------------------------------------------------------*/
extern pico_status_t sigAllocate(picoos_MemoryManager mm,
        sig_innerobj_t *sig_inObj);
extern void sigDeallocate(picoos_MemoryManager mm, sig_innerobj_t *sig_inObj);
extern void sigDspInitialize(sig_innerobj_t *sig_inObj, picoos_int32 resetMode);

/*------------------------------------------------------------------
 Exported (to picosig.c) Processing routines :
 routine number, name and content can be changed. unique I/O parameter should be  "sig"
 ------------------------------------------------------------------*/
extern void mel_2_lin_init(sig_innerobj_t *sig_inObj);
extern void save_transition_frame(sig_innerobj_t *sig_inObj);
extern void mel_2_lin_init(sig_innerobj_t *sig_inObj);
extern void post_filter_init(sig_innerobj_t *sig_inObj);
extern void mel_2_lin_lookup(sig_innerobj_t *sig_inObj, picoos_uint32 mgc);
extern void post_filter(sig_innerobj_t *sig_inObj);
extern void phase_spec2(sig_innerobj_t *sig_inObj);
extern void env_spec(sig_innerobj_t *sig_inObj);
extern void save_transition_frame(sig_innerobj_t *sig_inObj);
extern void td_psola2(sig_innerobj_t *sig_inObj);
extern void impulse_response(sig_innerobj_t *sig_inObj);
extern void overlap_add(sig_innerobj_t *sig_inObj);

/* -------------------------------------------------------------------
 * symbolic vs area assignements
 * -------------------------------------------------------------------*/
#define WavBuff_p   int_vec26       /*output is Wav buffer (2*FFTSize)*/
#define window_p    int_vec25       /*window function (hanning) */
#define ImpResp_p   int_vec23       /*output step 6*/
#define imp_p       int_vec24       /*output step 6*/
#define warp_p      bvalue1         /*warp factor */
#define voxbnd_p    ibvalue2         /*phase spectra reconstruction noise factor V*/  /*  fixed point */
#define voxbnd2_p   ibvalue3         /*phase spectra reconstruction noise factor UV */  /*  fixed point */
#define E_p         bvalue4         /*energy after Envelope spectrum calculation */
#define F0_p        bvalue5         /*pitch*/
#define sMod_p      bvalue6         /*speaker modification factor*/
#define voicing     bvalue7         /*voicing*/
#define Fuv_p       bvalue8         /*unrectified pitch (for unvoiced too)*/
#define m1_p        ivalue1         /*ceporder(melorder=ceporder-1) */
#define m2_p        ivalue2         /*fftorder*/
#define windowLen_p ivalue2         /*same as fftorder*/
#define hfftsize_p  ivalue3         /*fftorder/2*/
#define framesz_p   ivalue4         /*displacement*/
#define voiced_p    ivalue5         /*voicing state*/
#define nRes_p      ivalue6         /*generic result*/
#define i_p         ivalue7         /*generic counter*/
#define j_p         ivalue8         /*generic counter*/
#define hop_p       ivalue9         /*hop */
#define nextPeak_p  ivalue10        /*nextPeak*/
#define phId_p      ivalue14        /*phonetic id*/
#define prevVoiced_p ivalue16        /*previous voicing state (for frame 1)*/
#define nV          ivalue17
#define nU          ivalue18
#define VoicTrans   ivalue19        /*  */
#define Fs_p        lvalue1         /*Sampling frequency*/
#define VCutoff_p   lvalue2         /*voicing cut off frequency in Hz*/
#define UVCutoff_p  lvalue3         /*unvoicing cut off frequency in Hz*/
/* Reusable area */
#define wcep_pI     int_vec28       /*input step1*/
#define d_p         int_vec38       /*output mel_2_lin_init  : table lookup  vector D*/
#define A_p         idx_vect2       /*output mel_2_lin_init  : table lookup  vector A*/
#define ang_p       int_vec39       /*output step4*/
#define EnV         int_vec30
#define EnU         int_vec31
#define randCosTbl  int_vec34
#define randSinTbl  int_vec35
#define outCosTbl   int_vec36
#define outSinTbl   int_vec37
#define cos_table   int_vec40
#define norm_window_p int_vec22     /*window function (hanning) */
#define norm_window2_p int_vec27    /*window function (hanning) */
#define F2r_p       int_vec32       /*output step 7*/
#define F2i_p       int_vec33       /*output step 7*/
#define LocV        idx_vect8       /*excitation position voiced pulses*/
#define LocU        idx_vect9       /*excitation position unvoiced pulses*/

#define CepBuff       int_vec41     /*Buffer for incoming cepstral vector pointers*/
#define PhsBuff       int_vec42     /*Buffer for incoming phase vector pointers*/
#define F0Buff        idx_vect10    /*Buffer for incoming F0 values*/
#define PhIdBuff      idx_vect11    /*Buffer for incoming PhId values*/
#define VoicingBuff   idx_vect12    /*Buffer for incoming voicing values*/
#define FuVBuff       idx_vect13    /*Buffer for incoming FuV values*/
#define VoxBndBuff    idx_vect14    /*Buffer for incoming VoxBnd values*/

#define n_available   ivalue20      /*variable for indexing the incoming buffers*/


#ifdef __cplusplus
}
#endif

#endif
