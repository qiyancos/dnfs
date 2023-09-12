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

#ifndef DNFSD_NFS_FILE_HANDLE_H
#define DNFSD_NFS_FILE_HANDLE_H

#include "nfs/nfs_common_data.h"

/**
 *
 * @brief Test if an NFS v4 file handle is empty.
 *
 * This routine is used to test if a fh is empty (contains no data).
 *
 * @param pfh [IN] file handle to test.
 *
 * @return NFS4_OK if successful, NFS4ERR_NOFILEHANDLE is fh is empty.
 *
 */
int nfs4_Is_Fh_Empty(nfs_fh4 *pfh);

#endif //DNFSD_NFS_FILE_HANDLE_H
