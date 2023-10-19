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
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_rename(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*创建链接路径*/
    string from_file_path;
    string to_file_path;

    /*操作状态*/
    nfsstat3 status;

    /*保存目录操作前属性信息*/
    struct pre_op_attr from_pre{};
    struct pre_op_attr to_pre{};

    /*数据指针*/
    RENAME3args *rename_args = &arg->arg_rename3;
    RENAME3resok *rename_res_ok = &res->res_rename3.RENAME3res_u.resok;
    RENAME3resfail *rename_res_fail = &res->res_rename3.RENAME3res_u.resfail;

    if (rename_args->from.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_rename get dir handle len is 0");
        goto out;
    }

    /*获取源文件目录句柄*/
    get_file_handle(rename_args->from.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rename obtained from dir handle is '%s',the length is '%d' file name is '%s'",
        rename_args->from.dir.data.data_val,
        rename_args->from.dir.data.data_len,
        rename_args->from.name);

    /*判断源目录存不存在*/
    if (!judge_file_exit(rename_args->from.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_rename3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_rename obtained source dir handle '%s' not exist",
            rename_args->from.dir.data.data_val);
        goto out;
    }

    /*获取源目录之前的属性*/
    res->res_rename3.status = get_pre_op_attr(rename_args->from.dir.data.data_val,
                                              from_pre);
    if (res->res_rename3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain from dir '%s' pre_attributes",
            rename_args->from.dir.data.data_val);
        goto out;
    }

    /*获取源文件*/
    from_file_path =
            string(rename_args->from.dir.data.data_val) + "/" + rename_args->from.name;

    LOG(MODULE_NAME, D_INFO,
        "The value of the source file path is '%s'", from_file_path.c_str());

    /*判断源文件存不存在*/
    if (!judge_file_exit(from_file_path,
                         S_IFDIR | S_IFCHR | S_IFREG | S_IFLNK | S_IFIFO | S_IFBLK |
                         S_IFSOCK)) {
        rc = NFS_REQ_ERROR;
        res->res_rename3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_rename source file '%s' not exist",
            from_file_path.c_str());
        goto out;
    }

    /*获取目标目录句柄*/
    get_file_handle(rename_args->to.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_rename obtained to dir handle is '%s',the length is '%d' file name is '%s'",
        rename_args->to.dir.data.data_val,
        rename_args->to.dir.data.data_len,
        rename_args->to.name);

    /*判断源目录存不存在*/
    if (!judge_file_exit(rename_args->to.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_rename3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_rename obtained to dir handle '%s' not exist",
            rename_args->to.dir.data.data_val);
        goto out;
    }

    /*获取源目录之前的属性*/
    res->res_rename3.status = get_pre_op_attr(rename_args->to.dir.data.data_val,
                                              to_pre);
    if (res->res_rename3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain to dir '%s' pre_attributes",
            rename_args->to.dir.data.data_val);
        goto out;
    }

    /*获取目标文件*/
    to_file_path =
            string(rename_args->to.dir.data.data_val) + "/" + rename_args->to.name;

    LOG(MODULE_NAME, D_INFO,
        "The value of the new file path is '%s'", from_file_path.c_str());

    /*重命名*/
    if (rename(from_file_path.c_str(), to_file_path.c_str()) != 0) {
        rc = NFS_REQ_ERROR;
        /*创建失败*/
        res->res_rename3.status = NFS3ERR_NOTEMPTY;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_renmae rename '%s' to '%s' failed",
            from_file_path.c_str(), to_file_path.c_str());
        goto outfail;
    }

    /*重名名成功*/
    /*获取源目录wcc信息*/
    res->res_rename3.status = get_wcc_data(rename_args->from.dir.data.data_val,
                                           from_pre,
                                           rename_res_ok->fromdir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rename3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain from '%s' resok wcc_data",
            rename_args->from.dir.data.data_val);
    }

    /*获取目标目录wcc信息*/
    res->res_rename3.status = get_wcc_data(rename_args->to.dir.data.data_val,
                                           to_pre,
                                           rename_res_ok->todir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rename3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain to '%s' resok wcc_data",
            rename_args->to.dir.data.data_val);
    }

    goto out;

    outfail:
    /*重命名失败*/
    /*获取源目录wcc信息*/
    res->res_rename3.status = get_wcc_data(rename_args->from.dir.data.data_val,
                                           from_pre,
                                           rename_res_fail->fromdir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rename3.status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain from '%s' resfail wcc_data",
            rename_args->from.dir.data.data_val);
    }

    /*获取目标目录wcc信息*/
    res->res_rename3.status = get_wcc_data(rename_args->to.dir.data.data_val,
                                           to_pre,
                                           rename_res_fail->todir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_rename3.status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_rename failed to obtain to '%s' resfail wcc_data",
            rename_args->to.dir.data.data_val);
    }

    out:

    return rc;
}

void nfs3_rename_free(nfs_res_t *res) {
}

bool xdr_RENAME3args(XDR *xdrs, RENAME3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->from))
        return FALSE;
    if (!xdr_diropargs3(xdrs, &objp->to))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3resok(XDR *xdrs, RENAME3resok *objp) {
    if (!xdr_wcc_data(xdrs, &objp->fromdir_wcc))
        return FALSE;
    if (!xdr_wcc_data(xdrs, &objp->todir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3resfail(XDR *xdrs, RENAME3resfail *objp) {
    if (!xdr_wcc_data(xdrs, &objp->fromdir_wcc))
        return FALSE;
    if (!xdr_wcc_data(xdrs, &objp->todir_wcc))
        return FALSE;
    return TRUE;
}

bool xdr_RENAME3res(XDR *xdrs, RENAME3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return FALSE;
    switch (objp->status) {
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
