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
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#include <sys/time.h>

#include "nfs/nfs_setattr.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_setattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    u_int fh_data_len = arg->arg_setattr3.object.data.data_len;
    char **fh_data_val = &arg->arg_setattr3.object.data.data_val;
    sattr3 *new_attr = &arg->arg_setattr3.new_attributes;
    SETATTR3resfail *resfail = &res->res_setattr3.SETATTR3res_u.resfail;
    SETATTR3resok *resok = &res->res_setattr3.SETATTR3res_u.resok;
    int rc = NFS_REQ_OK;
    pre_op_attr pre_attr = {};

    /* to avoid setting it on each error case */
    resfail->obj_wcc.before.attributes_follow = FALSE;
    resfail->obj_wcc.after.attributes_follow = FALSE;

    if (fh_data_len == 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "nfs_setattr get file handle len is 0");
        goto out;
    }

    get_file_handle(arg->arg_setattr3.object);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_setattr obtained file handle is '%s', and the length is '%d'",
        *fh_data_val, fh_data_len);

    /*获取之前的属性*/
    res->res_setattr3.status = get_pre_op_attr(*fh_data_val, pre_attr);
    if (res->res_setattr3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_setattr failed to obtain '%s' pre_attributes",
            fh_data_val);
        goto out;
    }

    if (arg->arg_setattr3.guard.check)
    {
        /* This pack of lines implements the "guard check" setattr. This
         * feature of nfsv3 is used to avoid several setattr to occur
         * concurrently on the same object, from different clients
         */
        nfstime3 *obj_ctime = &arg->arg_setattr3.guard.sattrguard3_u.obj_ctime;
        nfstime3 *pre_ctime = &pre_attr.pre_op_attr_u.attributes.ctime;

        LOG(MODULE_NAME, D_INFO, "css=%d acs=%d csn=%d acn=%d",
            obj_ctime->tv_sec, pre_ctime->tv_sec,
            obj_ctime->tv_nsec, pre_ctime->tv_nsec);

        if (obj_ctime->tv_sec != pre_ctime->tv_sec || obj_ctime->tv_nsec != pre_ctime->tv_nsec)
        {
            res->res_setattr3.status = NFS3ERR_NOT_SYNC;
            rc = NFS_REQ_OK;
            LOG(MODULE_NAME, D_ERROR, "guard check failed");
            goto out;
        }
    }

    res->res_setattr3.status = nfs_set_sattr3(*fh_data_val, *new_attr);
    if (res->res_setattr3.status != NFS3_OK)
    {
        LOG(MODULE_NAME, D_ERROR, "setattr(in nfs3_setattr) failed");
        rc = NFS_REQ_ERROR;
        goto outfail;
    }

    /*获取成功的文件弱属性对比*/
    res->res_setattr3.status = get_wcc_data(*fh_data_val, pre_attr, resok->obj_wcc);
    /*获取弱属性信息失败*/
    if (res->res_setattr3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_setattr failed to obtain '%s' resok obj_wcc",
            fh_data_val);
    }
    goto out;

outfail:
    /*获取失败的wccdata*/
    res->res_setattr3.status = get_wcc_data(*fh_data_val, pre_attr, resfail->obj_wcc);
    /*获取弱属性信息失败*/
    if (res->res_setattr3.status != NFS3_OK)
    {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_setattr failed to obtain '%s' resfail obj_wcc",
            fh_data_val);
        goto out;
    }
    return rc;

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
    switch (objp->check)
    {
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
    switch (objp->status)
    {
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
