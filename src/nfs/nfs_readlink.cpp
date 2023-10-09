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
#include "nfs/nfs_readlink.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_readlink(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    if (arg->arg_access3.object.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "nfs_access get file handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_access3.object);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_access obtained file handle is '%s', and the length is '%d'",
        arg->arg_access3.object.data.data_val,
        arg->arg_access3.object.data.data_len);
    out:

    return rc;
}

void nfs3_readlink_free(nfs_res_t *res) {
/*    if (res->res_readlink3.status == NFS3_OK)
        free(res->res_readlink3.READLINK3res_u.resok.data);*/
}

bool xdr_READLINK3args(XDR *xdrs, READLINK3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->symlink))
        return (false);
    return (true);
}

bool xdr_READLINK3resok(XDR *xdrs, READLINK3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->symlink_attributes))
        return (false);
    if (!xdr_nfspath3(xdrs, &objp->data))
        return (false);
    return (true);
}

bool xdr_READLINK3resfail(XDR *xdrs, READLINK3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->symlink_attributes))
        return (false);
    return (true);
}

bool xdr_READLINK3res(XDR *xdrs, READLINK3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_READLINK3resok(xdrs, &objp->READLINK3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_READLINK3resfail(xdrs, &objp->READLINK3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
