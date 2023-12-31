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
#ifndef DNFSD_NFS_GETATTR_H
#define DNFSD_NFS_GETATTR_H

#include "nfs_args.h"

struct GETATTR3args {
    nfs_fh3 object;
};

struct GETATTR3resok {
    fattr3 obj_attributes;
};

struct GETATTR3res {
    nfsstat3 status;
    union {
        GETATTR3resok resok;
    } GETATTR3res_u;
};

int nfs3_getattr(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

void nfs3_getattr_free(nfs_res_t *resp);

bool xdr_GETATTR3args(XDR *xdrs, GETATTR3args *objp);

bool xdr_GETATTR3resok(XDR *xdrs, GETATTR3resok *objp);

bool xdr_GETATTR3res(XDR *xdrs, GETATTR3res *objp);


#endif //DNFSD_NFS_GETATTR_H
