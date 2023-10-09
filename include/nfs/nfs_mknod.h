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
#ifndef DNFSD_NFS_MKNOD_H
#define DNFSD_NFS_MKNOD_H

#include "nfs/nfs_args.h"

struct devicedata3 {
    sattr3 dev_attributes;
    specdata3 spec;
};

struct mknoddata3 {
    ftype3 type;
    union {
        devicedata3 device;
        sattr3 pipe_attributes;
    } mknoddata3_u;
};

struct MKNOD3args {
    diropargs3 where;
    mknoddata3 what;
};

struct MKNOD3resok {
    post_op_fh3 obj;
    post_op_attr obj_attributes;
    wcc_data dir_wcc;
};

struct MKNOD3resfail {
    wcc_data dir_wcc;
};

struct MKNOD3res {
    nfsstat3 status;
    union {
        MKNOD3resok resok;
        MKNOD3resfail resfail;
    } MKNOD3res_u;
};

int nfs3_mknod(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_mknod_free(nfs_res_t *res);

extern bool xdr_devicedata3(XDR *xdrs, devicedata3 *objp);

extern bool xdr_mknoddata3(XDR *xdrs, mknoddata3 *objp);

extern bool xdr_MKNOD3args(XDR *xdrs, MKNOD3args *objp);

extern bool xdr_MKNOD3resok(XDR *xdrs, MKNOD3resok *objp);

extern bool xdr_MKNOD3resfail(XDR *xdrs, MKNOD3resfail *objp);

extern bool xdr_MKNOD3res(XDR *xdrs, MKNOD3res *objp);

#endif //DNFSD_NFS_MKNOD_H
