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
#include "nfs/nfs_setattr.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_setattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{

    int rc = NFS_REQ_OK;

    if (arg->arg_setattr3.object.data.data_len == 0) {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME,L_ERROR,
            "nfs_setattr get file handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_setattr3.object);

    LOG(MODULE_NAME, D_INFO, "The value of the nfs_setattr obtained file handle is '%s', and the length is '%d'",
        arg->arg_setattr3.object.data.data_val,
        arg->arg_setattr3.object.data.data_len);
    out:

    return rc;

}

void nfs3_setattr_free(nfs_res_t *res)
{
    /* Nothing to do here */
}
bool xdr_sattrguard3(XDR *xdrs, sattrguard3 *objp)
{
    if (!xdr_bool(xdrs, &objp->check))
        return (false);
    switch (objp->check) {
        case TRUE:
            if (!xdr_nfstime3(xdrs, &objp->sattrguard3_u.obj_ctime))
                return (false);
            break;
        case FALSE:
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_SETATTR3args(XDR *xdrs, SETATTR3args *objp)
{
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    if (!xdr_sattr3(xdrs, &objp->new_attributes))
        return (false);
    if (!xdr_sattrguard3(xdrs, &objp->guard))
        return (false);
    return (true);
}

bool xdr_SETATTR3resok(XDR *xdrs, SETATTR3resok *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->obj_wcc))
        return (false);
    return (true);
}

bool xdr_SETATTR3resfail(XDR *xdrs, SETATTR3resfail *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->obj_wcc))
        return (false);
    return (true);
}

bool xdr_SETATTR3res(XDR *xdrs, SETATTR3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_SETATTR3resok(xdrs, &objp->SETATTR3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_SETATTR3resfail(xdrs, &objp->SETATTR3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
