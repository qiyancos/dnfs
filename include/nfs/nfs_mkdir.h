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
#ifndef DNFSD_NFS_MKDIR_H
#define DNFSD_NFS_MKDIR_H

#include "nfs_args.h"

struct MKDIR3args {
    diropargs3 where;
    sattr3 attributes;
};

struct MKDIR3resok {
    post_op_fh3 obj;
    post_op_attr obj_attributes;
    wcc_data dir_wcc;
};

struct MKDIR3resfail {
    wcc_data dir_wcc;
};

struct MKDIR3res {
    nfsstat3 status;
    union {
        MKDIR3resok resok;
        MKDIR3resfail resfail;
    } MKDIR3res_u;
};


int nfs3_mkdir(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_mkdir_free(nfs_res_t *res);

extern bool xdr_MKDIR3args(XDR *xdrs, MKDIR3args *objp);

extern bool xdr_MKDIR3resok(XDR *xdrs, MKDIR3resok *objp);

extern bool xdr_MKDIR3resfail(XDR *xdrs, MKDIR3resfail *objp);

extern bool xdr_MKDIR3res(XDR *xdrs, MKDIR3res *objp);

#endif //DNFSD_NFS_MKDIR_H
