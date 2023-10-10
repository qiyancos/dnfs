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
#include "nfs/nfs_fsinfo.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_fsinfo(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int rc = NFS_REQ_OK;
    /*构造参数指针*/
    FSINFO3args *fsinfo_args = &arg->arg_fsinfo3;
    FSINFO3resok *fsinfo_res_ok = &res->res_fsinfo3.FSINFO3res_u.resok;
    FSINFO3resfail *fsinfo_res_fail = &res->res_fsinfo3.FSINFO3res_u.resfail;


    if (fsinfo_args->fsroot.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "nfs_fsinfo get file handle len is 0");
        goto out;
    }

    get_file_handle(fsinfo_args->fsroot);

    LOG(MODULE_NAME, D_INFO,
        "The value of the nfs_fsinfo obtained file handle is '%s', and the length is '%d'",
        fsinfo_args->fsroot.data.data_val,
        fsinfo_args->fsroot.data.data_len);

    /* To avoid setting it on each error case */
    fsinfo_res_fail->obj_attributes.attributes_follow = FALSE;

    res->res_fsinfo3.status = nfs_set_post_op_attr(fsinfo_args->fsroot.data.data_val,
                                                   &fsinfo_res_ok->obj_attributes);
    if (res->res_fsinfo3.status != NFS3_OK) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "Interface nfs_fsinfo failed to obtain '%s' attributes",
            fsinfo_args->fsroot.data.data_val);
        goto out;
    }

    // 文件系统支持的属性
    fsinfo_res_ok->properties =
            FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;
    // todo 从配置文件读取，暂时给固定值
    // read请求支持的最大大小
    fsinfo_res_ok->rtmax = 512 * 1024;
    // read请求首选大小
    fsinfo_res_ok->rtpref = 512 * 1024;
    // read请求大小的建议倍数 This field is generally unused, it will be removed in V4
    fsinfo_res_ok->rtmult = DEV_BSIZE;
    // write请求支持的最大大小
    fsinfo_res_ok->wtmax = 512 * 1024;
    // write请求首选大小
    fsinfo_res_ok->wtpref = 512 * 1024;
    // write请求大小的建议倍数 This field is generally unused, it will be removed in V4
    fsinfo_res_ok->wtmult = DEV_BSIZE;
    // readdir请求建议大小
    fsinfo_res_ok->dtpref = 512 * 1024;
    // 文件系统上文件的最大大小
    fsinfo_res_ok->maxfilesize = (uint64_t) 1024 * 1024 * 1024 * 1024 * 16;
    // 服务器保证的时间精确度 (0,1)纳秒 (0,1000000)毫秒 (1,0)秒
    fsinfo_res_ok->time_delta.tv_sec = 1;
    fsinfo_res_ok->time_delta.tv_nsec = 0;

    res->res_fsinfo3.status = NFS3_OK;

    out:
    return rc;
}


void nfs3_fsinfo_free(nfs_res_t *res) {
    /* Nothing to do here */
}

bool xdr_FSINFO3args(XDR *xdrs, FSINFO3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->fsroot))
        return (false);
    return (true);
}

bool xdr_FSINFO3resok(XDR *xdrs, FSINFO3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->dtpref))
        return (false);
    if (!xdr_size3(xdrs, &objp->maxfilesize))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->time_delta))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->properties))
        return (false);
    return (true);
}

bool xdr_FSINFO3resfail(XDR *xdrs, FSINFO3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_FSINFO3res(XDR *xdrs, FSINFO3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_FSINFO3resok(xdrs, &objp->FSINFO3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_FSINFO3resfail(xdrs, &objp->FSINFO3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}

