/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

extern "C" {
#include "intrinsic.h"
}

#include <string.h>
#include <dirent.h>
#include <assert.h>
#include <sys/param.h>

#include "nfs/nfs_utils.h"
#include "log/log.h"

#define MODULE_NAME "DNFS"

void copy_tag(utf8str_cs *dest, utf8str_cs *src)  {
    utf8string_dup(dest, src->utf8string_val, src->utf8string_len);
}

inline utf8string * utf8string_dup(utf8string *d, const char *s, size_t l) {
    if (s == NULL || l == 0) {
        d->utf8string_val = 0;
        d->utf8string_len = 0;
        return d;
    }

    d->utf8string_val = (char *)malloc(l + 1);

    if (d->utf8string_val == NULL) {
        LOG(MODULE_NAME, L_ERROR, "utf8string_dup malloc failed");
        abort();
    }
    d->utf8string_len = l;
    memcpy(d->utf8string_val, s, l + 1);
    return d;
}

/** path_filter
 * scan the path we are given for bad filenames.
 *
 * scan control:
 *    UTF8_SCAN_NOSLASH - detect and reject '/' in names
 *    UTF8_NODOT - detect and reject "." and ".." as the name
 *    UTF8_SCAN_CKUTF8 - detect invalid utf8 sequences
 *
 * NULL termination is required.  It also speeds up the scan
 * UTF-8 scanner courtesy Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/>
 * GPL licensed per licensing referenced in source.
 *
 */
nfsstat4 path_filter(const char *name, int scan)
{
    const unsigned char *np = (const unsigned char *)name;
    nfsstat4 status = NFS4_OK;
    unsigned int c, first;

    first = 1;
    c = *np++;
    while (c) {
        if (likely(c < 0x80)) {
            /* ascii */
            if (unlikely(c == '/' && (scan & UTF8_SCAN_NOSLASH))) {
                status = NFS4ERR_BADCHAR;
                goto error;
            }
            if (unlikely
                    (first && c == '.' && (scan & UTF8_SCAN_NODOT))) {
                if (np[0] == '\0'
                    || (np[0] == '.' && np[1] == '\0')) {
                    status = NFS4ERR_BADNAME;
                    goto error;
                }
            }
        } else if (likely(scan & UTF8_SCAN_CKUTF8)) {
            /* UTF-8 range */
            if ((c & 0xe0) == 0xc0) {
                /* 2 octet UTF-8 */
                if ((*np & 0xc0) != 0x80 ||
                    (c & 0xfe) == 0xc0) {
                    /* overlong */
                    goto badutf8;
                } else {
                    np++;
                }
            } else if ((c & 0xf0) == 0xe0) {
                /* 3 octet UTF-8 */
                if (/* overlong */
                        (*np & 0xc0) != 0x80 ||
                        (np[1] & 0xc0) != 0x80 ||
                        (c == 0xe0 && (*np & 0xe0) == 0x80) ||
                        /* surrogate */
                        (c == 0xed && (*np & 0xe0) == 0xa0) ||
                        (c == 0xef && *np == 0xbf &&
                         (np[1] & 0xfe) == 0xbe)) {
                    /* U+fffe - u+ffff */
                    goto badutf8;
                } else {
                    np += 2;
                }
            } else if ((c & 0xf8) == 0xf0) {
                /* 4 octet UTF-8 */
                if (/* overlong */
                        (*np & 0xc0) != 0x80 ||
                        (np[1] & 0xc0) != 0x80 ||
                        (np[2] & 0xc0) != 0x80 ||
                        (c == 0xf0 && (*np & 0xf0) == 0x80) ||
                        (c == 0xf4 && *np > 0x8f) || c > 0xf4) {
                    /* > u+10ffff */
                    goto badutf8;
                } else {
                    np += 3;
                }
            } else {
                goto badutf8;
            }
        }
        c = *np++;
        first = 0;
    }
    return NFS4_OK;

    badutf8:
    status = NFS4ERR_INVAL;
    error:
    return status;
}

inline nfsstat4 nfs4_utf8string_scan(const utf8string *input, int scan) {
    if (input->utf8string_val == NULL || input->utf8string_len == 0)
        return NFS4ERR_INVAL;

    if (((scan & UTF8_SCAN_PATH) && input->utf8string_len > MAXPATHLEN) ||
        (!(scan & UTF8_SCAN_PATH) && input->utf8string_len > MAXNAMLEN))
        return NFS4ERR_NAMETOOLONG;

    /* Nothing else to do */
    if (scan == UTF8_SCAN_NONE || scan == UTF8_SCAN_PATH)
        return NFS4_OK;

    /* utf8strings are now NUL terminated, so it's ok to call the filter */
    return path_filter(input->utf8string_val, scan);
}

inline void now(struct timespec *ts) {
    int rc;

    rc = clock_gettime(CLOCK_REALTIME, ts);
    if (rc != 0) {
        LOG(MODULE_NAME, L_ERROR, "Failed to get timestamp");
        assert(0);	/* if this is broken, we are toast so die */
    }
}

nfsstat4 check_resp_room(compound_data_t *data, uint32_t op_resp_size) {
    nfsstat4 status;
    uint32_t test_response_size = data->resp_size +
                                  sizeof(nfs_opnum4) + op_resp_size +
                                  sizeof(nfs_opnum4) + sizeof(nfsstat4);

    if (data->minorversion == 0 || data->session == NULL) {
        if (test_response_size > default_max_resp_room()) {
            return NFS4ERR_RESOURCE;
        } else {
            return NFS4_OK;
        }
    }

    /* Check that op_resp_size plus nfs_opnum4 plus at least another
     * response (nfs_opnum4 plus nfsstat4) fits.
     */
    if (test_response_size >
        data->session->fore_channel_attrs.ca_maxresponsesize) {
        /* Response is larger than maximum response size. */
        status = NFS4ERR_REP_TOO_BIG;
        goto err;
    }

    if (!data->sa_cachethis) {
        /* Response size is ok, and not cached. */
        goto ok;
    }

    /* Check that op_resp_size plus nfs_opnum4 plus at least another
     * response (nfs_opnum4 plus nfsstat4) fits.
     */
    if (test_response_size >
        data->session->fore_channel_attrs.ca_maxresponsesize_cached) {
        /* Response is too big to cache. */
        status = NFS4ERR_REP_TOO_BIG_TO_CACHE;
        goto err;
    }

    /* Response is ok to cache. */

    ok:

    LogFullDebug(COMPONENT_NFS_V4,
                 "Status of %s in position %d is ok so far, op response size = %"
    PRIu32" total response size would be = %"PRIu32
            " out of max %"PRIu32"/%"PRIu32,
            data->opname, data->oppos,
            op_resp_size, test_response_size,
            data->session->fore_channel_attrs.ca_maxresponsesize,
            data->session->fore_channel_attrs.ca_maxresponsesize_cached);

    return NFS4_OK;

    err:

    LogDebug(COMPONENT_NFS_V4,
             "Status of %s in position %d is %s, op response size = %"
    PRIu32" total response size would have been = %"PRIu32
            " out of max %"PRIu32"/%"PRIu32,
            data->opname, data->oppos,
            nfsstat4_to_str(status),
            op_resp_size, test_response_size,
            data->session->fore_channel_attrs.ca_maxresponsesize,
            data->session->fore_channel_attrs.ca_maxresponsesize_cached);

    return status;
}