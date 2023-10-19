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
#include "nfs/nfs_readlink.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_readlink(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;

    /*链接文件属性*/
    post_op_attr pos_a = {};

    /*读取数据大小*/
    size_t data_len;

    /*数据指针*/
    READLINK3args *readlink_args = &arg->arg_readlink3;
    READLINK3resok *readlink_res_ok = &res->res_readlink3.READLINK3res_u.resok;
    READLINK3resfail *readlink_res_fail = &res->res_readlink3.READLINK3res_u.resfail;

    if (readlink_args->symlink.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "nfs_readlink get file handle len is 0");
        goto out;
    }

    get_file_handle(readlink_args->symlink);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_readlink obtained file handle is '%s', and the length is '%d'",
        readlink_args->symlink.data.data_val,
        readlink_args->symlink.data.data_len);

    /*判文件存不存在*/
    if (!judge_file_exit(readlink_args->symlink.data.data_val, S_IFLNK)) {
        rc = NFS_REQ_ERROR;
        res->res_readlink3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_readlink file '%s' not exist",
            readlink_args->symlink.data.data_val);
        goto out;
    }

    /*获取文件属性*/
    res->res_readlink3.status = nfs_set_post_op_attr(readlink_args->symlink.data.data_val,
                                                     &pos_a);
    if (res->res_readlink3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_readlink failed to obtain '%s' attributes",
            readlink_args->symlink.data.data_val);
        goto out;
    }

    /*为文件内容分配空间*/
    data_len = pos_a.post_op_attr_u.attributes.size;
    readlink_res_ok->data = (char *) calloc(0,data_len);

    /*读取文件内容*/
    if (readlink(readlink_args->symlink.data.data_val, readlink_res_ok->data, data_len) ==
        -1) {
        rc = NFS_REQ_ERROR;
        /*创建失败*/
        res->res_readlink3.status = NFS3ERR_IO;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_readlink read '%s' content failed",
            readlink_args->symlink.data.data_val);
        goto outfail;
    }

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_readlink link '%s' content is '%s'",
        readlink_args->symlink.data.data_val,
        readlink_res_ok->data);

    /*读取成功*/
    readlink_res_ok->symlink_attributes = pos_a;
    goto out;

    outfail:
    /*读取失败，返回*/
    readlink_res_fail->symlink_attributes = pos_a;

    out:

    return rc;
}

void nfs3_readlink_free(nfs_res_t *res) {
    if (res->res_readlink3.status == NFS3_OK)
        gsh_free(res->res_readlink3.READLINK3res_u.resok.data);
}

bool xdr_READLINK3args(XDR *xdrs, READLINK3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->symlink))
        return (false);
    return (true);
}

bool xdr_READLINK3resok(XDR *xdrs, READLINK3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->symlink_attributes))
        return (false);
    if (!xdr_nfspath3(xdrs, &objp->data))
        return (false);
    return (true);
}

bool xdr_READLINK3resfail(XDR *xdrs, READLINK3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->symlink_attributes))
        return (false);
    return (true);
}

bool xdr_READLINK3res(XDR *xdrs, READLINK3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_READLINK3resok(xdrs, &objp->READLINK3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_READLINK3resfail(xdrs, &objp->READLINK3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
