// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
// Copyright 2022 Jeff Epler for Adafruit Industries
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define BUNDLE_MAX_CERTS (200)

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "mbedtls/version.h"
#include "mbedtls/x509_crt.h"
#include "lib/mbedtls_config/crt_bundle.h"

#define BUNDLE_HEADER_OFFSET 2
#define CRT_HEADER_OFFSET 4
// Each certificate is stored with the first NAME_HASH_LEN bytes of the SHA-256 of its DER subject
// name instead of the name itself (tools/gen_crt_bundle.py). The hash only selects the candidate
// root; the signature check decides.
#define NAME_HASH_LEN 8

/* a dummy certificate so that
 * cacert_ptr passes non-NULL check during handshake */
static mbedtls_x509_crt s_dummy_crt;

#define TAG "x509-crt-bundle"

#define LOGE(tag, fmt, ...) mp_printf(&mp_plat_print, tag ":" fmt "\n",##__VA_ARGS__)
#if 0
#define LOGI(tag, fmt, ...) mp_printf(&mp_plat_print, tag ":" fmt "\n",##__VA_ARGS__)
#define LOGD(tag, fmt, ...) mp_printf(&mp_plat_print, tag ":" fmt "\n",##__VA_ARGS__)
#else
#define LOGI(tag, fmt, ...) do {} while (0)
#define LOGD(tag, fmt, ...) do {} while (0)
#endif

extern const uint8_t x509_crt_imported_bundle_bin_start[] __asm__ ("_binary_x509_crt_bundle_start");
extern const uint8_t x509_crt_imported_bundle_bin_end[]   __asm__ ("_binary_x509_crt_bundle_end");


typedef struct crt_bundle_t {
    const uint8_t **crts;
    uint16_t num_certs;
    size_t x509_crt_bundle_len;
} crt_bundle_t;

static crt_bundle_t s_crt_bundle;

static int crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len);


static int crt_check_signature(mbedtls_x509_crt *child, const uint8_t *pub_key_buf, size_t pub_key_len) {
    int ret = 0;
    mbedtls_x509_crt parent;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    mbedtls_x509_crt_init(&parent);

    if ((ret = mbedtls_pk_parse_public_key(&parent.pk, pub_key_buf, pub_key_len)) != 0) {
        LOGE(TAG, "PK parse failed with error %X", ret);
        goto cleanup;
    }


    md_info = mbedtls_md_info_from_type(child->MBEDTLS_PRIVATE(sig_md));
    if ((ret = mbedtls_md(md_info, child->tbs.p, child->tbs.len, hash)) != 0) {
        LOGE(TAG, "Internal mbedTLS error %X", ret);
        goto cleanup;
    }

    if ((ret = mbedtls_pk_verify_ext(
        child->MBEDTLS_PRIVATE(sig_pk), &parent.pk,
        child->MBEDTLS_PRIVATE(sig_md), hash, mbedtls_md_get_size(md_info),
        child->MBEDTLS_PRIVATE(sig).p, child->MBEDTLS_PRIVATE(sig).len)) != 0) {
        LOGE(TAG, "PK verify failed with error %X", ret);
        goto cleanup;
    }
cleanup:
    mbedtls_x509_crt_free(&parent);

    return ret;
}


static unsigned char s_name_hash[32];

/* Index of the first bundled root whose name hash matches `name`, or -1. Leaves the hash in
 * s_name_hash for crt_name_hash_equal(). */
static int crt_bundle_find(const mbedtls_x509_buf *name) {
    if (mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), name->p, name->len, s_name_hash) != 0) {
        return -1;
    }
    int start = 0;
    int end = s_crt_bundle.num_certs - 1;
    while (start <= end) {
        int middle = (start + end) / 2;
        int cmp_res = memcmp(s_name_hash, s_crt_bundle.crts[middle] + CRT_HEADER_OFFSET, NAME_HASH_LEN);
        if (cmp_res == 0) {
            while (middle > 0 &&
                   memcmp(s_name_hash, s_crt_bundle.crts[middle - 1] + CRT_HEADER_OFFSET, NAME_HASH_LEN) == 0) {
                middle--;
            }
            return middle;
        } else if (cmp_res < 0) {
            end = middle - 1;
        } else {
            start = middle + 1;
        }
    }
    return -1;
}

static bool crt_name_hash_equal(int i) {
    return memcmp(s_name_hash, s_crt_bundle.crts[i] + CRT_HEADER_OFFSET, NAME_HASH_LEN) == 0;
}

/* This callback is called for every certificate in the chain. If the chain
 * is proper each intermediate certificate is validated through its parent
 * in the x509_crt_verify_chain() function. So this callback should
 * only verify the first untrusted link in the chain is signed by the
 * root certificate in the trusted bundle
*/
static int crt_verify_callback(void *buf, mbedtls_x509_crt *crt, int depth, uint32_t *flags) {
    mbedtls_x509_crt *child = crt;

    /* It's OK for a trusted cert to have a weak signature hash alg.
       as we already trust this certificate */
    uint32_t flags_filtered = *flags & ~(MBEDTLS_X509_BADCERT_BAD_MD);

    if (flags_filtered != MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
        return 0;
    }


    if (s_crt_bundle.crts == NULL) {
        LOGE(TAG, "No certificates in bundle");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    LOGD(TAG, "%d certificates in bundle", s_crt_bundle.num_certs);

    /* A chain can carry a cross-signed copy of a root that is in the bundle, issued by a root
     * that is not (GTS Root R4 signed by the retired GlobalSign Root CA). Trust that copy when
     * its subject and public key match the bundled root. */
    int i = crt_bundle_find(&child->subject_raw);
    for (; i >= 0 && i < s_crt_bundle.num_certs && crt_name_hash_equal(i); i++) {
        size_t key_len = s_crt_bundle.crts[i][2] << 8 | s_crt_bundle.crts[i][3];
        if (key_len == child->pk_raw.len &&
            memcmp(s_crt_bundle.crts[i] + CRT_HEADER_OFFSET + NAME_HASH_LEN, child->pk_raw.p, key_len) == 0) {
            *flags = 0;
            return 0;
        }
    }

    /* Otherwise the root that issued this certificate must verify its signature */
    int ret = MBEDTLS_ERR_X509_FATAL_ERROR;
    i = crt_bundle_find(&child->issuer_raw);
    for (; i >= 0 && i < s_crt_bundle.num_certs && ret != 0 && crt_name_hash_equal(i); i++) {
        size_t key_len = s_crt_bundle.crts[i][2] << 8 | s_crt_bundle.crts[i][3];
        ret = crt_check_signature(child, s_crt_bundle.crts[i] + CRT_HEADER_OFFSET + NAME_HASH_LEN, key_len);
    }

    if (ret == 0) {
        LOGI(TAG, "Certificate validated");
        *flags = 0;
        return 0;
    }

    LOGE(TAG, "Failed to verify certificate");
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}


/* Initialize the bundle into an array so we can do binary search for certs,
   the bundle generated by the python utility is already presorted by subject name
 */
static int crt_bundle_init(const uint8_t *x509_bundle, size_t bundle_size) {
    if (bundle_size < BUNDLE_HEADER_OFFSET + CRT_HEADER_OFFSET) {
        LOGE(TAG, "Invalid certificate bundle");
        return -MP_EINVAL;
    }

    uint16_t num_certs = (x509_bundle[0] << 8) | x509_bundle[1];
    if (num_certs > BUNDLE_MAX_CERTS) {
        // No. of certs in the certificate bundle = %d exceeds\n"
        // Max allowed certificates in the certificate bundle = %d\n"
        // Please update the menuconfig option with appropriate value", num_certs, BUNDLE_MAX_CERTS
        return -MP_E2BIG;
    }

    const uint8_t **crts = m_tracked_calloc(num_certs, sizeof(x509_bundle));
    if (crts == NULL) {
        LOGE(TAG, "Unable to allocate memory for bundle");
        return -MP_ENOMEM;
    }

    const uint8_t *cur_crt;
    /* This is the maximum region that is allowed to access */
    const uint8_t *bundle_end = x509_bundle + bundle_size;
    cur_crt = x509_bundle + BUNDLE_HEADER_OFFSET;

    for (int i = 0; i < num_certs; i++) {
        crts[i] = cur_crt;
        if (cur_crt + CRT_HEADER_OFFSET > bundle_end) {
            LOGE(TAG, "Invalid certificate bundle");
            m_tracked_free(crts);
            return -MP_EINVAL;
        }
        size_t name_len = cur_crt[0] << 8 | cur_crt[1];
        size_t key_len = cur_crt[2] << 8 | cur_crt[3];
        if (name_len != NAME_HASH_LEN) {
            LOGE(TAG, "Invalid certificate bundle");
            m_tracked_free(crts);
            return -MP_EINVAL;
        }
        cur_crt = cur_crt + CRT_HEADER_OFFSET + name_len + key_len;
    }

    if (cur_crt > bundle_end) {
        LOGE(TAG, "Invalid certificate bundle");
        m_tracked_free(crts);
        return -MP_EINVAL;
    }

    /* The previous crt bundle is only updated when initialization of the
     * current crt_bundle is successful */
    /* Free previous crt_bundle */
    m_tracked_free(s_crt_bundle.crts);
    s_crt_bundle.num_certs = num_certs;
    s_crt_bundle.crts = crts;
    return 0;
}

int crt_bundle_attach(mbedtls_ssl_config *ssl_conf) {
    int ret = 0;
    // If no bundle has been set by the user then use the bundle embedded in the binary
    if (s_crt_bundle.crts == NULL) {
        ret = crt_bundle_init(x509_crt_imported_bundle_bin_start, x509_crt_imported_bundle_bin_end - x509_crt_imported_bundle_bin_start);
    }

    if (ret != 0) {
        return ret;
    }

    if (ssl_conf) {
        /* point to a dummy certificate
         * This is only required so that the
         * cacert_ptr passes non-NULL check during handshake
         */
        mbedtls_x509_crt_init(&s_dummy_crt);
        mbedtls_ssl_conf_ca_chain(ssl_conf, &s_dummy_crt, NULL);
        mbedtls_ssl_conf_verify(ssl_conf, crt_verify_callback, NULL);
    }

    return ret;
}

void crt_bundle_detach(mbedtls_ssl_config *conf) {
    m_tracked_free(s_crt_bundle.crts);
    s_crt_bundle.crts = NULL;
    if (conf) {
        mbedtls_ssl_conf_verify(conf, NULL, NULL);
    }
}

int crt_bundle_set(const uint8_t *x509_bundle, size_t bundle_size) {
    return crt_bundle_init(x509_bundle, bundle_size);
}
