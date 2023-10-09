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
#include "nfs/nfs_remove.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_remove(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;

    if (arg->arg_remove3.object.dir.data.data_len == 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_remove get dir handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_remove3.object.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_remove obtained file handle is '%s', and the length is '%d'",
        arg->arg_remove3.object.dir.data.data_val,
        arg->arg_remove3.object.dir.data.data_len);
out:

    return rc;
}

void nfs3_remove_free(nfs_res_t *res)
{
}

bool xdr_REMOVE3args(XDR *xdrs, REMOVE3args *objp)
{
	if (!xdr_diropargs3(xdrs, &objp->object))
		return FALSE;
	return TRUE;
}

bool xdr_REMOVE3resok(XDR *xdrs, REMOVE3resok *objp)
{
	if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
		return FALSE;
	return TRUE;
}

bool xdr_REMOVE3resfail(XDR *xdrs, REMOVE3resfail *objp)
{
	if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
		return FALSE;
	return TRUE;
}

bool xdr_REMOVE3res(XDR *xdrs, REMOVE3res *objp)
{
	if (!xdr_nfsstat3(xdrs, &objp->status))
		return FALSE;
	switch (objp->status)
	{
	case NFS3_OK:
		if (!xdr_REMOVE3resok(xdrs, &objp->REMOVE3res_u.resok))
			return FALSE;
		break;
	default:
		if (!xdr_REMOVE3resfail(xdrs, &objp->REMOVE3res_u.resfail))
			return FALSE;
		break;
	}
	return TRUE;
}
