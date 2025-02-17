// Copyright 2023 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** \file sha256.h
 *  \brief SHA256 implementation
 *
 *  This contains an implementation of the SHA256 algorithm
 *  It was originally copied from Brad Conte
 *  https://github.com/B-Con/crypto-algorithms/blob/master/sha256.c
 *  and modified to meet ros2 code formatting and compiler warning requirements.
 *  Algorithm specification can be found here:
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2withchangenotice.pdf
 *  This implementation uses little endian byte order.
 *  This implementation makes no security guarantees, its use case if for
 *  non-sensitive comparison of message digests.
 */

#ifndef RCUTILS__SHA256_H_
#define RCUTILS__SHA256_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>

#include "rcutils/visibility_control.h"

#define RCUTILS_SHA256_BLOCK_SIZE 32

typedef struct RCUTILS_PUBLIC_TYPE rcutils_sha256_ctx_s
{
  uint8_t data[64];
  size_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
} rcutils_sha256_ctx_t;

/// Initialize the sha256 algorithm context with starting state.
/**
 * Call this on any new context before starting to input data.
 *
 * \param[inout] ctx
 * \return void
 */
RCUTILS_PUBLIC
void rcutils_sha256_init(rcutils_sha256_ctx_t * ctx);

/// Add data to the sha256 algorithm
/**
 * This may be called repeatedly on an initialized context.
 *
 * \param[inout] ctx Initialized sha256 context struct
 * \param[in] data Data to add to the total message being hashed
 * \param[in] data_len Size of the input data.
 * \return void
 */
RCUTILS_PUBLIC
void rcutils_sha256_update(rcutils_sha256_ctx_t * ctx, const uint8_t * data, size_t data_len);

/// Finalize and output sha256 hash for all data added.
/**
 * Call only once on a context that has been initialized, and optionally updated with data.
 *
 * \param[inout] ctx Initialized sha256 context struct
 * \param[out] output_hash Calculated sha256 message digest to be filled
 * \return void
 */
#ifdef DOXYGEN_ONLY
// One of the tools used by rosdoc2 misunderstands uint8_t[] as a uint8_t,
// so make it a pointer for documentation purposes.
RCUTILS_PUBLIC
void rcutils_sha256_final(
  rcutils_sha256_ctx_t * ctx,
  uint8_t * output_hash);
#else
RCUTILS_PUBLIC
void rcutils_sha256_final(
  rcutils_sha256_ctx_t * ctx,
  uint8_t output_hash[RCUTILS_SHA256_BLOCK_SIZE]);
#endif

#ifdef __cplusplus
}
#endif

#endif  // RCUTILS__SHA256_H_
