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
#include "dnfsd/dnfs_meta_data.h"
typedef enum dupreq_status {
    DUPREQ_SUCCESS = 0,
    DUPREQ_BEING_PROCESSED,
    DUPREQ_EXISTS,
    DUPREQ_DROP,
} dupreq_status_t;

static inline void free_nfs_res(nfs_res_t *res)
{
    free(res);
}

void nfs_dupreq_rele(nfs_request_t *reqnfs);

dupreq_status_t nfs_dupreq_start(nfs_request_t *);
#endif //DNFSD_NFS_UTILS_H
