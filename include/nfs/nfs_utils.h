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
#include "nfs/nfs_compound_base.h"

void copy_tag(utf8str_cs *dest, utf8str_cs *src);

inline utf8string * utf8string_dup(utf8string *d, const char *s, size_t l);

inline nfsstat4 nfs4_utf8string_scan(const utf8string *input, int scan);

void now(struct timespec *ts);

nfsstat4 check_resp_room(compound_data_t *data, uint32_t op_resp_size);

#endif //DNFSD_NFS_UTILS_H
