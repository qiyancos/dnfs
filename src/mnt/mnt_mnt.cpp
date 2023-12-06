/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "mnt/mnt_mnt.h"
#include "mnt/mnt_base.h"
#include "mnt/mnt_xdr.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "utils/common_utils.h"

#define MODULE_NAME "MNT"

int mnt_mnt(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {
    int index_auth = 1;
    int retval = NFS_REQ_OK;

    /*数据指针*/
    mnt3_dirpath *mount_dir = &arg->arg_mnt;
    mountres3_ok *mount_res =
            &res->res_mnt3.mountres3_u.mountinfo;
    auto *fh3 = (nfs_fh3 *) &mount_res->fhandle;


    LOG(MODULE_NAME, D_INFO,
        "REQUEST PROCESSING: Calling MNT_MNT path=%s", mount_dir);

    /* Quick escape if an unsupported MOUNT version */
    if (req->rq_msg.cb_vers != MOUNT_V3) {
        LOG(MODULE_NAME, D_WARN, "Only supports mount 3 protocol");
        retval = NFS_REQ_ERROR;
        goto out;
    }

    if (mount_dir == nullptr) {
        LOG(MODULE_NAME, D_ERROR,
            "NULL path passed as Mount argument !!!");
        retval = NFS_REQ_DROP;
        goto out;
    }

    /* Paranoid command to clean the result struct. */
    memset(res, 0, sizeof(nfs_res_t));

    fh3->data.data_val = *mount_dir;
    fh3->data.data_len = strlen(*mount_dir);

    /*添加权限*/
    mount_res->auth_flavors.auth_flavors_val = (int *) gsh_calloc(index_auth,
                                                                  sizeof(int));
    mount_res->auth_flavors.auth_flavors_val[0] = AUTH_NONE;
    mount_res->auth_flavors.auth_flavors_len = index_auth;

    LOG(MODULE_NAME, D_INFO,
        "REQUEST PROCESSING: Request MNT_MNT file_handle=%s,len is %d",
        fh3->data.data_val, fh3->data.data_len);

    out:
    return retval;

}

void mnt3_mnt_free(nfs_res_t *res) {
    mountres3_ok *resok = &res->res_mnt3.mountres3_u.mountinfo;

    if (res->res_mnt3.fhs_status == MNT3_OK) {
        gsh_free(resok->auth_flavors.auth_flavors_val);
//        free(resok->fhandle.fhandle3_val);
    }
}


bool xdr_mountres3_ok(XDR *xdrs, mountres3_ok *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_fhandle3(xdrs, &objp->fhandle))
        return (false);
    if (!xdr_array
            (xdrs, (char **) &objp->auth_flavors.auth_flavors_val,
             &objp->auth_flavors.auth_flavors_len, XDR_ARRAY_MAXLEN,
             sizeof(int), (xdrproc_t) xdr_int))
        return (false);
    return (true);
}

bool xdr_mountres3(XDR *xdrs, mountres3 *objp) {

#if defined(_LP64) || defined(_KERNEL)
    int __attribute__ ((__unused__)) *buf;
#else
    register long __attribute__ ((__unused__)) * buf;
#endif

    if (!xdr_mountstat3(xdrs, &objp->fhs_status))
        return (false);
    switch (objp->fhs_status) {
        case MNT3_OK:
            if (!xdr_mountres3_ok(xdrs, &objp->mountres3_u.mountinfo))
                return (false);
            break;
        default:
            return (true);
    }
    return (true);
}