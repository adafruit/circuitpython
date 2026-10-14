// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018-2019 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2026 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// TF-PSA-Crypto configuration, selected with TF_PSA_CRYPTO_CONFIG_FILE.
//
// mbedtls 4.0 split its configuration in two: MBEDTLS_CONFIG_FILE now covers only
// TLS and X.509 (see mbedtls_config.h next to this file), and everything
// cryptographic moved here. Algorithms and key types are requested with PSA_WANT_*
// rather than the old MBEDTLS_<alg>_C module switches; the builtin implementations
// they need are derived from those by tf-psa-crypto's own config_adjust headers.

#pragma once

// Platform integration ///////////////////////////////////////////////////////

#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS
#define MBEDTLS_DEPRECATED_REMOVED

// Memory allocation hooks, so mbedtls allocations are tracked like the rest of the
// VM's.
#include <stdlib.h>
#include <stdio.h>
void *m_tracked_calloc(size_t nmemb, size_t size);
void m_tracked_free(void *ptr);
#define MBEDTLS_PLATFORM_STD_CALLOC m_tracked_calloc
#define MBEDTLS_PLATFORM_STD_FREE m_tracked_free
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO snprintf

// Randomness /////////////////////////////////////////////////////////////////

// Take randomness straight from the port's TRNG rather than seeding mbedtls's own
// entropy accumulator and CTR-DRBG. mbedtls 4.x dropped MBEDTLS_ENTROPY_HARDWARE_ALT,
// which is how this was wired before, and going direct also keeps entropy.c and
// ctr_drbg.c out of the build. mbedtls_psa_external_get_random() is in
// mbedtls_port.c.
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

// Size/speed tradeoffs ///////////////////////////////////////////////////////

#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_AES_FEWER_TABLES
#define MBEDTLS_SHA256_SMALLER
#define MBEDTLS_ECP_NIST_OPTIM

// Hashes /////////////////////////////////////////////////////////////////////

// SHA-1 is still needed to parse certificates that sign with it, and is what
// hashlib exposes as "sha1".
#define PSA_WANT_ALG_SHA_1
#define PSA_WANT_ALG_SHA_224
#define PSA_WANT_ALG_SHA_256
#define PSA_WANT_ALG_SHA_384
#define PSA_WANT_ALG_SHA_512
#define PSA_WANT_ALG_HMAC
#define PSA_WANT_KEY_TYPE_HMAC

// Key derivation /////////////////////////////////////////////////////////////

// The TLS 1.2 key schedule. HKDF is deliberately absent: it is the TLS 1.3 key
// schedule, and mbedtls_config.h enables TLS 1.2 only.
#define PSA_WANT_ALG_TLS12_PRF
#define PSA_WANT_ALG_TLS12_PSK_TO_MS
#define PSA_WANT_KEY_TYPE_DERIVE
#define PSA_WANT_KEY_TYPE_RAW_DATA

// Bulk ciphers ///////////////////////////////////////////////////////////////

#define PSA_WANT_KEY_TYPE_AES
#define PSA_WANT_ALG_GCM
#define PSA_WANT_ALG_CCM
#define PSA_WANT_ALG_CBC_NO_PADDING
#define PSA_WANT_ALG_CBC_PKCS7
#define PSA_WANT_ALG_ECB_NO_PADDING

// RP2 has no AES accelerator, so ChaCha20-Poly1305 is 40-70% faster than AES-GCM here,
// measured over HTTPS on Pico W and Pico 2 W. mbedtls offers it ahead of AES-GCM, and
// real-world CDNs do negotiate it. Costs about 6 kB. espressif leaves it off, since
// ESP32 has AES hardware.
#define PSA_WANT_KEY_TYPE_CHACHA20
#define PSA_WANT_ALG_CHACHA20_POLY1305

// Public key /////////////////////////////////////////////////////////////////

#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN
#define PSA_WANT_ALG_RSA_PSS
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT

#define PSA_WANT_ALG_ECDH
#define PSA_WANT_ALG_ECDSA
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT
// An ECDHE handshake generates an ephemeral key pair, sends its public part, then
// runs the key agreement, so it needs GENERATE, EXPORT and DERIVE as well as the two
// above. Without them psa_generate_key() fails and the TLS 1.2 client reports
// MBEDTLS_ERR_SSL_HW_ACCEL_FAILED -- a legacy name that in 4.x just means a PSA call
// failed. See ssl_tls12_client.c.
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE

// P-256 and P-384 cover essentially every public CA. Curve25519 is here for x25519
// ECDHE, which servers commonly prefer (RFC 8422). Deliberately absent:
//  - P-521: no public CA issues from it, and it cost 5792 bytes on Pico W.
//  - Brainpool and secp256k1: unused on the public web. espressif enables secp256k1
//    only because ESP-IDF defaults it on.
// The 192- and 224-bit curves the mbedtls 2.28 config enabled are gone from 4.x
// upstream; there is no PSA_WANT_ECC_SECP_R1_192/_224 to select.
#define PSA_WANT_ECC_SECP_R1_256
#define PSA_WANT_ECC_SECP_R1_384
#define PSA_WANT_ECC_MONTGOMERY_255

// Key and certificate parsing ////////////////////////////////////////////////

#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_WRITE_C
#define MBEDTLS_MD_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
