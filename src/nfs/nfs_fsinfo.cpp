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
#include <sys/stat.h>

#include "nfs/nfs_fsinfo.h"
#include "nfs/nfs_base.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"
int nfs3_fsinfo(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;
    FSINFO3resok *const FSINFO_FIELD = &res->res_fsinfo3.FSINFO3res_u.resok;

    LOG(MODULE_NAME, L_INFO, "The value of the obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_fsinfo3.fsroot.data.data_val,
        arg->arg_fsinfo3.fsroot.data.data_len);

    /* To avoid setting it on each error case */
    res->res_fsinfo3.FSINFO3res_u.resfail.obj_attributes.attributes_follow = FALSE;

    struct stat buf;
    int stat_res = stat(arg->arg_fsinfo3.fsroot.data.data_val, &buf);
    if (stat_res != 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "'stat %s' failed, errno: %d",
            arg->arg_fsinfo3.fsroot.data.data_val, errno);
        switch (errno)
        {
        case ENOENT:
            res->res_fsinfo3.status = NFS3ERR_NOENT;
            break;
        case ENOTDIR:
            res->res_fsinfo3.status = NFS3ERR_NOTDIR;
            break;
        case EACCES:
            res->res_fsinfo3.status = NFS3ERR_ACCES;
            break;
        default:
            res->res_fsinfo3.status = NFS3ERR_BADHANDLE;
            break;
        }
        goto out;
    }

    FSINFO_FIELD->properties =
        FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;
    // todo 暂时给固定值
    FSINFO_FIELD->rtmax = 512 * 1024;
    FSINFO_FIELD->rtpref = 512 * 1024;
    /* This field is generally unused, it will be removed in V4 */
    FSINFO_FIELD->rtmult = DEV_BSIZE;
    FSINFO_FIELD->wtmax = 512 * 1024;
    FSINFO_FIELD->wtpref = 512 * 1024;
    /* This field is generally unused, it will be removed in V4 */
    FSINFO_FIELD->wtmult = DEV_BSIZE;
    FSINFO_FIELD->dtpref = 512 * 1024;
    FSINFO_FIELD->maxfilesize = 17592186040320;
    FSINFO_FIELD->time_delta.tv_sec = 1;
    FSINFO_FIELD->time_delta.tv_nsec = 0;
    
    if (!nfs_set_post_op_attr(&buf, &res->res_fsinfo3.FSINFO3res_u.resok.obj_attributes)){
        rc = NFS_REQ_ERROR;
        goto out;
    }
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
