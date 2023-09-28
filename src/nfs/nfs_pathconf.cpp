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
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_pathconf(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    PATHCONF3resfail *resfail = &res->res_pathconf3.PATHCONF3res_u.resfail;
    PATHCONF3resok *resok = &res->res_pathconf3.PATHCONF3res_u.resok;

    LOG(MODULE_NAME, L_INFO, "The value of the nfs_pathconf obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_pathconf3.object.data.data_val,
        arg->arg_pathconf3.object.data.data_len);

    /* to avoid setting it on each error case */
    resfail->obj_attributes.attributes_follow = FALSE;

    if (arg->arg_pathconf3.object.data.data_val == nullptr) {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME,L_ERROR,
            "nfs_pathconf get file handle is null");
        goto out;
    }

    res->res_pathconf3.status =nfs_set_post_op_attr(arg->arg_pathconf3.object.data.data_val, &res->res_pathconf3.PATHCONF3res_u.resok.obj_attributes);
    if (res->res_pathconf3.status!=NFS3_OK)
    {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "'stat %s' failed",
            arg->arg_pathconf3.object.data.data_val);
        goto out;
    }

    resok->linkmax = 1024;
    resok->name_max = NAME_MAX;
    resok->no_trunc = true;
    resok->chown_restricted = true;
    resok->case_insensitive = true;
    resok->case_preserving = true;

    res->res_pathconf3.status = NFS3_OK;

    out:
    return rc;
}

void nfs3_pathconf_free(nfs_res_t *res) {
    /* Nothing to do here */
}
