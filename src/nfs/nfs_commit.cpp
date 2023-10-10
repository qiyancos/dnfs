/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *              Piyuyang pi_yuyang@163.com
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
#include "nfs/nfs_commit.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_commit(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*数据指针*/
    COMMIT3args *commit_args = &arg->arg_commit3;
    COMMIT3resok *commit_res_ok = &res->res_commit3.COMMIT3res_u.resok;
    COMMIT3resfail *commit_res_fail = &res->res_commit3.COMMIT3res_u.resfail;

    if (commit_args->file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_commit get dir handle len is 0");
        goto out;
    }

    get_file_handle(commit_args->file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_commit obtained file handle is '%s', and the length is '%d'",
        commit_args->file.data.data_val,
        commit_args->file.data.data_len);
    out:

    return rc;
}

void nfs3_commit_free(nfs_res_t *res) {
}

bool xdr_COMMIT3args(XDR *xdrs, COMMIT3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return FALSE;
    if (!xdr_offset3(xdrs, &objp->offset))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->count))
        return FALSE;
    return TRUE;
}

bool xdr_COMMIT3resok(XDR *xdrs, COMMIT3resok *objp) {
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return FALSE;
    if (!xdr_writeverf3(xdrs, objp->verf))
        return FALSE;
    return TRUE;
}

bool xdr_COMMIT3resfail(XDR *xdrs, COMMIT3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_COMMIT3res(XDR *xdrs, COMMIT3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_COMMIT3resok(xdrs, &objp->COMMIT3res_u.resok))
                return FALSE;
            break;
        default:
            if (!xdr_COMMIT3resfail(xdrs, &objp->COMMIT3res_u.resfail))
                return FALSE;
            break;
    }
    return TRUE;
}