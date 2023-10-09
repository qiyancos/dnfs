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
#include "nfs/nfs_link.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_link(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {

    int rc = NFS_REQ_OK;

    if (arg->arg_link3.file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_link get file handle len is 0");
        goto out;
    }

    if (arg->arg_link3.link.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_link get dir handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_link3.link.dir);
    get_file_handle(arg->arg_link3.file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_link obtained file handle is '%s', and the length is '%d'",
        arg->arg_link3.file.data.data_val,
        arg->arg_link3.file.data.data_len);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_link obtained dir handle is '%s', and the length is '%d'",
        arg->arg_link3.link.dir.data.data_val,
        arg->arg_link3.link.dir.data.data_len);
    out:

    return rc;
}

void nfs3_link_free(nfs_res_t *resp) {
    /* Nothing to do here */
}


bool xdr_LINK3args(XDR *xdrs, LINK3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return (false);
    if (!xdr_diropargs3(xdrs, &objp->link))
        return (false);
    return (true);
}

bool xdr_LINK3resok(XDR *xdrs, LINK3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->linkdir_wcc))
        return (false);
    return (true);
}

bool xdr_LINK3resfail(XDR *xdrs, LINK3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->linkdir_wcc))
        return (false);
    return (true);
}

bool xdr_LINK3res(XDR *xdrs, LINK3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_LINK3resok(xdrs, &objp->LINK3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_LINK3resfail(xdrs, &objp->LINK3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
