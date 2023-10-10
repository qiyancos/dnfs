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
#include "nfs/nfs_mkdir.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_mkdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*创建目录名*/
    string dir_path;

    /*获取目录属性信息状态*/
    nfsstat3 status;

    /*保存目录操作前属性信息*/
    struct pre_op_attr pre{};

    /*数据指针*/
    MKDIR3args *mkdir_args = &arg->arg_mkdir3;
    MKDIR3resok *mkdir_res_ok = &res->res_mkdir3.MKDIR3res_u.resok;
    MKDIR3resfail *mkdir_res_fail = &res->res_mkdir3.MKDIR3res_u.resfail;

    if (mkdir_args->where.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_mkdir get dir handle len is 0");
        goto out;
    }

    get_file_handle(mkdir_args->where.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_mkdir obtained dir handle is '%s', and the length is '%d'",
        mkdir_args->where.dir.data.data_val,
        mkdir_args->where.dir.data.data_len);

    /*判断主目录存不存在*/
    if (!judge_file_exit(mkdir_args->where.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_mkdir3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_mkdir obtained file handle '%s' not exist",
            mkdir_args->where.dir.data.data_val);
        goto out;
    }

    /*获取之前的属性*/
    res->res_mkdir3.status = get_pre_op_attr(mkdir_args->where.dir.data.data_val,
                                             pre);
    if (res->res_mkdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_mkdir failed to obtain '%s' pre_attributes",
            mkdir_args->where.dir.data.data_val);
        goto out;
    }

    /*获取目录路径*/
    dir_path = string(mkdir_args->where.dir.data.data_val) + "/" +
               mkdir_args->where.name;

    LOG(MODULE_NAME, D_INFO,
        "Interface nfs_mkdir make dir path is '%s'",
        dir_path.c_str());

    /*创建目录*/
    if (mkdir((char *) dir_path.c_str(), mkdir_args->attributes.mode.set_mode3_u.mode) !=
        0) {
        rc = NFS_REQ_ERROR;
        /*创建失败*/
        res->res_mkdir3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_mkdir make dir '%s' failed",
            dir_path.c_str());
        goto outfail;
    }

    /*todo 看是否需要更改属主等信息*/

    /*创建成功返回数据*/
    /*获取文件句柄*/
    mkdir_res_ok->obj.post_op_fh3_u.handle.data.data_val = (char *) dir_path.c_str();
    mkdir_res_ok->obj.post_op_fh3_u.handle.data.data_len = strlen(
            mkdir_res_ok->obj.post_op_fh3_u.handle.data.data_val);

    /*获取创建目录属性*/
    res->res_mkdir3.status = nfs_set_post_op_attr(
            mkdir_res_ok->obj.post_op_fh3_u.handle.data.data_val,
            &mkdir_res_ok->obj_attributes);
    if (res->res_mkdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_mkdir resok failed to obtain fir '%s' attributes",
            mkdir_res_ok->obj.post_op_fh3_u.handle.data.data_val);
    }

    /*获取根目录弱属性信息*/
    res->res_mkdir3.status = get_wcc_data(
            mkdir_args->where.dir.data.data_val, pre,
            mkdir_res_ok->dir_wcc);

    if (res->res_mkdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_mkdir resok failed to obtain '%s' wcc_data",
            mkdir_args->where.dir.data.data_val);
    }
    goto out;

    outfail:
    /*获取失败的wccdata*/
    status = get_wcc_data(mkdir_args->where.dir.data.data_val,
                          pre,
                          mkdir_res_fail->dir_wcc);
    /*获取弱属性信息失败*/
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' resfail wcc_data",
            mkdir_args->where.dir.data.data_val);
    }

    out:

    return rc;
}

void nfs3_mkdir_free(nfs_res_t *res) {
/*    nfs_fh3 *handle =
            &res->res_mkdir3.MKDIR3res_u.resok.obj.post_op_fh3_u.handle;

    if ((res->res_mkdir3.status == NFS3_OK)
        && (res->res_mkdir3.MKDIR3res_u.resok.obj.handle_follows)) {
        free(handle->data.data_val);
    }*/
}

bool xdr_MKDIR3args(XDR *xdrs, MKDIR3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->where))
        return (false);
    if (!xdr_sattr3(xdrs, &objp->attributes))
        return (false);
    return (true);
}

bool xdr_MKDIR3resok(XDR *xdrs, MKDIR3resok *objp) {
    if (!xdr_post_op_fh3(xdrs, &objp->obj))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKDIR3resfail(XDR *xdrs, MKDIR3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return (false);
    return (true);
}

bool xdr_MKDIR3res(XDR *xdrs, MKDIR3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_MKDIR3resok(xdrs, &objp->MKDIR3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_MKDIR3resfail(xdrs, &objp->MKDIR3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
