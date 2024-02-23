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
#include <sys/sysmacros.h>

#include "nfs/nfs_mknod.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_mknod(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;
    struct pre_op_attr pre = {};
    int make_res = -1;
    std::string file_path;
    mode_t mode = 0;
    dev_t dev;
    sattr3 *new_attr;

    /*数据指针*/
    MKNOD3args *mknod_args = &arg->arg_mknod3;
    specdata3 *spec = &mknod_args->what.mknoddata3_u.device.spec;
    sattr3 *dev_attributes = &mknod_args->what.mknoddata3_u.device.dev_attributes;
    sattr3 *pipe_attributes = &mknod_args->what.mknoddata3_u.pipe_attributes;
    MKNOD3resok *mknod_res_ok = &res->res_mknod3.MKNOD3res_u.resok;
    MKNOD3resfail *mknod_res_fail = &res->res_mknod3.MKNOD3res_u.resfail;

    /* to avoid setting them on each error case */
    mknod_res_fail->dir_wcc.before.attributes_follow = FALSE;
    mknod_res_fail->dir_wcc.after.attributes_follow = FALSE;

    if (mknod_args->where.dir.data.data_len == 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR, "arg_mknod get dir handle len is 0");
        goto out;
    }

    get_file_handle(mknod_args->where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_mknod obtained dir handle is '%s', and the length is '%d'",
        mknod_args->where.dir.data.data_val,
        mknod_args->where.dir.data.data_len);

    /*判断创建目录存不存在*/
    if (!judge_file_exit(mknod_args->where.dir.data.data_val, S_IFDIR))
    {
        rc = NFS_REQ_ERROR;
        res->res_mknod3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_mknod obtained dir handle '%s' not exist",
            mknod_args->where.dir.data.data_val);
        goto out;
    }
    if (mknod_args->where.name == nullptr || *mknod_args->where.name == '\0')
    {
        rc = NFS_REQ_ERROR;
        res->res_mknod3.status = NFS3ERR_INVAL;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_mknod obtained where.name('%s') invalid",
            mknod_args->where.name);
        goto out;
    }

    /*获取之前的属性*/
    res->res_mknod3.status = get_pre_op_attr(mknod_args->where.dir.data.data_val, pre);
    if (res->res_mknod3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_mknod failed to obtain '%s' pre_attributes",
            mknod_args->where.dir.data.data_val);
        goto out;
    }

    /*获取创建路径*/
    file_path = std::string(mknod_args->where.dir.data.data_val) + "/" + mknod_args->where.name;

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_mknod file path is '%s'", file_path.c_str());

    switch (mknod_args->what.type)
    {
    case NF3CHR:
        mode |= S_IFCHR;
        break;
    case NF3BLK:
        mode |= S_IFBLK;
        break;
    case NF3FIFO:
        mode |= S_IFIFO;
        break;
    case NF3SOCK:
        mode |= S_IFSOCK;
        break;
    default:
        res->res_mknod3.status = NFS3ERR_BADTYPE;
        rc = NFS_REQ_OK;
        goto out;
    }

    switch (mknod_args->what.type)
    {
    case NF3CHR:
    case NF3BLK:
        mode |= dev_attributes->mode.set_mode3_u.mode;
        dev = makedev(spec->specdata1, spec->specdata2);
        make_res = mknod(file_path.c_str(), mode, dev);
        new_attr = dev_attributes;
        break;
    case NF3FIFO:
    case NF3SOCK:
        mode |= pipe_attributes->mode.set_mode3_u.mode;
        make_res = mknod(file_path.c_str(), mode, 0);
        new_attr = pipe_attributes;
        break;
    default:
        res->res_mknod3.status = NFS3ERR_BADTYPE;
        rc = NFS_REQ_ERROR;
        goto out;
    }
    if (make_res != 0)
    {
        LOG(MODULE_NAME, D_ERROR, "create special file failed");
        rc = NFS_REQ_ERROR;
        res->res_mknod3.status = NFS3ERR_NOENT;
        goto outfail;
    }

    res->res_mknod3.status = nfs_set_sattr3(file_path.c_str(), *new_attr);
    if (res->res_mknod3.status != NFS3_OK)
    {
        LOG(MODULE_NAME, D_ERROR, "setattr(in nfs3_mknod) failed");
        rc = NFS_REQ_ERROR;
        goto out;
    }

    /*获取文件句柄*/
    set_file_handle(&mknod_res_ok->obj.post_op_fh3_u.handle, file_path);

    /*获取文件属性*/
    res->res_mknod3.status = nfs_set_post_op_attr(
        mknod_res_ok->obj.post_op_fh3_u.handle.data.data_val,
        &mknod_res_ok->obj_attributes);
    if (res->res_mknod3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' resok attributes",
            mknod_res_ok->obj.post_op_fh3_u.handle.data.data_val);
    }

    /*获取目录wcc信息*/
    res->res_mknod3.status = get_wcc_data(mknod_args->where.dir.data.data_val,
                                          pre,
                                          mknod_res_ok->dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_mknod3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_create failed to obtain '%s' resok wcc_data",
            mknod_args->where.dir.data.data_val);
    }

out:
    return rc;

outfail:
    /*获取失败目录wcc信息*/
    res->res_mknod3.status = get_wcc_data(mknod_args->where.dir.data.data_val,
                                          pre,
                                          mknod_res_fail->dir_wcc);
    if (res->res_mknod3.status != NFS3_OK)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_mknod failed to obtain '%s' resfail wcc_data",
            mknod_args->where.dir.data.data_val);
    }
    goto out;
}

void nfs3_mknod_free(nfs_res_t *res)
{
}

bool xdr_devicedata3(XDR *xdrs, devicedata3 *objp)
{
    if (!xdr_sattr3(xdrs, &objp->dev_attributes))
        return (false);
    if (!xdr_specdata3(xdrs, &objp->spec))
        return (false);
    return (true);
}

bool xdr_mknoddata3(XDR *xdrs, mknoddata3 *objp)
{
    if (!xdr_ftype3(xdrs, &objp->type))
        return (false);
    switch (objp->type)
    {
    case NF3CHR:
    case NF3BLK:
        if (!xdr_devicedata3(xdrs, &objp->mknoddata3_u.device))
            return (false);
        break;
    case NF3SOCK:
    case NF3FIFO:
        if (!xdr_sattr3(xdrs, &objp->mknoddata3_u.pipe_attributes))
            return (false);
        break;
    default:
        return (true);
        break;
    }
    return (true);
}

bool xdr_MKNOD3args(XDR *xdrs, MKNOD3args *objp)
{
    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_mknoddata3(xdrs, &objp->what))
        return (false);
    return (true);
}

bool xdr_MKNOD3resok(XDR *xdrs, MKNOD3resok *objp)
{
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKNOD3resfail(XDR *xdrs, MKNOD3resfail *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKNOD3res(XDR *xdrs, MKNOD3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status)
    {
    case NFS3_OK:
        if (!xdr_MKNOD3resok(xdrs, &objp->MKNOD3res_u.resok))
            return (false);
        break;
    default:
        if (!xdr_MKNOD3resfail(xdrs, &objp->MKNOD3res_u.resfail))
            return (false);
        break;
    }
    return (true);
}