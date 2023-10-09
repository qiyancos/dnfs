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

    /*保存文件路径*/
    string filepath;

    /*保存文件操作前属性信息*/
    struct pre_op_attr pre{};

    if (arg->arg_rmdir3.object.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_rmdir get dir handle len is 0");
        goto out;
    }

    /*获取文件句柄*/
    get_file_handle(arg->arg_rmdir3.object.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rmdir obtained file handle is '%s', and the length is '%d'",
        arg->arg_rmdir3.object.dir.data.data_val,
        arg->arg_rmdir3.object.dir.data.data_len);

    LOG(MODULE_NAME, D_INFO,
        "The name of the arg_rmdir subdirectory to delete is '%s'",
        arg->arg_rmdir3.object.name);

    /*获取之前的属性*/
    res->res_rmdir3.status = get_pre_op_attr(arg->arg_rmdir3.object.dir.data.data_val,
                                             pre);
    if (res->res_rmdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' pre_attributes",
            arg->arg_rmdir3.object.dir.data.data_val);
        goto out;
    }

    /*判断文件是否存在*/
    filepath = string(arg->arg_readdirplus3.dir.data.data_val) + "/" +
               arg->arg_rmdir3.object.name;

    /*如果删除的目录不存在*/
    if (!judge_file_exit(filepath, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        goto outfail;
    }

    /*删除目录*/
    if (remove_directory(filepath)) {
        rc = NFS_REQ_OK;
        goto outok;
    }

outok:
    /*获取成功的文件弱属性对比*/
    res->res_rmdir3.status = get_wcc_data(arg->arg_rmdir3.object.dir.data.data_val,
                                          pre,
                                          res->res_rmdir3.RMDIR3res_u.resok.dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rmdir3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' resok wcc_data",
            arg->arg_rmdir3.object.dir.data.data_val);
    }
    goto out;

outfail:
    /*获取失败的wccdata*/
    res->res_rmdir3.status = get_wcc_data(arg->arg_rmdir3.object.dir.data.data_val,
                                          pre,
                                          res->res_rmdir3.RMDIR3res_u.resfail.dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rmdir3.status != NFS3_OK) {
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_rmdir failed to obtain '%s' resfail wcc_data",
            arg->arg_rmdir3.object.dir.data.data_val);
        goto out;
    }
    /*文件夹不存在*/
    res->res_rmdir3.status = NFS3ERR_NOTDIR;

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