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

#ifndef DNFSD_NFS_UTILS_H
#define DNFSD_NFS_UTILS_H

#include <sys/types.h>

#include "nfs/nfs_common_data.h"

#define UTF8_SCAN_NONE    0x00	/* do no validation other than size */
#define UTF8_SCAN_NOSLASH 0x01	/* disallow '/' */
#define UTF8_SCAN_NODOT   0x02	/* disallow '.' and '..' */
#define UTF8_SCAN_CKUTF8  0x04	/* validate utf8 */
#define UTF8_SCAN_PATH    0x10	/* validate path length */

void copy_tag(utf8str_cs *dest, utf8str_cs *src);

inline utf8string * utf8string_dup(utf8string *d, const char *s, size_t l);

inline nfsstat4 nfs4_utf8string_scan(const utf8string *input, int scan);

#endif //DNFSD_NFS_UTILS_H
