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
/**
 * @brief Start a duplicate request transaction
 *
 * Finds any matching request entry in the cache, if one exists, else
 * creates one in the START state.  On any non-error return, the refcnt
 * of the corresponding entry is incremented.
 *
 * @param[in] reqnfs  The NFS request data
 *
 * @retval DUPREQ_SUCCESS if successful.
 */
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "rpc/svc_auth.h"

#define MODULE_NAME "DNFS"

void nfs_dupreq_rele(nfs_request_t *reqnfs)
{

        LOG(MODULE_NAME,L_INFO, "releasing res %p",
                     reqnfs->svc.rq_u2);
        reqnfs->funcdesc->free_function(
                static_cast<nfs_res_t *>(reqnfs->svc.rq_u2));
        free_nfs_res((nfs_res_t *)reqnfs->svc.rq_u2);


    if (reqnfs->svc.rq_auth)
        SVCAUTH_RELEASE(&reqnfs->svc);
}


dupreq_status_t nfs_dupreq_start(nfs_request_t *reqnfs) {
    auto *p_ =(nfs_res_t *) malloc(sizeof(nfs_res_t));
        if (nullptr == p_) {
        LOG(MODULE_NAME, L_ERROR, "Request '%s' result failed to allocate memory",reqnfs->funcdesc->funcname);
        return DUPREQ_DROP;
    }
    reqnfs->res_nfs= p_;
    reqnfs->svc.rq_u2 = p_;
    return DUPREQ_SUCCESS;
}