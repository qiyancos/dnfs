/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "mnt/mnt_umntAll.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "MNT"

int mnt_umntAll(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    LOG(MODULE_NAME, D_INFO, "REQUEST PROCESSING: Calling MNT_UMNTALL");
    return NFS_REQ_OK;
}

void mnt_umntAll_free(nfs_res_t *res) {
    /* Nothing to do */
}
