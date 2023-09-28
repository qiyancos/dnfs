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
#include "mnt/mnt_mnt.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "mnt/mnt_base.h"

#define MODULE_NAME "MNT"
int mnt_Mnt(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int index_auth = 0;
    int retval = NFS_REQ_OK;
    auto *fh3 = (nfs_fh3 *) &res->res_mnt3.mountres3_u.mountinfo.fhandle;
    mountres3_ok * const RES_MOUNTINFO =
            &res->res_mnt3.mountres3_u.mountinfo;


    LOG(MODULE_NAME,L_INFO,
             "REQUEST PROCESSING: Calling MNT_MNT path=%s", arg->arg_mnt);

    /* Quick escape if an unsupported MOUNT version */
    if (req->rq_msg.cb_vers != MOUNT_V3) {
        LOG(MODULE_NAME,L_WARN,"Only supports mount 3 protocol");
        retval = NFS_REQ_ERROR;
        goto out;
    }

    if (arg->arg_mnt == nullptr) {
        LOG(MODULE_NAME,L_ERROR,
                "NULL path passed as Mount argument !!!");
        retval = NFS_REQ_DROP;
        goto out;
    }

    /* Paranoid command to clean the result struct. */
    memset(res, 0, sizeof(nfs_res_t));

    fh3->data.data_val=arg->arg_mnt;
    fh3->data.data_len=sizeof(arg->arg_mnt);

    RES_MOUNTINFO->auth_flavors.auth_flavors_val = (int*)calloc(index_auth, sizeof(int));

    RES_MOUNTINFO->auth_flavors.auth_flavors_len = index_auth;

    out:
    return retval;

}

void mnt3_Mnt_Free(nfs_res_t *res)
{
    mountres3_ok *resok = &res->res_mnt3.mountres3_u.mountinfo;

    if (res->res_mnt3.fhs_status == MNT3_OK) {
        free(resok->auth_flavors.auth_flavors_val);
        free(resok->fhandle.fhandle3_val);
    }
}