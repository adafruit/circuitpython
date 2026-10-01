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
 * @file picosig2.c
 *
 * Signal Generation PU - Internal functions - Implementation
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
 */
#include "picoos.h"
#include "picodsp.h"
#include "picosig2.h"
#include "picofftsg.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif
/*---------------------------------------------------------------------------
 * INTERNAL FUNCTIONS DECLARATION
 *---------------------------------------------------------------------------*/
static void gen_hann2(sig_innerobj_t *sig_inObj);
static void get_simple_excitation(sig_innerobj_t *sig_inObj,
        picoos_int16 *nextPeak);
static void enh_wind_init(sig_innerobj_t *sig_inObj);
static void init_rand(sig_innerobj_t *sig_inObj);
static void get_trig(picoos_int32 ang, picoos_int32 *table, picoos_int32 *cs,
        picoos_int32 *sn);

/*---------------------------------------------------------------------------
 * PICO SYSTEM FUNCTIONS
 *---------------------------------------------------------------------------*/
/**
 * allocation of DSP memory for SIG PU
 * @param   mm : memory manager
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  PICO_OK : allocation successful
 * @return  PICO_ERR_OTHER : allocation NOT successful
 * @callgraph
 * @callergraph
 */
pico_status_t sigAllocate(picoos_MemoryManager mm, sig_innerobj_t *sig_inObj)
{
    picoos_int16 *data_i;
    picoos_int32 *d32;
    picoos_int32 nCount;

    sig_inObj->int_vec22 =
    sig_inObj->int_vec23 =
    sig_inObj->int_vec24 =
    sig_inObj->int_vec25 =
    sig_inObj->int_vec26 =
    sig_inObj->int_vec28 =
    sig_inObj->int_vec29 =
    sig_inObj->int_vec30 =
    sig_inObj->int_vec31 =
    sig_inObj->int_vec32 =
    sig_inObj->int_vec33 =
    sig_inObj->int_vec34 =
    sig_inObj->int_vec35 =
    sig_inObj->int_vec36 =
    sig_inObj->int_vec37 =
    sig_inObj->int_vec38 =
    sig_inObj->int_vec39 =
    sig_inObj->int_vec40 = NULL;

    sig_inObj->sig_vec1 = NULL;

    sig_inObj->idx_vect1 = sig_inObj->idx_vect2 = sig_inObj->idx_vect4 = NULL;
    sig_inObj->idx_vect5 = sig_inObj->idx_vect6 = sig_inObj->idx_vect7 =
    sig_inObj->idx_vect8 = sig_inObj->idx_vect9 = NULL;
    sig_inObj->ivalue17 = sig_inObj->ivalue18 = 0;

    /*-----------------------------------------------------------------
     * Memory allocations
     * NOTE : it would be far better to do a single allocation
     *          and to do pointer initialization inside this routine
     * ------------------------------------------------------------------*/
    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_FFTSIZE);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect1 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_HFFTSIZE_P1);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect2 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_FFTSIZE);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect4 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_FFTSIZE);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect5 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_FFTSIZE);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect6 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_HFFTSIZE_P1);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect7 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_MAX_EX);
    if (NULL == data_i) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect8 = data_i;

    data_i = (picoos_int16 *) picoos_allocate(mm, sizeof(picoos_int16)
            * PICODSP_MAX_EX);
    if (data_i == NULL) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->idx_vect9 = data_i;

    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec22 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec23 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec24 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec25 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE * 2);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec26 = d32;

    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec28 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec29 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec38 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec30 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec31 = d32;

    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec32 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec33 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_N_RAND_TABLE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec34 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_N_RAND_TABLE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec35 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_N_RAND_TABLE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec36 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_N_RAND_TABLE);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec37 = d32;

    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_HFFTSIZE_P1);
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec39 = d32;
    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32) * (1
            + PICODSP_COS_TABLE_LEN));
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->int_vec40 = d32;

    for (nCount = 0; nCount < CEPST_BUFF_SIZE; nCount++) {
        d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32) * (PICODSP_CEPORDER));
        if (NULL == d32) {
            sigDeallocate(mm, sig_inObj);
            return PICO_ERR_OTHER;
        }
        sig_inObj->int_vec41[nCount] = d32;
    }

    for (nCount = 0; nCount < PHASE_BUFF_SIZE; nCount++) {
        d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32) * (PICODSP_PHASEORDER));
        if (NULL == d32) {
            sigDeallocate(mm, sig_inObj);
            return PICO_ERR_OTHER;
        }
        sig_inObj->int_vec42[nCount] = d32;
    }

    d32 = (picoos_int32 *) picoos_allocate(mm, sizeof(picoos_int32)
            * PICODSP_FFTSIZE * 2); /* - fixed point */
    if (NULL == d32) {
        sigDeallocate(mm, sig_inObj);
        return PICO_ERR_OTHER;
    }
    sig_inObj->sig_vec1 = d32;

    return PICO_OK;
}/*sigAllocate*/

/**
 * frees DSP memory for SIG PU
 * @param   mm : memory manager
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @callgraph
 * @callergraph
 */
void sigDeallocate(picoos_MemoryManager mm, sig_innerobj_t *sig_inObj)
{
    picoos_int32 nCount;
    /*-----------------------------------------------------------------
     * Memory de-allocations
     * ------------------------------------------------------------------*/
    if (NULL != sig_inObj->idx_vect1)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect1));
    if (NULL != sig_inObj->idx_vect2)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect2));
    if (NULL != sig_inObj->idx_vect4)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect4));
    if (NULL != sig_inObj->idx_vect5)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect5));
    if (NULL != sig_inObj->idx_vect6)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect6));
    if (NULL != sig_inObj->idx_vect7)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect7));
    if (NULL != sig_inObj->idx_vect8)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect8));
    if (NULL != sig_inObj->idx_vect9)
        picoos_deallocate(mm, (void *) &(sig_inObj->idx_vect9));

    if (NULL != sig_inObj->int_vec22)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec22));
    if (NULL != sig_inObj->int_vec23)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec23));
    if (NULL != sig_inObj->int_vec24)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec24));
    if (NULL != sig_inObj->int_vec25)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec25));
    if (NULL != sig_inObj->int_vec26)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec26));
    if (NULL != sig_inObj->int_vec28)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec28));
    if (NULL != sig_inObj->int_vec29)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec29));
    if (NULL != sig_inObj->int_vec38)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec38));
    if (NULL != sig_inObj->int_vec30)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec30));
    if (NULL != sig_inObj->int_vec31)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec31));
    if (NULL != sig_inObj->int_vec32)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec32));
    if (NULL != sig_inObj->int_vec33)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec33));
    if (NULL != sig_inObj->int_vec34)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec34));
    if (NULL != sig_inObj->int_vec35)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec35));
    if (NULL != sig_inObj->int_vec36)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec36));
    if (NULL != sig_inObj->int_vec37)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec37));
    if (NULL != sig_inObj->int_vec39)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec39));
    if (NULL != sig_inObj->int_vec40)
        picoos_deallocate(mm, (void *) &(sig_inObj->int_vec40));

    for (nCount = 0; nCount < CEPST_BUFF_SIZE; nCount++) {
        if (NULL != sig_inObj->int_vec41[nCount]) {
            picoos_deallocate(mm, (void *) &(sig_inObj->int_vec41[nCount]));
        }
    }

    for (nCount = 0; nCount < PHASE_BUFF_SIZE; nCount++) {
        if (NULL != sig_inObj->int_vec42[nCount]) {
            picoos_deallocate(mm, (void *) &(sig_inObj->int_vec42[nCount]));
        }
    }

    if (NULL != sig_inObj->sig_vec1) {
        picoos_deallocate(mm, (void *) &(sig_inObj->sig_vec1));
    }
}/*sigDeAllocate*/

/**
 * initializes all memory neededed by DSP at instance creation time
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @callgraph
 * @callergraph
 */
void sigDspInitialize(sig_innerobj_t *sig_inObj, picoos_int32 resetMode)
{
    picoos_int32 i, j;
    picoos_int32 *pnt;

    if (resetMode == PICO_RESET_SOFT) {
        /*minimal initialization when receiving a soft reset */
        return;
    }
    /*-----------------------------------------------------------------
     * Initialization
     * ------------------------------------------------------------------*/
    sig_inObj->warp_p = PICODSP_FREQ_WARP_FACT;
    sig_inObj->VCutoff_p = PICODSP_V_CUTOFF_FREQ; /*voicing cut off frequency in Hz (will be modeled in the future)*/
    sig_inObj->UVCutoff_p = PICODSP_UV_CUTOFF_FREQ;/*unvoiced frames only (periodize lowest components to mask bad voicing transitions)*/
    sig_inObj->Fs_p = PICODSP_SAMP_FREQ; /*Sampling freq*/

    sig_inObj->m1_p = PICODSP_CEPORDER;
    sig_inObj->m2_p = PICODSP_FFTSIZE; /*also initializes windowLen*/
    sig_inObj->framesz_p = PICODSP_DISPLACE; /*1/4th of the frame size = displacement*/
    sig_inObj->hfftsize_p = PICODSP_H_FFTSIZE; /*half of the FFT size*/
    sig_inObj->voxbnd_p = (picoos_int32) ((picoos_single) sig_inObj->hfftsize_p
            / ((picoos_single) sig_inObj->Fs_p / (picoos_single) 2)
            * (picoos_single) sig_inObj->VCutoff_p);
    sig_inObj->voxbnd2_p
            = (picoos_int32) ((picoos_single) sig_inObj->hfftsize_p
                    / ((picoos_single) sig_inObj->Fs_p / (picoos_single) 2)
                    * (picoos_single) sig_inObj->UVCutoff_p);
    sig_inObj->hop_p = sig_inObj->framesz_p;
    sig_inObj->nextPeak_p = (((int) (PICODSP_FFTSIZE))
            / ((int) PICODSP_DISPLACE) - 1) * sig_inObj->hop_p;
    sig_inObj->phId_p = 0; /*phonetic id*/
    sig_inObj->E_p = (picoos_single) 0.0f;
    sig_inObj->F0_p = (picoos_single) 0.0f;
    sig_inObj->voiced_p = 0;
    sig_inObj->nV = sig_inObj->nU = 0;
    sig_inObj->sMod_p = (picoos_single) 1.0f;

    /*cleanup vectors*/
    for (i = 0; i < 2 * PICODSP_FFTSIZE; i++) {
        sig_inObj->sig_vec1[i] = 0;
        sig_inObj->int_vec26[i] = 0; /*wav buff cleanup */
    }

    for (i = 0; i < PICODSP_FFTSIZE; i++) {
        sig_inObj->idx_vect1[i] = sig_inObj->idx_vect4[i]
                = sig_inObj->idx_vect5[i] = sig_inObj->idx_vect6[i] = 0;
        sig_inObj->int_vec32[i] = sig_inObj->int_vec33[i] = 0;
    }

    for (i = 0; i < PICODSP_HFFTSIZE_P1; i++) {
        sig_inObj->idx_vect2[i] = (picoos_int16) 0;
    }

    for (i = 0; i < CEPST_BUFF_SIZE; i++) {
        sig_inObj->F0Buff[i]=0;
        sig_inObj->PhIdBuff[i]=0;
        sig_inObj->VoicingBuff[i]=0;
        sig_inObj->FuVBuff[i]=0;
        if (NULL != sig_inObj->CepBuff[i]) {
            pnt = sig_inObj->CepBuff[i];
            for (j = 0; j < PICODSP_CEPORDER; j++) {
                pnt[j] = 0;
            }
        }
    }

    for (i = 0; i < PHASE_BUFF_SIZE; i++) {
        if (NULL != sig_inObj->int_vec42[i]) {
            pnt = sig_inObj->int_vec42[i];
            for (j = 0; j < PICODSP_PHASEORDER; j++) {
                pnt[j] = 0;
            }
        }
    }
    sig_inObj->n_available=0;
    /*---------------------------------------------
     Init    formant enhancement window
     hanning window,
     Post Filter Hermite's interpolator Matrix
     Mel-2-Lin lookup tables
     ---------------------------------------------*/
    enh_wind_init(sig_inObj); /*creates the formant enhancement window*/
    init_rand(sig_inObj);
    gen_hann2(sig_inObj);
    mel_2_lin_init(sig_inObj);

}/*sigDspInitialize*/

/*-------------------------------------------------------------------------------
 PROCESSING FUNCTIONS : CALLED WITHIN sigStep (cfr. picosig.c)
 --------------------------------------------------------------------------------*/
/**
 * convert from mel scale to linear scale
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @param   scmeanMGC : mean value of the MGC
 * @return  void
 * @callgraph
 * @callergraph
 * @remarks translated from matlab code to c-code
 * Input
 * - c1 : input mfcc vector (ceporder=m1, real)
 * - m1 : input order
 * - A,B,D : lookup tables
 * - m2 : output order
 * - Xr,Xi (m2=FFT size, real) temporary arrays for FFT
 * - WNr,WNi (m2=FFT size, real) cos and sin precalculated tables
 * Output
 * - Xr (m2=FFT size, real) linear cepstral vector
 */
void mel_2_lin_lookup(sig_innerobj_t *sig_inObj, picoos_uint32 scmeanMGC)
{
    /*Local vars*/
    picoos_int16 nI, k;
    picoos_int32 delta, term1, term2;

    /*Local vars to be linked with sig data object*/
    picoos_int32 *c1, *XXr;
    picoos_single K1;
    picoos_int32 *D, K2, shift;
    picoos_int16 m1, *A, m2, m4, voiced, i;

    /*Link local variables with sig data object*/
    c1 = sig_inObj->wcep_pI;
    m1 = sig_inObj->m1_p;
    m2 = PICODSP_FFTSIZE;
    m4 = m2 >> 1;

    A = sig_inObj->A_p;
    D = sig_inObj->d_p;

    XXr = sig_inObj->wcep_pI;
    voiced = sig_inObj->voiced_p;

    shift = 27 - scmeanMGC;
    K2 = 1 << shift;
    K1 = (picoos_single) PICODSP_START_FLOAT_NORM * K2;
    XXr[0] = (picoos_int32) ((picoos_single) c1[0] * K1);
    for (nI = 1; nI < m1; nI++) {
        XXr[nI] = c1[nI] << shift;
    }
    i = sizeof(picoos_int32) * (PICODSP_FFTSIZE - m1);
    picoos_mem_set(XXr + m1, 0, i);
    dfct_nmf(m4, XXr); /* DFCT directly in fixed point */

    /* *****************************************************************************************
     Linear frequency scale envelope through interpolation.
     Two additions and one multiplication per entry.

     Optimization of linear interpolation algorithm
     - Start from 1 and stop at PICODSP_H_FFTSIZE-1 because 0 and PICODSP_H_FFTSIZE are invariant points
     - B[k]=A[k]+1 except for 0 and PICODSP_H_FFTSIZE
     - get rid of extra -1 operation by adapting the table A[]

     *******************************************************************************************/
    for (nI = 1; nI < PICODSP_H_FFTSIZE; nI++) {
        k = A[nI];
        term2 = XXr[k];
        term1 = XXr[k + 1];
        delta = term1 - term2;
        XXr[nI] = term2 + ((D[nI] * delta) >> 5); /* ok because nI<=A[nI] <=B[nI] */
    }
}/*mel_2_lin_lookup*/

/**
 * calculate phase
 * @remarks voiced phase taken from phase codebook and smoothed,
 * @remarks unvoiced phase - random
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @callgraph
 * @callergraph
 */
void phase_spec2(sig_innerobj_t *sig_inObj)
{
    picoos_int16 nI, iRand, firstUV;
    picoos_int32 *tmp1, *tmp2;
    picoos_int16 VOXBND_M1;
    picoos_int32 *spect, *ang;
    picoos_int16 voiced, m2;
    picoos_int32 *co, *so, *c, *s, voxbnd, voxbnd2;
    picoos_int16 i,j, k, n_comp;
    picoos_int16 *Pvoxbnd;
    picoos_int32 *phs_p2, *phs_p1, *phs_n1, *phs_n2;
    picoos_int32 *phs;

    /*Link local variables to sig data object*/
    spect = sig_inObj->wcep_pI; /* spect_p;*/
    /* current spect scale : times PICODSP_FIX_SCALE1 */
    ang = sig_inObj->ang_p;
    voxbnd = (picoos_int32) (sig_inObj->voxbnd_p * sig_inObj->voicing);
    voxbnd2 = sig_inObj->voxbnd2_p;
    voiced = sig_inObj->voiced_p;
    m2 = sig_inObj->m2_p;
    VOXBND_M1 = voxbnd - 1;
    firstUV = 1;

    /*code starts here*/
    if (voiced == 1) {
        firstUV = voxbnd;
        Pvoxbnd =  sig_inObj->VoxBndBuff;
        n_comp   = Pvoxbnd[2];
        phs_p2 = sig_inObj->PhsBuff[0];
        phs_p1 = sig_inObj->PhsBuff[1];
        phs    = sig_inObj->PhsBuff[2];
        phs_n1 = sig_inObj->PhsBuff[3];
        phs_n2 = sig_inObj->PhsBuff[4];

        /* find and smooth components which have full context */
        j = n_comp;
        for (i=0; i<5; i++) {
            if (Pvoxbnd[i]<j) j = Pvoxbnd[i];
        }
        for (i=0; i<j; i++) {
            ang[i] = -(((phs_p2[i]+phs_p1[i]+phs[i]+phs_n1[i]+phs_n2[i])<<6) / 5);
        }

        /* find and smooth components which at least one component on each side */
        k = n_comp;
        if (Pvoxbnd[2]<k) k = Pvoxbnd[2];
        if (Pvoxbnd[4]<k) k = Pvoxbnd[4];
        for (i=j; i<k; i++) {  /* smooth using only two surrounding neighbours */
                ang[i] = -(((phs_p1[i]+phs[i]+phs_n1[i])<<6) / 3);
        }

        /* handle rest of components - at least one side does not exist */
        for (i=k; i<n_comp; i++) {
            ang[i] = -(phs[i]<<6); /* - simple copy without smoothing */
        }

        /*Phase unwrap - cumsum */
        tmp1 = &(ang[1]);
        tmp2 = &(ang[0]);
        /* current ang scale : PICODSP_M_PI = PICODSP_FIX_SCALE2 */
        FAST_DEVICE(VOXBND_M1,*(tmp1++)+=*(tmp2)-PICODSP_FIX_SCALE2;*(tmp2)=(*tmp2>=0)?(*tmp2)>>PICODSP_SHIFT_FACT4:-((-(*tmp2))>>PICODSP_SHIFT_FACT4);tmp2++);
        *tmp2 = (*tmp2 >= 0) ? (*tmp2) >> PICODSP_SHIFT_FACT4 : -((-(*tmp2))
                >> PICODSP_SHIFT_FACT4); /*ang[voxbnd-1]/=2;*/
    }

    /* now for the unvoiced part */
    iRand = sig_inObj->iRand;
    c = sig_inObj->randCosTbl + iRand;
    s = sig_inObj->randSinTbl + iRand;
    co = sig_inObj->outCosTbl + firstUV;
    so = sig_inObj->outSinTbl + firstUV;
    for (nI = firstUV; nI < PICODSP_HFFTSIZE_P1 - 1; nI++) {
        *co++ = *c++;
        *so++ = *s++;
    }
    *co = 1;
    *so = 0;
    sig_inObj->iRand += (PICODSP_HFFTSIZE_P1 - firstUV);
    if (sig_inObj->iRand > PICODSP_N_RAND_TABLE - PICODSP_HFFTSIZE_P1)
        sig_inObj->iRand = 1 + sig_inObj->iRand + PICODSP_HFFTSIZE_P1
            - PICODSP_N_RAND_TABLE;
}/*phase_spec2*/

/**
 * Prepare Envelope spectrum for inverse FFT
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @remarks make phase bilateral -->> angh (FFT size, real)
 * @remarks combine in complex input vector for IFFT F = e**(spet/2+j*ang)
 * @remarks Compute energy -->> E (scalar, real)
 * @callgraph
 * @callergraph
 * Input
 * - spect (FFT size, real)
 * - ang (half FFT size -1, real)
 * - m2    fftsize
 *  - WNr,WNi (FFT size, real) the tabulated sine and cosine values
 *  - brev (FFT size, real) the tabulated bit reversal indexes
 * Output
 * - Fr, Fi (FFT size, complex) the envelope spectrum
 * - E (scalar, real) the energy
 */
void env_spec(sig_innerobj_t *sig_inObj)
{

    picoos_int16 nI;
    picoos_int32 fcX, fsX, fExp, voxbnd;
    picoos_int32 *spect, *ang, *ctbl;
    picoos_int16 voiced, prev_voiced;
    picoos_int32 *co, *so;
    picoos_int32 *Fr, *Fi;
    picoos_single mult;

    /*Link local variables to sig object*/
    spect = sig_inObj->wcep_pI; /*spect_p*/
    /*  current spect scale : times PICODSP_FIX_SCALE1 */
    ang = sig_inObj->ang_p;
    /*  current spect scale : PICODSP_M_PI =  PICODSP_FIX_SCALE2 */
    Fr = sig_inObj->F2r_p;
    Fi = sig_inObj->F2i_p;
    voiced = sig_inObj->voiced_p;
    prev_voiced = sig_inObj->prevVoiced_p;
    voxbnd = (picoos_int32) (sig_inObj->voxbnd_p * sig_inObj->voicing);
    ctbl = sig_inObj->cos_table;
    /*  ctbl scale : times 4096 */
    mult = PICODSP_ENVSPEC_K1 / PICODSP_FIX_SCALE1;

    /*remove dc from real part*/
    if (sig_inObj->F0_p > 120) {
        spect[0] = spect[1] = 0;
        spect[2] /= PICODSP_ENVSPEC_K2;
    } else {
        spect[0] = 0;
    }

    /* if using rand table, use sin and cos tables as well */
    if (voiced || (prev_voiced)) {
        /*Envelope becomes a complex exponential : F=exp(.5*spect + j*angh);*/
        for (nI = 0; nI < voxbnd; nI++) {
            get_trig(ang[nI], ctbl, &fcX, &fsX);
            fExp = (picoos_int32) EXP((double)spect[nI]*mult);
            Fr[nI] = fExp * fcX;
            Fi[nI] = fExp * fsX;
        }
        /*         ao=sig_inObj->ang_p+(picoos_int32)voxbnd; */
        co = sig_inObj->outCosTbl + voxbnd;
        so = sig_inObj->outSinTbl + voxbnd;

        for (nI = voxbnd; nI < PICODSP_HFFTSIZE_P1; nI++) {
            fcX = *co++;
            fsX = *so++;
            fExp = (picoos_int32) EXP((double)spect[nI]*mult);
            Fr[nI] = fExp * fcX;
            Fi[nI] = fExp * fsX;
        }
    } else {
        /*ao=sig_inObj->ang_p+1;*/
        co = sig_inObj->outCosTbl + 1;
        so = sig_inObj->outSinTbl + 1;
        for (nI = 1; nI < PICODSP_HFFTSIZE_P1; nI++) {
            fcX = *co++;
            fsX = *so++;
            fExp = (picoos_int32) EXP((double)spect[nI]*mult);

            Fr[nI] = fExp * fcX;
            Fi[nI] = fExp * fsX;
        }
    }

}/*env_spec*/

/**
 * Calculates the impulse response of the comlpex spectrum through inverse rFFT
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @remarks Imp corresponds with the real part of the FFT
 * @callgraph
 * @callergraph
 * Input
 * - Fr, Fi (FFT size, real & imaginary) the complex envelope spectrum (only first half of spectrum)
 * Output
 * - Imp: impulse response (length: m2)
 * - E (scalar, real) RMS value
 */
void impulse_response(sig_innerobj_t *sig_inObj)
{
    /*Define local variables*/
    picoos_single f;
    picoos_int16 nI, nn, m2, m4, voiced;
    picoos_single *E;
    picoos_int32 *norm_window; /* - fixed point */
    picoos_int32 *fr, *Fr, *Fi, *t1, ff; /* - fixed point */

    /*Link local variables with sig object*/
    m2 = sig_inObj->m2_p;
    m4 = m2 >> 1;
    Fr = sig_inObj->F2r_p;
    Fi = sig_inObj->F2i_p;
    norm_window = sig_inObj->norm_window_p;
    E = &(sig_inObj->E_p); /*as pointer: value will be modified*/
    voiced = sig_inObj->voiced_p;
    fr = sig_inObj->imp_p;

    /*Inverse FFT*/
    for (nI = 0, nn = 0; nI < m4; nI++, nn += 2) {
        fr[nn] = Fr[nI]; /* - fixed point */
    }

    fr[1] = (picoos_int32) (Fr[m4]);
    for (nI = 1, nn = 3; nI < m4; nI++, nn += 2) {
        fr[nn] = -Fi[nI]; /* - fixed point */
    }

    rdft(m2, -1, fr);
    /*window, normalize and differentiate*/
    *E = norm_result(m2, fr, norm_window);

    if (*E > 0) {
        f = *E * PICODSP_FIXRESP_NORM;
    } else {
        f = 20; /*PICODSP_FIXRESP_NORM*/
    }
    ff = (picoos_int32) f;
    if (ff < 1)
        ff = 1;
    /*normalize impulse response*/
    t1 = fr;FAST_DEVICE(PICODSP_FFTSIZE,*(t1++) /= ff;); /* - fixed point */

} /* impulse_response */

/**
 * time domain pitch synchronous overlap add over two frames (when no voicing transition)
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @remarks Special treatment at voicing boundaries
 * @remarks Introduced to get rid of time-domain aliasing (and additional speed up)
 * @callgraph
 * @callergraph
 */
void td_psola2(sig_innerobj_t *sig_inObj)
{
    picoos_int16 nI;
    picoos_int16 hop, m2, *nextPeak, voiced;
    picoos_int32 *t1, *t2;
    picoos_int16 cnt;
    picoos_int32 *fr, *v1, ff, f;
    picoos_int16 a, i;
    picoos_int32 *window;
    picoos_int16 s = (picoos_int16) 1;
    window = sig_inObj->window_p;

    /*Link local variables with sig object*/
    hop = sig_inObj->hop_p;
    m2 = sig_inObj->m2_p;
    nextPeak = &(sig_inObj->nextPeak_p);
    voiced = sig_inObj->voiced_p;
    fr = sig_inObj->imp_p;
    /*toggle the pointers and initialize signal vector */
    v1 = sig_inObj->sig_vec1;

    t1 = v1;
    FAST_DEVICE(PICODSP_FFTSIZE-PICODSP_DISPLACE,*(t1++)=0;);
    t1 = &(v1[PICODSP_FFTSIZE - PICODSP_DISPLACE]);
    t2 = &(v1[PICODSP_FFTSIZE]);
    FAST_DEVICE(PICODSP_FFTSIZE, *(t1++)=*(t2++););
    t1 = &(v1[2 * PICODSP_FFTSIZE - PICODSP_DISPLACE]);FAST_DEVICE(PICODSP_DISPLACE,*(t1++)=0;);
    /*calculate excitation points*/
    get_simple_excitation(sig_inObj, nextPeak);

    /*TD-PSOLA based on excitation vector */
    if ((sig_inObj->nU == 0) && (sig_inObj->voiced_p == 1)) {
        /* purely voiced */
        for (nI = 0; nI < sig_inObj->nV; nI++) {
            f = sig_inObj->EnV[nI];
            a = 0;
            cnt = PICODSP_FFTSIZE;
            ff = (f * window[sig_inObj->LocV[nI]]) >> PICODSP_SHIFT_FACT1;
            t1 = &(v1[a + sig_inObj->LocV[nI]]);
            t2 = &(fr[a]);
            if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff;);
        }
    } else if ((sig_inObj->nV == 0) && (sig_inObj->voiced_p == 0)) {
        /* PURELY UNVOICED*/
        for (nI = 0; nI < sig_inObj->nU; nI++) {
            f = sig_inObj->EnU[nI];
            s = -s; /*reverse order to reduce the periodicity effect*/
            if (s == 1) {
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[a + sig_inObj->LocU[nI]]);
                t2 = &(fr[a]);
                if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff; );
            } else { /*s==-1*/
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[(m2 - 1 - a) + sig_inObj->LocU[nI]]);
                t2 = &(fr[a]);
                if (cnt > 0)FAST_DEVICE(cnt,*(t1--)+=*(t2++)*ff; );
            }
        }
    } else if (sig_inObj->VoicTrans == 0) {
        /*voicing transition from unvoiced to voiced*/
        for (nI = 0; nI < sig_inObj->nV; nI++) {
            f = sig_inObj->EnV[nI];
            a = 0;
            cnt = PICODSP_FFTSIZE;
            ff = (f * window[sig_inObj->LocV[nI]]) >> PICODSP_SHIFT_FACT1;
            t1 = &(v1[a + sig_inObj->LocV[nI]]);
            t2 = &(fr[a]);
            if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff;);
        }
        /*add remaining stuff from unvoiced part*/
        for (nI = 0; nI < sig_inObj->nU; nI++) {
            f = sig_inObj->EnU[nI];
            s = -s; /*reverse order to reduce the periodicity effect*/
            if (s == 1) {
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[a + sig_inObj->LocU[nI]]);
                t2 = &(sig_inObj->ImpResp_p[a]); /*saved impulse response*/
                if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff; );
            } else {
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[(m2 - 1 - a) + sig_inObj->LocU[nI]]);
                t2 = &(sig_inObj->ImpResp_p[a]);
                if (cnt > 0)FAST_DEVICE(cnt,*(t1--)+=*(t2++)*ff; );
            }
        }
    } else {
        /*voiced to unvoiced*/
        for (nI = 0; nI < sig_inObj->nU; nI++) {
            f = sig_inObj->EnU[nI];
            s = -s; /*reverse order to reduce the periodicity effect*/
            if (s > 0) {
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[a + sig_inObj->LocU[nI]]);
                t2 = &(fr[a]);
                if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff; );
            } else {
                a = 0;
                cnt = PICODSP_FFTSIZE;
                ff = (f * window[sig_inObj->LocU[nI]]) >> PICODSP_SHIFT_FACT1;
                t1 = &(v1[(m2 - 1 - a) + sig_inObj->LocU[nI]]);
                t2 = &(fr[a]);
                if (cnt > 0)FAST_DEVICE(cnt,*(t1--)+=*(t2++)*ff; );
            }
        }
        /*add remaining stuff from voiced part*/
        for (nI = 0; nI < sig_inObj->nV; nI++) {
            f = sig_inObj->EnV[nI];
            a = 0;
            cnt = PICODSP_FFTSIZE;
            ff = (f * window[sig_inObj->LocV[nI]]) >> PICODSP_SHIFT_FACT1;
            t1 = &(v1[a + sig_inObj->LocV[nI]]);
            t2 = &(sig_inObj->ImpResp_p[a]);
            if (cnt > 0)FAST_DEVICE(cnt,*(t1++)+=*(t2++)*ff;);
        }
    }

    t1 = sig_inObj->sig_vec1;
    for (i = 0; i < PICODSP_FFTSIZE; i++, t1++) {
        if (*t1 >= 0)
            *t1 >>= PICODSP_SHIFT_FACT5;
        else
            *t1 = -((-*t1) >> PICODSP_SHIFT_FACT5);
    }

}/*td_psola2*/

/**
 * overlap + add summing of impulse responses on the final destination sample buffer
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @remarks Special treatment at voicing boundaries
 * @remarks Introduced to get rid of time-domain aliasing (and additional speed up)
 * Input
 * - wlet : the generic impulse response (FFT size, real)
 * - window : the windowing funcion (FFT size, real) fixed
 * - WavBuff : the destination buffer with past samples (FFT size*2, short)
 * - m2 : fftsize
 * Output
 * - WavBuff : the destination buffer with updated samples (FFT size*2, short)
 * @callgraph
 * @callergraph
 */
void overlap_add(sig_innerobj_t *sig_inObj)
{
    /*Local variables*/
    picoos_int32 *w, *v;

    /*Link local variables with sig object*/
    w = sig_inObj->WavBuff_p;
    v = sig_inObj->sig_vec1;

    FAST_DEVICE(PICODSP_FFTSIZE, *(w++)+=*(v++)<<PICODSP_SHIFT_FACT6;);

}/*overlap_add*/

/*-------------------------------------------------------------------------------
 INITIALIZATION AND INTERNAL    FUNCTIONS
 --------------------------------------------------------------------------------*/
/**
 * Hanning window initialization
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @return  PICO_OK
 * @callgraph
 * @callergraph
 */
static void gen_hann2(sig_innerobj_t *sig_inObj)
{
    picoos_int32 *hann;
    picoos_int32 *norm;
    /*link local variables with sig object*/
    hann = sig_inObj->window_p;
    norm = sig_inObj->norm_window_p;

    norm[0] = 80224;
    norm[1] = 320832;
    norm[2] = 721696;
    norm[3] = 1282560;
    norm[4] = 2003104;
    norm[5] = 2882880;
    norm[6] = 3921376;
    norm[7] = 5117984;
    norm[8] = 6471952;
    norm[9] = 7982496;
    norm[10] = 9648720;
    norm[11] = 11469616;
    norm[12] = 13444080;
    norm[13] = 15570960;
    norm[14] = 17848976;
    norm[15] = 20276752;
    norm[16] = 22852864;
    norm[17] = 25575744;
    norm[18] = 28443776;
    norm[19] = 31455264;
    norm[20] = 34608368;
    norm[21] = 37901248;
    norm[22] = 41331904;
    norm[23] = 44898304;
    norm[24] = 48598304;
    norm[25] = 52429696;
    norm[26] = 56390192;
    norm[27] = 60477408;
    norm[28] = 64688944;
    norm[29] = 69022240;
    norm[30] = 73474720;
    norm[31] = 78043744;
    norm[32] = 82726544;
    norm[33] = 87520352;
    norm[34] = 92422272;
    norm[35] = 97429408;
    norm[36] = 102538752;
    norm[37] = 107747248;
    norm[38] = 113051776;
    norm[39] = 118449184;
    norm[40] = 123936224;
    norm[41] = 129509648;
    norm[42] = 135166080;
    norm[43] = 140902192;
    norm[44] = 146714528;
    norm[45] = 152599584;
    norm[46] = 158553904;
    norm[47] = 164573888;
    norm[48] = 170655936;
    norm[49] = 176796448;
    norm[50] = 182991712;
    norm[51] = 189238064;
    norm[52] = 195531744;
    norm[53] = 201868992;
    norm[54] = 208246016;
    norm[55] = 214659040;
    norm[56] = 221104176;
    norm[57] = 227577616;
    norm[58] = 234075488;
    norm[59] = 240593872;
    norm[60] = 247128912;
    norm[61] = 253676688;
    norm[62] = 260233280;
    norm[63] = 266794768;
    norm[64] = 273357248;
    norm[65] = 279916768;
    norm[66] = 286469440;
    norm[67] = 293011360;
    norm[68] = 299538560;
    norm[69] = 306047168;
    norm[70] = 312533312;
    norm[71] = 318993088;
    norm[72] = 325422656;
    norm[73] = 331818144;
    norm[74] = 338175744;
    norm[75] = 344491680;
    norm[76] = 350762176;
    norm[77] = 356983424;
    norm[78] = 363151808;
    norm[79] = 369263520;
    norm[80] = 375315008;
    norm[81] = 381302592;
    norm[82] = 387222720;
    norm[83] = 393071872;
    norm[84] = 398846528;
    norm[85] = 404543232;
    norm[86] = 410158560;
    norm[87] = 415689216;
    norm[88] = 421131840;
    norm[89] = 426483200;
    norm[90] = 431740096;
    norm[91] = 436899392;
    norm[92] = 441958016;
    norm[93] = 446912928;
    norm[94] = 451761152;
    norm[95] = 456499840;
    norm[96] = 461126080;
    norm[97] = 465637152;
    norm[98] = 470030400;
    norm[99] = 474303104;
    norm[100] = 478452800;
    norm[101] = 482476960;
    norm[102] = 486373184;
    norm[103] = 490139200;
    norm[104] = 493772640;
    norm[105] = 497271424;
    norm[106] = 500633440;
    norm[107] = 503856704;
    norm[108] = 506939200;
    norm[109] = 509879168;
    norm[110] = 512674880;
    norm[111] = 515324544;
    norm[112] = 517826688;
    norm[113] = 520179776;
    norm[114] = 522382368;
    norm[115] = 524433184;
    norm[116] = 526331008;
    norm[117] = 528074688;
    norm[118] = 529663200;
    norm[119] = 531095552;
    norm[120] = 532370944;
    norm[121] = 533488576;
    norm[122] = 534447808;
    norm[123] = 535248000;
    norm[124] = 535888768;
    norm[125] = 536369664;
    norm[126] = 536690432;
    norm[127] = 536850880;
    norm[128] = 536850880;
    norm[129] = 536690432;
    norm[130] = 536369664;
    norm[131] = 535888768;
    norm[132] = 535248000;
    norm[133] = 534447808;
    norm[134] = 533488576;
    norm[135] = 532370944;
    norm[136] = 531095552;
    norm[137] = 529663200;
    norm[138] = 528074688;
    norm[139] = 526331008;
    norm[140] = 524433216;
    norm[141] = 522382368;
    norm[142] = 520179776;
    norm[143] = 517826688;
    norm[144] = 515324544;
    norm[145] = 512674880;
    norm[146] = 509879168;
    norm[147] = 506939200;
    norm[148] = 503856704;
    norm[149] = 500633472;
    norm[150] = 497271424;
    norm[151] = 493772672;
    norm[152] = 490139200;
    norm[153] = 486373184;
    norm[154] = 482476992;
    norm[155] = 478452800;
    norm[156] = 474303104;
    norm[157] = 470030400;
    norm[158] = 465637184;
    norm[159] = 461126080;
    norm[160] = 456499840;
    norm[161] = 451761152;
    norm[162] = 446912960;
    norm[163] = 441958016;
    norm[164] = 436899424;
    norm[165] = 431740096;
    norm[166] = 426483200;
    norm[167] = 421131840;
    norm[168] = 415689216;
    norm[169] = 410158560;
    norm[170] = 404543232;
    norm[171] = 398846528;
    norm[172] = 393071872;
    norm[173] = 387222720;
    norm[174] = 381302592;
    norm[175] = 375315008;
    norm[176] = 369263552;
    norm[177] = 363151808;
    norm[178] = 356983456;
    norm[179] = 350762176;
    norm[180] = 344491712;
    norm[181] = 338175776;
    norm[182] = 331818144;
    norm[183] = 325422656;
    norm[184] = 318993088;
    norm[185] = 312533312;
    norm[186] = 306047168;
    norm[187] = 299538560;
    norm[188] = 293011360;
    norm[189] = 286469472;
    norm[190] = 279916800;
    norm[191] = 273357248;
    norm[192] = 266794784;
    norm[193] = 260233280;
    norm[194] = 253676688;
    norm[195] = 247128928;
    norm[196] = 240593888;
    norm[197] = 234075488;
    norm[198] = 227577632;
    norm[199] = 221104192;
    norm[200] = 214659040;
    norm[201] = 208246032;
    norm[202] = 201868992;
    norm[203] = 195531744;
    norm[204] = 189238080;
    norm[205] = 182991728;
    norm[206] = 176796448;
    norm[207] = 170655952;
    norm[208] = 164573888;
    norm[209] = 158553920;
    norm[210] = 152599600;
    norm[211] = 146714528;
    norm[212] = 140902208;
    norm[213] = 135166096;
    norm[214] = 129509648;
    norm[215] = 123936240;
    norm[216] = 118449184;
    norm[217] = 113051776;
    norm[218] = 107747248;
    norm[219] = 102538752;
    norm[220] = 97429424;
    norm[221] = 92422288;
    norm[222] = 87520352;
    norm[223] = 82726544;
    norm[224] = 78043744;
    norm[225] = 73474736;
    norm[226] = 69022240;
    norm[227] = 64688944;
    norm[228] = 60477424;
    norm[229] = 56390192;
    norm[230] = 52429696;
    norm[231] = 48598304;
    norm[232] = 44898304;
    norm[233] = 41331904;
    norm[234] = 37901248;
    norm[235] = 34608384;
    norm[236] = 31455264;
    norm[237] = 28443792;
    norm[238] = 25575744;
    norm[239] = 22852864;
    norm[240] = 20276752;
    norm[241] = 17848976;
    norm[242] = 15570960;
    norm[243] = 13444080;
    norm[244] = 11469616;
    norm[245] = 9648720;
    norm[246] = 7982512;
    norm[247] = 6471952;
    norm[248] = 5117984;
    norm[249] = 3921376;
    norm[250] = 2882880;
    norm[251] = 2003104;
    norm[252] = 1282560;
    norm[253] = 721696;
    norm[254] = 320832;
    norm[255] = 80224;
    hann[0] = 0;
    hann[1] = 0;
    hann[2] = 1;
    hann[3] = 2;
    hann[4] = 3;
    hann[5] = 5;
    hann[6] = 7;
    hann[7] = 9;
    hann[8] = 12;
    hann[9] = 15;
    hann[10] = 18;
    hann[11] = 21;
    hann[12] = 25;
    hann[13] = 29;
    hann[14] = 34;
    hann[15] = 38;
    hann[16] = 43;
    hann[17] = 48;
    hann[18] = 54;
    hann[19] = 59;
    hann[20] = 66;
    hann[21] = 72;
    hann[22] = 78;
    hann[23] = 85;
    hann[24] = 92;
    hann[25] = 100;
    hann[26] = 107;
    hann[27] = 115;
    hann[28] = 123;
    hann[29] = 131;
    hann[30] = 140;
    hann[31] = 148;
    hann[32] = 157;
    hann[33] = 166;
    hann[34] = 176;
    hann[35] = 185;
    hann[36] = 195;
    hann[37] = 205;
    hann[38] = 215;
    hann[39] = 225;
    hann[40] = 236;
    hann[41] = 247;
    hann[42] = 257;
    hann[43] = 268;
    hann[44] = 279;
    hann[45] = 291;
    hann[46] = 302;
    hann[47] = 313;
    hann[48] = 325;
    hann[49] = 337;
    hann[50] = 349;
    hann[51] = 360;
    hann[52] = 372;
    hann[53] = 385;
    hann[54] = 397;
    hann[55] = 409;
    hann[56] = 421;
    hann[57] = 434;
    hann[58] = 446;
    hann[59] = 458;
    hann[60] = 471;
    hann[61] = 483;
    hann[62] = 496;
    hann[63] = 508;
    hann[64] = 521;
    hann[65] = 533;
    hann[66] = 546;
    hann[67] = 558;
    hann[68] = 571;
    hann[69] = 583;
    hann[70] = 596;
    hann[71] = 608;
    hann[72] = 620;
    hann[73] = 632;
    hann[74] = 645;
    hann[75] = 657;
    hann[76] = 669;
    hann[77] = 680;
    hann[78] = 692;
    hann[79] = 704;
    hann[80] = 715;
    hann[81] = 727;
    hann[82] = 738;
    hann[83] = 749;
    hann[84] = 760;
    hann[85] = 771;
    hann[86] = 782;
    hann[87] = 792;
    hann[88] = 803;
    hann[89] = 813;
    hann[90] = 823;
    hann[91] = 833;
    hann[92] = 842;
    hann[93] = 852;
    hann[94] = 861;
    hann[95] = 870;
    hann[96] = 879;
    hann[97] = 888;
    hann[98] = 896;
    hann[99] = 904;
    hann[100] = 912;
    hann[101] = 920;
    hann[102] = 927;
    hann[103] = 934;
    hann[104] = 941;
    hann[105] = 948;
    hann[106] = 954;
    hann[107] = 961;
    hann[108] = 966;
    hann[109] = 972;
    hann[110] = 977;
    hann[111] = 982;
    hann[112] = 987;
    hann[113] = 992;
    hann[114] = 996;
    hann[115] = 1000;
    hann[116] = 1003;
    hann[117] = 1007;
    hann[118] = 1010;
    hann[119] = 1012;
    hann[120] = 1015;
    hann[121] = 1017;
    hann[122] = 1019;
    hann[123] = 1020;
    hann[124] = 1022;
    hann[125] = 1023;
    hann[126] = 1023;
    hann[127] = 1023;
    hann[128] = 1023;
    hann[129] = 1023;
    hann[130] = 1023;
    hann[131] = 1022;
    hann[132] = 1020;
    hann[133] = 1019;
    hann[134] = 1017;
    hann[135] = 1015;
    hann[136] = 1012;
    hann[137] = 1010;
    hann[138] = 1007;
    hann[139] = 1003;
    hann[140] = 1000;
    hann[141] = 996;
    hann[142] = 992;
    hann[143] = 987;
    hann[144] = 982;
    hann[145] = 977;
    hann[146] = 972;
    hann[147] = 966;
    hann[148] = 961;
    hann[149] = 954;
    hann[150] = 948;
    hann[151] = 941;
    hann[152] = 934;
    hann[153] = 927;
    hann[154] = 920;
    hann[155] = 912;
    hann[156] = 904;
    hann[157] = 896;
    hann[158] = 888;
    hann[159] = 879;
    hann[160] = 870;
    hann[161] = 861;
    hann[162] = 852;
    hann[163] = 842;
    hann[164] = 833;
    hann[165] = 823;
    hann[166] = 813;
    hann[167] = 803;
    hann[168] = 792;
    hann[169] = 782;
    hann[170] = 771;
    hann[171] = 760;
    hann[172] = 749;
    hann[173] = 738;
    hann[174] = 727;
    hann[175] = 715;
    hann[176] = 704;
    hann[177] = 692;
    hann[178] = 680;
    hann[179] = 669;
    hann[180] = 657;
    hann[181] = 645;
    hann[182] = 632;
    hann[183] = 620;
    hann[184] = 608;
    hann[185] = 596;
    hann[186] = 583;
    hann[187] = 571;
    hann[188] = 558;
    hann[189] = 546;
    hann[190] = 533;
    hann[191] = 521;
    hann[192] = 508;
    hann[193] = 496;
    hann[194] = 483;
    hann[195] = 471;
    hann[196] = 458;
    hann[197] = 446;
    hann[198] = 434;
    hann[199] = 421;
    hann[200] = 409;
    hann[201] = 397;
    hann[202] = 385;
    hann[203] = 372;
    hann[204] = 360;
    hann[205] = 349;
    hann[206] = 337;
    hann[207] = 325;
    hann[208] = 313;
    hann[209] = 302;
    hann[210] = 291;
    hann[211] = 279;
    hann[212] = 268;
    hann[213] = 257;
    hann[214] = 247;
    hann[215] = 236;
    hann[216] = 225;
    hann[217] = 215;
    hann[218] = 205;
    hann[219] = 195;
    hann[220] = 185;
    hann[221] = 176;
    hann[222] = 166;
    hann[223] = 157;
    hann[224] = 148;
    hann[225] = 140;
    hann[226] = 131;
    hann[227] = 123;
    hann[228] = 115;
    hann[229] = 107;
    hann[230] = 100;
    hann[231] = 92;
    hann[232] = 85;
    hann[233] = 78;
    hann[234] = 72;
    hann[235] = 66;
    hann[236] = 59;
    hann[237] = 54;
    hann[238] = 48;
    hann[239] = 43;
    hann[240] = 38;
    hann[241] = 34;
    hann[242] = 29;
    hann[243] = 25;
    hann[244] = 21;
    hann[245] = 18;
    hann[246] = 15;
    hann[247] = 12;
    hann[248] = 9;
    hann[249] = 7;
    hann[250] = 5;
    hann[251] = 3;
    hann[252] = 2;
    hann[253] = 1;
    hann[254] = 0;
    hann[255] = 0;

} /* gen_hann2 */

/**
 * Creates the non uniform enhancement window (bilinearly mapped hannning window)
 * in order to reduce the formant enhancement in the high spectrum
 * @param    mm : memory manager
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @return   void
 * @callgraph
 * @callergraph
 * @remarks the outgput is based on the matlab script below\n
 matlab script
 -------------
 function makeEnhWind(alpha)
 N=129; % =HFFTSIZEE_P1
 s=(bilinmap(alpha,N));
 h=[hann(N)'];
 x=(1:N)/N;
 W=interp1(x,h,s); W(1)=0;

 fid=fopen('enhwind.txt','wt');
 fprintf(fid,'picoos_int16 enh_wind_init(sig_innerobj_t *sig_inObj) {\n');
 for i=1:N,
 fprintf(fid,['   sig_inObj->enhwind[' int2str(i) '] = (picoos_single)' num2str(W(i),'%0.7g') ';\n']);
 end;
 fprintf(fid,'   return PICO_OK;\n');
 fprintf(fid,' }; \n');
 fclose(fid);
 %figure(1); plot(x*8,W);
 */
static void enh_wind_init(sig_innerobj_t *sig_inObj)
{
    /*     picoos_int16 i; */
    picoos_int32 *c;

    c = sig_inObj->cos_table;

    c[0] = 4096;
    c[1] = 4095;
    c[2] = 4095;
    c[3] = 4095;
    c[4] = 4095;
    c[5] = 4095;
    c[6] = 4095;
    c[7] = 4095;
    c[8] = 4094;
    c[9] = 4094;
    c[10] = 4094;
    c[11] = 4093;
    c[12] = 4093;
    c[13] = 4092;
    c[14] = 4092;
    c[15] = 4091;
    c[16] = 4091;
    c[17] = 4090;
    c[18] = 4089;
    c[19] = 4089;
    c[20] = 4088;
    c[21] = 4087;
    c[22] = 4086;
    c[23] = 4085;
    c[24] = 4084;
    c[25] = 4083;
    c[26] = 4082;
    c[27] = 4081;
    c[28] = 4080;
    c[29] = 4079;
    c[30] = 4078;
    c[31] = 4077;
    c[32] = 4076;
    c[33] = 4075;
    c[34] = 4073;
    c[35] = 4072;
    c[36] = 4071;
    c[37] = 4069;
    c[38] = 4068;
    c[39] = 4066;
    c[40] = 4065;
    c[41] = 4063;
    c[42] = 4062;
    c[43] = 4060;
    c[44] = 4058;
    c[45] = 4057;
    c[46] = 4055;
    c[47] = 4053;
    c[48] = 4051;
    c[49] = 4049;
    c[50] = 4047;
    c[51] = 4045;
    c[52] = 4043;
    c[53] = 4041;
    c[54] = 4039;
    c[55] = 4037;
    c[56] = 4035;
    c[57] = 4033;
    c[58] = 4031;
    c[59] = 4029;
    c[60] = 4026;
    c[61] = 4024;
    c[62] = 4022;
    c[63] = 4019;
    c[64] = 4017;
    c[65] = 4014;
    c[66] = 4012;
    c[67] = 4009;
    c[68] = 4007;
    c[69] = 4004;
    c[70] = 4001;
    c[71] = 3999;
    c[72] = 3996;
    c[73] = 3993;
    c[74] = 3990;
    c[75] = 3988;
    c[76] = 3985;
    c[77] = 3982;
    c[78] = 3979;
    c[79] = 3976;
    c[80] = 3973;
    c[81] = 3970;
    c[82] = 3967;
    c[83] = 3963;
    c[84] = 3960;
    c[85] = 3957;
    c[86] = 3954;
    c[87] = 3950;
    c[88] = 3947;
    c[89] = 3944;
    c[90] = 3940;
    c[91] = 3937;
    c[92] = 3933;
    c[93] = 3930;
    c[94] = 3926;
    c[95] = 3923;
    c[96] = 3919;
    c[97] = 3915;
    c[98] = 3912;
    c[99] = 3908;
    c[100] = 3904;
    c[101] = 3900;
    c[102] = 3897;
    c[103] = 3893;
    c[104] = 3889;
    c[105] = 3885;
    c[106] = 3881;
    c[107] = 3877;
    c[108] = 3873;
    c[109] = 3869;
    c[110] = 3864;
    c[111] = 3860;
    c[112] = 3856;
    c[113] = 3852;
    c[114] = 3848;
    c[115] = 3843;
    c[116] = 3839;
    c[117] = 3834;
    c[118] = 3830;
    c[119] = 3826;
    c[120] = 3821;
    c[121] = 3816;
    c[122] = 3812;
    c[123] = 3807;
    c[124] = 3803;
    c[125] = 3798;
    c[126] = 3793;
    c[127] = 3789;
    c[128] = 3784;
    c[129] = 3779;
    c[130] = 3774;
    c[131] = 3769;
    c[132] = 3764;
    c[133] = 3759;
    c[134] = 3754;
    c[135] = 3749;
    c[136] = 3744;
    c[137] = 3739;
    c[138] = 3734;
    c[139] = 3729;
    c[140] = 3723;
    c[141] = 3718;
    c[142] = 3713;
    c[143] = 3708;
    c[144] = 3702;
    c[145] = 3697;
    c[146] = 3691;
    c[147] = 3686;
    c[148] = 3680;
    c[149] = 3675;
    c[150] = 3669;
    c[151] = 3664;
    c[152] = 3658;
    c[153] = 3652;
    c[154] = 3647;
    c[155] = 3641;
    c[156] = 3635;
    c[157] = 3629;
    c[158] = 3624;
    c[159] = 3618;
    c[160] = 3612;
    c[161] = 3606;
    c[162] = 3600;
    c[163] = 3594;
    c[164] = 3588;
    c[165] = 3582;
    c[166] = 3576;
    c[167] = 3570;
    c[168] = 3563;
    c[169] = 3557;
    c[170] = 3551;
    c[171] = 3545;
    c[172] = 3538;
    c[173] = 3532;
    c[174] = 3526;
    c[175] = 3519;
    c[176] = 3513;
    c[177] = 3506;
    c[178] = 3500;
    c[179] = 3493;
    c[180] = 3487;
    c[181] = 3480;
    c[182] = 3473;
    c[183] = 3467;
    c[184] = 3460;
    c[185] = 3453;
    c[186] = 3447;
    c[187] = 3440;
    c[188] = 3433;
    c[189] = 3426;
    c[190] = 3419;
    c[191] = 3412;
    c[192] = 3405;
    c[193] = 3398;
    c[194] = 3391;
    c[195] = 3384;
    c[196] = 3377;
    c[197] = 3370;
    c[198] = 3363;
    c[199] = 3356;
    c[200] = 3348;
    c[201] = 3341;
    c[202] = 3334;
    c[203] = 3326;
    c[204] = 3319;
    c[205] = 3312;
    c[206] = 3304;
    c[207] = 3297;
    c[208] = 3289;
    c[209] = 3282;
    c[210] = 3274;
    c[211] = 3267;
    c[212] = 3259;
    c[213] = 3252;
    c[214] = 3244;
    c[215] = 3236;
    c[216] = 3229;
    c[217] = 3221;
    c[218] = 3213;
    c[219] = 3205;
    c[220] = 3197;
    c[221] = 3190;
    c[222] = 3182;
    c[223] = 3174;
    c[224] = 3166;
    c[225] = 3158;
    c[226] = 3150;
    c[227] = 3142;
    c[228] = 3134;
    c[229] = 3126;
    c[230] = 3117;
    c[231] = 3109;
    c[232] = 3101;
    c[233] = 3093;
    c[234] = 3085;
    c[235] = 3076;
    c[236] = 3068;
    c[237] = 3060;
    c[238] = 3051;
    c[239] = 3043;
    c[240] = 3034;
    c[241] = 3026;
    c[242] = 3018;
    c[243] = 3009;
    c[244] = 3000;
    c[245] = 2992;
    c[246] = 2983;
    c[247] = 2975;
    c[248] = 2966;
    c[249] = 2957;
    c[250] = 2949;
    c[251] = 2940;
    c[252] = 2931;
    c[253] = 2922;
    c[254] = 2914;
    c[255] = 2905;
    c[256] = 2896;
    c[257] = 2887;
    c[258] = 2878;
    c[259] = 2869;
    c[260] = 2860;
    c[261] = 2851;
    c[262] = 2842;
    c[263] = 2833;
    c[264] = 2824;
    c[265] = 2815;
    c[266] = 2806;
    c[267] = 2796;
    c[268] = 2787;
    c[269] = 2778;
    c[270] = 2769;
    c[271] = 2760;
    c[272] = 2750;
    c[273] = 2741;
    c[274] = 2732;
    c[275] = 2722;
    c[276] = 2713;
    c[277] = 2703;
    c[278] = 2694;
    c[279] = 2684;
    c[280] = 2675;
    c[281] = 2665;
    c[282] = 2656;
    c[283] = 2646;
    c[284] = 2637;
    c[285] = 2627;
    c[286] = 2617;
    c[287] = 2608;
    c[288] = 2598;
    c[289] = 2588;
    c[290] = 2578;
    c[291] = 2569;
    c[292] = 2559;
    c[293] = 2549;
    c[294] = 2539;
    c[295] = 2529;
    c[296] = 2519;
    c[297] = 2510;
    c[298] = 2500;
    c[299] = 2490;
    c[300] = 2480;
    c[301] = 2470;
    c[302] = 2460;
    c[303] = 2450;
    c[304] = 2439;
    c[305] = 2429;
    c[306] = 2419;
    c[307] = 2409;
    c[308] = 2399;
    c[309] = 2389;
    c[310] = 2379;
    c[311] = 2368;
    c[312] = 2358;
    c[313] = 2348;
    c[314] = 2337;
    c[315] = 2327;
    c[316] = 2317;
    c[317] = 2306;
    c[318] = 2296;
    c[319] = 2286;
    c[320] = 2275;
    c[321] = 2265;
    c[322] = 2254;
    c[323] = 2244;
    c[324] = 2233;
    c[325] = 2223;
    c[326] = 2212;
    c[327] = 2201;
    c[328] = 2191;
    c[329] = 2180;
    c[330] = 2170;
    c[331] = 2159;
    c[332] = 2148;
    c[333] = 2138;
    c[334] = 2127;
    c[335] = 2116;
    c[336] = 2105;
    c[337] = 2094;
    c[338] = 2084;
    c[339] = 2073;
    c[340] = 2062;
    c[341] = 2051;
    c[342] = 2040;
    c[343] = 2029;
    c[344] = 2018;
    c[345] = 2007;
    c[346] = 1997;
    c[347] = 1986;
    c[348] = 1975;
    c[349] = 1964;
    c[350] = 1952;
    c[351] = 1941;
    c[352] = 1930;
    c[353] = 1919;
    c[354] = 1908;
    c[355] = 1897;
    c[356] = 1886;
    c[357] = 1875;
    c[358] = 1864;
    c[359] = 1852;
    c[360] = 1841;
    c[361] = 1830;
    c[362] = 1819;
    c[363] = 1807;
    c[364] = 1796;
    c[365] = 1785;
    c[366] = 1773;
    c[367] = 1762;
    c[368] = 1751;
    c[369] = 1739;
    c[370] = 1728;
    c[371] = 1717;
    c[372] = 1705;
    c[373] = 1694;
    c[374] = 1682;
    c[375] = 1671;
    c[376] = 1659;
    c[377] = 1648;
    c[378] = 1636;
    c[379] = 1625;
    c[380] = 1613;
    c[381] = 1602;
    c[382] = 1590;
    c[383] = 1579;
    c[384] = 1567;
    c[385] = 1555;
    c[386] = 1544;
    c[387] = 1532;
    c[388] = 1520;
    c[389] = 1509;
    c[390] = 1497;
    c[391] = 1485;
    c[392] = 1474;
    c[393] = 1462;
    c[394] = 1450;
    c[395] = 1438;
    c[396] = 1427;
    c[397] = 1415;
    c[398] = 1403;
    c[399] = 1391;
    c[400] = 1379;
    c[401] = 1368;
    c[402] = 1356;
    c[403] = 1344;
    c[404] = 1332;
    c[405] = 1320;
    c[406] = 1308;
    c[407] = 1296;
    c[408] = 1284;
    c[409] = 1272;
    c[410] = 1260;
    c[411] = 1248;
    c[412] = 1237;
    c[413] = 1225;
    c[414] = 1213;
    c[415] = 1201;
    c[416] = 1189;
    c[417] = 1176;
    c[418] = 1164;
    c[419] = 1152;
    c[420] = 1140;
    c[421] = 1128;
    c[422] = 1116;
    c[423] = 1104;
    c[424] = 1092;
    c[425] = 1080;
    c[426] = 1068;
    c[427] = 1056;
    c[428] = 1043;
    c[429] = 1031;
    c[430] = 1019;
    c[431] = 1007;
    c[432] = 995;
    c[433] = 983;
    c[434] = 970;
    c[435] = 958;
    c[436] = 946;
    c[437] = 934;
    c[438] = 921;
    c[439] = 909;
    c[440] = 897;
    c[441] = 885;
    c[442] = 872;
    c[443] = 860;
    c[444] = 848;
    c[445] = 836;
    c[446] = 823;
    c[447] = 811;
    c[448] = 799;
    c[449] = 786;
    c[450] = 774;
    c[451] = 762;
    c[452] = 749;
    c[453] = 737;
    c[454] = 725;
    c[455] = 712;
    c[456] = 700;
    c[457] = 687;
    c[458] = 675;
    c[459] = 663;
    c[460] = 650;
    c[461] = 638;
    c[462] = 625;
    c[463] = 613;
    c[464] = 601;
    c[465] = 588;
    c[466] = 576;
    c[467] = 563;
    c[468] = 551;
    c[469] = 538;
    c[470] = 526;
    c[471] = 513;
    c[472] = 501;
    c[473] = 488;
    c[474] = 476;
    c[475] = 463;
    c[476] = 451;
    c[477] = 438;
    c[478] = 426;
    c[479] = 413;
    c[480] = 401;
    c[481] = 388;
    c[482] = 376;
    c[483] = 363;
    c[484] = 351;
    c[485] = 338;
    c[486] = 326;
    c[487] = 313;
    c[488] = 301;
    c[489] = 288;
    c[490] = 276;
    c[491] = 263;
    c[492] = 251;
    c[493] = 238;
    c[494] = 226;
    c[495] = 213;
    c[496] = 200;
    c[497] = 188;
    c[498] = 175;
    c[499] = 163;
    c[500] = 150;
    c[501] = 138;
    c[502] = 125;
    c[503] = 113;
    c[504] = 100;
    c[505] = 87;
    c[506] = 75;
    c[507] = 62;
    c[508] = 50;
    c[509] = 37;
    c[510] = 25;
    c[511] = 12;
    c[512] = 0;
} /*enh_wind_init*/

/**
 * Initializes a useful large array of random numbers
 * @param  sig_inObj : sig PU internal object of the sub-object
 * @return void
 * @callgraph
 * @callergraph
 */
static void init_rand(sig_innerobj_t *sig_inObj)
{
    picoos_int32 *q = sig_inObj->int_vec34;
    picoos_int32 *r = sig_inObj->int_vec35;

    sig_inObj->iRand = 0;

    q[0] = -2198;
    r[0] = 3455;
    q[1] = 3226;
    r[1] = -2522;
    q[2] = -845;
    r[2] = 4007;
    q[3] = -1227;
    r[3] = 3907;
    q[4] = -3480;
    r[4] = 2158;
    q[5] = -1325;
    r[5] = -3875;
    q[6] = 2089;
    r[6] = -3522;
    q[7] = -468;
    r[7] = 4069;
    q[8] = 711;
    r[8] = -4033;
    q[9] = 3862;
    r[9] = 1362;
    q[10] = 4054;
    r[10] = -579;
    q[11] = 2825;
    r[11] = 2965;
    q[12] = 2704;
    r[12] = -3076;
    q[13] = 4081;
    r[13] = 344;
    q[14] = -3912;
    r[14] = 1211;
    q[15] = -3541;
    r[15] = 2058;
    q[16] = 2694;
    r[16] = 3084;
    q[17] = 835;
    r[17] = 4009;
    q[18] = -2578;
    r[18] = -3182;
    q[19] = 3205;
    r[19] = 2550;
    q[20] = -4074;
    r[20] = -418;
    q[21] = -183;
    r[21] = -4091;
    q[22] = -2665;
    r[22] = -3110;
    q[23] = -1367;
    r[23] = 3860;
    q[24] = -2266;
    r[24] = -3411;
    q[25] = 3327;
    r[25] = -2387;
    q[26] = -2807;
    r[26] = -2982;
    q[27] = -3175;
    r[27] = -2587;
    q[28] = -4095;
    r[28] = 27;
    q[29] = -811;
    r[29] = -4014;
    q[30] = 4082;
    r[30] = 332;
    q[31] = -2175;
    r[31] = 3470;
    q[32] = 3112;
    r[32] = 2662;
    q[33] = 1168;
    r[33] = -3925;
    q[34] = 2659;
    r[34] = 3115;
    q[35] = 4048;
    r[35] = 622;
    q[36] = 4092;
    r[36] = -165;
    q[37] = -4036;
    r[37] = 697;
    q[38] = 1081;
    r[38] = -3950;
    q[39] = -548;
    r[39] = 4059;
    q[40] = 4038;
    r[40] = 685;
    q[41] = -511;
    r[41] = 4063;
    q[42] = 3317;
    r[42] = -2402;
    q[43] = -3180;
    r[43] = 2580;
    q[44] = 851;
    r[44] = -4006;
    q[45] = 2458;
    r[45] = -3276;
    q[46] = -1453;
    r[46] = 3829;
    q[47] = -3577;
    r[47] = 1995;
    q[48] = -3708;
    r[48] = -1738;
    q[49] = -3890;
    r[49] = 1282;
    q[50] = 4041;
    r[50] = 666;
    q[51] = -3511;
    r[51] = -2108;
    q[52] = -1454;
    r[52] = -3828;
    q[53] = 2124;
    r[53] = 3502;
    q[54] = -3159;
    r[54] = 2606;
    q[55] = 2384;
    r[55] = -3330;
    q[56] = -3767;
    r[56] = -1607;
    q[57] = -4063;
    r[57] = -513;
    q[58] = 3952;
    r[58] = -1075;
    q[59] = -3778;
    r[59] = -1581;
    q[60] = -301;
    r[60] = -4084;
    q[61] = -4026;
    r[61] = 751;
    q[62] = -3346;
    r[62] = 2361;
    q[63] = -2426;
    r[63] = 3299;
    q[64] = 428;
    r[64] = -4073;
    q[65] = 3968;
    r[65] = 1012;
    q[66] = 2900;
    r[66] = -2892;
    q[67] = -263;
    r[67] = 4087;
    q[68] = 4083;
    r[68] = 322;
    q[69] = 2024;
    r[69] = 3560;
    q[70] = 4015;
    r[70] = 808;
    q[71] = -3971;
    r[71] = -1000;
    q[72] = 3785;
    r[72] = -1564;
    q[73] = -3726;
    r[73] = 1701;
    q[74] = -3714;
    r[74] = 1725;
    q[75] = 743;
    r[75] = 4027;
    q[76] = 875;
    r[76] = -4001;
    q[77] = 294;
    r[77] = 4085;
    q[78] = 2611;
    r[78] = 3155;
    q[79] = 2491;
    r[79] = -3251;
    q[80] = 1558;
    r[80] = 3787;
    q[81] = -2063;
    r[81] = -3538;
    q[82] = 3809;
    r[82] = -1505;
    q[83] = -2987;
    r[83] = 2802;
    q[84] = -1955;
    r[84] = 3599;
    q[85] = 1980;
    r[85] = -3585;
    q[86] = -539;
    r[86] = -4060;
    q[87] = -3210;
    r[87] = 2543;
    q[88] = 2415;
    r[88] = -3308;
    q[89] = 1587;
    r[89] = 3775;
    q[90] = -3943;
    r[90] = 1106;
    q[91] = 3476;
    r[91] = 2165;
    q[92] = 2253;
    r[92] = 3420;
    q[93] = -2584;
    r[93] = 3177;
    q[94] = 3804;
    r[94] = -1518;
    q[95] = -3637;
    r[95] = 1883;
    q[96] = 3289;
    r[96] = -2440;
    q[97] = -1621;
    r[97] = 3761;
    q[98] = 1645;
    r[98] = 3751;
    q[99] = -3471;
    r[99] = 2173;
    q[100] = 4071;
    r[100] = -449;
    q[101] = -872;
    r[101] = -4001;
    q[102] = -3897;
    r[102] = 1259;
    q[103] = -3590;
    r[103] = 1970;
    q[104] = -2456;
    r[104] = -3277;
    q[105] = -3004;
    r[105] = 2783;
    q[106] = 2589;
    r[106] = 3173;
    q[107] = 3727;
    r[107] = -1698;
    q[108] = 2992;
    r[108] = 2796;
    q[109] = 794;
    r[109] = -4018;
    q[110] = -918;
    r[110] = 3991;
    q[111] = 1446;
    r[111] = -3831;
    q[112] = 3871;
    r[112] = -1338;
    q[113] = -612;
    r[113] = -4049;
    q[114] = -1566;
    r[114] = -3784;
    q[115] = 672;
    r[115] = -4040;
    q[116] = 3841;
    r[116] = 1422;
    q[117] = 3545;
    r[117] = -2051;
    q[118] = -1982;
    r[118] = -3584;
    q[119] = -3413;
    r[119] = 2263;
    q[120] = -3265;
    r[120] = -2473;
    q[121] = -2876;
    r[121] = -2915;
    q[122] = 4094;
    r[122] = -117;
    q[123] = -269;
    r[123] = 4087;
    q[124] = -4077;
    r[124] = 391;
    q[125] = -3759;
    r[125] = 1626;
    q[126] = 1639;
    r[126] = 3753;
    q[127] = 3041;
    r[127] = -2743;
    q[128] = 5;
    r[128] = 4095;
    q[129] = 2778;
    r[129] = -3009;
    q[130] = 1121;
    r[130] = -3939;
    q[131] = -455;
    r[131] = -4070;
    q[132] = 3532;
    r[132] = 2073;
    q[133] = -143;
    r[133] = -4093;
    q[134] = -2357;
    r[134] = -3349;
    q[135] = 458;
    r[135] = 4070;
    q[136] = -2887;
    r[136] = -2904;
    q[137] = -1104;
    r[137] = 3944;
    q[138] = -2104;
    r[138] = -3513;
    q[139] = 126;
    r[139] = 4094;
    q[140] = -3655;
    r[140] = -1848;
    q[141] = -3896;
    r[141] = 1263;
    q[142] = -3874;
    r[142] = -1327;
    q[143] = 4058;
    r[143] = 553;
    q[144] = -1831;
    r[144] = -3663;
    q[145] = -255;
    r[145] = -4088;
    q[146] = -1211;
    r[146] = 3912;
    q[147] = 445;
    r[147] = 4071;
    q[148] = 2268;
    r[148] = 3410;
    q[149] = -4010;
    r[149] = 833;
    q[150] = 2621;
    r[150] = 3147;
    q[151] = -250;
    r[151] = 4088;
    q[152] = -3409;
    r[152] = -2269;
    q[153] = -2710;
    r[153] = -3070;
    q[154] = 4063;
    r[154] = 518;
    q[155] = -3611;
    r[155] = -1933;
    q[156] = -3707;
    r[156] = -1741;
    q[157] = -1151;
    r[157] = -3930;
    q[158] = 3976;
    r[158] = -983;
    q[159] = -1736;
    r[159] = 3709;
    q[160] = 3669;
    r[160] = 1820;
    q[161] = -143;
    r[161] = 4093;
    q[162] = -3879;
    r[162] = -1313;
    q[163] = -2242;
    r[163] = -3427;
    q[164] = -4095;
    r[164] = 0;
    q[165] = -1159;
    r[165] = -3928;
    q[166] = -3155;
    r[166] = 2611;
    q[167] = -2887;
    r[167] = -2904;
    q[168] = -4095;
    r[168] = 56;
    q[169] = -3861;
    r[169] = -1364;
    q[170] = -2814;
    r[170] = 2976;
    q[171] = -3680;
    r[171] = -1798;
    q[172] = -4094;
    r[172] = -107;
    q[173] = -3626;
    r[173] = 1903;
    q[174] = 3403;
    r[174] = 2278;
    q[175] = -1735;
    r[175] = -3710;
    q[176] = -2126;
    r[176] = -3500;
    q[177] = 3183;
    r[177] = -2577;
    q[178] = -3499;
    r[178] = 2128;
    q[179] = -1736;
    r[179] = 3709;
    q[180] = 2592;
    r[180] = -3170;
    q[181] = 3875;
    r[181] = 1326;
    q[182] = 3596;
    r[182] = 1960;
    q[183] = 3915;
    r[183] = -1202;
    q[184] = 1570;
    r[184] = 3783;
    q[185] = -3319;
    r[185] = -2400;
    q[186] = 4019;
    r[186] = 787;
    q[187] = -187;
    r[187] = 4091;
    q[188] = 1370;
    r[188] = -3859;
    q[189] = -4091;
    r[189] = 199;
    q[190] = 3626;
    r[190] = 1904;
    q[191] = -2943;
    r[191] = 2848;
    q[192] = 56;
    r[192] = 4095;
    q[193] = 2824;
    r[193] = 2966;
    q[194] = -3994;
    r[194] = -904;
    q[195] = 56;
    r[195] = 4095;
    q[196] = -2045;
    r[196] = 3548;
    q[197] = -3653;
    r[197] = 1850;
    q[198] = -2864;
    r[198] = 2927;
    q[199] = -1996;
    r[199] = 3576;
    q[200] = -4061;
    r[200] = 527;
    q[201] = 159;
    r[201] = 4092;
    q[202] = -3363;
    r[202] = 2336;
    q[203] = -4074;
    r[203] = 421;
    q[204] = 2043;
    r[204] = 3549;
    q[205] = 4095;
    r[205] = -70;
    q[206] = -2107;
    r[206] = -3512;
    q[207] = -1973;
    r[207] = 3589;
    q[208] = -3138;
    r[208] = 2631;
    q[209] = -3625;
    r[209] = -1905;
    q[210] = 2413;
    r[210] = 3309;
    q[211] = -50;
    r[211] = -4095;
    q[212] = 2813;
    r[212] = 2976;
    q[213] = -535;
    r[213] = -4060;
    q[214] = 1250;
    r[214] = 3900;
    q[215] = 1670;
    r[215] = -3739;
    q[216] = 1945;
    r[216] = -3604;
    q[217] = -476;
    r[217] = -4068;
    q[218] = -3659;
    r[218] = -1840;
    q[219] = 2745;
    r[219] = 3039;
    q[220] = -674;
    r[220] = -4040;
    q[221] = 2383;
    r[221] = 3330;
    q[222] = 4086;
    r[222] = 274;
    q[223] = -4030;
    r[223] = 730;
    q[224] = 768;
    r[224] = -4023;
    q[225] = 3925;
    r[225] = 1170;
    q[226] = 785;
    r[226] = 4019;
    q[227] = -3101;
    r[227] = -2675;
    q[228] = 4030;
    r[228] = -729;
    q[229] = 3422;
    r[229] = 2249;
    q[230] = -3847;
    r[230] = 1403;
    q[231] = 3902;
    r[231] = -1243;
    q[232] = 2114;
    r[232] = -3507;
    q[233] = -2359;
    r[233] = 3348;
    q[234] = 3754;
    r[234] = -1638;
    q[235] = -4095;
    r[235] = -83;
    q[236] = 2301;
    r[236] = -3388;
    q[237] = 3336;
    r[237] = 2375;
    q[238] = -2045;
    r[238] = 3548;
    q[239] = -413;
    r[239] = -4075;
    q[240] = 1848;
    r[240] = 3655;
    q[241] = 4072;
    r[241] = -437;
    q[242] = 4069;
    r[242] = -463;
    q[243] = 1386;
    r[243] = -3854;
    q[244] = 966;
    r[244] = 3980;
    q[245] = -1684;
    r[245] = -3733;
    q[246] = 2953;
    r[246] = 2837;
    q[247] = -3961;
    r[247] = -1040;
    q[248] = 3512;
    r[248] = -2107;
    q[249] = 1363;
    r[249] = 3862;
    q[250] = 1883;
    r[250] = 3637;
    q[251] = 2657;
    r[251] = 3116;
    q[252] = 2347;
    r[252] = -3356;
    q[253] = -1635;
    r[253] = -3755;
    q[254] = 3170;
    r[254] = 2593;
    q[255] = 2856;
    r[255] = 2935;
    q[256] = 494;
    r[256] = 4066;
    q[257] = 1936;
    r[257] = -3609;
    q[258] = 245;
    r[258] = 4088;
    q[259] = -1211;
    r[259] = -3912;
    q[260] = -3600;
    r[260] = 1952;
    q[261] = 1632;
    r[261] = 3756;
    q[262] = 2341;
    r[262] = 3360;
    q[263] = 186;
    r[263] = -4091;
    q[264] = 4011;
    r[264] = 829;
    q[265] = -3490;
    r[265] = -2143;
    q[266] = 269;
    r[266] = -4087;
    q[267] = -2939;
    r[267] = 2852;
    q[268] = 1600;
    r[268] = 3770;
    q[269] = -3405;
    r[269] = -2275;
    q[270] = -3134;
    r[270] = -2636;
    q[271] = 2642;
    r[271] = -3129;
    q[272] = 3629;
    r[272] = 1898;
    q[273] = 3413;
    r[273] = 2264;
    q[274] = 2050;
    r[274] = 3545;
    q[275] = 988;
    r[275] = -3975;
    q[276] = -660;
    r[276] = 4042;
    q[277] = 978;
    r[277] = -3977;
    q[278] = 1965;
    r[278] = -3593;
    q[279] = -1513;
    r[279] = -3806;
    q[280] = -4076;
    r[280] = 401;
    q[281] = -4094;
    r[281] = -92;
    q[282] = -1914;
    r[282] = 3621;
    q[283] = 2006;
    r[283] = -3570;
    q[284] = -1550;
    r[284] = -3791;
    q[285] = 3774;
    r[285] = -1591;
    q[286] = -3958;
    r[286] = 1052;
    q[287] = -3576;
    r[287] = 1997;
    q[288] = -382;
    r[288] = 4078;
    q[289] = 1288;
    r[289] = 3888;
    q[290] = -2965;
    r[290] = -2825;
    q[291] = 1608;
    r[291] = 3767;
    q[292] = 3052;
    r[292] = -2731;
    q[293] = -622;
    r[293] = 4048;
    q[294] = -3836;
    r[294] = 1434;
    q[295] = -3542;
    r[295] = 2056;
    q[296] = -2648;
    r[296] = 3124;
    q[297] = -1178;
    r[297] = -3922;
    q[298] = -1109;
    r[298] = 3942;
    q[299] = 3910;
    r[299] = 1217;
    q[300] = 1199;
    r[300] = -3916;
    q[301] = -3386;
    r[301] = 2303;
    q[302] = -3453;
    r[302] = 2202;
    q[303] = -2877;
    r[303] = 2914;
    q[304] = 4095;
    r[304] = -47;
    q[305] = 3635;
    r[305] = 1886;
    q[306] = -2134;
    r[306] = -3495;
    q[307] = 613;
    r[307] = -4049;
    q[308] = -2700;
    r[308] = 3079;
    q[309] = 4091;
    r[309] = -195;
    q[310] = 3989;
    r[310] = -927;
    q[311] = -2385;
    r[311] = 3329;
    q[312] = 4094;
    r[312] = -103;
    q[313] = 1044;
    r[313] = -3960;
    q[314] = -1734;
    r[314] = -3710;
    q[315] = 1646;
    r[315] = 3750;
    q[316] = 575;
    r[316] = 4055;
    q[317] = -2629;
    r[317] = -3140;
    q[318] = 3266;
    r[318] = 2471;
    q[319] = 4091;
    r[319] = -194;
    q[320] = -2154;
    r[320] = 3483;
    q[321] = 659;
    r[321] = 4042;
    q[322] = -1785;
    r[322] = -3686;
    q[323] = -717;
    r[323] = -4032;
    q[324] = 4095;
    r[324] = -37;
    q[325] = -2963;
    r[325] = -2827;
    q[326] = -2645;
    r[326] = -3126;
    q[327] = 2619;
    r[327] = -3148;
    q[328] = 1855;
    r[328] = -3651;
    q[329] = -3726;
    r[329] = 1699;
    q[330] = -3437;
    r[330] = 2227;
    q[331] = 2948;
    r[331] = 2842;
    q[332] = -2125;
    r[332] = 3501;
    q[333] = -1700;
    r[333] = 3726;
    q[334] = 4094;
    r[334] = -101;
    q[335] = 2084;
    r[335] = -3525;
    q[336] = 3225;
    r[336] = -2524;
    q[337] = 2220;
    r[337] = 3442;
    q[338] = 3174;
    r[338] = 2588;
    q[339] = 229;
    r[339] = -4089;
    q[340] = -2381;
    r[340] = -3332;
    q[341] = -3677;
    r[341] = -1803;
    q[342] = -3191;
    r[342] = -2567;
    q[343] = 2465;
    r[343] = 3270;
    q[344] = 2681;
    r[344] = -3096;
    q[345] = 975;
    r[345] = -3978;
    q[346] = 2004;
    r[346] = -3572;
    q[347] = -3442;
    r[347] = -2219;
    q[348] = 3676;
    r[348] = -1805;
    q[349] = -3753;
    r[349] = 1638;
    q[350] = 3544;
    r[350] = 2053;
    q[351] = 397;
    r[351] = -4076;
    q[352] = 2221;
    r[352] = 3440;
    q[353] = -302;
    r[353] = 4084;
    q[354] = 4083;
    r[354] = -323;
    q[355] = -2253;
    r[355] = -3420;
    q[356] = -3038;
    r[356] = 2746;
    q[357] = 2884;
    r[357] = 2908;
    q[358] = 4070;
    r[358] = 454;
    q[359] = -1072;
    r[359] = -3953;
    q[360] = 3831;
    r[360] = 1449;
    q[361] = 3663;
    r[361] = -1831;
    q[362] = -1971;
    r[362] = 3590;
    q[363] = 3226;
    r[363] = -2522;
    q[364] = -145;
    r[364] = -4093;
    q[365] = 1882;
    r[365] = -3637;
    q[366] = 529;
    r[366] = 4061;
    q[367] = 2637;
    r[367] = 3133;
    q[368] = -4077;
    r[368] = 389;
    q[369] = 2156;
    r[369] = -3482;
    q[370] = -3276;
    r[370] = 2458;
    q[371] = -2687;
    r[371] = -3090;
    q[372] = 3469;
    r[372] = -2177;
    q[373] = -4093;
    r[373] = -139;
    q[374] = -850;
    r[374] = 4006;
    q[375] = -625;
    r[375] = 4048;
    q[376] = 1110;
    r[376] = -3942;
    q[377] = -3078;
    r[377] = -2702;
    q[378] = -2719;
    r[378] = 3063;
    q[379] = 742;
    r[379] = 4028;
    q[380] = -3902;
    r[380] = -1245;
    q[381] = 3888;
    r[381] = -1287;
    q[382] = -4081;
    r[382] = 347;
    q[383] = 1070;
    r[383] = 3953;
    q[384] = -996;
    r[384] = -3972;
    q[385] = 4041;
    r[385] = -668;
    q[386] = -2712;
    r[386] = 3069;
    q[387] = -3403;
    r[387] = -2279;
    q[388] = -3320;
    r[388] = -2398;
    q[389] = 3036;
    r[389] = -2749;
    q[390] = 1308;
    r[390] = -3881;
    q[391] = 2256;
    r[391] = 3418;
    q[392] = -1486;
    r[392] = 3816;
    q[393] = -2771;
    r[393] = -3015;
    q[394] = -3883;
    r[394] = -1302;
    q[395] = -3867;
    r[395] = -1349;
    q[396] = 3952;
    r[396] = -1075;
    q[397] = -789;
    r[397] = 4019;
    q[398] = 1458;
    r[398] = 3827;
    q[399] = 3832;
    r[399] = -1446;
    q[400] = -3001;
    r[400] = -2787;
    q[401] = 3463;
    r[401] = 2186;
    q[402] = 3606;
    r[402] = 1942;
    q[403] = 4023;
    r[403] = 764;
    q[404] = 3387;
    r[404] = 2303;
    q[405] = 2648;
    r[405] = -3124;
    q[406] = 1370;
    r[406] = -3860;
    q[407] = -3134;
    r[407] = 2636;
    q[408] = 4051;
    r[408] = -600;
    q[409] = -1977;
    r[409] = -3587;
    q[410] = 3160;
    r[410] = 2605;
    q[411] = 4042;
    r[411] = 659;
    q[412] = 3004;
    r[412] = 2783;
    q[413] = 3370;
    r[413] = 2327;
    q[414] = -419;
    r[414] = -4074;
    q[415] = -1968;
    r[415] = 3591;
    q[416] = -3705;
    r[416] = -1746;
    q[417] = -3331;
    r[417] = -2383;
    q[418] = -3634;
    r[418] = 1888;
    q[419] = -1981;
    r[419] = -3584;
    q[420] = 4069;
    r[420] = -469;
    q[421] = -628;
    r[421] = -4047;
    q[422] = -1900;
    r[422] = 3628;
    q[423] = 1039;
    r[423] = -3961;
    q[424] = 2554;
    r[424] = -3201;
    q[425] = -2955;
    r[425] = 2836;
    q[426] = 2286;
    r[426] = -3398;
    q[427] = -1624;
    r[427] = 3760;
    q[428] = 2213;
    r[428] = 3446;
    q[429] = -3989;
    r[429] = -926;
    q[430] = 192;
    r[430] = -4091;
    q[431] = -723;
    r[431] = 4031;
    q[432] = 2878;
    r[432] = 2913;
    q[433] = -2109;
    r[433] = 3511;
    q[434] = 1463;
    r[434] = -3825;
    q[435] = -741;
    r[435] = -4028;
    q[436] = -1314;
    r[436] = -3879;
    q[437] = 3115;
    r[437] = 2659;
    q[438] = -3160;
    r[438] = -2605;
    q[439] = 1868;
    r[439] = 3644;
    q[440] = -824;
    r[440] = 4012;
    q[441] = 781;
    r[441] = 4020;
    q[442] = -1257;
    r[442] = -3898;
    q[443] = 3331;
    r[443] = -2382;
    q[444] = 1642;
    r[444] = -3752;
    q[445] = 3748;
    r[445] = -1650;
    q[446] = -487;
    r[446] = -4066;
    q[447] = 3085;
    r[447] = -2694;
    q[448] = 4009;
    r[448] = 839;
    q[449] = -2308;
    r[449] = -3383;
    q[450] = 3850;
    r[450] = 1397;
    q[451] = -4078;
    r[451] = -374;
    q[452] = 2989;
    r[452] = -2799;
    q[453] = 3023;
    r[453] = -2762;
    q[454] = 1397;
    r[454] = -3850;
    q[455] = 323;
    r[455] = 4083;
    q[456] = 268;
    r[456] = -4087;
    q[457] = 2414;
    r[457] = 3308;
    q[458] = 3876;
    r[458] = 1322;
    q[459] = -3584;
    r[459] = 1982;
    q[460] = 1603;
    r[460] = 3769;
    q[461] = -1502;
    r[461] = 3810;
    q[462] = 1318;
    r[462] = 3878;
    q[463] = 1554;
    r[463] = -3789;
    q[464] = 2492;
    r[464] = 3250;
    q[465] = -4093;
    r[465] = -154;
    q[466] = 4008;
    r[466] = 842;
    q[467] = -2279;
    r[467] = 3403;
    q[468] = 3013;
    r[468] = 2774;
    q[469] = 2557;
    r[469] = 3199;
    q[470] = 4068;
    r[470] = 475;
    q[471] = 3324;
    r[471] = -2392;
    q[472] = 2653;
    r[472] = -3120;
    q[473] = 796;
    r[473] = 4017;
    q[474] = -1312;
    r[474] = 3880;
    q[475] = 1794;
    r[475] = 3681;
    q[476] = -2347;
    r[476] = -3356;
    q[477] = -4008;
    r[477] = -840;
    q[478] = -3773;
    r[478] = -1592;
    q[479] = 1609;
    r[479] = 3766;
    q[480] = -1564;
    r[480] = -3785;
    q[481] = 3004;
    r[481] = 2784;
    q[482] = 1258;
    r[482] = 3897;
    q[483] = 3729;
    r[483] = 1693;
    q[484] = -4095;
    r[484] = -28;
    q[485] = -4093;
    r[485] = -146;
    q[486] = 1393;
    r[486] = -3851;
    q[487] = 297;
    r[487] = -4085;
    q[488] = 2294;
    r[488] = 3393;
    q[489] = -2562;
    r[489] = 3195;
    q[490] = -1716;
    r[490] = -3718;
    q[491] = 2224;
    r[491] = -3439;
    q[492] = 2032;
    r[492] = 3555;
    q[493] = -2968;
    r[493] = 2822;
    q[494] = 2338;
    r[494] = 3363;
    q[495] = 1584;
    r[495] = -3776;
    q[496] = -3072;
    r[496] = 2708;
    q[497] = -1596;
    r[497] = -3771;
    q[498] = -2256;
    r[498] = -3418;
    q[499] = 4095;
    r[499] = 89;
    q[500] = -1949;
    r[500] = 3602;
    q[501] = 1844;
    r[501] = 3657;
    q[502] = -3375;
    r[502] = 2319;
    q[503] = -1481;
    r[503] = -3818;
    q[504] = 3228;
    r[504] = -2520;
    q[505] = 1116;
    r[505] = 3940;
    q[506] = -2783;
    r[506] = 3004;
    q[507] = 3915;
    r[507] = 1201;
    q[508] = 283;
    r[508] = 4086;
    q[509] = -3732;
    r[509] = 1685;
    q[510] = -433;
    r[510] = -4072;
    q[511] = -3667;
    r[511] = 1823;
    q[512] = 3883;
    r[512] = 1300;
    q[513] = -3742;
    r[513] = 1663;
    q[514] = 4093;
    r[514] = -143;
    q[515] = 3874;
    r[515] = 1328;
    q[516] = -3800;
    r[516] = 1528;
    q[517] = -1257;
    r[517] = 3898;
    q[518] = -1606;
    r[518] = 3767;
    q[519] = 3394;
    r[519] = 2291;
    q[520] = 2255;
    r[520] = 3419;
    q[521] = -4094;
    r[521] = 120;
    q[522] = -3767;
    r[522] = 1606;
    q[523] = 1849;
    r[523] = -3654;
    q[524] = -2883;
    r[524] = 2908;
    q[525] = 3469;
    r[525] = 2176;
    q[526] = 2654;
    r[526] = 3119;
    q[527] = -239;
    r[527] = 4088;
    q[528] = -651;
    r[528] = 4043;
    q[529] = -1140;
    r[529] = 3934;
    q[530] = 328;
    r[530] = -4082;
    q[531] = 3246;
    r[531] = 2497;
    q[532] = 4026;
    r[532] = -753;
    q[533] = -2041;
    r[533] = -3550;
    q[534] = -1154;
    r[534] = 3929;
    q[535] = -2710;
    r[535] = 3070;
    q[536] = -2860;
    r[536] = 2932;
    q[537] = 2097;
    r[537] = 3517;
    q[538] = 3492;
    r[538] = -2140;
    q[539] = 3123;
    r[539] = 2649;
    q[540] = 3360;
    r[540] = 2342;
    q[541] = 2498;
    r[541] = 3245;
    q[542] = 3976;
    r[542] = 982;
    q[543] = -2441;
    r[543] = -3288;
    q[544] = 3601;
    r[544] = 1951;
    q[545] = -4008;
    r[545] = -842;
    q[546] = 1243;
    r[546] = 3902;
    q[547] = 4069;
    r[547] = 466;
    q[548] = -2031;
    r[548] = 3556;
    q[549] = 4077;
    r[549] = 386;
    q[550] = -3112;
    r[550] = -2663;
    q[551] = 4087;
    r[551] = -262;
    q[552] = 4087;
    r[552] = 266;
    q[553] = -3907;
    r[553] = -1228;
    q[554] = -1611;
    r[554] = 3765;
    q[555] = 3066;
    r[555] = -2715;
    q[556] = 2657;
    r[556] = 3117;
    q[557] = 3912;
    r[557] = -1213;
    q[558] = -2531;
    r[558] = -3220;
    q[559] = 3500;
    r[559] = -2127;
    q[560] = -76;
    r[560] = -4095;
    q[561] = 3413;
    r[561] = -2264;
    q[562] = -4071;
    r[562] = -448;
    q[563] = 828;
    r[563] = 4011;
    q[564] = 3664;
    r[564] = 1830;
    q[565] = -1578;
    r[565] = 3779;
    q[566] = 3555;
    r[566] = 2033;
    q[567] = 3868;
    r[567] = -1345;
    q[568] = 4054;
    r[568] = -580;
    q[569] = -4094;
    r[569] = 124;
    q[570] = -3820;
    r[570] = -1477;
    q[571] = -3658;
    r[571] = -1842;
    q[572] = 2595;
    r[572] = 3168;
    q[573] = 3354;
    r[573] = 2350;
    q[574] = -701;
    r[574] = -4035;
    q[575] = -772;
    r[575] = -4022;
    q[576] = 2799;
    r[576] = 2990;
    q[577] = -3632;
    r[577] = 1893;
    q[578] = 310;
    r[578] = 4084;
    q[579] = 3984;
    r[579] = -947;
    q[580] = 3794;
    r[580] = -1542;
    q[581] = -2419;
    r[581] = 3304;
    q[582] = -3916;
    r[582] = 1200;
    q[583] = -3886;
    r[583] = 1292;
    q[584] = -3299;
    r[584] = 2426;
    q[585] = -437;
    r[585] = 4072;
    q[586] = 2053;
    r[586] = -3544;
    q[587] = 3987;
    r[587] = 937;
    q[588] = -789;
    r[588] = -4019;
    q[589] = 4055;
    r[589] = -575;
    q[590] = -3894;
    r[590] = 1270;
    q[591] = 4003;
    r[591] = -864;
    q[592] = -3060;
    r[592] = 2721;
    q[593] = -4009;
    r[593] = 836;
    q[594] = -1655;
    r[594] = -3746;
    q[595] = 3954;
    r[595] = -1067;
    q[596] = -773;
    r[596] = 4022;
    q[597] = -422;
    r[597] = 4074;
    q[598] = -3384;
    r[598] = 2306;
    q[599] = 195;
    r[599] = -4091;
    q[600] = -298;
    r[600] = 4085;
    q[601] = -3988;
    r[601] = 931;
    q[602] = 2014;
    r[602] = -3566;
    q[603] = 3349;
    r[603] = -2357;
    q[604] = 3800;
    r[604] = 1526;
    q[605] = 3858;
    r[605] = 1374;
    q[606] = 2947;
    r[606] = 2844;
    q[607] = -1483;
    r[607] = -3818;
    q[608] = 4056;
    r[608] = -565;
    q[609] = 2612;
    r[609] = -3154;
    q[610] = 2326;
    r[610] = 3371;
    q[611] = -3545;
    r[611] = 2051;
    q[612] = -1001;
    r[612] = -3971;
    q[613] = 3211;
    r[613] = 2541;
    q[614] = -2717;
    r[614] = 3065;
    q[615] = -3159;
    r[615] = -2606;
    q[616] = 2869;
    r[616] = -2922;
    q[617] = -1290;
    r[617] = -3887;
    q[618] = 2479;
    r[618] = 3260;
    q[619] = 3420;
    r[619] = 2252;
    q[620] = 1823;
    r[620] = 3667;
    q[621] = 3368;
    r[621] = 2330;
    q[622] = -3819;
    r[622] = -1480;
    q[623] = 3800;
    r[623] = 1528;
    q[624] = 3773;
    r[624] = 1594;
    q[625] = -189;
    r[625] = -4091;
    q[626] = -4067;
    r[626] = -485;
    q[627] = 2277;
    r[627] = -3404;
    q[628] = -4089;
    r[628] = -233;
    q[629] = -3634;
    r[629] = 1889;
    q[630] = 3292;
    r[630] = 2437;
    q[631] = -530;
    r[631] = 4061;
    q[632] = -3109;
    r[632] = 2666;
    q[633] = -3741;
    r[633] = 1667;
    q[634] = -1903;
    r[634] = -3626;
    q[635] = 3879;
    r[635] = -1315;
    q[636] = 4083;
    r[636] = -315;
    q[637] = -1148;
    r[637] = 3931;
    q[638] = 2630;
    r[638] = 3139;
    q[639] = -4001;
    r[639] = 876;
    q[640] = -2295;
    r[640] = -3392;
    q[641] = 1090;
    r[641] = -3948;
    q[642] = -3024;
    r[642] = 2762;
    q[643] = 2728;
    r[643] = -3054;
    q[644] = -3305;
    r[644] = 2419;
    q[645] = 60;
    r[645] = 4095;
    q[646] = 4048;
    r[646] = -620;
    q[647] = 589;
    r[647] = -4053;
    q[648] = -3867;
    r[648] = 1347;
    q[649] = -2944;
    r[649] = -2847;
    q[650] = -2721;
    r[650] = 3060;
    q[651] = 2928;
    r[651] = 2863;
    q[652] = 801;
    r[652] = 4016;
    q[653] = -3644;
    r[653] = 1870;
    q[654] = -1648;
    r[654] = -3749;
    q[655] = 825;
    r[655] = -4012;
    q[656] = -2036;
    r[656] = -3553;
    q[657] = -1192;
    r[657] = -3918;
    q[658] = 2875;
    r[658] = 2916;
    q[659] = -1831;
    r[659] = -3663;
    q[660] = -2865;
    r[660] = -2926;
    q[661] = -575;
    r[661] = -4055;
    q[662] = -3870;
    r[662] = 1340;
    q[663] = -4080;
    r[663] = -356;
    q[664] = -2176;
    r[664] = -3469;
    q[665] = -2986;
    r[665] = -2803;
    q[666] = 3978;
    r[666] = -972;
    q[667] = 2437;
    r[667] = 3291;
    q[668] = -3528;
    r[668] = 2080;
    q[669] = -3300;
    r[669] = -2425;
    q[670] = 3085;
    r[670] = 2693;
    q[671] = -3700;
    r[671] = -1756;
    q[672] = 3216;
    r[672] = -2535;
    q[673] = 4094;
    r[673] = -91;
    q[674] = 3775;
    r[674] = -1589;
    q[675] = 1097;
    r[675] = -3946;
    q[676] = -152;
    r[676] = -4093;
    q[677] = -3490;
    r[677] = 2142;
    q[678] = 3747;
    r[678] = 1654;
    q[679] = -1490;
    r[679] = -3815;
    q[680] = -3998;
    r[680] = -886;
    q[681] = 3726;
    r[681] = -1700;
    q[682] = -1600;
    r[682] = 3770;
    q[683] = -87;
    r[683] = 4095;
    q[684] = 2538;
    r[684] = -3214;
    q[685] = -4095;
    r[685] = 52;
    q[686] = -3993;
    r[686] = -910;
    q[687] = 4051;
    r[687] = 603;
    q[688] = -1242;
    r[688] = -3902;
    q[689] = 2155;
    r[689] = 3482;
    q[690] = 1270;
    r[690] = 3893;
    q[691] = 1919;
    r[691] = -3618;
    q[692] = -3145;
    r[692] = 2623;
    q[693] = 2475;
    r[693] = 3263;
    q[694] = 2226;
    r[694] = -3437;
    q[695] = -3894;
    r[695] = -1269;
    q[696] = -429;
    r[696] = 4073;
    q[697] = -1346;
    r[697] = 3868;
    q[698] = 1297;
    r[698] = 3885;
    q[699] = 1699;
    r[699] = 3726;
    q[700] = -3375;
    r[700] = 2319;
    q[701] = 1577;
    r[701] = -3779;
    q[702] = -63;
    r[702] = 4095;
    q[703] = 1215;
    r[703] = -3911;
    q[704] = -1492;
    r[704] = 3814;
    q[705] = -1530;
    r[705] = -3799;
    q[706] = 3442;
    r[706] = 2218;
    q[707] = -3867;
    r[707] = -1349;
    q[708] = -3291;
    r[708] = -2437;
    q[709] = -2253;
    r[709] = -3420;
    q[710] = -150;
    r[710] = -4093;
    q[711] = -2686;
    r[711] = -3092;
    q[712] = 3470;
    r[712] = 2175;
    q[713] = -3826;
    r[713] = -1461;
    q[714] = -3148;
    r[714] = 2619;
    q[715] = -3858;
    r[715] = 1375;
    q[716] = -3844;
    r[716] = -1412;
    q[717] = -3652;
    r[717] = 1854;
    q[718] = 4018;
    r[718] = -791;
    q[719] = 179;
    r[719] = -4092;
    q[720] = 3498;
    r[720] = 2129;
    q[721] = -1999;
    r[721] = -3574;
    q[722] = 3531;
    r[722] = 2075;
    q[723] = 4050;
    r[723] = -606;
    q[724] = -1639;
    r[724] = 3753;
    q[725] = -3661;
    r[725] = 1835;
    q[726] = 4039;
    r[726] = 679;
    q[727] = 3561;
    r[727] = 2023;
    q[728] = 528;
    r[728] = 4061;
    q[729] = -634;
    r[729] = -4046;
    q[730] = 364;
    r[730] = -4079;
    q[731] = 2735;
    r[731] = 3048;
    q[732] = 3978;
    r[732] = 973;
    q[733] = -4073;
    r[733] = -427;
    q[734] = -3722;
    r[734] = 1708;
    q[735] = 2356;
    r[735] = -3350;
    q[736] = -1125;
    r[736] = -3938;
    q[737] = 4054;
    r[737] = 580;
    q[738] = 3328;
    r[738] = -2387;
    q[739] = 1439;
    r[739] = -3834;
    q[740] = 1746;
    r[740] = 3705;
    q[741] = 2507;
    r[741] = 3238;
    q[742] = 3839;
    r[742] = -1427;
    q[743] = 488;
    r[743] = -4066;
    q[744] = 1187;
    r[744] = 3920;
    q[745] = 2038;
    r[745] = -3552;
    q[746] = -905;
    r[746] = -3994;
    q[747] = -236;
    r[747] = 4089;
    q[748] = 208;
    r[748] = -4090;
    q[749] = 1660;
    r[749] = 3744;
    q[750] = -4074;
    r[750] = -415;
    q[751] = -2304;
    r[751] = 3385;
    q[752] = -2457;
    r[752] = 3276;
    q[753] = 3302;
    r[753] = 2423;
    q[754] = 1778;
    r[754] = -3689;
    q[755] = 2019;
    r[755] = 3563;
    q[756] = 4037;
    r[756] = 687;
    q[757] = -2365;
    r[757] = 3343;
    q[758] = 5;
    r[758] = -4095;
    q[759] = 160;
    r[759] = -4092;

} /*initRand*/

/**
 * initializes the MEL-2_LINEAR LOOKUP TABLE
 * @param   sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @remarks translated from matlab code to c-code
 * @callgraph
 * @callergraph
 *
 *  input
 *  - bilinTab : base address of bilinTable destination vector
 *  - alpha : warping factor
 *  - size : size of the vectors to be generated
 *  - A,B : base address of array of indexes for lookup table implementation
 *  - D : base address of delta array for lookup table implementation
 *
 *  output
 *  - bilinTab, A, B, D  : initialized vectors
 */
void mel_2_lin_init(sig_innerobj_t *sig_inObj)
{

    /*Declare variables tied to  I/O PARAMS formerly passed by value or reference*/
    picoos_single alpha;
    picoos_int32 *D;
    picoos_int32 size;
    picoos_int16 *A;

    /*Link local variables with sig data object*/

    alpha = sig_inObj->warp_p;
    size = (sig_inObj->hfftsize_p) + 1;
    A = sig_inObj->A_p;
    D = sig_inObj->d_p;
    /*
     fixed point interpolation tables
     scaling factor: 0x20
     corresponding bit shift: 5
     */

    A[0] = 0;
    D[0] = 0;
    A[1] = 2;
    D[1] = 14;
    A[2] = 4;
    D[2] = 29;
    A[3] = 7;
    D[3] = 11;
    A[4] = 9;
    D[4] = 24;
    A[5] = 12;
    D[5] = 5;
    A[6] = 14;
    D[6] = 18;
    A[7] = 16;
    D[7] = 30;
    A[8] = 19;
    D[8] = 9;
    A[9] = 21;
    D[9] = 19;
    A[10] = 23;
    D[10] = 29;
    A[11] = 26;
    D[11] = 5;
    A[12] = 28;
    D[12] = 12;
    A[13] = 30;
    D[13] = 19;
    A[14] = 32;
    D[14] = 24;
    A[15] = 34;
    D[15] = 27;
    A[16] = 36;
    D[16] = 30;
    A[17] = 38;
    D[17] = 31;
    A[18] = 40;
    D[18] = 31;
    A[19] = 42;
    D[19] = 29;
    A[20] = 44;
    D[20] = 26;
    A[21] = 46;
    D[21] = 22;
    A[22] = 48;
    D[22] = 17;
    A[23] = 50;
    D[23] = 10;
    A[24] = 52;
    D[24] = 2;
    A[25] = 53;
    D[25] = 24;
    A[26] = 55;
    D[26] = 13;
    A[27] = 57;
    D[27] = 1;
    A[28] = 58;
    D[28] = 20;
    A[29] = 60;
    D[29] = 5;
    A[30] = 61;
    D[30] = 21;
    A[31] = 63;
    D[31] = 4;
    A[32] = 64;
    D[32] = 18;
    A[33] = 65;
    D[33] = 31;
    A[34] = 67;
    D[34] = 11;
    A[35] = 68;
    D[35] = 21;
    A[36] = 69;
    D[36] = 31;
    A[37] = 71;
    D[37] = 7;
    A[38] = 72;
    D[38] = 14;
    A[39] = 73;
    D[39] = 21;
    A[40] = 74;
    D[40] = 27;
    A[41] = 75;
    D[41] = 31;
    A[42] = 77;
    D[42] = 3;
    A[43] = 78;
    D[43] = 6;
    A[44] = 79;
    D[44] = 8;
    A[45] = 80;
    D[45] = 10;
    A[46] = 81;
    D[46] = 10;
    A[47] = 82;
    D[47] = 10;
    A[48] = 83;
    D[48] = 9;
    A[49] = 84;
    D[49] = 8;
    A[50] = 85;
    D[50] = 6;
    A[51] = 86;
    D[51] = 3;
    A[52] = 86;
    D[52] = 31;
    A[53] = 87;
    D[53] = 27;
    A[54] = 88;
    D[54] = 23;
    A[55] = 89;
    D[55] = 18;
    A[56] = 90;
    D[56] = 12;
    A[57] = 91;
    D[57] = 6;
    A[58] = 91;
    D[58] = 31;
    A[59] = 92;
    D[59] = 24;
    A[60] = 93;
    D[60] = 16;
    A[61] = 94;
    D[61] = 8;
    A[62] = 94;
    D[62] = 31;
    A[63] = 95;
    D[63] = 22;
    A[64] = 96;
    D[64] = 13;
    A[65] = 97;
    D[65] = 3;
    A[66] = 97;
    D[66] = 25;
    A[67] = 98;
    D[67] = 14;
    A[68] = 99;
    D[68] = 3;
    A[69] = 99;
    D[69] = 24;
    A[70] = 100;
    D[70] = 13;
    A[71] = 101;
    D[71] = 1;
    A[72] = 101;
    D[72] = 21;
    A[73] = 102;
    D[73] = 8;
    A[74] = 102;
    D[74] = 27;
    A[75] = 103;
    D[75] = 14;
    A[76] = 104;
    D[76] = 1;
    A[77] = 104;
    D[77] = 19;
    A[78] = 105;
    D[78] = 6;
    A[79] = 105;
    D[79] = 24;
    A[80] = 106;
    D[80] = 9;
    A[81] = 106;
    D[81] = 27;
    A[82] = 107;
    D[82] = 12;
    A[83] = 107;
    D[83] = 29;
    A[84] = 108;
    D[84] = 14;
    A[85] = 108;
    D[85] = 31;
    A[86] = 109;
    D[86] = 15;
    A[87] = 109;
    D[87] = 31;
    A[88] = 110;
    D[88] = 16;
    A[89] = 110;
    D[89] = 31;
    A[90] = 111;
    D[90] = 15;
    A[91] = 111;
    D[91] = 31;
    A[92] = 112;
    D[92] = 14;
    A[93] = 112;
    D[93] = 30;
    A[94] = 113;
    D[94] = 13;
    A[95] = 113;
    D[95] = 28;
    A[96] = 114;
    D[96] = 11;
    A[97] = 114;
    D[97] = 26;
    A[98] = 115;
    D[98] = 9;
    A[99] = 115;
    D[99] = 23;
    A[100] = 116;
    D[100] = 6;
    A[101] = 116;
    D[101] = 20;
    A[102] = 117;
    D[102] = 2;
    A[103] = 117;
    D[103] = 16;
    A[104] = 117;
    D[104] = 31;
    A[105] = 118;
    D[105] = 13;
    A[106] = 118;
    D[106] = 27;
    A[107] = 119;
    D[107] = 8;
    A[108] = 119;
    D[108] = 22;
    A[109] = 120;
    D[109] = 4;
    A[110] = 120;
    D[110] = 17;
    A[111] = 120;
    D[111] = 31;
    A[112] = 121;
    D[112] = 13;
    A[113] = 121;
    D[113] = 26;
    A[114] = 122;
    D[114] = 8;
    A[115] = 122;
    D[115] = 21;
    A[116] = 123;
    D[116] = 2;
    A[117] = 123;
    D[117] = 15;
    A[118] = 123;
    D[118] = 29;
    A[119] = 124;
    D[119] = 10;
    A[120] = 124;
    D[120] = 23;
    A[121] = 125;
    D[121] = 4;
    A[122] = 125;
    D[122] = 17;
    A[123] = 125;
    D[123] = 31;
    A[124] = 126;
    D[124] = 12;
    A[125] = 126;
    D[125] = 25;
    A[126] = 127;
    D[126] = 6;
    A[127] = 127;
    D[127] = 19;
    A[128] = 128;
    D[128] = 0;

}/*mel_2_lin_init*/

/**
 * function to be documented
 * @param    ang : ??
 * @param    table : ??
 * @param    cs : ??
 * @param    sn : ??
 * @return  void
 * @callgraph
 * @callergraph
 */
static void get_trig(picoos_int32 ang, picoos_int32 *table, picoos_int32 *cs,
        picoos_int32 *sn)
{
    picoos_int32 i, j, k;

    i = k = ang >> PICODSP_PI_SHIFT; /*  * PICODSP_COS_TABLE_LEN2/PICODSP_FIX_SCALE2 */
    if (i < 0)
        i = -i;
    j = 1;
    i &= (PICODSP_COS_TABLE_LEN4 - 1);
    if (i > PICODSP_COS_TABLE_LEN2)
        i = PICODSP_COS_TABLE_LEN4 - i;
    if (i > PICODSP_COS_TABLE_LEN) {
        j = -1;
        i = PICODSP_COS_TABLE_LEN2 - i;
    }
    if (j == 1)
        *cs = table[i];
    else
        *cs = -table[i];

    i = k - PICODSP_COS_TABLE_LEN;
    if (i < 0)
        i = -i;
    j = 1;
    i &= (PICODSP_COS_TABLE_LEN4 - 1);
    if (i > PICODSP_COS_TABLE_LEN2)
        i = PICODSP_COS_TABLE_LEN4 - i;
    if (i > PICODSP_COS_TABLE_LEN) {
        j = -1;
        i = PICODSP_COS_TABLE_LEN2 - i;
    }
    if (j == 1)
        *sn = table[i];
    else
        *sn = -table[i];
}/*get_trig*/

/**
 * function to be documented
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @return  void
 * @callgraph
 * @callergraph
 */
void save_transition_frame(sig_innerobj_t *sig_inObj)
{
    picoos_int32 *tmp, *tmp2; /*for loop unrolling*/

    if (sig_inObj->voiced_p != sig_inObj->prevVoiced_p) {
        sig_inObj->VoicTrans = sig_inObj->prevVoiced_p; /*remember last voicing transition*/
        tmp = sig_inObj->ImpResp_p;
        tmp2 = sig_inObj->imp_p;
        FAST_DEVICE(PICODSP_FFTSIZE,*(tmp++)=*(tmp2++););
        if (sig_inObj->voiced_p == 1)
            sig_inObj->nV = 0;
        else
            sig_inObj->nU = 0; /*to avoid problems in case of very short voiced or unvoiced parts (less than 4 frames long)*/
    }
}/*save_transition_frame*/

/**
 * calculates an unweighted excitation window
 * @param    sig_inObj : sig PU internal object of the sub-object
 * @param    nextPeak : position of next peak (excitation position)
 * @return  PICO_OK
 * @callgraph
 * @callergraph
 * input
 * - hop : hop size in samples
 * - winlen : excitation window length
 * - E : energy
 * - F0 : pitch
 * - nextPeak : state that remembers next excitation index
 * - Fs - sampling frequency
 * output
 * - LocV, LocU : (MAX_EN size) location of excitation points
 * - EnV,  EnU    : (MAX_EN size) RMS values of excitation (scaled)
 * - nV, nU :    (integers) number of excitation points
 * - nextPeak    new position of lastPeak to calculate next frame
 */
static void get_simple_excitation(sig_innerobj_t *sig_inObj,
        picoos_int16 *nextPeak)
{
    /*Define local variables*/
    picoos_int16 nI, nJ, k;
    /* picoos_single    InvSqrt3=(picoos_single)2/(picoos_single)sqrt(3.0); *//*constant*/
    picoos_int32 Ti, sqrtTi;
    picoos_int16 hop, winlen, Fs;
    picoos_single E, F0;
    picoos_int16 voiced;
    picoos_single fact; /*normalization factor*/
    picoos_single rounding = 0.5f;

    /*Link local variables to sig object*/
    hop = sig_inObj->hop_p;
    winlen = sig_inObj->m2_p;
    Fs = sig_inObj->Fs_p;
    E = sig_inObj->E_p;
    F0 = sig_inObj->F0_p;
    voiced = sig_inObj->voiced_p;

    E = (E > 5) ? 9 : (E > 1) ? 2 * E - 1 : E;


    /* shift previous excitation window by hop samples*/
    for (nI = 0; nI < sig_inObj->nV; nI++) {
        sig_inObj->LocV[nI] = sig_inObj->LocV[nI] - hop;
    }
    for (nI = 0; nI < sig_inObj->nU; nI++) {
        sig_inObj->LocU[nI] = sig_inObj->LocU[nI] - hop;
    }

    /*get rid of the voiced points that fall out of the interval*/
    nI = 0;
    while ((sig_inObj->LocV[nI] < 0) && nI < sig_inObj->nV)
        nI++;

    for (nJ = nI; nJ < sig_inObj->nV; nJ++) {
        sig_inObj->LocV[nJ - nI] = sig_inObj->LocV[nJ];
        sig_inObj->EnV[nJ - nI] = sig_inObj->EnV[nJ];
    }
    sig_inObj->nV -= nI;
    /*get rid of the unvoiced points that fall out of the interval */
    nI = 0;
    while ((sig_inObj->LocU[nI] < 0) && nI < sig_inObj->nU)
        nI++;

    for (nJ = nI; nJ < sig_inObj->nU; nJ++) {
        sig_inObj->LocU[nJ - nI] = sig_inObj->LocU[nJ];
        sig_inObj->EnU[nJ - nI] = sig_inObj->EnU[nJ];
    }
    sig_inObj->nU -= nI;

    *nextPeak -= hop;
    k = *nextPeak;

    fact = 3;
    if (voiced == 0) { /*Unvoiced*/

        Ti = (picoos_int32) (rounding + (picoos_single) Fs
                / (picoos_single) sig_inObj->Fuv_p); /* round Period*/
        sqrtTi = (picoos_int32) (E * sqrt((double) Fs
                / (hop * sig_inObj->Fuv_p)) * fact * PICODSP_GETEXC_K1);
        while (k < winlen) {
            if (k < winlen) {
                sig_inObj->LocU[sig_inObj->nU] = k;
                sig_inObj->EnU[sig_inObj->nU] = sqrtTi;
                sig_inObj->nU++;
                k += (picoos_int16) Ti;
            }
        }
    } else { /*Voiced*/
        Ti
                = (picoos_int32) (rounding + (picoos_single) Fs
                        / (picoos_single) F0); /*Period*/
        sqrtTi = (picoos_int32) (E
                * sqrt((double) Fs / (hop * sig_inObj->F0_p)) * fact
                * PICODSP_GETEXC_K1);
        while (k < winlen) {
            sig_inObj->LocV[sig_inObj->nV] = k;
            sig_inObj->EnV[sig_inObj->nV] = sqrtTi;
            sig_inObj->nV++;
            k += (picoos_int16) Ti;
        }
    }
    *nextPeak = k;

}/*get_simple_excitation*/

#ifdef __cplusplus
}
#endif

/* end picosig2.c */
