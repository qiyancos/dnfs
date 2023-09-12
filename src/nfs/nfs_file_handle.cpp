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

#include "log/log.h"
#include "nfs/nfs_file_handle.h"

#define MODULE_NAME "DNFS"

int nfs4_Is_Fh_Empty(nfs_fh4 *pfh) {
    if (pfh == nullptr) {
        LOG(MODULE_NAME, L_ERROR, "INVALID HANDLE: pfh=NULL");
        return NFS4ERR_NOFILEHANDLE;
    }

    if (pfh->nfs_fh4_len == 0) {
        LOG(MODULE_NAME, L_ERROR, "INVALID HANDLE: empty");
        return NFS4ERR_NOFILEHANDLE;
    }

    return NFS4_OK;
}