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
#include "nfs/nfs_mknod.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_mknod(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*数据指针*/
    MKNOD3args *mknod_args = &arg->arg_mknod3;
    MKNOD3resok *mknod_res_ok = &res->res_mknod3.MKNOD3res_u.resok;
    MKNOD3resfail *mknod_res_fail = &res->res_mknod3.MKNOD3res_u.resfail;

    if (mknod_args->where.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_mknod get dir handle len is 0");
        goto out;
    }

    get_file_handle(mknod_args->where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_mknod obtained dir handle is '%s', and the length is '%d'",
        mknod_args->where.dir.data.data_val,
        mknod_args->where.dir.data.data_len);
    out:

    return rc;
}

void nfs3_mknod_free(nfs_res_t *res) {
/*    nfs_fh3 *handle =
            &res->res_mknod3.MKNOD3res_u.resok.obj.post_op_fh3_u.handle;

    if ((res->res_mknod3.status == NFS3_OK)
        && (res->res_mknod3.MKNOD3res_u.resok.obj.handle_follows)) {
        free(handle->data.data_val);
    }*/
}

bool xdr_devicedata3(XDR *xdrs, devicedata3 *objp) {
    if (!xdr_sattr3(xdrs, &objp->dev_attributes))
        return (false);
    if (!xdr_specdata3(xdrs, &objp->spec))
        return (false);
    return (true);
}

bool xdr_mknoddata3(XDR *xdrs, mknoddata3 *objp) {
    if (!xdr_ftype3(xdrs, &objp->type))
        return (false);
    switch (objp->type) {
        case NF3CHR:
        case NF3BLK:
            if (!xdr_devicedata3(xdrs, &objp->mknoddata3_u.device))
                return (false);
            break;
        case NF3SOCK:
        case NF3FIFO:
            if (!xdr_sattr3(xdrs, &objp->mknoddata3_u.pipe_attributes))
                return (false);
            break;
        default:
            return (true);
            break;
    }
    return (true);
}

bool xdr_MKNOD3args(XDR *xdrs, MKNOD3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_mknoddata3(xdrs, &objp->what))
        return (false);
    return (true);
}

bool xdr_MKNOD3resok(XDR *xdrs, MKNOD3resok *objp) {
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKNOD3resfail(XDR *xdrs, MKNOD3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKNOD3res(XDR *xdrs, MKNOD3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_MKNOD3resok(xdrs, &objp->MKNOD3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_MKNOD3resfail(xdrs, &objp->MKNOD3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}