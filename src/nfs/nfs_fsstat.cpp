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
#include "nfs/nfs_fsstat.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include <sys/statvfs.h>

#define MODULE_NAME "NFS"

int nfs3_fsstat(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    struct statvfs buffstatvfs{};
    int retval = 0;

    /*数据指针*/
    FSSTAT3args *fsstat_args = &arg->arg_fsstat3;
    FSSTAT3resok *fsstat_res_ok = &res->res_fsstat3.FSSTAT3res_u.resok;
    FSSTAT3resfail *fsstat_res_fail = &res->res_fsstat3.FSSTAT3res_u.resfail;

    if (fsstat_args->fsroot.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR,
            "nfs_fsstat get file handle len is 0");
        goto out;
    }

    get_file_handle(fsstat_args->fsroot);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_fsstat obtained file handle is '%s', and the length is data_val is '%d'",
        fsstat_args->fsroot.data.data_val,
        fsstat_args->fsroot.data.data_len);

    /* to avoid setting it on each error case */
    fsstat_res_fail->obj_attributes.attributes_follow =
            FALSE;

    res->res_fsstat3.status = nfs_set_post_op_attr(fsstat_args->fsroot.data.data_val,
                                                   &fsstat_res_ok->obj_attributes);
    if (res->res_fsstat3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, D_ERROR, "Interface nfs_fsstat failed to obtain '%s' attributes",
            fsstat_args->fsroot.data.data_val);
        goto out;
    }

    retval = statvfs(fsstat_args->fsroot.data.data_val, &buffstatvfs);

    if (retval < 0) {
        rc = NFS_REQ_ERROR;
        goto out;
    }

    /*文件系统总空间大小*/
    fsstat_res_ok->tbytes =
            buffstatvfs.f_frsize * buffstatvfs.f_blocks;

    /*文件系统剩余空间大小*/
    fsstat_res_ok->fbytes =
            buffstatvfs.f_frsize * buffstatvfs.f_bfree;

    /*使用bytes计算的剩余空间信息，供通过rpc身份认证的用户使用*/
    fsstat_res_ok->abytes =
            buffstatvfs.f_frsize * buffstatvfs.f_bavail;

    /*文件系统中文件插槽的总数，（在unix系统中通常对应配置的节点信息）*/
    fsstat_res_ok->tfiles = buffstatvfs.f_files;

    /*文件系统中剩余文件插槽的数目*/
    fsstat_res_ok->ffiles = buffstatvfs.f_ffree;

    /*共通过rpc验证的用户使用的空闲插槽数目*/
    fsstat_res_ok->afiles = buffstatvfs.f_favail;

    /* volatile FS 文件系统更改的描述，用来设置文件系统在设置时间内不期望被更改，
     * 例如在不变系统下可以设置为最大的无符号整数，在变化频繁的系统下，设置为0，
     * 用户端可以使用它来进行缓存管理，是可变的并且可以在任何时候更改 */
    fsstat_res_ok->invarsec = 0;

    res->res_fsstat3.status = NFS3_OK;

    LOG(MODULE_NAME, D_INFO,
        "nfs_Fsstat --> tbytes=%u fbytes=%u abytes=%u",
        fsstat_res_ok->tbytes,
        fsstat_res_ok->fbytes,
        fsstat_res_ok->abytes);

    LOG(MODULE_NAME, D_INFO,
        "nfs_Fsstat --> tfiles=%u ffiles=%u afiles=%u",
        fsstat_res_ok->tfiles,
        fsstat_res_ok->ffiles,
        fsstat_res_ok->afiles);

    out:

    return rc;
}

void nfs3_fsstat_free(nfs_res_t *res) {
    /* Nothing to do here */
}


bool xdr_FSSTAT3args(XDR *xdrs, FSSTAT3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->fsroot))
        return (false);

    return (true);
}

bool xdr_FSSTAT3resok(XDR *xdrs, FSSTAT3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_size3(xdrs, &objp->tbytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->fbytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->abytes))
        return (false);
    if (!xdr_size3(xdrs, &objp->tfiles))
        return (false);
    if (!xdr_size3(xdrs, &objp->ffiles))
        return (false);
    if (!xdr_size3(xdrs, &objp->afiles))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->invarsec))
        return (false);
    return (true);
}

bool xdr_FSSTAT3resfail(XDR *xdrs, FSSTAT3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_FSSTAT3res(XDR *xdrs, FSSTAT3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_FSSTAT3resok(xdrs, &objp->FSSTAT3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_FSSTAT3resfail(xdrs, &objp->FSSTAT3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}