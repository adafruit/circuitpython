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
 * @file picofftsg.c
 *
 * FFT/DCT related data types, constants and functions in Pico
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
*/

#include "picoos.h"
#include "picofftsg.h"
#include "picodbg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup picofft
 * ---------------------------------------------------\n
 * <b> Fast Fourier/Cosine/Sine Transform </b>\n
 * Adapted from http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html (Copyright Takuya OOURA, 1996-2001)\n
 * ---------------------------------------------------\n

  Overall features
  - dimension   :one
  - data length :power of 2
  - decimation  :frequency
  - radix       :split-radix
  - data        :inplace
  - table       :not use

  functions
  - cdft: Complex Discrete Fourier Transform
  - rdft: Real Discrete Fourier Transform
  - ddct: Discrete Cosine Transform
  - ddst: Discrete Sine Transform
  - dfct: Cosine Transform of RDFT (Real Symmetric DFT)
  - dfst: Sine Transform of RDFT (Real Anti-symmetric DFT)

  function prototypes
  - void cdft(picoos_int32, picoos_int32, PICOFFTSG_FFTTYPE *);
  - void rdft(picoos_int32, picoos_int32, PICOFFTSG_FFTTYPE *);
  - void ddct(picoos_int32, picoos_int32, PICOFFTSG_FFTTYPE *);
  - void ddst(picoos_int32, picoos_int32, PICOFFTSG_FFTTYPE *);
  - void dfct(picoos_int32, PICOFFTSG_FFTTYPE *);
  - void dfst(picoos_int32, PICOFFTSG_FFTTYPE *);

  <b>Complex DFT (Discrete Fourier Transform)</b>

  [definition]
  - <case1>
      - X[k] = sum_j=0^n-1 x[j]*exp(2*pi*i*j*k/n), 0<=k<n
  - <case2>
      - X[k] = sum_j=0^n-1 x[j]*exp(-2*pi*i*j*k/n), 0<=k<n
  - (notes: sum_j=0^n-1 is a summation from j=0 to n-1)

  [usage]
  - <case1>
      - cdft(2*n, 1, a);
  - <case2>
      - cdft(2*n, -1, a);

  [parameters]
  - 2*n            :data length (picoos_int32)
  - n >= 1, n = power of 2
  - a[0...2*n-1]   :input/output data (PICOFFTSG_FFTTYPE *)
  - input data
      - a[2*j] = Re(x[j]),
      - a[2*j+1] = Im(x[j]), 0<=j<n
  - output data
      - a[2*k] = Re(X[k]),
      - a[2*k+1] = Im(X[k]), 0<=k<n

  [remark]
  - Inverse of cdft(2*n, -1, a); is
      -cdft(2*n, 1, a);
          - for (j = 0; j <= 2 * n - 1; j++) {
              - a[j] *= 1.0 / n;
          - }


  <b> Real DFT / Inverse of Real DFT </b>

  [definition]
  - <case1> RDFT
      - R[k] = sum_j=0^n-1 a[j]*cos(2*pi*j*k/n), 0<=k<=n/2
      - I[k] = sum_j=0^n-1 a[j]*sin(2*pi*j*k/n), 0<k<n/2
  -    <case2> IRDFT (excluding scale)
      - a[k] = (R[0] + R[n/2]*cos(pi*k))/2 +
      - sum_j=1^n/2-1 R[j]*cos(2*pi*j*k/n) +
      - sum_j=1^n/2-1 I[j]*sin(2*pi*j*k/n), 0<=k<n

  [usage]
  - <case1>
      - rdft(n, 1, a);
  -    <case2>
      - rdft(n, -1, a);

  [parameters]
  - n              :data length (picoos_int32)
  - n >= 2, n = power of 2
  - a[0...n-1]     :input/output data (PICOFFTSG_FFTTYPE *)
  - <case1>
      - output data
          - a[2*k] = R[k], 0<=k<n/2
          - a[2*k+1] = I[k], 0<k<n/2
          - a[1] = R[n/2]
  - <case2>
      - input data
          - a[2*j] = R[j], 0<=j<n/2
          - a[2*j+1] = I[j], 0<j<n/2
          - a[1] = R[n/2]

  [remark]
  - Inverse of rdft(n, 1, a); is
  - rdft(n, -1, a);
      - for (j = 0; j <= n - 1; j++) {
          - a[j] *= 2.0 / n;
     -}


  <b> DCT (Discrete Cosine Transform) / Inverse of DCT</b>

  [definition]
  - <case1> IDCT (excluding scale)
      - C[k] = sum_j=0^n-1 a[j]*cos(pi*j*(k+1/2)/n), 0<=k<n
  - <case2> DCT
      - C[k] = sum_j=0^n-1 a[j]*cos(pi*(j+1/2)*k/n), 0<=k<n

  [usage]
  - <case1>
    - ddct(n, 1, a);
  - <case2>
    - ddct(n, -1, a);

  [parameters]
  - n              :data length (picoos_int32)
  - n >= 2, n = power of 2
  - a[0...n-1]     :input/output data (PICOFFTSG_FFTTYPE *)
  - output data
      - a[k] = C[k], 0<=k<n

  [remark]
  - Inverse of ddct(n, -1, a); is
  - a[0] *= 0.5;
  - ddct(n, 1, a);
  - for (j = 0; j <= n - 1; j++) {
      - a[j] *= 2.0 / n;
  - }

  <b> DST (Discrete Sine Transform) / Inverse of DST</b>

  [definition]
  - <case1> IDST (excluding scale)
      - S[k] = sum_j=1^n A[j]*sin(pi*j*(k+1/2)/n), 0<=k<n
  - <case2> DST
      - S[k] = sum_j=0^n-1 a[j]*sin(pi*(j+1/2)*k/n), 0<k<=n

  [usage]
  - <case1>
      - ddst(n, 1, a);
  - <case2>
      - ddst(n, -1, a);

  [parameters]
  - n              :data length (picoos_int32)
  - n >= 2, n = power of 2
  - a[0...n-1]     :input/output data (PICOFFTSG_FFTTYPE *)
  - <case1>
      - input data
          - a[j] = A[j], 0<j<n
          - a[0] = A[n]
      - output data
          - a[k] = S[k], 0<=k<n
  - <case2>
    - output data
        - a[k] = S[k], 0<k<n
        - a[0] = S[n]

  [remark]
  - Inverse of ddst(n, -1, a); is
  - a[0] *= 0.5;
  - ddst(n, 1, a);
  - for (j = 0; j <= n - 1; j++) {
      - a[j] *= 2.0 / n;
  - }

  <b> Cosine Transform of RDFT (Real Symmetric DFT)</b>

  [definition]
  - C[k] = sum_j=0^n a[j]*cos(pi*j*k/n), 0<=k<=n

  [usage]
  - dfct(n, a);

  [parameters]
  - n              :data length - 1 (picoos_int32)
  - n >= 2, n = power of 2
  - a[0...n]       :input/output data (PICOFFTSG_FFTTYPE *)

  - output data
    - a[k] = C[k], 0<=k<=n

  [remark]
  - Inverse of a[0] *= 0.5; a[n] *= 0.5; dfct(n, a); is
  - a[0] *= 0.5;
  - a[n] *= 0.5;
  - dfct(n, a);
  - for (j = 0; j <= n; j++) {
      - a[j] *= 2.0 / n;
  - }

  <b> Sine Transform of RDFT (Real Anti-symmetric DFT)</b>

  [definition]
  - S[k] = sum_j=1^n-1 a[j]*sin(pi*j*k/n), 0<k<n

  [usage]
  - dfst(n, a);

  [parameters]
  - n              :data length + 1 (picoos_int32)
  - n >= 2, n = power of 2
  - a[0...n-1]     :input/output data (PICOFFTSG_FFTTYPE *)
  - output data
      - a[k] = S[k], 0<k<n
      - (a[0] is used for work area)

  [remark]
  - Inverse of dfst(n, a); is
  - dfst(n, a);
  - for (j = 1; j <= n - 1; j++) {
    - a[j] *= 2.0 / n;
  - }

*/

/* fixed point multiplier for weights */
#define PICODSP_WGT_SHIFT  (0x20000000)  /* 2^29 */
#define PICOFFTSG_WGT_SHIFT2 (0x10000000)  /* PICODSP_WGT_SHIFT/2 */
#define PICOFFTSG_WGT_N_SHIFT (29)  /* 2^29 */
/* fixed point known constants */
#ifndef WR5000  /* cos(M_PI_2*0.5000) */
#define WR5000      (PICOFFTSG_FFTTYPE)(0.707106781186547524400844362104849039284835937688*PICODSP_WGT_SHIFT)
#ifndef WR2500  /* cos(M_PI_2*0.2500) */
#define WR2500      (PICOFFTSG_FFTTYPE)(0.923879532511286756128183189396788286822416625863*PICODSP_WGT_SHIFT)
#endif
#ifndef WI2500  /* sin(M_PI_2*0.2500) */
#define WI2500      (PICOFFTSG_FFTTYPE)(0.382683432365089771728459984030398866761344562485*PICODSP_WGT_SHIFT)
#endif
#ifndef WR1250  /* cos(M_PI_2*0.1250) */
#define WR1250      (PICOFFTSG_FFTTYPE)(0.980785280403230449126182236134239036973933730893*PICODSP_WGT_SHIFT)
#endif
#ifndef WI1250  /* sin(M_PI_2*0.1250) */
#define WI1250      (PICOFFTSG_FFTTYPE)(0.195090322016128267848284868477022240927691617751*PICODSP_WGT_SHIFT)
#endif
#ifndef WR3750  /* cos(M_PI_2*0.3750) */
#define WR3750      (PICOFFTSG_FFTTYPE)(0.831469612302545237078788377617905756738560811987*PICODSP_WGT_SHIFT)
#endif
#ifndef WI3750  /* sin(M_PI_2*0.3750) */
#define WI3750      (PICOFFTSG_FFTTYPE)(0.555570233019602224742830813948532874374937190754*PICODSP_WGT_SHIFT)
#endif

#else

 /*#ifndef M_PI_2
   #define M_PI_2      (PICOFFTSG_FFTTYPE)1.570796326794896619231321691639751442098584699687
   #endif*/
#ifndef WR5000  /* cos(M_PI_2*0.5000) */
#define WR5000      (PICOFFTSG_FFTTYPE)0.707106781186547524400844362104849039284835937688
#endif
#ifndef WR2500  /* cos(M_PI_2*0.2500) */
#define WR2500      (PICOFFTSG_FFTTYPE)0.923879532511286756128183189396788286822416625863
#endif
#ifndef WI2500  /* sin(M_PI_2*0.2500) */
#define WI2500      (PICOFFTSG_FFTTYPE)0.382683432365089771728459984030398866761344562485
#endif
#ifndef WR1250  /* cos(M_PI_2*0.1250) */
#define WR1250      (PICOFFTSG_FFTTYPE)0.980785280403230449126182236134239036973933730893
#endif
#ifndef WI1250  /* sin(M_PI_2*0.1250) */
#define WI1250      (PICOFFTSG_FFTTYPE)0.195090322016128267848284868477022240927691617751
#endif
#ifndef WR3750  /* cos(M_PI_2*0.3750) */
#define WR3750      (PICOFFTSG_FFTTYPE)0.831469612302545237078788377617905756738560811987
#endif
#ifndef WI3750  /* sin(M_PI_2*0.3750) */
#define WI3750      (PICOFFTSG_FFTTYPE)0.555570233019602224742830813948532874374937190754
#endif

#endif

#ifndef CDFT_LOOP_DIV  /* control of the CDFT's speed & tolerance */
#define CDFT_LOOP_DIV 32
#define CDFT_LOOP_DIV_4 128
#endif

#ifndef RDFT_LOOP_DIV  /* control of the RDFT's speed & tolerance */
#define RDFT_LOOP_DIV 64
#define RDFT_LOOP_DIV4 256
#define RDFT_LOOP_DIV_4 256
#endif

#ifndef DCST_LOOP_DIV  /* control of the DCT,DST's speed & tolerance */
#define DCST_LOOP_DIV 64
#define DCST_LOOP_DIV2 128
#endif


#define POW1 (0x1)
#define POW2 (0x2)
#define POW3 (0x4)
#define POW4 (0x8)
#define POW5 (0x10)
#define POW6 (0x20)
#define POW7 (0x40)
#define POW8 (0x80)
#define POW9 (0x100)
#define POW10 (0x200)
#define POW11 (0x400)
#define POW12 (0x800)
#define POW13 (0x1000)
#define POW14 (0x2000)
#define POW15 (0x4000)
#define POW16 (0x8000)
#define POW17 (0x10000)
#define POW18 (0x20000)
#define POW19 (0x40000)
#define POW20 (0x80000)
#define POW21 (0x100000)
#define POW22 (0x200000)
#define POW23 (0x400000)
#define POW24 (0x800000)
#define POW25 (0x1000000)
#define POW26 (0x2000000)
#define POW27 (0x4000000)
#define POW28 (0x8000000)
#define POW29 (0x10000000)
#define POW30 (0x20000000)
#define POW31 (0x40000000)
/**************
 * useful macros
 ************** */
#define picofftsg_highestBitPos(x) (x==0?0:(x>=POW17?(x>=POW25?(x>=POW29?(x>=POW31?31:(x>=POW30?30:29)):(x>=POW27?(x>=POW28?28:27):(x>=POW26?26:25))):(x>=POW21?(x>=POW23?(x>=POW24?24:23):(x>=POW22?22:21)):(x>=POW19?(x>=POW20?20:19):(x>=POW18?18:17)))):(x>=POW9?(x>=POW13?(x>=POW15?(x>=POW16?16:15):(x>=POW14?14:13)):(x>=POW11?(x>=POW12?12:11):(x>=POW10?10:9))):(x>=POW5?(x>=POW7?(x>=POW8?8:7):(x>=POW6?6:5)):(x>=POW3?(x>=POW4?4:3):(x>=POW2?2:1))))))
#define picofftsg_highestBit(x) (x==0?0:(x<0?(zz=-x,(zz>=POW17?(zz>=POW25?(zz>=POW29?(zz>=POW31?31:(zz>=POW30?30:29)):(zz>=POW27?(zz>=POW28?28:27):(zz>=POW26?26:25))):(zz>=POW21?(zz>=POW23?(zz>=POW24?24:23):(zz>=POW22?22:21)):(zz>=POW19?(zz>=POW20?20:19):(zz>=POW18?18:17)))):(zz>=POW9?(zz>=POW13?(zz>=POW15?(zz>=POW16?16:15):(zz>=POW14?14:13)):(zz>=POW11?(zz>=POW12?12:11):(zz>=POW10?10:9))):(zz>=POW5?(zz>=POW7?(zz>=POW8?8:7):(zz>=POW6?6:5)):(zz>=POW3?(zz>=POW4?4:3):(zz>=POW2?2:1)))))):(x>=POW17?(x>=POW25?(x>=POW29?(x>=POW31?31:(x>=POW30?30:29)):(x>=POW27?(x>=POW28?28:27):(x>=POW26?26:25))):(x>=POW21?(x>=POW23?(x>=POW24?24:23):(x>=POW22?22:21)):(x>=POW19?(x>=POW20?20:19):(x>=POW18?18:17)))):(x>=POW9?(x>=POW13?(x>=POW15?(x>=POW16?16:15):(x>=POW14?14:13)):(x>=POW11?(x>=POW12?12:11):(x>=POW10?10:9))):(x>=POW5?(x>=POW7?(x>=POW8?8:7):(x>=POW6?6:5)):(x>=POW3?(x>=POW4?4:3):(x>=POW2?2:1)))))))
#define Mult_W_W picofftsg_mult_w_w


/* ***********************************************************************************************/
/* forward declarations */
/* ***********************************************************************************************/
static PICOFFTSG_FFTTYPE picofftsg_mult_w_w(PICOFFTSG_FFTTYPE x1, PICOFFTSG_FFTTYPE y1);
static PICOFFTSG_FFTTYPE picofftsg_mult_w_a(PICOFFTSG_FFTTYPE x1, PICOFFTSG_FFTTYPE y1);


static void cftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void rftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void rftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);

static void cftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void rftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void rftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void dctsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void dctsub4(picoos_int32 n, PICOFFTSG_FFTTYPE *a);

static void ddct(picoos_int32 n, picoos_int32 isgn, PICOFFTSG_FFTTYPE *a);
static void bitrv1(picoos_int32 n, PICOFFTSG_FFTTYPE *a);

static void bitrv2(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void bitrv216(PICOFFTSG_FFTTYPE *a);
static void bitrv208(PICOFFTSG_FFTTYPE *a);
static void cftmdl1(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftrec4(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftleaf(picoos_int32 n, picoos_int32 isplt, PICOFFTSG_FFTTYPE *a);
static void cftfx41(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftf161(PICOFFTSG_FFTTYPE *a);
static void cftf081(PICOFFTSG_FFTTYPE *a);
static void cftf040(PICOFFTSG_FFTTYPE *a);
static void cftx020(PICOFFTSG_FFTTYPE *a);

void bitrv2conj(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
void bitrv216neg(PICOFFTSG_FFTTYPE *a);
void bitrv208neg(PICOFFTSG_FFTTYPE *a);
void cftb1st(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
void cftrec4(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
void cftleaf(picoos_int32 n, picoos_int32 isplt, PICOFFTSG_FFTTYPE *a);
void cftfx41(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
void cftf161(PICOFFTSG_FFTTYPE *a);
void cftf081(PICOFFTSG_FFTTYPE *a);
void cftb040(PICOFFTSG_FFTTYPE *a);
void cftx020(PICOFFTSG_FFTTYPE *a);

static picoos_int32 cfttree(picoos_int32 n, picoos_int32 j, picoos_int32 k, PICOFFTSG_FFTTYPE *a);
static void cftleaf(picoos_int32 n, picoos_int32 isplt, PICOFFTSG_FFTTYPE *a);
static void cftmdl1(picoos_int32 n, PICOFFTSG_FFTTYPE *a);

static void cftmdl1(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftmdl2(picoos_int32 n, PICOFFTSG_FFTTYPE *a);

static void cftmdl1(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftmdl2(picoos_int32 n, PICOFFTSG_FFTTYPE *a);
static void cftf161(PICOFFTSG_FFTTYPE *a);
static void cftf162(PICOFFTSG_FFTTYPE *a);
static void cftf081(PICOFFTSG_FFTTYPE *a);
static void cftf082(PICOFFTSG_FFTTYPE *a);

static void cftf161(PICOFFTSG_FFTTYPE *a);
static void cftf162(PICOFFTSG_FFTTYPE *a);
static void cftf081(PICOFFTSG_FFTTYPE *a);
static void cftf082(PICOFFTSG_FFTTYPE *a);

/* ***********************************************************************************************/
/* Exported functions */
/* ***********************************************************************************************/
void rdft(picoos_int32 n, picoos_int32 isgn, PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE xi;

    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a);
            rftfsub(n, a);
        } else if (n == 4) {
            cftfsub(n, a);
        }
        xi = a[0] - a[1];
        a[0] += a[1];
        a[1] = xi;
    } else {
        a[1] =  (a[0] - a[1]) / 2;
        a[0] -= a[1];
        if (n > 4) {
            rftbsub(n, a);
            cftbsub(n, a);
        } else if (n == 4) {
            cftbsub(n, a);
        }
    }

}


picoos_single norm_result(picoos_int32 m2, PICOFFTSG_FFTTYPE *tmpX, PICOFFTSG_FFTTYPE *norm_window)
{
    picoos_int16 nI;
    PICOFFTSG_FFTTYPE a,b, E;

    E = (picoos_int32)0;
    for (nI=0; nI<m2; nI++) {
        a = (norm_window[nI]>>18) * ((tmpX[nI]>0) ? tmpX[nI]>>11 : -((-tmpX[nI])>>11));
        tmpX[nI] = a;
        b = (a>=0?a:-a)  >> 18;
        E += (b*b);
    }

    if (E>0) {
        return (picoos_single)sqrt((double)E/16.0)/m2;
    }
    else {
        return 0.0;
    }
}

void ddct(picoos_int32 n, picoos_int32 isgn, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 j;
    PICOFFTSG_FFTTYPE xr;

    if (isgn < 0) {
        xr = a[n - 1];
        for (j = n - 2; j >= 2; j -= 2) {
            a[j + 1] = a[j] - a[j - 1];
            a[j] += a[j - 1];
        }
        a[1] = a[0] - xr;
        a[0] += xr;
        if (n > 4) {
            rftbsub(n, a);
            cftbsub(n, a);
        } else if (n == 4) {
            cftbsub(n, a);
        }
    }
    if (n > 4) {
        dctsub(n, a);
    } else {
        dctsub4(n, a);
    }
    if (isgn >= 0) {
        if (n > 4) {
            cftfsub(n, a);
            rftfsub(n, a);
        } else if (n == 4) {
            cftfsub(n, a);
        }


        xr = a[0] - a[1];
        a[0] += a[1];
        for (j = 2; j < n; j += 2) {
            a[j - 1] = a[j] - a[j + 1];
            a[j] += a[j + 1];
        }
        a[n - 1] = xr;
    }
}

void dfct_nmf(picoos_int32 n, picoos_int32 *a)
{
    picoos_int32 j, k, m, mh;
    PICOFFTSG_FFTTYPE xr, xi, yr, yi, an;
    PICOFFTSG_FFTTYPE *aj, *ak, *amj, *amk;

    m = n >> 1;
    for (j = 0; j < m; j++) {
        k = n - j;
        xr = a[j] + a[k];
        a[j] -= a[k];
        a[k] = xr;
    }
    an = a[n];
    while (m >= 2) {
        ddct(m, 1, a);
        if (m > 2) {
            bitrv1(m, a);
        }
        mh = m >> 1;
        xi = a[m];
        a[m] = a[0];
        a[0] = an - xi;
        an += xi;
        k = m-1;
        aj = a + 1; ak = a + k; amj = aj + m; amk = ak + m;
        for (j = 1; j < mh; j++, aj++, ak--, amj++, amk--) {
            xr = *amk;
            xi = *amj;
            yr = *aj;
            yi = *ak;
            *amj = yr;
            *amk = yi;
            *aj = xr - xi;
            *ak = xr + xi;
        }
        xr = *aj;
        *aj = *amj;
        *amj = xr;

        m = mh;
    }

    xi = a[1];
    a[1] = a[0];
    a[0] = an + xi;
    a[n] = an - xi;
    if (n > 2) {
        bitrv1(n, a);
    }

}

/* ***********************************************************************************************/
/* internal routines */
/* ***********************************************************************************************/
/*
  mult two numbers which are guaranteed to be in the range -1..1
  shift right as little as possible before mult, and the rest after the mult
  Also, shift bigger number more - lose less accuracy
 */
static PICOFFTSG_FFTTYPE picofftsg_mult_w_w(PICOFFTSG_FFTTYPE x1, PICOFFTSG_FFTTYPE y1)
{
    PICOFFTSG_FFTTYPE x, y;
    x = x1>=0 ? x1>>15 : -((-x1)>>15);
    y = y1>=0 ? y1>>14 : -((-y1)>>14);
    return  x * y;
}

static PICOFFTSG_FFTTYPE picofftsg_mult_w_a(PICOFFTSG_FFTTYPE x1, PICOFFTSG_FFTTYPE y1)
{
    PICOFFTSG_FFTTYPE x, y;


    x = x1>=0 ? x1>>15 : -((-x1)>>15);
    y = y1>=0 ? y1>>14 : -((-y1)>>14);
    return x * y;
}

static void cftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{

    if (n > 8) {
        if (n > 32) {
            cftmdl1(n, a);
                if (n > 512) {
                    cftrec4(n, a);
                } else if (n > 128) {
                    cftleaf(n, 1, a);
                } else {
                    cftfx41(n, a);
                }
            bitrv2(n, a);
        } else if (n == 32) {
            cftf161(a);
            bitrv216(a);
        } else {
            cftf081(a);
            bitrv208(a);
        }
    } else if (n == 8) {
        cftf040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}


void cftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    if (n > 8) {
        if (n > 32) {
            cftb1st(n, a);
                if (n > 512) {
                    cftrec4(n, a);
                } else if (n > 128) {
                    cftleaf(n, 1, a);
                } else {
                    cftfx41(n, a);
                }
            bitrv2conj(n, a);
        } else if (n == 32) {
            cftf161(a);
            bitrv216neg(a);
        } else {
            cftf081(a);
            bitrv208neg(a);
        }
    } else if (n == 8) {
        cftb040(a);
    } else if (n == 4) {
        cftx020(a);
    }
}

/* **************************************************************************************************/

/* **************************************************************************************************/
void bitrv2(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 j0, k0, j1, k1, l, m, i, j, k, nh, m2;
    PICOFFTSG_FFTTYPE xr, xi, yr, yi;

    m = 4;
    for (l = n >> 2; l > 8; l >>= 2) {
        m <<= 1;
    }
    m2 = m + m;
    nh = n >> 1;
    if (l == 8) {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 4) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 4) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 += m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 += m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = nh >> 1; i > (k ^= i); i >>= 1) {
                    /* Avoid warning*/
                };
            }
            k1 = j0 + k0;
            j1 = k1 + 2;
            k1 += nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += m;
            k1 += m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += m;
            k1 -= m;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= 2;
            k1 -= nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nh + 2;
            k1 += nh + 2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= nh - m;
            k1 += m2 - 2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1) {
                /* Avoid warning */
            }
        }
    } else {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 4) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 4) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = nh >> 1; i > (k ^= i); i >>= 1){
                    /* Avoid warning */
                }
            }
            k1 = j0 + k0;
            j1 = k1 + 2;
            k1 += nh;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += m;
            k1 += m;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1){
                /* Avoid  warning */
            }
        }
    }
}


void bitrv2conj(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 j0, k0, j1, k1, l, m, i, j, k, nh, m2;
    PICOFFTSG_FFTTYPE xr, xi, yr, yi;


    m = 4;
    for (l = n >> 2; l > 8; l >>= 2) {
        m <<= 1;
    }
    m2 = m + m;
    nh = n >> 1;
    if (l == 8) {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 4) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 4) {
                xr = a[j];
                xi = -a[j + 1];
                yr = a[k];
                yi = -a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 += m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 += m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = nh >> 1; i > (k ^= i); i >>= 1) {
                    /* Avoid warning */
                }
            }
            k1 = j0 + k0;
            j1 = k1 + 2;
            k1 += nh;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            j1 += m;
            k1 += m2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += m;
            k1 -= m;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= 2;
            k1 -= nh;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 += nh + 2;
            k1 += nh + 2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            j1 -= nh - m;
            k1 += m2 - 2;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1) { /* Avoid warning*/

            }
        }
    } else {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 4) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 4) {
                xr = a[j];
                xi = -a[j + 1];
                yr = a[k];
                yi = -a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += nh;
                k1 += 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += 2;
                k1 += nh;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= nh;
                k1 -= 2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 -= m;
                k1 -= m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = nh >> 1; i > (k ^= i); i >>= 1) {
                    /* Avoid warning*/
                }
            }
            k1 = j0 + k0;
            j1 = k1 + 2;
            k1 += nh;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            j1 += m;
            k1 += m;
            a[j1 - 1] = -a[j1 - 1];
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            a[k1 + 3] = -a[k1 + 3];
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1) {
                /* Avoid warning*/
            }
        }
    }
}


void bitrv216(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
        x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i,
        x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;

    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    a[2] = x8r;
    a[3] = x8i;
    a[4] = x4r;
    a[5] = x4i;
    a[6] = x12r;
    a[7] = x12i;
    a[8] = x2r;
    a[9] = x2i;
    a[10] = x10r;
    a[11] = x10i;
    a[14] = x14r;
    a[15] = x14i;
    a[16] = x1r;
    a[17] = x1i;
    a[20] = x5r;
    a[21] = x5i;
    a[22] = x13r;
    a[23] = x13i;
    a[24] = x3r;
    a[25] = x3i;
    a[26] = x11r;
    a[27] = x11i;
    a[28] = x7r;
    a[29] = x7i;
}


void bitrv216neg(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
        x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i,
        x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i,
        x13r, x13i, x14r, x14i, x15r, x15i;

    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    x8r = a[16];
    x8i = a[17];
    x9r = a[18];
    x9i = a[19];
    x10r = a[20];
    x10i = a[21];
    x11r = a[22];
    x11i = a[23];
    x12r = a[24];
    x12i = a[25];
    x13r = a[26];
    x13i = a[27];
    x14r = a[28];
    x14i = a[29];
    x15r = a[30];
    x15i = a[31];
    a[2] = x15r;
    a[3] = x15i;
    a[4] = x7r;
    a[5] = x7i;
    a[6] = x11r;
    a[7] = x11i;
    a[8] = x3r;
    a[9] = x3i;
    a[10] = x13r;
    a[11] = x13i;
    a[12] = x5r;
    a[13] = x5i;
    a[14] = x9r;
    a[15] = x9i;
    a[16] = x1r;
    a[17] = x1i;
    a[18] = x14r;
    a[19] = x14i;
    a[20] = x6r;
    a[21] = x6i;
    a[22] = x10r;
    a[23] = x10i;
    a[24] = x2r;
    a[25] = x2i;
    a[26] = x12r;
    a[27] = x12i;
    a[28] = x4r;
    a[29] = x4i;
    a[30] = x8r;
    a[31] = x8i;
}


void bitrv208(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;

    x1r = a[2];
    x1i = a[3];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x6r = a[12];
    x6i = a[13];
    a[2] = x4r;
    a[3] = x4i;
    a[6] = x6r;
    a[7] = x6i;
    a[8] = x1r;
    a[9] = x1i;
    a[12] = x3r;
    a[13] = x3i;
}


void bitrv208neg(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i,
        x5r, x5i, x6r, x6i, x7r, x7i;

    x1r = a[2];
    x1i = a[3];
    x2r = a[4];
    x2i = a[5];
    x3r = a[6];
    x3i = a[7];
    x4r = a[8];
    x4i = a[9];
    x5r = a[10];
    x5i = a[11];
    x6r = a[12];
    x6i = a[13];
    x7r = a[14];
    x7i = a[15];
    a[2] = x7r;
    a[3] = x7i;
    a[4] = x3r;
    a[5] = x3i;
    a[6] = x5r;
    a[7] = x5i;
    a[8] = x1r;
    a[9] = x1i;
    a[10] = x6r;
    a[11] = x6i;
    a[12] = x2r;
    a[13] = x2i;
    a[14] = x4r;
    a[15] = x4i;
}


void bitrv1(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 j0, k0, j1, k1, l, m, i, j, k, nh;
    PICOFFTSG_FFTTYPE x;
    nh = n >> 1;
    x = a[1];
    a[1] = a[nh];
    a[nh] = x;
    m = 2;
    for (l = n >> 2; l > 2; l >>= 2) {
        m <<= 1;
    }
    if (l == 2) {
        j1 = m + 1;
        k1 = m + nh;
        x = a[j1];
        a[j1] = a[k1];
        a[k1] = x;
        j0 = 0;
        for (k0 = 2; k0 < m; k0 += 2) {
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1) {
                /* Avoid warning*/
            }
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                x = a[j];
                a[j] = a[k];
                a[k] = x;
                j1 = j + m;
                k1 = k + m;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 += nh;
                k1++;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 -= m;
                k1 -= m;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1++;
                k1 += nh;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 += m;
                k1 += m;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 -= nh;
                k1--;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 -= m;
                k1 -= m;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                for (i = nh >> 1; i > (k ^= i); i >>= 1) {
                    /* Avoid warning*/
                }
            }
            k1 = j0 + k0;
            j1 = k1 + 1;
            k1 += nh;
            x = a[j1];
            a[j1] = a[k1];
            a[k1] = x;
            j1 += m;
            k1 += m;
            x = a[j1];
            a[j1] = a[k1];
            a[k1] = x;
        }
    } else {
        j0 = 0;
        for (k0 = 2; k0 < m; k0 += 2) {
            for (i = nh >> 1; i > (j0 ^= i); i >>= 1) {
                /* Avoid warning*/
            }
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                x = a[j];
                a[j] = a[k];
                a[k] = x;
                j1 = j + nh;
                k1 = k + 1;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1++;
                k1 += nh;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                j1 -= nh;
                k1--;
                x = a[j1];
                a[j1] = a[k1];
                a[k1] = x;
                for (i = nh >> 1; i > (k ^= i); i >>= 1) {
                    /* Avoid warning*/
                }
            }
            k1 = j0 + k0;
            j1 = k1 + 1;
            k1 += nh;
            x = a[j1];
            a[j1] = a[k1];
            a[k1] = x;
        }
    }
}


/* **************************************************************************************************/

/* **************************************************************************************************/

void cftb1st(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, j0, j1, j2, j3, m, mh;
    PICOFFTSG_FFTTYPE wk1r, wk1i, wk3r, wk3i,
        wd1r, wd1i, wd3r, wd3i, ss1, ss3;
    PICOFFTSG_FFTTYPE x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = -a[1] - a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = -a[1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    a[j2] = x1r + x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r - x3i;
    a[j3 + 1] = x1i - x3r;
    wd1r = PICODSP_WGT_SHIFT;
    wd1i = 0;
    wd3r = PICODSP_WGT_SHIFT;
    wd3i = 0;

    wk1r  = (PICOFFTSG_FFTTYPE) (0.998795449734  *PICODSP_WGT_SHIFT);
    wk1i  = (PICOFFTSG_FFTTYPE) (0.049067676067  *PICODSP_WGT_SHIFT);
    ss1   = (PICOFFTSG_FFTTYPE) (0.098135352135  *PICODSP_WGT_SHIFT);
    wk3i  = (PICOFFTSG_FFTTYPE) (-0.146730467677 *PICODSP_WGT_SHIFT);
    wk3r  = (PICOFFTSG_FFTTYPE) (0.989176511765  *PICODSP_WGT_SHIFT);
    ss3   = (PICOFFTSG_FFTTYPE) (-0.293460935354 *PICODSP_WGT_SHIFT);

    i = 0;
    for (;;) {
        i0 = i + CDFT_LOOP_DIV_4;
        if (i0 > mh - 4) {
            i0 = mh - 4;
        }
        for (j = i + 2; j < i0; j += 4) {

            wd1r -= Mult_W_W(ss1, wk1i);
            wd1i += Mult_W_W(ss1, wk1r);
            wd3r -= Mult_W_W(ss3, wk3i);
            wd3i += Mult_W_W(ss3, wk3r);

            j1 = j + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j] + a[j2];
            x0i = -a[j + 1] - a[j2 + 1];
            x1r = a[j] - a[j2];
            x1i = -a[j + 1] + a[j2 + 1];
            x2r = a[j1] + a[j3];
            x2i = a[j1 + 1] + a[j3 + 1];
            x3r = a[j1] - a[j3];
            x3i = a[j1 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i - x2i;
            a[j1] = x0r - x2r;
            a[j1 + 1] = x0i + x2i;
            x0r = x1r + x3i;
            x0i = x1i + x3r;
            a[j2] = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
            a[j2 + 1] = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
            x0r = x1r - x3i;
            x0i = x1i - x3r;
            a[j3] = Mult_W_W(wk3r, x0r) + Mult_W_W(wk3i, x0i);
            a[j3 + 1] = Mult_W_W(wk3r, x0i) - Mult_W_W(wk3i, x0r);
            x0r = a[j + 2] + a[j2 + 2];
            x0i = -a[j + 3] - a[j2 + 3];
            x1r = a[j + 2] - a[j2 + 2];
            x1i = -a[j + 3] + a[j2 + 3];
            x2r = a[j1 + 2] + a[j3 + 2];
            x2i = a[j1 + 3] + a[j3 + 3];
            x3r = a[j1 + 2] - a[j3 + 2];
            x3i = a[j1 + 3] - a[j3 + 3];
            a[j + 2] = x0r + x2r;
            a[j + 3] = x0i - x2i;
            a[j1 + 2] = x0r - x2r;
            a[j1 + 3] = x0i + x2i;
            x0r = x1r + x3i;
            x0i = x1i + x3r;
            a[j2 + 2] = Mult_W_W(wd1r, x0r) - Mult_W_W(wd1i, x0i);
            a[j2 + 3] = Mult_W_W(wd1r, x0i) + Mult_W_W(wd1i, x0r);
            x0r = x1r - x3i;
            x0i = x1i - x3r;
            a[j3 + 2] = Mult_W_W(wd3r, x0r) + Mult_W_W(wd3i, x0i);
            a[j3 + 3] = Mult_W_W(wd3r, x0i) - Mult_W_W(wd3i, x0r);
            j0 = m - j;
            j1 = j0 + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j0] + a[j2];
            x0i = -a[j0 + 1] - a[j2 + 1];
            x1r = a[j0] - a[j2];
            x1i = -a[j0 + 1] + a[j2 + 1];
            x2r = a[j1] + a[j3];
            x2i = a[j1 + 1] + a[j3 + 1];
            x3r = a[j1] - a[j3];
            x3i = a[j1 + 1] - a[j3 + 1];
            a[j0] = x0r + x2r;
            a[j0 + 1] = x0i - x2i;
            a[j1] = x0r - x2r;
            a[j1 + 1] = x0i + x2i;
            x0r = x1r + x3i;
            x0i = x1i + x3r;
            a[j2] = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
            a[j2 + 1] = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
            x0r = x1r - x3i;
            x0i = x1i - x3r;
            a[j3] = Mult_W_W(wk3i, x0r) + Mult_W_W(wk3r, x0i);
            a[j3 + 1] = Mult_W_W(wk3i, x0i) - Mult_W_W(wk3r, x0r);
            x0r = a[j0 - 2] + a[j2 - 2];
            x0i = -a[j0 - 1] - a[j2 - 1];
            x1r = a[j0 - 2] - a[j2 - 2];
            x1i = -a[j0 - 1] + a[j2 - 1];
            x2r = a[j1 - 2] + a[j3 - 2];
            x2i = a[j1 - 1] + a[j3 - 1];
            x3r = a[j1 - 2] - a[j3 - 2];
            x3i = a[j1 - 1] - a[j3 - 1];
            a[j0 - 2] = x0r + x2r;
            a[j0 - 1] = x0i - x2i;
            a[j1 - 2] = x0r - x2r;
            a[j1 - 1] = x0i + x2i;
            x0r = x1r + x3i;
            x0i = x1i + x3r;
            a[j2 - 2] = Mult_W_W(wd1i, x0r) - Mult_W_W(wd1r, x0i);
            a[j2 - 1] = Mult_W_W(wd1i, x0i) + Mult_W_W(wd1r, x0r);
            x0r = x1r - x3i;
            x0i = x1i - x3r;
            a[j3 - 2] = Mult_W_W(wd3i, x0r) + Mult_W_W(wd3r, x0i);
            a[j3 - 1] = Mult_W_W(wd3i, x0i) - Mult_W_W(wd3r, x0r);
            wk1r -= Mult_W_W(ss1, wd1i);
            wk1i += Mult_W_W(ss1, wd1r);
            wk3r -= Mult_W_W(ss3, wd3i);
            wk3i += Mult_W_W(ss3, wd3r);
        }
        if (i0 == mh - 4) {
            break;
        }
    }
    wd1r = WR5000;
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = -a[j0 - 1] - a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = -a[j0 - 1] + a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i - x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    a[j2 - 1] = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = Mult_W_W(wk3r, x0r) + Mult_W_W(wk3i, x0i);
    a[j3 - 1] = Mult_W_W(wk3r, x0i) - Mult_W_W(wk3i, x0r);
    x0r = a[j0] + a[j2];
    x0i = -a[j0 + 1] - a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = -a[j0 + 1] + a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i - x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2] = picofftsg_mult_w_a(wd1r, (x0r - x0i));
    a[j2 + 1] = picofftsg_mult_w_a(wd1r, (x0i + x0r));
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3] = -picofftsg_mult_w_a(wd1r, (x0r + x0i));
    a[j3 + 1] = -picofftsg_mult_w_a(wd1r, (x0i - x0r));
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = -a[j0 + 3] - a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = -a[j0 + 3] + a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i - x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i + x2i;
    x0r = x1r + x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    a[j2 + 3] = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = x1r - x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = Mult_W_W(wk3i, x0r) + Mult_W_W(wk3r, x0i);
    a[j3 + 3] = Mult_W_W(wk3i, x0i) - Mult_W_W(wk3r, x0r);
}

void cftrec4(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 isplt, j, k, m;

    m = n;
    while (m > 512) {
        m >>= 2;
        cftmdl1(m, &a[n - m]);
    }
    cftleaf(m, 1, &a[n - m]);
    k = 0;
    for (j = n - m; j > 0; j -= m) {
        k++;
        isplt = cfttree(m, j, k, a);
        cftleaf(m, isplt, &a[j - m]);
    }
}


picoos_int32 cfttree(picoos_int32 n, picoos_int32 j, picoos_int32 k, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, isplt, m;

    if ((k & 3) != 0) {
        isplt = k & 1;
        if (isplt != 0) {
            cftmdl1(n, &a[j - n]);
        } else {
            cftmdl2(n, &a[j - n]);
        }
    } else {
        m = n;
        for (i = k; (i & 3) == 0; i >>= 2) {
            m <<= 2;
        }
        isplt = i & 1;
        if (isplt != 0) {
            while (m > 128) {
                cftmdl1(m, &a[j - m]);
                m >>= 2;
            }
        } else {
            while (m > 128) {
                cftmdl2(m, &a[j - m]);
                m >>= 2;
            }
        }
    }
    return isplt;
}


void cftleaf(picoos_int32 n, picoos_int32 isplt, PICOFFTSG_FFTTYPE *a)
{

    if (n == 512) {
        cftmdl1(128, a);
        cftf161(a);
        cftf162(&a[32]);
        cftf161(&a[64]);
        cftf161(&a[96]);
        cftmdl2(128, &a[128]);
        cftf161(&a[128]);
        cftf162(&a[160]);
        cftf161(&a[192]);
        cftf162(&a[224]);
        cftmdl1(128, &a[256]);
        cftf161(&a[256]);
        cftf162(&a[288]);
        cftf161(&a[320]);
        cftf161(&a[352]);
        if (isplt != 0) {
            cftmdl1(128, &a[384]);
            cftf161(&a[480]);
        } else {
            cftmdl2(128, &a[384]);
            cftf162(&a[480]);
        }
        cftf161(&a[384]);
        cftf162(&a[416]);
        cftf161(&a[448]);
    } else {
        cftmdl1(64, a);
        cftf081(a);
        cftf082(&a[16]);
        cftf081(&a[32]);
        cftf081(&a[48]);
        cftmdl2(64, &a[64]);
        cftf081(&a[64]);
        cftf082(&a[80]);
        cftf081(&a[96]);
        cftf082(&a[112]);
        cftmdl1(64, &a[128]);
        cftf081(&a[128]);
        cftf082(&a[144]);
        cftf081(&a[160]);
        cftf081(&a[176]);
        if (isplt != 0) {
            cftmdl1(64, &a[192]);
            cftf081(&a[240]);
        } else {
            cftmdl2(64, &a[192]);
            cftf082(&a[240]);
        }
        cftf081(&a[192]);
        cftf082(&a[208]);
        cftf081(&a[224]);
    }
}


void cftmdl1(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, j0, j1, j2, j3, m, mh;
    PICOFFTSG_FFTTYPE wk1r, wk1i, wk3r, wk3i,
        wd1r, wd1i, wd3r, wd3i, ss1, ss3;
    PICOFFTSG_FFTTYPE x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    mh = n >> 3;
    m = 2 * mh;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] + a[j2];
    x0i = a[1] + a[j2 + 1];
    x1r = a[0] - a[j2];
    x1i = a[1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    a[j2] = x1r - x3i;
    a[j2 + 1] = x1i + x3r;
    a[j3] = x1r + x3i;
    a[j3 + 1] = x1i - x3r;
    wd1r = (PICOFFTSG_FFTTYPE)(PICODSP_WGT_SHIFT);
    wd1i = 0;
    wd3r = (PICOFFTSG_FFTTYPE)(PICODSP_WGT_SHIFT);
    wd3i = 0;
    wk1r  =  (PICOFFTSG_FFTTYPE) (0.980785250664  *PICODSP_WGT_SHIFT);
    wk1i  =  (PICOFFTSG_FFTTYPE) (0.195090323687  *PICODSP_WGT_SHIFT);
    ss1   =  (PICOFFTSG_FFTTYPE) (0.390180647373  *PICODSP_WGT_SHIFT);
    wk3i  =  (PICOFFTSG_FFTTYPE) (-0.555570185184 *PICODSP_WGT_SHIFT);
    wk3r  =  (PICOFFTSG_FFTTYPE) (0.831469595432  *PICODSP_WGT_SHIFT);
    ss3   =  (PICOFFTSG_FFTTYPE) (-1.111140370369 *PICODSP_WGT_SHIFT);

    i = 0;
    for (;;) {
        i0 = i + CDFT_LOOP_DIV_4;
        if (i0 > mh - 4) {
            i0 = mh - 4;
        }
        for (j = i + 2; j < i0; j += 4) {
            wd1r -= Mult_W_W(ss1, wk1i);
            wd1i += Mult_W_W(ss1, wk1r);
            wd3r -= Mult_W_W(ss3, wk3i);
            wd3i += Mult_W_W(ss3, wk3r);
            j1 = j + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j] + a[j2];
            x0i = a[j + 1] + a[j2 + 1];
            x1r = a[j] - a[j2];
            x1i = a[j + 1] - a[j2 + 1];
            x2r = a[j1] + a[j3];
            x2i = a[j1 + 1] + a[j3 + 1];
            x3r = a[j1] - a[j3];
            x3i = a[j1 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            a[j1] = x0r - x2r;
            a[j1 + 1] = x0i - x2i;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j2] = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
            a[j2 + 1] = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = Mult_W_W(wk3r, x0r) + Mult_W_W(wk3i, x0i);
            a[j3 + 1] = Mult_W_W(wk3r, x0i) - Mult_W_W(wk3i, x0r);
            x0r = a[j + 2] + a[j2 + 2];
            x0i = a[j + 3] + a[j2 + 3];
            x1r = a[j + 2] - a[j2 + 2];
            x1i = a[j + 3] - a[j2 + 3];
            x2r = a[j1 + 2] + a[j3 + 2];
            x2i = a[j1 + 3] + a[j3 + 3];
            x3r = a[j1 + 2] - a[j3 + 2];
            x3i = a[j1 + 3] - a[j3 + 3];
            a[j + 2] = x0r + x2r;
            a[j + 3] = x0i + x2i;
            a[j1 + 2] = x0r - x2r;
            a[j1 + 3] = x0i - x2i;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j2 + 2] = Mult_W_W(wd1r, x0r) - Mult_W_W(wd1i, x0i);
            a[j2 + 3] = Mult_W_W(wd1r, x0i) + Mult_W_W(wd1i, x0r);
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3 + 2] = Mult_W_W(wd3r, x0r) + Mult_W_W(wd3i, x0i);
            a[j3 + 3] = Mult_W_W(wd3r, x0i) - Mult_W_W(wd3i, x0r);
            j0 = m - j;
            j1 = j0 + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j0] + a[j2];
            x0i = a[j0 + 1] + a[j2 + 1];
            x1r = a[j0] - a[j2];
            x1i = a[j0 + 1] - a[j2 + 1];
            x2r = a[j1] + a[j3];
            x2i = a[j1 + 1] + a[j3 + 1];
            x3r = a[j1] - a[j3];
            x3i = a[j1 + 1] - a[j3 + 1];
            a[j0] = x0r + x2r;
            a[j0 + 1] = x0i + x2i;
            a[j1] = x0r - x2r;
            a[j1 + 1] = x0i - x2i;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j2] = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
            a[j2 + 1] = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = Mult_W_W(wk3i, x0r) + Mult_W_W(wk3r, x0i);
            a[j3 + 1] = Mult_W_W(wk3i, x0i) - Mult_W_W(wk3r, x0r);
            x0r = a[j0 - 2] + a[j2 - 2];
            x0i = a[j0 - 1] + a[j2 - 1];
            x1r = a[j0 - 2] - a[j2 - 2];
            x1i = a[j0 - 1] - a[j2 - 1];
            x2r = a[j1 - 2] + a[j3 - 2];
            x2i = a[j1 - 1] + a[j3 - 1];
            x3r = a[j1 - 2] - a[j3 - 2];
            x3i = a[j1 - 1] - a[j3 - 1];
            a[j0 - 2] = x0r + x2r;
            a[j0 - 1] = x0i + x2i;
            a[j1 - 2] = x0r - x2r;
            a[j1 - 1] = x0i - x2i;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j2 - 2] = Mult_W_W(wd1i, x0r) - Mult_W_W(wd1r, x0i);
            a[j2 - 1] = Mult_W_W(wd1i, x0i) + Mult_W_W(wd1r, x0r);
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3 - 2] = Mult_W_W(wd3i, x0r) + Mult_W_W(wd3r, x0i);
            a[j3 - 1] = Mult_W_W(wd3i, x0i) - Mult_W_W(wd3r, x0r);
            wk1r -= Mult_W_W(ss1, wd1i);
            wk1i += Mult_W_W(ss1, wd1r);
            wk3r -= Mult_W_W(ss3, wd3i);
            wk3i += Mult_W_W(ss3, wd3r);
        }
        if (i0 == mh - 4) {
            break;
        }
    }
    wd1r = WR5000;
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] + a[j2 - 2];
    x0i = a[j0 - 1] + a[j2 - 1];
    x1r = a[j0 - 2] - a[j2 - 2];
    x1i = a[j0 - 1] - a[j2 - 1];
    x2r = a[j1 - 2] + a[j3 - 2];
    x2i = a[j1 - 1] + a[j3 - 1];
    x3r = a[j1 - 2] - a[j3 - 2];
    x3i = a[j1 - 1] - a[j3 - 1];
    a[j0 - 2] = x0r + x2r;
    a[j0 - 1] = x0i + x2i;
    a[j1 - 2] = x0r - x2r;
    a[j1 - 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 - 2] = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    a[j2 - 1] = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 - 2] = Mult_W_W(wk3r, x0r) + Mult_W_W(wk3i, x0i);
    a[j3 - 1] = Mult_W_W(wk3r, x0i) - Mult_W_W(wk3i, x0r);
    x0r = a[j0] + a[j2];
    x0i = a[j0 + 1] + a[j2 + 1];
    x1r = a[j0] - a[j2];
    x1i = a[j0 + 1] - a[j2 + 1];
    x2r = a[j1] + a[j3];
    x2i = a[j1 + 1] + a[j3 + 1];
    x3r = a[j1] - a[j3];
    x3i = a[j1 + 1] - a[j3 + 1];
    a[j0] = x0r + x2r;
    a[j0 + 1] = x0i + x2i;
    a[j1] = x0r - x2r;
    a[j1 + 1] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2] = picofftsg_mult_w_a(wd1r, (x0r - x0i));
    a[j2 + 1] = picofftsg_mult_w_a(wd1r, (x0i + x0r));
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3] = -picofftsg_mult_w_a(wd1r, (x0r + x0i));
    a[j3 + 1] = -picofftsg_mult_w_a(wd1r, (x0i - x0r));
    x0r = a[j0 + 2] + a[j2 + 2];
    x0i = a[j0 + 3] + a[j2 + 3];
    x1r = a[j0 + 2] - a[j2 + 2];
    x1i = a[j0 + 3] - a[j2 + 3];
    x2r = a[j1 + 2] + a[j3 + 2];
    x2i = a[j1 + 3] + a[j3 + 3];
    x3r = a[j1 + 2] - a[j3 + 2];
    x3i = a[j1 + 3] - a[j3 + 3];
    a[j0 + 2] = x0r + x2r;
    a[j0 + 3] = x0i + x2i;
    a[j1 + 2] = x0r - x2r;
    a[j1 + 3] = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[j2 + 2] = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    a[j2 + 3] = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    a[j3 + 2] = Mult_W_W(wk3i, x0r) + Mult_W_W(wk3r, x0i);
    a[j3 + 3] = Mult_W_W(wk3i, x0i) - Mult_W_W(wk3r, x0r);
}


void cftmdl2(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, j0, j1, j2, j3, m, mh;
    PICOFFTSG_FFTTYPE wn4r, wk1r, wk1i, wk3r, wk3i,
        wl1r, wl1i, wl3r, wl3i, wd1r, wd1i, wd3r, wd3i,
        we1r, we1i, we3r, we3i, ss1, ss3;
    PICOFFTSG_FFTTYPE x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;

    mh = n >> 3;
    m = 2 * mh;
    wn4r = WR5000;
    j1 = m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[0] - a[j2 + 1];
    x0i = a[1] + a[j2];
    x1r = a[0] + a[j2 + 1];
    x1i = a[1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = picofftsg_mult_w_a(wn4r, (x2r - x2i));
    y0i = picofftsg_mult_w_a(wn4r, (x2i + x2r));
    a[0] = x0r + y0r;
    a[1] = x0i + y0i;
    a[j1] = x0r - y0r;
    a[j1 + 1] = x0i - y0i;
    y0r = picofftsg_mult_w_a(wn4r, (x3r - x3i));
    y0i = picofftsg_mult_w_a(wn4r, (x3i + x3r));
    a[j2] = x1r - y0i;
    a[j2 + 1] = x1i + y0r;
    a[j3] = x1r + y0i;
    a[j3 + 1] = x1i - y0r;
    wl1r = PICODSP_WGT_SHIFT;
    wl1i = 0;
    wl3r = PICODSP_WGT_SHIFT;
    wl3i = 0;
    we1r = wn4r;
    we1i = wn4r;
    we3r = -wn4r;
    we3i = -wn4r;

    wk1r  =  (PICOFFTSG_FFTTYPE)(0.995184719563  *PICODSP_WGT_SHIFT);
    wk1i  =  (PICOFFTSG_FFTTYPE)(0.098017141223  *PICODSP_WGT_SHIFT);
    wd1r  =  (PICOFFTSG_FFTTYPE)(0.634393274784  *PICODSP_WGT_SHIFT);
    wd1i  =  (PICOFFTSG_FFTTYPE)(0.773010432720  *PICODSP_WGT_SHIFT);
    ss1   =  (PICOFFTSG_FFTTYPE)(0.196034282446  *PICODSP_WGT_SHIFT);
    wk3i  =  (PICOFFTSG_FFTTYPE)(-0.290284663439 *PICODSP_WGT_SHIFT);
    wk3r  =  (PICOFFTSG_FFTTYPE)(0.956940352917  *PICODSP_WGT_SHIFT);
    ss3   =  (PICOFFTSG_FFTTYPE)(-0.580569326878 *PICODSP_WGT_SHIFT);
    wd3r  =  (PICOFFTSG_FFTTYPE)(-0.881921231747 *PICODSP_WGT_SHIFT);
    wd3i  =  (PICOFFTSG_FFTTYPE)(-0.471396744251 *PICODSP_WGT_SHIFT);

    i = 0;
    for (;;) {
        i0 = i + 4 * CDFT_LOOP_DIV;
        if (i0 > mh - 4) {
            i0 = mh - 4;
        }
        for (j = i + 2; j < i0; j += 4) {
            wl1r -= Mult_W_W(ss1, wk1i);
            wl1i += Mult_W_W(ss1, wk1r);
            wl3r -= Mult_W_W(ss3, wk3i);
            wl3i += Mult_W_W(ss3, wk3r);
            we1r -= Mult_W_W(ss1, wd1i);
            we1i += Mult_W_W(ss1, wd1r);
            we3r -= Mult_W_W(ss3, wd3i);
            we3i += Mult_W_W(ss3, wd3r);
            j1 = j + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j] - a[j2 + 1];
            x0i = a[j + 1] + a[j2];
            x1r = a[j] + a[j2 + 1];
            x1i = a[j + 1] - a[j2];
            x2r = a[j1] - a[j3 + 1];
            x2i = a[j1 + 1] + a[j3];
            x3r = a[j1] + a[j3 + 1];
            x3i = a[j1 + 1] - a[j3];
            y0r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
            y0i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
            y2r = Mult_W_W(wd1r, x2r) - Mult_W_W(wd1i, x2i);
            y2i = Mult_W_W(wd1r, x2i) + Mult_W_W(wd1i, x2r);
            a[j] = y0r + y2r;
            a[j + 1] = y0i + y2i;
            a[j1] = y0r - y2r;
            a[j1 + 1] = y0i - y2i;
            y0r = Mult_W_W(wk3r, x1r) + Mult_W_W(wk3i, x1i);
            y0i = Mult_W_W(wk3r, x1i) - Mult_W_W(wk3i, x1r);
            y2r = Mult_W_W(wd3r, x3r) + Mult_W_W(wd3i, x3i);
            y2i = Mult_W_W(wd3r, x3i) - Mult_W_W(wd3i, x3r);
            a[j2] = y0r + y2r;
            a[j2 + 1] = y0i + y2i;
            a[j3] = y0r - y2r;
            a[j3 + 1] = y0i - y2i;
            x0r = a[j + 2] - a[j2 + 3];
            x0i = a[j + 3] + a[j2 + 2];
            x1r = a[j + 2] + a[j2 + 3];
            x1i = a[j + 3] - a[j2 + 2];
            x2r = a[j1 + 2] - a[j3 + 3];
            x2i = a[j1 + 3] + a[j3 + 2];
            x3r = a[j1 + 2] + a[j3 + 3];
            x3i = a[j1 + 3] - a[j3 + 2];
            y0r = Mult_W_W(wl1r, x0r) - Mult_W_W(wl1i, x0i);
            y0i = Mult_W_W(wl1r, x0i) + Mult_W_W(wl1i, x0r);
            y2r = Mult_W_W(we1r, x2r) - Mult_W_W(we1i, x2i);
            y2i = Mult_W_W(we1r, x2i) + Mult_W_W(we1i, x2r);
            a[j + 2] = y0r + y2r;
            a[j + 3] = y0i + y2i;
            a[j1 + 2] = y0r - y2r;
            a[j1 + 3] = y0i - y2i;
            y0r = Mult_W_W(wl3r, x1r) + Mult_W_W(wl3i, x1i);
            y0i = Mult_W_W(wl3r, x1i) - Mult_W_W(wl3i, x1r);
            y2r = Mult_W_W(we3r, x3r) + Mult_W_W(we3i, x3i);
            y2i = Mult_W_W(we3r, x3i) - Mult_W_W(we3i, x3r);
            a[j2 + 2] = y0r + y2r;
            a[j2 + 3] = y0i + y2i;
            a[j3 + 2] = y0r - y2r;
            a[j3 + 3] = y0i - y2i;
            j0 = m - j;
            j1 = j0 + m;
            j2 = j1 + m;
            j3 = j2 + m;
            x0r = a[j0] - a[j2 + 1];
            x0i = a[j0 + 1] + a[j2];
            x1r = a[j0] + a[j2 + 1];
            x1i = a[j0 + 1] - a[j2];
            x2r = a[j1] - a[j3 + 1];
            x2i = a[j1 + 1] + a[j3];
            x3r = a[j1] + a[j3 + 1];
            x3i = a[j1 + 1] - a[j3];
            y0r = Mult_W_W(wd1i, x0r) - Mult_W_W(wd1r, x0i);
            y0i = Mult_W_W(wd1i, x0i) + Mult_W_W(wd1r, x0r);
            y2r = Mult_W_W(wk1i, x2r) - Mult_W_W(wk1r, x2i);
            y2i = Mult_W_W(wk1i, x2i) + Mult_W_W(wk1r, x2r);
            a[j0] = y0r + y2r;
            a[j0 + 1] = y0i + y2i;
            a[j1] = y0r - y2r;
            a[j1 + 1] = y0i - y2i;
            y0r = Mult_W_W(wd3i, x1r) + Mult_W_W(wd3r, x1i);
            y0i = Mult_W_W(wd3i, x1i) - Mult_W_W(wd3r, x1r);
            y2r = Mult_W_W(wk3i, x3r) + Mult_W_W(wk3r, x3i);
            y2i = Mult_W_W(wk3i, x3i) - Mult_W_W(wk3r, x3r);
            a[j2] = y0r + y2r;
            a[j2 + 1] = y0i + y2i;
            a[j3] = y0r - y2r;
            a[j3 + 1] = y0i - y2i;
            x0r = a[j0 - 2] - a[j2 - 1];
            x0i = a[j0 - 1] + a[j2 - 2];
            x1r = a[j0 - 2] + a[j2 - 1];
            x1i = a[j0 - 1] - a[j2 - 2];
            x2r = a[j1 - 2] - a[j3 - 1];
            x2i = a[j1 - 1] + a[j3 - 2];
            x3r = a[j1 - 2] + a[j3 - 1];
            x3i = a[j1 - 1] - a[j3 - 2];
            y0r = Mult_W_W(we1i, x0r) - Mult_W_W(we1r, x0i);
            y0i = Mult_W_W(we1i, x0i) + Mult_W_W(we1r, x0r);
            y2r = Mult_W_W(wl1i, x2r) - Mult_W_W(wl1r, x2i);
            y2i = Mult_W_W(wl1i, x2i) + Mult_W_W(wl1r, x2r);
            a[j0 - 2] = y0r + y2r;
            a[j0 - 1] = y0i + y2i;
            a[j1 - 2] = y0r - y2r;
            a[j1 - 1] = y0i - y2i;
            y0r = Mult_W_W(we3i, x1r) + Mult_W_W(we3r, x1i);
            y0i = Mult_W_W(we3i, x1i) - Mult_W_W(we3r, x1r);
            y2r = Mult_W_W(wl3i, x3r) + Mult_W_W(wl3r, x3i);
            y2i = Mult_W_W(wl3i, x3i) - Mult_W_W(wl3r, x3r);
            a[j2 - 2] = y0r + y2r;
            a[j2 - 1] = y0i + y2i;
            a[j3 - 2] = y0r - y2r;
            a[j3 - 1] = y0i - y2i;
            wk1r -= Mult_W_W(ss1, wl1i);
            wk1i += Mult_W_W(ss1, wl1r);
            wk3r -= Mult_W_W(ss3, wl3i);
            wk3i += Mult_W_W(ss3, wl3r);
            wd1r -= Mult_W_W(ss1, we1i);
            wd1i += Mult_W_W(ss1, we1r);
            wd3r -= Mult_W_W(ss3, we3i);
            wd3i += Mult_W_W(ss3, we3r);
        }
        if (i0 == mh - 4) {
            break;
        }
    }
    wl1r = WR2500;
    wl1i = WI2500;
    j0 = mh;
    j1 = j0 + m;
    j2 = j1 + m;
    j3 = j2 + m;
    x0r = a[j0 - 2] - a[j2 - 1];
    x0i = a[j0 - 1] + a[j2 - 2];
    x1r = a[j0 - 2] + a[j2 - 1];
    x1i = a[j0 - 1] - a[j2 - 2];
    x2r = a[j1 - 2] - a[j3 - 1];
    x2i = a[j1 - 1] + a[j3 - 2];
    x3r = a[j1 - 2] + a[j3 - 1];
    x3i = a[j1 - 1] - a[j3 - 2];
    y0r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    y0i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    y2r = Mult_W_W(wd1r, x2r) - Mult_W_W(wd1i, x2i);
    y2i = Mult_W_W(wd1r, x2i) + Mult_W_W(wd1i, x2r);
    a[j0 - 2] = y0r + y2r;
    a[j0 - 1] = y0i + y2i;
    a[j1 - 2] = y0r - y2r;
    a[j1 - 1] = y0i - y2i;
    y0r = Mult_W_W(wk3r, x1r) + Mult_W_W(wk3i, x1i);
    y0i = Mult_W_W(wk3r, x1i) - Mult_W_W(wk3i, x1r);
    y2r = Mult_W_W(wd3r, x3r) + Mult_W_W(wd3i, x3i);
    y2i = Mult_W_W(wd3r, x3i) - Mult_W_W(wd3i, x3r);
    a[j2 - 2] = y0r + y2r;
    a[j2 - 1] = y0i + y2i;
    a[j3 - 2] = y0r - y2r;
    a[j3 - 1] = y0i - y2i;
    x0r = a[j0] - a[j2 + 1];
    x0i = a[j0 + 1] + a[j2];
    x1r = a[j0] + a[j2 + 1];
    x1i = a[j0 + 1] - a[j2];
    x2r = a[j1] - a[j3 + 1];
    x2i = a[j1 + 1] + a[j3];
    x3r = a[j1] + a[j3 + 1];
    x3i = a[j1 + 1] - a[j3];
    y0r = Mult_W_W(wl1r, x0r) - Mult_W_W(wl1i, x0i);
    y0i = Mult_W_W(wl1r, x0i) + Mult_W_W(wl1i, x0r);
    y2r = Mult_W_W(wl1i, x2r) - Mult_W_W(wl1r, x2i);
    y2i = Mult_W_W(wl1i, x2i) + Mult_W_W(wl1r, x2r);
    a[j0] = y0r + y2r;
    a[j0 + 1] = y0i + y2i;
    a[j1] = y0r - y2r;
    a[j1 + 1] = y0i - y2i;
    y0r = Mult_W_W(wl1i, x1r) - Mult_W_W(wl1r, x1i);
    y0i = Mult_W_W(wl1i, x1i) + Mult_W_W(wl1r, x1r);
    y2r = Mult_W_W(wl1r, x3r) - Mult_W_W(wl1i, x3i);
    y2i = Mult_W_W(wl1r, x3i) + Mult_W_W(wl1i, x3r);
    a[j2] = y0r - y2r;
    a[j2 + 1] = y0i - y2i;
    a[j3] = y0r + y2r;
    a[j3 + 1] = y0i + y2i;
    x0r = a[j0 + 2] - a[j2 + 3];
    x0i = a[j0 + 3] + a[j2 + 2];
    x1r = a[j0 + 2] + a[j2 + 3];
    x1i = a[j0 + 3] - a[j2 + 2];
    x2r = a[j1 + 2] - a[j3 + 3];
    x2i = a[j1 + 3] + a[j3 + 2];
    x3r = a[j1 + 2] + a[j3 + 3];
    x3i = a[j1 + 3] - a[j3 + 2];
    y0r = Mult_W_W(wd1i, x0r) - Mult_W_W(wd1r, x0i);
    y0i = Mult_W_W(wd1i, x0i) + Mult_W_W(wd1r, x0r);
    y2r = Mult_W_W(wk1i, x2r) - Mult_W_W(wk1r, x2i);
    y2i = Mult_W_W(wk1i, x2i) + Mult_W_W(wk1r, x2r);
    a[j0 + 2] = y0r + y2r;
    a[j0 + 3] = y0i + y2i;
    a[j1 + 2] = y0r - y2r;
    a[j1 + 3] = y0i - y2i;
    y0r = Mult_W_W(wd3i, x1r) + Mult_W_W(wd3r, x1i);
    y0i = Mult_W_W(wd3i, x1i) - Mult_W_W(wd3r, x1r);
    y2r = Mult_W_W(wk3i, x3r) + Mult_W_W(wk3r, x3i);
    y2i = Mult_W_W(wk3i, x3i) - Mult_W_W(wk3r, x3r);
    a[j2 + 2] = y0r + y2r;
    a[j2 + 3] = y0i + y2i;
    a[j3 + 2] = y0r - y2r;
    a[j3 + 3] = y0i - y2i;
}


void cftfx41(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{

    if (n == 128) {
        cftf161(a);
        cftf162(&a[32]);
        cftf161(&a[64]);
        cftf161(&a[96]);
    } else {
        cftf081(a);
        cftf082(&a[16]);
        cftf081(&a[32]);
        cftf081(&a[48]);
    }
}


void cftf161(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE wn4r, wk1r, wk1i,
        x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i,
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i,
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

    wn4r = WR5000;
    wk1r = WR2500;
    wk1i = WI2500;
    x0r = a[0] + a[16];
    x0i = a[1] + a[17];
    x1r = a[0] - a[16];
    x1i = a[1] - a[17];
    x2r = a[8] + a[24];
    x2i = a[9] + a[25];
    x3r = a[8] - a[24];
    x3i = a[9] - a[25];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y4r = x0r - x2r;
    y4i = x0i - x2i;
    y8r = x1r - x3i;
    y8i = x1i + x3r;
    y12r = x1r + x3i;
    y12i = x1i - x3r;
    x0r = a[2] + a[18];
    x0i = a[3] + a[19];
    x1r = a[2] - a[18];
    x1i = a[3] - a[19];
    x2r = a[10] + a[26];
    x2i = a[11] + a[27];
    x3r = a[10] - a[26];
    x3i = a[11] - a[27];
    y1r = x0r + x2r;
    y1i = x0i + x2i;
    y5r = x0r - x2r;
    y5i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y9r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    y9i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y13r = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    y13i = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = a[4] + a[20];
    x0i = a[5] + a[21];
    x1r = a[4] - a[20];
    x1i = a[5] - a[21];
    x2r = a[12] + a[28];
    x2i = a[13] + a[29];
    x3r = a[12] - a[28];
    x3i = a[13] - a[29];
    y2r = x0r + x2r;
    y2i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y10r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    y10i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y14r = picofftsg_mult_w_a(wn4r, (x0r + x0i));
    y14i = picofftsg_mult_w_a(wn4r, (x0i - x0r));
    x0r = a[6] + a[22];
    x0i = a[7] + a[23];
    x1r = a[6] - a[22];
    x1i = a[7] - a[23];
    x2r = a[14] + a[30];
    x2i = a[15] + a[31];
    x3r = a[14] - a[30];
    x3i = a[15] - a[31];
    y3r = x0r + x2r;
    y3i = x0i + x2i;
    y7r = x0r - x2r;
    y7i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    y11r = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    y11i = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = x1r + x3i;
    x0i = x1i - x3r;
    y15r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    y15i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = y12r - y14r;
    x0i = y12i - y14i;
    x1r = y12r + y14r;
    x1i = y12i + y14i;
    x2r = y13r - y15r;
    x2i = y13i - y15i;
    x3r = y13r + y15r;
    x3i = y13i + y15i;
    a[24] = x0r + x2r;
    a[25] = x0i + x2i;
    a[26] = x0r - x2r;
    a[27] = x0i - x2i;
    a[28] = x1r - x3i;
    a[29] = x1i + x3r;
    a[30] = x1r + x3i;
    a[31] = x1i - x3r;
    x0r = y8r + y10r;
    x0i = y8i + y10i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    x3r = y9r - y11r;
    x3i = y9i - y11i;
    a[16] = x0r + x2r;
    a[17] = x0i + x2i;
    a[18] = x0r - x2r;
    a[19] = x0i - x2i;
    a[20] = x1r - x3i;
    a[21] = x1i + x3r;
    a[22] = x1r + x3i;
    a[23] = x1i - x3r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x3r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x3i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    x0r = y4r - y6i;
    x0i = y4i + y6r;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[10] = x0r - x2r;
    a[11] = x0i - x2i;
    a[12] = x1r - x3i;
    a[13] = x1i + x3r;
    a[14] = x1r + x3i;
    a[15] = x1i - x3r;
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    x3r = y1r - y3r;
    x3i = y1i - y3i;
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x0r - x2r;
    a[3] = x0i - x2i;
    a[4] = x1r - x3i;
    a[5] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}


void cftf162(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i,
        x0r, x0i, x1r, x1i, x2r, x2i,
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i,
        y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i,
        y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

    wn4r = WR5000;
    wk1r = WR1250;
    wk1i = WI1250;
    wk2r = WR2500;
    wk2i = WI2500;
    wk3r = WR3750;
    wk3i = WI3750;
    x1r = a[0] - a[17];
    x1i = a[1] + a[16];
    x0r = a[8] - a[25];
    x0i = a[9] + a[24];
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    y0r = x1r + x2r;
    y0i = x1i + x2i;
    y4r = x1r - x2r;
    y4i = x1i - x2i;
    x1r = a[0] + a[17];
    x1i = a[1] - a[16];
    x0r = a[8] + a[25];
    x0i = a[9] - a[24];
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    y8r = x1r - x2i;
    y8i = x1i + x2r;
    y12r = x1r + x2i;
    y12i = x1i - x2r;
    x0r = a[2] - a[19];
    x0i = a[3] + a[18];
    x1r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    x1i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = a[10] - a[27];
    x0i = a[11] + a[26];
    x2r = Mult_W_W(wk3i, x0r) - Mult_W_W(wk3r, x0i);
    x2i = Mult_W_W(wk3i, x0i) + Mult_W_W(wk3r, x0r);
    y1r = x1r + x2r;
    y1i = x1i + x2i;
    y5r = x1r - x2r;
    y5i = x1i - x2i;
    x0r = a[2] + a[19];
    x0i = a[3] - a[18];
    x1r = Mult_W_W(wk3r, x0r) - Mult_W_W(wk3i, x0i);
    x1i = Mult_W_W(wk3r, x0i) + Mult_W_W(wk3i, x0r);
    x0r = a[10] + a[27];
    x0i = a[11] - a[26];
    x2r = Mult_W_W(wk1r, x0r) + Mult_W_W(wk1i, x0i);
    x2i = Mult_W_W(wk1r, x0i) - Mult_W_W(wk1i, x0r);
    y9r = x1r - x2r;
    y9i = x1i - x2i;
    y13r = x1r + x2r;
    y13i = x1i + x2i;
    x0r = a[4] - a[21];
    x0i = a[5] + a[20];
    x1r = Mult_W_W(wk2r, x0r) - Mult_W_W(wk2i, x0i);
    x1i = Mult_W_W(wk2r, x0i) + Mult_W_W(wk2i, x0r);
    x0r = a[12] - a[29];
    x0i = a[13] + a[28];
    x2r = Mult_W_W(wk2i, x0r) - Mult_W_W(wk2r, x0i);
    x2i = Mult_W_W(wk2i, x0i) + Mult_W_W(wk2r, x0r);
    y2r = x1r + x2r;
    y2i = x1i + x2i;
    y6r = x1r - x2r;
    y6i = x1i - x2i;
    x0r = a[4] + a[21];
    x0i = a[5] - a[20];
    x1r = Mult_W_W(wk2i, x0r) - Mult_W_W(wk2r, x0i);
    x1i = Mult_W_W(wk2i, x0i) + Mult_W_W(wk2r, x0r);
    x0r = a[12] + a[29];
    x0i = a[13] - a[28];
    x2r = Mult_W_W(wk2r, x0r) - Mult_W_W(wk2i, x0i);
    x2i = Mult_W_W(wk2r, x0i) + Mult_W_W(wk2i, x0r);
    y10r = x1r - x2r;
    y10i = x1i - x2i;
    y14r = x1r + x2r;
    y14i = x1i + x2i;
    x0r = a[6] - a[23];
    x0i = a[7] + a[22];
    x1r = Mult_W_W(wk3r, x0r) - Mult_W_W(wk3i, x0i);
    x1i = Mult_W_W(wk3r, x0i) + Mult_W_W(wk3i, x0r);
    x0r = a[14] - a[31];
    x0i = a[15] + a[30];
    x2r = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    x2i = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    y3r = x1r + x2r;
    y3i = x1i + x2i;
    y7r = x1r - x2r;
    y7i = x1i - x2i;
    x0r = a[6] + a[23];
    x0i = a[7] - a[22];
    x1r = Mult_W_W(wk1i, x0r) + Mult_W_W(wk1r, x0i);
    x1i = Mult_W_W(wk1i, x0i) - Mult_W_W(wk1r, x0r);
    x0r = a[14] + a[31];
    x0i = a[15] - a[30];
    x2r = Mult_W_W(wk3i, x0r) - Mult_W_W(wk3r, x0i);
    x2i = Mult_W_W(wk3i, x0i) + Mult_W_W(wk3r, x0r);
    y11r = x1r + x2r;
    y11i = x1i + x2i;
    y15r = x1r - x2r;
    y15i = x1i - x2i;
    x1r = y0r + y2r;
    x1i = y0i + y2i;
    x2r = y1r + y3r;
    x2i = y1i + y3i;
    a[0] = x1r + x2r;
    a[1] = x1i + x2i;
    a[2] = x1r - x2r;
    a[3] = x1i - x2i;
    x1r = y0r - y2r;
    x1i = y0i - y2i;
    x2r = y1r - y3r;
    x2i = y1i - y3i;
    a[4] = x1r - x2i;
    a[5] = x1i + x2r;
    a[6] = x1r + x2i;
    a[7] = x1i - x2r;
    x1r = y4r - y6i;
    x1i = y4i + y6r;
    x0r = y5r - y7i;
    x0i = y5i + y7r;
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    a[8] = x1r + x2r;
    a[9] = x1i + x2i;
    a[10] = x1r - x2r;
    a[11] = x1i - x2i;
    x1r = y4r + y6i;
    x1i = y4i - y6r;
    x0r = y5r + y7i;
    x0i = y5i - y7r;
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    a[12] = x1r - x2i;
    a[13] = x1i + x2r;
    a[14] = x1r + x2i;
    a[15] = x1i - x2r;
    x1r = y8r + y10r;
    x1i = y8i + y10i;
    x2r = y9r - y11r;
    x2i = y9i - y11i;
    a[16] = x1r + x2r;
    a[17] = x1i + x2i;
    a[18] = x1r - x2r;
    a[19] = x1i - x2i;
    x1r = y8r - y10r;
    x1i = y8i - y10i;
    x2r = y9r + y11r;
    x2i = y9i + y11i;
    a[20] = x1r - x2i;
    a[21] = x1i + x2r;
    a[22] = x1r + x2i;
    a[23] = x1i - x2r;
    x1r = y12r - y14i;
    x1i = y12i + y14r;
    x0r = y13r + y15i;
    x0i = y13i - y15r;
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    a[24] = x1r + x2r;
    a[25] = x1i + x2i;
    a[26] = x1r - x2r;
    a[27] = x1i - x2i;
    x1r = y12r + y14i;
    x1i = y12i - y14r;
    x0r = y13r - y15i;
    x0i = y13i + y15r;
    x2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    x2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    a[28] = x1r - x2i;
    a[29] = x1i + x2r;
    a[30] = x1r + x2i;
    a[31] = x1i - x2r;
}


void cftf081(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i,
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

    wn4r = WR5000;
    x0r = a[0] + a[8];
    x0i = a[1] + a[9];
    x1r = a[0] - a[8];
    x1i = a[1] - a[9];
    x2r = a[4] + a[12];
    x2i = a[5] + a[13];
    x3r = a[4] - a[12];
    x3i = a[5] - a[13];
    y0r = x0r + x2r;
    y0i = x0i + x2i;
    y2r = x0r - x2r;
    y2i = x0i - x2i;
    y1r = x1r - x3i;
    y1i = x1i + x3r;
    y3r = x1r + x3i;
    y3i = x1i - x3r;
    x0r = a[2] + a[10];
    x0i = a[3] + a[11];
    x1r = a[2] - a[10];
    x1i = a[3] - a[11];
    x2r = a[6] + a[14];
    x2i = a[7] + a[15];
    x3r = a[6] - a[14];
    x3i = a[7] - a[15];
    y4r = x0r + x2r;
    y4i = x0i + x2i;
    y6r = x0r - x2r;
    y6i = x0i - x2i;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    x2r = x1r + x3i;
    x2i = x1i - x3r;
    y5r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    y5i = picofftsg_mult_w_a(wn4r, (x0r + x0i));
    y7r = picofftsg_mult_w_a(wn4r, (x2r - x2i));
    y7i = picofftsg_mult_w_a(wn4r, (x2r + x2i));
    a[8] = y1r + y5r;
    a[9] = y1i + y5i;
    a[10] = y1r - y5r;
    a[11] = y1i - y5i;
    a[12] = y3r - y7i;
    a[13] = y3i + y7r;
    a[14] = y3r + y7i;
    a[15] = y3i - y7r;
    a[0] = y0r + y4r;
    a[1] = y0i + y4i;
    a[2] = y0r - y4r;
    a[3] = y0i - y4i;
    a[4] = y2r - y6i;
    a[5] = y2i + y6r;
    a[6] = y2r + y6i;
    a[7] = y2i - y6r;
}


void cftf082(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i,
        y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i,
        y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

    wn4r = WR5000;
    wk1r = WR2500;
    wk1i = WI2500;
    y0r = a[0] - a[9];
    y0i = a[1] + a[8];
    y1r = a[0] + a[9];
    y1i = a[1] - a[8];
    x0r = a[4] - a[13];
    x0i = a[5] + a[12];
    y2r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    y2i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    x0r = a[4] + a[13];
    x0i = a[5] - a[12];
    y3r = picofftsg_mult_w_a(wn4r, (x0r - x0i));
    y3i = picofftsg_mult_w_a(wn4r, (x0i + x0r));
    x0r = a[2] - a[11];
    x0i = a[3] + a[10];
    y4r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    y4i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = a[2] + a[11];
    x0i = a[3] - a[10];
    y5r = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    y5i = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = a[6] - a[15];
    x0i = a[7] + a[14];
    y6r = Mult_W_W(wk1i, x0r) - Mult_W_W(wk1r, x0i);
    y6i = Mult_W_W(wk1i, x0i) + Mult_W_W(wk1r, x0r);
    x0r = a[6] + a[15];
    x0i = a[7] - a[14];
    y7r = Mult_W_W(wk1r, x0r) - Mult_W_W(wk1i, x0i);
    y7i = Mult_W_W(wk1r, x0i) + Mult_W_W(wk1i, x0r);
    x0r = y0r + y2r;
    x0i = y0i + y2i;
    x1r = y4r + y6r;
    x1i = y4i + y6i;
    a[0] = x0r + x1r;
    a[1] = x0i + x1i;
    a[2] = x0r - x1r;
    a[3] = x0i - x1i;
    x0r = y0r - y2r;
    x0i = y0i - y2i;
    x1r = y4r - y6r;
    x1i = y4i - y6i;
    a[4] = x0r - x1i;
    a[5] = x0i + x1r;
    a[6] = x0r + x1i;
    a[7] = x0i - x1r;
    x0r = y1r - y3i;
    x0i = y1i + y3r;
    x1r = y5r - y7r;
    x1i = y5i - y7i;
    a[8] = x0r + x1r;
    a[9] = x0i + x1i;
    a[10] = x0r - x1r;
    a[11] = x0i - x1i;
    x0r = y1r + y3i;
    x0i = y1i - y3r;
    x1r = y5r + y7r;
    x1i = y5i + y7i;
    a[12] = x0r - x1i;
    a[13] = x0i + x1r;
    a[14] = x0r + x1i;
    a[15] = x0i - x1r;
}


void cftf040(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
}


void cftb040(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

    x0r = a[0] + a[4];
    x0i = a[1] + a[5];
    x1r = a[0] - a[4];
    x1i = a[1] - a[5];
    x2r = a[2] + a[6];
    x2i = a[3] + a[7];
    x3r = a[2] - a[6];
    x3i = a[3] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[2] = x1r + x3i;
    a[3] = x1i - x3r;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[6] = x1r - x3i;
    a[7] = x1i + x3r;
}


void cftx020(PICOFFTSG_FFTTYPE *a)
{
    PICOFFTSG_FFTTYPE x0r, x0i;

    x0r = a[0] - a[2];
    x0i = a[1] - a[3];
    a[0] += a[2];
    a[1] += a[3];
    a[2] = x0r;
    a[3] = x0i;
}


void rftfsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, k;
    PICOFFTSG_FFTTYPE w1r, w1i, wkr, wki, wdr, wdi, ss, xr, xi, yr, yi;

    wkr = 0;
    wki = 0;

    switch (n) {
        case 8 :
            wdi=(PICOFFTSG_FFTTYPE)(0.353553414345*PICODSP_WGT_SHIFT); wdr=(PICOFFTSG_FFTTYPE)(0.146446630359*PICODSP_WGT_SHIFT);
            w1r=(PICOFFTSG_FFTTYPE)(0.707106709480*PICODSP_WGT_SHIFT); w1i=(PICOFFTSG_FFTTYPE)(0.707106828690*PICODSP_WGT_SHIFT);
            ss =(PICOFFTSG_FFTTYPE)(1.414213657379*PICODSP_WGT_SHIFT); break;
        case 16 :
            wdi=(PICOFFTSG_FFTTYPE)(0.191341713071*PICODSP_WGT_SHIFT); wdr=(PICOFFTSG_FFTTYPE)(0.038060232997*PICODSP_WGT_SHIFT);
            w1r=(PICOFFTSG_FFTTYPE)(0.923879504204*PICODSP_WGT_SHIFT); w1i=(PICOFFTSG_FFTTYPE)(0.382683426142*PICODSP_WGT_SHIFT);
            ss =(PICOFFTSG_FFTTYPE)(0.765366852283*PICODSP_WGT_SHIFT); break;
        case 32 :
            wdi=(PICOFFTSG_FFTTYPE)(0.097545161843*PICODSP_WGT_SHIFT); wdr=(PICOFFTSG_FFTTYPE)(0.009607359767*PICODSP_WGT_SHIFT);
            w1r=(PICOFFTSG_FFTTYPE)(0.980785250664*PICODSP_WGT_SHIFT); w1i=(PICOFFTSG_FFTTYPE)(0.195090323687*PICODSP_WGT_SHIFT);
            ss =(PICOFFTSG_FFTTYPE)(0.390180647373*PICODSP_WGT_SHIFT); break;
        case 64 :
            wdi=(PICOFFTSG_FFTTYPE)(0.049008570611*PICODSP_WGT_SHIFT); wdr=(PICOFFTSG_FFTTYPE)(0.002407636726*PICODSP_WGT_SHIFT);
            w1r=(PICOFFTSG_FFTTYPE)(0.995184719563*PICODSP_WGT_SHIFT); w1i=(PICOFFTSG_FFTTYPE)(0.098017141223*PICODSP_WGT_SHIFT);
            ss =(PICOFFTSG_FFTTYPE)(0.196034282446*PICODSP_WGT_SHIFT); break;
        default :
            wdr = 0; wdi = 0; ss = 0;
            break;
    }

    i = n >> 1;
    for (;;) {
        i0 = i - RDFT_LOOP_DIV_4;
        if (i0 < 4) {
            i0 = 4;
        }
        for (j = i - 4; j >= i0; j -= 4) {
            k = n - j;
            xr = a[j + 2] - a[k - 2];
            xi = a[j + 3] + a[k - 1];
            yr = Mult_W_W(wdr, xr) - Mult_W_W(wdi, xi);
            yi = Mult_W_W(wdr, xi) + Mult_W_W(wdi, xr);
            a[j + 2] -= yr;
            a[j + 3] -= yi;
            a[k - 2] += yr;
            a[k - 1] -= yi;
            wkr += Mult_W_W(ss, wdi);
            wki += picofftsg_mult_w_w(ss, (PICOFFTSG_WGT_SHIFT2 - wdr));
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = Mult_W_W(wkr, xr) - Mult_W_W(wki, xi);
            yi = Mult_W_W(wkr, xi) + Mult_W_W(wki, xr);
            a[j] -= yr;
            a[j + 1] -= yi;
            a[k] += yr;
            a[k + 1] -= yi;
            wdr += Mult_W_W(ss, wki);
            wdi += picofftsg_mult_w_w(ss, (PICOFFTSG_WGT_SHIFT2 - wkr));
        }
        if (i0 == 4) {
            break;
        }
    }

    xr = a[2] - a[n - 2];
    xi = a[3] + a[n - 1];
    yr = Mult_W_W(wdr, xr) - Mult_W_W(wdi, xi);
    yi = Mult_W_W(wdr, xi) + Mult_W_W(wdi, xr);

    a[2] -= yr;
    a[3] -= yi;
    a[n - 2] += yr;
    a[n - 1] -= yi;


}


void rftbsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, k;
    PICOFFTSG_FFTTYPE w1r, w1i, wkr, wki, wdr, wdi, ss, xr, xi, yr, yi;
    wkr = 0;
    wki = 0;
    wdi=(PICOFFTSG_FFTTYPE)(0.012270614505*PICODSP_WGT_SHIFT);
    wdr=(PICOFFTSG_FFTTYPE)(0.000150590655*PICODSP_WGT_SHIFT);
    w1r=(PICOFFTSG_FFTTYPE)(0.999698817730*PICODSP_WGT_SHIFT);
    w1i=(PICOFFTSG_FFTTYPE)(0.024541229010*PICODSP_WGT_SHIFT);
    ss=(PICOFFTSG_FFTTYPE)(0.049082458019*PICODSP_WGT_SHIFT);

    i = n >> 1;
    for (;;) {
        i0 = i - RDFT_LOOP_DIV4;
        if (i0 < 4) {
            i0 = 4;
        }
        for (j = i - 4; j >= i0; j -= 4) {
            k = n - j;
            xr = a[j + 2] - a[k - 2];
            xi = a[j + 3] + a[k - 1];
            yr = Mult_W_W(wdr, xr) + Mult_W_W(wdi, xi);
            yi = Mult_W_W(wdr, xi) - Mult_W_W(wdi, xr);
            a[j + 2] -= yr;
            a[j + 3] -= yi;
            a[k - 2] += yr;
            a[k - 1] -= yi;
            wkr += Mult_W_W(ss, wdi);
            wki += picofftsg_mult_w_w(ss, (PICOFFTSG_WGT_SHIFT2 - wdr));
            xr = a[j] - a[k];
            xi = a[j + 1] + a[k + 1];
            yr = Mult_W_W(wkr, xr) + Mult_W_W(wki, xi);
            yi = Mult_W_W(wkr, xi) - Mult_W_W(wki, xr);
            a[j] -= yr;
            a[j + 1] -= yi;
            a[k] += yr;
            a[k + 1] -= yi;
            wdr += Mult_W_W(ss, wki);
            wdi += picofftsg_mult_w_w(ss, (PICOFFTSG_WGT_SHIFT2 - wkr));
        }
        if (i0 == 4) {
            break;
        }
    }
    xr = a[2] - a[n - 2];
    xi = a[3] + a[n - 1];
    yr = Mult_W_W(wdr, xr) + Mult_W_W(wdi, xi);
    yi = Mult_W_W(wdr, xi) - Mult_W_W(wdi, xr);
    a[2] -= yr;
    a[3] -= yi;
    a[n - 2] += yr;
    a[n - 1] -= yi;
}


void dctsub(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 i, i0, j, k, m;
    PICOFFTSG_FFTTYPE w1r, w1i, wkr, wki, wdr, wdi, ss, xr, xi, yr, yi;
    wkr = (PICOFFTSG_FFTTYPE)(0.5*PICODSP_WGT_SHIFT);
    wki = (PICOFFTSG_FFTTYPE)(0.5*PICODSP_WGT_SHIFT);

    switch (n) {
        case 8 :  wdi=(PICOFFTSG_FFTTYPE)(0.587937772274*PICODSP_WGT_SHIFT);
            wdr=(PICOFFTSG_FFTTYPE)(0.392847478390*PICODSP_WGT_SHIFT); w1r=(PICOFFTSG_FFTTYPE)(0.980785250664*PICODSP_WGT_SHIFT);
            w1i=(PICOFFTSG_FFTTYPE)(0.195090323687*PICODSP_WGT_SHIFT); ss =(PICOFFTSG_FFTTYPE)(0.390180647373*PICODSP_WGT_SHIFT); break;
        case 16 : wdi=(PICOFFTSG_FFTTYPE)(0.546600937843*PICODSP_WGT_SHIFT);
            wdr=(PICOFFTSG_FFTTYPE)(0.448583781719*PICODSP_WGT_SHIFT); w1r=(PICOFFTSG_FFTTYPE)(0.995184719563*PICODSP_WGT_SHIFT);
            w1i=(PICOFFTSG_FFTTYPE)(0.098017141223*PICODSP_WGT_SHIFT); ss =(PICOFFTSG_FFTTYPE)(0.196034282446*PICODSP_WGT_SHIFT); break;
        case 32 : wdi=(PICOFFTSG_FFTTYPE)(0.523931562901*PICODSP_WGT_SHIFT);
            wdr=(PICOFFTSG_FFTTYPE)(0.474863886833*PICODSP_WGT_SHIFT); w1r=(PICOFFTSG_FFTTYPE)(0.998795449734*PICODSP_WGT_SHIFT);
            w1i=(PICOFFTSG_FFTTYPE)(0.049067676067*PICODSP_WGT_SHIFT); ss =(PICOFFTSG_FFTTYPE)(0.098135352135*PICODSP_WGT_SHIFT); break;
        case 64 : wdi=(PICOFFTSG_FFTTYPE)(0.512120008469*PICODSP_WGT_SHIFT);
            wdr=(PICOFFTSG_FFTTYPE)(0.487578809261*PICODSP_WGT_SHIFT); w1r=(PICOFFTSG_FFTTYPE)(0.999698817730*PICODSP_WGT_SHIFT);
            w1i=(PICOFFTSG_FFTTYPE)(0.024541229010*PICODSP_WGT_SHIFT); ss =(PICOFFTSG_FFTTYPE)(0.049082458019*PICODSP_WGT_SHIFT); break;
        default:
            wdr = 0; wdi = 0; ss = 0; break;
    }

    m = n >> 1;
    i = 0;
    for (;;) {
        i0 = i + DCST_LOOP_DIV2;
        if (i0 > m - 2) {
            i0 = m - 2;
        }
        for (j = i + 2; j <= i0; j += 2) {
            k = n - j;
            xr = picofftsg_mult_w_a(wdi, a[j - 1]) - picofftsg_mult_w_a(wdr, a[k + 1]);
            xi = picofftsg_mult_w_a(wdr, a[j - 1]) + picofftsg_mult_w_a(wdi, a[k + 1]);
            wkr -= Mult_W_W(ss, wdi);
            wki += Mult_W_W(ss, wdr);
            yr = Mult_W_W(wki, a[j]) - Mult_W_W(wkr, a[k]);
            yi = Mult_W_W(wkr, a[j]) + Mult_W_W(wki, a[k]);
            wdr -= Mult_W_W(ss, wki);
            wdi += Mult_W_W(ss, wkr);
            a[k + 1] = xr;
            a[k] = yr;
            a[j - 1] = xi;
            a[j] = yi;
        }
        if (i0 == m - 2) {
            break;
        }
    }
    xr = picofftsg_mult_w_a(wdi, a[m - 1]) - picofftsg_mult_w_a(wdr, a[m + 1]);
    a[m - 1] = picofftsg_mult_w_a(wdr, a[m - 1]) + picofftsg_mult_w_a(wdi, a[m + 1]);
    a[m + 1] = xr;
    a[m] = Mult_W_W(WR5000, a[m]);
}


void dctsub4(picoos_int32 n, PICOFFTSG_FFTTYPE *a)
{
    picoos_int32 m;
    PICOFFTSG_FFTTYPE wki, wdr, wdi, xr;

    wki = WR5000;
    m = n >> 1;
    if (m == 2) {
        wdr = Mult_W_W(wki, WI2500);
        wdi = Mult_W_W(wki, WR2500);
        xr = Mult_W_W(wdi, a[1]) - Mult_W_W(wdr, a[3]);
        a[1] = Mult_W_W(wdr, a[1]) + Mult_W_W(wdi, a[3]);
        a[3] = xr;
    }
    a[m] = Mult_W_W(wki, a[m]);
}

#ifdef __cplusplus
}
#endif
