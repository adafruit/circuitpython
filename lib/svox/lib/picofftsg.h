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
 * @file picofftsg.h
 *
 * include file for FFT/DCT related data types, constants and functions in Pico
 *
 * Copyright (C) 2008-2009 SVOX AG, Baslerstr. 30, 8048 Zuerich, Switzerland
 * All rights reserved.
 *
 * History:
 * - 2009-04-20 -- initial version
 *
*/
#ifndef PICOFFTSG_H_
#define PICOFFTSG_H_

#include "picoos.h"
#include "picodsp.h"


#ifdef __cplusplus
extern "C" {

#endif
#if 0
}
#endif

#define PICOFFTSG_FFTTYPE picoos_int32

extern void rdft(int n, int isgn, PICOFFTSG_FFTTYPE *a);
extern void dfct(int n, float *a, int VAL_SHIFT);
extern void dfct_nmf(int n, int *a);
extern float norm_result(int m2, PICOFFTSG_FFTTYPE *tmpX, PICOFFTSG_FFTTYPE *norm_window);

#ifdef __cplusplus
}
#endif

#endif
