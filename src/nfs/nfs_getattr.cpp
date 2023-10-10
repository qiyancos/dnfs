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
#include "nfs/nfs_getattr.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_getattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    struct post_op_attr get_a{};

    /*数据指针*/
    GETATTR3args *getattr_args = &arg->arg_getattr3;
    GETATTR3resok *getattr_res_ok = &res->res_getattr3.GETATTR3res_u.resok;

    if (getattr_args->object.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "nfs_getattr get file handle len is 0");
        goto out;
    }

    get_file_handle(getattr_args->object);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_getattr obtained file handle is '%s', and the length is '%d'",
        getattr_args->object.data.data_val,
        getattr_args->object.data.data_len);


    res->res_getattr3.status = nfs_set_post_op_attr(
            getattr_args->object.data.data_val, &get_a);
    if (res->res_getattr3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_getattr failed to obtain '%s' attributes",
            getattr_args->object.data.data_val);
        goto out;
    }

    memcpy(&getattr_res_ok->obj_attributes, &get_a.post_op_attr_u,
           sizeof(get_a.post_op_attr_u));

    out:
    return rc;

}

/**
 * @brief Free the result structure allocated for nfs3_getattr.
 *
 * @param[in,out] resp Result structure
 *
 */
void nfs3_getattr_free(nfs_res_t *resp) {
    /* Nothing to do here */
}

bool xdr_GETATTR3args(XDR *xdrs, GETATTR3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    return (true);
}

bool xdr_GETATTR3resok(XDR *xdrs, GETATTR3resok *objp) {
    if (!xdr_fattr3(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_GETATTR3res(XDR *xdrs, GETATTR3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_GETATTR3resok(xdrs, &objp->GETATTR3res_u.resok))
                return (false);
            break;
        default:
            return (true);
    }
    return (true);
}