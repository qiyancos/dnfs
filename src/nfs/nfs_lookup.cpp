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
#include "nfs/nfs_lookup.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_lookup(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    /*文件路径*/
    string filepath;
    /*操作状态*/
    nfsstat3 status;

    /*数据指针*/
    LOOKUP3args *lookup_args = &arg->arg_lookup3;
    LOOKUP3resok *lookup_res_ok = &res->res_lookup3.LOOKUP3res_u.resok;
    LOOKUP3resfail *lookup_res_fail = &res->res_lookup3.LOOKUP3res_u.resfail;

    if (lookup_args->what.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_link get dir handle len is 0");
        goto out;
    }

    get_file_handle(lookup_args->what.dir);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_lookup obtained dir handle is '%s', and the length is '%d'",
        lookup_args->what.dir.data.data_val,
        lookup_args->what.dir.data.data_len);

    /*判断主目录存不存在*/
    if (!judge_file_exit(lookup_args->what.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_lookup3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_lookup obtained file handle '%s' not exist",
            lookup_args->what.dir.data.data_val);
        goto out;
    }

    /*判断文件是否存在*/
    filepath = string(lookup_args->what.dir.data.data_val) + "/" +
               lookup_args->what.name;

    LOG(MODULE_NAME, D_INFO,
        "Interface nfs_lookup lookup file path is '%s'",
        filepath.c_str());

    /*如果查找的目录不存在,跳转fail*/
    if (!judge_file_exit(filepath.c_str(),
                         S_IFDIR | S_IFCHR | S_IFREG | S_IFLNK | S_IFIFO | S_IFBLK |
                         S_IFSOCK)) {
        rc = NFS_REQ_ERROR;
        /*文件不存在*/
        res->res_rmdir3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_lookup lookup file '%s' not exist",
            filepath.c_str());
        goto outfail;
    }
    /*成功查找到文件*/
    /*获取文件句柄*/
    set_file_handle(&lookup_res_ok->object, filepath);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_lookup lookup file handle is '%s', and the length is '%d'",
        lookup_res_ok->object.data.data_val,
        lookup_res_ok->object.data.data_len);

    /*获取目录属性*/
    res->res_lookup3.status = nfs_set_post_op_attr(
            lookup_args->what.dir.data.data_val,
            &lookup_res_ok->dir_attributes);
    if (res->res_lookup3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_lookup resok failed to obtain fir '%s' attributes",
            lookup_args->what.dir.data.data_val);
    }

    /*获取文件属性*/
    res->res_lookup3.status = nfs_set_post_op_attr(
            lookup_res_ok->object.data.data_val,
            &lookup_res_ok->obj_attributes);
    if (res->res_lookup3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_lookup resok failed to obtain '%s' attributes",
            filepath.c_str());
    }

    goto out;

    outfail:
    /*获取目录属性*/
    status = nfs_set_post_op_attr(
            lookup_args->what.dir.data.data_val,
            &lookup_res_fail->dir_attributes);
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_lookup resfail failed to obtain dir '%s' attributes",
            lookup_args->what.dir.data.data_val);
    }

    out:
    return rc;
}

void nfs3_lookup_free(nfs_res_t *res) {
    /*释放句柄内存*/
    if (res->res_lookup3.status == NFS3_OK) {
        gsh_free(res->res_lookup3.LOOKUP3res_u.resok.object.data.data_val);
    }
}

bool xdr_LOOKUP3args(XDR *xdrs, LOOKUP3args *objp) {
    if (!xdr_diropargs3(xdrs, &objp->what))
        return (false);
    return (true);
}

bool xdr_LOOKUP3resok(XDR *xdrs, LOOKUP3resok *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->object))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return (false);
    return (true);
}

bool xdr_LOOKUP3resfail(XDR *xdrs, LOOKUP3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->dir_attributes))
        return (false);
    return (true);
}

bool xdr_LOOKUP3res(XDR *xdrs, LOOKUP3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_LOOKUP3resok(xdrs, &objp->LOOKUP3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_LOOKUP3resfail(xdrs, &objp->LOOKUP3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}