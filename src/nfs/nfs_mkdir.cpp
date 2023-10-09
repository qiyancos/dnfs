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
#include "nfs/nfs_mkdir.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_mkdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    if (arg->arg_mkdir3.where.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_mkdir get dir handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_mkdir3.where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_mkdir obtained dir handle is '%s', and the length is '%d'",
        arg->arg_mkdir3.where.dir.data.data_val,
        arg->arg_mkdir3.where.dir.data.data_len);
    out:

    return rc;
}

void nfs3_mkdir_free(nfs_res_t *res) {
/*    nfs_fh3 *handle =
            &res->res_mkdir3.MKDIR3res_u.resok.obj.post_op_fh3_u.handle;

    if ((res->res_mkdir3.status == NFS3_OK)
        && (res->res_mkdir3.MKDIR3res_u.resok.obj.handle_follows)) {
        free(handle->data.data_val);
    }*/
}

bool xdr_MKDIR3args(XDR *xdrs, MKDIR3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_sattr3(xdrs, &objp->attributes))
        return (false);
    return (true);
}

bool xdr_MKDIR3resok(XDR *xdrs, MKDIR3resok *objp) {
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKDIR3resfail(XDR *xdrs, MKDIR3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKDIR3res(XDR *xdrs, MKDIR3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_MKDIR3resok(xdrs, &objp->MKDIR3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_MKDIR3resfail(xdrs, &objp->MKDIR3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
