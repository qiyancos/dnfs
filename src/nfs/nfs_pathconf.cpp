/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "nfs/nfs_pathconf.h"
#include "nfs/nfs_base.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_pathconf(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    PATHCONF3resfail *resfail = &res->res_pathconf3.PATHCONF3res_u.resfail;
    PATHCONF3resok *resok = &res->res_pathconf3.PATHCONF3res_u.resok;

    LOG(MODULE_NAME, L_INFO, "pathconf file handle %s", &arg->arg_pathconf3.object.data.data_val);

    nfs_fh3 *file_f= &arg->arg_pathconf3.object;

    /* to avoid setting it on each error case */
    resfail->obj_attributes.attributes_follow = FALSE;

    if (file_f == nullptr) {
        /* Status and rc have been set by nfs3_FhandleToCache */
        goto out;
    }

//    resok->linkmax = exp_hdl->exp_ops.fs_maxlink(exp_hdl);
//    resok->name_max = exp_hdl->exp_ops.fs_maxnamelen(exp_hdl);
//    resok->no_trunc = exp_hdl->exp_ops.fs_supports(exp_hdl, fso_no_trunc);
//    resok->chown_restricted =
//            exp_hdl->exp_ops.fs_supports(exp_hdl, fso_chown_restricted);
//    resok->case_insensitive =
//            exp_hdl->exp_ops.fs_supports(exp_hdl, fso_case_insensitive);
//    resok->case_preserving =
//            exp_hdl->exp_ops.fs_supports(exp_hdl, fso_case_preserving);

    out:
    return rc;
}

void nfs3_pathconf_free(nfs_res_t *res) {
    /* Nothing to do here */
}
