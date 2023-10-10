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
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_pathconf(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*数据指针*/
    PATHCONF3args *pathconf_args = &arg->arg_pathconf3;
    PATHCONF3resok *pathconf_res_ok = &res->res_pathconf3.PATHCONF3res_u.resok;
    PATHCONF3resfail *pathconf_res_fail = &res->res_pathconf3.PATHCONF3res_u.resfail;

    if (pathconf_args->object.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "nfs_pathconf get file handle len is 0");
        goto out;
    }

    get_file_handle(pathconf_args->object);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_pathconf obtained file handle is '%s', and the length is '%d'",
        pathconf_args->object.data.data_val,
        pathconf_args->object.data.data_len);

    /* to avoid setting it on each error case */
    pathconf_res_fail->obj_attributes.attributes_follow = FALSE;

    res->res_pathconf3.status = nfs_set_post_op_attr(
            pathconf_args->object.data.data_val,
            &pathconf_res_ok->obj_attributes);

    if (res->res_pathconf3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_pathconf failed to obtain '%s' attributes",
            pathconf_args->object.data.data_val);
        goto out;
    }

    pathconf_res_ok->linkmax = 1024;
    pathconf_res_ok->name_max = NAME_MAX;
    pathconf_res_ok->no_trunc = true;
    pathconf_res_ok->chown_restricted = true;
    pathconf_res_ok->case_insensitive = true;
    pathconf_res_ok->case_preserving = true;

    res->res_pathconf3.status = NFS3_OK;

    out:
    return rc;
}

void nfs3_pathconf_free(nfs_res_t *res) {
    /* Nothing to do here */
}


bool xdr_PATHCONF3args(XDR *xdrs, PATHCONF3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    return (true);
}

bool xdr_PATHCONF3resok(XDR *xdrs, PATHCONF3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->linkmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->name_max))
        return (false);
    if (!xdr_bool(xdrs, &objp->no_trunc))
        return (false);
    if (!xdr_bool(xdrs, &objp->chown_restricted))
        return (false);
    if (!xdr_bool(xdrs, &objp->case_insensitive))
        return (false);
    if (!xdr_bool(xdrs, &objp->case_preserving))
        return (false);
    return (true);
}

bool xdr_PATHCONF3resfail(XDR *xdrs, PATHCONF3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_PATHCONF3res(XDR *xdrs, PATHCONF3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_PATHCONF3resok(xdrs, &objp->PATHCONF3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_PATHCONF3resfail(xdrs, &objp->PATHCONF3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
