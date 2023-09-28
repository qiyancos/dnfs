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
#include "nfs/nfs_fsinfo.h"
#include "nfs/nfs_base.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"
int nfs3_fsinfo(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;
    FSINFO3resok *const FSINFO_FIELD = &res->res_fsinfo3.FSINFO3res_u.resok;

    LOG(MODULE_NAME, L_INFO, "The value of the nfs_fsinfo obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_fsinfo3.fsroot.data.data_val,
        arg->arg_fsinfo3.fsroot.data.data_len);

    /* To avoid setting it on each error case */
    res->res_fsinfo3.FSINFO3res_u.resfail.obj_attributes.attributes_follow = FALSE;

    if (arg->arg_fsinfo3.fsroot.data.data_val == nullptr) {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME,L_ERROR,
            "nfs_fsinfo get file handle is null");
        goto out;
    }

    res->res_fsinfo3.status =nfs_set_post_op_attr(arg->arg_fsinfo3.fsroot.data.data_val, &res->res_fsinfo3.FSINFO3res_u.resok.obj_attributes);
    if (res->res_fsinfo3.status!=NFS3_OK)
    {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "'stat %s' failed",
            arg->arg_fsinfo3.fsroot.data.data_val);
        goto out;
    }

    // 文件系统支持的属性
    FSINFO_FIELD->properties =
        FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;
    // todo 从配置文件读取，暂时给固定值
    // read请求支持的最大大小
    FSINFO_FIELD->rtmax = 512 * 1024;
    // read请求首选大小
    FSINFO_FIELD->rtpref = 512 * 1024;
    // read请求大小的建议倍数 This field is generally unused, it will be removed in V4
    FSINFO_FIELD->rtmult = DEV_BSIZE;
    // write请求支持的最大大小
    FSINFO_FIELD->wtmax = 512 * 1024;
    // write请求首选大小
    FSINFO_FIELD->wtpref = 512 * 1024;
    // write请求大小的建议倍数 This field is generally unused, it will be removed in V4
    FSINFO_FIELD->wtmult = DEV_BSIZE;
    // readdir请求建议大小
    FSINFO_FIELD->dtpref = 512 * 1024;
    // 文件系统上文件的最大大小
    FSINFO_FIELD->maxfilesize = (uint64_t)1024 * 1024 * 1024 * 1024 * 16;
    // 服务器保证的时间精确度 (0,1)纳秒 (0,1000000)毫秒 (1,0)秒
    FSINFO_FIELD->time_delta.tv_sec = 1;
    FSINFO_FIELD->time_delta.tv_nsec = 0;

    res->res_fsinfo3.status = NFS3_OK;

out:
    return rc;
} /* nfs3_fsinfo */

/**
 * @brief Free the result structure allocated for nfs3_fsinfo.
 *
 * This function frees the result structure allocated for nfs3_fsinfo.
 *
 * @param[in,out] res The result structure
 *
 */
void nfs3_fsinfo_free(nfs_res_t *res)
{
    /* Nothing to do here */
}
