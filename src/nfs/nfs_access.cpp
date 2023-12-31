/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#include "nfs/nfs_access.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_access(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*建立数据指针*/
    ACCESS3args *access_args = &arg->arg_access3;
    ACCESS3resfail *access_res_fail = &res->res_access3.ACCESS3res_u.resfail;
    ACCESS3resok *access_res_ok = &res->res_access3.ACCESS3res_u.resok;

    if (access_args->object.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "nfs_access get file handle len is 0");
        goto out;
    }

    get_file_handle(access_args->object);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_access obtained file handle is '%s', and the length is '%d'",
        access_args->object.data.data_val,
        access_args->object.data.data_len);

    res->res_access3.status = nfs_set_post_op_attr(access_args->object.data.data_val,
                                                   &access_res_ok->obj_attributes);
    if (res->res_access3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "Interface nfs_access failed to obtain '%s' attributes",
            access_args->object.data.data_val);
        goto out;
    }

    /*todo 先直接给与都能过*/
    access_res_ok->access = 0xff;

    out:

    return rc;
}

void nfs3_access_free(nfs_res_t *res) {
    /* Nothing to do */
}


bool xdr_ACCESS3args(XDR *xdrs, ACCESS3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->access))
        return (false);
    return (true);
}

bool xdr_ACCESS3resok(XDR *xdrs, ACCESS3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->access))
        return (false);
    return (true);
}

bool xdr_ACCESS3resfail(XDR *xdrs, ACCESS3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_ACCESS3res(XDR *xdrs, ACCESS3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_ACCESS3resok(xdrs, &objp->ACCESS3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_ACCESS3resfail(xdrs, &objp->ACCESS3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}