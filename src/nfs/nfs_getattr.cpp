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
#include "nfs/nfs_getattr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"
int nfs3_getattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;
    struct post_op_attr get_a{};

    LOG(MODULE_NAME, L_INFO, "The value of the nfs_getattr obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_getattr3.object.data.data_val,
        arg->arg_getattr3.object.data.data_len);

    if (arg->arg_getattr3.object.data.data_val == nullptr) {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME,L_ERROR,
            "nfs_getattr get file handle is null");
        goto out;
    }
    res->res_getattr3.status =nfs_set_post_op_attr(arg->arg_getattr3.object.data.data_val, &get_a);
    if (res->res_getattr3.status!=NFS3_OK)
    {
        rc=NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "nfs_getattr stat %s' failed",
            arg->arg_getattr3.object.data.data_val);
        goto out;
    }

    memcpy(&res->res_getattr3.GETATTR3res_u.resok.obj_attributes,&get_a.post_op_attr_u,
           sizeof(get_a.post_op_attr_u));

    out:
    return rc;

}

/**
 * @brief Free the result structure allocated for nfs3_getattr.
 *
 * @param[in,out] resp Result structure
 *
 */
void nfs3_getattr_free(nfs_res_t *resp)
{
    /* Nothing to do here */
}