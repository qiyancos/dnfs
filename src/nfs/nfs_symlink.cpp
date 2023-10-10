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
#include "nfs/nfs_symlink.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_symlink(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*数据指针*/
    SYMLINK3args *symllink_args = &arg->arg_symlink3;
    SYMLINK3resok *symllink_res_ok = &res->res_symlink3.SYMLINK3res_u.resok;
    SYMLINK3resfail *symllink_res_fail = &res->res_symlink3.SYMLINK3res_u.resfail;

    if (symllink_args->where.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_symlink get dir handle len is 0");
        goto out;
    }

    get_file_handle(symllink_args->where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_symlink obtained dir handle is '%s', and the length is '%d'",
        symllink_args->where.dir.data.data_val,
        symllink_args->where.dir.data.data_len);
    out:

    return rc;
}

void nfs3_symlink_free(nfs_res_t *res) {
/*    symlink3resok *resok = &res->res_symlink3.symlink3res_u.resok;

    if (res->res_symlink3.status == NFS3_OK && resok->obj.handle_follows)
        free(resok->obj.post_op_fh3_u.handle.data.data_val);*/
}

bool xdr_symlinkdata3(XDR *xdrs, symlinkdata3 *objp) {
    if (!xdr_sattr3(xdrs, &objp->symlink_attributes))
        return (false);
    if (!xdr_nfspath3(xdrs, &objp->symlink_data))
        return (false);
    return (true);
}

bool xdr_SYMLINK3args(XDR *xdrs, SYMLINK3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_symlinkdata3(xdrs, &objp->symlink))
        return (false);
    return (true);
}

bool xdr_SYMLINK3resok(XDR *xdrs, SYMLINK3resok *objp) {
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_SYMLINK3resfail(XDR *xdrs, SYMLINK3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_SYMLINK3res(XDR *xdrs, SYMLINK3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_SYMLINK3resok(xdrs, &objp->SYMLINK3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_SYMLINK3resfail(xdrs, &objp->SYMLINK3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
