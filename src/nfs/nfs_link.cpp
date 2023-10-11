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
#include "nfs/nfs_link.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "string"

using namespace std;

#define MODULE_NAME "NFS"

int nfs3_link(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {

    int rc = NFS_REQ_OK;

    /*创建链接路径*/
    string file_path;

    /*操作状态*/
    nfsstat3 status;

    /*保存目录操作前属性信息*/
    struct pre_op_attr pre{};


    /*数据指针*/
    LINK3args *link_args = &arg->arg_link3;
    LINK3resok *link_res_ok = &res->res_link3.LINK3res_u.resok;
    LINK3resfail *link_res_fail = &res->res_link3.LINK3res_u.resfail;

    if (link_args->file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_link get file handle len is 0");
        goto out;
    }

    if (link_args->link.dir.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "arg_link get dir handle len is 0");
        goto out;
    }

    get_file_handle(link_args->link.dir);
    get_file_handle(link_args->file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_link obtained source file handle is '%s', and the length is '%d'",
        link_args->file.data.data_val,
        link_args->file.data.data_len);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_link obtained to dir file is '%s', and the length is '%d'",
        link_args->link.dir.data.data_val,
        link_args->link.dir.data.data_len);

    /*判断创建目录存不存在*/
    if (!judge_file_exit(link_args->link.dir.data.data_val, S_IFDIR)) {
        rc = NFS_REQ_ERROR;
        res->res_link3.status = NFS3ERR_NOTDIR;
        LOG(MODULE_NAME, D_ERROR,
            "The value of the arg_link obtained link dir handle '%s' not exist",
            link_args->link.dir.data.data_val);
        goto out;
    }

    /*获取之前的属性*/
    res->res_link3.status = get_pre_op_attr(link_args->link.dir.data.data_val,
                                            pre);
    if (res->res_link3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to obtain '%s' pre_attributes",
            link_args->link.dir.data.data_val);
        goto out;
    }

    /*获取链接路径*/
    file_path = string(link_args->link.dir.data.data_val) + "/" +
                link_args->link.name;

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_link link to file path is '%s'", file_path.c_str());

    /*创建链接*/
    if (link(link_args->file.data.data_val, file_path.c_str()) != 0) {
        rc = NFS_REQ_ERROR;
        res->res_link3.status = NFS3ERR_NOENT;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to create link file '%s'",
            file_path.c_str());
        goto outfail;
    }

    /*建立成功*/
    /*获取文件属性*/
    res->res_link3.status = nfs_set_post_op_attr(link_args->file.data.data_val,
                                                 &link_res_ok->file_attributes);
    if (res->res_link3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to obtain '%s' resok attributes",
            link_args->file.data.data_val);
    }

    /*获取wccdata信息*/
    res->res_link3.status = get_wcc_data(link_args->link.dir.data.data_val,
                                         pre,
                                         link_res_ok->linkdir_wcc);
    /*获取弱属性信息失败*/
    if (res->res_link3.status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to obtain '%s' resok wcc_data",
            link_args->link.dir.data.data_val);
    }

    goto out;

    outfail:
    /*获取文件属性*/
    status = nfs_set_post_op_attr(link_args->file.data.data_val,
                                  &link_res_fail->file_attributes);
    if (status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to obtain '%s' resfail attributes",
            link_args->file.data.data_val);
    }

    /*获取失败的wccdata*/
    status = get_wcc_data(link_args->link.dir.data.data_val,
                          pre,
                          link_res_fail->linkdir_wcc);
    /*获取弱属性信息失败*/
    if (status != NFS3_OK) {
        LOG(MODULE_NAME, D_ERROR,
            "Interface nfs_link failed to obtain '%s' resfail wcc_data",
            link_args->link.dir.data.data_val);
    }

    out:

    return rc;
}

void nfs3_link_free(nfs_res_t *resp) {
    /* Nothing to do here */
}


bool xdr_LINK3args(XDR *xdrs, LINK3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return (false);
    if (!xdr_diropargs3(xdrs, &objp->link))
        return (false);
    return (true);
}

bool xdr_LINK3resok(XDR *xdrs, LINK3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->linkdir_wcc))
        return (false);
    return (true);
}

bool xdr_LINK3resfail(XDR *xdrs, LINK3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    if (!xdr_wcc_data(xdrs, &objp->linkdir_wcc))
        return (false);
    return (true);
}

bool xdr_LINK3res(XDR *xdrs, LINK3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_LINK3resok(xdrs, &objp->LINK3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_LINK3resfail(xdrs, &objp->LINK3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}
