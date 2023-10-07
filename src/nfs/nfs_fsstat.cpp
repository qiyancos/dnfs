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
#include "nfs/nfs_fsstat.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_fsstat(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;

    LOG(MODULE_NAME, D_INFO, "The value of the nfs_fsstat obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_fsstat3.fsroot.data.data_val,
        arg->arg_fsstat3.fsroot.data.data_len);

    /* to avoid setting it on each error case */
    res->res_fsstat3.FSSTAT3res_u.resfail.obj_attributes.attributes_follow =
            FALSE;

    if (arg->arg_fsstat3.fsroot.data.data_val == nullptr) {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME,L_ERROR,
            "nfs_fsstat get file handle is null");
        goto out;
    }

    res->res_fsstat3.status =nfs_set_post_op_attr(arg->arg_fsstat3.fsroot.data.data_val, &res->res_fsstat3.FSSTAT3res_u.resok.obj_attributes);
    if (res->res_fsstat3.status!=NFS3_OK)
    {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "Interface nfs_fsstat failed to obtain '%s' attributes",
            arg->arg_fsstat3.fsroot.data.data_val);
        goto out;
    }

//    res->res_fsstat3.FSSTAT3res_u.resok.tbytes = dynamicinfo.total_bytes;
    res->res_fsstat3.FSSTAT3res_u.resok.tbytes = 10000;
//    res->res_fsstat3.FSSTAT3res_u.resok.fbytes = dynamicinfo.free_bytes;
    res->res_fsstat3.FSSTAT3res_u.resok.fbytes = 10000;
//    res->res_fsstat3.FSSTAT3res_u.resok.abytes = dynamicinfo.avail_bytes;
    res->res_fsstat3.FSSTAT3res_u.resok.abytes = 5;
//    res->res_fsstat3.FSSTAT3res_u.resok.tfiles = dynamicinfo.total_files;
    res->res_fsstat3.FSSTAT3res_u.resok.tfiles = 5;
//    res->res_fsstat3.FSSTAT3res_u.resok.ffiles = dynamicinfo.free_files;
    res->res_fsstat3.FSSTAT3res_u.resok.ffiles = 5;
//    res->res_fsstat3.FSSTAT3res_u.resok.afiles = dynamicinfo.avail_files;
    res->res_fsstat3.FSSTAT3res_u.resok.afiles = 6;
    /* volatile FS */
    res->res_fsstat3.FSSTAT3res_u.resok.invarsec = 0;

    res->res_fsstat3.status = NFS3_OK;

    LOG(MODULE_NAME,D_INFO,
                 "nfs_Fsstat --> tbytes=%u fbytes=%u abytes=%u",
                 res->res_fsstat3.FSSTAT3res_u.resok.tbytes,
                 res->res_fsstat3.FSSTAT3res_u.resok.fbytes,
                 res->res_fsstat3.FSSTAT3res_u.resok.abytes);

    LOG(MODULE_NAME,D_INFO,
                 "nfs_Fsstat --> tfiles=%u ffiles=%u afiles=%u",
                 res->res_fsstat3.FSSTAT3res_u.resok.tfiles,
                 res->res_fsstat3.FSSTAT3res_u.resok.ffiles,
                 res->res_fsstat3.FSSTAT3res_u.resok.afiles);

    rc = NFS_REQ_OK;
    
    out:

    return rc;
}

void nfs3_fsstat_free(nfs_res_t *res)
{
    /* Nothing to do here */
}


bool xdr_FSSTAT3args(XDR *xdrs, FSSTAT3args *objp)
{
    if (!xdr_nfs_fh3(xdrs, &objp->fsroot))
        return (false);
    return (true);
}

bool xdr_FSSTAT3resok(XDR *xdrs, FSSTAT3resok *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_size3(xdrs, &objp->tbytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->fbytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->abytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->tfiles))
        return (false);
    if (!xdr_size3(xdrs, &objp->ffiles))
        return (false);
    if (!xdr_size3(xdrs, &objp->afiles))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->invarsec))
        return (false);
    return (true);
}

bool xdr_FSSTAT3resfail(XDR *xdrs, FSSTAT3resfail *objp)
{
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_FSSTAT3res(XDR *xdrs, FSSTAT3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_FSSTAT3resok(xdrs, &objp->FSSTAT3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_FSSTAT3resfail(xdrs, &objp->FSSTAT3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}