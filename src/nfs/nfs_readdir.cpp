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

#include "nfs/nfs_readdir.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "dnfsd/dnfs_config.h"

#define MODULE_NAME "NFS"

int nfs3_readdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*数据指针*/
    READDIR3args *readdir_args = &arg->arg_readdir3;
    READDIR3resok *readdir_res_ok = &res->res_readdir3.READDIR3res_u.resok;
    READDIR3resfail *readdir_res_fail = &res->res_readdir3.READDIR3res_u.resfail;

    if (readdir_args->dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_readdir get dir handle len is 0");
        goto out;
    }

    get_file_handle(readdir_args->dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_readdir obtained dir handle is '%s', and the length is '%d'",
        readdir_args->dir.data.data_val,
        readdir_args->dir.data.data_len);
    out:

    return rc;
}

void nfs3_readdir_free(nfs_res_t *res) {
}

bool xdr_READDIR3args(XDR *xdrs, READDIR3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->dir))
        return FALSE;
    if (!xdr_cookie3(xdrs, &objp->cookie))
        return FALSE;
    if (!xdr_cookieverf3(xdrs, objp->cookieverf))
        return FALSE;
    if (!xdr_count3(xdrs, &objp->count))
        return FALSE;
    return TRUE;
}

bool xdr_entry3(XDR *xdrs, entry3 *objp) {
    if (!xdr_fileid3(xdrs, &objp->fileid))
        return FALSE;
    if (!xdr_filename3(xdrs, &objp->name))
        return FALSE;
    if (!xdr_cookie3(xdrs, &objp->cookie))
        return FALSE;
    if (!xdr_pointer(xdrs, (void **) &objp->nextentry, sizeof(entry3),
                     (xdrproc_t) xdr_entry3))
        return FALSE;
    return TRUE;
}

bool xdr_dirlist3(XDR *xdrs, dirlist3 *objp) {
    if (!xdr_pointer(xdrs, (void **) &objp->entries, sizeof(entry3),
                     (xdrproc_t) xdr_entry3))
        return FALSE;
    if (!xdr_bool(xdrs, &objp->eof))
        return FALSE;
    return TRUE;
}

bool xdr_READDIR3resok(XDR *xdrs, READDIR3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    if (!xdr_cookieverf3(xdrs, objp->cookieverf))
        return FALSE;
    if (!xdr_dirlist3(xdrs, &objp->reply))
        return FALSE;
    return TRUE;
}

bool xdr_READDIR3resfail(XDR *xdrs, READDIR3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return FALSE;
    return TRUE;
}

bool xdr_READDIR3res(XDR *xdrs, READDIR3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_READDIR3resok(xdrs, &objp->READDIR3res_u.resok))
                return FALSE;
            break;
        default:
            if (!xdr_READDIR3resfail(xdrs, &objp->READDIR3res_u.resfail))
                return FALSE;
            break;
    }
    return TRUE;
}
