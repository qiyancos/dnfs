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
#include "nfs/nfs_rmdir.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_rmdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;

    if (arg->arg_rmdir3.object.dir.data.data_len == 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_rmdir get dir handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_rmdir3.object.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rmdir obtained file handle is '%s', and the length is '%d'",
        arg->arg_rmdir3.object.dir.data.data_val,
        arg->arg_rmdir3.object.dir.data.data_len);
out:

    return rc;
}

void nfs3_rmdir_free(nfs_res_t *res)
{
}

bool xdr_RMDIR3args(XDR *xdrs, RMDIR3args *objp)
{
	if (!xdr_diropargs3(xdrs, &objp->object))
		return FALSE;
	return TRUE;
}

bool xdr_RMDIR3resok(XDR *xdrs, RMDIR3resok *objp)
{
	if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
		return FALSE;
	return TRUE;
}

bool xdr_RMDIR3resfail(XDR *xdrs, RMDIR3resfail *objp)
{
	if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
		return FALSE;
	return TRUE;
}

bool xdr_RMDIR3res(XDR *xdrs, RMDIR3res *objp)
{
	if (!xdr_nfsstat3(xdrs, &objp->status))
		return FALSE;
	switch (objp->status)
	{
	case NFS3_OK:
		if (!xdr_RMDIR3resok(xdrs, &objp->RMDIR3res_u.resok))
			return FALSE;
		break;
	default:
		if (!xdr_RMDIR3resfail(xdrs, &objp->RMDIR3res_u.resfail))
			return FALSE;
		break;
	}
	return TRUE;
}