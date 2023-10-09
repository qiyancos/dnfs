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

#include "nfs/nfs_rename.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "dnfsd/dnfs_config.h"

#define MODULE_NAME "NFS"
int nfs3_rename(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;

    if (arg->arg_rename3.from.dir.data.data_len == 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_rename get dir handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_rename3.from.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rename obtained dir handle is '%s', and the length is '%d'",
        arg->arg_rename3.from.dir.data.data_val,
        arg->arg_rename3.from.dir.data.data_len);
out:

    return rc;
}

void nfs3_rename_free(nfs_res_t *res)
{
}

bool xdr_RENAME3args(XDR *xdrs, RENAME3args *objp)
{
    if (!xdr_diropargs3(xdrs, &objp->from))
        return FALSE;
    if (!xdr_diropargs3(xdrs, &objp->to))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3resok(XDR *xdrs, RENAME3resok *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->fromdir_wcc))
        return FALSE;
    if (!xdr_wcc_data(xdrs, &objp->todir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3resfail(XDR *xdrs, RENAME3resfail *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->fromdir_wcc))
        return FALSE;
    if (!xdr_wcc_data(xdrs, &objp->todir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3res(XDR *xdrs, RENAME3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status)
    {
    case NFS3_OK:
        if (!xdr_RENAME3resok(xdrs, &objp->RENAME3res_u.resok))
            return FALSE;
        break;
    default:
        if (!xdr_RENAME3resfail(xdrs, &objp->RENAME3res_u.resfail))
            return FALSE;
        break;
    }
    return TRUE;
}
