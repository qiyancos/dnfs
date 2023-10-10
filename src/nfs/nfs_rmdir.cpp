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
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_rmdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*保存目录路径*/
    string dir_path;

    /*保存目录操作前属性信息*/
    struct pre_op_attr pre{};

    /*操作状态*/
    nfsstat3 status;

    /*数据指针*/
    RMDIR3args *rmdir_args = &arg->arg_rmdir3;
    RMDIR3resok *rmdir_res_ok = &res->res_rmdir3.RMDIR3res_u.resok;
    RMDIR3resfail *rmdir_res_fail = &res->res_rmdir3.RMDIR3res_u.resfail;

    if (rmdir_args->object.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_rmdir get dir handle len is 0");
        goto out;
    }

    /*获取目录句柄*/
    get_file_handle(rmdir_args->object.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rmdir obtained file handle is '%s', and the length is '%d'",
        rmdir_args->object.dir.data.data_val,
        rmdir_args->object.dir.data.data_len);

    LOG(MODULE_NAME, D_INFO,
        "The name of the arg_rmdir subdirectory to delete is '%s'",
        rmdir_args->object.name);

    /*判断主目录存不存在*/
    if (!judge_file_exit(rmdir_args->object.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_rmdir3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_rmdir obtained file handle '%s' not exist",
            arg->arg_remove3.object.dir.data.data_val);
        goto out;
    }

    /*获取之前的属性*/
    res->res_rmdir3.status = get_pre_op_attr(rmdir_args->object.dir.data.data_val,
                                             pre);
    if (res->res_rmdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' pre_attributes",
            rmdir_args->object.dir.data.data_val);
        goto out;
    }

    /*判断目录是否存在*/
    dir_path = string(rmdir_args->object.dir.data.data_val) + "/" +
               rmdir_args->object.name;

    LOG(MODULE_NAME, L_INFO,
        "Interface nfs_rmdir remove dir path is '%s'",
        dir_path.c_str());


    /*如果删除的目录不存在*/
    if (!judge_file_exit(dir_path, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        /*目录不存在*/
        res->res_rmdir3.status = NFS3ERR_NOTDIR;
        goto outfail;
    }

    /*删除目录*/
    if (!remove_directory(dir_path)) {
        rc = NFS_REQ_ERROR;
        /*删除失败*/
        res->res_remove3.status = NFS3ERR_IO;
        goto outfail;
    }

    /*成功删除文件*/
    /*获取成功的目录弱属性对比*/
    res->res_rmdir3.status = get_wcc_data(rmdir_args->object.dir.data.data_val,
                                          pre,
                                          rmdir_res_ok->dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rmdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' resok wcc_data",
            rmdir_args->object.dir.data.data_val);
    }
    goto out;

    outfail:
    /*获取失败的wccdata*/
    status = get_wcc_data(rmdir_args->object.dir.data.data_val,
                          pre,
                          rmdir_res_fail->dir_wcc);
    /*获取弱属性信息失败*/
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' resfail wcc_data",
            rmdir_args->object.dir.data.data_val);
    }

    out:
    return rc;
}

void nfs3_rmdir_free(nfs_res_t *res) {
}

bool xdr_RMDIR3args(XDR *xdrs, RMDIR3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->object))
        return FALSE;
    return TRUE;
}

bool xdr_RMDIR3resok(XDR *xdrs, RMDIR3resok *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RMDIR3resfail(XDR *xdrs, RMDIR3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RMDIR3res(XDR *xdrs, RMDIR3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
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