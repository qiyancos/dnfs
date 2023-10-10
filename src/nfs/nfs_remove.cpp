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
#include "string"

using namespace std;
#define MODULE_NAME "NFS"

int nfs3_remove(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*删除文件路径*/
    string filepath;
    /*操作状态*/
    nfsstat3 status;

    /*保存目录操作前属性信息*/
    struct pre_op_attr pre{};

    /*数据指针*/
    REMOVE3args *remove_args = &arg->arg_remove3;
    REMOVE3resok *remove_res_ok = &res->res_remove3.REMOVE3res_u.resok;
    REMOVE3resfail *remove_res_fail = &res->res_remove3.REMOVE3res_u.resfail;

    if (remove_args->object.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_remove get dir handle len is 0");
        goto out;
    }

    get_file_handle(remove_args->object.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_remove obtained file handle is '%s', and the length is '%d'",
        remove_args->object.dir.data.data_val,
        remove_args->object.dir.data.data_len);

    /*判断主目录存不存在*/
    if (!judge_file_exit(remove_args->object.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_rmdir3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_remove obtained file handle '%s' not exist",
            remove_args->object.dir.data.data_val);
        goto out;
    }

    /*获取之前的属性*/
    res->res_remove3.status = get_pre_op_attr(remove_args->object.dir.data.data_val,
                                              pre);
    if (res->res_remove3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_remove failed to obtain '%s' pre_attributes",
            remove_args->object.dir.data.data_val);
        goto out;
    }

    /*判断目录是否存在*/
    filepath = string(remove_args->object.dir.data.data_val) + "/" +
               remove_args->object.name;

    LOG(MODULE_NAME, L_INFO,
        "Interface nfs_remove remove file path is '%s'",
        filepath.c_str());

    /*如果删除的文件不存在*/
    if (!judge_file_exit(filepath, S_IFREG | S_IFLNK)) {
        rc = NFS_REQ_ERROR;
        /*文件不存在*/
        res->res_remove3.status = NFS3ERR_NOENT;
        goto outfail;
    }
    /*删除文件*/
    if (!remove_file(filepath)) {
        rc = NFS_REQ_ERROR;
        /*删除失败*/
        res->res_remove3.status = NFS3ERR_IO;
        goto outfail;
    }

    /*成功删除文件*/
    /*获取成功的目录弱属性对比*/
    res->res_remove3.status = get_wcc_data(remove_args->object.dir.data.data_val,
                                           pre,
                                           remove_res_ok->dir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_remove3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_remove failed to obtain '%s' resok wcc_data",
            remove_args->object.dir.data.data_val);
    }

    goto out;
    outfail:
    /*获取失败的wccdata*/
    status = get_wcc_data(remove_args->object.dir.data.data_val,
                          pre,
                          remove_res_fail->dir_wcc);
    /*获取弱属性信息失败*/
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, L_ERROR,
            "Interface nfs_remove failed to obtain '%s' resfail wcc_data",
            remove_args->object.dir.data.data_val);
    }

    out:
    return rc;
}

void nfs3_remove_free(nfs_res_t *res) {
}

bool xdr_REMOVE3args(XDR *xdrs, REMOVE3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->object))
        return FALSE;
    return TRUE;
}

bool xdr_REMOVE3resok(XDR *xdrs, REMOVE3resok *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_REMOVE3resfail(XDR *xdrs, REMOVE3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->dir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_REMOVE3res(XDR *xdrs, REMOVE3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
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
