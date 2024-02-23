/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include "log/log.h"
#include "nfs/nfs_null.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs_null([[maybe_unused]] nfs_arg_t *arg,
             [[maybe_unused]] struct svc_req *req,
             [[maybe_unused]] nfs_res_t *res) {
    LOG(MODULE_NAME, D_INFO, "REQUEST PROCESSING: Calling NFS_NULL");
    return 0;
}

void nfs_null_free([[maybe_unused]] nfs_res_t *res) {
    /* Nothing to do here */
}